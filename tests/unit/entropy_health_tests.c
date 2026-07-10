#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac_entropy_health_internal.h"

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
    ano_crypto_internal_entropy_health_t health;
    uint8_t startup_good[32];
    uint8_t startup_duplicate[32];
    uint8_t continuous_sample[32];
    uint8_t continuous_next[32];
    uint8_t disabled_large_sample[ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN + 1u];
    size_t i;

    for (i = 0u; i < sizeof(startup_good); ++i) {
        startup_good[i] = (uint8_t)i;
        continuous_sample[i] = (uint8_t)(0x20u + (uint8_t)i);
        continuous_next[i] = (uint8_t)(0x60u + (uint8_t)i);
    }
    for (i = 0u; i < sizeof(startup_duplicate) / 2u; ++i) {
        startup_duplicate[i] = (uint8_t)(0x80u + (uint8_t)i);
        startup_duplicate[i + sizeof(startup_duplicate) / 2u] = startup_duplicate[i];
    }

    expect_status("null health init",
                  ano_crypto_internal_entropy_health_init(
                      0,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED,
                      0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("unsupported health mode",
                  ano_crypto_internal_entropy_health_init(&health, 99u, 32u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("oversized health sample policy",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE,
                      (size_t)ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MAX_SAMPLE_LEN + 1u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("disabled health init",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_DISABLED,
                      0u),
                  ANO_CRYPTO_OK);
    memset(disabled_large_sample, 0xa5, sizeof(disabled_large_sample));
    expect_status("disabled health is no-op above active sample ceiling",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, disabled_large_sample, sizeof(disabled_large_sample)),
                  ANO_CRYPTO_OK);

    expect_status("startup candidate init",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_STARTUP_DUPLICATE_BLOCK_CANDIDATE,
                      sizeof(startup_good)),
                  ANO_CRYPTO_OK);
    expect_status("startup distinct halves pass",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, startup_good, sizeof(startup_good)),
                  ANO_CRYPTO_OK);
    expect_status("startup duplicate halves fail",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, startup_duplicate, sizeof(startup_duplicate)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);

    expect_status("continuous candidate init",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE,
                      sizeof(continuous_sample)),
                  ANO_CRYPTO_OK);
    expect_status("continuous first sample pass",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, continuous_sample, sizeof(continuous_sample)),
                  ANO_CRYPTO_OK);
    expect_status("continuous next sample pass",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, continuous_next, sizeof(continuous_next)),
                  ANO_CRYPTO_OK);
    expect_status("continuous duplicate sample fail",
                  ano_crypto_internal_entropy_health_evaluate(
                      &health, continuous_next, sizeof(continuous_next)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);

    ano_crypto_internal_entropy_health_reset(&health);
    expect_true("health reset zeroizes state",
                buffer_is_zero((const uint8_t *)&health, sizeof(health)));

    memset(startup_good, 0, sizeof(startup_good));
    memset(startup_duplicate, 0, sizeof(startup_duplicate));
    memset(continuous_sample, 0, sizeof(continuous_sample));
    memset(continuous_next, 0, sizeof(continuous_next));
    memset(disabled_large_sample, 0, sizeof(disabled_large_sample));

    if (g_failures != 0) {
        fprintf(stderr, "entropy_health_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("entropy_health_tests: PASS");
    return 0;
}
