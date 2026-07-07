#include <anocrypto/ac_status.h>

const char *ano_crypto_status_name(ano_crypto_status_t status)
{
    switch (status) {
    case ANO_CRYPTO_OK:
        return "ANO_CRYPTO_OK";
    case ANO_CRYPTO_ERR_INVALID_ARGUMENT:
        return "ANO_CRYPTO_ERR_INVALID_ARGUMENT";
    case ANO_CRYPTO_ERR_BUFFER_TOO_SMALL:
        return "ANO_CRYPTO_ERR_BUFFER_TOO_SMALL";
    case ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM:
        return "ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM";
    case ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER:
        return "ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER";
    case ANO_CRYPTO_ERR_SELF_TEST_FAILED:
        return "ANO_CRYPTO_ERR_SELF_TEST_FAILED";
    case ANO_CRYPTO_ERR_FATAL_STATE:
        return "ANO_CRYPTO_ERR_FATAL_STATE";
    case ANO_CRYPTO_ERR_NOT_INITIALIZED:
        return "ANO_CRYPTO_ERR_NOT_INITIALIZED";
    case ANO_CRYPTO_ERR_NOT_APPROVED_IN_MODE:
        return "ANO_CRYPTO_ERR_NOT_APPROVED_IN_MODE";
    case ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED:
        return "ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED";
    case ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED:
        return "ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED";
    case ANO_CRYPTO_ERR_AUTHENTICATION_FAILED:
        return "ANO_CRYPTO_ERR_AUTHENTICATION_FAILED";
    case ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED:
        return "ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED";
    case ANO_CRYPTO_ERR_INTERNAL_ERROR:
        return "ANO_CRYPTO_ERR_INTERNAL_ERROR";
    default:
        return "ANO_CRYPTO_ERR_UNKNOWN_STATUS";
    }
}

int ano_crypto_status_is_ok(ano_crypto_status_t status)
{
    return status == ANO_CRYPTO_OK ? 1 : 0;
}

int ano_crypto_status_is_error(ano_crypto_status_t status)
{
    return status == ANO_CRYPTO_OK ? 0 : 1;
}
