#include <anocrypto/ac_module.h>
#include <anocrypto/ac_selftest.h>
#include "ac_internal.h"

static ano_crypto_module_state_t g_module_state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
static ano_crypto_status_t g_last_error = ANO_CRYPTO_OK;
static int g_fatal_latched = 0;

static void ano_crypto_set_state(ano_crypto_module_state_t state)
{
    g_module_state = state;
}

void ano_crypto_internal_module_mark_error(ano_crypto_status_t status)
{
    if (status == ANO_CRYPTO_OK) {
        status = ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    g_last_error = status;
    g_fatal_latched = 1;
    ano_crypto_set_state(ANO_CRYPTO_MODULE_STATE_ERROR_STATE);
}

ano_crypto_status_t ano_crypto_module_init(
    const ano_crypto_module_config_t *config)
{
    ano_crypto_status_t st;

    if (config != 0) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    if (g_module_state == ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        return ANO_CRYPTO_OK;
    }

    if (g_module_state == ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING) {
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    ano_crypto_set_state(ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING);

    st = ano_crypto_selftest_run(ANO_CRYPTO_SELFTEST_FLAG_STARTUP |
                                 ANO_CRYPTO_SELFTEST_FLAG_KAT_PLACEHOLDER |
                                 ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    g_last_error = ANO_CRYPTO_OK;
    ano_crypto_set_state(ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_shutdown(void)
{
    if (g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE || g_fatal_latched != 0) {
        /* Keep ERROR_STATE sticky. Shutdown is acknowledged but does not recover. */
        return ANO_CRYPTO_OK;
    }

    ano_crypto_set_state(ANO_CRYPTO_MODULE_STATE_SHUTDOWN);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_get_state(
    ano_crypto_module_state_t *out_state)
{
    if (out_state == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_state = g_module_state;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_get_last_error(
    ano_crypto_status_t *out_status)
{
    if (out_status == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_status = g_last_error;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_require_operational(void)
{
    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    if (g_module_state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        return ANO_CRYPTO_ERR_NOT_INITIALIZED;
    }

    return ANO_CRYPTO_OK;
}

const char *ano_crypto_module_state_name(ano_crypto_module_state_t state)
{
    switch (state) {
    case ANO_CRYPTO_MODULE_STATE_UNINITIALIZED:
        return "UNINITIALIZED";
    case ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING:
        return "SELF_TEST_RUNNING";
    case ANO_CRYPTO_MODULE_STATE_OPERATIONAL:
        return "OPERATIONAL";
    case ANO_CRYPTO_MODULE_STATE_ERROR_STATE:
        return "ERROR_STATE";
    case ANO_CRYPTO_MODULE_STATE_SHUTDOWN:
        return "SHUTDOWN";
    default:
        return "UNKNOWN_MODULE_STATE";
    }
}
