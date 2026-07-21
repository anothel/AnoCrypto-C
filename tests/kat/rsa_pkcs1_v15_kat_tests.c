#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/rsa_test_helpers.h"

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
    uint8_t signature[256];
    size_t written = 0u;
    memset(signature, 0xa5, sizeof(signature));
    CHECK(ano_crypto_signature_sign(private_key, algorithm, message, message_len,
                                    signature, sizeof(signature), &written));
    if (written != sizeof(signature) || memcmp(signature, expected, sizeof(signature)) != 0) {
        fprintf(stderr, "RSA KAT signature mismatch\n");
        return 0;
    }
    CHECK(ano_crypto_signature_verify(public_key, algorithm, message, message_len,
                                      signature, written));
    CHECK(ano_crypto_signature_verify(private_key, algorithm, message, message_len,
                                      signature, written));
    return 1;
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
    CHECK(ano_crypto_key_destroy(private_key));
    CHECK(ano_crypto_key_destroy(public_key));
    CHECK(ano_crypto_module_shutdown());
    puts("RSA PKCS#1 v1.5 SHA-2 KATs passed");
    return 0;
}
