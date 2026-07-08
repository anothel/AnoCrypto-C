#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;
static uint8_t g_max_request[ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE];
static uint8_t g_too_large_request[ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE + 1u];

static const uint8_t entropy_input[32] = {
    0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u,
    0x08u, 0x09u, 0x0au, 0x0bu, 0x0cu, 0x0du, 0x0eu, 0x0fu,
    0x10u, 0x11u, 0x12u, 0x13u, 0x14u, 0x15u, 0x16u, 0x17u,
    0x18u, 0x19u, 0x1au, 0x1bu, 0x1cu, 0x1du, 0x1eu, 0x1fu
};

static const uint8_t nonce[16] = {
    0xa0u, 0xa1u, 0xa2u, 0xa3u, 0xa4u, 0xa5u, 0xa6u, 0xa7u,
    0xa8u, 0xa9u, 0xaau, 0xabu, 0xacu, 0xadu, 0xaeu, 0xafu
};

static const uint8_t personalization[32] = {
    0x41u, 0x6eu, 0x6fu, 0x43u, 0x72u, 0x79u, 0x70u, 0x74u,
    0x6fu, 0x2du, 0x43u, 0x20u, 0x50u, 0x31u, 0x2du, 0x39u,
    0x20u, 0x70u, 0x65u, 0x72u, 0x73u, 0x6fu, 0x6eu, 0x61u,
    0x6cu, 0x69u, 0x7au, 0x61u, 0x74u, 0x69u, 0x6fu, 0x6eu
};

static const uint8_t additional_input[29] = {
    0x50u, 0x31u, 0x2du, 0x39u, 0x20u, 0x61u, 0x64u, 0x64u,
    0x69u, 0x74u, 0x69u, 0x6fu, 0x6eu, 0x61u, 0x6cu, 0x20u,
    0x69u, 0x6eu, 0x70u, 0x75u, 0x74u, 0x20u, 0x72u, 0x65u,
    0x71u, 0x75u, 0x65u, 0x73u, 0x74u
};

static const uint8_t reseed_entropy[32] = {
    0x80u, 0x81u, 0x82u, 0x83u, 0x84u, 0x85u, 0x86u, 0x87u,
    0x88u, 0x89u, 0x8au, 0x8bu, 0x8cu, 0x8du, 0x8eu, 0x8fu,
    0x90u, 0x91u, 0x92u, 0x93u, 0x94u, 0x95u, 0x96u, 0x97u,
    0x98u, 0x99u, 0x9au, 0x9bu, 0x9cu, 0x9du, 0x9eu, 0x9fu
};

static const uint8_t reseed_additional[28] = {
    0x50u, 0x31u, 0x2du, 0x39u, 0x20u, 0x72u, 0x65u, 0x73u,
    0x65u, 0x65u, 0x64u, 0x20u, 0x61u, 0x64u, 0x64u, 0x69u,
    0x74u, 0x69u, 0x6fu, 0x6eu, 0x61u, 0x6cu, 0x20u, 0x69u,
    0x6eu, 0x70u, 0x75u, 0x74u
};

static const uint8_t expected_no_personalization[64] = {
    0xf1u, 0xa9u, 0x5au, 0x25u, 0x6au, 0x83u, 0x9cu, 0xf7u,
    0x86u, 0x8eu, 0x84u, 0xa0u, 0xb1u, 0x87u, 0xbfu, 0xeeu,
    0x4du, 0xa3u, 0x1bu, 0x09u, 0x2eu, 0x9du, 0xd2u, 0x68u,
    0x02u, 0x9eu, 0x65u, 0xf7u, 0x27u, 0xf7u, 0x76u, 0xf5u,
    0x0du, 0x52u, 0x57u, 0x7cu, 0x8fu, 0x20u, 0x1cu, 0x22u,
    0x0bu, 0xf9u, 0x90u, 0x8du, 0xacu, 0x60u, 0x7au, 0xd2u,
    0x94u, 0xb9u, 0x1au, 0x67u, 0x1fu, 0x3cu, 0x06u, 0x2fu,
    0xa6u, 0x73u, 0x5bu, 0x45u, 0x6eu, 0xc7u, 0xc8u, 0x82u
};

