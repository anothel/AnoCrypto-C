#ifndef ANO_CRYPTO_AC_AEAD_H
#define ANO_CRYPTO_AC_AEAD_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_AES256_GCM_KEY_SIZE 32u
#define ANO_CRYPTO_AES256_GCM_NONCE_SIZE 12u
#define ANO_CRYPTO_AES256_GCM_TAG_SIZE 16u

/*
 * AES-256-GCM one-shot authenticated encryption.
 *
 * Key and format contract:
 * - key_handle must reference an in-memory BLOCK_CIPHER key of exactly 32 bytes.
 * - nonce_len must be exactly 12 and tag_len must be exactly 16.
 * - aad may be empty; aad == NULL is accepted only when aad_len == 0.
 * - empty plaintext/ciphertext is valid and produces/authenticates a tag.
 * - ciphertext length equals plaintext length.
 *
 * Buffer and failure contract:
 * - out_len is output capacity and must be at least the input length.
 * - exact in-place payload operation is supported; partial payload overlap is rejected.
 * - read-only input ranges may overlap each other.
 * - writable payload output must not overlap nonce, non-empty AAD, or the tag range.
 * - encryption tag output must not overlap any non-empty input or payload output range.
 * - decryption authenticates before writing plaintext; no unauthenticated plaintext
 *   is released, including for exact in-place operation.
 * - on every failure after a structurally valid writable range is accepted,
 *   the complete operation output range is cleared and *out_written is zero.
 * - authentication mismatch returns ANO_CRYPTO_ERR_AUTHENTICATION_FAILED.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_encrypt(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *plaintext,
    size_t plaintext_len,
    uint8_t *ciphertext,
    size_t ciphertext_len,
    size_t *out_written,
    uint8_t *tag,
    size_t tag_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_decrypt(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t *ciphertext,
    size_t ciphertext_len,
    const uint8_t *tag,
    size_t tag_len,
    uint8_t *plaintext,
    size_t plaintext_len,
    size_t *out_written);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_AEAD_H */
