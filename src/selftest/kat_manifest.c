#include "ac_kat_manifest.h"

/*
 * P1-1 KAT manifest.
 *
 * Vectors embedded in selftest.c and tests/kat/sha256_hmac_kat_tests.c are
 * limited to public specification/test-vector inputs recorded in
 * cleanroom/SOURCE_MANIFEST.md. The HMAC test keys are public test-vector
 * materials only and are not operational secrets.
 */
static const ano_crypto_internal_kat_manifest_entry_t g_kat_manifest[] = {
    {
        "hash.sha256.empty",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 example digest",
        "p1-implemented"
    },
    {
        "hash.sha256.abc",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 example digest",
        "p1-implemented"
    },
    {
        "hmac.sha256.rfc4231.tc1",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 1",
        "p1-implemented"
    },
    {
        "hmac.sha256.rfc4231.tc2",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 2",
        "p1-implemented"
    },
    {
        "drbg.hmac-sha256.placeholder",
        "HMAC_DRBG-SHA-256",
        "TODO(P1): public-source DRBG KAT manifest entry required before implementation",
        "placeholder"
    }
};

size_t ano_crypto_internal_kat_manifest_count(void)
{
    return sizeof(g_kat_manifest) / sizeof(g_kat_manifest[0]);
}

const ano_crypto_internal_kat_manifest_entry_t *ano_crypto_internal_kat_manifest_get(
    size_t index)
{
    if (index >= ano_crypto_internal_kat_manifest_count()) {
        return 0;
    }

    return &g_kat_manifest[index];
}
