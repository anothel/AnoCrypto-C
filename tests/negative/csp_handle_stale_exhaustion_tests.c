#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

#define P1_TEST_EXPECTED_CSP_CAPACITY 16u

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static void fill_key(uint8_t *out, size_t out_len, uint8_t seed)
{
    size_t i;

    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(seed + (uint8_t)(i * 3u));
    }
}

int main(void)
{
    ano_crypto_csp_handle_t handles[P1_TEST_EXPECTED_CSP_CAPACITY];
    ano_crypto_csp_handle_t extra = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t stale = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t replacement = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
    uint8_t key[32];
    uint8_t msg[3] = { 'a', 'b', 'c' };
    uint8_t mac[32];
    size_t written = 0u;
    int is_valid = 0;
    size_t i;

    for (i = 0u; i < P1_TEST_EXPECTED_CSP_CAPACITY; ++i) {
        handles[i] = ANO_CRYPTO_CSP_INVALID_HANDLE;
    }

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    for (i = 0u; i < P1_TEST_EXPECTED_CSP_CAPACITY; ++i) {
        fill_key(key, sizeof(key), (uint8_t)(0x30u + (uint8_t)i));
        expect_status("fill csp table", ano_crypto_key_import_secret(
                          ANO_CRYPTO_KEY_TYPE_HMAC,
                          key, sizeof(key),
                          ANO_CRYPTO_KEY_FLAG_NONE,
                          &handles[i]), ANO_CRYPTO_OK);
        if (handles[i] == ANO_CRYPTO_CSP_INVALID_HANDLE) {
            fprintf(stderr, "FAIL fill csp table returned invalid handle at index %lu\n",
                    (unsigned long)i);
            ++g_failures;
        }
    }

    fill_key(key, sizeof(key), 0x80u);
    expect_status("csp table exhaustion", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC,
                      key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE,
                      &extra), ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED);
    if (extra != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        fprintf(stderr, "FAIL exhausted import wrote non-invalid handle\n");
        ++g_failures;
    }

    stale = handles[0];
    expect_status("destroy first handle", ano_crypto_key_destroy(stale), ANO_CRYPTO_OK);
    expect_status("destroyed state before reuse", ano_crypto_key_get_state(stale, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_DESTROYED) {
        fprintf(stderr, "FAIL destroyed handle state before reuse: got %s expected DESTROYED\n",
                ano_crypto_csp_state_name(state));
        ++g_failures;
    }

    fill_key(key, sizeof(key), 0x90u);
    expect_status("replacement import after one free", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC,
                      key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_NONE,
                      &replacement), ANO_CRYPTO_OK);
    if (replacement == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        fprintf(stderr, "FAIL replacement import returned invalid handle\n");
        ++g_failures;
    }
    if (replacement == stale) {
        fprintf(stderr, "FAIL replacement handle reused exact stale handle value\n");
        ++g_failures;
    }

    expect_status("stale handle invalid query", ano_crypto_csp_handle_is_valid(stale, &is_valid), ANO_CRYPTO_OK);
    if (is_valid != 0) {
        fprintf(stderr, "FAIL stale handle reported valid after slot reuse\n");
        ++g_failures;
    }

    expect_status("stale handle state after reuse", ano_crypto_key_get_state(stale, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_INVALID) {
        fprintf(stderr, "FAIL stale handle state after reuse: got %s expected INVALID\n",
                ano_crypto_csp_state_name(state));
        ++g_failures;
    }

    expect_status("stale handle hmac rejected", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256,
                      stale,
                      msg, sizeof(msg),
                      mac, sizeof(mac),
                      &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("stale handle destroy rejected", ano_crypto_key_destroy(stale),
                  ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("replacement handle valid query", ano_crypto_csp_handle_is_valid(replacement, &is_valid),
                  ANO_CRYPTO_OK);
    if (is_valid != 1) {
        fprintf(stderr, "FAIL replacement handle not valid\n");
        ++g_failures;
    }

    expect_status("destroy replacement handle", ano_crypto_key_destroy(replacement), ANO_CRYPTO_OK);

    for (i = 1u; i < P1_TEST_EXPECTED_CSP_CAPACITY; ++i) {
        expect_status("destroy remaining handle", ano_crypto_key_destroy(handles[i]), ANO_CRYPTO_OK);
    }

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "csp_handle_stale_exhaustion_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("csp_handle_stale_exhaustion_tests: PASS");
    return 0;
}
