/*
 * mcuboot_watchdog.h
 * Wrapper watchdog accessible depuis tous les fichiers MCUBoot
 */

#ifndef MCUBOOT_WATCHDOG_H
#define MCUBOOT_WATCHDOG_H

/* Déclaration de la fonction implémentée dans main.c */
void mcuboot_watchdog_feed(void);

#endif /* MCUBOOT_WATCHDOG_H */
