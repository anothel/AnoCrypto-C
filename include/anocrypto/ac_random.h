#ifndef ANO_CRYPTO_AC_RANDOM_H
#define ANO_CRYPTO_AC_RANDOM_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1-16 public random-service boundary.
 *
 * The random-service ABI is reserved for review. P1-16 intentionally does not
 * expose OS-entropy-backed random byte generation as an implemented customer
 * service. The implemented entropy adapter and platform provider remain
 * internal seams feeding explicit DRBG tests and future design work.
 *
 * Reserved functions validate their arguments, enforce the module service gate
 * where appropriate, zeroize caller output on closed-fail paths, and return
 * ANO_CRYPTO_ERR_NOT_IMPLEMENTED until API/ABI, entropy policy, reseed policy,
 * fork/process behavior, and evidence requirements are reviewed.
 */
#define ANO_CRYPTO_RANDOM_FLAG_NONE 0x00000000u
#define ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED 0x00000001u

#define ANO_CRYPTO_RANDOM_SERVICE_RESERVED 0u

ANO_CRYPTO_API int ano_crypto_random_is_available(void);

ANO_CRYPTO_API const char *ano_crypto_random_service_name(void);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_random_context_size(
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_random_init(
    void *ctx,
    size_t ctx_len,
    const uint8_t *personalization,
    size_t personalization_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_random_generate(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_random_reseed(
    void *ctx,
    const uint8_t *additional_input,
    size_t additional_input_len,
    uint32_t flags);

/* Cleanup is allowed so callers can clear opaque storage on error paths. */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_random_uninit(
    void *ctx,
    size_t ctx_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_RANDOM_H */
