#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <anocrypto/anocrypto.h>
#include "ac_entropy_internal.h"
#include "ac_process_guard_internal.h"

static const char *platform_name(void)
{
#if defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "apple";
#elif defined(__linux__)
    return "linux";
#else
    return "unreviewed";
#endif
}

int main(void)
{
    uint8_t output[32];
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    ano_crypto_status_t init_status;
    ano_crypto_status_t generate_status = ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    ano_crypto_status_t reseed_status = ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    ano_crypto_status_t uninit_status = ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    int available;
    int result = 0;

    available = ano_crypto_random_is_available();
    printf("platform=%s\n", platform_name());
    printf("provider=%s\n", ano_crypto_internal_platform_entropy_provider_name());
    printf("provider_available=%s\n",
           ano_crypto_internal_platform_entropy_is_available() ? "yes" : "no");
    printf("process_marker_supported=%s\n",
           ano_crypto_internal_process_guard_is_supported() ? "yes" : "no");
    printf("random_service_available=%s\n", available ? "yes" : "no");
    printf("generated_bytes_logged=no\n");

    init_status = ano_crypto_module_init(0);
    if (init_status == ANO_CRYPTO_OK && available) {
        init_status = ano_crypto_random_context_size(&ctx_size);
        if (init_status == ANO_CRYPTO_OK) {
            ctx = (uint8_t *)malloc(ctx_size);
            if (ctx == 0) {
                init_status = ANO_CRYPTO_ERR_INTERNAL_ERROR;
            }
        }
        if (init_status == ANO_CRYPTO_OK) {
            init_status = ano_crypto_random_init(
                ctx, ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
        }
        if (init_status == ANO_CRYPTO_OK) {
            generate_status = ano_crypto_random_generate(
                ctx, output, sizeof(output), 0, 0u,
                ANO_CRYPTO_RANDOM_FLAG_NONE);
        }
        if (generate_status == ANO_CRYPTO_OK) {
            reseed_status = ano_crypto_random_reseed(
                ctx, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
        }
        if (ctx != 0) {
            uninit_status = ano_crypto_random_uninit(ctx, ctx_size);
        }
    }

    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    free(ctx);
    (void)ano_crypto_module_shutdown();

    printf("init_status=%s\n", ano_crypto_status_name(init_status));
    printf("generate_status=%s\n", ano_crypto_status_name(generate_status));
    printf("reseed_status=%s\n", ano_crypto_status_name(reseed_status));
    printf("uninit_status=%s\n", ano_crypto_status_name(uninit_status));

#if defined(ANO_CRYPTO_EXPECT_RANDOM_AVAILABLE)
    if (!available) {
        result = 1;
    }
#endif
    if (available &&
        (init_status != ANO_CRYPTO_OK ||
         generate_status != ANO_CRYPTO_OK ||
         reseed_status != ANO_CRYPTO_OK ||
         uninit_status != ANO_CRYPTO_OK)) {
        result = 1;
    }
    return result;
}
