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

#define ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE 0x00000000u

typedef enum ano_crypto_aead_direction_e {
    ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT = 1,
    ANO_CRYPTO_AEAD_DIRECTION_DECRYPT = 2
} ano_crypto_aead_direction_t;

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

/*
 * AES-256-GCM streaming context API.
 *
 * Lifecycle and state contract:
 * - context storage is caller-owned, opaque, and sized with context_size().
 * - storage passed as ctx must be obtained from malloc/calloc/realloc or have
 *   equivalent alignment suitable for any C object; raw byte-array storage is
 *   not portable unless the caller separately guarantees that alignment.
 * - init snapshots one live 32-byte BLOCK_CIPHER key and binds one 12-byte
 *   nonce to either an encrypt or decrypt context; later handle destruction
 *   prevents new init but does not revoke the already initialized context.
 * - the same nonce/key pair must not be reused.
 * - AAD updates are accepted only before the first non-empty payload update.
 * - zero-length AAD and payload updates are valid no-ops and do not advance the
 *   AAD-to-payload phase.
 * - payload updates return exactly input_len bytes and support exact in-place
 *   operation; partial overlap is rejected.
 * - decrypt update output is explicitly UNVERIFIED until decrypt_final succeeds.
 *   Callers must buffer, withhold, and wipe it on any finalization failure.
 * - a correct-direction finalization attempt consumes and clears the context,
 *   whether it succeeds or fails. Repeated update/final calls are rejected.
 * - uninit is allowed outside OPERATIONAL and clears the reviewed opaque extent.
 *
 * Buffer contract:
 * - NULL is accepted only for zero-length AAD or payload input/output.
 * - context storage must not overlap nonce, AAD, payload, tag, or out_written.
 * - out_written must not overlap payload input or output.
 * - exact payload input/output alias is supported; all partial overlap is rejected.
 * - after structurally valid, disjoint output and out_written ranges are
 *   accepted, failed updates clear the caller-authorized payload output extent
 *   and set *out_written to zero; no product framing or rollback policy is
 *   implemented.
 * - contexts are not safe for concurrent access without caller synchronization.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_context_size(
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_aead_direction_t direction,
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *nonce,
    size_t nonce_len,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_aad_update(
    void *ctx,
    const uint8_t *aad,
    size_t aad_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_encrypt_final(
    void *ctx,
    uint8_t *tag,
    size_t tag_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_decrypt_final(
    void *ctx,
    const uint8_t *tag,
    size_t tag_len);

/* Cleanup is allowed outside OPERATIONAL to clear caller-owned context storage. */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_aes256_gcm_stream_uninit(
    void *ctx,
    size_t ctx_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_AEAD_H */
