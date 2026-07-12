#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n", name,
                ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++failures;
    }
}

int main(void)
{
    static const uint8_t key16[16] = {0u};
    static const uint8_t key15[15] = {0u};
    static const uint8_t hmac_key[4] = {'J','e','f','e'};
    static const uint8_t iv[16] = {0u};
    static const uint8_t message[17] = {1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,11u,12u,13u,14u,15u,16u,17u};
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t cipher_ctx[512];
    uint8_t hmac_ctx[512];
    ano_crypto_csp_handle_t cmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t cipher_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t hmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    size_t cipher_ctx_size = 0u;
    size_t written = 123u;

    expect_status("context size null", ano_crypto_cmac_context_size(0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("mac size null", ano_crypto_cmac_mac_size(0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("before module init", ano_crypto_cmac_compute_with_key(1u, 0, 0u, tag, sizeof(tag), &written, 0u), ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("bad CMAC key length", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC, key15, sizeof(key15), ANO_CRYPTO_KEY_FLAG_NONE, &cmac_handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("exportable CMAC key", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE, &cmac_handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("CMAC key import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_NONE, &cmac_handle), ANO_CRYPTO_OK);
    expect_status("cipher key import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_NONE, &cipher_handle), ANO_CRYPTO_OK);
    expect_status("hmac key import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC, hmac_key, sizeof(hmac_key), ANO_CRYPTO_KEY_FLAG_NONE, &hmac_handle), ANO_CRYPTO_OK);
    expect_status("context size", ano_crypto_cmac_context_size(&ctx_size), ANO_CRYPTO_OK);
    if (ctx_size == 0u) {
        return 2;
    }
    ctx = (uint8_t *)calloc(1u, ctx_size);
    if (ctx == 0) {
        return 2;
    }

    expect_status("init null ctx", ano_crypto_cmac_init(0, ctx_size, cmac_handle, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init short ctx", ano_crypto_cmac_init(ctx, ctx_size - 1u, cmac_handle, 0u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("init invalid handle", ano_crypto_cmac_init(ctx, ctx_size, ANO_CRYPTO_CSP_INVALID_HANDLE, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init unsupported flag", ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 1u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong cipher key purpose", ano_crypto_cmac_init(ctx, ctx_size, cipher_handle, 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong hmac key purpose", ano_crypto_cmac_init(ctx, ctx_size, hmac_handle, 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("init good", ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u), ANO_CRYPTO_OK);
    expect_status("zero update", ano_crypto_cmac_update(ctx, 0, 0u), ANO_CRYPTO_OK);
    expect_status("null nonzero update", ano_crypto_cmac_update(ctx, 0, 1u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("message update", ano_crypto_cmac_update(ctx, message, sizeof(message)), ANO_CRYPTO_OK);

    written = 123u;
    expect_status("short final", ano_crypto_cmac_final(ctx, tag, sizeof(tag) - 1u, &written), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (written != 0u) {
        ++failures;
    }
    expect_status("final after short", ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written), ANO_CRYPTO_OK);
    if (written != sizeof(tag)) {
        ++failures;
    }
    expect_status("repeated final", ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("update after final", ano_crypto_cmac_update(ctx, message, sizeof(message)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("compute null input nonzero", ano_crypto_cmac_compute_with_key(cmac_handle, 0, 1u, tag, sizeof(tag), &written, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute null output", ano_crypto_cmac_compute_with_key(cmac_handle, 0, 0u, 0, sizeof(tag), &written, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute null written", ano_crypto_cmac_compute_with_key(cmac_handle, 0, 0u, tag, sizeof(tag), 0, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute short output", ano_crypto_cmac_compute_with_key(cmac_handle, 0, 0u, tag, sizeof(tag) - 1u, &written, 0u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("compute empty", ano_crypto_cmac_compute_with_key(cmac_handle, 0, 0u, tag, sizeof(tag), &written, 0u), ANO_CRYPTO_OK);

    memcpy(altered, tag, sizeof(altered));
    altered[0] ^= 0x80u;
    expect_status("verify null tag", ano_crypto_cmac_verify_with_key(cmac_handle, 0, 0u, 0, sizeof(tag), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("verify short tag", ano_crypto_cmac_verify_with_key(cmac_handle, 0, 0u, tag, sizeof(tag) - 1u, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("verify success", ano_crypto_cmac_verify_with_key(cmac_handle, 0, 0u, tag, sizeof(tag), 0u), ANO_CRYPTO_OK);
    expect_status("verify mismatch", ano_crypto_cmac_verify_with_key(cmac_handle, 0, 0u, altered, sizeof(altered), 0u), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);

    memset(cipher_ctx, 0, sizeof(cipher_ctx));
    memset(hmac_ctx, 0, sizeof(hmac_ctx));
    expect_status("cipher context size", ano_crypto_cipher_context_size(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, &cipher_ctx_size), ANO_CRYPTO_OK);
    expect_status("CMAC handle rejected by cipher", ano_crypto_cipher_init(cipher_ctx, sizeof(cipher_ctx), ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, cmac_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("CMAC context init", ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u), ANO_CRYPTO_OK);
    expect_status("CMAC context rejected by cipher update", ano_crypto_cipher_update(ctx, message, 16u, cipher_ctx, sizeof(cipher_ctx), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("CMAC context rejected by HMAC update", ano_crypto_hmac_update(ctx, message, sizeof(message)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("HMAC context init", ano_crypto_hmac_init(hmac_ctx, sizeof(hmac_ctx), ANO_CRYPTO_HASH_ALG_SHA256, hmac_key, sizeof(hmac_key)), ANO_CRYPTO_OK);
    expect_status("HMAC context rejected by CMAC update", ano_crypto_cmac_update(hmac_ctx, message, sizeof(message)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_cmac_uninit(ctx, ctx_size);
    (void)ano_crypto_csp_zeroize(hmac_ctx, sizeof(hmac_ctx));

    expect_status("destroy CMAC", ano_crypto_key_destroy(cmac_handle), ANO_CRYPTO_OK);
    expect_status("stale CMAC handle", ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("destroy cipher", ano_crypto_key_destroy(cipher_handle), ANO_CRYPTO_OK);
    expect_status("destroy HMAC", ano_crypto_key_destroy(hmac_handle), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("cleanup after shutdown", ano_crypto_cmac_uninit(ctx, ctx_size), ANO_CRYPTO_OK);

    (void)cipher_ctx_size;
    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));
    free(ctx);

    if (failures != 0) {
        fprintf(stderr, "CMAC negative failures: %d\n", failures);
        return 1;
    }
    printf("AES-CMAC API and key-purpose negative boundaries passed (tags not printed)\n");
    return 0;
}
