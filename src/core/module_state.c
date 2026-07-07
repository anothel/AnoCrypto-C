#include <anocrypto/ac_module.h>
#include <anocrypto/ac_selftest.h>
#include "ac_internal.h"
#include "ac_thread_internal.h"

static ano_crypto_module_state_t g_module_state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
static ano_crypto_status_t g_last_error = ANO_CRYPTO_OK;
static int g_fatal_latched = 0;

static void ano_crypto_set_state_unlocked(ano_crypto_module_state_t state)
{
    g_module_state = state;
}

static void ano_crypto_mark_error_unlocked(ano_crypto_status_t status)
{
    if (status == ANO_CRYPTO_OK) {
        status = ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    g_last_error = status;
    g_fatal_latched = 1;
    ano_crypto_set_state_unlocked(ANO_CRYPTO_MODULE_STATE_ERROR_STATE);
}

void ano_crypto_internal_module_mark_error(ano_crypto_status_t status)
{
    ano_crypto_status_t lock_status;

    lock_status = ano_crypto_internal_global_lock();
    if (lock_status == ANO_CRYPTO_OK) {
        ano_crypto_mark_error_unlocked(status);
        ano_crypto_internal_global_unlock();
        return;
    }

    /* Last-resort sticky error recording if the lock itself fails. */
    ano_crypto_mark_error_unlocked(status);
}

ano_crypto_status_t ano_crypto_module_init(
    const ano_crypto_module_config_t *config)
{
    ano_crypto_status_t st;

    if (config != 0) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    if (g_module_state == ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_OK;
    }

    if (g_module_state == ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    ano_crypto_set_state_unlocked(ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING);
    ano_crypto_internal_global_unlock();

    st = ano_crypto_selftest_run(ANO_CRYPTO_SELFTEST_FLAG_STARTUP |
                                 ANO_CRYPTO_SELFTEST_FLAG_KAT_PLACEHOLDER |
                                 ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER);

    if (ano_crypto_internal_global_lock() != ANO_CRYPTO_OK) {
        if (st == ANO_CRYPTO_OK) {
            return ANO_CRYPTO_ERR_INTERNAL_ERROR;
        }
        return st;
    }

    if (st != ANO_CRYPTO_OK) {
        if (g_module_state != ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
            ano_crypto_mark_error_unlocked(st);
        }
        ano_crypto_internal_global_unlock();
        return st;
    }

    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    g_last_error = ANO_CRYPTO_OK;
    ano_crypto_set_state_unlocked(ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_shutdown(void)
{
    ano_crypto_status_t st;

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE || g_fatal_latched != 0) {
        /* Keep ERROR_STATE sticky. Shutdown is acknowledged but does not recover. */
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_OK;
    }

    ano_crypto_set_state_unlocked(ANO_CRYPTO_MODULE_STATE_SHUTDOWN);
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_get_state(
    ano_crypto_module_state_t *out_state)
{
    ano_crypto_status_t st;

    if (out_state == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_state = g_module_state;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_get_last_error(
    ano_crypto_status_t *out_status)
{
    ano_crypto_status_t st;

    if (out_status == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_status = g_last_error;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_module_require_operational(void)
{
    ano_crypto_status_t st;
    ano_crypto_status_t result;

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        result = ANO_CRYPTO_ERR_FATAL_STATE;
    } else if (g_module_state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        result = ANO_CRYPTO_ERR_NOT_INITIALIZED;
    } else {
        result = ANO_CRYPTO_OK;
    }

    ano_crypto_internal_global_unlock();
    return result;
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
