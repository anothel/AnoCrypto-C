#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac_entropy_internal.h"

#if !defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
#error "platform_entropy_failure_injection_tests requires ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS"
#endif

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
    ano_crypto_internal_entropy_source_t os_source;
    uint8_t first[32];
    uint8_t second[32];
    size_t i;

    expect_status("operational probe source init",
                  ano_crypto_internal_entropy_source_init_os(&os_source, 16u),
                  ANO_CRYPTO_OK);

    expect_status("deterministic hook configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0x20u, 0u),
                  ANO_CRYPTO_OK);
    expect_true("test hook provider available",
                ano_crypto_internal_platform_entropy_is_available() != 0);
    expect_status("deterministic first read",
                  ano_crypto_internal_platform_entropy_get(first, sizeof(first)),
                  ANO_CRYPTO_OK);
    expect_status("deterministic second read",
                  ano_crypto_internal_platform_entropy_get(second, sizeof(second)),
                  ANO_CRYPTO_OK);
    expect_status("deterministic operational probe",
                  ano_crypto_internal_entropy_source_probe_operational(&os_source),
                  ANO_CRYPTO_OK);
    for (i = 0u; i < sizeof(first); ++i) {
        expect_true("deterministic first sequence",
                    first[i] == (uint8_t)(0x20u + (uint8_t)i));
        expect_true("deterministic second sequence",
                    second[i] == (uint8_t)(0x40u + (uint8_t)i));
    }

    memset(first, 0xa5, sizeof(first));
    expect_status("forced failure configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL,
                      0u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("forced provider failure",
                  ano_crypto_internal_platform_entropy_get(first, sizeof(first)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_status("forced operational probe failure",
                  ano_crypto_internal_entropy_source_probe_operational(&os_source),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("forced failure zeroizes output",
                buffer_is_zero(first, sizeof(first)));

    memset(first, 0xa5, sizeof(first));
    expect_status("partial failure configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL,
                      0x70u, 9u),
                  ANO_CRYPTO_OK);
    expect_status("partial provider failure",
                  ano_crypto_internal_platform_entropy_get(first, sizeof(first)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_status("partial operational probe failure",
                  ano_crypto_internal_entropy_source_probe_operational(&os_source),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("partial provider failure zeroizes full output",
                buffer_is_zero(first, sizeof(first)));

    expect_status("zero length rejected",
                  ano_crypto_internal_platform_entropy_get(first, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    first[0] = 0xa5u;
    expect_status("oversized provider request rejected",
                  ano_crypto_internal_platform_entropy_get(
                      first,
                      (size_t)ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN + 1u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_true("oversized provider request does not touch output",
                first[0] == 0xa5u);

    ano_crypto_internal_entropy_test_hook_reset();
    memset(first, 0, sizeof(first));
    memset(second, 0, sizeof(second));

    if (g_failures != 0) {
        fprintf(stderr, "platform_entropy_failure_injection_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("platform_entropy_failure_injection_tests: PASS");
    return 0;
}
