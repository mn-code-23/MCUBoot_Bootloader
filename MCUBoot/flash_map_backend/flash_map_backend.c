/*
 * flash_map_backend.c
 * Implémentation HAL Shim Flash pour STM32L4R5VGT6
 *
 * Flash : 1 Mo, page size 4 Ko, écriture par double-mots (64 bits)
 * Toutes les opérations nécessitent un déverrouillage préalable de la Flash.
 */
#include <stdio.h>
#include "flash_map_backend.h"
#include "stm32l4xx_hal.h"
#include "../mcuboot_config/mcuboot_config.h"
#include <string.h>
#include <assert.h>

/* Déclaration externe du handle IWDG défini dans main.c */
extern IWDG_HandleTypeDef hiwdg;

/* ================================================================
 * TABLE DE CARTOGRAPHIE DES ZONES FLASH
 * Doit correspondre exactement à flash_map_backend.h
 * ================================================================ */
static const struct flash_area flash_map[] = {
    {
        .fa_id        = FLASH_AREA_BOOTLOADER,
        .fa_device_id = 0,
        .fa_off       = FLASH_AREA_BOOTLOADER_OFFSET,
        .fa_size      = FLASH_AREA_BOOTLOADER_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_IMAGE_PRIMARY(0),
        .fa_device_id = 0,
        .fa_off       = FLASH_AREA_IMAGE_0_OFFSET,
        .fa_size      = FLASH_AREA_IMAGE_0_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_IMAGE_SECONDARY(0),
        .fa_device_id = 0,
        .fa_off       = FLASH_AREA_IMAGE_1_OFFSET,
        .fa_size      = FLASH_AREA_IMAGE_1_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_IMAGE_SCRATCH,
        .fa_device_id = 0,
        .fa_off       = FLASH_AREA_IMAGE_SCRATCH_OFFSET,
        .fa_size      = FLASH_AREA_IMAGE_SCRATCH_SIZE,
    },
};

#define FLASH_MAP_ENTRY_COUNT \
    (sizeof(flash_map) / sizeof(flash_map[0]))

/* ================================================================
 * FONCTIONS UTILITAIRES INTERNES
 * ================================================================ */

/*
 * Convertit un offset relatif (depuis FLASH_BASE) en adresse absolue.
 */
static inline uint32_t flash_abs_addr(const struct flash_area *fa,
                                       uint32_t off)
{
    return FLASH_BASE_ADDRESS + fa->fa_off + off;
}

/*
 * Retourne le numéro de page Flash à partir d'une adresse absolue.
 * Le L4R5 (1 Mo monobanque) numérote ses pages de 0 à 255.
 */
static inline uint32_t addr_to_page(uint32_t abs_addr)
{
    return (abs_addr - FLASH_BASE_ADDRESS) / FLASH_PAGE_SIZE;
}

/*
 * Vérifie que l'accès (offset + longueur) reste dans les bornes de la zone.
 * Retourne 0 si OK, -1 si débordement.
 */
static int flash_area_check_bounds(const struct flash_area *fa,
                                    uint32_t off, uint32_t len)
{
    if (off + len > fa->fa_size) {
        MCUBOOT_LOG_ERR("Flash bounds check failed: off=0x%08X len=0x%X "
                        "size=0x%X", off, len, fa->fa_size);
        return -1;
    }
    return 0;
}

/* ================================================================
 * flash_area_open()
 * Retrouve une zone par son ID et retourne un pointeur vers sa
 * structure. Pas de ressource hardware à allouer ici.
 * ================================================================ */
int flash_area_open(uint8_t id, const struct flash_area **fap)
{
    if (fap == NULL) {
        return -1;
    }

    for (uint32_t i = 0; i < FLASH_MAP_ENTRY_COUNT; i++) {
        if (flash_map[i].fa_id == id) {
            *fap = &flash_map[i];
            MCUBOOT_LOG_DBG("flash_area_open: id=%d offset=0x%08X size=0x%X",
                            id, flash_map[i].fa_off, flash_map[i].fa_size);
            return 0;
        }
    }

    MCUBOOT_LOG_ERR("flash_area_open: ID %d non trouvé", id);
    return -1;
}

/* ================================================================
 * flash_area_close()
 * Libère la zone. Ici, rien à libérer — on s'assure juste que
 * la Flash est reverouillée.
 * ================================================================ */
void flash_area_close(const struct flash_area *fa)
{
    (void)fa;
    /* S'assurer que la Flash est bien verrouillée après utilisation */
    HAL_FLASH_Lock();
}

