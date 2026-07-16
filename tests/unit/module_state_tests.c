#include <stdio.h>
#include <stdint.h>
#include <anocrypto/anocrypto.h>
#include "ac_internal.h"

#define ASSERT_STATUS(expr, expected) do { \
    ano_crypto_status_t _st = (expr); \
    if (_st != (expected)) { \
        fprintf(stderr, "ASSERT_STATUS failed at %s:%d: got %s, expected %s\n", \
                __FILE__, __LINE__, \
                ano_crypto_status_name(_st), \
                ano_crypto_status_name((expected))); \
        return 1; \
    } \
} while (0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "ASSERT_TRUE failed at %s:%d: %s\n", __FILE__, __LINE__, #expr); \
        return 1; \
    } \
} while (0)

static int test_initial_state_blocks_services(void)
{
    ano_crypto_module_state_t state;

    ASSERT_STATUS(ano_crypto_module_get_state(&state), ANO_CRYPTO_OK);
    ASSERT_TRUE(state == ANO_CRYPTO_MODULE_STATE_UNINITIALIZED);
    ASSERT_STATUS(ano_crypto_module_require_operational(), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    return 0;
}

static int test_init_selftest_to_operational(void)
{
    ano_crypto_module_state_t state;
    ano_crypto_status_t last_selftest;
    uint32_t last_flags;

    ASSERT_STATUS(ano_crypto_module_init(0), ANO_CRYPTO_OK);
    ASSERT_STATUS(ano_crypto_module_get_state(&state), ANO_CRYPTO_OK);
    ASSERT_TRUE(state == ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    ASSERT_STATUS(ano_crypto_module_require_operational(), ANO_CRYPTO_OK);

    ASSERT_STATUS(ano_crypto_selftest_get_last_status(&last_selftest), ANO_CRYPTO_OK);
    ASSERT_TRUE(last_selftest == ANO_CRYPTO_OK);

    ASSERT_STATUS(ano_crypto_selftest_get_last_flags(&last_flags), ANO_CRYPTO_OK);
    ASSERT_TRUE((last_flags & ANO_CRYPTO_SELFTEST_FLAG_STARTUP) != 0u);

    return 0;
}

static int test_build_and_version_queries(void)
{
    const char *version = 0;
    const char *fingerprint = 0;
    uint32_t major = 0u;
    uint32_t minor = 0u;
    uint32_t patch = 0u;
    ano_crypto_build_identity_t identity;

    ASSERT_STATUS(ano_crypto_version_string(&version), ANO_CRYPTO_OK);
    ASSERT_TRUE(version != 0);

    ASSERT_STATUS(ano_crypto_version_components(&major, &minor, &patch), ANO_CRYPTO_OK);
    ASSERT_TRUE(major == ANO_CRYPTO_VERSION_MAJOR);
    ASSERT_TRUE(minor == ANO_CRYPTO_VERSION_MINOR);
    ASSERT_TRUE(patch == ANO_CRYPTO_VERSION_PATCH);

    ASSERT_STATUS(ano_crypto_build_identity(&identity), ANO_CRYPTO_OK);
    ASSERT_TRUE(identity.abi_version == ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION);
    ASSERT_TRUE(identity.product_name != 0);
    ASSERT_TRUE(identity.version != 0);

    ASSERT_STATUS(ano_crypto_build_fingerprint(&fingerprint), ANO_CRYPTO_OK);
    ASSERT_TRUE(fingerprint != 0);
    return 0;
}

static int test_shutdown_blocks_services(void)
{
    ano_crypto_module_state_t state;

    ASSERT_STATUS(ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    ASSERT_STATUS(ano_crypto_module_get_state(&state), ANO_CRYPTO_OK);
    ASSERT_TRUE(state == ANO_CRYPTO_MODULE_STATE_SHUTDOWN);
    ASSERT_STATUS(ano_crypto_module_require_operational(), ANO_CRYPTO_ERR_NOT_INITIALIZED);
    return 0;
}

static int test_orderly_shutdown_allows_full_reinit(void)
{
    ano_crypto_module_state_t state;
    ano_crypto_status_t last_selftest;
    uint32_t last_flags;

    ASSERT_STATUS(ano_crypto_module_init(0), ANO_CRYPTO_OK);
    ASSERT_STATUS(ano_crypto_module_get_state(&state), ANO_CRYPTO_OK);
    ASSERT_TRUE(state == ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    ASSERT_STATUS(ano_crypto_module_require_operational(), ANO_CRYPTO_OK);

    ASSERT_STATUS(ano_crypto_selftest_get_last_status(&last_selftest), ANO_CRYPTO_OK);
    ASSERT_TRUE(last_selftest == ANO_CRYPTO_OK);
    ASSERT_STATUS(ano_crypto_selftest_get_last_flags(&last_flags), ANO_CRYPTO_OK);
    ASSERT_TRUE((last_flags & ANO_CRYPTO_SELFTEST_FLAG_STARTUP) != 0u);

    return 0;
}

static int test_internal_fatal_transition_enters_error_state(void)
{
    ano_crypto_module_state_t state;
    ano_crypto_status_t last_error;

    ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_SELF_TEST_FAILED);
    ASSERT_STATUS(ano_crypto_module_get_state(&state), ANO_CRYPTO_OK);
    ASSERT_TRUE(state == ANO_CRYPTO_MODULE_STATE_ERROR_STATE);
    ASSERT_STATUS(ano_crypto_module_get_last_error(&last_error), ANO_CRYPTO_OK);
    ASSERT_TRUE(last_error == ANO_CRYPTO_ERR_SELF_TEST_FAILED);
    ASSERT_STATUS(ano_crypto_module_require_operational(), ANO_CRYPTO_ERR_FATAL_STATE);
    ASSERT_STATUS(ano_crypto_module_init(0), ANO_CRYPTO_ERR_FATAL_STATE);
    return 0;
}

int main(void)
{
    if (test_initial_state_blocks_services() != 0) {
        return 1;
    }
    if (test_init_selftest_to_operational() != 0) {
        return 1;
    }
    if (test_build_and_version_queries() != 0) {
        return 1;
    }
    if (test_shutdown_blocks_services() != 0) {
        return 1;
    }
    if (test_orderly_shutdown_allows_full_reinit() != 0) {
        return 1;
    }
    if (test_internal_fatal_transition_enters_error_state() != 0) {
        return 1;
    }

    puts("anocrypto-c module_state_tests: PASS");
    return 0;
}
