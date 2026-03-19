/*
 * boot_hal.c
 * Logique de démarrage : MPU, nettoyage état, saut vers l'application
 * STM32L4R5VGT6
 */

#include "boot_hal.h"
#include "bootutil/bootutil.h"
#include "flash_map_backend.h"
#include "stm32l4xx_hal.h"
#include "mcuboot_config.h"

/* ================================================================
 * DÉCLARATIONS EXTERNES
 * hiwdg est déclaré dans main.c par CubeMX
 * ================================================================ */
extern IWDG_HandleTypeDef hiwdg;

/* ================================================================
 * boot_platform_quit()
 * Appelée par MCUBoot en cas d'erreur fatale irrécupérable.
 * On bloque en attendant que le watchdog déclenche un reset.
 * ================================================================ */
void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    (void)vt;
    MCUBOOT_LOG_ERR("boot_platform_quit: erreur fatale — "
                    "attente reset watchdog...");

    /* Désactiver toutes les interruptions */
    __disable_irq();

    /* Boucle infinie — le IWDG va resetter dans ~5s */
    while (1) {
        __NOP();
    }
}

/* ================================================================
 * boot_hal_mpu_config()
 * Configure le MPU pour protéger la zone bootloader en lecture seule
 * et rendre la zone de clés inaccessible depuis l'application.
 *
 * MPU du Cortex-M4 : 8 régions disponibles, numérotées 0 à 7.
 * ================================================================ */
static void boot_hal_mpu_config(void)
{
    MPU_Region_InitTypeDef mpu_region;

    /* Désactiver le MPU avant de le reconfigurer */
    HAL_MPU_Disable();

    /* ----------------------------------------------------------
     * Région 0 : Zone BOOTLOADER (0x08000000 → +128 Ko)
     * Accès : Lecture seule pour tous (privileged + unprivileged)
     * Exécution : Autorisée (le code bootloader peut s'exécuter)
     * Cela empêche l'application d'écraser le bootloader en RAM.
     * ---------------------------------------------------------- */
    mpu_region.Enable           = MPU_REGION_ENABLE;
    mpu_region.Number           = MPU_REGION_NUMBER0;
    mpu_region.BaseAddress      = 0x08000000;
    mpu_region.Size             = MPU_REGION_SIZE_128KB;
    mpu_region.SubRegionDisable = 0x00;
    mpu_region.TypeExtField     = MPU_TEX_LEVEL0;
    mpu_region.AccessPermission = MPU_REGION_PRIV_RO_URO; /* RO partout */
    mpu_region.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    mpu_region.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    mpu_region.IsCacheable      = MPU_ACCESS_CACHEABLE;
    mpu_region.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    HAL_MPU_ConfigRegion(&mpu_region);

    /* ----------------------------------------------------------
     * Région 1 : Zone USER DATA (0x080F0000 → +64 Ko)
     * Accès : Lecture/Écriture (l'application en a besoin)
     * ---------------------------------------------------------- */
    mpu_region.Enable           = MPU_REGION_ENABLE;
    mpu_region.Number           = MPU_REGION_NUMBER1;
    mpu_region.BaseAddress      = 0x080F0000;
    mpu_region.Size             = MPU_REGION_SIZE_64KB;
    mpu_region.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu_region.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&mpu_region);

    /* ----------------------------------------------------------
     * Région 2 : RAM2 (0x10000000 → +64 Ko)
     * Région avec parity check HW — protection contre corruption
     * ---------------------------------------------------------- */
    mpu_region.Enable           = MPU_REGION_ENABLE;
    mpu_region.Number           = MPU_REGION_NUMBER2;
    mpu_region.BaseAddress      = 0x10000000;
    mpu_region.Size             = MPU_REGION_SIZE_64KB;
    mpu_region.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu_region.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
    mpu_region.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    HAL_MPU_ConfigRegion(&mpu_region);

    /* Activer le MPU avec accès par défaut au fond (background region) */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

    MCUBOOT_LOG_INF("MPU configuré : bootloader protégé en lecture seule");
}

/* ================================================================
 * boot_hal_hw_deinit()
 * Réinitialise les périphériques avant de sauter vers l'application.
 *
 * IMPORTANT : l'application va réinitialiser tous ses périphériques
 * via son propre SystemInit() + HAL_Init(). Il faut remettre le
 * hardware dans un état "propre" avant de lui passer la main.
 * ================================================================ */
