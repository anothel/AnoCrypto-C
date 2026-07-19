#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
    for (i = 0u; i < len; ++i) diff = (uint8_t)(diff | (uint8_t)(got[i] ^ expected[i]));
    if (diff != 0u) { fprintf(stderr, "FAIL %s: byte mismatch\n", name); ++failures; }
}


static void expect_zeroed(const char *name, const uint8_t *buffer, size_t len)
{
    size_t i;
    for (i = 0u; i < len; ++i) {
        if (buffer[i] != 0u) {
            fprintf(stderr, "FAIL %s: byte %zu not zeroized\n", name, i);
            ++failures;
            return;
        }
    }
}

static void run_vector(const char *name, const uint8_t *password, size_t password_len,
                       const uint8_t *salt, size_t salt_len, const uint8_t expected[32], void *workspace)
{
    uint8_t out[32];
    memset(out, 0xa5, sizeof(out));
    memset(workspace, 0xa5, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    expect_status(name, ano_crypto_scrypt_derive_key(password, password_len, salt, salt_len,
                  workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE, out), ANO_CRYPTO_OK);
    expect_bytes(name, out, expected, sizeof(out));
    expect_zeroed(name, (const uint8_t *)workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
}

static void test_vectors(void *workspace)
{
    static const uint8_t expected_empty[32] = {
        0x2c,0x80,0x4b,0xf5,0xd0,0xcc,0xb2,0x80,0x51,0xf2,0x0f,0xb0,0xb9,0x9b,0xf7,0xe9,
        0xa4,0x5a,0x67,0x49,0x66,0x50,0x53,0x60,0x18,0xdd,0x88,0x39,0xd9,0x37,0x07,0x48};
    static const uint8_t expected_password[32] = {
        0x3f,0xa8,0x51,0x18,0xff,0xfe,0x97,0x32,0xda,0x7f,0x4c,0x63,0xef,0x61,0xf5,0xc6,
        0x45,0x81,0x44,0xd0,0x36,0x40,0x1f,0x88,0x29,0xdd,0x4e,0x5b,0x2c,0x50,0xc5,0x1f};
    static const uint8_t skp1_salt[32] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
    static const uint8_t expected_skp1[32] = {
        0xb3,0xfb,0xb7,0xc9,0xf7,0xc9,0xa4,0x41,0x38,0xec,0x43,0x85,0xae,0xcd,0x45,0x9d,
        0xed,0xe3,0x21,0xf5,0x97,0xeb,0xa9,0x2e,0x7e,0x0b,0x81,0x38,0xcf,0x4a,0xd3,0xa0};
    static const uint8_t password[] = "password";
    static const uint8_t nacl[] = "NaCl";
    static const uint8_t skp1_password[] = "known SKP1 password";

    run_vector("empty password/salt", 0, 0u, 0, 0u, expected_empty, workspace);
    run_vector("password NaCl", password, sizeof(password)-1u, nacl, sizeof(nacl)-1u, expected_password, workspace);
    run_vector("SKP1 fixed profile", skp1_password, sizeof(skp1_password)-1u,
               skp1_salt, sizeof(skp1_salt), expected_skp1, workspace);
}

static void test_exact_skp1_compatibility(void *workspace)
{
    static const uint8_t header[64] = {
        0x53,0x4b,0x50,0x31,0x01,0x01,0x01,0x00,0x00,0x10,0x00,0x00,
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x01};
    static const uint8_t record_header[9] = {0,0,0,0,0,0,0,0x11,1};
    static const uint8_t ciphertext[17] = {0x8c,0x37,0xd6,0xaf,0x06,0xe9,0x10,0x22,0x19,0x83,0x4f,0xca,0xc4,0x3a,0x8a,0x4b,0x5b};
    static const uint8_t tag[16] = {0xd9,0xa6,0xb9,0xcc,0x65,0x8c,0x9e,0x3d,0x7b,0xb9,0xa6,0x28,0x07,0x9f,0xf4,0xe3};
    static const uint8_t password[] = "known SKP1 password";
    static const uint8_t caller_aad[] = "fixture:aad";
    static const uint8_t expected_plaintext[] = "known SKP1 vector";
    uint8_t aad[64+9+11];
    uint8_t key[32];
    uint8_t nonce[12];
    uint8_t plaintext[17];
    size_t written = 0u;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    memcpy(aad, header, 64u); memcpy(aad+64u, record_header, 9u); memcpy(aad+73u, caller_aad, 11u);
    memcpy(nonce, header+44u, 8u); memcpy(nonce+8u, record_header, 4u);
    memset(workspace, 0xa5, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    expect_status("SKP1 derive", ano_crypto_scrypt_derive_key(password, sizeof(password)-1u,
                  header+12u, 32u, workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE, key), ANO_CRYPTO_OK);
    expect_zeroed("SKP1 workspace", (const uint8_t *)workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    expect_status("SKP1 import", ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                  key, sizeof(key), ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_OK);
    expect_status("SKP1 decrypt", ano_crypto_aes256_gcm_decrypt(handle, nonce, sizeof(nonce), aad, sizeof(aad),
                  ciphertext, sizeof(ciphertext), tag, sizeof(tag), plaintext, sizeof(plaintext), &written), ANO_CRYPTO_OK);
    if (written != sizeof(plaintext)) { fprintf(stderr, "FAIL SKP1 written\n"); ++failures; }
    expect_bytes("SKP1 plaintext", plaintext, expected_plaintext, sizeof(plaintext));
    expect_status("SKP1 destroy", ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
    (void)ano_crypto_csp_zeroize(key, sizeof(key));
    (void)ano_crypto_csp_zeroize(plaintext, sizeof(plaintext));
}

int main(void)
{
    void *workspace = malloc(ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    if (workspace == 0) { fputs("FAIL workspace allocation\n", stderr); return 1; }
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_vectors(workspace);
    test_exact_skp1_compatibility(workspace);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    free(workspace);
    if (failures != 0) { fprintf(stderr, "scrypt_kat_tests: %d failure(s)\n", failures); return 1; }
    puts("scrypt_kat_tests: PASS");
    return 0;
}
