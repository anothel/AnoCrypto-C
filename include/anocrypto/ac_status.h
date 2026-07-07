#ifndef ANO_CRYPTO_AC_STATUS_H
#define ANO_CRYPTO_AC_STATUS_H

#include <stdint.h>

#if defined(_WIN32) && defined(ANO_CRYPTO_BUILD_SHARED)
#  if defined(ANO_CRYPTO_BUILDING_LIBRARY)
#    define ANO_CRYPTO_API __declspec(dllexport)
#  else
#    define ANO_CRYPTO_API __declspec(dllimport)
#  endif
#else
#  define ANO_CRYPTO_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Stable status values for the C ABI.
 *
 * Rules:
 * - Keep existing numeric values stable after a public release.
 * - Add new values before ANO_CRYPTO_ERR_INTERNAL_ERROR where possible.
 * - Do not use errno, exceptions, or global mutable error strings as the API contract.
 */
typedef enum ano_crypto_status_e {
    ANO_CRYPTO_OK = 0,
    ANO_CRYPTO_ERR_INVALID_ARGUMENT = 1,
    ANO_CRYPTO_ERR_BUFFER_TOO_SMALL = 2,
    ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM = 3,
    ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER = 4,
    ANO_CRYPTO_ERR_SELF_TEST_FAILED = 5,
    ANO_CRYPTO_ERR_FATAL_STATE = 6,
    ANO_CRYPTO_ERR_NOT_INITIALIZED = 7,
    ANO_CRYPTO_ERR_NOT_APPROVED_IN_MODE = 8,
    ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED = 9,
    ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED = 10,
    ANO_CRYPTO_ERR_AUTHENTICATION_FAILED = 11,
    ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED = 12,
    ANO_CRYPTO_ERR_INTERNAL_ERROR = 255
} ano_crypto_status_t;

ANO_CRYPTO_API const char *ano_crypto_status_name(ano_crypto_status_t status);
ANO_CRYPTO_API int ano_crypto_status_is_ok(ano_crypto_status_t status);
ANO_CRYPTO_API int ano_crypto_status_is_error(ano_crypto_status_t status);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ANO_CRYPTO_AC_STATUS_H */