/* ================================================================
 * flash_area_read()
 * Lecture depuis la Flash : simple copie mémoire (la Flash est
 * mappée en XIP sur le bus AHB du Cortex-M4).
 * Pas besoin de déverrouillage pour la lecture.
 * ================================================================ */
int flash_area_read(const struct flash_area *fa, uint32_t off,
                    void *dst, uint32_t len)
{

    /* Lecture directe via le bus mémoire (0 wait-state avec ART) */
    /* ══ ADRESSE ABSOLUE ══ */
    uint32_t src_addr = FLASH_BASE_ADDRESS + fa->fa_off + off;
    memcpy(dst, (const void *)src_addr, len);

    return 0;
}

/* ================================================================
 * flash_area_write()
 * Écriture dans la Flash :
 *   - Le STM32L4R5 exige une écriture par DOUBLE-MOT (64 bits = 8 octets)
 *   - La zone à écrire doit avoir été préalablement effacée (valeur 0xFF)
 *   - L'adresse de destination doit être alignée sur 8 octets
 * ================================================================ */

int flash_area_write(const struct flash_area *fa,
                     uint32_t off,
                     const void *src,
                     uint32_t len)
{
    /* ── Utiliser le helper qui ajoute FLASH_BASE correctement ── */
    uint32_t       dest_addr = flash_abs_addr(fa, off);
    const uint8_t *data      = (const uint8_t *)src;
    uint32_t       i         = 0;
    HAL_StatusTypeDef status;

    if (fa == NULL || src == NULL) return -1;
    if (flash_area_check_bounds(fa, off, len) != 0) return -1;

    if ((len % 8) != 0 || (dest_addr % 8) != 0) {
        MCUBOOT_LOG_ERR("flash_area_write: alignement incorrect "
                        "addr=0x%08lX len=%lu", dest_addr, len);
        return -1;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    if (HAL_FLASH_Unlock() != HAL_OK) {
        MCUBOOT_LOG_ERR("flash_area_write: Unlock échoué CR=0x%08lX",
                        FLASH->CR);
        return -1;
    }

    while (i < len) {
        uint64_t dword;
        memcpy(&dword, data + i, sizeof(uint64_t));

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                   dest_addr + i, dword);
        if (status != HAL_OK) {
            MCUBOOT_LOG_ERR("flash_area_write: erreur à 0x%08lX "
                            "(HAL=%d, SR=0x%08lX, CR=0x%08lX)",
                            dest_addr + i, status,
                            FLASH->SR, FLASH->CR);
            HAL_FLASH_Lock();
            return -1;
        }
        i += 8;
    }

    HAL_FLASH_Lock();
    return 0;
}


/* ================================================================
 * flash_area_erase()
 * Effacement par pages de 4 Ko.
 * L'effacement met tous les bits à 1 (valeur 0xFF par octet).
 * ================================================================ */
int flash_area_erase(const struct flash_area *fa,
                     uint32_t off, uint32_t len)
{
    FLASH_EraseInitTypeDef erase;
    uint32_t page_error;
    HAL_StatusTypeDef status;

    if (fa == NULL) return -1;
    if (flash_area_check_bounds(fa, off, len) != 0) return -1;

    uint32_t abs_start = flash_abs_addr(fa, off); /* ex: 0x08080000 */
    uint32_t nb_pages  = len / FLASH_PAGE_SIZE;   /* ex: 96 pages × 4Ko */

    /* ── STM32L4R5 DUAL BANK ──────────────────────────────────────
     * Bank 1 : 0x08000000 → 0x0807FFFF (512 Ko, pages 0-127)
     * Bank 2 : 0x08080000 → 0x080FFFFF (512 Ko, pages 0-127)
     * ────────────────────────────────────────────────────────── */
    #define BANK2_START 0x08080000UL

    if (abs_start >= BANK2_START) {
        /* ── BANK 2 ── page relative à la banque */
        erase.Banks = FLASH_BANK_2;
        erase.Page  = (abs_start - BANK2_START) / FLASH_PAGE_SIZE;
    } else {
        /* ── BANK 1 ── */
        erase.Banks = FLASH_BANK_1;
        erase.Page  = (abs_start - FLASH_BASE_ADDRESS) / FLASH_PAGE_SIZE;
    }

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.NbPages   = nb_pages;

    MCUBOOT_LOG_INF("flash_area_erase: Bank%d page %lu → page %lu "
                    "(%lu pages, %lu Ko)",
                    (erase.Banks == FLASH_BANK_1) ? 1 : 2,
                    erase.Page,
                    erase.Page + nb_pages - 1,
                    nb_pages,
                    (nb_pages * FLASH_PAGE_SIZE) / 1024);

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    HAL_FLASH_Unlock();

    status = HAL_FLASHEx_Erase(&erase, &page_error);

    HAL_FLASH_Lock();

    if (status != HAL_OK) {
        MCUBOOT_LOG_ERR("flash_area_erase: erreur page %lu "
                        "(HAL=%d, SR=0x%08lX)",
                        page_error, status, FLASH->SR);
        return -1;
    }

    MCUBOOT_WATCHDOG_FEED();
    return 0;
}


