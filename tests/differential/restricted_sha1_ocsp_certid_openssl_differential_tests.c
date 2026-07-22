#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include <openssl/evp.h>
#include <openssl/ocsp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

static int fail(const char *message)
{
    fprintf(stderr, "restricted SHA-1 OCSP differential failure: %s\n", message);
    return 1;
}

static int digest_matches_openssl(const uint8_t *input, size_t input_len)
{
    uint8_t actual[ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE];
    uint8_t expected[EVP_MAX_MD_SIZE];
    unsigned int expected_len = 0u;
    size_t written = 0u;

    if (ano_crypto_ocsp_certid_sha1_digest(
            input, input_len, actual, sizeof(actual), &written) != ANO_CRYPTO_OK) {
        return 0;
    }
    if (EVP_Digest(input, input_len, expected, &expected_len, EVP_sha1(), 0) != 1) {
        return 0;
    }
    return written == sizeof(actual) && expected_len == sizeof(actual) &&
           memcmp(actual, expected, sizeof(actual)) == 0;
}

int main(void)
{
    EVP_PKEY_CTX *keygen = 0;
    EVP_PKEY *key = 0;
    X509 *issuer = 0;
    X509_NAME *name = 0;
    OCSP_CERTID *id = 0;
    ASN1_OCTET_STRING *name_hash = 0;
    ASN1_OBJECT *hash_algorithm = 0;
    ASN1_OCTET_STRING *key_hash = 0;
    ASN1_INTEGER *serial = 0;
    const ASN1_BIT_STRING *public_key_bits = 0;
    unsigned char *name_der = 0;
    unsigned char *cursor;
    int name_der_len;
    uint8_t actual_name_hash[ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE];
    uint8_t actual_key_hash[ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE];
    size_t written = 0u;
    int result = 1;
    size_t index;

    if (ano_crypto_module_init(0) != ANO_CRYPTO_OK) {
        return fail("module init");
    }

    for (index = 0u; index < 64u; ++index) {
        uint8_t input[257];
        size_t length = (index * 37u) % sizeof(input);
        size_t inner;
        for (inner = 0u; inner < length; ++inner) {
            input[inner] = (uint8_t)(inner * 13u + index * 7u);
        }
        if (!digest_matches_openssl(input, length)) {
            goto cleanup;
        }
    }

    keygen = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0);
    if (keygen == 0 || EVP_PKEY_keygen_init(keygen) != 1 ||
        EVP_PKEY_CTX_set_rsa_keygen_bits(keygen, 2048) != 1 ||
        EVP_PKEY_keygen(keygen, &key) != 1) {
        goto cleanup;
    }

    issuer = X509_new();
    name = X509_NAME_new();
    if (issuer == 0 || name == 0 ||
        X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC,
            (const unsigned char *)"KR", -1, -1, 0) != 1 ||
        X509_NAME_add_entry_by_txt(name, "O", MBSTRING_UTF8,
            (const unsigned char *)"AnoPKI", -1, -1, 0) != 1 ||
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_UTF8,
            (const unsigned char *)"OCSP CertID Test CA", -1, -1, 0) != 1 ||
        X509_set_subject_name(issuer, name) != 1 ||
        X509_set_issuer_name(issuer, name) != 1 ||
        X509_set_pubkey(issuer, key) != 1 ||
        ASN1_INTEGER_set(X509_get_serialNumber(issuer), 1) != 1) {
        goto cleanup;
    }

    id = OCSP_cert_to_id(EVP_sha1(), issuer, issuer);
    if (id == 0 || OCSP_id_get0_info(
            &name_hash, &hash_algorithm, &key_hash, &serial, id) != 1 ||
        OBJ_obj2nid(hash_algorithm) != NID_sha1 ||
        name_hash == 0 || key_hash == 0 ||
        name_hash->length != (int)ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE ||
        key_hash->length != (int)ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE) {
        goto cleanup;
    }

    name_der_len = i2d_X509_NAME(X509_get_subject_name(issuer), 0);
    if (name_der_len <= 0) {
        goto cleanup;
    }
    name_der = OPENSSL_malloc((size_t)name_der_len);
    if (name_der == 0) {
        goto cleanup;
    }
    cursor = name_der;
    if (i2d_X509_NAME(X509_get_subject_name(issuer), &cursor) != name_der_len) {
        goto cleanup;
    }

    public_key_bits = X509_get0_pubkey_bitstr(issuer);
    if (public_key_bits == 0 || public_key_bits->data == 0 || public_key_bits->length <= 0) {
        goto cleanup;
    }

    if (ano_crypto_ocsp_certid_sha1_digest(
            name_der, (size_t)name_der_len, actual_name_hash,
            sizeof(actual_name_hash), &written) != ANO_CRYPTO_OK ||
        written != sizeof(actual_name_hash) ||
        memcmp(actual_name_hash, name_hash->data, sizeof(actual_name_hash)) != 0) {
        goto cleanup;
    }
    if (ano_crypto_ocsp_certid_sha1_digest(
            public_key_bits->data, (size_t)public_key_bits->length,
            actual_key_hash, sizeof(actual_key_hash), &written) != ANO_CRYPTO_OK ||
        written != sizeof(actual_key_hash) ||
        memcmp(actual_key_hash, key_hash->data, sizeof(actual_key_hash)) != 0) {
        goto cleanup;
    }

    result = 0;

cleanup:
    if (name_der != 0) {
        OPENSSL_free(name_der);
    }
    OCSP_CERTID_free(id);
    X509_NAME_free(name);
    X509_free(issuer);
    EVP_PKEY_free(key);
    EVP_PKEY_CTX_free(keygen);
    (void)ano_crypto_module_shutdown();

    if (result != 0) {
        return fail("comparison or OCSP CertID byte extraction");
    }
    puts("restricted_sha1_ocsp_certid_openssl_differential_tests: PASS comparisons=66");
    return 0;
}
