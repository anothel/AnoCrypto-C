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

static void expect_bytes(const char *name, const uint8_t *got, const uint8_t *expected, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(got[i] ^ expected[i]));
    }
    if (diff != 0u) {
        fprintf(stderr, "FAIL %s: byte mismatch\n", name);
        ++failures;
    }
}

static ano_crypto_csp_handle_t import_key(const uint8_t key[32])
{
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    expect_status("key import", ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, key, 32u,
        ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_OK);
    return handle;
}

static void *new_context(size_t *out_size)
{
    void *ctx;
    expect_status("context size", ano_crypto_aes256_gcm_stream_context_size(out_size), ANO_CRYPTO_OK);
    ctx = calloc(1u, *out_size);
    if (ctx == 0) {
        fprintf(stderr, "FAIL context allocation\n");
        ++failures;
    }
    return ctx;
}

static void test_nist_chunk_splits(void)
{
    static const uint8_t key[32] = {
        0xfeu,0xffu,0xe9u,0x92u,0x86u,0x65u,0x73u,0x1cu,0x6du,0x6au,0x8fu,0x94u,0x67u,0x30u,0x83u,0x08u,
        0xfeu,0xffu,0xe9u,0x92u,0x86u,0x65u,0x73u,0x1cu,0x6du,0x6au,0x8fu,0x94u,0x67u,0x30u,0x83u,0x08u};
    static const uint8_t nonce[12] = {0xcau,0xfeu,0xbau,0xbeu,0xfau,0xceu,0xdbu,0xadu,0xdeu,0xcau,0xf8u,0x88u};
    static const uint8_t aad[20] = {0xfeu,0xedu,0xfau,0xceu,0xdeu,0xadu,0xbeu,0xefu,0xfeu,0xedu,0xfau,0xceu,0xdeu,0xadu,0xbeu,0xefu,0xabu,0xadu,0xdau,0xd2u};
    static const uint8_t plaintext[60] = {
        0xd9u,0x31u,0x32u,0x25u,0xf8u,0x84u,0x06u,0xe5u,0xa5u,0x59u,0x09u,0xc5u,0xafu,0xf5u,0x26u,0x9au,
        0x86u,0xa7u,0xa9u,0x53u,0x15u,0x34u,0xf7u,0xdau,0x2eu,0x4cu,0x30u,0x3du,0x8au,0x31u,0x8au,0x72u,
        0x1cu,0x3cu,0x0cu,0x95u,0x95u,0x68u,0x09u,0x53u,0x2fu,0xcfu,0x0eu,0x24u,0x49u,0xa6u,0xb5u,0x25u,
        0xb1u,0x6au,0xedu,0xf5u,0xaau,0x0du,0xe6u,0x57u,0xbau,0x63u,0x7bu,0x39u};
    static const uint8_t expected_ciphertext[60] = {
        0x52u,0x2du,0xc1u,0xf0u,0x99u,0x56u,0x7du,0x07u,0xf4u,0x7fu,0x37u,0xa3u,0x2au,0x84u,0x42u,0x7du,
        0x64u,0x3au,0x8cu,0xdcu,0xbfu,0xe5u,0xc0u,0xc9u,0x75u,0x98u,0xa2u,0xbdu,0x25u,0x55u,0xd1u,0xaau,
        0x8cu,0xb0u,0x8eu,0x48u,0x59u,0x0du,0xbbu,0x3du,0xa7u,0xb0u,0x8bu,0x10u,0x56u,0x82u,0x88u,0x38u,
        0xc5u,0xf6u,0x1eu,0x63u,0x93u,0xbau,0x7au,0x0au,0xbcu,0xc9u,0xf6u,0x62u};
    static const uint8_t expected_tag[16] = {0x76u,0xfcu,0x6eu,0xceu,0x0fu,0x4eu,0x17u,0x68u,0xcdu,0xdfu,0x88u,0x53u,0xbbu,0x2du,0x55u,0x1bu};
    static const size_t splits[] = {1u, 15u, 17u, 27u};
    ano_crypto_csp_handle_t handle = import_key(key);
    size_t ctx_size = 0u;
    void *ctx = new_context(&ctx_size);
    uint8_t ciphertext[60];
    uint8_t recovered[60];
    uint8_t tag[16];
    size_t offset = 0u;
    size_t i;
    size_t written = 0u;

    if (ctx == 0) return;
    expect_status("stream encrypt init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, handle,
        nonce, sizeof(nonce), ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("AAD part 1", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, 3u), ANO_CRYPTO_OK);
    expect_status("AAD zero", ano_crypto_aes256_gcm_stream_aad_update(ctx, 0, 0u), ANO_CRYPTO_OK);
    expect_status("AAD part 2", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad + 3u, sizeof(aad) - 3u), ANO_CRYPTO_OK);
    for (i = 0u; i < sizeof(splits) / sizeof(splits[0]); ++i) {
        expect_status("encrypt update", ano_crypto_aes256_gcm_stream_update(
            ctx, plaintext + offset, splits[i], ciphertext + offset, splits[i], &written), ANO_CRYPTO_OK);
        if (written != splits[i]) { fprintf(stderr, "FAIL encrypt written\n"); ++failures; }
        offset += splits[i];
    }
    expect_status("encrypt final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_bytes("stream ciphertext", ciphertext, expected_ciphertext, sizeof(ciphertext));
    expect_bytes("stream tag", tag, expected_tag, sizeof(tag));

    expect_status("stream decrypt init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_DECRYPT, handle,
        nonce, sizeof(nonce), ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("decrypt AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_OK);
    offset = 0u;
    for (i = sizeof(splits) / sizeof(splits[0]); i > 0u; --i) {
        size_t take = splits[i - 1u];
        expect_status("decrypt update", ano_crypto_aes256_gcm_stream_update(
            ctx, ciphertext + offset, take, recovered + offset, take, &written), ANO_CRYPTO_OK);
        offset += take;
    }
    expect_status("decrypt final", ano_crypto_aes256_gcm_stream_decrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_bytes("stream plaintext", recovered, plaintext, sizeof(plaintext));

    memcpy(recovered, plaintext, sizeof(recovered));
    expect_status("in-place init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, handle,
        nonce, sizeof(nonce), ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("in-place AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, aad, sizeof(aad)), ANO_CRYPTO_OK);
    expect_status("in-place update 1", ano_crypto_aes256_gcm_stream_update(ctx, recovered, 19u, recovered, 19u, &written), ANO_CRYPTO_OK);
    expect_status("in-place update 2", ano_crypto_aes256_gcm_stream_update(ctx, recovered + 19u, 41u, recovered + 19u, 41u, &written), ANO_CRYPTO_OK);
    expect_status("in-place final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_bytes("in-place stream ciphertext", recovered, expected_ciphertext, sizeof(recovered));

    expect_status("context uninit", ano_crypto_aes256_gcm_stream_uninit(ctx, ctx_size), ANO_CRYPTO_OK);
    free(ctx);
    expect_status("key destroy", ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
}

static void test_zero_update_and_skt1(void)
{
    static const uint8_t zero_key[32] = {0u};
    static const uint8_t skt1[51] = {
        0x53u,0x4bu,0x54u,0x31u,0x01u,0x45u,0x46u,0x8au,0x05u,0x5fu,0xc8u,0xcbu,0xb1u,0x35u,0x04u,0x08u,0x8eu,
        0xcbu,0xbbu,0x69u,0x3cu,0x58u,0x77u,0xb1u,0x56u,0x66u,0x7eu,0x61u,0x43u,0x79u,0xb7u,0x04u,0x59u,0x0bu,
        0x7au,0x96u,0x4fu,0x7eu,0x72u,0xc6u,0xc3u,0x80u,0x86u,0xceu,0x83u,0x84u,0x17u,0x87u,0xc8u,0x16u,0x07u};
    static const uint8_t plaintext[18] = "hello anosecurekit";
    static const uint8_t caller_aad[9] = "record:v1";
    ano_crypto_csp_handle_t handle = import_key(zero_key);
    size_t ctx_size = 0u;
    void *ctx = new_context(&ctx_size);
    uint8_t ciphertext[18];
    uint8_t tag[16];
    uint8_t empty_tag[16];
    uint8_t expected_empty_tag[16];
    const uint8_t aad_a = (uint8_t)'a';
    const uint8_t aad_b = (uint8_t)'b';
    const uint8_t aad_ab[2] = {(uint8_t)'a', (uint8_t)'b'};
    size_t written = 99u;

    if (ctx == 0) return;
    expect_status("zero init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, handle,
        skt1 + 5u, 12u, ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("zero AAD a", ano_crypto_aes256_gcm_stream_aad_update(ctx, &aad_a, 1u), ANO_CRYPTO_OK);
    expect_status("zero payload", ano_crypto_aes256_gcm_stream_update(ctx, 0, 0u, 0, 0u, &written), ANO_CRYPTO_OK);
    if (written != 0u) { fprintf(stderr, "FAIL zero update length\n"); ++failures; }
    expect_status("AAD after zero payload", ano_crypto_aes256_gcm_stream_aad_update(ctx, &aad_b, 1u), ANO_CRYPTO_OK);
    expect_status("zero final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, empty_tag, sizeof(empty_tag)), ANO_CRYPTO_OK);
    expect_status("empty one-shot", ano_crypto_aes256_gcm_encrypt(
        handle, skt1 + 5u, 12u, aad_ab, sizeof(aad_ab), 0, 0u,
        0, 0u, &written, expected_empty_tag, sizeof(expected_empty_tag)), ANO_CRYPTO_OK);
    expect_bytes("zero update tag", empty_tag, expected_empty_tag, sizeof(empty_tag));

    expect_status("SKT1 init", ano_crypto_aes256_gcm_stream_init(
        ctx, ctx_size, ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT, handle,
        skt1 + 5u, 12u, ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE), ANO_CRYPTO_OK);
    expect_status("SKT1 header AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, skt1, 5u), ANO_CRYPTO_OK);
    expect_status("SKT1 caller AAD", ano_crypto_aes256_gcm_stream_aad_update(ctx, caller_aad, sizeof(caller_aad)), ANO_CRYPTO_OK);
    expect_status("SKT1 payload 1", ano_crypto_aes256_gcm_stream_update(ctx, plaintext, 5u, ciphertext, 5u, &written), ANO_CRYPTO_OK);
    expect_status("SKT1 payload 2", ano_crypto_aes256_gcm_stream_update(ctx, plaintext + 5u, 13u, ciphertext + 5u, 13u, &written), ANO_CRYPTO_OK);
    expect_status("SKT1 final", ano_crypto_aes256_gcm_stream_encrypt_final(ctx, tag, sizeof(tag)), ANO_CRYPTO_OK);
    expect_bytes("SKT1 ciphertext", ciphertext, skt1 + 17u, sizeof(ciphertext));
    expect_bytes("SKT1 tag", tag, skt1 + 35u, sizeof(tag));

    free(ctx);
    expect_status("key destroy", ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
}

int main(void)
{
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_nist_chunk_splits();
    test_zero_update_and_skt1();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    if (failures != 0) {
        fprintf(stderr, "aes256_gcm_streaming_tests: %d failure(s)\n", failures);
        return 1;
    }
    puts("aes256_gcm_streaming_tests: PASS");
    return 0;
}
