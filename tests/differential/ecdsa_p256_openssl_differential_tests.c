#include <stdio.h>
#include <string.h>

#include <anocrypto/anocrypto.h>
#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>

#include "../fixtures/ecdsa_p256_test_vectors.h"

#define GENERATED_KEY_CASES 16u
#define REQUIRE(condition)                                                        \
    do {                                                                          \
        if (!(condition)) {                                                        \
            fprintf(stderr, "ECDSA differential line %d\n", __LINE__);          \
            return 1;                                                             \
        }                                                                         \
    } while (0)

static EVP_PKEY *make_fixed_pkey(int private_key)
{
    uint8_t encoded[65];
    OSSL_PARAM_BLD *builder = OSSL_PARAM_BLD_new();
    OSSL_PARAM *params = NULL;
    EVP_PKEY_CTX *context = NULL;
    EVP_PKEY *key = NULL;
    BIGNUM *d = NULL;
    int ok = builder != NULL;

    encoded[0] = 0x04u;
    memcpy(encoded + 1u, ECDSA_P256_X, 32u);
    memcpy(encoded + 33u, ECDSA_P256_Y, 32u);
    if (ok) {
        ok = OSSL_PARAM_BLD_push_utf8_string(
                 builder, OSSL_PKEY_PARAM_GROUP_NAME, "prime256v1", 0u) == 1 &&
             OSSL_PARAM_BLD_push_octet_string(
                 builder, OSSL_PKEY_PARAM_PUB_KEY, encoded, sizeof(encoded)) == 1;
    }
    if (private_key && ok) {
        d = BN_bin2bn(ECDSA_P256_D, 32, NULL);
        ok = d != NULL &&
             OSSL_PARAM_BLD_push_BN(builder, OSSL_PKEY_PARAM_PRIV_KEY, d) == 1;
    }
    if (ok) {
        params = OSSL_PARAM_BLD_to_param(builder);
    }
    if (params != NULL) {
        context = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    }
    if (!(context != NULL && EVP_PKEY_fromdata_init(context) == 1 &&
          EVP_PKEY_fromdata(context, &key,
                            private_key ? EVP_PKEY_KEYPAIR : EVP_PKEY_PUBLIC_KEY,
                            params) == 1)) {
        EVP_PKEY_free(key);
        key = NULL;
    }
    EVP_PKEY_CTX_free(context);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(builder);
    BN_free(d);
    return key;
}

static EVP_PKEY *generate_p256_key(void)
{
    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    EVP_PKEY *key = NULL;
    if (context == NULL || EVP_PKEY_keygen_init(context) != 1 ||
        EVP_PKEY_CTX_set_group_name(context, "prime256v1") != 1 ||
        EVP_PKEY_generate(context, &key) != 1) {
        EVP_PKEY_free(key);
        key = NULL;
    }
    EVP_PKEY_CTX_free(context);
    return key;
}

static int extract_components(EVP_PKEY *key,
                              uint8_t x[32],
                              uint8_t y[32],
                              uint8_t d[32])
{
    BIGNUM *x_bn = NULL;
    BIGNUM *y_bn = NULL;
    BIGNUM *d_bn = NULL;
    int ok = EVP_PKEY_get_bn_param(key, OSSL_PKEY_PARAM_EC_PUB_X, &x_bn) == 1 &&
             EVP_PKEY_get_bn_param(key, OSSL_PKEY_PARAM_EC_PUB_Y, &y_bn) == 1 &&
             EVP_PKEY_get_bn_param(key, OSSL_PKEY_PARAM_PRIV_KEY, &d_bn) == 1;
    if (ok) {
        ok = BN_bn2binpad(x_bn, x, 32) == 32 &&
             BN_bn2binpad(y_bn, y, 32) == 32 &&
             BN_bn2binpad(d_bn, d, 32) == 32;
    }
    BN_free(x_bn);
    BN_free(y_bn);
    BN_clear_free(d_bn);
    return ok;
}

