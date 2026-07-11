#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ac_entropy_health_internal.h"
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

static ano_crypto_status_t deterministic_callback(
    void *user_context,
    uint8_t *out,
    size_t out_len)
{
    uint8_t *next = (uint8_t *)user_context;
    size_t i;

    if (next == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = *next;
        *next = (uint8_t)(*next + 1u);
    }
    return ANO_CRYPTO_OK;
}

int main(void)
{
    ano_crypto_internal_entropy_source_t os_source;
    ano_crypto_internal_entropy_source_t callback_source;
    ano_crypto_internal_entropy_source_t raw_candidate;
    ano_crypto_internal_entropy_health_t health;
    uint32_t source_class = 0u;
    uint32_t health_owner = 0u;
    uint32_t restart_policy = 0u;
    uint8_t next = 0x20u;
    ano_crypto_status_t st;

    expect_status("os source init",
                  ano_crypto_internal_entropy_source_init_os(&os_source, 16u),
                  ANO_CRYPTO_OK);
    expect_status("os policy query",
                  ano_crypto_internal_entropy_source_get_policy(
                      &os_source, &source_class, &health_owner, &restart_policy),
                  ANO_CRYPTO_OK);
    expect_true("os source is external conditioned RBG",
                source_class == ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG);
    expect_true("os health tests are provider-owned",
                health_owner == ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER);
    expect_true("os provider is reacquired per call",
                restart_policy == ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_PROVIDER_PER_CALL);

    expect_status("health skeleton init",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE,
                      32u),
                  ANO_CRYPTO_OK);
    expect_status("os output health attachment rejected",
                  ano_crypto_internal_entropy_source_attach_health_test(
                      &os_source, &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_MODULE_FATAL),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("callback source init",
                  ano_crypto_internal_entropy_source_init_callback(
                      &callback_source, deterministic_callback, &next, 16u),
                  ANO_CRYPTO_OK);
    expect_status("unclassified callback health attachment rejected",
                  ano_crypto_internal_entropy_source_attach_health_test(
                      &callback_source, &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_OPERATION_ONLY),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("callback operational probe rejected",
                  ano_crypto_internal_entropy_source_probe_operational(&callback_source),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("raw candidate source init",
                  ano_crypto_internal_entropy_source_init_callback_noise_candidate(
                      &raw_candidate, deterministic_callback, &next, 16u),
                  ANO_CRYPTO_OK);
    expect_status("raw candidate policy query",
                  ano_crypto_internal_entropy_source_get_policy(
                      &raw_candidate, &source_class, &health_owner, &restart_policy),
                  ANO_CRYPTO_OK);
    expect_true("raw candidate class explicit",
                source_class == ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_RAW_NOISE_TEST_CANDIDATE);
    expect_true("raw candidate health skeleton module-owned",
                health_owner == ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_MODULE_ENGINEERING_CANDIDATE);
    expect_status("raw candidate health attachment allowed",
                  ano_crypto_internal_entropy_source_attach_health_test(
                      &raw_candidate, &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_OPERATION_ONLY),
                  ANO_CRYPTO_OK);

    st = ano_crypto_internal_entropy_source_probe_operational(&os_source);
    if (ano_crypto_internal_platform_entropy_is_available()) {
        expect_status("os operational probe", st, ANO_CRYPTO_OK);
    } else {
        expect_status("os operational probe unsupported", st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    }

    ano_crypto_internal_entropy_health_reset(&health);

    if (g_failures != 0) {
        fprintf(stderr, "entropy_operational_policy_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("entropy_operational_policy_tests: PASS");
    return 0;
}
