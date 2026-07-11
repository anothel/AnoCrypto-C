#ifndef ANO_CRYPTO_AC_HMAC_H
#define ANO_CRYPTO_AC_HMAC_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API for HMAC services.
 *
 * P1-22 scope:
 * - HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 one-shot and streaming
 *   processing are implemented and covered by public RFC 4231 KATs.
 * - The CSP HMAC-key handle compute path accepts all three SHA-2 variants.
 * - Key material and computed MAC bytes must never be logged by callers or by
 *   this library.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_context_size(
    ano_crypto_hash_algorithm_t hash_algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_mac_size(
    ano_crypto_hash_algorithm_t hash_algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_compute(
    ano_crypto_hash_algorithm_t hash_algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

/*
 * This API snapshots an in-memory HMAC key handle under the internal CSP lock,
 * computes with the local copy, and zeroizes the copy before returning.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_hmac_compute_with_key(
    ano_crypto_hash_algorithm_t hash_algorithm,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_HMAC_H */
