#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int fail_status(const char *step, ano_crypto_status_t status)
{
    fprintf(stderr, "AnoCrypto-C Core PoC: %s failed: %s\n", step, ano_crypto_status_name(status));
    return 1;
}

static int fail_message(const char *message)
{
    fprintf(stderr, "AnoCrypto-C Core PoC: %s\n", message);
    return 1;
}

static int check_status(const char *step, ano_crypto_status_t status)
{
    if (status != ANO_CRYPTO_OK) {
        return fail_status(step, status);
    }
    return 0;
}

static int run_hash_smoke(void)
{
    static const uint8_t message[] = { 'A', 'n', 'o', 'C', 'r', 'y', 'p', 't', 'o', '-', 'C', ' ', 'P', 'o', 'C' };
    static const ano_crypto_hash_algorithm_t algorithms[] = {
        ANO_CRYPTO_HASH_ALG_SHA256,
        ANO_CRYPTO_HASH_ALG_SHA384,
        ANO_CRYPTO_HASH_ALG_SHA512
    };
    uint8_t digest[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t index;

    for (index = 0u; index < (sizeof(algorithms) / sizeof(algorithms[0])); ++index) {
        size_t expected = 0u;
        size_t written = 0u;
        ano_crypto_status_t status;

        status = ano_crypto_hash_digest_size(algorithms[index], &expected);
        if (status != ANO_CRYPTO_OK) {
            return fail_status("SHA-2 digest size", status);
        }
        status = ano_crypto_hash_digest(algorithms[index],
                                        message,
                                        sizeof(message),
                                        digest,
                                        sizeof(digest),
                                        &written);
        if (status != ANO_CRYPTO_OK) {
            (void)ano_crypto_csp_zeroize(digest, sizeof(digest));
            return fail_status("SHA-2 digest", status);
        }
        if (written != expected) {
            (void)ano_crypto_csp_zeroize(digest, sizeof(digest));
            return fail_message("SHA-2 digest length mismatch");
        }

        printf("- %s digest smoke: PASS (%lu bytes written; digest not printed)\n",
               ano_crypto_hash_algorithm_name(algorithms[index]),
               (unsigned long)written);
        (void)ano_crypto_csp_zeroize(digest, sizeof(digest));
    }

    return 0;
}

static int run_hmac_key_handle_smoke(void)
{
    /* Public demo bytes only. This is not operational secret material. */
    static const uint8_t demo_key[] = {
        'p', 'u', 'b', 'l', 'i', 'c', '-', 'p', 'o', 'c', '-', 'h', 'm', 'a', 'c', '-', 'k', 'e', 'y'
    };
    static const uint8_t message[] = { 'c', 'o', 'r', 'e', '-', 'p', 'o', 'c' };
    static const ano_crypto_hash_algorithm_t algorithms[] = {
        ANO_CRYPTO_HASH_ALG_SHA256,
        ANO_CRYPTO_HASH_ALG_SHA384,
        ANO_CRYPTO_HASH_ALG_SHA512
    };
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t mac[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t index;
    ano_crypto_status_t status;

    status = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                                          demo_key,
                                          sizeof(demo_key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("HMAC demo key import", status);
    }
    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return fail_message("HMAC demo key import returned an invalid handle");
    }

    for (index = 0u; index < (sizeof(algorithms) / sizeof(algorithms[0])); ++index) {
        size_t expected = 0u;
        size_t written = 0u;

        status = ano_crypto_hmac_mac_size(algorithms[index], &expected);
        if (status != ANO_CRYPTO_OK) {
            (void)ano_crypto_key_destroy(handle);
            return fail_status("HMAC size", status);
        }
        status = ano_crypto_hmac_compute_with_key(algorithms[index],
                                                  handle,
                                                  message,
                                                  sizeof(message),
                                                  mac,
                                                  sizeof(mac),
                                                  &written);
        if (status != ANO_CRYPTO_OK) {
            (void)ano_crypto_csp_zeroize(mac, sizeof(mac));
            (void)ano_crypto_key_destroy(handle);
            return fail_status("HMAC with key handle", status);
        }
        if (written != expected) {
            (void)ano_crypto_csp_zeroize(mac, sizeof(mac));
            (void)ano_crypto_key_destroy(handle);
            return fail_message("HMAC length mismatch");
        }

        printf("- HMAC-%s key-handle smoke: PASS (%lu bytes written; MAC not printed)\n",
               ano_crypto_hash_algorithm_name(algorithms[index]),
               (unsigned long)written);
        (void)ano_crypto_csp_zeroize(mac, sizeof(mac));
    }

    status = ano_crypto_key_destroy(handle);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("HMAC demo key destroy", status);
    }

    return 0;
}

