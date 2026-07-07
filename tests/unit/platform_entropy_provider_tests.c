#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "ac_entropy_internal.h"

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static void expect_true(const char *name, int condition)
{
    if (!condition) {
        fprintf(stderr, "FAIL %s\n", name);
        ++g_failures;
    }
}

static int buffer_is_zero(const uint8_t *buffer, size_t buffer_len)
{
    size_t i;

    for (i = 0u; i < buffer_len; ++i) {
        if (buffer[i] != 0u) {
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    uint8_t out[64];
    const char *provider = ano_crypto_internal_platform_entropy_provider_name();
    size_t i;

    expect_true("provider name non-null", provider != 0);
    printf("platform_entropy_provider_tests: provider=%s available=%d\n",
           provider == 0 ? "(null)" : provider,
           ano_crypto_internal_platform_entropy_is_available());

    expect_status("null output", ano_crypto_internal_platform_entropy_get(0, sizeof(out)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("zero length", ano_crypto_internal_platform_entropy_get(out, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    for (i = 0u; i < sizeof(out); ++i) {
        out[i] = 0xaau;
    }

    {
        ano_crypto_status_t st = ano_crypto_internal_platform_entropy_get(out, sizeof(out));
        if (ano_crypto_internal_platform_entropy_is_available()) {
            expect_status("platform entropy get", st, ANO_CRYPTO_OK);
            /* Do not print or otherwise persist entropy bytes. */
            for (i = 0u; i < sizeof(out); ++i) {
                out[i] = 0u;
            }
        } else {
            expect_status("platform entropy unsupported", st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
            expect_true("unsupported zeroizes output", buffer_is_zero(out, sizeof(out)));
        }
    }

    if (g_failures != 0) {
        fprintf(stderr, "platform_entropy_provider_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("platform_entropy_provider_tests: PASS");
    return 0;
}
