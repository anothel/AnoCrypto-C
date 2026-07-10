#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac_entropy_internal.h"

typedef struct provider_state_s {
    unsigned int calls;
    size_t partial_len;
} provider_state_t;

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

static ano_crypto_status_t fail_without_write(void *user_context, uint8_t *out, size_t out_len)
{
    provider_state_t *state = (provider_state_t *)user_context;

    (void)out;
    (void)out_len;
    ++state->calls;
    return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
}

static ano_crypto_status_t partial_then_fail(void *user_context, uint8_t *out, size_t out_len)
{
    provider_state_t *state = (provider_state_t *)user_context;
    size_t partial_len = state->partial_len;
    size_t i;

    ++state->calls;
    if (partial_len > out_len) {
        partial_len = out_len;
    }
    for (i = 0u; i < partial_len; ++i) {
        out[i] = (uint8_t)(0x40u + (uint8_t)i);
    }
    return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
}

int main(void)
{
    ano_crypto_internal_entropy_source_t source;
    provider_state_t state;
    uint8_t out[32];

    memset(&state, 0, sizeof(state));
    memset(out, 0xa5, sizeof(out));

    expect_status("init fail provider",
                  ano_crypto_internal_entropy_source_init_callback(
                      &source, fail_without_write, &state, 1u),
                  ANO_CRYPTO_OK);
    expect_status("source failure",
                  ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("source failure zeroizes output", buffer_is_zero(out, sizeof(out)));
    expect_true("source failure called provider", state.calls == 1u);

    memset(out, 0xa5, sizeof(out));
    state.calls = 0u;
    state.partial_len = 7u;
    expect_status("init partial provider",
                  ano_crypto_internal_entropy_source_init_callback(
                      &source, partial_then_fail, &state, 1u),
                  ANO_CRYPTO_OK);
    expect_status("partial output failure",
                  ano_crypto_internal_entropy_source_get(&source, out, sizeof(out)),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("partial failure zeroizes full output", buffer_is_zero(out, sizeof(out)));
    expect_true("partial failure called provider", state.calls == 1u);

    memset(out, 0xa5, sizeof(out));
    state.calls = 0u;
    expect_status("zero length rejected",
                  ano_crypto_internal_entropy_source_get(&source, out, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_true("zero length does not call provider", state.calls == 0u);

    expect_status("oversized request rejected before provider",
                  ano_crypto_internal_entropy_source_get(
                      &source, out,
                      (size_t)ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN + 1u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_true("oversized request does not call provider", state.calls == 0u);
    expect_true("oversized rejection does not touch unknown-size output", out[0] == 0xa5u);

    expect_status("oversized minimum rejected",
                  ano_crypto_internal_entropy_source_init_callback(
                      &source, fail_without_write, &state,
                      (size_t)ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN + 1u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    if (g_failures != 0) {
        fprintf(stderr, "entropy_source_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("entropy_source_negative_tests: PASS");
    return 0;
}
