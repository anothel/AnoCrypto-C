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

static void expect_zero(const char *name, const uint8_t *data, size_t len)
{
    size_t i;
    for (i = 0u; i < len; ++i) {
        if (data[i] != 0u) {
            fprintf(stderr, "FAIL %s: byte %lu not cleared\n", name, (unsigned long)i);
            ++g_failures;
            return;
        }
    }
}

int main(void)
{
    static uint8_t oversized[ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE + 1u];
    uint8_t input[64];
    uint8_t out[64];
    const uint8_t *wrapping_input = (const uint8_t *)(uintptr_t)(UINTPTR_MAX - 1u);
    uint8_t *wrapping_output = (uint8_t *)(uintptr_t)(UINTPTR_MAX - 1u);

    memset(input, 0x11, sizeof(input));
    memset(out, 0xa5, sizeof(out));
    expect_status("before init",
                  ano_crypto_hkdf_sha256(input, 16u, input + 16u, 16u, input + 32u, 16u, out, 32u),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_zero("before init output", out, 32u);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("NULL output nonzero", ano_crypto_hkdf_sha256(0, 0u, 0, 0u, 0, 0u, 0, 1u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    memset(out, 0xa5, sizeof(out));
    expect_status("NULL IKM nonzero", ano_crypto_hkdf_sha256(0, 1u, 0, 0u, 0, 0u, out, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("NULL IKM clears", out, 32u);

    memset(out, 0xa5, sizeof(out));
    expect_status("NULL salt nonzero", ano_crypto_hkdf_sha256(input, 1u, 0, 1u, 0, 0u, out, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("NULL salt clears", out, 32u);

    memset(out, 0xa5, sizeof(out));
    expect_status("NULL info nonzero", ano_crypto_hkdf_sha256(input, 1u, 0, 0u, 0, 1u, out, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("NULL info clears", out, 32u);

    memset(oversized, 0xa5, sizeof(oversized));
    expect_status("8161 rejected", ano_crypto_hkdf_sha256(input, 1u, 0, 0u, 0, 0u, oversized, sizeof(oversized)),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_zero("8161 clears", oversized, sizeof(oversized));

    memset(out, 0xa5, sizeof(out));
    expect_status("wrapping IKM range",
                  ano_crypto_hkdf_sha256(wrapping_input, 4u, 0, 0u, 0, 0u, out, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("wrapping IKM clears", out, 32u);

    expect_status("wrapping output range",
                  ano_crypto_hkdf_sha256(0, 0u, 0, 0u, 0, 0u, wrapping_output, 4u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    memset(input, 0x22, sizeof(input));
    expect_status("exact output/input alias",
                  ano_crypto_hkdf_sha256(input, 32u, 0, 0u, 0, 0u, input, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("exact alias clears", input, 32u);

    memset(input, 0x33, sizeof(input));
    expect_status("partial output/info overlap",
                  ano_crypto_hkdf_sha256(0, 0u, 0, 0u, input, 16u, input + 4u, 32u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("partial overlap clears", input + 4u, 32u);

    memset(input, 0x44, sizeof(input));
    memset(out, 0xa5, sizeof(out));
    expect_status("read-only input alias allowed",
                  ano_crypto_hkdf_sha256(input, 16u, input, 16u, input, 16u, out, 32u),
                  ANO_CRYPTO_OK);

    expect_status("zero output NULLs", ano_crypto_hkdf_sha256(0, 0u, 0, 0u, 0, 0u, 0, 0u),
                  ANO_CRYPTO_OK);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    memset(out, 0xa5, sizeof(out));
    expect_status("after shutdown",
                  ano_crypto_hkdf_sha256(input, 1u, 0, 0u, 0, 0u, out, 32u),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_zero("after shutdown clears", out, 32u);

    if (g_failures != 0) {
        fprintf(stderr, "hkdf_sha256_api_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }
    puts("hkdf_sha256_api_negative_tests: PASS");
    return 0;
}