static int der_to_raw(const unsigned char *der,
                      size_t der_len,
                      unsigned char raw[64])
{
    const unsigned char *cursor = der;
    ECDSA_SIG *signature = d2i_ECDSA_SIG(NULL, &cursor, (long)der_len);
    const BIGNUM *r = NULL;
    const BIGNUM *s = NULL;
    int ok = signature != NULL && cursor == der + der_len;
    if (ok) {
        ECDSA_SIG_get0(signature, &r, &s);
        ok = BN_bn2binpad(r, raw, 32) == 32 &&
             BN_bn2binpad(s, raw + 32u, 32) == 32;
    }
    ECDSA_SIG_free(signature);
    return ok;
}

static int raw_to_der(const unsigned char raw[64],
                      unsigned char **out,
                      size_t *out_len)
{
    ECDSA_SIG *signature = ECDSA_SIG_new();
    BIGNUM *r = BN_bin2bn(raw, 32, NULL);
    BIGNUM *s = BN_bin2bn(raw + 32u, 32, NULL);
    unsigned char *cursor;
    int length;
    int ok = signature != NULL && r != NULL && s != NULL;
    if (ok) {
        ok = ECDSA_SIG_set0(signature, r, s) == 1;
        if (ok) {
            r = NULL;
            s = NULL;
        }
    }
    length = ok ? i2d_ECDSA_SIG(signature, NULL) : 0;
    if (length <= 0) {
        ok = 0;
    }
    if (ok) {
        *out = OPENSSL_malloc((size_t)length);
        cursor = *out;
        ok = *out != NULL && i2d_ECDSA_SIG(signature, &cursor) == length;
        if (ok) {
            *out_len = (size_t)length;
        }
    }
    BN_free(r);
    BN_free(s);
    ECDSA_SIG_free(signature);
    return ok;
}

static int openssl_sign(EVP_PKEY *key,
                        const unsigned char *message,
                        size_t message_len,
                        unsigned char raw[64])
{
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    unsigned char der[80];
    size_t der_len = sizeof(der);
    int ok = context != NULL;
    if (ok) {
        ok = EVP_DigestSignInit(context, NULL, EVP_sha256(), NULL, key) == 1;
    }
    if (ok) {
        ok = EVP_DigestSign(context, der, &der_len, message, message_len) == 1 &&
             der_to_raw(der, der_len, raw);
    }
    EVP_MD_CTX_free(context);
    return ok;
}

static int openssl_verify(EVP_PKEY *key,
                          const unsigned char *message,
                          size_t message_len,
                          const unsigned char raw[64])
{
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    unsigned char *der = NULL;
    size_t der_len = 0u;
    int ok = context != NULL && raw_to_der(raw, &der, &der_len);
    if (ok) {
        ok = EVP_DigestVerifyInit(context, NULL, EVP_sha256(), NULL, key) == 1;
    }
    if (ok) {
        ok = EVP_DigestVerify(context, der, der_len, message, message_len) == 1;
    }
    OPENSSL_free(der);
    EVP_MD_CTX_free(context);
    return ok;
}

static int run_fixed_operation_compatibility(unsigned *comparisons)
{
    static const unsigned char csr_message[] = "CSR proof-of-possession";
    static const unsigned char certificate_message[] = "certificate issue TBS";
    static const unsigned char crl_message[] = "CRL signing TBS";
    static const unsigned char ocsp_message[] = "OCSP response signing TBS";
    const unsigned char *messages[] = {
        csr_message, certificate_message, crl_message, ocsp_message
    };
    const size_t lengths[] = {
        sizeof(csr_message) - 1u, sizeof(certificate_message) - 1u,
        sizeof(crl_message) - 1u, sizeof(ocsp_message) - 1u
    };
    EVP_PKEY *openssl_private = make_fixed_pkey(1);
    EVP_PKEY *openssl_public = make_fixed_pkey(0);
    ano_crypto_csp_handle_t private_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t public_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    unsigned char module_signature[64];
    unsigned char openssl_signature[64];
    size_t written;
    size_t index;
    int ok = openssl_private != NULL && openssl_public != NULL &&
             ecdsa_import_private(&private_key) == ANO_CRYPTO_OK &&
             ecdsa_import_public(&public_key) == ANO_CRYPTO_OK;

    for (index = 0u; ok && index < 4u; ++index) {
        written = 0u;
        ok = ano_crypto_signature_sign(
                 private_key, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                 messages[index], lengths[index], module_signature,
                 sizeof(module_signature), &written) == ANO_CRYPTO_OK &&
             written == sizeof(module_signature) &&
             openssl_verify(openssl_public, messages[index], lengths[index],
                            module_signature) &&
             openssl_sign(openssl_private, messages[index], lengths[index],
                          openssl_signature) &&
             ano_crypto_signature_verify(
                 public_key, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                 messages[index], lengths[index], openssl_signature,
                 sizeof(openssl_signature)) == ANO_CRYPTO_OK;
        *comparisons += 2u;
    }
    if (private_key != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        ok = ano_crypto_key_destroy(private_key) == ANO_CRYPTO_OK && ok;
    }
    if (public_key != ANO_CRYPTO_CSP_INVALID_HANDLE) {
        ok = ano_crypto_key_destroy(public_key) == ANO_CRYPTO_OK && ok;
    }
    EVP_PKEY_free(openssl_private);
    EVP_PKEY_free(openssl_public);
    return ok;
}