/* ================================================================
 * flash_area_align()
 * Retourne l'alignement d'écriture minimal requis par le hardware.
 * STM32L4R5 : 8 octets (double-mot 64 bits).
 * ================================================================ */
uint32_t flash_area_align(const struct flash_area *fa)
{
    (void)fa;
    return FLASH_WRITE_ALIGNMENT;   /* 8 octets */
}

/* ================================================================
 * flash_area_erased_val()
 * Retourne la valeur d'un octet effacé.
 * Sur Flash NOR (STM32), l'effacement met tout à 0xFF.
 * ================================================================ */
uint32_t flash_area_erased_val(const struct flash_area *fa)
{
    (void)fa;
    return 0xFF;
}

/* ================================================================
 * flash_area_get_sectors()
 * Retourne la liste des secteurs (pages) d'une zone Flash.
 * MCUBoot l'utilise pour planifier les opérations de swap.
 * ================================================================ */

/* ================================================================
 * ACCESSEURS flash_area
 * Convertis de static inline en fonctions normales pour éviter
 * les problèmes de linkage avec bootutil/src/
 * ================================================================ */

uint32_t flash_area_get_off(const struct flash_area *fa)
{
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa)
{
    return fa->fa_size;
}

uint8_t flash_area_get_id(const struct flash_area *fa)
{
    return fa->fa_id;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa)
{
    return fa->fa_device_id;
}

/* ================================================================
 * ACCESSEURS flash_sector
 * ================================================================ */

uint32_t flash_sector_get_off(const struct flash_sector *fs)
{
    return fs->fs_off;
}

uint32_t flash_sector_get_size(const struct flash_sector *fs)
{
    return fs->fs_size;
}

/* ================================================================
 * flash_area_get_sector()
 * Trouve le secteur (page 4 Ko) qui contient l'offset 'off'
 * dans la zone flash 'fa', et remplit la structure 'sector'.
 *
 * Appelée par bootutil_area.c pour les opérations d'effacement
 * et de calcul du trailer.
 * ================================================================ */
int flash_area_get_sector(const struct flash_area *fa,
                           uint32_t off,
                           struct flash_sector *sector)
{
    uint32_t sector_index;

    if (fa == NULL || sector == NULL) {
        return -1;
    }

    if (off >= fa->fa_size) {
        return -1;
    }

    /* Calculer l'index de la page qui contient cet offset */
    sector_index = off / MCUBOOT_FLASH_PAGE_SIZE;

    /* Remplir la structure sector */
    sector->fs_off  = sector_index * MCUBOOT_FLASH_PAGE_SIZE;
    sector->fs_size = MCUBOOT_FLASH_PAGE_SIZE;

    return 0;
}

int flash_area_get_sectors(int fa_id, uint32_t *count,
                            struct flash_sector *sectors)
{
    const struct flash_area *fa;
    uint32_t nb_sectors;
    uint32_t i;

    if (count == NULL || sectors == NULL) {
        return -1;
    }

    /* Retrouver la zone par son ID */
    if (flash_area_open((uint8_t)fa_id, &fa) != 0) {
        return -1;
    }

    /* Nombre de pages (secteurs) dans cette zone */
    nb_sectors = fa->fa_size / FLASH_PAGE_SIZE;

    if (*count < nb_sectors) {
        MCUBOOT_LOG_ERR("flash_area_get_sectors: buffer trop petit "
                        "(%d < %d)", *count, nb_sectors);
        return -1;
    }

    /* Remplir le tableau de secteurs */
    for (i = 0; i < nb_sectors; i++) {
        sectors[i].fs_off  = i * FLASH_PAGE_SIZE;
        sectors[i].fs_size = FLASH_PAGE_SIZE;
    }

    *count = nb_sectors;

    MCUBOOT_LOG_DBG("flash_area_get_sectors: fa_id=%d → %d secteurs "
                    "de %d Ko", fa_id, nb_sectors, FLASH_PAGE_SIZE / 1024);

