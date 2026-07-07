#ifndef ANO_CRYPTO_AC_CSP_H
#define ANO_CRYPTO_AC_CSP_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CSP handle skeleton.
 *
 * The handle is intentionally opaque to callers. Value 0 is always invalid.
 * Storage, ownership, export policy, and persistence are TODO(P1/P2).
 */
typedef uint64_t ano_crypto_csp_handle_t;

#define ANO_CRYPTO_CSP_INVALID_HANDLE ((ano_crypto_csp_handle_t)0u)

typedef enum ano_crypto_csp_state_e {
    ANO_CRYPTO_CSP_STATE_INVALID = 0,
    ANO_CRYPTO_CSP_STATE_ALLOCATED = 1,
    ANO_CRYPTO_CSP_STATE_INITIALIZED = 2,
    ANO_CRYPTO_CSP_STATE_ACTIVE = 3,
    ANO_CRYPTO_CSP_STATE_RESTRICTED = 4,
    ANO_CRYPTO_CSP_STATE_ZEROIZED = 5,
    ANO_CRYPTO_CSP_STATE_DESTROYED = 6
} ano_crypto_csp_state_t;

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_csp_handle_is_valid(
    ano_crypto_csp_handle_t handle,
    int *out_is_valid);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_csp_handle_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state);

ANO_CRYPTO_API const char *ano_crypto_csp_state_name(
    ano_crypto_csp_state_t state);

/*
 * Best-effort explicit memory clearing helper for caller-owned buffers.
 * TODO(P1): Review per compiler/platform and add build evidence.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_csp_zeroize(
    void *buffer,
    size_t buffer_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_CSP_H */
