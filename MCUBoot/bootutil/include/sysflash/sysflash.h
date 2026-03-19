/*
 * sysflash/sysflash.h
 * Définitions des IDs de zones Flash pour MCUBoot
 * Adapté pour STM32L4R5VGT6 — 1 Mo Flash
 */

#ifndef SYSFLASH_H
#define SYSFLASH_H

#include "../../../mcuboot_config/mcuboot_config.h"

/*
 * Identifiants des zones Flash utilisés par MCUBoot en interne.
 * Ces valeurs correspondent à notre flash_map[] dans flash_map_backend.c
 */

/* Slot primaire (application active) */
#define FLASH_AREA_IMAGE_PRIMARY(x)     (1 + 2 * (x))

/* Slot secondaire (zone OTA) */
#define FLASH_AREA_IMAGE_SECONDARY(x)   (2 + 2 * (x))

/* Zone scratch pour le swap */
#define FLASH_AREA_IMAGE_SCRATCH        3

/* Bootloader lui-même */
#define FLASH_AREA_BOOTLOADER           0

/*
 * Nombre maximum d'images (1 dans notre cas)
 */
#ifndef MCUBOOT_IMAGE_NUMBER
#define MCUBOOT_IMAGE_NUMBER  1
#endif

/*
 * Device ID de la Flash interne STM32
 */
#define FLASH_DEVICE_INTERNAL_FLASH     0

#endif /* SYSFLASH_H */
