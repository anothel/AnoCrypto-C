#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    {
        static const ano_crypto_hash_algorithm_t algorithms[] = {
            ANO_CRYPTO_HASH_ALG_SHA256,
            ANO_CRYPTO_HASH_ALG_SHA384,
            ANO_CRYPTO_HASH_ALG_SHA512
        };
        size_t i;
        for (i = 0u; i < (sizeof(algorithms) / sizeof(algorithms[0])); ++i) {
            st = ano_crypto_hash_context_size(algorithms[i], &size);
            if (st != ANO_CRYPTO_OK || size == 0u) {
                fprintf(stderr, "hash context size query failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
            st = ano_crypto_hmac_context_size(algorithms[i], &size);
            if (st != ANO_CRYPTO_OK || size == 0u) {
                fprintf(stderr, "hmac context size query failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
        }
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
        static const uint8_t ikm[] = { 'i', 'n', 's', 't', 'a', 'l', 'l' };
        static const uint8_t info[] = { 'p', '3', '-', '0', '2' };
        uint8_t okm[32];
        st = ano_crypto_hkdf_sha256(ikm, sizeof(ikm), NULL, 0u, info, sizeof(info), okm, sizeof(okm));
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "HKDF-SHA-256 install smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
        (void)ano_crypto_csp_zeroize(okm, sizeof(okm));
    }

    {
        static const uint8_t password[] = { 'p', 'a', 's', 's', 'w', 'o', 'r', 'd' };
        static const uint8_t salt[] = { 'N', 'a', 'C', 'l' };
        static const uint8_t expected[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE] = {
            0x3fu,0xa8u,0x51u,0x18u,0xffu,0xfeu,0x97u,0x32u,0xdau,0x7fu,0x4cu,0x63u,0xefu,0x61u,0xf5u,0xc6u,
            0x45u,0x81u,0x44u,0xd0u,0x36u,0x40u,0x1fu,0x88u,0x29u,0xddu,0x4eu,0x5bu,0x2cu,0x50u,0xc5u,0x1fu };
        void *workspace = malloc(ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
        uint8_t derived[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE];
        if (workspace == NULL) {
            fprintf(stderr, "scrypt workspace allocation failed\n");
            return 1;
        }
        st = ano_crypto_scrypt_derive_key(password, sizeof(password), salt, sizeof(salt),
                                           workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,
                                           derived);
        free(workspace);
        if (st != ANO_CRYPTO_OK || memcmp(derived, expected, sizeof(derived)) != 0) {
            (void)ano_crypto_csp_zeroize(derived, sizeof(derived));
            fprintf(stderr, "scrypt install smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
        (void)ano_crypto_csp_zeroize(derived, sizeof(derived));
    }

    {
        static const uint8_t key[ANO_CRYPTO_AES256_GCM_KEY_SIZE] = { 0u };
        static const uint8_t nonce[ANO_CRYPTO_AES256_GCM_NONCE_SIZE] = { 0u };
        static const uint8_t message[] = { 'i', 'n', 's', 't', 'a', 'l', 'l', '-', 'g', 'c', 'm' };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t ciphertext[sizeof(message)];
        uint8_t recovered[sizeof(message)];
        uint8_t tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
        size_t written = 0u;

        st = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                                          key, sizeof(key), ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
        if (st != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "AES-256-GCM key import smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
        st = ano_crypto_aes256_gcm_encrypt(handle, nonce, sizeof(nonce),
                                            NULL, 0u, message, sizeof(message),
                                            ciphertext, sizeof(ciphertext), &written,
                                            tag, sizeof(tag));
        if (st != ANO_CRYPTO_OK || written != sizeof(message)) {
            (void)ano_crypto_key_destroy(handle);
            fprintf(stderr, "AES-256-GCM encrypt smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
        st = ano_crypto_aes256_gcm_decrypt(handle, nonce, sizeof(nonce),
                                            NULL, 0u, ciphertext, sizeof(ciphertext),
                                            tag, sizeof(tag), recovered, sizeof(recovered),
                                            &written);
        if (st != ANO_CRYPTO_OK || written != sizeof(message) ||
            memcmp(recovered, message, sizeof(message)) != 0) {
            (void)ano_crypto_key_destroy(handle);
            fprintf(stderr, "AES-256-GCM decrypt smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
        (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
        (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
        (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK) {
            return 1;
        }
    }

    {
        static const uint8_t msg[] = { 'a', 'b', 'c' };
        static const ano_crypto_hash_algorithm_t algorithms[] = {
            ANO_CRYPTO_HASH_ALG_SHA256,
            ANO_CRYPTO_HASH_ALG_SHA384,
            ANO_CRYPTO_HASH_ALG_SHA512
        };
        uint8_t digest[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
        size_t i;

        for (i = 0u; i < (sizeof(algorithms) / sizeof(algorithms[0])); ++i) {
            size_t expected = 0u;
            size_t written = 0u;
            st = ano_crypto_hash_digest_size(algorithms[i], &expected);
            if (st != ANO_CRYPTO_OK) {
                fprintf(stderr, "sha2 digest size smoke failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
            st = ano_crypto_hash_digest(algorithms[i],
                                        msg, sizeof(msg), digest, sizeof(digest), &written);
            if (st != ANO_CRYPTO_OK || written != expected) {
                fprintf(stderr, "sha2 digest smoke failed: %s\n", ano_crypto_status_name(st));
                return 1;
            }
            (void)ano_crypto_csp_zeroize(digest, sizeof(digest));
        }
    }

    {
        static const uint8_t key[] = { 'J', 'e', 'f', 'e' };
        static const uint8_t msg[] = { 'a', 'b', 'c' };
        static const ano_crypto_hash_algorithm_t algorithms[] = {
            ANO_CRYPTO_HASH_ALG_SHA256,
            ANO_CRYPTO_HASH_ALG_SHA384,
            ANO_CRYPTO_HASH_ALG_SHA512
        };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t mac[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
        size_t i;

        st = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                                          key, sizeof(key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
        if (st != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "hmac key import smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        for (i = 0u; i < (sizeof(algorithms) / sizeof(algorithms[0])); ++i) {
            size_t expected = 0u;
            size_t written = 0u;
            st = ano_crypto_hmac_mac_size(algorithms[i], &expected);
            if (st != ANO_CRYPTO_OK) {
                (void)ano_crypto_key_destroy(handle);
                return 1;
            }
            st = ano_crypto_hmac_compute_with_key(algorithms[i],
                                                  handle,
                                                  msg, sizeof(msg),
                                                  mac, sizeof(mac),
                                                  &written);
            if (st != ANO_CRYPTO_OK || written != expected) {
                fprintf(stderr, "hmac key handle smoke failed: %s\n", ano_crypto_status_name(st));
                (void)ano_crypto_key_destroy(handle);
                return 1;
            }
            (void)ano_crypto_csp_zeroize(mac, sizeof(mac));
        }

        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "hmac key destroy smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }
    }

    {
        static const uint8_t key[ANO_CRYPTO_CIPHER_AES128_KEY_SIZE] = {
            0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u,
            0x08u, 0x09u, 0x0au, 0x0bu, 0x0cu, 0x0du, 0x0eu, 0x0fu
        };
        static const uint8_t iv[ANO_CRYPTO_CIPHER_CBC_IV_SIZE] = {
            0x0fu, 0x0eu, 0x0du, 0x0cu, 0x0bu, 0x0au, 0x09u, 0x08u,
            0x07u, 0x06u, 0x05u, 0x04u, 0x03u, 0x02u, 0x01u, 0x00u
        };
        static const uint8_t plaintext[ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE] = {
            0x10u, 0x11u, 0x12u, 0x13u, 0x14u, 0x15u, 0x16u, 0x17u,
            0x18u, 0x19u, 0x1au, 0x1bu, 0x1cu, 0x1du, 0x1eu, 0x1fu
        };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t ciphertext[ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE];
        uint8_t recovered[ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE];
        size_t written = 0u;
        size_t i;
        uint8_t difference = 0u;

        st = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                                          key, sizeof(key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
        if (st != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "AES key import smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES,
                                               ANO_CRYPTO_CIPHER_MODE_CBC,
                                               ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
                                               handle,
                                               iv, sizeof(iv),
                                               plaintext, sizeof(plaintext),
                                               ciphertext, sizeof(ciphertext),
                                               &written,
                                               ANO_CRYPTO_CIPHER_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != sizeof(ciphertext)) {
            fprintf(stderr, "AES-CBC encrypt smoke failed: %s\n", ano_crypto_status_name(st));
            (void)ano_crypto_key_destroy(handle);
            return 1;
        }

        st = ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES,
                                               ANO_CRYPTO_CIPHER_MODE_CBC,
                                               ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
                                               handle,
                                               iv, sizeof(iv),
                                               ciphertext, sizeof(ciphertext),
                                               recovered, sizeof(recovered),
                                               &written,
                                               ANO_CRYPTO_CIPHER_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != sizeof(recovered)) {
            fprintf(stderr, "AES-CBC decrypt smoke failed: %s\n", ano_crypto_status_name(st));
            (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
            (void)ano_crypto_key_destroy(handle);
            return 1;
        }

        for (i = 0u; i < sizeof(plaintext); ++i) {
            difference |= (uint8_t)(plaintext[i] ^ recovered[i]);
        }
        (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
        (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK || difference != 0u) {
            fprintf(stderr, "AES-CBC round-trip smoke failed\n");
            return 1;
        }
    }

    {
        static const uint8_t key[16] = {
            0x2bu, 0x7eu, 0x15u, 0x16u, 0x28u, 0xaeu, 0xd2u, 0xa6u,
            0xabu, 0xf7u, 0x15u, 0x88u, 0x09u, 0xcfu, 0x4fu, 0x3cu
        };
        static const uint8_t message[] = { 'c', 'm', 'a', 'c', '-', 's', 'm', 'o', 'k', 'e' };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
        size_t written = 0u;

        st = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC,
                                          key, sizeof(key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
        if (st != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "CMAC key import smoke failed: %s\n", ano_crypto_status_name(st));
            return 1;
        }

        st = ano_crypto_cmac_compute_with_key(handle,
                                               message, sizeof(message),
                                               tag, sizeof(tag),
                                               &written,
                                               ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != ANO_CRYPTO_CMAC_TAG_SIZE) {
            fprintf(stderr, "CMAC compute smoke failed: %s\n", ano_crypto_status_name(st));
            (void)ano_crypto_key_destroy(handle);
            return 1;
        }

        st = ano_crypto_cmac_verify_with_key(handle,
                                              message, sizeof(message),
                                              tag, sizeof(tag),
                                              ANO_CRYPTO_CMAC_FLAG_NONE);
        (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "CMAC verify smoke failed: %s\n", ano_crypto_status_name(st));
            (void)ano_crypto_key_destroy(handle);
            return 1;
        }

        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK) {
            fprintf(stderr, "CMAC key destroy smoke failed: %s\n", ano_crypto_status_name(st));
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
