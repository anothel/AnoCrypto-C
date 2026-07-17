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
#define ANO_CRYPTO_BUILD_FINGERPRINT "AnoCrypto-C-p2-05-csp-zeroization"
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
    out_identity->notes = "P2-05 CSP ownership and zeroization evidence closure; implemented services remain SHA-256/384/512, matching HMAC variants, HMAC_DRBG-SHA-256, the source-backed random lifecycle, CSP-backed HMAC, AES encryption and dedicated CMAC keys, AES-128/192/256 CBC without padding, and full-tag AES-CMAC. P2-05 retains AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1, P2-03 self-test/state design, EDP-P2-04-R1, the P1-29 public headers, reviewed 69-symbol ABI-0 surface, P1-33 clean synthetic tagged-release rehearsal, and fail-closed hosted-artifact review model. CZP-P2-05-R1 inventories module-owned and caller-owned sensitive state and wipes live module-owned HMAC/AES/CMAC key slots on explicit destroy, orderly shutdown, and locked fatal transition. Caller-owned opaque contexts remain caller-managed through documented final/uninit operations. Exact OE-01 zeroization compiler review and formal CSP classification remain NOT_RUN or EXTERNAL_CONFIRMATION_REQUIRED. The selected integrity candidate INT-HMAC-SHA256-SINGLE-SO-R1 remains NOT IMPLEMENTED. P1-34 actual hosted review remains BLOCKED_EXTERNAL with hosted lanes NOT_RUN and review UNREVIEWED. Target-specific side-channel review, production signing/timestamping, and external release authorization remain TODO or EXTERNAL_CONFIRMATION_REQUIRED";

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