static int run_drbg_smoke(void)
{
    uint8_t entropy[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE];
    uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE];
    uint8_t output[ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE];
    void *ctx = NULL;
    size_t ctx_size = 0u;
    size_t i;
    ano_crypto_status_t status;

    status = ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &ctx_size);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("HMAC_DRBG context size", status);
    }
    if (ctx_size == 0u) {
        return fail_message("HMAC_DRBG context size is zero");
    }

    ctx = malloc(ctx_size);
    if (ctx == NULL) {
        return fail_message("HMAC_DRBG context allocation failed");
    }
    (void)memset(ctx, 0, ctx_size);

    /* Public deterministic demo bytes only. Do not use this pattern for operational entropy. */
    for (i = 0u; i < sizeof(entropy); ++i) {
        entropy[i] = (uint8_t)(0xA0u + (uint8_t)i);
    }
    for (i = 0u; i < sizeof(nonce); ++i) {
        nonce[i] = (uint8_t)(0xC0u + (uint8_t)i);
    }

    status = ano_crypto_drbg_instantiate(ctx,
                                         ctx_size,
                                         ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                         entropy,
                                         sizeof(entropy),
                                         nonce,
                                         sizeof(nonce),
                                         NULL,
                                         0u,
                                         ANO_CRYPTO_DRBG_FLAG_NONE);
    if (status != ANO_CRYPTO_OK) {
        free(ctx);
        return fail_status("HMAC_DRBG instantiate", status);
    }

    status = ano_crypto_drbg_generate(ctx,
                                      output,
                                      sizeof(output),
                                      NULL,
                                      0u,
                                      ANO_CRYPTO_DRBG_FLAG_NONE);
    if (status != ANO_CRYPTO_OK) {
        (void)ano_crypto_drbg_uninstantiate(ctx, ctx_size);
        free(ctx);
        return fail_status("HMAC_DRBG generate", status);
    }

    status = ano_crypto_drbg_uninstantiate(ctx, ctx_size);
    if (status != ANO_CRYPTO_OK) {
        free(ctx);
        return fail_status("HMAC_DRBG uninstantiate", status);
    }

    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    free(ctx);

    printf("- HMAC_DRBG-SHA-256 smoke: PASS (%lu bytes generated; output not printed)\n",
           (unsigned long)sizeof(output));
    return 0;
}

static int run_random_service_smoke(void)
{
    uint8_t output[32];
    void *ctx = NULL;
    size_t ctx_size = 0u;
    ano_crypto_status_t status;

    if (!ano_crypto_random_is_available()) {
        printf("- public random service: UNAVAILABLE on this build\n");
        return 0;
    }

    status = ano_crypto_random_context_size(&ctx_size);
    if (status != ANO_CRYPTO_OK || ctx_size == 0u) {
        return fail_status("random context size", status);
    }

    ctx = malloc(ctx_size);
    if (ctx == NULL) {
        return fail_message("random context allocation failed");
    }
    (void)memset(ctx, 0, ctx_size);

    status = ano_crypto_random_init(
        ctx, ctx_size, NULL, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (status != ANO_CRYPTO_OK) {
        free(ctx);
        return fail_status("random init", status);
    }

    status = ano_crypto_random_generate(
        ctx, output, sizeof(output), NULL, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (status != ANO_CRYPTO_OK) {
        (void)ano_crypto_random_uninit(ctx, ctx_size);
        free(ctx);
        return fail_status("random generate", status);
    }

    status = ano_crypto_random_reseed(
        ctx, NULL, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (status != ANO_CRYPTO_OK) {
        (void)ano_crypto_random_uninit(ctx, ctx_size);
        (void)ano_crypto_csp_zeroize(output, sizeof(output));
        free(ctx);
        return fail_status("random reseed", status);
    }

    status = ano_crypto_random_uninit(ctx, ctx_size);
    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    free(ctx);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("random uninit", status);
    }

    printf("- public random service: PASS (%lu bytes generated; output not printed)\n",
           (unsigned long)sizeof(output));
    return 0;
}

int main(void)
{
    const char *version = NULL;
    const char *fingerprint = NULL;
    ano_crypto_build_identity_t identity;
    ano_crypto_module_state_t state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
    ano_crypto_status_t status;
    int result = 0;

    status = ano_crypto_version_string(&version);
    if (check_status("version query", status) != 0 || version == NULL) {
        return 1;
    }

    status = ano_crypto_build_identity(&identity);
    if (check_status("build identity query", status) != 0 || identity.product_name == NULL) {
        return 1;
    }

    status = ano_crypto_build_fingerprint(&fingerprint);
    if (check_status("build fingerprint query", status) != 0 || fingerprint == NULL) {
        return 1;
    }

    printf("AnoCrypto-C Core PoC\n");
    printf("- product: %s\n", identity.product_name);
    printf("- version: %s\n", version);
    printf("- fingerprint: %s\n", fingerprint);

    status = ano_crypto_module_init(NULL);
    if (check_status("module init", status) != 0) {
        return 1;
    }

    status = ano_crypto_module_get_state(&state);
    if (check_status("module state query", status) != 0) {
        (void)ano_crypto_module_shutdown();
        return 1;
    }
    if (state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        (void)ano_crypto_module_shutdown();
        return fail_message("module did not enter OPERATIONAL state");
    }

    if (run_hash_smoke() != 0) {
        result = 1;
    } else if (run_hmac_key_handle_smoke() != 0) {
        result = 1;
    } else if (run_drbg_smoke() != 0) {
        result = 1;
    } else if (run_random_service_smoke() != 0) {
        result = 1;
    }

    status = ano_crypto_module_shutdown();
    if (status != ANO_CRYPTO_OK && result == 0) {
        return fail_status("module shutdown", status);
    }

    if (result != 0) {
        return result;
    }

    printf("AnoCrypto-C Core PoC: PASS\n");
    return 0;
}
