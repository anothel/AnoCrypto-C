#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static void fill_bytes(uint8_t *out, size_t out_len, uint8_t start)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(start + (uint8_t)i);
    }
}

int main(void)
{
    uint8_t key[32];
    uint8_t oversized_key[ANO_CRYPTO_KEY_MAX_SECRET_BYTES + 1u];
    uint8_t msg[3] = { 'a', 'b', 'c' };
    uint8_t mac[32];
    size_t written = 0u;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t cleanup_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
    int is_valid = 99;

    fill_bytes(key, sizeof(key), 0x20u);
    fill_bytes(oversized_key, sizeof(oversized_key), 0x80u);

    expect_status("import before module init", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_ERR_NOT_INITIALIZED);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    expect_status("handle valid null output", ano_crypto_csp_handle_is_valid(
                      ANO_CRYPTO_CSP_INVALID_HANDLE, 0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("invalid handle is not valid", ano_crypto_csp_handle_is_valid(
                      ANO_CRYPTO_CSP_INVALID_HANDLE, &is_valid), ANO_CRYPTO_OK);
    if (is_valid != 0) {
        fprintf(stderr, "FAIL invalid handle reported valid\n");
        ++g_failures;
    }
    expect_status("invalid handle state", ano_crypto_key_get_state(
                      ANO_CRYPTO_CSP_INVALID_HANDLE, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_INVALID) {
        fprintf(stderr, "FAIL invalid state query returned %s\n", ano_crypto_csp_state_name(state));
        ++g_failures;
    }

    expect_status("import null output", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, 0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("import null key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, 0, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("import zero-length key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, 0u,
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("import oversized key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, oversized_key, sizeof(oversized_key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("import exportable flag rejected", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE, &handle),
                  ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("import unknown flag rejected", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      0x80000000u, &handle), ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    expect_status("import reserved key type", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_SECRET_BYTES, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_ERR_NOT_IMPLEMENTED);

    expect_status("destroy invalid handle", ano_crypto_key_destroy(
                      ANO_CRYPTO_CSP_INVALID_HANDLE), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("import valid hmac key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &handle), ANO_CRYPTO_OK);
    expect_status("hmac with handle short out", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256, handle,
                      msg, sizeof(msg), mac, 31u, &written), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_status("hmac with invalid handle", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256, ANO_CRYPTO_CSP_INVALID_HANDLE,
                      msg, sizeof(msg), mac, sizeof(mac), &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("destroy valid key", ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
    expect_status("destroy same key twice", ano_crypto_key_destroy(handle), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("import cleanup key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC, key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE, &cleanup_handle), ANO_CRYPTO_OK);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    expect_status("hmac after shutdown blocked", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256, cleanup_handle,
                      msg, sizeof(msg), mac, sizeof(mac), &written), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_status("destroy after shutdown allowed", ano_crypto_key_destroy(cleanup_handle), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "csp_key_lifecycle_negative_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("csp_key_lifecycle_negative_tests: PASS");
    return 0;
}
