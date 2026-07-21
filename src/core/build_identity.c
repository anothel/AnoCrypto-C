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
#define ANO_CRYPTO_BUILD_FINGERPRINT "AnoCrypto-C-p3-09-ecdsa-p256-sha256"
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
    out_identity->notes = "P3-09 adds ECDSA P-256 SHA-256 fixed-width key import and RFC 6979 message signing/verification on additive ANOCRYPTO_C_6 while retaining RSA on ANOCRYPTO_C_5, ABI lines ANOCRYPTO_C_0 through ANOCRYPTO_C_4, and the P1-29 compatibility baseline. PEM, DER, ASN.1, PKCS#8, SPKI, CSR, X.509, CRL, OCSP, key_ref routing, and external-provider policy remain downstream-owned. KCMVP is DEFERRED, P1-34 hosted lanes are NOT_RUN and UNREVIEWED, repository signing is UNSIGNED, external release authorization is NO, and no push, tag, signing, timestamping, publication, or external distribution is authorized.";

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
