#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"

typedef struct repeat_source_s {
    uint8_t first;
} repeat_source_t;

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

static ano_crypto_status_t repeat_entropy(void *user_context, uint8_t *out, size_t out_len)
{
    repeat_source_t *state = (repeat_source_t *)user_context;
    size_t i;

    if (state == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(state->first + (uint8_t)i);
    }
    return ANO_CRYPTO_OK;
}

int main(void)
{
    ano_crypto_internal_entropy_source_t source;
    ano_crypto_internal_entropy_health_t health;
    repeat_source_t provider;
    ano_crypto_module_state_t module_state;
    ano_crypto_status_t last_error;
    uint8_t initial_entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t *ctx;
    size_t ctx_size = 0u;
    size_t i;

    for (i = 0u; i < sizeof(initial_entropy); ++i) {
        initial_entropy[i] = (uint8_t)(0x10u + (uint8_t)i);
    }
    for (i = 0u; i < sizeof(nonce); ++i) {
        nonce[i] = (uint8_t)(0x90u + (uint8_t)i);
    }
    provider.first = 0x40u;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("context size",
                  ano_crypto_drbg_context_size(
                      ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate context\n");
        return 1;
    }
    memset(ctx, 0, ctx_size);

    expect_status("explicit instantiate",
                  ano_crypto_drbg_instantiate(
                      ctx, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      initial_entropy, sizeof(initial_entropy),
                      nonce, sizeof(nonce),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("source init",
                  ano_crypto_internal_entropy_source_init_callback(
                      &source, repeat_entropy, &provider,
                      ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE),
                  ANO_CRYPTO_OK);
    expect_status("continuous health init",
                  ano_crypto_internal_entropy_health_init(
                      &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_MODE_CONTINUOUS_DUPLICATE_BLOCK_CANDIDATE,
                      ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE),
                  ANO_CRYPTO_OK);
    expect_status("attach fatal health policy",
                  ano_crypto_internal_entropy_source_attach_health_test(
                      &source, &health,
                      ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_MODULE_FATAL),
                  ANO_CRYPTO_OK);

    expect_status("first reseed sample passes",
                  ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
                      ctx, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("duplicate reseed sample fails health test",
                  ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
                      ctx, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("fatal health failure zeroizes DRBG context",
                buffer_is_zero(ctx, ctx_size));
    expect_status("module state query",
                  ano_crypto_module_get_state(&module_state), ANO_CRYPTO_OK);
    expect_true("module enters error state",
                module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE);
    expect_status("last error query",
                  ano_crypto_module_get_last_error(&last_error), ANO_CRYPTO_OK);
    expect_true("last error records entropy source failure",
                last_error == ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_status("service gate is fatal",
                  ano_crypto_module_require_operational(),
                  ANO_CRYPTO_ERR_FATAL_STATE);

    ano_crypto_internal_entropy_health_reset(&health);
    memset(initial_entropy, 0, sizeof(initial_entropy));
    memset(nonce, 0, sizeof(nonce));
    free(ctx);

    if (g_failures != 0) {
        fprintf(stderr, "entropy_health_error_state_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("entropy_health_error_state_tests: PASS");
    return 0;
}
