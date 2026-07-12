#ifndef ANO_CRYPTO_AC_CMAC_H
#define ANO_CRYPTO_AC_CMAC_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * P1-29 AES-CMAC hardened public slice.
 *
 * Implemented scope:
 * - AES-128, AES-192, and AES-256 through a dedicated CMAC-purpose CSP key.
 * - Full 16-byte tags only; tag truncation is not accepted.
 * - Empty messages and arbitrary streaming chunk lengths are supported.
 * - Context storage is caller-owned and opaque.
 * - Successful finalization clears the secret-dependent context state.
 * - Message input, output tag, output-length storage, and context storage must
 *   not overlap. For verification, message input and the supplied tag must also
 *   be disjoint. Rejected preflight calls do not consume a valid context.
 * - A nonzero short uninit length clears the caller-authorized prefix and
 *   returns BUFFER_TOO_SMALL. A zero length cannot safely modify storage. In
 *   either case callers must retry with the queried size to clear the complete
 *   opaque extent; no short-length cleanup is considered complete.
 * - With a sufficient length, uninit clears only the reviewed opaque extent,
 *   even when a larger caller buffer is supplied.
 * - No key export, persistence, protocol formatting, or implicit allocation.
 */

#define ANO_CRYPTO_CMAC_TAG_SIZE 16u
#define ANO_CRYPTO_CMAC_FLAG_NONE 0x00000000u

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_context_size(
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_mac_size(
    size_t *out_size);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_init(
    void *ctx,
    size_t ctx_len,
    ano_crypto_csp_handle_t key_handle,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_update(
    void *ctx,
    const uint8_t *input,
    size_t input_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_final(
    void *ctx,
    uint8_t *out,
    size_t out_len,
    size_t *out_written);

/* Cleanup is allowed outside OPERATIONAL to clear caller-owned context storage. */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_uninit(
    void *ctx,
    size_t ctx_len);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_compute_with_key(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    uint8_t *out,
    size_t out_len,
    size_t *out_written,
    uint32_t flags);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_cmac_verify_with_key(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *input,
    size_t input_len,
    const uint8_t *tag,
    size_t tag_len,
    uint32_t flags);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_CMAC_H */
