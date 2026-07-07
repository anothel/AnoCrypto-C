#include <anocrypto/ac_csp.h>

ano_crypto_status_t ano_crypto_csp_handle_is_valid(
    ano_crypto_csp_handle_t handle,
    int *out_is_valid)
{
    if (out_is_valid == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_is_valid = (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) ? 1 : 0;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_csp_handle_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state)
{
    if (out_state == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        *out_state = ANO_CRYPTO_CSP_STATE_INVALID;
        return ANO_CRYPTO_OK;
    }

    /* TODO(P1): Replace with real handle table lookup. */
    *out_state = ANO_CRYPTO_CSP_STATE_INVALID;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

const char *ano_crypto_csp_state_name(
    ano_crypto_csp_state_t state)
{
    switch (state) {
    case ANO_CRYPTO_CSP_STATE_INVALID:
        return "INVALID";
    case ANO_CRYPTO_CSP_STATE_ALLOCATED:
        return "ALLOCATED";
    case ANO_CRYPTO_CSP_STATE_INITIALIZED:
        return "INITIALIZED";
    case ANO_CRYPTO_CSP_STATE_ACTIVE:
        return "ACTIVE";
    case ANO_CRYPTO_CSP_STATE_RESTRICTED:
        return "RESTRICTED";
    case ANO_CRYPTO_CSP_STATE_ZEROIZED:
        return "ZEROIZED";
    case ANO_CRYPTO_CSP_STATE_DESTROYED:
        return "DESTROYED";
    default:
        return "UNKNOWN_CSP_STATE";
    }
}