static const uint8_t expected_with_personalization[64] = {
    0x56u, 0x4bu, 0xedu, 0x7cu, 0x6cu, 0x43u, 0xa4u, 0x20u,
    0xbau, 0xddu, 0x25u, 0xf6u, 0x7du, 0x85u, 0x54u, 0x60u,
    0x03u, 0xf2u, 0x65u, 0xf1u, 0x13u, 0xd9u, 0x8cu, 0x09u,
    0xd5u, 0x7du, 0x70u, 0xb0u, 0x2cu, 0x7bu, 0xa1u, 0xa2u,
    0xf0u, 0x8bu, 0xccu, 0x43u, 0xd7u, 0xd5u, 0x1bu, 0x47u,
    0x23u, 0xdcu, 0x3du, 0x1fu, 0x62u, 0xadu, 0xdbu, 0x25u,
    0x57u, 0xc5u, 0x3cu, 0x81u, 0x32u, 0xb2u, 0x84u, 0xd5u,
    0xcdu, 0x6eu, 0xcdu, 0xc9u, 0xbau, 0x8du, 0x8fu, 0x67u
};

static const uint8_t expected_with_additional_input[64] = {
    0x9bu, 0xbcu, 0xe2u, 0x6du, 0xbbu, 0x46u, 0xaau, 0x19u,
    0xa4u, 0x4au, 0xc3u, 0x9cu, 0x1eu, 0x63u, 0x8eu, 0x5du,
    0x55u, 0x74u, 0x1au, 0x66u, 0xd4u, 0xe4u, 0xcau, 0x01u,
    0xbau, 0xc5u, 0x7eu, 0x2fu, 0x39u, 0x33u, 0x02u, 0x04u,
    0xb5u, 0x51u, 0x5fu, 0x81u, 0x78u, 0xb1u, 0x4du, 0xfeu,
    0xb7u, 0x58u, 0xfdu, 0x95u, 0x3du, 0x15u, 0xbfu, 0x65u,
    0xacu, 0xb9u, 0xd2u, 0x85u, 0x52u, 0xc0u, 0x89u, 0x73u,
    0x58u, 0x39u, 0xffu, 0x25u, 0x44u, 0x13u, 0xdeu, 0x9cu
};

static const uint8_t expected_after_reseed[64] = {
    0xb3u, 0x55u, 0xf4u, 0xa2u, 0x5du, 0x18u, 0xcbu, 0xf1u,
    0xfbu, 0x34u, 0x20u, 0x82u, 0x78u, 0xa0u, 0xa3u, 0x7au,
    0xcau, 0x10u, 0x57u, 0xd1u, 0xf6u, 0x57u, 0x71u, 0x90u,
    0x89u, 0x42u, 0xc2u, 0x19u, 0x5fu, 0x8cu, 0x4au, 0x05u,
    0x9cu, 0x0cu, 0xabu, 0xa8u, 0x18u, 0xd2u, 0x47u, 0x27u,
    0x0eu, 0x7fu, 0xb3u, 0x26u, 0xabu, 0xa8u, 0xf1u, 0x53u,
    0x2bu, 0xc7u, 0x19u, 0xddu, 0x92u, 0x0au, 0xfeu, 0x1eu,
    0x2eu, 0xa2u, 0xefu, 0xa3u, 0xc5u, 0x91u, 0xa7u, 0x84u
};

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
}

static void fill_bytes(uint8_t *out, size_t out_len, uint8_t value)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = value;
    }
}

