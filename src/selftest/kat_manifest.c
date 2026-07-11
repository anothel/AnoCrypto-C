#include "ac_kat_manifest.h"

/*
 * P1-20 KAT manifest.
 * The public random service reuses the HMAC_DRBG-SHA-256 core covered by the
 * startup KAT below. Generated random output is not a KAT fixture and is never
 * written to logs or evidence.
 *
 * Vectors embedded in selftest.c, src/drbg/drbg.c, and tests/kat are limited to public
 * specification/test-vector inputs recorded in cleanroom/SOURCE_MANIFEST.md.
 * The HMAC test keys are public test-vector materials only and are not
 * operational secrets.
 */
static const ano_crypto_internal_kat_manifest_entry_t g_kat_manifest[] = {
    {
        "hash.sha384.reserved-empty",
        "SHA-384",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard; P1-16 reserves API metadata only",
        "p1-16-reserved-kat-plan"
    },
    {
        "hash.sha512.reserved-empty",
        "SHA-512",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard; P1-16 reserves API metadata only",
        "p1-16-reserved-kat-plan"
    },
    {
        "hmac.sha384.reserved-rfc4231",
        "HMAC-SHA-384",
        "SRC-1002: RFC 4231 HMAC-SHA-384 test vectors; P1-16 reserves API metadata only",
        "p1-16-reserved-kat-plan"
    },
    {
        "hmac.sha512.reserved-rfc4231",
        "HMAC-SHA-512",
        "SRC-1002: RFC 4231 HMAC-SHA-512 test vectors; P1-16 reserves API metadata only",
        "p1-16-reserved-kat-plan"
    },
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
        "drbg.hmac-sha256.personalization-regression",
        "HMAC_DRBG-SHA-256",
        "DER-9001: locally generated deterministic HMAC_DRBG-SHA-256 personalization fixture derived from public SP 800-90A Rev.1 algorithm",
        "p1-9-boundary-test"
    },
    {
        "drbg.hmac-sha256.additional-input-regression",
        "HMAC_DRBG-SHA-256",
        "DER-9001: locally generated deterministic HMAC_DRBG-SHA-256 additional-input fixture derived from public SP 800-90A Rev.1 algorithm",
        "p1-9-boundary-test"
    },
    {
        "drbg.hmac-sha256.reseed-after-generate-regression",
        "HMAC_DRBG-SHA-256",
        "DER-9001: locally generated deterministic HMAC_DRBG-SHA-256 reseed-after-generate fixture derived from public SP 800-90A Rev.1 algorithm",
        "p1-9-boundary-test"
    },
    {
        "drbg.hmac-sha256.request-size-boundary",
        "HMAC_DRBG-SHA-256",
        "DER-9002: local request-size boundary test for maximum generate request and oversized-request rejection",
        "p1-9-boundary-test"
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
