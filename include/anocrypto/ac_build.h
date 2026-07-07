#ifndef ANO_CRYPTO_AC_BUILD_H
#define ANO_CRYPTO_AC_BUILD_H

#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION 1u

typedef struct ano_crypto_build_identity_s {
    uint32_t abi_version;
    const char *product_name;
    const char *version;
    const char *git_commit;
    const char *build_type;
    const char *compiler;
    const char *target;
    const char *notes;
} ano_crypto_build_identity_t;

/*
 * Build identity query APIs are allowed in all module states, including error state.
 *
 * TODO(P1): Decide whether to replace this public struct with field-by-index getters
 * for stricter ABI evolution. The abi_version field is included as a P0 guardrail.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_build_identity(
    ano_crypto_build_identity_t *out_identity);

ANO_CRYPTO_API ano_crypto_status_t ano_crypto_build_fingerprint(
    const char **out_fingerprint);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_BUILD_H */
