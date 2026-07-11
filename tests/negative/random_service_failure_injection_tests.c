#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>
#include "ac_entropy_internal.h"

#if !defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
#error "random_service_failure_injection_tests requires ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS"
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
    uint8_t *ctx;
    uint8_t out[32];
    size_t ctx_size = 0u;

    expect_status("deterministic hook configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0x10u, 0u),
                  ANO_CRYPTO_OK);
    expect_true("hook-backed random available", ano_crypto_random_is_available() != 0);
    expect_status("hook-backed context size",
                  ano_crypto_random_context_size(&ctx_size), ANO_CRYPTO_OK);
    expect_true("hook-backed context size nonzero", ctx_size != 0u);

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL random failure-injection context allocation\n");
        return 1;
    }

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("deterministic random init",
                  ano_crypto_random_init(
                      ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("deterministic random generate",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)memset(out, 0, sizeof(out));
    expect_status("deterministic random reseed",
                  ano_crypto_random_reseed(
                      ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("deterministic random uninit",
                  ano_crypto_random_uninit(ctx, ctx_size), ANO_CRYPTO_OK);

    expect_status("forced failure configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL, 0u, 0u),
                  ANO_CRYPTO_OK);
    memset(ctx, 0xa5, ctx_size);
    expect_status("forced random init failure",
                  ano_crypto_random_init(
                      ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("forced init failure clears context", buffer_is_zero(ctx, ctx_size));

    expect_status("partial failure configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL,
                      0x60u, 7u),
                  ANO_CRYPTO_OK);
    memset(ctx, 0xa5, ctx_size);
    expect_status("partial random init failure",
                  ano_crypto_random_init(
                      ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("partial init failure clears context", buffer_is_zero(ctx, ctx_size));

    expect_status("reseed setup configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0x80u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("reseed setup init",
                  ano_crypto_random_init(
                      ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("reseed forced failure configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL, 0u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("random reseed entropy failure",
                  ano_crypto_random_reseed(
                      ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_status("old random state remains usable after manual reseed failure",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)memset(out, 0, sizeof(out));
    expect_status("reseed retry configure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0xa0u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("random reseed retry",
                  ano_crypto_random_reseed(
                      ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("final random uninit",
                  ano_crypto_random_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    ano_crypto_internal_entropy_test_hook_reset();
    free(ctx);

    if (g_failures != 0) {
        fprintf(stderr, "random_service_failure_injection_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("random_service_failure_injection_tests: PASS");
    return 0;
}
