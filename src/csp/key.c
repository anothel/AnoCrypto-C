#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>

static ano_crypto_status_t ano_crypto_key_validate_type(
    ano_crypto_key_type_t key_type)
{
    switch (key_type) {
    case ANO_CRYPTO_KEY_TYPE_SECRET_BYTES:
    case ANO_CRYPTO_KEY_TYPE_HMAC:
    case ANO_CRYPTO_KEY_TYPE_DRBG_SEED_MATERIAL:
    case ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER:
    case ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY:
    case ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY:
        return ANO_CRYPTO_OK;
    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
}

ano_crypto_status_t ano_crypto_key_import_secret(
    ano_crypto_key_type_t key_type,
    const uint8_t *key_material,
    size_t key_material_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if ((key_material == 0 && key_material_len != 0u) || out_handle == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if ((flags & ~(ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE |
                   ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE)) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_key_validate_type(key_type);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_key_destroy(
    ano_crypto_csp_handle_t handle)
{
    ano_crypto_status_t st;

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
}

ano_crypto_status_t ano_crypto_key_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state)
{
    return ano_crypto_csp_handle_get_state(handle, out_state);
}

const char *ano_crypto_key_type_name(
    ano_crypto_key_type_t key_type)
{
    switch (key_type) {
    case ANO_CRYPTO_KEY_TYPE_SECRET_BYTES:
        return "SECRET_BYTES";
    case ANO_CRYPTO_KEY_TYPE_HMAC:
        return "HMAC";
    case ANO_CRYPTO_KEY_TYPE_DRBG_SEED_MATERIAL:
        return "DRBG_SEED_MATERIAL";
    case ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER:
        return "BLOCK_CIPHER";
    case ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY:
        return "PUBLIC_KEY";
    case ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY:
        return "PRIVATE_KEY";
    default:
        return "UNKNOWN_KEY_TYPE";
    }
}
