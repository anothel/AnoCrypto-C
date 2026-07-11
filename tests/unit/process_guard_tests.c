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
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"
#include "ac_process_guard_internal.h"

typedef struct counter_source_s {
    uint8_t next;
} counter_source_t;

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

static ano_crypto_status_t counter_entropy(void *user_context, uint8_t *out, size_t out_len)
{
    counter_source_t *counter = (counter_source_t *)user_context;
    size_t i;

    if (counter == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = counter->next;
        counter->next = (uint8_t)(counter->next + 1u);
    }
    return ANO_CRYPTO_OK;
}

static void test_pure_process_guard(void)
{
    ano_crypto_internal_process_guard_t guard;
    uint64_t marker;

    memset(&guard, 0, sizeof(guard));
    expect_status("inactive guard permits use",
                  ano_crypto_internal_process_guard_check_marker(&guard, 7u),
                  ANO_CRYPTO_OK);

    expect_status("guard init", ano_crypto_internal_process_guard_init(&guard), ANO_CRYPTO_OK);
    marker = ano_crypto_internal_process_marker_current();
    if (marker != 0u) {
        expect_status("same process marker",
                      ano_crypto_internal_process_guard_check_marker(&guard, marker),
                      ANO_CRYPTO_OK);
        expect_status("changed process marker requires reseed",
                      ano_crypto_internal_process_guard_check_marker(&guard, marker + 1u),
                      ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED);
    } else {
        expect_true("unsupported platform leaves guard inactive", guard.active == 0u);
    }

    ano_crypto_internal_process_guard_reset(&guard);
    expect_true("guard reset", guard.active == 0u && guard.process_marker == 0u);
}

#if defined(__unix__) || defined(__APPLE__)
static void test_source_backed_drbg_after_fork(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t source;
    counter_source_t counter;
    uint8_t *ctx;
    pid_t child;
    int child_status = 0;

    counter.next = 0x30u;
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate fork test context\n");
        ++g_failures;
        return;
    }
    memset(ctx, 0, ctx_size);

    expect_status("fork source init",
                  ano_crypto_internal_entropy_source_init_callback(
                      &source, counter_entropy, &counter, 16u),
                  ANO_CRYPTO_OK);
    expect_status("fork source-backed instantiate",
                  ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
                      ctx, ctx_size, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    child = fork();
    if (child < 0) {
        fprintf(stderr, "FAIL fork unavailable\n");
        ++g_failures;
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
        free(ctx);
        return;
    }

    if (child == 0) {
        uint8_t out[32];
        ano_crypto_status_t st;
        int child_failed = 0;

        memset(out, 0xa5, sizeof(out));
        st = ano_crypto_drbg_generate(
            ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
        if (st != ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED ||
            !buffer_is_zero(out, sizeof(out))) {
            child_failed = 1;
        }

        st = ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
            ctx, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            child_failed = 1;
        }

        memset(out, 0, sizeof(out));
        st = ano_crypto_drbg_generate(
            ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            child_failed = 1;
        }

        memset(out, 0, sizeof(out));
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
        _exit(child_failed == 0 ? 0 : 1);
    }

    if (waitpid(child, &child_status, 0) != child ||
        !WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
        fprintf(stderr, "FAIL child process guard behavior\n");
        ++g_failures;
    }

    (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
    free(ctx);
}
#endif

#if defined(__unix__) || defined(__APPLE__)
static void test_explicit_entropy_drbg_remains_caller_managed(size_t ctx_size)
{
    uint8_t entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t *ctx;
    size_t i;
    pid_t child;
    int child_status = 0;

    for (i = 0u; i < sizeof(entropy); ++i) {
        entropy[i] = (uint8_t)(0x50u + (uint8_t)i);
    }
    for (i = 0u; i < sizeof(nonce); ++i) {
        nonce[i] = (uint8_t)(0x90u + (uint8_t)i);
    }

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate explicit-entropy context\n");
        ++g_failures;
        return;
    }
    memset(ctx, 0, ctx_size);

    expect_status("explicit-entropy instantiate",
                  ano_crypto_drbg_instantiate(
                      ctx, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      entropy, sizeof(entropy), nonce, sizeof(nonce),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    child = fork();
    if (child < 0) {
        fprintf(stderr, "FAIL fork unavailable for explicit-entropy boundary\n");
        ++g_failures;
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
        free(ctx);
        return;
    }

    if (child == 0) {
        uint8_t out[32];
        ano_crypto_status_t st;

        memset(out, 0, sizeof(out));
        st = ano_crypto_drbg_generate(
            ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
        memset(out, 0, sizeof(out));
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
        _exit(st == ANO_CRYPTO_OK ? 0 : 1);
    }

    if (waitpid(child, &child_status, 0) != child ||
        !WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
        fprintf(stderr, "FAIL explicit-entropy caller-managed boundary\n");
        ++g_failures;
    }

    (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
    memset(entropy, 0, sizeof(entropy));
    memset(nonce, 0, sizeof(nonce));
    free(ctx);
}
#endif

int main(void)
{
    size_t ctx_size = 0u;

    test_pure_process_guard();

    expect_status("context size",
                  ano_crypto_drbg_context_size(
                      ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

#if defined(__unix__) || defined(__APPLE__)
    test_source_backed_drbg_after_fork(ctx_size);
    test_explicit_entropy_drbg_remains_caller_managed(ctx_size);
#else
    (void)ctx_size;
#endif

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "process_guard_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("process_guard_tests: PASS");
    return 0;
}