static int get_ctx_size(size_t *out_ctx_size)
{
    ano_crypto_status_t st;

    st = ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, out_ctx_size);
    expect_status("drbg context size", st, ANO_CRYPTO_OK);
    if (st != ANO_CRYPTO_OK || *out_ctx_size == 0u || *out_ctx_size > 256u) {
        fprintf(stderr, "FAIL drbg context size value\n");
        ++g_failures;
        return 0;
    }

    return 1;
}

static void expect_output(const char *name, const uint8_t *got, const uint8_t *expected, size_t len)
{
    if (!bytes_equal(got, expected, len)) {
        fprintf(stderr, "FAIL %s output mismatch\n", name);
        ++g_failures;
    }
}

static void test_personalization_and_additional_input_vectors(void)
{
    uint8_t ctx[256];
    uint8_t generated[64];
    size_t ctx_size = 0u;

    if (!get_ctx_size(&ctx_size)) {
        return;
    }

    expect_status("drbg instantiate no personalization",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg generate no personalization",
                  ano_crypto_drbg_generate(ctx, generated, sizeof(generated),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_output("no-personalization", generated, expected_no_personalization, sizeof(generated));
    expect_status("drbg uninstantiate no personalization",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);

    expect_status("drbg instantiate with personalization",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              personalization, sizeof(personalization),
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg generate with personalization",
                  ano_crypto_drbg_generate(ctx, generated, sizeof(generated),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_output("with-personalization", generated, expected_with_personalization, sizeof(generated));
    expect_status("drbg uninstantiate with personalization",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);

    expect_status("drbg instantiate for additional input",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              personalization, sizeof(personalization),
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg generate with additional input",
                  ano_crypto_drbg_generate(ctx, generated, sizeof(generated),
                                           additional_input, sizeof(additional_input),
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_output("with-additional-input", generated, expected_with_additional_input, sizeof(generated));
    expect_status("drbg uninstantiate additional input",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);
}

static void test_reseed_after_generate_vector(void)
{
    uint8_t ctx[256];
    uint8_t generated[64];
    size_t ctx_size = 0u;

    if (!get_ctx_size(&ctx_size)) {
        return;
    }

    expect_status("drbg instantiate before reseed-after-generate",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              personalization, sizeof(personalization),
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg first generate before reseed",
                  ano_crypto_drbg_generate(ctx, generated, sizeof(generated),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg reseed after generate",
                  ano_crypto_drbg_reseed(ctx,
                                         reseed_entropy, sizeof(reseed_entropy),
                                         reseed_additional, sizeof(reseed_additional),
                                         ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_status("drbg generate after reseed",
                  ano_crypto_drbg_generate(ctx, generated, sizeof(generated),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);
    expect_output("after-reseed", generated, expected_after_reseed, sizeof(generated));
    expect_status("drbg uninstantiate after reseed",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);
}

static void test_request_size_boundaries(void)
{
    uint8_t ctx[256];
    size_t ctx_size = 0u;

    if (!get_ctx_size(&ctx_size)) {
        return;
    }

    expect_status("drbg instantiate request boundary",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    fill_bytes(g_max_request, sizeof(g_max_request), 0xa5u);
    expect_status("drbg generate max request size",
                  ano_crypto_drbg_generate(ctx,
                                           g_max_request, sizeof(g_max_request),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    fill_bytes(g_too_large_request, sizeof(g_too_large_request), 0x5au);
    expect_status("drbg generate request too large",
                  ano_crypto_drbg_generate(ctx,
                                           g_too_large_request, sizeof(g_too_large_request),
                                           0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    if (g_too_large_request[0] != 0x5au ||
        g_too_large_request[sizeof(g_too_large_request) - 1u] != 0x5au) {
        fprintf(stderr, "FAIL request-too-large output buffer was modified\n");
        ++g_failures;
    }

    expect_status("drbg uninstantiate request boundary",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);
}

int main(void)
{
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_personalization_and_additional_input_vectors();
    test_reseed_after_generate_vector();
    test_request_size_boundaries();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hmac_drbg_boundary_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("hmac_drbg_boundary_tests: PASS");
    return 0;
}
