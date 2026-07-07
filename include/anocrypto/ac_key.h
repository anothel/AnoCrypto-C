#ifndef ANO_CRYPTO_AC_KEY_H
#define ANO_CRYPTO_AC_KEY_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Key/CSP lifecycle skeleton.
 *
 * Raw key material is accepted only as caller-provided input to future import
 * operations. P1-5 implements an in-memory HMAC key handle path only. It does
 * not export, print, persist, or test with operational secret material.
 */
typedef enum ano_crypto_key_type_e {
    ANO_CRYPTO_KEY_TYPE_SECRET_BYTES = 1,
    ANO_CRYPTO_KEY_TYPE_HMAC = 2,
    ANO_CRYPTO_KEY_TYPE_DRBG_SEED_MATERIAL = 3,
    ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER = 4,
    ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY = 5,
    ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY = 6
} ano_crypto_key_type_t;

#define ANO_CRYPTO_KEY_FLAG_NONE 0x00000000u
#define ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE 0x00000001u
#define ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE 0x00000002u

#define ANO_CRYPTO_KEY_MAX_SECRET_BYTES 128u

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_import_secret(
    ano_crypto_key_type_t key_type,
    const uint8_t *key_material,
    size_t key_material_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_destroy(
    ano_crypto_csp_handle_t handle);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_key_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state);

ANO_CRYPTO_API const char *ano_crypto_key_type_name(
    ano_crypto_key_type_t key_type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_KEY_H */
