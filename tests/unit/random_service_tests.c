#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

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

    if (buffer == 0) {
        return 0;
    }
    for (i = 0u; i < buffer_len; ++i) {
        if (buffer[i] != 0u) {
            return 0;
        }
    }
    return 1;
}

static int buffer_is_value(const uint8_t *buffer, size_t buffer_len, uint8_t value)
{
    size_t i;

    if (buffer == 0) {
        return 0;
    }
    for (i = 0u; i < buffer_len; ++i) {
        if (buffer[i] != value) {
            return 0;
        }
    }
    return 1;
}

static void test_provider_unavailable(void)
{
    uint8_t ctx[64];
    size_t ctx_size = 123u;

    expect_true("random service unavailable", ano_crypto_random_is_available() == 0);
    expect_status("unavailable context size",
                  ano_crypto_random_context_size(&ctx_size),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    expect_true("unavailable context size zero", ctx_size == 0u);

    expect_status("unavailable module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    memset(ctx, 0xa5, sizeof(ctx));
    expect_status("unavailable random init",
                  ano_crypto_random_init(ctx, sizeof(ctx), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
    expect_true("unavailable init clears context", buffer_is_zero(ctx, sizeof(ctx)));
    expect_status("unavailable module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
}

#if defined(__unix__) || defined(__APPLE__)
static void test_fork_reseed(void *ctx, size_t ctx_size)
{
    pid_t child;
    int child_status = 0;

    child = fork();
    if (child < 0) {
        fprintf(stderr, "FAIL public random fork unavailable\n");
        ++g_failures;
        return;
    }

    if (child == 0) {
        uint8_t out[32];
        ano_crypto_status_t st;

        memset(out, 0xa5, sizeof(out));
        st = ano_crypto_random_generate(
            ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
        if (st != ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED ||
            !buffer_is_zero(out, sizeof(out))) {
            _exit(21);
        }

        st = ano_crypto_random_reseed(
            ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            _exit(22);
        }

        st = ano_crypto_random_generate(
            ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            _exit(23);
        }

        (void)ano_crypto_random_uninit(ctx, ctx_size);
        (void)memset(out, 0, sizeof(out));
        _exit(0);
    }

    if (waitpid(child, &child_status, 0) != child ||
        !WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
        fprintf(stderr, "FAIL public random fork child status\n");
        ++g_failures;
    }
}
#endif

static void test_provider_available(void)
{
    static const uint8_t personalization[] = { 'p', '1', '-', '1', '9' };
    static const uint8_t additional_input[] = { 0x11u, 0x22u, 0x33u };
    uint8_t out[48];
    uint8_t entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t *ctx = 0;
    uint8_t *explicit_ctx = 0;
    uint8_t *oversized = 0;
    size_t ctx_size = 0u;
    size_t drbg_ctx_size = 0u;
    size_t i;

    expect_true("random service available", ano_crypto_random_is_available() != 0);
    expect_true("random service name", ano_crypto_random_service_name() != 0);
    expect_status("random context size",
                  ano_crypto_random_context_size(&ctx_size), ANO_CRYPTO_OK);
    expect_true("random context size nonzero", ctx_size != 0u);

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL random context allocation\n");
        ++g_failures;
        return;
    }

    memset(ctx, 0xa5, ctx_size);
    expect_status("random init before module",
                  ano_crypto_random_init(ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_true("pre-module init clears context", buffer_is_zero(ctx, ctx_size));

    expect_status("random module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    memset(ctx, 0xa5, ctx_size);
    expect_status("random init prediction resistance rejected",
                  ano_crypto_random_init(
                      ctx, ctx_size, 0, 0u,
                      ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_true("unsupported init clears context", buffer_is_zero(ctx, ctx_size));

    if (ctx_size > 1u) {
        memset(ctx, 0xa5, ctx_size);
        expect_status("random init small context",
                      ano_crypto_random_init(
                          ctx, ctx_size - 1u, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                      ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
        expect_true("small context region cleared", buffer_is_zero(ctx, ctx_size - 1u));
    }

    expect_status("random init",
                  ano_crypto_random_init(
                      ctx, ctx_size,
                      personalization, sizeof(personalization),
                      ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("random generate",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)memset(out, 0, sizeof(out));

    expect_status("random generate additional input",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out),
                      additional_input, sizeof(additional_input),
                      ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)memset(out, 0, sizeof(out));

    memset(out, 0xa5, sizeof(out));
    expect_status("random generate invalid additional input",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 1u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_true("invalid additional input clears output", buffer_is_zero(out, sizeof(out)));

    memset(out, 0xa5, sizeof(out));
    expect_status("random generate prediction resistance rejected",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u,
                      ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_true("unsupported generate clears output", buffer_is_zero(out, sizeof(out)));

    oversized = (uint8_t *)malloc((size_t)ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE + 1u);
    if (oversized == 0) {
        fprintf(stderr, "FAIL oversized output allocation\n");
        ++g_failures;
    } else {
        memset(oversized, 0xa5, (size_t)ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE + 1u);
        expect_status("random oversized request",
                      ano_crypto_random_generate(
                          ctx, oversized,
                          (size_t)ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE + 1u,
                          0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                      ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
        expect_true("oversized request clears output",
                    buffer_is_zero(
                        oversized, (size_t)ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE + 1u));
        free(oversized);
    }

    expect_status("random reseed",
                  ano_crypto_random_reseed(
                      ctx,
                      additional_input, sizeof(additional_input),
                      ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("random reseed prediction resistance rejected",
                  ano_crypto_random_reseed(
                      ctx, 0, 0u,
                      ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    memset(out, 0xa5, sizeof(out));
    expect_status("explicit DRBG generate rejects random context",
                  ano_crypto_drbg_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_true("explicit DRBG rejection leaves output untouched",
                buffer_is_value(out, sizeof(out), 0xa5u));
    memset(entropy, 0x3c, sizeof(entropy));
    expect_status("explicit DRBG reseed rejects random context",
                  ano_crypto_drbg_reseed(
                      ctx,
                      entropy, sizeof(entropy),
                      0, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("explicit DRBG context size",
                  ano_crypto_drbg_context_size(
                      ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &drbg_ctx_size),
                  ANO_CRYPTO_OK);
    explicit_ctx = (uint8_t *)malloc(drbg_ctx_size);
    if (explicit_ctx == 0) {
        fprintf(stderr, "FAIL explicit DRBG context allocation\n");
        ++g_failures;
    } else {
        for (i = 0u; i < sizeof(entropy); ++i) {
            entropy[i] = (uint8_t)(0x30u + (uint8_t)i);
        }
        for (i = 0u; i < sizeof(nonce); ++i) {
            nonce[i] = (uint8_t)(0x70u + (uint8_t)i);
        }
        expect_status("explicit DRBG instantiate",
                      ano_crypto_drbg_instantiate(
                          explicit_ctx, drbg_ctx_size,
                          ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                          entropy, sizeof(entropy), nonce, sizeof(nonce),
                          0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                      ANO_CRYPTO_OK);
        memset(out, 0xa5, sizeof(out));
        expect_status("random rejects explicit DRBG context",
                      ano_crypto_random_generate(
                          explicit_ctx, out, sizeof(out),
                          0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                      ANO_CRYPTO_ERR_INVALID_ARGUMENT);
        expect_true("cross-API misuse clears output", buffer_is_zero(out, sizeof(out)));
        expect_status("random reseed rejects explicit DRBG context",
                      ano_crypto_random_reseed(
                          explicit_ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                      ANO_CRYPTO_ERR_INVALID_ARGUMENT);
        (void)ano_crypto_drbg_uninstantiate(explicit_ctx, drbg_ctx_size);
        free(explicit_ctx);
    }
    (void)memset(entropy, 0, sizeof(entropy));
    (void)memset(nonce, 0, sizeof(nonce));

#if defined(__unix__) || defined(__APPLE__)
    test_fork_reseed(ctx, ctx_size);
#endif

    expect_status("parent random generate after fork",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_OK);
    (void)memset(out, 0, sizeof(out));

    expect_status("random module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    memset(out, 0xa5, sizeof(out));
    expect_status("random generate after shutdown",
                  ano_crypto_random_generate(
                      ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_true("post-shutdown generate clears output", buffer_is_zero(out, sizeof(out)));

    expect_status("random uninit", ano_crypto_random_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    expect_true("random uninit clears context", buffer_is_zero(ctx, ctx_size));
    free(ctx);
}

int main(void)
{
    expect_status("random context size null",
                  ano_crypto_random_context_size(0),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_true("random service name nonnull", ano_crypto_random_service_name() != 0);

    if (ano_crypto_random_is_available()) {
        test_provider_available();
    } else {
        test_provider_unavailable();
    }

    if (g_failures != 0) {
        fprintf(stderr, "random_service_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("random_service_tests: PASS");
    return 0;
}
