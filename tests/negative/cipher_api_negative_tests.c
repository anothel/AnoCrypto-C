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
    static const uint8_t fresh_iv[16] = {1u};
    uint8_t input[48];
    uint8_t output[48];
    uint8_t overlap[64];
    uint8_t hmac_ctx[512];
    uint8_t cleanup_probe[2] = {0xa5u, 0xa5u};
    ano_crypto_csp_handle_t aes_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t hmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    size_t ctx_size = 0u;
    uint8_t *ctx = 0;
    size_t written = 123u;
    size_t i;

    for (i = 0u; i < sizeof(input); ++i) input[i] = (uint8_t)i;
    memset(output, 0xa5, sizeof(output));
    memset(overlap, 0x5a, sizeof(overlap));
    memset(hmac_ctx, 0, sizeof(hmac_ctx));

    expect_status("context size null", ano_crypto_cipher_context_size(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, 0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("context unsupported algorithm", ano_crypto_cipher_context_size((ano_crypto_cipher_algorithm_t)99, ANO_CRYPTO_CIPHER_MODE_CBC, &ctx_size), ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);
    expect_status("context unsupported mode", ano_crypto_cipher_context_size(ANO_CRYPTO_CIPHER_ALG_AES, (ano_crypto_cipher_mode_t)99, &ctx_size), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("cipher before init", ano_crypto_cipher_crypt_with_key(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, 1u, iv, sizeof(iv), input, 16u, output, sizeof(output), &written, 0u), ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("bad aes key length", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key15, sizeof(key15), ANO_CRYPTO_KEY_FLAG_NONE, &aes_handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("exportable aes key", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE, &aes_handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("aes key import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_NONE, &aes_handle), ANO_CRYPTO_OK);
    expect_status("hmac key import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC, hmac_key, sizeof(hmac_key), ANO_CRYPTO_KEY_FLAG_NONE, &hmac_handle), ANO_CRYPTO_OK);
    expect_status("context size", ano_crypto_cipher_context_size(ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, &ctx_size), ANO_CRYPTO_OK);
    ctx = (uint8_t *)calloc(1u, ctx_size + 1u);
    if (ctx == 0) return 2;

    expect_status("init null ctx", ano_crypto_cipher_init(0, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init short ctx", ano_crypto_cipher_init(ctx, ctx_size - 1u, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("init invalid direction", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, (ano_crypto_cipher_direction_t)99, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("init short iv", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv)-1u, 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("init flags", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 1u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong key type", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, hmac_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("init good", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_OK);
    expect_status("failed reinit invalid direction", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, (ano_crypto_cipher_direction_t)99, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("failed reinit invalidates context", ano_crypto_cipher_update(ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("reinit after invalid direction", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_OK);
    expect_status("failed reinit wrong key type", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, hmac_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong key reinit invalidates context", ano_crypto_cipher_update(ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("reinit after wrong key", ano_crypto_cipher_init(ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC, ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_OK);

    written = 123u;
    expect_status("update zero", ano_crypto_cipher_update(ctx, input, 0u, output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    if (written != 0u) ++failures;
    expect_status("update partial block", ano_crypto_cipher_update(ctx, input, 15u, output, sizeof(output), &written), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("update short output", ano_crypto_cipher_update(ctx, input, 16u, output, 15u, &written), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("update partial overlap forward", ano_crypto_cipher_update(ctx, overlap, 32u, overlap + 1u, 32u, &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("update partial overlap backward", ano_crypto_cipher_update(ctx, overlap + 1u, 32u, overlap, 32u, &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("update null written", ano_crypto_cipher_update(ctx, input, 16u, output, sizeof(output), 0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("hmac ctx init", ano_crypto_hmac_init(hmac_ctx, sizeof(hmac_ctx), ANO_CRYPTO_HASH_ALG_SHA256, hmac_key, sizeof(hmac_key)), ANO_CRYPTO_OK);
    expect_status("cross api hmac as cipher", ano_crypto_cipher_update(hmac_ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("cross api cipher as hmac", ano_crypto_hmac_update(ctx, input, 16u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_csp_zeroize(hmac_ctx, sizeof(hmac_ctx));

    expect_status("uninit", ano_crypto_cipher_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    expect_status("update after uninit", ano_crypto_cipher_update(ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("CBC init before shutdown", ano_crypto_cipher_init(
        ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC,
        ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, iv, sizeof(iv), 0u), ANO_CRYPTO_OK);
    expect_status("destroy hmac", ano_crypto_key_destroy(hmac_handle), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("CBC update after shutdown", ano_crypto_cipher_update(
        ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("zero-length CBC cleanup", ano_crypto_cipher_uninit(cleanup_probe, 0u),
                  ANO_CRYPTO_OK);
    if (cleanup_probe[0] != 0xa5u || cleanup_probe[1] != 0xa5u) ++failures;
    expect_status("short CBC cleanup", ano_crypto_cipher_uninit(cleanup_probe, 1u), ANO_CRYPTO_OK);
    if (cleanup_probe[0] != 0u || cleanup_probe[1] != 0xa5u) ++failures;
    ctx[ctx_size] = 0xa5u;
    expect_status("CBC cleanup after shutdown", ano_crypto_cipher_uninit(ctx, ctx_size + 1u),
                  ANO_CRYPTO_OK);
    for (i = 0u; i < ctx_size + 1u; ++i) {
        if (ctx[i] != 0u) {
            fputs("FAIL CBC cleanup did not clear the supplied extent\n", stderr);
            ++failures;
            break;
        }
    }

    expect_status("module reinit for fresh CBC", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("stale handle after reinit", ano_crypto_cipher_init(
        ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC,
        ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, fresh_iv, sizeof(fresh_iv), 0u),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("destroy AES tombstone", ano_crypto_key_destroy(aes_handle), ANO_CRYPTO_OK);
    expect_status("fresh AES key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key16, sizeof(key16), ANO_CRYPTO_KEY_FLAG_NONE,
        &aes_handle), ANO_CRYPTO_OK);
    expect_status("fresh CBC init", ano_crypto_cipher_init(
        ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC,
        ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, fresh_iv, sizeof(fresh_iv), 0u), ANO_CRYPTO_OK);
    expect_status("fresh CBC update", ano_crypto_cipher_update(
        ctx, input, 16u, output, sizeof(output), &written), ANO_CRYPTO_OK);
    if (written != 16u) ++failures;
    expect_status("fresh CBC cleanup", ano_crypto_cipher_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    expect_status("destroy fresh AES key", ano_crypto_key_destroy(aes_handle), ANO_CRYPTO_OK);
    expect_status("destroyed AES key rejected", ano_crypto_cipher_init(
        ctx, ctx_size, ANO_CRYPTO_CIPHER_ALG_AES, ANO_CRYPTO_CIPHER_MODE_CBC,
        ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT, aes_handle, fresh_iv, sizeof(fresh_iv), 0u),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("final module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    free(ctx);
    (void)ano_crypto_csp_zeroize(output, sizeof(output));
    if (failures != 0) { fprintf(stderr, "cipher negative failures: %d\n", failures); return 1; }
    printf("Cipher API negative boundaries passed (values not printed)\n");
    return 0;
}
