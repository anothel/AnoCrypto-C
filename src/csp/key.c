#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_key.h>
#include <anocrypto/ac_module.h>
#include "ac_csp_internal.h"
#include "ac_internal.h"
#include "ac_rsa_internal.h"
#include "ac_ecdsa_internal.h"

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
    case ANO_CRYPTO_KEY_TYPE_CMAC:
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

    if (out_handle == 0 || key_material == 0 || key_material_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    if ((flags & ~(ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE |
                   ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE)) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_key_validate_type(key_type);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    return ano_crypto_internal_csp_import_secret(
        key_type, key_material, key_material_len, flags, out_handle);
}


ano_crypto_status_t ano_crypto_rsa_key_import(
    const uint8_t *modulus,
    size_t modulus_len,
    const uint8_t *public_exponent,
    size_t public_exponent_len,
    const uint8_t *private_exponent,
    size_t private_exponent_len,
    const uint8_t *prime1,
    size_t prime1_len,
    const uint8_t *prime2,
    size_t prime2_len,
    const uint8_t *exponent1,
    size_t exponent1_len,
    const uint8_t *exponent2,
    size_t exponent2_len,
    const uint8_t *coefficient,
    size_t coefficient_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    const ano_crypto_rsa_components_internal_t components = {
        modulus, modulus_len,
        public_exponent, public_exponent_len,
        private_exponent, private_exponent_len,
        prime1, prime1_len,
        prime2, prime2_len,
        exponent1, exponent1_len,
        exponent2, exponent2_len,
        coefficient, coefficient_len
    };
    ano_crypto_rsa_key_internal_t key;
    ano_crypto_key_type_t key_type;
    ano_crypto_status_t st;

    if (out_handle == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (flags != ANO_CRYPTO_KEY_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    st = ano_crypto_internal_rsa_validate_and_prepare(&components, &key, &key_type);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_csp_import_rsa(&key, key_type, flags, out_handle);
    }
    ano_crypto_internal_zeroize(&key, sizeof(key));
    return st;
}

ano_crypto_status_t ano_crypto_ec_p256_key_import(
    const uint8_t *public_x,
    size_t public_x_len,
    const uint8_t *public_y,
    size_t public_y_len,
    const uint8_t *private_scalar,
    size_t private_scalar_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    ano_crypto_ec_p256_key_internal_t key;
    ano_crypto_key_type_t key_type;
    ano_crypto_status_t st;

    if (out_handle == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (flags != ANO_CRYPTO_KEY_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    st = ano_crypto_internal_ec_p256_validate_and_prepare(
        public_x, public_x_len, public_y, public_y_len,
        private_scalar, private_scalar_len, &key, &key_type);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_csp_import_ec_p256(&key, key_type, flags, out_handle);
    }
    ano_crypto_internal_zeroize(&key, sizeof(key));
    return st;
}

ano_crypto_status_t ano_crypto_key_destroy(
    ano_crypto_csp_handle_t handle)
{
    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    /* Cleanup is allowed outside OPERATIONAL so caller-owned workflows can clear CSPs. */
    return ano_crypto_internal_csp_destroy(handle);
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
    case ANO_CRYPTO_KEY_TYPE_CMAC:
        return "CMAC";
    default:
        return "UNKNOWN_KEY_TYPE";
    }
}
