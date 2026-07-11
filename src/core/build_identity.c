#include <anocrypto/ac_build.h>
#include <anocrypto/ac_version.h>

#ifndef ANO_CRYPTO_BUILD_GIT_COMMIT
#define ANO_CRYPTO_BUILD_GIT_COMMIT "unknown"
#endif

#ifndef ANO_CRYPTO_BUILD_TYPE
#define ANO_CRYPTO_BUILD_TYPE "unknown"
#endif

#ifndef ANO_CRYPTO_BUILD_COMPILER
#define ANO_CRYPTO_BUILD_COMPILER "unknown"
#endif

#ifndef ANO_CRYPTO_BUILD_TARGET
#define ANO_CRYPTO_BUILD_TARGET "unknown"
#endif

#ifndef ANO_CRYPTO_BUILD_FINGERPRINT
#define ANO_CRYPTO_BUILD_FINGERPRINT "AnoCrypto-C-p1-25-aes-cbc"
#endif

ano_crypto_status_t ano_crypto_version_string(const char **out_version)
{
    if (out_version == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_version = ANO_CRYPTO_VERSION_STRING;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_version_components(
    uint32_t *out_major,
    uint32_t *out_minor,
    uint32_t *out_patch)
{
    if (out_major == 0 || out_minor == 0 || out_patch == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_major = ANO_CRYPTO_VERSION_MAJOR;
    *out_minor = ANO_CRYPTO_VERSION_MINOR;
    *out_patch = ANO_CRYPTO_VERSION_PATCH;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_build_identity(
    ano_crypto_build_identity_t *out_identity)
{
    if (out_identity == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    out_identity->abi_version = ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION;
    out_identity->product_name = "AnoCrypto-C";
    out_identity->version = ANO_CRYPTO_VERSION_STRING;
    out_identity->git_commit = ANO_CRYPTO_BUILD_GIT_COMMIT;
    out_identity->build_type = ANO_CRYPTO_BUILD_TYPE;
    out_identity->compiler = ANO_CRYPTO_BUILD_COMPILER;
    out_identity->target = ANO_CRYPTO_BUILD_TARGET;
    out_identity->notes = "P1-25 C99 engineering slice; implemented services include SHA-256/384/512, matching HMAC variants, HMAC_DRBG-SHA-256, the source-backed random lifecycle, CSP-backed HMAC and AES keys, AES-128/192/256, and CBC without padding. Startup/extended KATs, negative tests, OpenSSL 3.x test-only differential lanes, portability regressions, concurrency coverage, Linux GCC address-and-undefined sanitizer coverage, build-identity ABI-0 policy, and clean Release exported-symbol checks are included. Prediction resistance, automatic VM snapshot/restore or suspend/resume detection, direct noise-source selection and final health-test thresholds, reviewed hosted Windows/macOS and 32-bit artifacts, big-endian execution evidence, ThreadSanitizer policy, Hash_DRBG, CTR, AEAD, padding helpers, CMAC/GMAC, domestic algorithms, hardware dispatch, and final side-channel/compiler-output review remain TODO";

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_build_fingerprint(
    const char **out_fingerprint)
{
    if (out_fingerprint == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_fingerprint = ANO_CRYPTO_BUILD_FINGERPRINT;
    return ANO_CRYPTO_OK;
}
