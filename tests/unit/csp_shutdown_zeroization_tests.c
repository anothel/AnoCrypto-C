#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <anocrypto/ac_cmac.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_hmac.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t actual,
                          ano_crypto_status_t expected)
{
    if (actual != expected) {
        (void)fprintf(stderr, "FAIL %s: expected=%s actual=%s\n",
                      name, ano_crypto_status_name(expected),
                      ano_crypto_status_name(actual));
        ++g_failures;
    }
}

static void expect_state(const char *name, ano_crypto_csp_handle_t handle,
                         ano_crypto_csp_state_t expected)
{
    ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
    expect_status(name, ano_crypto_key_get_state(handle, &state), ANO_CRYPTO_OK);
    if (state != expected) {
        (void)fprintf(stderr, "FAIL %s: expected state=%s actual=%s\n",
                      name, ano_crypto_csp_state_name(expected),
                      ano_crypto_csp_state_name(state));
        ++g_failures;
    }
}

static void expect_invalid(const char *name, ano_crypto_csp_handle_t handle)
{
    int is_valid = 1;
    expect_status(name, ano_crypto_csp_handle_is_valid(handle, &is_valid),
                  ANO_CRYPTO_OK);
    if (is_valid != 0) {
        (void)fprintf(stderr, "FAIL %s: handle remained valid\n", name);
        ++g_failures;
    }
}

int main(void)
{
    static const uint8_t hmac_key[32] = {
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
        0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
        0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f
    };
    static const uint8_t aes_key[16] = {
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
        0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f
    };
    ano_crypto_csp_handle_t hmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t cipher_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t cmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("import HMAC",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                      hmac_key, sizeof(hmac_key),
                      ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE,
                      &hmac_handle),
                  ANO_CRYPTO_OK);
    expect_status("import cipher",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                      aes_key, sizeof(aes_key),
                      ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE,
                      &cipher_handle),
                  ANO_CRYPTO_OK);
    expect_status("import CMAC",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_CMAC,
                      aes_key, sizeof(aes_key),
                      ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE,
                      &cmac_handle),
                  ANO_CRYPTO_OK);

    expect_state("HMAC active", hmac_handle, ANO_CRYPTO_CSP_STATE_ACTIVE);
    expect_state("cipher active", cipher_handle, ANO_CRYPTO_CSP_STATE_ACTIVE);
    expect_state("CMAC active", cmac_handle, ANO_CRYPTO_CSP_STATE_ACTIVE);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    expect_state("HMAC zeroized", hmac_handle, ANO_CRYPTO_CSP_STATE_ZEROIZED);
    expect_state("cipher zeroized", cipher_handle, ANO_CRYPTO_CSP_STATE_ZEROIZED);
    expect_state("CMAC zeroized", cmac_handle, ANO_CRYPTO_CSP_STATE_ZEROIZED);
    expect_invalid("HMAC invalid after shutdown", hmac_handle);
    expect_invalid("cipher invalid after shutdown", cipher_handle);
    expect_invalid("CMAC invalid after shutdown", cmac_handle);

    expect_status("explicit destroy HMAC tombstone",
                  ano_crypto_key_destroy(hmac_handle), ANO_CRYPTO_OK);
    expect_status("explicit destroy cipher tombstone",
                  ano_crypto_key_destroy(cipher_handle), ANO_CRYPTO_OK);
    expect_status("explicit destroy CMAC tombstone",
                  ano_crypto_key_destroy(cmac_handle), ANO_CRYPTO_OK);
    expect_state("HMAC destroyed", hmac_handle, ANO_CRYPTO_CSP_STATE_DESTROYED);
    expect_state("cipher destroyed", cipher_handle, ANO_CRYPTO_CSP_STATE_DESTROYED);
    expect_state("CMAC destroyed", cmac_handle, ANO_CRYPTO_CSP_STATE_DESTROYED);

    expect_status("module reinit", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_invalid("old HMAC invalid after reinit", hmac_handle);
    expect_invalid("old cipher invalid after reinit", cipher_handle);
    expect_invalid("old CMAC invalid after reinit", cmac_handle);
    expect_status("final shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        (void)fprintf(stderr, "csp_shutdown_zeroization_tests: %d failure(s)\n",
                      g_failures);
        return 1;
    }

    (void)puts("csp_shutdown_zeroization_tests: PASS");
    return 0;
}
