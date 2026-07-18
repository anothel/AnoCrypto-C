#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static int hex_value(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

static void expect_hex(const char *name, const uint8_t *got, size_t got_len, const char *hex)
{
    size_t i;
    size_t hex_len = strlen(hex);
    if (hex_len != got_len * 2u) {
        fprintf(stderr, "FAIL %s: expected hex length mismatch\n", name);
        ++g_failures;
        return;
    }
    for (i = 0u; i < got_len; ++i) {
        int high = hex_value(hex[i * 2u]);
        int low = hex_value(hex[i * 2u + 1u]);
        uint8_t expected = (uint8_t)((high << 4) | low);
        if (high < 0 || low < 0 || got[i] != expected) {
            fprintf(stderr, "FAIL %s: mismatch at byte %lu\n", name, (unsigned long)i);
            ++g_failures;
            return;
        }
    }
}

static void test_rfc5869_cases(void)
{
    uint8_t out[82];
    uint8_t ikm1[22];
    uint8_t salt1[13];
    uint8_t info1[10];
    uint8_t ikm2[80];
    uint8_t salt2[80];
    uint8_t info2[80];
    size_t i;

    memset(ikm1, 0x0b, sizeof(ikm1));
    for (i = 0u; i < sizeof(salt1); ++i) salt1[i] = (uint8_t)i;
    for (i = 0u; i < sizeof(info1); ++i) info1[i] = (uint8_t)(0xf0u + (uint8_t)i);
    expect_status("RFC 5869 case 1",
                  ano_crypto_hkdf_sha256(ikm1, sizeof(ikm1), salt1, sizeof(salt1),
                                          info1, sizeof(info1), out, 42u),
                  ANO_CRYPTO_OK);
    expect_hex("RFC 5869 case 1", out, 42u,
               "3cb25f25faacd57a90434f64d0362f2a"
               "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
               "34007208d5b887185865");

    for (i = 0u; i < sizeof(ikm2); ++i) {
        ikm2[i] = (uint8_t)i;
        salt2[i] = (uint8_t)(0x60u + (uint8_t)i);
        info2[i] = (uint8_t)(0xb0u + (uint8_t)i);
    }
    expect_status("RFC 5869 case 2",
                  ano_crypto_hkdf_sha256(ikm2, sizeof(ikm2), salt2, sizeof(salt2),
                                          info2, sizeof(info2), out, sizeof(out)),
                  ANO_CRYPTO_OK);
    expect_hex("RFC 5869 case 2", out, sizeof(out),
               "b11e398dc80327a1c8e7f78c596a49344"
               "f012eda2d4efad8a050cc4c19afa97c59"
               "045a99cac7827271cb41c65e590e09da3"
               "275600c2f09b8367793a9aca3db71cc3"
               "0c58179ec3e87c14c01d5c1f3434f1d87");

    expect_status("RFC 5869 case 3 empty salt/info",
                  ano_crypto_hkdf_sha256(ikm1, sizeof(ikm1), 0, 0u, 0, 0u, out, 42u),
                  ANO_CRYPTO_OK);
    expect_hex("RFC 5869 case 3", out, 42u,
               "8da4e775a563c18f715f802a063c5a31"
               "b8a11f5c5ee1879ec3454e5f3c738d2d"
               "9d201395faa4b61a96c8");
}

static void test_empty_and_short_outputs(void)
{
    static const uint8_t salt[] = { 's', 'a', 'l', 't' };
    static const uint8_t info[] = { 'i', 'n', 'f', 'o' };
    uint8_t out[32];

    expect_status("zero-length output",
                  ano_crypto_hkdf_sha256(0, 0u, 0, 0u, 0, 0u, 0, 0u),
                  ANO_CRYPTO_OK);
    expect_status("empty IKM",
                  ano_crypto_hkdf_sha256(0, 0u, salt, sizeof(salt), info, sizeof(info), out, sizeof(out)),
                  ANO_CRYPTO_OK);
    expect_hex("empty IKM", out, sizeof(out),
               "7aac7b8120501c2c8e1ee50e6cde1353"
               "61e99ceb9d8d406ac528b9e9175614c0");

    expect_status("one-byte output",
                  ano_crypto_hkdf_sha256(0, 0u, salt, sizeof(salt), info, sizeof(info), out, 1u),
                  ANO_CRYPTO_OK);
    expect_hex("one-byte output", out, 1u, "7a");
}

static void test_maximum_output(void)
{
    static uint8_t out[ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE];
    uint8_t ikm[32];
    uint8_t salt[32];
    static const uint8_t info[] = "AnoCrypto-C HKDF boundary 8160";
    size_t i;

    for (i = 0u; i < sizeof(ikm); ++i) {
        ikm[i] = (uint8_t)i;
        salt[i] = (uint8_t)(32u + i);
    }
    expect_status("8160-byte output",
                  ano_crypto_hkdf_sha256(ikm, sizeof(ikm), salt, sizeof(salt),
                                          info, sizeof(info) - 1u, out, sizeof(out)),
                  ANO_CRYPTO_OK);
    expect_hex("8160 first block", out, 32u,
               "f5d360275b877f810743589cec3ab819"
               "edbed4d04f691dbe32403f382864e705");
    expect_hex("8160 last block", out + sizeof(out) - 32u, 32u,
               "434f892612a4e161a7b9b699c09554d8"
               "09cf2777ec388bef9103620d054725e4");
}

static void test_anosecurekit_skf1_info_bytes(void)
{
    uint8_t ikm[32] = { 0u };
    uint8_t salt[32];
    uint8_t out[32];
    static const uint8_t info[] = {
        0x61u,0x6eu,0x6fu,0x73u,0x65u,0x63u,0x75u,0x72u,
        0x65u,0x6bu,0x69u,0x74u,0x20u,0x66u,0x69u,0x6cu,
        0x65u,0x20u,0x73u,0x65u,0x61u,0x6cu,0x69u,0x6eu,
        0x67u,0x20u,0x76u,0x31u
    };
    size_t i;

    for (i = 0u; i < sizeof(salt); ++i) salt[i] = (uint8_t)i;
    expect_status("AnoSecureKit SKF1 info bytes",
                  ano_crypto_hkdf_sha256(ikm, sizeof(ikm), salt, sizeof(salt),
                                          info, sizeof(info), out, sizeof(out)),
                  ANO_CRYPTO_OK);
    expect_hex("AnoSecureKit SKF1 file key", out, sizeof(out),
               "b2b5cce70684ba7d441b1525b91a44e6"
               "6784aa349ebc4da767511993d663c26b");
}

int main(void)
{
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_rfc5869_cases();
    test_empty_and_short_outputs();
    test_maximum_output();
    test_anosecurekit_skf1_info_bytes();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hkdf_sha256_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }
    puts("hkdf_sha256_kat_tests: PASS");
    return 0;
}
