#include <anocrypto/ac_module.h>
#include <anocrypto/ac_selftest.h>
#include "ac_internal.h"
#include "ac_csp_internal.h"
#include "ac_thread_internal.h"

static ano_crypto_module_state_t g_module_state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
static ano_crypto_status_t g_last_error = ANO_CRYPTO_OK;
static int g_fatal_latched = 0;
/* Protected by the global lock. Never reuse an initialization identity: state
 * alone cannot distinguish an old startup from a new startup after shutdown. */
static uint64_t g_init_generation = 0u;

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
        /* The module owns the CSP handle table, so a fatal transition clears
         * all live module-owned key slots before the sticky state is exposed. */
        ano_crypto_internal_csp_zeroize_all_unlocked();
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
    uint64_t init_generation;

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

    if (g_init_generation == UINT64_MAX) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED;
    }
    init_generation = ++g_init_generation;
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
            ano_crypto_internal_csp_zeroize_all_unlocked();
            ano_crypto_mark_error_unlocked(st);
        }
        ano_crypto_internal_global_unlock();
        return st;
    }

    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }

    /* Shutdown preempts this startup, even if a later init has already reached
     * SELF_TEST_RUNNING (or OPERATIONAL). Only the current startup may publish
     * success. Mandatory self-test failures and sticky fatal state above retain
     * precedence, including failures from an older startup. */
    if (g_module_state != ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING ||
        init_generation != g_init_generation) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_NOT_INITIALIZED;
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

    /* Orderly shutdown and fatal-state shutdown both clear the module-owned
     * CSP table. Caller-owned opaque operation contexts remain the caller's
     * responsibility because the module does not retain their addresses. */
    ano_crypto_internal_csp_zeroize_all_unlocked();

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

ano_crypto_status_t ano_crypto_internal_module_require_operational_unlocked(void)
{
    if (g_fatal_latched != 0 || g_module_state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE) {
        return ANO_CRYPTO_ERR_FATAL_STATE;
    }
    if (g_module_state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        return ANO_CRYPTO_ERR_NOT_INITIALIZED;
    }
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

    result = ano_crypto_internal_module_require_operational_unlocked();

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
