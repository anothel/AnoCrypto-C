#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>

#define STRESS_LIFECYCLE_COUNT 128u
#define STRESS_GENERATE_COUNT 64u
#define STRESS_OUTPUT_CAPACITY 1024u

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

static void fill_additional_input(uint8_t *buffer, size_t buffer_len,
                                  size_t lifecycle_index, size_t generate_index)
{
    size_t i;

    for (i = 0u; i < buffer_len; ++i) {
        buffer[i] = (uint8_t)((lifecycle_index + generate_index + i) & 0xffu);
    }
}

int main(void)
{
    static const size_t request_sizes[] = { 1u, 32u, 257u, 1024u };
    uint8_t additional_input[16];
    uint8_t output[STRESS_OUTPUT_CAPACITY];
    uint8_t *ctx;
    size_t ctx_size = 0u;
    size_t lifecycle_index;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "random_lifecycle_stress_tests: module init failed: %s\n",
                ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_random_context_size(&ctx_size);
    if (!ano_crypto_random_is_available()) {
        if (st != ANO_CRYPTO_ERR_NOT_IMPLEMENTED || ctx_size != 0u) {
            fprintf(stderr, "random_lifecycle_stress_tests: unavailable boundary mismatch\n");
            (void)ano_crypto_module_shutdown();
            return 1;
        }
        (void)ano_crypto_module_shutdown();
        puts("random_lifecycle_stress_tests: PASS (service unavailable by policy)");
        return 0;
    }
    if (st != ANO_CRYPTO_OK || ctx_size == 0u) {
        fprintf(stderr, "random_lifecycle_stress_tests: context size failed: %s\n",
                ano_crypto_status_name(st));
        (void)ano_crypto_module_shutdown();
        return 1;
    }

    ctx = (uint8_t *)malloc(ctx_size);
    if (ctx == 0) {
        fprintf(stderr, "random_lifecycle_stress_tests: context allocation failed\n");
        (void)ano_crypto_module_shutdown();
        return 1;
    }

    for (lifecycle_index = 0u;
         lifecycle_index < STRESS_LIFECYCLE_COUNT;
         ++lifecycle_index) {
        size_t generate_index;

        memset(ctx, 0, ctx_size);
        fill_additional_input(additional_input, sizeof(additional_input),
                              lifecycle_index, 0u);
        st = ano_crypto_random_init(
            ctx, ctx_size,
            additional_input, sizeof(additional_input),
            ANO_CRYPTO_RANDOM_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "random_lifecycle_stress_tests: init failed at lifecycle %lu: %s\n",
                    (unsigned long)lifecycle_index, ano_crypto_status_name(st));
            free(ctx);
            (void)ano_crypto_module_shutdown();
            return 1;
        }

        for (generate_index = 0u;
             generate_index < STRESS_GENERATE_COUNT;
             ++generate_index) {
            size_t request_len = request_sizes[
                generate_index % (sizeof(request_sizes) / sizeof(request_sizes[0]))];

            fill_additional_input(additional_input, sizeof(additional_input),
                                  lifecycle_index, generate_index);
            memset(output, 0, sizeof(output));
            st = ano_crypto_random_generate(
                ctx, output, request_len,
                additional_input, sizeof(additional_input),
                ANO_CRYPTO_RANDOM_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr,
                        "random_lifecycle_stress_tests: generate failed at lifecycle %lu request %lu: %s\n",
                        (unsigned long)lifecycle_index,
                        (unsigned long)generate_index,
                        ano_crypto_status_name(st));
                (void)ano_crypto_random_uninit(ctx, ctx_size);
                (void)ano_crypto_csp_zeroize(output, sizeof(output));
                free(ctx);
                (void)ano_crypto_module_shutdown();
                return 1;
            }
            (void)ano_crypto_csp_zeroize(output, sizeof(output));

            if ((generate_index + 1u) % 16u == 0u) {
                st = ano_crypto_random_reseed(
                    ctx,
                    additional_input, sizeof(additional_input),
                    ANO_CRYPTO_RANDOM_FLAG_NONE);
                if (st != ANO_CRYPTO_OK) {
                    fprintf(stderr,
                            "random_lifecycle_stress_tests: reseed failed at lifecycle %lu request %lu: %s\n",
                            (unsigned long)lifecycle_index,
                            (unsigned long)generate_index,
                            ano_crypto_status_name(st));
                    (void)ano_crypto_random_uninit(ctx, ctx_size);
                    free(ctx);
                    (void)ano_crypto_module_shutdown();
                    return 1;
                }
            }
        }

        st = ano_crypto_random_uninit(ctx, ctx_size);
        if (st != ANO_CRYPTO_OK || !buffer_is_zero(ctx, ctx_size)) {
            fprintf(stderr, "random_lifecycle_stress_tests: uninit/zeroize failed at lifecycle %lu\n",
                    (unsigned long)lifecycle_index);
            free(ctx);
            (void)ano_crypto_module_shutdown();
            return 1;
        }
    }

    (void)ano_crypto_csp_zeroize(additional_input, sizeof(additional_input));
    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    free(ctx);

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "random_lifecycle_stress_tests: module shutdown failed: %s\n",
                ano_crypto_status_name(st));
        return 1;
    }

    puts("random_lifecycle_stress_tests: PASS");
    return 0;
}
