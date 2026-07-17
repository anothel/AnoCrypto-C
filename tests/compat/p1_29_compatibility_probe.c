#include <anocrypto/anocrypto.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef ANO_CRYPTO_COMPAT_HEADER_MODE
#define ANO_CRYPTO_COMPAT_HEADER_MODE "unspecified"
#endif
#ifndef ANO_CRYPTO_EXPECT_RUNTIME_VERSION
#define ANO_CRYPTO_EXPECT_RUNTIME_VERSION "0.1.0-p2-11-human-rehearsal-handoff"
#endif

#define ANO_COMPAT_JOIN_INNER(a, b) a##b
#define ANO_COMPAT_JOIN(a, b) ANO_COMPAT_JOIN_INNER(a, b)
#define ANO_COMPAT_STATIC_ASSERT(expr) \
    typedef char ANO_COMPAT_JOIN(ano_compat_static_assert_, __LINE__)[(expr) ? 1 : -1]
#define ANO_COMPAT_ALIGNOF(type) offsetof(struct { char prefix; type value; }, value)
#define ANO_COMPAT_ALIGN_UP(value, alignment) \
    ((((value) + (alignment) - 1u) / (alignment)) * (alignment))

/* P1-29 numeric and layout contract retained by the ABI-0 line. */
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_OK == 0);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_INVALID_ARGUMENT == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_BUFFER_TOO_SMALL == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM == 3);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER == 4);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_SELF_TEST_FAILED == 5);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_FATAL_STATE == 6);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_NOT_INITIALIZED == 7);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_NOT_APPROVED_IN_MODE == 8);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED == 9);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED == 10);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_AUTHENTICATION_FAILED == 11);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED == 12);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_NOT_IMPLEMENTED == 13);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED == 14);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_ERR_INTERNAL_ERROR == 255);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_VERSION_MAJOR == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_VERSION_MINOR == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_VERSION_PATCH == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION == 1u);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_MODULE_STATE_UNINITIALIZED == 0);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_MODULE_STATE_OPERATIONAL == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_MODULE_STATE_ERROR_STATE == 3);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_MODULE_STATE_SHUTDOWN == 4);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SELFTEST_FLAG_DEFAULT == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SELFTEST_FLAG_STARTUP == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SELFTEST_FLAG_KAT_PLACEHOLDER == 2u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER == 4u);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_ALG_SHA256 == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_ALG_SHA384 == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_ALG_SHA512 == 3);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE == 32u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE == 48u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE == 64u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA256_BLOCK_SIZE == 64u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA384_BLOCK_SIZE == 128u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_HASH_SHA512_BLOCK_SIZE == 128u);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256 == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_ALG_HASH_SHA256 == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_STATE_UNINSTANTIATED == 0);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_STATE_INSTANTIATED == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_STATE_ERROR == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_FLAG_NONE == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_FLAG_PREDICTION_RESISTANCE_REQUESTED == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE == 32u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE == 16u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE == 65536u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE == 32u);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RANDOM_FLAG_NONE == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE == 65536u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RANDOM_SERVICE_IMPLEMENTED == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RANDOM_SERVICE_RESERVED == 0u);

ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_csp_handle_t) == 8u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_INVALID_HANDLE == (ano_crypto_csp_handle_t)0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_INVALID == 0);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_ALLOCATED == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_INITIALIZED == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_ACTIVE == 3);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_RESTRICTED == 4);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_ZEROIZED == 5);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CSP_STATE_DESTROYED == 6);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_SECRET_BYTES == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_HMAC == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_DRBG_SEED_MATERIAL == 3);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER == 4);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY == 5);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY == 6);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_TYPE_CMAC == 7);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_FLAG_NONE == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE == 1u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE == 2u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_KEY_MAX_SECRET_BYTES == 128u);

ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_ALG_AES == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_MODE_CBC == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_AES_BLOCK_SIZE == 16u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_AES128_KEY_SIZE == 16u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_AES192_KEY_SIZE == 24u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_AES256_KEY_SIZE == 32u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_CBC_IV_SIZE == 16u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CIPHER_FLAG_NONE == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CMAC_TAG_SIZE == 16u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_CMAC_FLAG_NONE == 0u);

/* The ABI-0 build-identity layout is pointer-model dependent but field order is fixed. */
ANO_COMPAT_STATIC_ASSERT(offsetof(ano_crypto_build_identity_t, abi_version) == 0u);
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, product_name) ==
    ANO_COMPAT_ALIGN_UP(sizeof(uint32_t), ANO_COMPAT_ALIGNOF(const char *)));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, version) ==
    offsetof(ano_crypto_build_identity_t, product_name) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, git_commit) ==
    offsetof(ano_crypto_build_identity_t, version) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, build_type) ==
    offsetof(ano_crypto_build_identity_t, git_commit) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, compiler) ==
    offsetof(ano_crypto_build_identity_t, build_type) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, target) ==
    offsetof(ano_crypto_build_identity_t, compiler) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    offsetof(ano_crypto_build_identity_t, notes) ==
    offsetof(ano_crypto_build_identity_t, target) + sizeof(const char *));
