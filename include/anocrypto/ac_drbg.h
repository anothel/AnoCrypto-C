#ifndef ANO_CRYPTO_AC_DRBG_H
#define ANO_CRYPTO_AC_DRBG_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1 public API for the DRBG core state machine.
 *
 * P1-3 scope:
 * - HMAC_DRBG-SHA-256 core processing is implemented with explicit caller-
 *   supplied entropy input for deterministic KAT and adapter-driven use.
 * - OS entropy collection is intentionally not implemented here. A later
 *   entropy adapter/random-service layer must provide entropy input and record
 *   its own evidence.
 * - Hash_DRBG-SHA-256 remains ABI-reserved and returns
 *   ANO_CRYPTO_ERR_NOT_IMPLEMENTED from processing APIs.
 * - Entropy, nonce, seed material, and generated secrets must never be logged.
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
/* Reserved until an entropy adapter can satisfy per-request reseed input. */
#define ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED 0x00000001u

#define ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE 32u
#define ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE 16u
#define ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE 65536u
#define ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE 32u

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_context_size(
    ano_crypto_drbg_algorithm_t algorithm,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_instantiate(
    void *ctx,
    size_t ctx_len,
    ano_crypto_drbg_algorithm_t algorithm,
    const uint8_t *entropy_input,
    size_t entropy_input_len,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_reseed(
    void *ctx,
    const uint8_t *entropy_input,
    size_t entropy_input_len,
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

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_drbg_get_state(
    const void *ctx,
    ano_crypto_drbg_state_t *out_state);

ANO_CRYPTO_API const char *ano_crypto_drbg_algorithm_name(
    ano_crypto_drbg_algorithm_t algorithm);

ANO_CRYPTO_API const char *ano_crypto_drbg_state_name(
    ano_crypto_drbg_state_t state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_DRBG_H */
