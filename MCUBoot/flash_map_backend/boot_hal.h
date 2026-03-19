/*
 * boot_hal.h
 * Interface publique du HAL Shim bootloader
 */

#ifndef BOOT_HAL_H
#define BOOT_HAL_H

#include <stdint.h>
#include "../bootutil/include/bootutil/bootutil.h"

struct boot_arm_vector_table;

/*
 * Adresse de début de l'application dans le Slot 0.
 * = Adresse Slot 0 + Taille header MCUBoot
 * = 0x08020000 + 0x400 = 0x08020400
 */
#define APP_START_ADDRESS    0x08020400UL

/*
 * Adresse du début du Slot 0 (sans le header).
 * Utilisée pour le VTOR.
 */
#define APP_SLOT0_ADDRESS    0x08020000UL

void boot_hal_jump_to_app  (uint32_t app_start);
void boot_platform_quit    (struct boot_arm_vector_table *vt);

#endif /* BOOT_HAL_H */
