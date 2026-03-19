/*
 * flash_map_backend.h
 * Cartographie Flash pour MCUBoot / STM32L4R5VGT6
 *
 * Mémoire Flash totale : 2 Mo (2 banques x 1 Mo)
 * Page size           : 4 Ko (0x1000)
 */

#ifndef FLASH_MAP_BACKEND_H
#define FLASH_MAP_BACKEND_H

#include <stdint.h>
#include <stddef.h>

/* ================================================================
 * IDENTIFIANTS DES ZONES FLASH (FA = Flash Area)
 * ================================================================ */
#define FLASH_AREA_BOOTLOADER         0   /* MCUBoot lui-même        */
/*#define FLASH_AREA_IMAGE_PRIMARY(x)   (1 + 2*(x))  */ /* Slot 0 actif */
/*#define FLASH_AREA_IMAGE_SECONDARY(x) (2 + 2*(x))  */ /* Slot 1 update*/
#define FLASH_AREA_IMAGE_SCRATCH      3   /* Zone temporaire swap    */

/* ================================================================
 * CARTOGRAPHIE PHYSIQUE DE LA FLASH — STM32L4R5VGT6 (1 Mo)
 *
 *  0x0800 0000 ┌──────────────────────┐
 *              │  BOOTLOADER (128 Ko) │  FA_ID = 0
 *  0x0802 0000 ├──────────────────────┤
 *              │  SLOT 0 - PRIMARY    │  FA_ID = 1
 *              │      (384 Ko)        │
 *  0x0808 0000 ├──────────────────────┤
 *              │  SLOT 1 - SECONDARY  │  FA_ID = 2
 *              │      (384 Ko)        │
 *  0x080E 0000 ├──────────────────────┤
 *              │  SCRATCH   (64 Ko)   │  FA_ID = 3
 *  0x080F 0000 ├──────────────────────┤
 *              │  USER DATA (64 Ko)   │
 *  0x0810 0000 └──────────────────────┘  ← Fin Flash 1 Mo
 * ================================================================ */

/* ================================================================
 * INCLURE sysflash.h QUI DÉFINIT LES IDs OFFICIELLEMENT
 * Ne pas redéfinir ici ce que sysflash.h définit déjà
 * ================================================================ */
#include "../bootutil/include/sysflash/sysflash.h"   /* FLASH_AREA_IMAGE_PRIMARY/SECONDARY */

/* ================================================================
 * CARTOGRAPHIE PHYSIQUE DE LA FLASH — STM32L4R5VGT6 (1 Mo)
 * ================================================================ */
#define FLASH_BASE_ADDRESS              0x08000000UL
#define MCUBOOT_FLASH_PAGE_SIZE         0x1000        /* 4 Ko par page */
#define FLASH_WRITE_ALIGNMENT           8
#define FLASH_TOTAL_SIZE                0x00100000    /* 1 Mo          */

/* Bootloader */
#define FLASH_AREA_BOOTLOADER_OFFSET    0x00000000
#define FLASH_AREA_BOOTLOADER_SIZE      0x00020000    /* 128 Ko */

/* Slot 0 */
#define FLASH_AREA_IMAGE_0_OFFSET       0x00020000    /* 0x0802 0000 */
#define FLASH_AREA_IMAGE_0_SIZE         0x00060000    /* 384 Ko      */

/* Slot 1 */
#define FLASH_AREA_IMAGE_1_OFFSET       0x00080000    /* 0x0808 0000 */
#define FLASH_AREA_IMAGE_1_SIZE         0x00060000    /* 384 Ko      */

/* Scratch */
#define FLASH_AREA_IMAGE_SCRATCH_OFFSET 0x000E0000    /* 0x080E 0000 */
#define FLASH_AREA_IMAGE_SCRATCH_SIZE   0x00010000    /* 64 Ko       */

/* User Data */
#define FLASH_AREA_USER_DATA_OFFSET     0x000F0000    /* 0x080F 0000 */
#define FLASH_AREA_USER_DATA_SIZE       0x00010000    /* 64 Ko       */

