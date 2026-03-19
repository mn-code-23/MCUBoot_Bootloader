/*
 * mbedtls_config.h
 * Configuration Mbed TLS minimale pour MCUBoot / STM32L4R5
 * Objectif : vérification ECDSA-P256 + SHA-256 uniquement
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* ================================================================
 * MODULES OBLIGATOIRES pour ECDSA-P256
 * ================================================================ */

/* Arithmétique big number (base de toute la crypto asymétrique) */
#define MBEDTLS_BIGNUM_C

/* Courbe elliptique P-256 */
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* ECDSA — vérification de signature */
#define MBEDTLS_ECDSA_C

/* SHA-256 — hash de l'image */
#define MBEDTLS_SHA256_C

/* ASN.1 — format des clés et signatures */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C

/* OID — Object Identifiers pour les clés */
#define MBEDTLS_OID_C

/* Clés publiques ECC */
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

/* ================================================================
 * OPTIMISATIONS MÉMOIRE — CRITIQUE pour tenir en 128 Ko
 * ================================================================ */

/* Désactiver les modules inutiles */
/* #define MBEDTLS_AES_C          */   /* Pas d'AES (pas de chiffrement) */
/* #define MBEDTLS_RSA_C          */   /* On utilise ECDSA, pas RSA       */
/* #define MBEDTLS_X509_USE_C     */   /* Pas de certificats X.509        */
/* #define MBEDTLS_SSL_TLS_C      */   /* Pas de TLS dans le bootloader   */
/* #define MBEDTLS_NET_C          */   /* Pas de réseau                   */
/* #define MBEDTLS_ENTROPY_C      */   /* Pas besoin d'entropie            */
/* #define MBEDTLS_CTR_DRBG_C     */   /* Pas de PRNG                     */

/* Optimisation taille des courbes ECP */
#define MBEDTLS_ECP_NIST_OPTIM          /* Optimisation NIST P-256        */
#define MBEDTLS_ECP_MAX_BITS  256       /* Limiter aux courbes 256 bits   */

/* Réduire l'empreinte mémoire des calculs ECP */
#define MBEDTLS_ECP_WINDOW_SIZE        2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0   /* Économise ~7 Ko de RAM */

/* ================================================================
 * ACCÉLÉRATEURS MATÉRIELS STM32L4R5
 * Le L4R5 dispose d'un accélérateur HASH (SHA-256 HW)
 * Décommenter quand le HAL crypto sera implémenté
 * ================================================================ */
/* #define MBEDTLS_SHA256_ALT  */   /* Remplace SHA-256 SW par HW STM32 */

/* ================================================================
 * PLATEFORME
 * ================================================================ */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY         /* Utiliser malloc/free custom    */
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C   /* Allocateur sur buffer statique */

/* Taille du buffer d'allocation statique (en octets) */
/* ECDSA-P256 a besoin d'environ 8-10 Ko de heap temporaire */
#define MBEDTLS_MEMORY_BUFFER_SIZE   (12 * 1024)

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
