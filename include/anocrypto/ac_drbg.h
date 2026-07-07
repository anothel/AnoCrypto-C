#ifndef ANO_CRYPTO_AC_DRBG_H
#define ANO_CRYPTO_AC_DRBG_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API skeleton for a DRBG state machine.
 *
 * The entropy source adapter and DRBG core are intentionally separate TODOs.
 * This header fixes instantiate/reseed/generate/uninstantiate API shape without
 * importing test vectors or algorithm implementation code.
 */
typedef enum ano_crypto_drbg_algorithm_e {
    ANO_CRYPTO_DRBG_ALG_HMAC_SHA256 = 1,
    ANO_CRYPTO_DRBG_ALG_HASH_SHA256 = 2
} ano_crypto_drbg_algorithm_t;

typedef enum ano_crypto_drbg_state_e {
    ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED = 0,
    ANO_CRYPTO_DRBG_STATE_INSTANTIATED = 1,
    ANO_CRYPTO_DRBG_STATE_ERROR = 2
} ano_crypto_drbg_state_t;

#define ANO_CRYPTO_DRBG_FLAG_NONE 0x00000000u
#define ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED 0x00000001u

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_context_size(
    ano_crypto_drbg_algorithm_t algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_instantiate(
    void *ctx,
    size_t ctx_len,
    ano_crypto_drbg_algorithm_t algorithm,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_reseed(
    void *ctx,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_generate(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

/*
 * Cleanup is allowed even if the module is no longer operational, so callers can
 * clear caller-provided DRBG storage during error handling.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_uninstantiate(
    void *ctx,
    size_t ctx_len);

ANO_CRYPTO_API const char *ano_crypto_drbg_algorithm_name(
    ano_crypto_drbg_algorithm_t algorithm);

ANO_CRYPTO_API const char *ano_crypto_drbg_state_name(
    ano_crypto_drbg_state_t state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_DRBG_H */