/* ================================================================
 * STRUCTURE flash_area — Interface MCUBoot
 * ================================================================ */
struct flash_area {
    uint8_t  fa_id;         /* Identifiant de la zone       */
    uint8_t  fa_device_id;  /* ID device (0 = Flash interne)*/
    uint32_t fa_off;        /* Offset depuis FLASH_BASE     */
    uint32_t fa_size;       /* Taille en octets             */
};

struct flash_sector {
    uint32_t fs_off;        /* Offset depuis début zone     */
    uint32_t fs_size;       /* Taille du secteur            */
};

/* ================================================================
 * PROTOTYPES — à implémenter dans flash_map_backend.c (Phase 3)
 * ================================================================ */
int  flash_area_open (uint8_t id, const struct flash_area **fap);
void flash_area_close(const struct flash_area *fa);
int  flash_area_read (const struct flash_area *fa, uint32_t off,
                      void *dst, uint32_t len);
int  flash_area_write(const struct flash_area *fa, uint32_t off,
                      const void *src, uint32_t len);
int  flash_area_erase(const struct flash_area *fa, uint32_t off,
                      uint32_t len);
uint32_t flash_area_align    (const struct flash_area *fa);
uint32_t flash_area_erased_val(const struct flash_area *fa);
int  flash_area_get_sectors  (int fa_id, uint32_t *count,
                               struct flash_sector *sectors);

/* ================================================================
 * API ACCESSEURS — Requises par MCUBoot v2.x
 * ================================================================ */

/* ================================================================
 * ACCESSEURS — déclarations simples (implémentées dans le .c)
 * ================================================================ */

/* Accesseurs pour flash_area */

uint32_t flash_area_get_off       (const struct flash_area *fa);
uint32_t flash_area_get_size      (const struct flash_area *fa);
uint8_t  flash_area_get_id        (const struct flash_area *fa);
uint8_t  flash_area_get_device_id (const struct flash_area *fa);

//inline uint32_t flash_area_get_off(const struct flash_area *fa)
//{
//    return fa->fa_off;
//}
//
//inline uint32_t flash_area_get_size(const struct flash_area *fa)
//{
//    return fa->fa_size;
//}
//
//inline uint8_t flash_area_get_id(const struct flash_area *fa)
//{
//    return fa->fa_id;
//}
//
//inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
//{
//    return fa->fa_device_id;
//}

/* Accesseurs pour flash_sector */

uint32_t flash_sector_get_off     (const struct flash_sector *fs);
uint32_t flash_sector_get_size    (const struct flash_sector *fs);

//inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
//{
//    return fs->fs_off;
//}
//
//inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
//{
//    return fs->fs_size;
//}

/* flash_area_get_sector() : trouve le secteur qui contient l'offset donné */
int flash_area_get_sector(const struct flash_area *fa,
                           uint32_t off,
                           struct flash_sector *sector);
//int flash_area_get_sector(const struct flash_area *fa, uint32_t off,
//                           struct flash_sector *sector);

/* Fonction legacy requise par bootutil_misc.c */
int flash_area_to_sectors(int fa_id, int *count,
                           struct flash_area *sectors);

/* ================================================================
 * flash_area_id_from_multi_image_slot_hook()
 * Fonction hook appelée par bootutil_loader.c (MCUBoot v2.x)
 * Convertit (image_index, slot) → ID de zone Flash
 * ================================================================ */
int flash_area_id_from_multi_image_slot_hook(int image_index, int slot, int *rc);


/* ================================================================
 * flash_area_id_from_multi_image_slot()
 * Wrapper legacy — appelle le hook pour compatibilité
 * ================================================================ */
int flash_area_id_from_multi_image_slot(int image_index, int slot);

/* ================================================================
 * flash_area_id_to_multi_image_slot()
 * Conversion inverse : ID de zone Flash → numéro de slot
 * ================================================================ */
int flash_area_id_to_multi_image_slot(int image_index, int area_id);

/* Wrapper simplifié pour image unique — appelé par split_go() dans loader.c */
int flash_area_id_from_image_slot(int slot);

#endif /* FLASH_MAP_BACKEND_H */