ANO_COMPAT_STATIC_ASSERT(
    sizeof(ano_crypto_build_identity_t) ==
    offsetof(ano_crypto_build_identity_t, notes) + sizeof(const char *));

/* Default compiler ABI policy: public enums use the ordinary int representation. */
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_status_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_module_state_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_hash_algorithm_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_drbg_algorithm_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_drbg_state_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_csp_state_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_key_type_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_cipher_algorithm_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_cipher_mode_t) == sizeof(int));
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_cipher_direction_t) == sizeof(int));

#define ANO_COMPAT_BIND(return_type, function_name, parameter_list) \
    do { \
        return_type (*function_name##_pointer) parameter_list = function_name; \
        if (function_name##_pointer == NULL) { \
            return 0; \
        } \
        count += 1u; \
    } while (0)

static unsigned int bind_p1_29_public_surface(void)
{
    unsigned int count = 0u;

    ANO_COMPAT_BIND(const char *, ano_crypto_status_name, (ano_crypto_status_t));
    ANO_COMPAT_BIND(int, ano_crypto_status_is_ok, (ano_crypto_status_t));
    ANO_COMPAT_BIND(int, ano_crypto_status_is_error, (ano_crypto_status_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_version_string, (const char **));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_version_components,
                    (uint32_t *, uint32_t *, uint32_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_build_identity,
                    (ano_crypto_build_identity_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_build_fingerprint, (const char **));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_module_init,
                    (const ano_crypto_module_config_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_module_shutdown, (void));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_module_get_state,
                    (ano_crypto_module_state_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_module_get_last_error,
                    (ano_crypto_status_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_module_require_operational, (void));
    ANO_COMPAT_BIND(const char *, ano_crypto_module_state_name, (ano_crypto_module_state_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_selftest_run, (uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_selftest_get_last_status,
                    (ano_crypto_status_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_selftest_get_last_flags, (uint32_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_context_size,
                    (ano_crypto_hash_algorithm_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_digest_size,
                    (ano_crypto_hash_algorithm_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_init,
                    (void *, size_t, ano_crypto_hash_algorithm_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_update,
                    (void *, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_final,
                    (void *, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hash_digest,
                    (ano_crypto_hash_algorithm_t, const uint8_t *, size_t,
                     uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(const char *, ano_crypto_hash_algorithm_name,
                    (ano_crypto_hash_algorithm_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_context_size,
                    (ano_crypto_hash_algorithm_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_mac_size,
                    (ano_crypto_hash_algorithm_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_init,
                    (void *, size_t, ano_crypto_hash_algorithm_t, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_update,
                    (void *, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_final,
                    (void *, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_compute,
                    (ano_crypto_hash_algorithm_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hmac_compute_with_key,
                    (ano_crypto_hash_algorithm_t, ano_crypto_csp_handle_t,
                     const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_context_size,
                    (ano_crypto_drbg_algorithm_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_instantiate,
                    (void *, size_t, ano_crypto_drbg_algorithm_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_reseed,
                    (void *, const uint8_t *, size_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_generate,
                    (void *, uint8_t *, size_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_uninstantiate,
                    (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_drbg_get_state,
                    (const void *, ano_crypto_drbg_state_t *));
    ANO_COMPAT_BIND(const char *, ano_crypto_drbg_algorithm_name,
                    (ano_crypto_drbg_algorithm_t));
    ANO_COMPAT_BIND(const char *, ano_crypto_drbg_state_name, (ano_crypto_drbg_state_t));
    ANO_COMPAT_BIND(int, ano_crypto_random_is_available, (void));
    ANO_COMPAT_BIND(const char *, ano_crypto_random_service_name, (void));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_random_context_size, (size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_random_init,
                    (void *, size_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_random_generate,
                    (void *, uint8_t *, size_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_random_reseed,
                    (void *, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_random_uninit, (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_csp_handle_is_valid,
                    (ano_crypto_csp_handle_t, int *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_csp_handle_get_state,
                    (ano_crypto_csp_handle_t, ano_crypto_csp_state_t *));
    ANO_COMPAT_BIND(const char *, ano_crypto_csp_state_name, (ano_crypto_csp_state_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_csp_zeroize, (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_key_import_secret,
                    (ano_crypto_key_type_t, const uint8_t *, size_t, uint32_t,
                     ano_crypto_csp_handle_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_key_destroy, (ano_crypto_csp_handle_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_key_get_state,
                    (ano_crypto_csp_handle_t, ano_crypto_csp_state_t *));
    ANO_COMPAT_BIND(const char *, ano_crypto_key_type_name, (ano_crypto_key_type_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cipher_context_size,
                    (ano_crypto_cipher_algorithm_t, ano_crypto_cipher_mode_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cipher_init,
                    (void *, size_t, ano_crypto_cipher_algorithm_t, ano_crypto_cipher_mode_t,
                     ano_crypto_cipher_direction_t, ano_crypto_csp_handle_t,
                     const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cipher_update,
                    (void *, const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cipher_uninit, (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cipher_crypt_with_key,
                    (ano_crypto_cipher_algorithm_t, ano_crypto_cipher_mode_t,
                     ano_crypto_cipher_direction_t, ano_crypto_csp_handle_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     uint8_t *, size_t, size_t *, uint32_t));
    ANO_COMPAT_BIND(const char *, ano_crypto_cipher_algorithm_name,
                    (ano_crypto_cipher_algorithm_t));
    ANO_COMPAT_BIND(const char *, ano_crypto_cipher_mode_name, (ano_crypto_cipher_mode_t));
    ANO_COMPAT_BIND(const char *, ano_crypto_cipher_direction_name,
                    (ano_crypto_cipher_direction_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_context_size, (size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_mac_size, (size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_init,
                    (void *, size_t, ano_crypto_csp_handle_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_update,
                    (void *, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_final,
                    (void *, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_uninit, (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_compute_with_key,
                    (ano_crypto_csp_handle_t, const uint8_t *, size_t,
                     uint8_t *, size_t, size_t *, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_cmac_verify_with_key,
                    (ano_crypto_csp_handle_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, uint32_t));

    return count;
}

static int check_build_identity_layout(void)
{
    ano_crypto_build_identity_t identity;
    const char *fingerprint = NULL;

    memset(&identity, 0, sizeof(identity));
    if (ano_crypto_build_identity(&identity) != ANO_CRYPTO_OK) {
        return 0;
    }
    if (identity.abi_version != ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION ||
        identity.product_name == NULL || identity.version == NULL ||
        identity.git_commit == NULL || identity.build_type == NULL ||
        identity.compiler == NULL || identity.target == NULL || identity.notes == NULL) {
        return 0;
    }
    if (ano_crypto_build_fingerprint(&fingerprint) != ANO_CRYPTO_OK || fingerprint == NULL) {
        return 0;
    }
    return 1;
}

int main(void)
{
    const char *runtime_version = NULL;
    uint32_t major = 0u;
    uint32_t minor = 0u;
    uint32_t patch = 0u;
    ano_crypto_module_state_t module_state = ANO_CRYPTO_MODULE_STATE_UNINITIALIZED;
    unsigned int symbol_count;

    symbol_count = bind_p1_29_public_surface();
    if (symbol_count != 69u) {
        return 1;
    }

#if defined(ANO_CRYPTO_EXPECT_P1_29_HEADER_SNAPSHOT)
    if (strcmp(ANO_CRYPTO_VERSION_STRING, "0.1.0-p1-29-cmac-hardening") != 0) {
        return 2;
    }
#elif defined(ANO_CRYPTO_EXPECT_CURRENT_HEADER)
    if (strcmp(ANO_CRYPTO_VERSION_STRING, ANO_CRYPTO_EXPECT_RUNTIME_VERSION) != 0) {
        return 2;
    }
#else
#error A compatibility header mode must be selected
#endif

    if (ano_crypto_version_components(&major, &minor, &patch) != ANO_CRYPTO_OK ||
        major != 0u || minor != 1u || patch != 0u) {
        return 3;
    }
    if (ano_crypto_version_string(&runtime_version) != ANO_CRYPTO_OK ||
        runtime_version == NULL ||
        strcmp(runtime_version, ANO_CRYPTO_EXPECT_RUNTIME_VERSION) != 0) {
        return 4;
    }
    if (!check_build_identity_layout()) {
        return 5;
    }

    if (ano_crypto_module_init(NULL) != ANO_CRYPTO_OK) {
        return 6;
    }
    if (ano_crypto_module_get_state(&module_state) != ANO_CRYPTO_OK ||
        module_state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        (void)ano_crypto_module_shutdown();
        return 7;
    }
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) {
        return 8;
    }

    printf("compatibility_baseline=0.1.0-p1-29-cmac-hardening\n");
    printf("runtime_version=%s\n", runtime_version);
    printf("header_mode=%s\n", ANO_CRYPTO_COMPAT_HEADER_MODE);
    printf("public_symbol_references=%u\n", symbol_count);
    printf("pointer_size=%u\n", (unsigned int)sizeof(void *));
    printf("size_t_size=%u\n", (unsigned int)sizeof(size_t));
    printf("public_enum_size=%u\n", (unsigned int)sizeof(ano_crypto_status_t));
    printf("build_identity_size=%u\n", (unsigned int)sizeof(ano_crypto_build_identity_t));
    printf("sensitive_values_logged=no\n");
    printf("result=PASS\n");
    return 0;
}
