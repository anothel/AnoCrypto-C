#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>
#include <anocrypto/ac_status.h>
#include "ac_internal.h"

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

int main(void)
{
    static const uint8_t key[32] = {
        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
        0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
        0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f
    };
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
    ano_crypto_module_state_t module_state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
    int is_valid = 1;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("import HMAC",
                  ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_HMAC,
                      key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE,
                      &handle),
                  ANO_CRYPTO_OK);

    ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED);

    expect_status("module state", ano_crypto_module_get_state(&module_state),
                  ANO_CRYPTO_OK);
    if (module_state != ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        (void)fprintf(stderr, "FAIL module did not enter ERROR_STATE\n");
        ++g_failures;
    }
    expect_status("CSP state", ano_crypto_key_get_state(handle, &state),
                  ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_ZEROIZED) {
        (void)fprintf(stderr, "FAIL CSP expected ZEROIZED actual=%s\n",
                      ano_crypto_csp_state_name(state));
        ++g_failures;
    }
    expect_status("CSP validity", ano_crypto_csp_handle_is_valid(handle, &is_valid),
                  ANO_CRYPTO_OK);
    if (is_valid != 0) {
        (void)fprintf(stderr, "FAIL fatal-transition handle remained valid\n");
        ++g_failures;
    }

    expect_status("destroy zeroized tombstone", ano_crypto_key_destroy(handle),
                  ANO_CRYPTO_OK);
    expect_status("module init remains fatal", ano_crypto_module_init(0),
                  ANO_CRYPTO_ERR_FATAL_STATE);
    expect_status("fatal shutdown acknowledged", ano_crypto_module_shutdown(),
                  ANO_CRYPTO_OK);
    expect_status("module still fatal", ano_crypto_module_init(0),
                  ANO_CRYPTO_ERR_FATAL_STATE);

    if (g_failures != 0) {
        (void)fprintf(stderr, "csp_fatal_zeroization_tests: %d failure(s)\n",
                      g_failures);
        return 1;
    }

    (void)puts("csp_fatal_zeroization_tests: PASS");
    return 0;
}