    return 0;
}

/* ================================================================
 * flash_area_id_from_multi_image_slot_hook()
 * Signature correcte : 3 paramètres — le 3ème est un code retour
 * ================================================================ */
int flash_area_id_from_multi_image_slot_hook(int image_index, int slot,
                                              int *rc)
{
    int fa_id;

    switch (slot) {
        case 0:
            fa_id = FLASH_AREA_IMAGE_PRIMARY(image_index);
            break;
        case 1:
            fa_id = FLASH_AREA_IMAGE_SECONDARY(image_index);
            break;
        default:
            MCUBOOT_LOG_ERR("slot invalide %d", slot);
            if (rc != NULL) { *rc = -1; }
            return -1;
    }

    if (rc != NULL) { *rc = 0; }
    return fa_id;
}

/* ================================================================
 * flash_area_id_from_multi_image_slot()
 * Wrapper legacy — appelle le hook
 * ================================================================ */
int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    int rc;
    return flash_area_id_from_multi_image_slot_hook(image_index, slot, &rc);
}

/* ================================================================
 * flash_area_to_sectors()
 * boot_sector_t = struct flash_area dans MCUBoot v2.x
 * Chaque "secteur" est décrit par une flash_area (off + size)
 * ================================================================ */
int flash_area_to_sectors(int fa_id, int *count,
                           struct flash_area *sectors)
{
    const struct flash_area *fa;
    uint32_t nb_sectors;
    uint32_t i;

    if (count == NULL || sectors == NULL) {
        return -1;
    }

    if (flash_area_open((uint8_t)fa_id, &fa) != 0) {
        return -1;
    }

    nb_sectors = fa->fa_size / MCUBOOT_FLASH_PAGE_SIZE;

    if ((uint32_t)(*count) < nb_sectors) {
        MCUBOOT_LOG_ERR("flash_area_to_sectors: buffer trop petit "
                        "(%d < %d)", *count, nb_sectors);
        return -1;
    }

    /* Remplir le tableau — chaque entrée est une flash_area
     * décrivant un secteur (page 4 Ko) de cette zone */
    for (i = 0; i < nb_sectors; i++) {
        sectors[i].fa_id        = (uint8_t)fa_id;
        sectors[i].fa_device_id = fa->fa_device_id;
        sectors[i].fa_off       = fa->fa_off + (i * MCUBOOT_FLASH_PAGE_SIZE);
        sectors[i].fa_size      = MCUBOOT_FLASH_PAGE_SIZE;
    }

    *count = (int)nb_sectors;
    return 0;
}

/* ================================================================
 * flash_area_id_from_image_slot()
 * Version simplifiée pour image_index = 0 (image unique).
 * Appelée par split_go() dans loader.c de MCUBoot.
 * ================================================================ */
int flash_area_id_from_image_slot(int slot)
{
    /* image_index = 0 car on n'a qu'une seule image (MCUBOOT_IMAGE_NUMBER = 1) */
    return flash_area_id_from_multi_image_slot(0, slot);
}

