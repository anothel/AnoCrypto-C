#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
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

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
}

static void test_hmac_drbg_nist_cavp_count0(void)
{
    static const uint8_t entropy_input[32] = {
        0x05u, 0xacu, 0x9fu, 0xc4u, 0xc6u, 0x2au, 0x02u, 0xe3u,
        0xf9u, 0x08u, 0x40u, 0xdau, 0x56u, 0x16u, 0x21u, 0x8cu,
        0x6du, 0xe5u, 0x74u, 0x3du, 0x66u, 0xb8u, 0xe0u, 0xfbu,
        0xf8u, 0x33u, 0x75u, 0x9cu, 0x59u, 0x28u, 0xb5u, 0x3du
    };
    static const uint8_t nonce[16] = {
        0x2bu, 0x89u, 0xa1u, 0x79u, 0x04u, 0x92u, 0x2eu, 0xd8u,
        0xf0u, 0x17u, 0xa6u, 0x30u, 0x44u, 0x84u, 0x85u, 0x45u
    };
    static const uint8_t entropy_input_reseed[32] = {
        0x27u, 0x91u, 0x12u, 0x6bu, 0x8bu, 0x52u, 0xeeu, 0x1fu,
        0xd9u, 0x39u, 0x2au, 0x0au, 0x13u, 0xe0u, 0x08u, 0x3bu,
        0xedu, 0x41u, 0x86u, 0xdcu, 0x64u, 0x9bu, 0x73u, 0x96u,
        0x07u, 0xacu, 0x70u, 0xecu, 0x8du, 0xceu, 0xcfu, 0x9bu
    };
    static const uint8_t additional_input_reseed[32] = {
        0x43u, 0xbau, 0xc1u, 0x3bu, 0xaeu, 0x71u, 0x50u, 0x92u,
        0xcfu, 0x7eu, 0xb2u, 0x80u, 0xa2u, 0xe1u, 0x0au, 0x96u,
        0x2fu, 0xafu, 0x72u, 0x33u, 0xc4u, 0x14u, 0x12u, 0xf6u,
        0x9bu, 0xc7u, 0x4au, 0x35u, 0xa5u, 0x84u, 0xe5u, 0x4cu
    };
    static const uint8_t additional_input_1[32] = {
        0x3fu, 0x2fu, 0xedu, 0x4bu, 0x68u, 0xd5u, 0x06u, 0xecu,
        0xefu, 0xa2u, 0x1fu, 0x3fu, 0x5bu, 0xb9u, 0x07u, 0xbeu,
        0xb0u, 0xf1u, 0x7du, 0xbcu, 0x30u, 0xf6u, 0xffu, 0xbbu,
        0xa5u, 0xe5u, 0x86u, 0x14u, 0x08u, 0xc5u, 0x3au, 0x1eu
    };
    static const uint8_t additional_input_2[32] = {
        0x52u, 0x90u, 0x30u, 0xdfu, 0x50u, 0xf4u, 0x10u, 0x98u,
        0x5fu, 0xdeu, 0x06u, 0x8du, 0xf8u, 0x2bu, 0x93u, 0x5eu,
        0xc2u, 0x3du, 0x83u, 0x9cu, 0xb4u, 0xb2u, 0x69u, 0x41u,
        0x4cu, 0x0eu, 0xdeu, 0x6cu, 0xffu, 0xeau, 0x5bu, 0x68u
    };
    static const uint8_t expected[128] = {
        0x02u, 0xddu, 0xffu, 0x51u, 0x73u, 0xdau, 0x2fu, 0xcfu,
        0xfau, 0x10u, 0x21u, 0x5bu, 0x03u, 0x0du, 0x66u, 0x0du,
        0x61u, 0x17u, 0x9eu, 0x61u, 0xecu, 0xc2u, 0x26u, 0x09u,
        0xb1u, 0x15u, 0x1au, 0x75u, 0xf1u, 0xcbu, 0xcbu, 0xb4u,
        0x36u, 0x3cu, 0x3au, 0x89u, 0x29u, 0x9bu, 0x4bu, 0x63u,
        0xacu, 0xa5u, 0xe5u, 0x81u, 0xe7u, 0x3cu, 0x86u, 0x04u,
        0x91u, 0x01u, 0x0au, 0xa3u, 0x5du, 0xe3u, 0x33u, 0x7cu,
        0xc6u, 0xc0u, 0x9eu, 0xbeu, 0xc8u, 0xc9u, 0x1au, 0x62u,
        0x87u, 0x58u, 0x6fu, 0x3au, 0x74u, 0xd9u, 0x69u, 0x4bu,
        0x46u, 0x2du, 0x27u, 0x20u, 0xeau, 0x2eu, 0x11u, 0xbbu,
        0xd0u, 0x2au, 0xf3u, 0x3au, 0xdeu, 0xfbu, 0x4au, 0x16u,
        0xe6u, 0xb3u, 0x70u, 0xfau, 0x0eu, 0xffu, 0xd5u, 0x7du,
        0x60u, 0x75u, 0x47u, 0xbdu, 0xcfu, 0xbbu, 0x78u, 0x31u,
        0xf5u, 0x4du, 0xe7u, 0x07u, 0x3au, 0xd2u, 0xa7u, 0xdau,
        0x98u, 0x7au, 0x00u, 0x16u, 0xa8u, 0x2fu, 0xa9u, 0x58u,
        0x77u, 0x9au, 0x16u, 0x86u, 0x74u, 0xb5u, 0x65u, 0x24u
    };
    uint8_t ctx_storage[256];
    uint8_t generated[128];
    size_t ctx_size = 0u;
    ano_crypto_drbg_state_t state = ANO_CRYPTO_DRBG_STATE_ERROR;

    expect_status("drbg context size",
                  ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx_storage)) {
        fprintf(stderr, "FAIL drbg context size: %lu\n", (unsigned long)ctx_size);
        ++g_failures;
        return;
    }

    expect_status("drbg instantiate",
                  ano_crypto_drbg_instantiate(ctx_storage, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy_input, sizeof(entropy_input),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("drbg get state instantiated",
                  ano_crypto_drbg_get_state(ctx_storage, &state),
                  ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_DRBG_STATE_INSTANTIATED) {
        fprintf(stderr, "FAIL drbg state: %s\n", ano_crypto_drbg_state_name(state));
        ++g_failures;
    }

    expect_status("drbg reseed",
                  ano_crypto_drbg_reseed(ctx_storage,
                                         entropy_input_reseed, sizeof(entropy_input_reseed),
                                         additional_input_reseed, sizeof(additional_input_reseed),
                                         ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("drbg generate first block",
                  ano_crypto_drbg_generate(ctx_storage, generated, sizeof(generated),
                                           additional_input_1, sizeof(additional_input_1),
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("drbg generate second block",
                  ano_crypto_drbg_generate(ctx_storage, generated, sizeof(generated),
                                           additional_input_2, sizeof(additional_input_2),
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    if (!bytes_equal(generated, expected, sizeof(expected))) {
        fprintf(stderr, "FAIL HMAC_DRBG-SHA-256 CAVP COUNT=0 output mismatch\n");
        ++g_failures;
    }

    expect_status("drbg uninstantiate",
                  ano_crypto_drbg_uninstantiate(ctx_storage, ctx_size),
                  ANO_CRYPTO_OK);
    expect_status("drbg state after uninstantiate",
                  ano_crypto_drbg_get_state(ctx_storage, &state),
                  ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED) {
        fprintf(stderr, "FAIL drbg state after uninstantiate: %s\n", ano_crypto_drbg_state_name(state));
        ++g_failures;
    }
}

int main(void)
{
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_hmac_drbg_nist_cavp_count0();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hmac_drbg_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("hmac_drbg_kat_tests: PASS");
    return 0;
}
