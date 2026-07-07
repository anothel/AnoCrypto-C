#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "ac_entropy_internal.h"

static int g_failures = 0;

typedef struct entropy_test_counter_s {
    uint8_t next;
} entropy_test_counter_t;

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

static ano_crypto_status_t counter_entropy(void *user_context, uint8_t *out, size_t out_len)
{
    entropy_test_counter_t *counter = (entropy_test_counter_t *)user_context;
    size_t i;

    if (counter == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = counter->next;
        ++counter->next;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t failing_entropy(void *user_context, uint8_t *out, size_t out_len)
{
    size_t i;

    (void)user_context;
    if (out != 0) {
        for (i = 0u; i < out_len; ++i) {
            out[i] = 0xa5u;
        }
    }

    return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
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
    ano_crypto_internal_entropy_source_t source;
    entropy_test_counter_t counter;
    uint8_t out[16];
    size_t i;

    for (i = 0u; i < sizeof(out); ++i) {
        out[i] = 0xaau;
    }

    expect_status("none init", ano_crypto_internal_entropy_source_init_none(&source), ANO_CRYPTO_OK);
    expect_status("none get", ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("none get zeroized output", buffer_is_zero(out, sizeof(out)));

    counter.next = 0x10u;
    expect_status("callback init", ano_crypto_internal_entropy_source_init_callback(
                      &source, counter_entropy, &counter, 8u), ANO_CRYPTO_OK);
    expect_status("callback get", ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_OK);
    expect_true("callback first byte", out[0] == 0x10u);
    expect_true("callback last byte", out[15] == 0x1fu);

    for (i = 0u; i < sizeof(out); ++i) {
        out[i] = 0xaau;
    }
    expect_status("callback min request", ano_crypto_internal_entropy_source_get(&source, out, 4u),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("callback min request zeroized prefix", buffer_is_zero(out, 4u));

    expect_status("failing callback init", ano_crypto_internal_entropy_source_init_callback(
                      &source, failing_entropy, 0, 8u), ANO_CRYPTO_OK);
    for (i = 0u; i < sizeof(out); ++i) {
        out[i] = 0xaau;
    }
    expect_status("failing callback get", ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("failing callback zeroized output", buffer_is_zero(out, sizeof(out)));

    expect_status("os reserved init", ano_crypto_internal_entropy_source_init_os_reserved(&source, 8u),
                  ANO_CRYPTO_OK);
    expect_status("os reserved get", ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    expect_true("os reserved zeroized output", buffer_is_zero(out, sizeof(out)));

    if (g_failures != 0) {
        fprintf(stderr, "entropy_adapter_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("entropy_adapter_tests: PASS");
    return 0;
}
