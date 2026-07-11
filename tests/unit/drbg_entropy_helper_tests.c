#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "ac_drbg_internal.h"
#include "ac_entropy_internal.h"

typedef struct counter_source_s {
    uint8_t next;
} counter_source_t;

typedef struct fail_second_source_s {
    unsigned int calls;
} fail_second_source_t;

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

static int buffer_equals(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
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
        ++counter->next;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t fail_on_second_call(void *user_context, uint8_t *out, size_t out_len)
{
    fail_second_source_t *state = (fail_second_source_t *)user_context;
    size_t i;

    if (state == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    state->calls += 1u;
    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(0x40u + (uint8_t)i);
    }

    if (state->calls == 2u) {
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    return ANO_CRYPTO_OK;
}

static void fill_counter_bytes(uint8_t first, uint8_t *out, size_t out_len)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(first + (uint8_t)i);
    }
}

static void test_requires_operational(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t source;
    counter_source_t counter;
    uint8_t *ctx;

    counter.next = 0x10u;
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate pre-init ctx\n");
        ++g_failures;
        return;
    }
    memset(ctx, 0xa5, ctx_size);

    expect_status("pre-init source init", ano_crypto_internal_entropy_source_init_callback(
                      &source, counter_entropy, &counter, 16u), ANO_CRYPTO_OK);
    expect_status("pre-init helper gate", ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
                      ctx, ctx_size, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    memset(ctx, 0, ctx_size);
    free(ctx);
}

