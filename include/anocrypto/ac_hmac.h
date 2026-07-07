#ifndef ANO_CRYPTO_AC_HMAC_H
#define ANO_CRYPTO_AC_HMAC_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API skeleton for HMAC services.
 *
 * Key material must never be logged by callers or by this library. Processing
 * functions are state-gated and return ANO_CRYPTO_ERR_NOT_IMPLEMENTED until a
 * reviewed implementation and public KAT evidence are added.
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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_HMAC_H */
