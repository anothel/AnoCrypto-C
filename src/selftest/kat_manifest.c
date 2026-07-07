#include "ac_kat_manifest.h"

/*
 * P1-8 KAT manifest.
 *
 * Vectors embedded in selftest.c, src/drbg/drbg.c, and tests/kat are limited to public
 * specification/test-vector inputs recorded in cleanroom/SOURCE_MANIFEST.md.
 * The HMAC test keys are public test-vector materials only and are not
 * operational secrets.
 */
static const ano_crypto_internal_kat_manifest_entry_t g_kat_manifest[] = {
    {
        "hash.sha256.empty",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 example digest",
        "p1-implemented-startup"
    },
    {
        "hash.sha256.abc",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 example digest",
        "p1-implemented-startup"
    },
    {
        "hash.sha256.abcdbc-public-long",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 multi-block example digest",
        "p1-implemented-startup"
    },
    {
        "hash.sha256.million-a",
        "SHA-256",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-256 million-character example digest",
        "p1-extended-kat-test"
    },
    {
        "hash.sha256.chunk-boundary-consistency",
        "SHA-256",
        "DER-2001: deterministic local API consistency test; not a public KAT source",
        "p1-extended-api-test"
    },
    {
        "hmac.sha256.rfc4231.tc1",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 1",
        "p1-implemented-startup"
    },
    {
        "hmac.sha256.rfc4231.tc2",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 2",
        "p1-implemented-startup"
    },
    {
        "hmac.sha256.rfc4231.tc3",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 3",
        "p1-extended-kat-test"
    },
    {
        "hmac.sha256.rfc4231.tc4",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 4",
        "p1-extended-kat-test"
    },
    {
        "hmac.sha256.rfc4231.tc6-long-key",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 6",
        "p1-implemented-startup"
    },
    {
        "hmac.sha256.rfc4231.tc7-long-key-long-data",
        "HMAC-SHA-256",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 7",
        "p1-extended-kat-test"
    },
    {
        "hmac.sha256.chunk-boundary-consistency",
        "HMAC-SHA-256",
        "DER-2002: deterministic local API consistency test; not a public KAT source",
        "p1-extended-api-test"
    },
    {
        "drbg.hmac-sha256.nist-cavp-no-pr-reseed-count0",
        "HMAC_DRBG-SHA-256",
        "SRC-1202: NIST CAVP DRBG test vector package, HMAC_DRBG-SHA-256 no prediction resistance with reseed COUNT=0",
        "p1-implemented-startup"
    },
    {
        "hmac.sha256.handle.rfc4231.tc2",
        "HMAC-SHA-256 with CSP handle",
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 2 through P1 HMAC key handle path",
        "p1-key-handle-kat-test"
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
