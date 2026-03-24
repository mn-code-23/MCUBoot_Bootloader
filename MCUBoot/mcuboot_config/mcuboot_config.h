/*
 * mcuboot_config.h
 * Configuration MCUBoot pour STM32L4R5VGT6
 * Projet : MCUBoot_Bootloader
 */

#ifndef MCUBOOT_CONFIG_H
#define MCUBOOT_CONFIG_H

#define MCUBOOT_HAVE_LOGGING     1    /* Mettre à 0 en production */

#include "mcuboot_logging.h"

/* ================================================================
 * 1. ALGORITHME DE SIGNATURE
 * ECDSA-P256 : recommandé pour Cortex-M4
 *   - Clé plus courte que RSA (32 octets vs 256+)
 *   - Vérification plus rapide avec accélérations HW
 * ================================================================ */

#define MCUBOOT_FIH_PROFILE_OFF         1

/*#define MCUBOOT_SIGN_EC256 */
#define MCUBOOT_SIGN_EC256              1
/* ================================================================
 * 2. ALGORITHME DE SWAP
 * SWAP_USING_MOVE : algorithme le plus adapté aux STM32
 *   - Déplace les pages une par une (pas besoin d'espace scratch
 *     égal à un slot entier)
 *   - Plus rapide que SWAP_USING_SCRATCH sur L4R5
 * ================================================================ */
/*#define MCUBOOT_SWAP_USING_MOVE */
#define MCUBOOT_SWAP_USING_MOVE         1

/* ================================================================
 * 3. VALIDATION SYSTÉMATIQUE
 * Vérifie la signature du Slot 0 à CHAQUE démarrage.
 * OBLIGATOIRE pour un vrai Secure Boot.
 * ================================================================ */
/*#define MCUBOOT_VALIDATE_PRIMARY_SLOT */
#define MCUBOOT_VALIDATE_PRIMARY_SLOT   1

/* ================================================================
 * 4. NOMBRE D'IMAGES GÉRÉES
 * 1 = une seule application (cas standard)
 * 2 = app + noyau séparés (cas avancé, non utilisé ici)
 * ================================================================ */
#define MCUBOOT_IMAGE_NUMBER  1

/* ================================================================
 * 5. TAILLE DU HEADER IMAGE
 * 0x400 = 1024 octets — espace réservé en tête de chaque image
 * pour le header MCUBoot (magic, flags, version, hash, signature)
 * ================================================================ */
/*#define BOOT_MAX_IMG_SECTORS   96 */   /* 384 Ko / 4 Ko par page */
/*REMPLACER par la macro que MCUBoot attend */
#define MCUBOOT_MAX_IMG_SECTORS         96

/* ================================================================
 * 6. TAILLE DE PAGE FLASH DU STM32L4R5
 * Le L4R5 a des pages de 4 Ko (0x1000) sur sa Flash interne
 * ================================================================ */
#define MCUBOOT_BOOT_MAX_ALIGN  8     /* Alignement écriture 64 bits */

/* ================================================================
 * 7. WATCHDOG
 * Activer le feed du watchdog pendant les opérations longues
 * (swap de slots). Adapter selon votre implémentation IWDG.
 * ================================================================ */
/* Remplacer l'ancienne macro par un appel de fonction */
#include "mcuboot_watchdog.h"
#define MCUBOOT_WATCHDOG_FEED()   mcuboot_watchdog_feed()
/* ================================================================
 * 8. LOGS DE DEBUG
 * Activer pendant le développement, DÉSACTIVER en production
 * pour réduire la taille et supprimer les informations sensibles
 * ================================================================ */

/* ================================================================
 * 9. ASSERT
 * ================================================================ */
#define MCUBOOT_ASSERT(x)                          \
    do {                                           \
        if (!(x)) {                                \
            while(1) { __NOP(); }                  \
        }                                          \
    } while(0)

/* ================================================================
 * 10. CRYPTOGRAPHIE — Backend Mbed TLS
 * ================================================================ */
/*#define MCUBOOT_USE_MBED_TLS */
#define MCUBOOT_USE_MBED_TLS            1

/* ================================================================
 * SYSTÈME DE LOG — Macros requises par MCUBoot internement
 * MCUBOOT_LOG_MODULE_DECLARE et REGISTER doivent être définis,
 * même vides, pour que bootutil_area.c compile correctement.
 * ================================================================ */
#define MCUBOOT_LOG_MODULE_DECLARE(module)   /* vide */
#define MCUBOOT_LOG_MODULE_REGISTER(module)  /* vide */

#define MBEDTLS_CONFIG_FILE "mbedtls_config.h"
#undef  MBEDTLS_HAVE_TIME
#undef  MBEDTLS_HAVE_TIME_DATE
#undef  MBEDTLS_PLATFORM_TIME_ALT
#undef  MBEDTLS_PLATFORM_TIME_MACRO
#undef  MBEDTLS_TIMING_C



#endif /* MCUBOOT_CONFIG_H */
