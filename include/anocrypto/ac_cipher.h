#ifndef ANO_CRYPTO_AC_CIPHER_H
#define ANO_CRYPTO_AC_CIPHER_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1-25 public block-cipher slice, hardened through P1-26.
 *
 * Implemented scope:
 * - AES-128, AES-192, and AES-256 through an in-memory CSP key handle.
 * - CBC without padding, using an exactly 16-byte caller-provided IV.
 * - Complete non-empty blocks only; output length equals input length.
 * - Exact in-place processing is supported. Partial overlap is rejected.
 * - No implicit allocation, IV generation, padding, key export, or persistence.
 *
 * The context is caller-owned opaque storage. Successful uninit clears the
 * expanded key schedule and chaining state. Failed reinitialization with a
 * structurally valid context buffer invalidates the earlier schedule. Callers must still protect and
 * clear their own input/output/IV buffers according to product requirements.
 */

#define ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE 16u
#define ANO_CRYPTO_CIPHER_AES128_KEY_SIZE 16u
#define ANO_CRYPTO_CIPHER_AES192_KEY_SIZE 24u
#define ANO_CRYPTO_CIPHER_AES256_KEY_SIZE 32u
#define ANO_CRYPTO_CIPHER_CBC_IV_SIZE 16u
#define ANO_CRYPTO_CIPHER_FLAG_NONE 0x00000000u

typedef enum ano_crypto_cipher_algorithm_e {
    ANO_CRYPTO_CIPHER_ALG_AES = 1
} ano_crypto_cipher_algorithm_t;

typedef enum ano_crypto_cipher_mode_e {
    ANO_CRYPTO_CIPHER_MODE_CBC = 1
} ano_crypto_cipher_mode_t;

typedef enum ano_crypto_cipher_direction_e {
    ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT = 1,
    ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT = 2
} ano_crypto_cipher_direction_t;

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cipher_context_size(
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cipher_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    ano_crypto_cipher_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *iv,
    size_t iv_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cipher_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

/* Cleanup is allowed outside OPERATIONAL to clear caller-owned context storage. */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cipher_uninit(
    void *ctx,
    size_t ctx_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cipher_crypt_with_key(
    ano_crypto_cipher_algorithm_t algorithm,
    ano_crypto_cipher_mode_t mode,
    ano_crypto_cipher_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *iv,
    size_t iv_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written,
    uint32_t flags);

ANO_CRYPTO_API const char *ano_crypto_cipher_algorithm_name(
    ano_crypto_cipher_algorithm_t algorithm);
ANO_CRYPTO_API const char *ano_crypto_cipher_mode_name(
    ano_crypto_cipher_mode_t mode);
ANO_CRYPTO_API const char *ano_crypto_cipher_direction_name(
    ano_crypto_cipher_direction_t direction);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_CIPHER_H */