static int run_generated_key_compatibility(unsigned *comparisons)
{
    unsigned case_index;
    for (case_index = 0u; case_index < GENERATED_KEY_CASES; ++case_index) {
        EVP_PKEY *openssl_key = generate_p256_key();
        uint8_t x[32];
        uint8_t y[32];
        uint8_t d[32];
        uint8_t message[48];
        uint8_t module_signature[64];
        uint8_t repeated_signature[64];
        uint8_t openssl_signature[64];
        ano_crypto_csp_handle_t private_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
        ano_crypto_csp_handle_t public_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
        size_t written = 0u;
        size_t repeated_written = 0u;
        size_t i;
        int ok = openssl_key != NULL && extract_components(openssl_key, x, y, d);

        for (i = 0u; i < sizeof(message); ++i) {
            message[i] = (uint8_t)(i * 29u + case_index * 17u + 3u);
        }
        if (ok) {
            ok = ano_crypto_ec_p256_key_import(
                     x, sizeof(x), y, sizeof(y), d, sizeof(d),
                     ANO_CRYPTO_KEY_FLAG_NONE, &private_key) == ANO_CRYPTO_OK &&
                 ano_crypto_ec_p256_key_import(
                     x, sizeof(x), y, sizeof(y), NULL, 0u,
                     ANO_CRYPTO_KEY_FLAG_NONE, &public_key) == ANO_CRYPTO_OK;
        }
        if (ok) {
            ok = ano_crypto_signature_sign(
                     private_key, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                     message, sizeof(message), module_signature,
                     sizeof(module_signature), &written) == ANO_CRYPTO_OK &&
                 written == sizeof(module_signature) &&
                 ano_crypto_signature_sign(
                     private_key, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                     message, sizeof(message), repeated_signature,
                     sizeof(repeated_signature), &repeated_written) == ANO_CRYPTO_OK &&
                 repeated_written == sizeof(repeated_signature) &&
                 memcmp(module_signature, repeated_signature,
                        sizeof(module_signature)) == 0 &&
                 openssl_verify(openssl_key, message, sizeof(message),
                                module_signature) &&
                 openssl_sign(openssl_key, message, sizeof(message),
                              openssl_signature) &&
                 ano_crypto_signature_verify(
                     public_key, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                     message, sizeof(message), openssl_signature,
                     sizeof(openssl_signature)) == ANO_CRYPTO_OK;
            *comparisons += 3u;
        }
        if (private_key != ANO_CRYPTO_CSP_INVALID_HANDLE) {
            ok = ano_crypto_key_destroy(private_key) == ANO_CRYPTO_OK && ok;
        }
        if (public_key != ANO_CRYPTO_CSP_INVALID_HANDLE) {
            ok = ano_crypto_key_destroy(public_key) == ANO_CRYPTO_OK && ok;
        }
        OPENSSL_cleanse(d, sizeof(d));
        EVP_PKEY_free(openssl_key);
        if (!ok) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    unsigned comparisons = 0u;
    REQUIRE(ano_crypto_module_init(NULL) == ANO_CRYPTO_OK);
    REQUIRE(run_fixed_operation_compatibility(&comparisons));
    REQUIRE(run_generated_key_compatibility(&comparisons));
    REQUIRE(ano_crypto_module_shutdown() == ANO_CRYPTO_OK);
    printf("OpenSSL ECDSA P-256 differential comparisons passed: %u\n",
           comparisons);
    return 0;
}