static void boot_hal_hw_deinit(void)
{
	HAL_Delay(200);

    /* Arrêter SysTick — l'application le reconfigurera */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    /* Désactiver toutes les interruptions NVIC actives */
    for (int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;  /* Disable all */
        NVIC->ICPR[i] = 0xFFFFFFFF;  /* Clear pending */
    }

    /* Remettre l'horloge système sur MSI (état reset) */
    /* L'application reconfigure son horloge via SystemInit() */
    HAL_RCC_DeInit();

    MCUBOOT_LOG_INF("Hardware déinitialisé — prêt pour le saut");
}

/* ================================================================
 * boot_hal_jump_to_app()
 * Effectue le saut vers l'application dans le Slot 0.
 *
 * Protocole de saut ARM Cortex-M4 :
 *   1. Vérifier que le vecteur de reset pointe vers la Flash app
 *   2. Désactiver les interruptions
 *   3. Mettre à jour le VTOR
 *   4. Charger le Stack Pointer de l'application
 *   5. Appeler le Reset Handler de l'application
 *
 * app_start : adresse de début du Slot 0 + offset header MCUBoot
 *             = 0x08020000 + 0x400 = 0x08020400
 * ================================================================ */
void boot_hal_jump_to_app(uint32_t app_start)
{
    /* Pointeur de fonction vers le Reset Handler de l'application */
    typedef void (*app_reset_handler_t)(void);
    app_reset_handler_t app_reset_handler;

    uint32_t app_sp;              /* Stack Pointer initial de l'app */
    uint32_t app_reset_vector;    /* Adresse du Reset Handler       */

    MCUBOOT_LOG_INF("Saut vers l'application à 0x%08X", app_start);

    /* ----------------------------------------------------------
     * Vérification de sanité : le vecteur de reset doit pointer
     * dans la plage valide de la Flash (Slot 0)
     * ---------------------------------------------------------- */
    app_sp           = *(volatile uint32_t *)(app_start);
    app_reset_vector = *(volatile uint32_t *)(app_start + 4);

    /* Le SP doit pointer dans la RAM (0x20000000 → 0x2008FFFF) */
    if (app_sp < 0x20000000 || app_sp > 0x2008FFFF) {
        MCUBOOT_LOG_ERR("SP invalide : 0x%08X — image corrompue ?",
                        app_sp);
        boot_platform_quit(NULL);
        return;
    }

    /* Le Reset Handler doit pointer dans le Slot 0 */
    if (app_reset_vector < 0x08020400 || app_reset_vector > 0x0807FFFF) {
        MCUBOOT_LOG_ERR("Reset vector invalide : 0x%08X", app_reset_vector);
        boot_platform_quit(NULL);
        return;
    }

    MCUBOOT_LOG_INF("SP app        = 0x%08X", app_sp);
    MCUBOOT_LOG_INF("Reset handler = 0x%08X", app_reset_vector);

    /* ----------------------------------------------------------
     * Configuration MPU avant de passer la main à l'application
     * ---------------------------------------------------------- */
    boot_hal_mpu_config();

    /* ----------------------------------------------------------
     * Déinitialisation du hardware
     * ---------------------------------------------------------- */
    boot_hal_hw_deinit();

    /* ----------------------------------------------------------
     * Séquence de saut — à partir d'ici, aucune interruption,
     * aucun retour possible.
     * ---------------------------------------------------------- */

    /* 1. Désactiver toutes les interruptions (PRIMASK = 1) */
    __disable_irq();

    /* 2. Mettre à jour le VTOR vers la table de vecteurs de l'app
     *    Le VTOR doit être aligné sur 512 octets minimum (Cortex-M4)
     *    0x08020000 est aligné sur 128 Ko → OK */
    SCB->VTOR = app_start & SCB_VTOR_TBLOFF_Msk;

    /* 3. Assurer la visibilité mémoire des registres modifiés */
    __DSB();
    __ISB();

    /* 4. Charger le Stack Pointer initial de l'application */
    __set_MSP(app_sp);

    /* 5. Construire le pointeur vers le Reset Handler (bit 0 = 1 pour Thumb) */
    app_reset_handler = (app_reset_handler_t)(app_reset_vector | 0x01);

    /* 6. SAUT — cette ligne est la dernière exécutée par le bootloader */
    app_reset_handler();

    /* Ne doit jamais être atteint */
    while (1) { __NOP(); }
}
