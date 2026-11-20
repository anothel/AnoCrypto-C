#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

static void fill_sequence(uint8_t *out, size_t out_len, uint8_t start)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(start + (uint8_t)i);
    }
}

static void test_drbg_negative_cases(void)
{
    uint8_t *ctx;
    uint8_t small_ctx[4];
    uint8_t cleanup_probe[2] = {0xa5u, 0xa5u};
    uint8_t entropy[32];
    uint8_t short_entropy[31];
    uint8_t nonce[16];
    uint8_t output[32];
    size_t ctx_size = 0u;
    size_t i;
    ano_crypto_drbg_state_t state = ANO_CRYPTO_DRBG_STATE_ERROR;

    fill_sequence(entropy, sizeof(entropy), 0x10u);
    fill_sequence(short_entropy, sizeof(short_entropy), 0x20u);
    fill_sequence(nonce, sizeof(nonce), 0x30u);

    expect_status("drbg context size null out",
                  ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, 0),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("drbg context size unsupported algorithm",
                  ano_crypto_drbg_context_size((ano_crypto_drbg_algorithm_t)999u, &ctx_size),
                  ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);

    expect_status("drbg context size valid",
                  ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size),
                  ANO_CRYPTO_OK);
    ctx = (uint8_t *)calloc(1u, ctx_size + 1u);
    if (ctx == NULL || ctx_size == 0u) {
        fputs("FAIL DRBG context allocation\n", stderr);
        ++g_failures;
        free(ctx);
        return;
    }

    expect_status("drbg instantiate before module init",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("drbg instantiate small context",
                  ano_crypto_drbg_instantiate(small_ctx, sizeof(small_ctx),
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);

    expect_status("drbg instantiate null entropy",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              0, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);

    expect_status("drbg instantiate short entropy",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              short_entropy, sizeof(short_entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);

    expect_status("drbg instantiate null nonce",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              0, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("drbg instantiate unsupported flag",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("hash drbg not implemented",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HASH_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    expect_status("drbg instantiate valid",
                  ano_crypto_drbg_instantiate(ctx, ctx_size,
                                              ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                              entropy, sizeof(entropy),
                                              nonce, sizeof(nonce),
                                              0, 0u,
                                              ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_OK);

    expect_status("drbg state valid",
                  ano_crypto_drbg_get_state(ctx, &state),
                  ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_DRBG_STATE_INSTANTIATED) {
        fprintf(stderr, "FAIL drbg state valid: %s\n", ano_crypto_drbg_state_name(state));
        ++g_failures;
    }

    expect_status("drbg generate null output",
                  ano_crypto_drbg_generate(ctx, 0, sizeof(output), 0, 0u,
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("drbg generate zero length",
                  ano_crypto_drbg_generate(ctx, output, 0u, 0, 0u,
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("drbg generate null additional nonzero length",
                  ano_crypto_drbg_generate(ctx, output, sizeof(output), 0, 1u,
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("drbg generate oversized request",
                  ano_crypto_drbg_generate(ctx, output,
                                           ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE + 1u,
                                           0, 0u,
                                           ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("drbg reseed short entropy enters error",
                  ano_crypto_drbg_reseed(ctx,
                                         short_entropy, sizeof(short_entropy),
                                         0, 0u,
                                         ANO_CRYPTO_DRBG_FLAG_NONE),
                  ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);

    expect_status("drbg state error after reseed failure",
                  ano_crypto_drbg_get_state(ctx, &state),
                  ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_DRBG_STATE_ERROR) {
        fprintf(stderr, "FAIL drbg state after reseed failure: %s\n", ano_crypto_drbg_state_name(state));
        ++g_failures;
    }

    expect_status("drbg uninstantiate error state",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size),
                  ANO_CRYPTO_OK);

    expect_status("DRBG instantiate before shutdown",
                  ano_crypto_drbg_instantiate(ctx, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      entropy, sizeof(entropy), nonce, sizeof(nonce),
                      NULL, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("module shutdown with active DRBG", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("DRBG generate after shutdown",
                  ano_crypto_drbg_generate(ctx, output, sizeof(output), NULL, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("DRBG reseed after shutdown",
                  ano_crypto_drbg_reseed(ctx, entropy, sizeof(entropy), NULL, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("zero-length DRBG cleanup", ano_crypto_drbg_uninstantiate(cleanup_probe, 0u),
                  ANO_CRYPTO_OK);
    if (cleanup_probe[0] != 0xa5u || cleanup_probe[1] != 0xa5u) ++g_failures;
    expect_status("short DRBG cleanup", ano_crypto_drbg_uninstantiate(cleanup_probe, 1u),
                  ANO_CRYPTO_OK);
    if (cleanup_probe[0] != 0u || cleanup_probe[1] != 0xa5u) ++g_failures;
    ctx[ctx_size] = 0xa5u;
    expect_status("DRBG uninstantiate after shutdown",
                  ano_crypto_drbg_uninstantiate(ctx, ctx_size + 1u), ANO_CRYPTO_OK);
    for (i = 0u; i < ctx_size + 1u; ++i) {
        if (ctx[i] != 0u) {
            fputs("FAIL DRBG cleanup did not clear the supplied extent\n", stderr);
            ++g_failures;
            break;
        }
    }
    expect_status("DRBG state after cleanup outside OPERATIONAL",
                  ano_crypto_drbg_get_state(ctx, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED) {
        fputs("FAIL DRBG state after shutdown cleanup\n", stderr);
        ++g_failures;
    }

    expect_status("module reinit for fresh DRBG", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    fill_sequence(entropy, sizeof(entropy), 0x40u);
    fill_sequence(nonce, sizeof(nonce), 0x50u);
    expect_status("fresh DRBG instantiate",
                  ano_crypto_drbg_instantiate(ctx, ctx_size, ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                      entropy, sizeof(entropy), nonce, sizeof(nonce),
                      NULL, 0u, ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("fresh DRBG generate",
                  ano_crypto_drbg_generate(ctx, output, sizeof(output), NULL, 0u,
                      ANO_CRYPTO_DRBG_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("fresh DRBG cleanup", ano_crypto_drbg_uninstantiate(ctx, ctx_size), ANO_CRYPTO_OK);
    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    free(ctx);
}

int main(void)
{
    test_drbg_negative_cases();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "drbg_api_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("drbg_api_negative_tests: PASS");
    return 0;
}
