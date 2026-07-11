#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <anocrypto/anocrypto.h>

int main(void)
{
    const char *version = 0;
    ano_crypto_module_state_t state;
    ano_crypto_build_identity_t identity;
    size_t size = 0u;
    ano_crypto_status_t st;

    st = ano_crypto_version_string(&version);
    if (st != ANO_CRYPTO_OK || version == 0) {
        fprintf(stderr, "version query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_build_identity(&identity);
    if (st != ANO_CRYPTO_OK || identity.product_name == 0) {
        fprintf(stderr, "build identity query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "hash context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_hmac_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "hmac context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "drbg context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_get_state(&state);
    if (st != ANO_CRYPTO_OK || state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        fprintf(stderr, "unexpected module state after init\n");
        return 1;
    }

    {
        static const uint8_t msg[] = { 'a', 'b', 'c' };
        uint8_t digest[32];
        size_t written = 0u;
        st = ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                    msg, sizeof(msg), digest, sizeof(digest), &written);
        if (st != ANO_CRYPTO_OK || written != sizeof(digest)) {
            fprintf(stderr, "sha256 smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
    }



    {
        static const uint8_t key[] = { 'J', 'e', 'f', 'e' };
        static const uint8_t msg[] = { 'a', 'b', 'c' };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t mac[32];
        size_t written = 0u;

        st = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                                          key, sizeof(key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
        if (st != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "hmac key import smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_hmac_compute_with_key(ANO_CRYPTO_HASH_ALG_SHA256,
                                              handle,
                                              msg, sizeof(msg),
                                              mac, sizeof(mac),
                                              &written);
        if (st != ANO_CRYPTO_OK || written != sizeof(mac)) {
            fprintf(stderr, "hmac key handle smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "hmac key destroy smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
    }

    {
        uint8_t ctx[256];
        uint8_t entropy[32];
        uint8_t nonce[16];
        uint8_t random_like_output[32];
        size_t i;

        for (i = 0u; i < sizeof(entropy); ++i) {
            entropy[i] = (uint8_t)(0x40u + (uint8_t)i);
        }
        for (i = 0u; i < sizeof(nonce); ++i) {
            nonce[i] = (uint8_t)(0x80u + (uint8_t)i);
        }

        st = ano_crypto_drbg_instantiate(ctx, sizeof(ctx),
                                         ANO_CRYPTO_DRBG_ALG_HMAC_SHA256,
                                         entropy, sizeof(entropy),
                                         nonce, sizeof(nonce),
                                         0, 0u,
                                         ANO_CRYPTO_DRBG_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "hmac-drbg instantiate smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_drbg_generate(ctx, random_like_output, sizeof(random_like_output),
                                      0, 0u, ANO_CRYPTO_DRBG_FLAG_NONE);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "hmac-drbg generate smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_drbg_uninstantiate(ctx, sizeof(ctx));
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "hmac-drbg uninstantiate smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
    }

    {
        size_t random_ctx_size = 0u;

        if (ano_crypto_random_is_available()) {
            uint8_t *random_ctx;
            uint8_t random_output[32];

            st = ano_crypto_random_context_size(&random_ctx_size);
            if (st != ANO_CRYPTO_OK || random_ctx_size == 0u) {
                fprintf(stderr, "random context size smoke failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }

            random_ctx = (uint8_t *)malloc(random_ctx_size);
            if (random_ctx == 0) {
                fprintf(stderr, "random context allocation failed\n");
                return 1;
            }

            st = ano_crypto_random_init(
                random_ctx, random_ctx_size, 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "random init smoke failed: %s\n", ano_crypto_status_name(st));
                free(random_ctx);
                return 1;
            }

            st = ano_crypto_random_generate(
                random_ctx, random_output, sizeof(random_output),
                0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "random generate smoke failed: %s\n", ano_crypto_status_name(st));
                (void)ano_crypto_random_uninit(random_ctx, random_ctx_size);
                free(random_ctx);
                return 1;
            }

            st = ano_crypto_random_uninit(random_ctx, random_ctx_size);
            (void)ano_crypto_csp_zeroize(random_output, sizeof(random_output));
            free(random_ctx);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "random uninit smoke failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
        } else {
            st = ano_crypto_random_context_size(&random_ctx_size);
            if (st != ANO_CRYPTO_ERR_NOT_IMPLEMENTED || random_ctx_size != 0u) {
                fprintf(stderr, "random unavailable boundary smoke failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
        }
    }

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module shutdown failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    printf("AnoCrypto-C install smoke: PASS (%s)\n", version);
    return 0;
}
