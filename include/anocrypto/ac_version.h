#ifndef ANO_CRYPTO_AC_VERSION_H
#define ANO_CRYPTO_AC_VERSION_H

#include <stdint.h>
#include <anocrypto/ac_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANO_CRYPTO_VERSION_MAJOR 0u
#define ANO_CRYPTO_VERSION_MINOR 1u
#define ANO_CRYPTO_VERSION_PATCH 0u
#define ANO_CRYPTO_VERSION_PRERELEASE "p1-32-signing-handoff"
#define ANO_CRYPTO_VERSION_STRING "0.1.0-p1-32-signing-handoff"

/*
 * Version query APIs are allowed in all module states, including error state.
 */
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_version_string(const char **out_version);
ANO_CRYPTO_API ano_crypto_status_t ano_crypto_version_components(
    uint32_t *out_major,
    uint32_t *out_minor,
    uint32_t *out_patch);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_VERSION_H */
