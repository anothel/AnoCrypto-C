#ifndef ANO_CRYPTO_CSP_INTERNAL_H
#define ANO_CRYPTO_CSP_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_INTERNAL_CSP_MAX_SECRET_BYTES 128u
#define ANO_CRYPTO_INTERNAL_CSP_TABLE_CAPACITY 16u

ano_crypto_status_t ano_crypto_internal_csp_import_secret(
    ano_crypto_key_type_t key_type,
    const uint8_t *key_material,
    size_t key_material_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle);

ano_crypto_status_t ano_crypto_internal_csp_destroy(
    ano_crypto_csp_handle_t handle);

/*
 * Zeroize every live module-owned CSP slot while the global lock is held.
 * The generation is retained and the state becomes ZEROIZED so a caller may
 * still complete explicit destruction after orderly shutdown or fatal error.
 */
void ano_crypto_internal_csp_zeroize_all_unlocked(void);

ano_crypto_status_t ano_crypto_internal_csp_copy_secret(
    ano_crypto_csp_handle_t handle,
    ano_crypto_key_type_t expected_type,
    uint8_t *out_key_material,
    size_t out_key_material_capacity,
    size_t *out_key_material_len);

#endif /* ANO_CRYPTO_CSP_INTERNAL_H */
