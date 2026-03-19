/*
 * mcuboot_logging.h
 * Redirection des logs MCUBoot vers UART2
 */

#ifndef MCUBOOT_LOGGING_H
#define MCUBOOT_LOGGING_H

#include <stdio.h>

#if MCUBOOT_HAVE_LOGGING

/* Utilise printf redirigé vers UART via _write() de syscalls.c */
#define MCUBOOT_LOG_ERR(_fmt, ...)   \
    printf("[MCUBoot][ERR] " _fmt "\r\n", ##__VA_ARGS__)

#define MCUBOOT_LOG_WRN(_fmt, ...)   \
    printf("[MCUBoot][WRN] " _fmt "\r\n", ##__VA_ARGS__)

#define MCUBOOT_LOG_INF(_fmt, ...)   \
    printf("[MCUBoot][INF] " _fmt "\r\n", ##__VA_ARGS__)

#define MCUBOOT_LOG_DBG(_fmt, ...)   \
    printf("[MCUBoot][DBG] " _fmt "\r\n", ##__VA_ARGS__)

#else

/* Production : logs désactivés, zéro overhead */
#define MCUBOOT_LOG_ERR(...)   ((void)0)
#define MCUBOOT_LOG_WRN(...)   ((void)0)
#define MCUBOOT_LOG_INF(...)   ((void)0)
#define MCUBOOT_LOG_DBG(...)   ((void)0)

#endif /* MCUBOOT_HAVE_LOGGING */

#endif /* MCUBOOT_LOGGING_H */
