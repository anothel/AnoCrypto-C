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

static int all_zero(const uint8_t *p, size_t n)
{
    size_t i;
    for (i = 0u; i < n; ++i) if (p[i] != 0u) return 0;
    return 1;
}

int main(void)
{
    uint8_t key_bytes[32] = {0u};
    uint8_t wrong_key_bytes[32] = {1u};
    uint8_t nonce[12] = {0u};
    static const uint8_t shutdown_nonce[12] = {0x41u};
    static const uint8_t shutdown_final_nonce[12] = {0x42u};
    uint8_t aad[3] = {1u,2u,3u};
    uint8_t input[32];
    uint8_t overlap_payload[33];
    uint8_t output[32];
    uint8_t ciphertext[32];
    uint8_t tag[16];
    uint8_t bad_tag[16];
    ano_crypto_csp_handle_t key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t wrong_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t snapshot_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    size_t ctx_size = 0u;
    uint8_t *ctx = 0;
    uint8_t *final_ctx = 0;
    uint8_t *misaligned_storage = 0;
    union {
        size_t written;
        uint8_t input[32];
    } overlap_storage;
    size_t written = 99u;
    size_t i;

    for (i = 0u; i < sizeof(input); ++i) input[i] = (uint8_t)i;
    memcpy(overlap_payload, input, sizeof(input));
    overlap_payload[32] = 0u;
    memset(output, 0xa5, sizeof(output));

    expect_status("context size", ano_crypto_aes256_gcm_stream_context_size(&ctx_size), ANO_CRYPTO_OK);
    ctx = (uint8_t *)calloc(1u, ctx_size + 1u);
    final_ctx = (uint8_t *)calloc(1u, ctx_size);
    misaligned_storage = (uint8_t *)calloc(1u, ctx_size + 1u);
    if (ctx == 0 || final_ctx == 0 || misaligned_storage == 0) {
        free(ctx);
        free(final_ctx);
        free(misaligned_storage);
        return 2;
    }

    expect_status("update before init", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, sizeof(output), &written), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    if (written != 0u || !all_zero(output, sizeof(output))) { fprintf(stderr, "FAIL before-init clearing\n"); ++failures; }

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key_bytes, sizeof(key_bytes), 0u, &key), ANO_CRYPTO_OK);
    expect_status("wrong key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, wrong_key_bytes, sizeof(wrong_key_bytes), 0u, &wrong_key), ANO_CRYPTO_OK);

    expect_status("short context", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size - 1u, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("misaligned context", ano_crypto_aes256_gcm_stream_init(
        misaligned_storage + 1u, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("bad direction", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, (ano_crypto_aead_direction_t)99, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("bad nonce length", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, 11u, 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("bad flags", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 1u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    expect_status("encrypt init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    expect_status("wrong final function", ano_crypto_aes256_gcm_stream_decrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_OK);
    expect_status("short output", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, 8u, &written), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (written != 0u || !all_zero(output, 8u)) { fprintf(stderr, "FAIL short-output clearing\n"); ++failures; }
    expect_status("partial overlap", ano_crypto_aes256_gcm_stream_update(
        ctx, overlap_payload, sizeof(input), overlap_payload + 1u,
        sizeof(input), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    memcpy(overlap_storage.input, input, sizeof(input));
    expect_status("out-written overlaps input", ano_crypto_aes256_gcm_stream_update(
        ctx, overlap_storage.input, sizeof(overlap_storage.input), output,
        sizeof(output), &overlap_storage.written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("valid update", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), ciphertext, sizeof(ciphertext), &written), ANO_CRYPTO_OK);
    expect_status("AAD after payload", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("encrypt final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_status("repeat final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    memset(output, 0xa5, sizeof(output));
    expect_status("update after final", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, sizeof(output), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    if (!all_zero(output, sizeof(output))) { fprintf(stderr, "FAIL post-final clearing\n"); ++failures; }

    expect_status("short-tag init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    memset(tag, 0xa5, sizeof(tag));
    expect_status("short encrypt tag", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, 15u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (!all_zero(tag, 15u)) { fprintf(stderr, "FAIL short-tag clearing\n"); ++failures; }
    expect_status("short tag consumed", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("null-tag init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    expect_status("null encrypt tag", ano_crypto_aes256_gcm_stream_encrypt_final(
        ctx, 0, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("null tag consumed", ano_crypto_aes256_gcm_stream_encrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("make ciphertext", ano_crypto_aes256_gcm_encrypt(
        key, nonce, sizeof(nonce), aad, sizeof(aad), input, sizeof(input),
        ciphertext, sizeof(ciphertext), &written, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_status("decrypt init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_DECRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    expect_status("decrypt AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_OK);
    expect_status("decrypt update", ano_crypto_aes256_gcm_stream_update(
        ctx, ciphertext, sizeof(ciphertext), output, sizeof(output), &written), ANO_CRYPTO_OK);
    if (memcmp(output, input, sizeof(input)) != 0) { fprintf(stderr, "FAIL unverified plaintext contract\n"); ++failures; }
    memcpy(bad_tag, tag, sizeof(tag)); bad_tag[0] ^= 1u;
    expect_status("bad tag", ano_crypto_aes256_gcm_stream_decrypt_final(
        ctx, bad_tag, sizeof(bad_tag)), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    if (memcmp(output, input, sizeof(input)) != 0) { fprintf(stderr, "FAIL caller-owned unverified output changed\n"); ++failures; }
    expect_status("bad tag consumed", ano_crypto_aes256_gcm_stream_decrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("wrong-key decrypt init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_DECRYPT, wrong_key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    expect_status("wrong-key AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_OK);
    expect_status("wrong-key update", ano_crypto_aes256_gcm_stream_update(
        ctx, ciphertext, sizeof(ciphertext), output, sizeof(output), &written), ANO_CRYPTO_OK);
    expect_status("wrong-key final", ano_crypto_aes256_gcm_stream_decrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);

    expect_status("snapshot key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key_bytes, sizeof(key_bytes), 0u,
        &snapshot_key), ANO_CRYPTO_OK);
    expect_status("snapshot init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, snapshot_key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_OK);
    expect_status("snapshot handle destroy", ano_crypto_key_destroy(snapshot_key),
        ANO_CRYPTO_OK);
    expect_status("snapshot update after destroy", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, sizeof(output), &written), ANO_CRYPTO_OK);
    expect_status("snapshot final after destroy", ano_crypto_aes256_gcm_stream_encrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_status("stale snapshot handle init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, snapshot_key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    memset(ctx, 0xa5, ctx_size);
    expect_status("zero-length GCM uninit", ano_crypto_aes256_gcm_stream_uninit(ctx, 0u),
                  ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    for (i = 0u; i < ctx_size; ++i) {
        if (ctx[i] != 0xa5u) {
            fputs("FAIL zero-length GCM cleanup changed storage\n", stderr);
            ++failures;
            break;
        }
    }
    expect_status("short uninit", ano_crypto_aes256_gcm_stream_uninit(ctx, ctx_size - 1u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    if (!all_zero(ctx, ctx_size - 1u) || ctx[ctx_size - 1u] != 0xa5u) {
        fputs("FAIL GCM short cleanup extent\n", stderr);
        ++failures;
    }
    expect_status("full uninit", ano_crypto_aes256_gcm_stream_uninit(ctx, ctx_size), ANO_CRYPTO_OK);

    expect_status("shutdown-context init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        shutdown_nonce, sizeof(shutdown_nonce), 0u), ANO_CRYPTO_OK);
    expect_status("shutdown-final context init", ano_crypto_aes256_gcm_stream_init(
        final_ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        shutdown_final_nonce, sizeof(shutdown_final_nonce), 0u), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    memset(output, 0xa5, sizeof(output));
    written = 99u;
    expect_status("update after shutdown", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, sizeof(output), &written), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    if (written != 0u || !all_zero(output, sizeof(output))) {
        fputs("FAIL GCM shutdown update output clearing\n", stderr);
        ++failures;
    }
    memset(tag, 0xa5, sizeof(tag));
    expect_status("GCM final after shutdown", ano_crypto_aes256_gcm_stream_encrypt_final(
        final_ctx, tag, sizeof(tag)), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    if (!all_zero(final_ctx, ctx_size) || !all_zero(tag, sizeof(tag))) {
        fputs("FAIL GCM shutdown final did not clear context and tag\n", stderr);
        ++failures;
    }
    ctx[ctx_size] = 0xa5u;
    expect_status("cleanup after shutdown", ano_crypto_aes256_gcm_stream_uninit(ctx, ctx_size + 1u), ANO_CRYPTO_OK);
    if (!all_zero(ctx, ctx_size) || ctx[ctx_size] != 0xa5u) {
        fputs("FAIL GCM cleanup did not stay within the opaque extent\n", stderr);
        ++failures;
    }

    expect_status("module reinit", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    nonce[0] = 0xa5u;
    expect_status("GCM stale handle after reinit", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key,
        nonce, sizeof(nonce), 0u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("destroy key", ano_crypto_key_destroy(key), ANO_CRYPTO_OK);
    expect_status("destroy wrong key", ano_crypto_key_destroy(wrong_key), ANO_CRYPTO_OK);
    expect_status("fresh GCM key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key_bytes, sizeof(key_bytes), 0u, &key), ANO_CRYPTO_OK);
    expect_status("fresh GCM init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, key, nonce, sizeof(nonce), 0u),
        ANO_CRYPTO_OK);
    expect_status("fresh GCM update", ano_crypto_aes256_gcm_stream_update(
        ctx, input, sizeof(input), output, sizeof(output), &written), ANO_CRYPTO_OK);
    if (written != sizeof(input)) {
        fputs("FAIL GCM payload length\n", stderr);
        ++failures;
    }
    expect_status("fresh GCM final", ano_crypto_aes256_gcm_stream_encrypt_final(
        ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    if (!all_zero(ctx, ctx_size)) {
        fputs("FAIL GCM final did not clear context\n", stderr);
        ++failures;
    }
    expect_status("destroy fresh GCM key", ano_crypto_key_destroy(key), ANO_CRYPTO_OK);
    expect_status("final shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    free(ctx);
    free(final_ctx);
    free(misaligned_storage);

    if (failures != 0) {
        fprintf(stderr, "aes256_gcm_streaming_negative_tests: %d failure(s)\n", failures);
        return 1;
    }
    puts("aes256_gcm_streaming_negative_tests: PASS");
    return 0;
}
