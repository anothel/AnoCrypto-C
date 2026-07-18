#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;
static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) { fprintf(stderr, "FAIL %s: got %s expected %s\n", name, ano_crypto_status_name(got), ano_crypto_status_name(expected)); ++failures; }
}
static void expect_zero(const char *name, const uint8_t *p, size_t n)
{
    size_t i; for (i = 0u; i < n; ++i) if (p[i] != 0u) { fprintf(stderr, "FAIL %s: uncleared byte %lu\n", name, (unsigned long)i); ++failures; return; }
}
static ano_crypto_csp_handle_t import_key(ano_crypto_key_type_t type, size_t len, uint8_t seed)
{
    uint8_t key[32]; size_t i; ano_crypto_csp_handle_t h = ANO_CRYPTO_CSP_INVALID_HANDLE;
    for (i = 0u; i < sizeof(key); ++i) key[i] = (uint8_t)(seed + i);
    expect_status("key import", ano_crypto_key_import_secret(type, key, len, ANO_CRYPTO_KEY_FLAG_NONE, &h), ANO_CRYPTO_OK);
    return h;
}

int main(void)
{
    uint8_t nonce[12] = {0u}; uint8_t aad[16] = {0u}; uint8_t input[64]; uint8_t output[64]; uint8_t tag[16];
    uint8_t good_ct[32]; uint8_t good_tag[16]; size_t written = 0u;
    ano_crypto_csp_handle_t key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t wrong_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t short_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t hmac_key = ANO_CRYPTO_CSP_INVALID_HANDLE;

    memset(input, 0x11, sizeof(input)); memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("before init", ano_crypto_aes256_gcm_encrypt(1u, nonce, 12u, aad, 0u, input, 32u, output, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_zero("before init ciphertext", output, 32u); expect_zero("before init tag", tag, 16u);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    key = import_key(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, 32u, 0x10u);
    wrong_key = import_key(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, 32u, 0x20u);
    short_key = import_key(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER, 16u, 0x30u);
    hmac_key = import_key(ANO_CRYPTO_KEY_TYPE_HMAC, 32u, 0x40u);

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag)); written = 99u;
    expect_status("short output", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, aad, sizeof(aad), input, 32u, output, 15u, &written, tag, 16u), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_zero("short output clear", output, 15u); expect_zero("short output tag clear", tag, 16u); if (written != 0u) ++failures;

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("nonce length", ano_crypto_aes256_gcm_encrypt(key, nonce, 11u, 0, 0u, input, 32u, output, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("nonce output clear", output, 32u); expect_zero("nonce tag clear", tag, 16u);

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("tag length", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, 0, 0u, input, 32u, output, 32u, &written, tag, 15u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("tag length output clear", output, 32u); expect_zero("tag length clear", tag, 15u);

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("NULL AAD", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, 0, 1u, input, 32u, output, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("NULL AAD output clear", output, 32u); expect_zero("NULL AAD tag clear", tag, 16u);

    memset(input, 0x33, sizeof(input)); memset(tag, 0xa5, sizeof(tag));
    expect_status("partial overlap", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, 0, 0u, input, 32u, input + 1u, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("partial overlap output clear", input + 1u, 32u); expect_zero("partial overlap tag clear", tag, 16u);

    memset(output, 0xa5, sizeof(output));
    expect_status("tag output overlap", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, 0, 0u, input, 32u, output, 32u, &written, output + 16u, 16u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("tag overlap output clear", output, 32u);

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("short AES key", ano_crypto_aes256_gcm_encrypt(short_key, nonce, 12u, 0, 0u, input, 32u, output, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_zero("short key output clear", output, 32u); expect_zero("short key tag clear", tag, 16u);

    memset(output, 0xa5, sizeof(output)); memset(tag, 0xa5, sizeof(tag));
    expect_status("wrong key type", ano_crypto_aes256_gcm_encrypt(hmac_key, nonce, 12u, 0, 0u, input, 32u, output, 32u, &written, tag, 16u), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_zero("wrong type output clear", output, 32u); expect_zero("wrong type tag clear", tag, 16u);

    expect_status("make valid ciphertext", ano_crypto_aes256_gcm_encrypt(key, nonce, 12u, aad, sizeof(aad), input, 32u, good_ct, sizeof(good_ct), &written, good_tag, sizeof(good_tag)), ANO_CRYPTO_OK);

    memset(output, 0xa5, sizeof(output)); tag[0] = good_tag[0] ^ 1u; memcpy(tag + 1u, good_tag + 1u, 15u);
    expect_status("bad tag", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), good_ct, sizeof(good_ct), tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("bad tag output clear", output, 32u); if (written != 0u) ++failures;

    memset(output, 0xa5, sizeof(output));
    expect_status("bad key", ano_crypto_aes256_gcm_decrypt(wrong_key, nonce, 12u, aad, sizeof(aad), good_ct, sizeof(good_ct), good_tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("bad key output clear", output, 32u);

    memset(output, 0xa5, sizeof(output)); nonce[0] ^= 1u;
    expect_status("bad nonce", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), good_ct, sizeof(good_ct), good_tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("bad nonce output clear", output, 32u); nonce[0] ^= 1u;

    memset(output, 0xa5, sizeof(output)); aad[0] ^= 1u;
    expect_status("bad AAD", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), good_ct, sizeof(good_ct), good_tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("bad AAD output clear", output, 32u); aad[0] ^= 1u;

    good_ct[0] ^= 1u; memset(output, 0xa5, sizeof(output));
    expect_status("bad ciphertext", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), good_ct, sizeof(good_ct), good_tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("bad ciphertext output clear", output, 32u); good_ct[0] ^= 1u;

    memcpy(output, good_ct, sizeof(good_ct)); tag[0] = good_tag[0] ^ 1u; memcpy(tag + 1u, good_tag + 1u, 15u);
    expect_status("in-place auth failure", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), output, 32u, tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    expect_zero("in-place auth failure clear", output, 32u);

    expect_status("destroy key", ano_crypto_key_destroy(key), ANO_CRYPTO_OK);
    expect_status("destroy wrong key", ano_crypto_key_destroy(wrong_key), ANO_CRYPTO_OK);
    expect_status("destroy short key", ano_crypto_key_destroy(short_key), ANO_CRYPTO_OK);
    expect_status("destroy HMAC key", ano_crypto_key_destroy(hmac_key), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    memset(output, 0xa5, sizeof(output));
    expect_status("after shutdown", ano_crypto_aes256_gcm_decrypt(key, nonce, 12u, aad, sizeof(aad), good_ct, 32u, good_tag, 16u, output, 32u, &written), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_zero("after shutdown clear", output, 32u);

    if (failures != 0) { fprintf(stderr, "aes256_gcm_api_negative_tests: %d failure(s)\n", failures); return 1; }
    puts("aes256_gcm_api_negative_tests: PASS"); return 0;
}
