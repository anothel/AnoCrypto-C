#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"

#if !defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
#error "random_reseed_limit_tests requires ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS"
#endif
#if !defined(ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS)
#error "random_reseed_limit_tests requires ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS"
#endif

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got,
                          ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
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

static void expect_true(const char *name, int condition)
{
    if (!condition) {
        fprintf(stderr, "FAIL %s\n", name);
        ++g_failures;
    }
}

int main(void)
{
    uint8_t output[32];
    uint8_t *ctx;
    size_t ctx_size = 0u;

    expect_status("configure deterministic provider",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0x20u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("context size", ano_crypto_random_context_size(&ctx_size), ANO_CRYPTO_OK);

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL context allocation\n");
        return 1;
    }

    expect_status("random init",
                  ano_crypto_random_init(ctx, ctx_size, 0, 0u,
                                         ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("set counter at interval",
                  ano_crypto_internal_hmac_drbg_sha256_test_set_reseed_counter(
                      ctx, ANO_CRYPTO_INTERNAL_HMAC_DRBG_RESEED_INTERVAL),
                  ANO_CRYPTO_OK);

    memset(output, 0, sizeof(output));
    expect_status("last allowed generate",
                  ano_crypto_random_generate(ctx, output, sizeof(output), 0, 0u,
                                             ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)ano_crypto_csp_zeroize(output, sizeof(output));

    memset(output, 0xa5, sizeof(output));
    expect_status("reseed required after interval",
                  ano_crypto_random_generate(ctx, output, sizeof(output), 0, 0u,
                                             ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED);
    expect_true("reseed-required output cleared", buffer_is_zero(output, sizeof(output)));

    expect_status("configure reseed failure",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL, 0u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("reseed failure at limit",
                  ano_crypto_random_reseed(ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    memset(output, 0xa5, sizeof(output));
    expect_status("failed reseed keeps limit gate",
                  ano_crypto_random_generate(ctx, output, sizeof(output), 0, 0u,
                                             ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED);
    expect_true("post-failure output cleared", buffer_is_zero(output, sizeof(output)));

    expect_status("configure reseed recovery",
                  ano_crypto_internal_entropy_test_hook_configure(
                      ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE,
                      0x90u, 0u),
                  ANO_CRYPTO_OK);
    expect_status("reseed recovery",
                  ano_crypto_random_reseed(ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("generate after recovery",
                  ano_crypto_random_generate(ctx, output, sizeof(output), 0, 0u,
                                             ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)ano_crypto_csp_zeroize(output, sizeof(output));

    expect_status("set counter max",
                  ano_crypto_internal_hmac_drbg_sha256_test_set_reseed_counter(
                      ctx, UINT64_MAX),
                  ANO_CRYPTO_OK);
    memset(output, 0xa5, sizeof(output));
    expect_status("max counter fails without wrap",
                  ano_crypto_random_generate(ctx, output, sizeof(output), 0, 0u,
                                             ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED);
    expect_true("max-counter output cleared", buffer_is_zero(output, sizeof(output)));

    expect_status("random uninit", ano_crypto_random_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    ano_crypto_internal_entropy_test_hook_reset();
    free(ctx);

    if (g_failures != 0) {
        fprintf(stderr, "random_reseed_limit_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("random_reseed_limit_tests: PASS");
    return 0;
}
