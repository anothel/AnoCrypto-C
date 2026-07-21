#include <anocrypto/anocrypto.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ANO_CRYPTO_COMPAT_HEADER_MODE
#define ANO_CRYPTO_COMPAT_HEADER_MODE "unspecified"
#endif
#ifndef ANO_CRYPTO_EXPECT_RUNTIME_VERSION
#define ANO_CRYPTO_EXPECT_RUNTIME_VERSION "0.1.0-p3-08-rsa-pkcs1-v15-sha2"
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
#if defined(ANO_CRYPTO_EXPECT_CURRENT_HEADER)
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE == 0u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_AEAD_DIRECTION_DECRYPT == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RSA2048_MODULUS_SIZE == 256u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RSA2048_PRIME_SIZE == 128u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_RSA2048_SIGNATURE_SIZE == 256u);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256 == 1);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384 == 2);
ANO_COMPAT_STATIC_ASSERT(ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512 == 3);
ANO_COMPAT_STATIC_ASSERT(sizeof(ano_crypto_signature_algorithm_t) == sizeof(int));
#endif

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

#if defined(ANO_CRYPTO_EXPECT_CURRENT_HEADER)
static unsigned int bind_current_additive_surface(void)
{
    unsigned int count = 0u;
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_hkdf_sha256,
                    (const uint8_t *, size_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_encrypt,
                    (ano_crypto_csp_handle_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     uint8_t *, size_t, size_t *, uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_decrypt,
                    (ano_crypto_csp_handle_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_context_size,
                    (size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_init,
                    (void *, size_t, ano_crypto_aead_direction_t,
                     ano_crypto_csp_handle_t, const uint8_t *, size_t, uint32_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_aad_update,
                    (void *, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_update,
                    (void *, const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_encrypt_final,
                    (void *, uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_decrypt_final,
                    (void *, const uint8_t *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_aes256_gcm_stream_uninit,
                    (void *, size_t));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_scrypt_derive_key,
                    (const uint8_t *, size_t, const uint8_t *, size_t,
                     void *, size_t, uint8_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_rsa_key_import,
                    (const uint8_t *, size_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     const uint8_t *, size_t, const uint8_t *, size_t,
                     uint32_t, ano_crypto_csp_handle_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_signature_sign,
                    (ano_crypto_csp_handle_t, ano_crypto_signature_algorithm_t,
                     const uint8_t *, size_t, uint8_t *, size_t, size_t *));
    ANO_COMPAT_BIND(ano_crypto_status_t, ano_crypto_signature_verify,
                    (ano_crypto_csp_handle_t, ano_crypto_signature_algorithm_t,
                     const uint8_t *, size_t, const uint8_t *, size_t));
    return count;
}
#endif

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
#if defined(ANO_CRYPTO_EXPECT_CURRENT_HEADER)
    symbol_count += bind_current_additive_surface();
    if (symbol_count != 83u) {
        return 1;
    }
#else
    if (symbol_count != 69u) {
        return 1;
    }
#endif

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
#if defined(ANO_CRYPTO_EXPECT_CURRENT_HEADER)
    {
        static const uint8_t ikm[] = { 'i', 'k', 'm' };
        uint8_t okm[16];
        if (ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE != 8160u ||
            ano_crypto_hkdf_sha256(ikm, sizeof(ikm), NULL, 0u, NULL, 0u, okm, sizeof(okm)) != ANO_CRYPTO_OK) {
            (void)ano_crypto_module_shutdown();
            return 8;
        }
    }
    {
        static const uint8_t key[ANO_CRYPTO_AES256_GCM_KEY_SIZE] = { 0u };
        static const uint8_t nonce[ANO_CRYPTO_AES256_GCM_NONCE_SIZE] = { 0u };
        static const uint8_t message[] = { 'g', 'c', 'm' };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t ciphertext[sizeof(message)];
        uint8_t plaintext[sizeof(message)];
        uint8_t tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
        size_t written = 0u;
        if (ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                                         key, sizeof(key), ANO_CRYPTO_KEY_FLAG_NONE,
                                         &handle) != ANO_CRYPTO_OK ||
            ano_crypto_aes256_gcm_encrypt(handle, nonce, sizeof(nonce),
                                           NULL, 0u, message, sizeof(message),
                                           ciphertext, sizeof(ciphertext), &written,
                                           tag, sizeof(tag)) != ANO_CRYPTO_OK ||
            written != sizeof(message) ||
            ano_crypto_aes256_gcm_decrypt(handle, nonce, sizeof(nonce),
                                           NULL, 0u, ciphertext, sizeof(ciphertext),
                                           tag, sizeof(tag), plaintext, sizeof(plaintext),
                                           &written) != ANO_CRYPTO_OK ||
            written != sizeof(message) ||
            memcmp(plaintext, message, sizeof(message)) != 0) {
            if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) {
                (void)ano_crypto_key_destroy(handle);
            }
            (void)ano_crypto_module_shutdown();
            return 9;
        }
        (void)ano_crypto_key_destroy(handle);
    }
    {
        static const uint8_t password[] = { 'p', 'a', 's', 's', 'w', 'o', 'r', 'd' };
        static const uint8_t salt[] = { 'N', 'a', 'C', 'l' };
        static const uint8_t expected[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE] = {
            0x3fu,0xa8u,0x51u,0x18u,0xffu,0xfeu,0x97u,0x32u,0xdau,0x7fu,0x4cu,0x63u,0xefu,0x61u,0xf5u,0xc6u,
            0x45u,0x81u,0x44u,0xd0u,0x36u,0x40u,0x1fu,0x88u,0x29u,0xddu,0x4eu,0x5bu,0x2cu,0x50u,0xc5u,0x1fu };
        void *workspace = malloc(ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
        uint8_t key[ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE];
        int failed = 0;
        if (workspace == NULL ||
            ano_crypto_scrypt_derive_key(password, sizeof(password), salt, sizeof(salt),
                                         workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,
                                         key) != ANO_CRYPTO_OK ||
            memcmp(key, expected, sizeof(key)) != 0) {
            failed = 1;
        }
        (void)ano_crypto_csp_zeroize(key, sizeof(key));
        free(workspace);
        if (failed) {
            (void)ano_crypto_module_shutdown();
            return 10;
        }
    }
    {
        static const uint8_t key[ANO_CRYPTO_AES256_GCM_KEY_SIZE] = { 0u };
        static const uint8_t nonce[ANO_CRYPTO_AES256_GCM_NONCE_SIZE] = { 1u };
        static const uint8_t aad[] = { 's', 't', 'r', 'e', 'a', 'm' };
        static const uint8_t message[] = { 'a', 'd', 'd', 'i', 't', 'i', 'v', 'e' };
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        void *enc = NULL;
        void *dec = NULL;
        size_t ctx_size = 0u;
        size_t written = 0u;
        uint8_t ciphertext[sizeof(message)];
        uint8_t plaintext[sizeof(message)];
        uint8_t tag[ANO_CRYPTO_AES256_GCM_TAG_SIZE];
        int failed = 0;
        if (ano_crypto_aes256_gcm_stream_context_size(&ctx_size) != ANO_CRYPTO_OK || ctx_size == 0u) {
            failed = 1;
        }
        if (!failed) {
            enc = calloc(1u, ctx_size);
            dec = calloc(1u, ctx_size);
            if (enc == NULL || dec == NULL) failed = 1;
        }
        if (!failed &&
            (ano_crypto_key_import_secret(ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                                          key, sizeof(key), ANO_CRYPTO_KEY_FLAG_NONE,
                                          &handle) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_init(enc, ctx_size,
                                               ANO_CRYPTO_AEAD_DIRECTION_ENCRYPT,
                                               handle, nonce, sizeof(nonce),
                                               ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_aad_update(enc, aad, sizeof(aad)) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_update(enc, message, sizeof(message),
                                                 ciphertext, sizeof(ciphertext),
                                                 &written) != ANO_CRYPTO_OK ||
             written != sizeof(message) ||
             ano_crypto_aes256_gcm_stream_encrypt_final(enc, tag, sizeof(tag)) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_init(dec, ctx_size,
                                               ANO_CRYPTO_AEAD_DIRECTION_DECRYPT,
                                               handle, nonce, sizeof(nonce),
                                               ANO_CRYPTO_AES256_GCM_STREAM_FLAG_NONE) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_aad_update(dec, aad, sizeof(aad)) != ANO_CRYPTO_OK ||
             ano_crypto_aes256_gcm_stream_update(dec, ciphertext, sizeof(ciphertext),
                                                 plaintext, sizeof(plaintext),
                                                 &written) != ANO_CRYPTO_OK ||
             written != sizeof(message) ||
             ano_crypto_aes256_gcm_stream_decrypt_final(dec, tag, sizeof(tag)) != ANO_CRYPTO_OK ||
             memcmp(plaintext, message, sizeof(message)) != 0)) {
            failed = 1;
        }
        if (enc != NULL) (void)ano_crypto_aes256_gcm_stream_uninit(enc, ctx_size);
        if (dec != NULL) (void)ano_crypto_aes256_gcm_stream_uninit(dec, ctx_size);
        if (handle != ANO_CRYPTO_CSP_INVALID_HANDLE) (void)ano_crypto_key_destroy(handle);
        free(enc);
        free(dec);
        if (failed) {
            (void)ano_crypto_module_shutdown();
            return 11;
        }
    }
#endif
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) {
        return 12;
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
