#include "ac_kat_manifest.h"

/*
 * P1-33 retained P1-29 KAT manifest with AES-CMAC startup and extended public-source coverage.
 * P1-23/P1-29 differential tests are separate engineering comparisons and do not
 * alter public KAT provenance entries.
 * SHA-256, SHA-384, SHA-512, their HMAC variants, and the
 * HMAC_DRBG-SHA-256 core are covered by startup or extended public-source
 * tests. Generated digest, MAC, entropy, and random output are never written
 * to logs or release evidence.
 *
 * Vectors embedded in selftest.c, src/drbg/drbg.c, and tests/kat are limited
 * to public specification/test-vector inputs recorded in
 * cleanroom/SOURCE_MANIFEST.md. HMAC test keys are public fixtures only and
 * are not operational secrets.
 */
static const ano_crypto_internal_kat_manifest_entry_t g_kat_manifest[] = {
    {
        "mac.aes-cmac.aes128.nist-example2",
        "AES-128-CMAC",
        "SRC-1501: NIST AES-CMAC Example 2 associated with SP 800-38B",
        "p2-02-retained-startup"
    },
    {
        "mac.aes-cmac.aes128-192-256.nist-examples",
        "AES-CMAC",
        "SRC-1501: NIST AES-CMAC examples for AES-128, AES-192, and AES-256",
        "p2-02-retained-extended-kat-test"
    },
    {
        "cipher.aes128.fips197",
        "AES-128",
        "SRC-1401: NIST FIPS 197 AES-128 cipher example",
        "p1-26-implemented-startup"
    },
    {
        "cipher.aes192.fips197",
        "AES-192",
        "SRC-1401: NIST FIPS 197 AES-192 cipher example",
        "p1-26-implemented-startup"
    },
    {
        "cipher.aes256.fips197",
        "AES-256",
        "SRC-1401: NIST FIPS 197 AES-256 cipher example",
        "p1-26-implemented-startup"
    },
    {
        "cipher.aes128.cbc.sp800-38a",
        "AES-128-CBC without padding",
        "SRC-1402: NIST SP 800-38A CBC-AES128 example",
        "p1-26-implemented-startup-and-extended"
    },
    {
        "cipher.aes192.aes256.cbc.sp800-38a",
        "AES-192/256-CBC without padding",
        "SRC-1402: NIST SP 800-38A CBC-AES192 and CBC-AES256 examples",
        "p1-26-extended-kat-test"
    },
    {
        "hash.sha384.empty",
        "SHA-384",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-384 example digest",
        "p1-22-implemented-startup"
    },
    {
        "hash.sha384.abc",
        "SHA-384",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-384 example digest",
        "p1-22-implemented-startup"
    },
    {
        "hash.sha512.empty",
        "SHA-512",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-512 example digest",
        "p1-22-implemented-startup"
    },
    {
        "hash.sha512.abc",
        "SHA-512",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-512 example digest",
        "p1-22-implemented-startup"
    },
    {
        "hash.sha384.sha512-public-long",
        "SHA-384",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-384 multi-block example digest",
        "p1-22-extended-kat-test"
    },
    {
        "hash.sha512.sha512-public-long",
        "SHA-512",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-512 multi-block example digest",
        "p1-22-extended-kat-test"
    },
    {
        "hash.sha384.million-a",
        "SHA-384",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-384 million-character example digest",
        "p1-22-extended-kat-test"
    },
    {
        "hash.sha512.million-a",
        "SHA-512",
        "SRC-1001: NIST FIPS 180-4 Secure Hash Standard, SHA-512 million-character example digest",
        "p1-22-extended-kat-test"
    },
    {
        "hmac.sha384.rfc4231.tc1",
        "HMAC-SHA-384",
        "SRC-1002: RFC 4231 HMAC-SHA-384 Test Case 1",
        "p1-22-implemented-startup"
    },
    {
        "hmac.sha384.rfc4231.tc2",
        "HMAC-SHA-384",
        "SRC-1002: RFC 4231 HMAC-SHA-384 Test Case 2",
        "p1-22-implemented-startup"
    },
    {
        "hmac.sha512.rfc4231.tc1",
        "HMAC-SHA-512",
        "SRC-1002: RFC 4231 HMAC-SHA-512 Test Case 1",
        "p1-22-implemented-startup"
    },
    {
        "hmac.sha512.rfc4231.tc2",
        "HMAC-SHA-512",
        "SRC-1002: RFC 4231 HMAC-SHA-512 Test Case 2",
        "p1-22-implemented-startup"
    },
    {
        "hmac.sha384.rfc4231.extended",
        "HMAC-SHA-384",
        "SRC-1002: RFC 4231 HMAC-SHA-384 Test Cases 3, 4, 6, and 7",
        "p1-22-extended-kat-test"
    },
    {
        "hmac.sha512.rfc4231.extended",
        "HMAC-SHA-512",
        "SRC-1002: RFC 4231 HMAC-SHA-512 Test Cases 3, 4, 6, and 7",
        "p1-22-extended-kat-test"
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
        "SRC-1002: RFC 4231 HMAC-SHA-256 Test Case 2 through the HMAC key handle path",
        "p1-key-handle-kat-test"
    },
    {
        "hmac.sha384.handle.rfc4231.tc2",
        "HMAC-SHA-384 with CSP handle",
        "SRC-1002: RFC 4231 HMAC-SHA-384 Test Case 2 through the HMAC key handle path",
        "p1-22-key-handle-kat-test"
    },
    {
        "hmac.sha512.handle.rfc4231.tc2",
        "HMAC-SHA-512 with CSP handle",
        "SRC-1002: RFC 4231 HMAC-SHA-512 Test Case 2 through the HMAC key handle path",
        "p1-22-key-handle-kat-test"
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
