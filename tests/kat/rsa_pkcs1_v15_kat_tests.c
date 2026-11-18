#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/rsa_test_helpers.h"

#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
#include "ac_entropy_internal.h"
#endif

static int check_status(ano_crypto_status_t st, int line)
{
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "RSA KAT failure line %d: %s\n", line, ano_crypto_status_name(st));
        return 0;
    }
    return 1;
}
#define CHECK(x) do { if (!check_status((x), __LINE__)) return 1; } while (0)

static int run_case(ano_crypto_csp_handle_t private_key,
                    ano_crypto_csp_handle_t public_key,
                    ano_crypto_signature_algorithm_t algorithm,
                    const uint8_t *message, size_t message_len,
                    const uint8_t expected[256])
{
    uint8_t signature[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE + 17u];
    size_t written = 99u;
    ano_crypto_status_t st;
    memset(signature, 0xa5, sizeof(signature));
    st = ano_crypto_signature_sign(private_key, algorithm, message, message_len,
                                   signature, sizeof(signature), &written);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
    if (!check_status(st, __LINE__)) return 0;
    if (written != ANO_CRYPTO_RSA2048_SIGNATURE_SIZE ||
        memcmp(signature, expected, ANO_CRYPTO_RSA2048_SIGNATURE_SIZE) != 0) {
        fprintf(stderr, "RSA KAT signature mismatch\n");
        return 0;
    }
#else
    {
        size_t i;
        if (st != ANO_CRYPTO_ERR_NOT_IMPLEMENTED || written != 0u) return 0;
        for (i = 0u; i < sizeof(signature); ++i) {
            if (signature[i] != 0u) return 0;
        }
        /* Verification remains available without signing/blinding entropy. */
        memcpy(signature, expected, ANO_CRYPTO_RSA2048_SIGNATURE_SIZE);
        written = ANO_CRYPTO_RSA2048_SIGNATURE_SIZE;
    }
#endif
    if (!check_status(ano_crypto_signature_verify(public_key, algorithm, message, message_len,
                                                  signature, written), __LINE__)) return 0;
    if (!check_status(ano_crypto_signature_verify(private_key, algorithm, message, message_len,
                                                  signature, written), __LINE__)) return 0;
    return 1;
}

static int check_error_propagation(ano_crypto_csp_handle_t priv,
                                   ano_crypto_csp_handle_t pub)
{
    if (run_case(ANO_CRYPTO_CSP_INVALID_HANDLE, pub,
                 ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                 RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA256) ||
        run_case(priv, ANO_CRYPTO_CSP_INVALID_HANDLE,
                 ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                 RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA256)) {
        fputs("RSA KAT swallowed an API error\n", stderr);
        return 1;
    }
#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
    {
        const uint32_t modes[] = {
            ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL,
            ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL
        };
        const ano_crypto_signature_algorithm_t algorithms[] = {
            ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
            ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384,
            ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512
        };
        size_t i, j, k;
        for (i = 0u; i < sizeof(modes) / sizeof(modes[0]); ++i) {
            CHECK(ano_crypto_internal_entropy_test_hook_configure(modes[i], 0x20u, 3u));
            for (j = 0u; j < sizeof(algorithms) / sizeof(algorithms[0]); ++j) {
                uint8_t signature[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE + 17u];
                size_t written = 99u;
                /* Queries and public verification must not consume entropy. */
                CHECK(ano_crypto_signature_sign(priv, algorithms[j],
                    RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), NULL, 0u, &written));
                if (written != ANO_CRYPTO_RSA2048_SIGNATURE_SIZE) return 1;
                memset(signature, 0xa5, sizeof(signature));
                if (ano_crypto_signature_sign(priv, algorithms[j],
                    RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), signature,
                    sizeof(signature), &written) != ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED ||
                    written != 0u) return 1;
                for (k = 0u; k < sizeof(signature); ++k) {
                    if (signature[k] != 0u) return 1;
                }
            }
            CHECK(ano_crypto_signature_verify(pub, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA256, 256u));
            if (run_case(priv, pub, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                         RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA256)) {
                fputs("RSA KAT swallowed an entropy error\n", stderr);
                return 1;
            }
        }
        ano_crypto_internal_entropy_test_hook_reset();
    }
#endif
    return 0;
}

int main(void)
{
    rsa_test_components_t components;
    ano_crypto_csp_handle_t private_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t public_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    size_t required = 0u;

    CHECK(ano_crypto_module_init(NULL));
    rsa_test_private_components(&components);
    CHECK(rsa_test_import(&components, ANO_CRYPTO_KEY_FLAG_NONE, &private_key));
    rsa_test_public_components(&components);
    CHECK(rsa_test_import(&components, ANO_CRYPTO_KEY_FLAG_NONE, &public_key));
    CHECK(ano_crypto_signature_sign(private_key,
                                    ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                                    RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                                    NULL, 0u, &required));
    if (required != 256u) return 1;
    if (!run_case(private_key, public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                  RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA256)) return 1;
    if (!run_case(private_key, public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384,
                  RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA384)) return 1;
    if (!run_case(private_key, public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512,
                  RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE), RSA_KAT_SIG_SHA512)) return 1;
    if (!run_case(private_key, public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                  NULL, 0u, RSA_KAT_SIG_EMPTY_SHA256)) return 1;
    if (check_error_propagation(private_key, public_key) != 0) return 1;
    CHECK(ano_crypto_key_destroy(private_key));
    CHECK(ano_crypto_key_destroy(public_key));
    CHECK(ano_crypto_module_shutdown());
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
    puts("RSA PKCS#1 v1.5 SHA-2 signing and verification KATs passed");
#else
    puts("RSA PKCS#1 v1.5 SHA-2 verification KATs and unavailable-entropy signing rejection passed");
#endif
    return 0;
}
