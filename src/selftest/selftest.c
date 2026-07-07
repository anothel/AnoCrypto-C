#include <anocrypto/ac_build.h>
#include <anocrypto/ac_selftest.h>
#include "ac_internal.h"

static ano_crypto_status_t g_selftest_last_status = ANO_CRYPTO_ERR_NOT_INITIALIZED;
static uint32_t g_selftest_last_flags = 0u;

static ano_crypto_status_t ano_crypto_selftest_check_build_identity(void)
{
    ano_crypto_build_identity_t identity;
    ano_crypto_status_t st;

    st = ano_crypto_build_identity(&identity);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (identity.product_name == 0 || identity.version == 0 || identity.compiler == 0) {
        return ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_run_placeholder_kats(void)
{
    /*
     * P0 placeholder only.
     * TODO(P1): Replace with public-source KAT manifest and per-algorithm tests.
     */
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_run(uint32_t flags)
{
    ano_crypto_status_t st;

    g_selftest_last_flags = flags;

#if defined(ANO_CRYPTO_ENABLE_TEST_HOOKS)
    if ((flags & ANO_CRYPTO_SELFTEST_FLAG_FORCE_FAILURE_FOR_TEST) != 0u) {
        g_selftest_last_status = ANO_CRYPTO_ERR_SELF_TEST_FAILED;
        ano_crypto_internal_module_mark_error(g_selftest_last_status);
        return g_selftest_last_status;
    }
#else
    if ((flags & 0x80000000u) != 0u) {
        g_selftest_last_status = ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        return g_selftest_last_status;
    }
#endif

    st = ano_crypto_selftest_check_build_identity();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_run_placeholder_kats();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    g_selftest_last_status = ANO_CRYPTO_OK;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_status(
    ano_crypto_status_t *out_status)
{
    if (out_status == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_status = g_selftest_last_status;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_flags(
    uint32_t *out_flags)
{
    if (out_flags == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_flags = g_selftest_last_flags;
    return ANO_CRYPTO_OK;
}