///*
// * flash_map_backend.c
// * Implémentation HAL Shim Flash pour STM32L4R5VGT6
// *
// * Flash : 1 Mo, page size 4 Ko, écriture par double-mots (64 bits)
// * Toutes les opérations nécessitent un déverrouillage préalable de la Flash.
// */
//
//#include "flash_map_backend.h"
//#include "stm32l4xx_hal.h"
//#include "mcuboot_config.h"
//#include <string.h>
//#include <assert.h>
//
///* Déclaration externe du handle IWDG défini dans main.c */
//extern IWDG_HandleTypeDef hiwdg;
//
///* ================================================================
// * TABLE DE CARTOGRAPHIE DES ZONES FLASH
// * Doit correspondre exactement à flash_map_backend.h
// * ================================================================ */
//static const struct flash_area flash_map[] = {
//    {
//        .fa_id        = FLASH_AREA_BOOTLOADER,
//        .fa_device_id = 0,
//        .fa_off       = FLASH_AREA_BOOTLOADER_OFFSET,
//        .fa_size      = FLASH_AREA_BOOTLOADER_SIZE,
//    },
//    {
//        .fa_id        = FLASH_AREA_IMAGE_PRIMARY(0),
//        .fa_device_id = 0,
//        .fa_off       = FLASH_AREA_IMAGE_0_OFFSET,
//        .fa_size      = FLASH_AREA_IMAGE_0_SIZE,
//    },
//    {
//        .fa_id        = FLASH_AREA_IMAGE_SECONDARY(0),
//        .fa_device_id = 0,
//        .fa_off       = FLASH_AREA_IMAGE_1_OFFSET,
//        .fa_size      = FLASH_AREA_IMAGE_1_SIZE,
//    },
//    {
//        .fa_id        = FLASH_AREA_IMAGE_SCRATCH,
//        .fa_device_id = 0,
//        .fa_off       = FLASH_AREA_IMAGE_SCRATCH_OFFSET,
//        .fa_size      = FLASH_AREA_IMAGE_SCRATCH_SIZE,
//    },
//};
//
//#define FLASH_MAP_ENTRY_COUNT \
//    (sizeof(flash_map) / sizeof(flash_map[0]))
//
///* ================================================================
// * FONCTIONS UTILITAIRES INTERNES
// * ================================================================ */
//
///*
// * Convertit un offset relatif (depuis FLASH_BASE) en adresse absolue.
// */
//static inline uint32_t flash_abs_addr(const struct flash_area *fa,
//                                       uint32_t off)
//{
//    return FLASH_BASE_ADDRESS + fa->fa_off + off;
//}
//
///*
// * Retourne le numéro de page Flash à partir d'une adresse absolue.
// * Le L4R5 (1 Mo monobanque) numérote ses pages de 0 à 255.
// */
//static inline uint32_t addr_to_page(uint32_t abs_addr)
//{
//    return (abs_addr - FLASH_BASE_ADDRESS) / FLASH_PAGE_SIZE;
//}
//
///*
// * Vérifie que l'accès (offset + longueur) reste dans les bornes de la zone.
// * Retourne 0 si OK, -1 si débordement.
// */
//static int flash_area_check_bounds(const struct flash_area *fa,
//                                    uint32_t off, uint32_t len)
//{
//    if (off + len > fa->fa_size) {
//        MCUBOOT_LOG_ERR("Flash bounds check failed: off=0x%08X len=0x%X "
//                        "size=0x%X", off, len, fa->fa_size);
//        return -1;
//    }
//    return 0;
//}
//
///* ================================================================
// * flash_area_open()
// * Retrouve une zone par son ID et retourne un pointeur vers sa
// * structure. Pas de ressource hardware à allouer ici.
// * ================================================================ */
//int flash_area_open(uint8_t id, const struct flash_area **fap)
//{
//    if (fap == NULL) {
//        return -1;
//    }
//
//    for (uint32_t i = 0; i < FLASH_MAP_ENTRY_COUNT; i++) {
//        if (flash_map[i].fa_id == id) {
//            *fap = &flash_map[i];
//            MCUBOOT_LOG_DBG("flash_area_open: id=%d offset=0x%08X size=0x%X",
//                            id, flash_map[i].fa_off, flash_map[i].fa_size);
//            return 0;
//        }
//    }
//
//    MCUBOOT_LOG_ERR("flash_area_open: ID %d non trouvé", id);
//    return -1;
//}
//
///* ================================================================
// * flash_area_close()
// * Libère la zone. Ici, rien à libérer — on s'assure juste que
// * la Flash est reverouillée.
// * ================================================================ */
//void flash_area_close(const struct flash_area *fa)
//{
//    (void)fa;
//    /* S'assurer que la Flash est bien verrouillée après utilisation */
//    HAL_FLASH_Lock();
//}
//
///* ================================================================
// * flash_area_read()
// * Lecture depuis la Flash : simple copie mémoire (la Flash est
// * mappée en XIP sur le bus AHB du Cortex-M4).
// * Pas besoin de déverrouillage pour la lecture.
// * ================================================================ */
//int flash_area_read(const struct flash_area *fa, uint32_t off,
//                    void *dst, uint32_t len)
//{
//    if (fa == NULL || dst == NULL) {
//        return -1;
//    }
//
//    if (flash_area_check_bounds(fa, off, len) != 0) {
//        return -1;
//    }
//
//    /* Lecture directe via le bus mémoire (0 wait-state avec ART) */
//    uint32_t src_addr = flash_abs_addr(fa, off);
//    memcpy(dst, (const void *)src_addr, len);
//
//    return 0;
//}
//
///* ================================================================
// * flash_area_write()
// * Écriture dans la Flash :
// *   - Le STM32L4R5 exige une écriture par DOUBLE-MOT (64 bits = 8 octets)
// *   - La zone à écrire doit avoir été préalablement effacée (valeur 0xFF)
// *   - L'adresse de destination doit être alignée sur 8 octets
// * ================================================================ */
//int flash_area_write(const struct flash_area *fa, uint32_t off,
//                     const void *src, uint32_t len)
//{
//    HAL_StatusTypeDef status;
//    uint32_t abs_addr;
//    uint64_t dword;
//    uint32_t bytes_written = 0;
//
//    if (fa == NULL || src == NULL) {
//        return -1;
//    }
//
//    /* Vérification des bornes */
//    if (flash_area_check_bounds(fa, off, len) != 0) {
//        return -1;
//    }
//
//    /* L'adresse et la longueur doivent être alignées sur 8 octets */
//    if ((off % FLASH_WRITE_ALIGNMENT != 0) ||
//        (len % FLASH_WRITE_ALIGNMENT != 0)) {
//        MCUBOOT_LOG_ERR("flash_area_write: alignement invalide "
//                        "off=0x%X len=0x%X (doit être multiple de %d)",
//                        off, len, FLASH_WRITE_ALIGNMENT);
//        return -1;
//    }
//
//    abs_addr = flash_abs_addr(fa, off);
//
//    MCUBOOT_LOG_DBG("flash_area_write: addr=0x%08X len=0x%X",
//                    abs_addr, len);
//
//    /* Déverrouillage de la Flash */
//    status = HAL_FLASH_Unlock();
//    if (status != HAL_OK) {
//        MCUBOOT_LOG_ERR("flash_area_write: déverrouillage échoué (%d)",
//                        status);
//        return -1;
//    }
//
//    /* Effacement du cache Flash avant écriture */
//    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
//
//    /* Écriture double-mot par double-mot (64 bits = 8 octets) */
//    while (bytes_written < len) {
//
//        /* Construire le double-mot depuis le buffer source */
//        memcpy(&dword,
//               (const uint8_t *)src + bytes_written,
//               sizeof(uint64_t));
//
//        /* Écriture HW — FLASH_TYPEPROGRAM_DOUBLEWORD est obligatoire */
//        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
//                                   abs_addr + bytes_written,
//                                   dword);
//
//        if (status != HAL_OK) {
//            MCUBOOT_LOG_ERR("flash_area_write: erreur écriture à "
//                            "0x%08X (HAL status=%d, FLASH_SR=0x%08X)",
//                            abs_addr + bytes_written, status, FLASH->SR);
//            HAL_FLASH_Lock();
//            return -1;
//        }
//
//        bytes_written += sizeof(uint64_t);
//
//        /* Nourrir le watchdog pendant les longues écritures */
//        MCUBOOT_WATCHDOG_FEED();
//    }
//
//    /* Vérification post-écriture : relire et comparer */
//    if (memcmp((const void *)abs_addr, src, len) != 0) {
//        MCUBOOT_LOG_ERR("flash_area_write: vérification post-écriture "
//                        "échouée à 0x%08X", abs_addr);
//        HAL_FLASH_Lock();
//        return -1;
//    }
//
//    HAL_FLASH_Lock();
//
//    MCUBOOT_LOG_DBG("flash_area_write: OK (%d octets écrits)", len);
//    return 0;
//}
//
///* ================================================================
// * flash_area_erase()
// * Effacement par pages de 4 Ko.
// * L'effacement met tous les bits à 1 (valeur 0xFF par octet).
// * ================================================================ */
//int flash_area_erase(const struct flash_area *fa, uint32_t off,
//                     uint32_t len)
//{
//    HAL_StatusTypeDef    status;
//    FLASH_EraseInitTypeDef erase_init;
//    uint32_t             page_error;
//    uint32_t             abs_addr;
//    uint32_t             first_page;
//    uint32_t             nb_pages;
//
//    if (fa == NULL) {
//        return -1;
//    }
//
//    /* L'offset et la longueur doivent être alignés sur une page (4 Ko) */
//    if ((off % FLASH_PAGE_SIZE != 0) || (len % FLASH_PAGE_SIZE != 0)) {
//        MCUBOOT_LOG_ERR("flash_area_erase: alignement page invalide "
//                        "off=0x%X len=0x%X (doit être multiple de 0x%X)",
//                        off, len, FLASH_PAGE_SIZE);
//        return -1;
//    }
//
//    if (flash_area_check_bounds(fa, off, len) != 0) {
//        return -1;
//    }
//
//    abs_addr   = flash_abs_addr(fa, off);
//    first_page = addr_to_page(abs_addr);
//    nb_pages   = len / FLASH_PAGE_SIZE;
//
//    MCUBOOT_LOG_INF("flash_area_erase: page %d → page %d (%d pages, %d Ko)",
//                    first_page, first_page + nb_pages - 1,
//                    nb_pages, len / 1024);
//
//    /* Déverrouillage */
//    status = HAL_FLASH_Unlock();
//    if (status != HAL_OK) {
//        MCUBOOT_LOG_ERR("flash_area_erase: déverrouillage échoué");
//        return -1;
//    }
//
//    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
//
//    /*
//     * Configuration de l'effacement.
//     * STM32L4R5VGT6 avec 1 Mo = monobanque → FLASH_BANK_1 uniquement.
//     * (La version 2 Mo est bibanque avec FLASH_BANK_1 et FLASH_BANK_2)
//     */
//    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
//    erase_init.Banks     = FLASH_BANK_1;     /* Toujours BANK_1 sur 1 Mo */
//    erase_init.Page      = first_page;
//    erase_init.NbPages   = nb_pages;
//
//    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
//
//    if (status != HAL_OK) {
//        MCUBOOT_LOG_ERR("flash_area_erase: erreur à la page %d "
//                        "(HAL status=%d, FLASH_SR=0x%08X)",
//                        page_error, status, FLASH->SR);
//        HAL_FLASH_Lock();
//        return -1;
//    }
//
//    /* Nourrir le watchdog après chaque effacement (opération longue) */
//    MCUBOOT_WATCHDOG_FEED();
//
//    HAL_FLASH_Lock();
//
//    MCUBOOT_LOG_DBG("flash_area_erase: OK");
//    return 0;
//}
//
///* ================================================================
// * flash_area_align()
// * Retourne l'alignement d'écriture minimal requis par le hardware.
// * STM32L4R5 : 8 octets (double-mot 64 bits).
// * ================================================================ */
//uint32_t flash_area_align(const struct flash_area *fa)
//{
//    (void)fa;
//    return FLASH_WRITE_ALIGNMENT;   /* 8 octets */
//}
//
///* ================================================================
// * flash_area_erased_val()
// * Retourne la valeur d'un octet effacé.
// * Sur Flash NOR (STM32), l'effacement met tout à 0xFF.
// * ================================================================ */
//uint32_t flash_area_erased_val(const struct flash_area *fa)
//{
//    (void)fa;
//    return 0xFF;
//}
//
///* ================================================================
// * flash_area_get_sectors()
// * Retourne la liste des secteurs (pages) d'une zone Flash.
// * MCUBoot l'utilise pour planifier les opérations de swap.
// * ================================================================ */
//
///* ================================================================
// * ACCESSEURS flash_area
// * Convertis de static inline en fonctions normales pour éviter
// * les problèmes de linkage avec bootutil/src/
// * ================================================================ */
//
//uint32_t flash_area_get_off(const struct flash_area *fa)
//{
//    return fa->fa_off;
//}
//
//uint32_t flash_area_get_size(const struct flash_area *fa)
//{
//    return fa->fa_size;
//}
//
//uint8_t flash_area_get_id(const struct flash_area *fa)
//{
//    return fa->fa_id;
//}
//
//uint8_t flash_area_get_device_id(const struct flash_area *fa)
//{
//    return fa->fa_device_id;
//}
//
///* ================================================================
// * ACCESSEURS flash_sector
// * ================================================================ */
//
//uint32_t flash_sector_get_off(const struct flash_sector *fs)
//{
//    return fs->fs_off;
//}
//
//uint32_t flash_sector_get_size(const struct flash_sector *fs)
//{
//    return fs->fs_size;
//}
//
///* ================================================================
// * flash_area_get_sector()
// * Trouve le secteur (page 4 Ko) qui contient l'offset 'off'
// * dans la zone flash 'fa', et remplit la structure 'sector'.
// *
// * Appelée par bootutil_area.c pour les opérations d'effacement
// * et de calcul du trailer.
// * ================================================================ */
//int flash_area_get_sector(const struct flash_area *fa,
//                           uint32_t off,
//                           struct flash_sector *sector)
//{
//    uint32_t sector_index;
//
//    if (fa == NULL || sector == NULL) {
//        return -1;
//    }
//
//    if (off >= fa->fa_size) {
//        return -1;
//    }
//
//    /* Calculer l'index de la page qui contient cet offset */
//    sector_index = off / MCUBOOT_FLASH_PAGE_SIZE;
//
//    /* Remplir la structure sector */
//    sector->fs_off  = sector_index * MCUBOOT_FLASH_PAGE_SIZE;
//    sector->fs_size = MCUBOOT_FLASH_PAGE_SIZE;
//
//    return 0;
//}
//
//int flash_area_get_sectors(int fa_id, uint32_t *count,
//                            struct flash_sector *sectors)
//{
//    const struct flash_area *fa;
//    uint32_t nb_sectors;
//    uint32_t i;
//
//    if (count == NULL || sectors == NULL) {
//        return -1;
//    }
//
//    /* Retrouver la zone par son ID */
//    if (flash_area_open((uint8_t)fa_id, &fa) != 0) {
//        return -1;
//    }
//
//    /* Nombre de pages (secteurs) dans cette zone */
//    nb_sectors = fa->fa_size / FLASH_PAGE_SIZE;
//
//    if (*count < nb_sectors) {
//        MCUBOOT_LOG_ERR("flash_area_get_sectors: buffer trop petit "
//                        "(%d < %d)", *count, nb_sectors);
//        return -1;
//    }
//
//    /* Remplir le tableau de secteurs */
//    for (i = 0; i < nb_sectors; i++) {
//        sectors[i].fs_off  = i * FLASH_PAGE_SIZE;
//        sectors[i].fs_size = FLASH_PAGE_SIZE;
//    }
//
//    *count = nb_sectors;
//
//    MCUBOOT_LOG_DBG("flash_area_get_sectors: fa_id=%d → %d secteurs "
//                    "de %d Ko", fa_id, nb_sectors, FLASH_PAGE_SIZE / 1024);
//
//    return 0;
//}
//
///* ================================================================
// * flash_area_id_from_multi_image_slot_hook()
// * Signature correcte : 3 paramètres — le 3ème est un code retour
// * ================================================================ */
//int flash_area_id_from_multi_image_slot_hook(int image_index, int slot,
//                                              int *rc)
//{
//    int fa_id;
//
//    switch (slot) {
//        case 0:
//            fa_id = FLASH_AREA_IMAGE_PRIMARY(image_index);
//            break;
//        case 1:
//            fa_id = FLASH_AREA_IMAGE_SECONDARY(image_index);
//            break;
//        default:
//            MCUBOOT_LOG_ERR("slot invalide %d", slot);
//            if (rc != NULL) { *rc = -1; }
//            return -1;
//    }
//
//    if (rc != NULL) { *rc = 0; }
//    return fa_id;
//}
//
///* ================================================================
// * flash_area_id_from_multi_image_slot()
// * Wrapper legacy — appelle le hook
// * ================================================================ */
//int flash_area_id_from_multi_image_slot(int image_index, int slot)
//{
//    int rc;
//    return flash_area_id_from_multi_image_slot_hook(image_index, slot, &rc);
//}
//
///* ================================================================
// * flash_area_to_sectors()
// * boot_sector_t = struct flash_area dans MCUBoot v2.x
// * Chaque "secteur" est décrit par une flash_area (off + size)
// * ================================================================ */
//int flash_area_to_sectors(int fa_id, int *count,
//                           struct flash_area *sectors)
//{
//    const struct flash_area *fa;
//    uint32_t nb_sectors;
//    uint32_t i;
//
//    if (count == NULL || sectors == NULL) {
//        return -1;
//    }
//
//    if (flash_area_open((uint8_t)fa_id, &fa) != 0) {
//        return -1;
//    }
//
//    nb_sectors = fa->fa_size / MCUBOOT_FLASH_PAGE_SIZE;
//
//    if ((uint32_t)(*count) < nb_sectors) {
//        MCUBOOT_LOG_ERR("flash_area_to_sectors: buffer trop petit "
//                        "(%d < %d)", *count, nb_sectors);
//        return -1;
//    }
//
//    /* Remplir le tableau — chaque entrée est une flash_area
//     * décrivant un secteur (page 4 Ko) de cette zone */
//    for (i = 0; i < nb_sectors; i++) {
//        sectors[i].fa_id        = (uint8_t)fa_id;
//        sectors[i].fa_device_id = fa->fa_device_id;
//        sectors[i].fa_off       = fa->fa_off + (i * MCUBOOT_FLASH_PAGE_SIZE);
//        sectors[i].fa_size      = MCUBOOT_FLASH_PAGE_SIZE;
//    }
//
//    *count = (int)nb_sectors;
//    return 0;
//}
//
///* ================================================================
// * flash_area_id_from_image_slot()
// * Version simplifiée pour image_index = 0 (image unique).
// * Appelée par split_go() dans loader.c de MCUBoot.
// * ================================================================ */
//int flash_area_id_from_image_slot(int slot)
//{
//    /* image_index = 0 car on n'a qu'une seule image (MCUBOOT_IMAGE_NUMBER = 1) */
//    return flash_area_id_from_multi_image_slot(0, slot);
//}
