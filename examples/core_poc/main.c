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

static int run_hkdf_smoke(void)
{
    static const uint8_t ikm[] = { 'A', 'n', 'o', 'C', 'r', 'y', 'p', 't', 'o', '-', 'C' };
    static const uint8_t salt[] = { 'p', 'u', 'b', 'l', 'i', 'c', '-', 'p', 'o', 'c' };
    static const uint8_t info[] = { 'h', 'k', 'd', 'f', '-', 's', 'h', 'a', '2', '5', '6' };
    uint8_t okm[32];
    ano_crypto_status_t status;

    status = ano_crypto_hkdf_sha256(ikm, sizeof(ikm), salt, sizeof(salt),
                                    info, sizeof(info), okm, sizeof(okm));
    if (status != ANO_CRYPTO_OK) {
        (void)ano_crypto_csp_zeroize(okm, sizeof(okm));
        return fail_status("HKDF-SHA-256", status);
    }
    (void)ano_crypto_csp_zeroize(okm, sizeof(okm));
    printf("- HKDF-SHA-256 smoke: PASS (32 bytes derived; output not printed)\n");
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

static int run_aes_cbc_smoke(void)
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
        0x20u, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x27u,
        0x28u, 0x29u, 0x2au, 0x2bu, 0x2cu, 0x2du, 0x2eu, 0x2fu
    };
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t ciphertext[ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE];
    uint8_t recovered[ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE];
    size_t written = 0u;
    size_t i;
    uint8_t difference = 0u;
    ano_crypto_status_t status;

    status = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                                          key, sizeof(key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("AES demo key import", status);
    }

    status = ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES,
                                               ANO_CRYPTO_CIPHER_MODE_CBC,
                                               ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
                                               handle,
                                               iv, sizeof(iv),
                                               plaintext, sizeof(plaintext),
                                               ciphertext, sizeof(ciphertext),
                                               &written,
                                               ANO_CRYPTO_CIPHER_FLAG_NONE);
    if (status != ANO_CRYPTO_OK || written != sizeof(ciphertext)) {
        (void)ano_crypto_key_destroy(handle);
        return fail_status("AES-CBC encrypt", status);
    }

    status = ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES,
                                               ANO_CRYPTO_CIPHER_MODE_CBC,
                                               ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
                                               handle,
                                               iv, sizeof(iv),
                                               ciphertext, sizeof(ciphertext),
                                               recovered, sizeof(recovered),
                                               &written,
                                               ANO_CRYPTO_CIPHER_FLAG_NONE);
    if (status != ANO_CRYPTO_OK || written != sizeof(recovered)) {
        (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
        (void)ano_crypto_key_destroy(handle);
        return fail_status("AES-CBC decrypt", status);
    }

    for (i = 0u; i < sizeof(plaintext); ++i) {
        difference |= (uint8_t)(plaintext[i] ^ recovered[i]);
    }
    (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
    (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));
    status = ano_crypto_key_destroy(handle);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("AES demo key destroy", status);
    }
    if (difference != 0u) {
        return fail_message("AES-CBC round-trip mismatch");
    }

    printf("- AES-128-CBC no-padding smoke: PASS (%lu bytes processed; values not printed)\n",
           (unsigned long)sizeof(plaintext));
    return 0;
}

static int run_cmac_smoke(void)
{
    /* Public demo bytes only. This is not operational secret material. */
    static const uint8_t demo_key[16] = {
        0x2bu, 0x7eu, 0x15u, 0x16u, 0x28u, 0xaeu, 0xd2u, 0xa6u,
        0xabu, 0xf7u, 0x15u, 0x88u, 0x09u, 0xcfu, 0x4fu, 0x3cu
    };
    static const uint8_t message[] = { 'c', 'o', 'r', 'e', '-', 'p', 'o', 'c', '-', 'c', 'm', 'a', 'c' };
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    size_t written = 0u;
    ano_crypto_status_t status;

    status = ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC,
                                          demo_key, sizeof(demo_key),
                                          ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle);
    if (status != ANO_CRYPTO_OK || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return fail_status("CMAC demo key import", status);
    }

    status = ano_crypto_cmac_compute_with_key(handle,
                                               message, sizeof(message),
                                               tag, sizeof(tag),
                                               &written,
                                               ANO_CRYPTO_CMAC_FLAG_NONE);
    if (status != ANO_CRYPTO_OK || written != ANO_CRYPTO_CMAC_TAG_SIZE) {
        (void)ano_crypto_key_destroy(handle);
        return fail_status("AES-CMAC compute", status);
    }

    status = ano_crypto_cmac_verify_with_key(handle,
                                              message, sizeof(message),
                                              tag, sizeof(tag),
                                              ANO_CRYPTO_CMAC_FLAG_NONE);
    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    if (status != ANO_CRYPTO_OK) {
        (void)ano_crypto_key_destroy(handle);
        return fail_status("AES-CMAC verify", status);
    }

    status = ano_crypto_key_destroy(handle);
    if (status != ANO_CRYPTO_OK) {
        return fail_status("CMAC demo key destroy", status);
    }

    printf("- AES-CMAC smoke: PASS (%u-byte tag; value not printed)\n",
           (unsigned int)ANO_CRYPTO_CMAC_TAG_SIZE);
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
    } else if (run_hkdf_smoke() != 0) {
        result = 1;
    } else if (run_drbg_smoke() != 0) {
        result = 1;
    } else if (run_aes_cbc_smoke() != 0) {
        result = 1;
    } else if (run_cmac_smoke() != 0) {
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