static void test_callback_instantiate_matches_explicit(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t source;
    counter_source_t counter;
    uint8_t entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t generated_from_helper[64];
    uint8_t generated_from_explicit[64];
    uint8_t *ctx_helper;
    uint8_t *ctx_explicit;

    ctx_helper = (uint8_t *)malloc(ctx_size);
    ctx_explicit = (uint8_t *)malloc(ctx_size);
    if (ctx_helper == 0 || ctx_explicit == 0) {
        fprintf(stderr, "FAIL allocate callback instantiate contexts\n");
        ++g_failures;
        free(ctx_helper);
        free(ctx_explicit);
        return;
    }

    counter.next = 0x20u;
    fill_counter_bytes(0x20u, entropy, sizeof(entropy));
    fill_counter_bytes((uint8_t)(0x20u + (uint8_t)sizeof(entropy)), nonce, sizeof(nonce));

    expect_status("callback source init", ano_crypto_internal_entropy_source_init_callback(
                      &source, counter_entropy, &counter, 16u), ANO_CRYPTO_OK);
    expect_status("helper instantiate", ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
                      ctx_helper, ctx_size, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("explicit instantiate", ano_crypto_drbg_instantiate(
                      ctx_explicit, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      entropy, sizeof(entropy), nonce, sizeof(nonce), 0, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("helper generate", ano_crypto_internal_hmac_drbg_sha256_generate_source_backed(
                      ctx_helper, generated_from_helper, sizeof(generated_from_helper),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("explicit generate", ano_crypto_drbg_generate(
                      ctx_explicit, generated_from_explicit, sizeof(generated_from_explicit),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_true("helper output equals explicit output", buffer_equals(
                    generated_from_helper, generated_from_explicit, sizeof(generated_from_helper)));

    (void)ano_crypto_drbg_uninstantiate(ctx_helper, ctx_size);
    (void)ano_crypto_drbg_uninstantiate(ctx_explicit, ctx_size);
    memset(entropy, 0, sizeof(entropy));
    memset(nonce, 0, sizeof(nonce));
    memset(generated_from_helper, 0, sizeof(generated_from_helper));
    memset(generated_from_explicit, 0, sizeof(generated_from_explicit));
    free(ctx_helper);
    free(ctx_explicit);
}

static void test_reseed_from_callback_matches_explicit(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t instantiate_source;
    ano_crypto_internal_entropy_source_t reseed_source;
    counter_source_t instantiate_counter;
    counter_source_t reseed_counter;
    uint8_t entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t reseed_entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t generated_from_helper[48];
    uint8_t generated_from_explicit[48];
    uint8_t *ctx_helper;
    uint8_t *ctx_explicit;

    ctx_helper = (uint8_t *)malloc(ctx_size);
    ctx_explicit = (uint8_t *)malloc(ctx_size);
    if (ctx_helper == 0 || ctx_explicit == 0) {
        fprintf(stderr, "FAIL allocate reseed contexts\n");
        ++g_failures;
        free(ctx_helper);
        free(ctx_explicit);
        return;
    }

    instantiate_counter.next = 0x30u;
    fill_counter_bytes(0x30u, entropy, sizeof(entropy));
    fill_counter_bytes((uint8_t)(0x30u + (uint8_t)sizeof(entropy)), nonce, sizeof(nonce));
    expect_status("instantiate source init", ano_crypto_internal_entropy_source_init_callback(
                      &instantiate_source, counter_entropy, &instantiate_counter, 16u), ANO_CRYPTO_OK);
    expect_status("helper instantiate before reseed", ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
                      ctx_helper, ctx_size, &instantiate_source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("explicit instantiate before reseed", ano_crypto_drbg_instantiate(
                      ctx_explicit, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      entropy, sizeof(entropy), nonce, sizeof(nonce), 0, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);

    reseed_counter.next = 0x70u;
    fill_counter_bytes(0x70u, reseed_entropy, sizeof(reseed_entropy));
    expect_status("reseed source init", ano_crypto_internal_entropy_source_init_callback(
                      &reseed_source, counter_entropy, &reseed_counter, 16u), ANO_CRYPTO_OK);
    expect_status("helper reseed", ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(
                      ctx_helper, &reseed_source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("explicit reseed", ano_crypto_drbg_reseed(
                      ctx_explicit, reseed_entropy, sizeof(reseed_entropy), 0, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("helper generate after reseed", ano_crypto_internal_hmac_drbg_sha256_generate_source_backed(
                      ctx_helper, generated_from_helper, sizeof(generated_from_helper),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("explicit generate after reseed", ano_crypto_drbg_generate(
                      ctx_explicit, generated_from_explicit, sizeof(generated_from_explicit),
                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_true("helper reseed output equals explicit output", buffer_equals(
                    generated_from_helper, generated_from_explicit, sizeof(generated_from_helper)));

    (void)ano_crypto_drbg_uninstantiate(ctx_helper, ctx_size);
    (void)ano_crypto_drbg_uninstantiate(ctx_explicit, ctx_size);
    memset(entropy, 0, sizeof(entropy));
    memset(nonce, 0, sizeof(nonce));
    memset(reseed_entropy, 0, sizeof(reseed_entropy));
    memset(generated_from_helper, 0, sizeof(generated_from_helper));
    memset(generated_from_explicit, 0, sizeof(generated_from_explicit));
    free(ctx_helper);
    free(ctx_explicit);
}

static void test_nonce_failure_zeroizes_context(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t source;
    fail_second_source_t fail_state;
    uint8_t *ctx;

    fail_state.calls = 0u;
    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate failure ctx\n");
        ++g_failures;
        return;
    }
    memset(ctx, 0xa5, ctx_size);

    expect_status("failing source init", ano_crypto_internal_entropy_source_init_callback(
                      &source, fail_on_second_call, &fail_state, 16u), ANO_CRYPTO_OK);
    expect_status("helper nonce failure", ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
                      ctx, ctx_size, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    expect_true("context zeroized after nonce failure", buffer_is_zero(ctx, ctx_size));

    free(ctx);
}

static void test_os_source_path(size_t ctx_size)
{
    ano_crypto_internal_entropy_source_t source;
    uint8_t *ctx;
    uint8_t generated[32];
    ano_crypto_status_t st;

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL allocate os source ctx\n");
        ++g_failures;
        return;
    }
    memset(ctx, 0xa5, ctx_size);
    memset(generated, 0, sizeof(generated));

    expect_status("os source init", ano_crypto_internal_entropy_source_init_os(&source, 16u),
                  ANO_CRYPTO_OK);
    st = ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(
        ctx, ctx_size, &source, 0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
    if (ano_crypto_internal_platform_entropy_is_available()) {
        expect_status("os helper instantiate", st, ANO_CRYPTO_OK);
        expect_status("os helper generate", ano_crypto_internal_hmac_drbg_sha256_generate_source_backed(
                          ctx, generated, sizeof(generated), 0, 0u,
                          ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
        memset(generated, 0, sizeof(generated));
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
    } else {
        expect_status("os helper unsupported", st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED);
        expect_true("unsupported os helper zeroized ctx", buffer_is_zero(ctx, ctx_size));
    }

    memset(generated, 0, sizeof(generated));
    free(ctx);
}

int main(void)
{
    size_t ctx_size = 0u;

    expect_status("context size", ano_crypto_drbg_context_size(
                      ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size), ANO_CRYPTO_OK);
    expect_true("context size nonzero", ctx_size != 0u);

    if (ctx_size == 0u) {
        return 1;
    }

    test_requires_operational(ctx_size);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_callback_instantiate_matches_explicit(ctx_size);
    test_reseed_from_callback_matches_explicit(ctx_size);
    test_nonce_failure_zeroizes_context(ctx_size);
    test_os_source_path(ctx_size);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "drbg_entropy_helper_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("drbg_entropy_helper_tests: PASS");
    return 0;
}
