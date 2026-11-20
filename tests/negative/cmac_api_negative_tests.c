#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;

static void expect_status(
    const char *name,
    ano_crypto_status_t got,
    ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n", name,
                ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++failures;
    }
}

static int bytes_are_zero(const uint8_t *value, size_t len)
{
    uint8_t accumulator = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        accumulator = (uint8_t)(accumulator | value[i]);
    }
    return accumulator == 0u;
}

static int bytes_are_value(const uint8_t *value, size_t len, uint8_t expected)
{
    size_t i;
    for (i = 0u; i < len; ++i) {
        if (value[i] != expected) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    static const uint8_t key16[16] = {0u};
    static const uint8_t key15[15] = {0u};
    static const uint8_t hmac_key[4] = {'J','e','f','e'};
    static const uint8_t iv[16] = {0u};
    static const uint8_t message[17] = {
        1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,11u,12u,13u,14u,15u,16u,17u
    };
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t overlap[64];
    uint8_t cipher_ctx[512];
    uint8_t hmac_ctx[512];
    ano_crypto_csp_handle_t cmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t cipher_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t hmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t *ctx = 0;
    uint8_t *guarded_ctx = 0;
    size_t ctx_size = 0u;
    size_t cipher_ctx_size = 0u;
    size_t written = 123u;

    expect_status("context size null", ano_crypto_cmac_context_size(0),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("mac size null", ano_crypto_cmac_mac_size(0),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("before module init",
                  ano_crypto_cmac_compute_with_key(
                      1u, 0, 0u, tag, sizeof(tag), &written, 0u),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("bad CMAC key length",
                  ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_CMAC, key15, sizeof(key15),
                      ANO_CRYPTO_KEY_FLAG_NONE, &cmac_handle),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("exportable CMAC key",
                  ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_CMAC, key16, sizeof(key16),
                      ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE, &cmac_handle),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("CMAC key import",
                  ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_CMAC, key16, sizeof(key16),
                      ANO_CRYPTO_KEY_FLAG_NONE, &cmac_handle),
                  ANO_CRYPTO_OK);
    expect_status("cipher key import",
                  ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key16, sizeof(key16),
                      ANO_CRYPTO_KEY_FLAG_NONE, &cipher_handle),
                  ANO_CRYPTO_OK);
    expect_status("hmac key import",
                  ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, hmac_key, sizeof(hmac_key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &hmac_handle),
                  ANO_CRYPTO_OK);
    expect_status("context size", ano_crypto_cmac_context_size(&ctx_size),
                  ANO_CRYPTO_OK);
    if (ctx_size == 0u) {
        return 2;
    }
    ctx = (uint8_t *)calloc(1u, ctx_size);
    guarded_ctx = (uint8_t *)malloc(ctx_size + 16u);
    if (ctx == 0 || guarded_ctx == 0) {
        free(ctx);
        free(guarded_ctx);
        return 2;
    }

    expect_status("init null ctx",
                  ano_crypto_cmac_init(0, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init short ctx",
                  ano_crypto_cmac_init(ctx, ctx_size - 1u, cmac_handle, 0u),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("init invalid handle",
                  ano_crypto_cmac_init(
                      ctx, ctx_size, ANO_CRYPTO_CSP_INVALID_HANDLE, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init unsupported flag",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 1u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong cipher key purpose",
                  ano_crypto_cmac_init(ctx, ctx_size, cipher_handle, 0u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("wrong hmac key purpose",
                  ano_crypto_cmac_init(ctx, ctx_size, hmac_handle, 0u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("init good", ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_OK);
    expect_status("zero update", ano_crypto_cmac_update(ctx, 0, 0u),
                  ANO_CRYPTO_OK);
    expect_status("null nonzero update", ano_crypto_cmac_update(ctx, 0, 1u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("context input overlap",
                  ano_crypto_cmac_update(ctx, ctx, 1u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("message update", ano_crypto_cmac_update(ctx, message, sizeof(message)),
                  ANO_CRYPTO_OK);

    written = 123u;
    expect_status("short final",
                  ano_crypto_cmac_final(
                      ctx, tag, sizeof(tag) - 1u, &written),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (written != 0u) {
        ++failures;
    }
    expect_status("context output overlap",
                  ano_crypto_cmac_final(
                      ctx, ctx, ANO_CRYPTO_CMAC_TAG_SIZE, &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("final after rejected overlap",
                  ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written),
                  ANO_CRYPTO_OK);
    if (written != sizeof(tag)) {
        ++failures;
    }
    expect_status("repeated final",
                  ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("update after final",
                  ano_crypto_cmac_update(ctx, message, sizeof(message)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("compute null input nonzero",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 1u, tag, sizeof(tag), &written, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute null output",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 0u, 0, sizeof(tag), &written, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute null written",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag), 0, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute short output",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag) - 1u, &written, 0u),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);

    memset(overlap, 0x3cu, sizeof(overlap));
    expect_status("compute input output overlap",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, overlap, 32u, overlap + 8u,
                      ANO_CRYPTO_CMAC_TAG_SIZE, &written, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("compute input written overlap",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, overlap, 32u, tag,
                      ANO_CRYPTO_CMAC_TAG_SIZE, (size_t *)(void *)(overlap + 8u), 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    memset(tag, 0xa5, sizeof(tag));
    expect_status("compute invalid handle clears output",
                  ano_crypto_cmac_compute_with_key(
                      ANO_CRYPTO_CSP_INVALID_HANDLE, 0, 0u,
                      tag, sizeof(tag), &written, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    if (!bytes_are_zero(tag, sizeof(tag)) || written != 0u) {
        ++failures;
    }
    memset(tag, 0xa5, sizeof(tag));
    expect_status("compute unsupported flag clears output",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag), &written, 1u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    if (!bytes_are_zero(tag, sizeof(tag)) || written != 0u) {
        ++failures;
    }
    expect_status("compute empty",
                  ano_crypto_cmac_compute_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag), &written, 0u),
                  ANO_CRYPTO_OK);

    memcpy(altered, tag, sizeof(altered));
    altered[0] ^= 0x80u;
    expect_status("verify null tag",
                  ano_crypto_cmac_verify_with_key(
                      cmac_handle, 0, 0u, 0, sizeof(tag), 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("verify short tag",
                  ano_crypto_cmac_verify_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag) - 1u, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("verify success",
                  ano_crypto_cmac_verify_with_key(
                      cmac_handle, 0, 0u, tag, sizeof(tag), 0u),
                  ANO_CRYPTO_OK);
    expect_status("verify mismatch",
                  ano_crypto_cmac_verify_with_key(
                      cmac_handle, 0, 0u, altered, sizeof(altered), 0u),
                  ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);

    expect_status("reinit before fail-closed test",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_OK);
    expect_status("failed reinit invalidates prior state",
                  ano_crypto_cmac_init(
                      ctx, ctx_size, ANO_CRYPTO_CSP_INVALID_HANDLE, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("update after failed reinit",
                  ano_crypto_cmac_update(ctx, message, sizeof(message)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("uninit null zero", ano_crypto_cmac_uninit(0, 0u),
                  ANO_CRYPTO_OK);
    expect_status("uninit null nonzero", ano_crypto_cmac_uninit(0, ctx_size),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("init before zero-length uninit",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_OK);
    expect_status("uninit zero-length nonnull",
                  ano_crypto_cmac_uninit(ctx, 0u),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("context retained after zero-length uninit",
                  ano_crypto_cmac_update(ctx, 0, 0u),
                  ANO_CRYPTO_OK);

    expect_status("uninit short", ano_crypto_cmac_uninit(ctx, ctx_size - 1u),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (!bytes_are_zero(ctx, ctx_size - 1u)) {
        ++failures;
    }
    expect_status("context invalidated after short uninit",
                  ano_crypto_cmac_update(ctx, 0, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("full cleanup after short uninit",
                  ano_crypto_cmac_uninit(ctx, ctx_size),
                  ANO_CRYPTO_OK);
    if (!bytes_are_zero(ctx, ctx_size)) {
        ++failures;
    }

    memset(guarded_ctx, 0xa5, ctx_size + 16u);
    expect_status("guarded init",
                  ano_crypto_cmac_init(
                      guarded_ctx, ctx_size + 16u, cmac_handle, 0u),
                  ANO_CRYPTO_OK);
    memset(guarded_ctx + ctx_size, 0xa5, 16u);
    expect_status("oversized uninit bounded",
                  ano_crypto_cmac_uninit(guarded_ctx, ctx_size + 16u),
                  ANO_CRYPTO_OK);
    if (!bytes_are_zero(guarded_ctx, ctx_size) ||
        !bytes_are_value(guarded_ctx + ctx_size, 16u, 0xa5u)) {
        ++failures;
    }

    memset(cipher_ctx, 0, sizeof(cipher_ctx));
    memset(hmac_ctx, 0, sizeof(hmac_ctx));
    expect_status("cipher context size",
                  ano_crypto_cipher_context_size(
                      ANO_CRYPTO_CIPHER_ALG_AES,
                      ANO_CRYPTO_CIPHER_MODE_CBC,
                      &cipher_ctx_size),
                  ANO_CRYPTO_OK);
    expect_status("CMAC handle rejected by cipher",
                  ano_crypto_cipher_init(
                      cipher_ctx, sizeof(cipher_ctx),
                      ANO_CRYPTO_CIPHER_ALG_AES,
                      ANO_CRYPTO_CIPHER_MODE_CBC,
                      ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
                      cmac_handle, iv, sizeof(iv), 0u),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("CMAC context init",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_OK);
    expect_status("CMAC context rejected by cipher update",
                  ano_crypto_cipher_update(
                      ctx, message, 16u, cipher_ctx, sizeof(cipher_ctx), &written),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("CMAC context rejected by HMAC update",
                  ano_crypto_hmac_update(ctx, message, sizeof(message)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("HMAC context init",
                  ano_crypto_hmac_init(
                      hmac_ctx, sizeof(hmac_ctx), ANO_CRYPTO_HASH_ALG_SHA256,
                      hmac_key, sizeof(hmac_key)),
                  ANO_CRYPTO_OK);
    expect_status("HMAC context rejected by CMAC update",
                  ano_crypto_cmac_update(hmac_ctx, message, sizeof(message)),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    (void)ano_crypto_cmac_uninit(ctx, ctx_size);
    (void)ano_crypto_csp_zeroize(hmac_ctx, sizeof(hmac_ctx));

    expect_status("destroy cipher", ano_crypto_key_destroy(cipher_handle),
                  ANO_CRYPTO_OK);
    expect_status("destroy HMAC", ano_crypto_key_destroy(hmac_handle),
                  ANO_CRYPTO_OK);
    expect_status("CMAC init before shutdown",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u), ANO_CRYPTO_OK);
    expect_status("CMAC update before shutdown",
                  ano_crypto_cmac_update(ctx, message, sizeof(message)), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("CMAC final after shutdown",
                  ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written),
                  ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("cleanup after shutdown", ano_crypto_cmac_uninit(ctx, ctx_size),
                  ANO_CRYPTO_OK);
    if (!bytes_are_zero(ctx, ctx_size)) {
        fputs("FAIL CMAC cleanup after shutdown\n", stderr);
        ++failures;
    }

    expect_status("module reinit for fresh CMAC", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("stale CMAC handle after reinit",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("destroy CMAC tombstone", ano_crypto_key_destroy(cmac_handle), ANO_CRYPTO_OK);
    expect_status("fresh CMAC key import",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC, key16, sizeof(key16),
                      ANO_CRYPTO_KEY_FLAG_NONE, &cmac_handle), ANO_CRYPTO_OK);
    expect_status("fresh CMAC init",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u), ANO_CRYPTO_OK);
    expect_status("fresh CMAC update",
                  ano_crypto_cmac_update(ctx, message, sizeof(message)), ANO_CRYPTO_OK);
    expect_status("fresh CMAC final",
                  ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written), ANO_CRYPTO_OK);
    expect_status("fresh CMAC tag verifies",
                  ano_crypto_cmac_verify_with_key(cmac_handle, message, sizeof(message),
                      tag, sizeof(tag), 0u), ANO_CRYPTO_OK);
    if (written != sizeof(tag) || !bytes_are_zero(ctx, ctx_size)) {
        fputs("FAIL CMAC final length or context clearing\n", stderr);
        ++failures;
    }
    expect_status("destroy fresh CMAC key", ano_crypto_key_destroy(cmac_handle), ANO_CRYPTO_OK);
    expect_status("destroyed CMAC key rejected",
                  ano_crypto_cmac_init(ctx, ctx_size, cmac_handle, 0u),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("final module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    (void)cipher_ctx_size;
    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));
    (void)ano_crypto_csp_zeroize(overlap, sizeof(overlap));
    free(ctx);
    free(guarded_ctx);

    if (failures != 0) {
        fprintf(stderr, "CMAC negative failures: %d\n", failures);
        return 1;
    }
    printf("AES-CMAC hardened API and key-purpose negative boundaries passed (tags not printed)\n");
    return 0;
}
