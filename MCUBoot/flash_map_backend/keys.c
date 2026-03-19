/*
 * keys.c — Clé publique ECDSA-P256 de production
 *
 * Générée par : imgtool keygen --key my_signing_key.pem --type ecdsa-p256
 * Extraite par : imgtool getpub  --key my_signing_key.pem
 *
 * Date de génération : 17/03/2026
 *
 * ⚠️ SÉCURITÉ :
 *   - Ce fichier contient UNIQUEMENT la clé PUBLIQUE → peut être versionné
 *   - La clé PRIVÉE (my_signing_key.pem) est stockée dans :
 *     E:\STM32\MCUBoot_Keys\ (hors du dépôt Git)
 *   - Ne JAMAIS committer my_signing_key.pem dans le dépôt
 */

#include "../bootutil/include/bootutil/sign_key.h"

/* ================================================================
 * Clé publique ECDSA-P256
 * Copier ici EXACTEMENT le tableau généré par imgtool getpub
 * ================================================================ */
static const unsigned char ecdsa_pub_key[] = {
	    0x04,
	    /* Coordonnée X — 32 octets */
	    0xed, 0xb7, 0x27, 0xb6, 0x68, 0x29, 0xa0, 0x26,
	    0xb6, 0xb9, 0xe1, 0x50, 0xed, 0x55, 0xb0, 0x83,
	    0x27, 0x10, 0xa8, 0xc7, 0x9c, 0x66, 0xbb, 0x9f,
	    0x73, 0xbf, 0xad, 0x63, 0x3c, 0x54, 0xda, 0x32,
	    /* Coordonnée Y — 32 octets */
	    0x4a, 0x29, 0xe5, 0x6a, 0xf1, 0x42, 0xd7, 0xef,
	    0x6e, 0x3e, 0x19, 0xe1, 0x58, 0xb5, 0xdb, 0x2b,
	    0x0a, 0x25, 0xfd, 0x90, 0x66, 0x3f, 0x09, 0x8f,
	    0xc6, 0xc2, 0x7d, 0xf6, 0x44, 0x3f, 0x8c, 0xd7,
};
static const unsigned int ecdsa_pub_key_len = sizeof(ecdsa_pub_key);

/* Structure requise par MCUBoot */
const struct bootutil_key bootutil_keys[] = {
    {
        .key = ecdsa_pub_key,
        .len = &ecdsa_pub_key_len,
    },
};

const int bootutil_key_cnt = 1;
