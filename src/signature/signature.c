#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_hash.h>
#include <anocrypto/ac_module.h>
#include <anocrypto/ac_signature.h>
#include "ac_ecdsa_internal.h"
#include "ac_entropy_internal.h"
#include "ac_internal.h"
#include "ac_rsa_internal.h"
#include "ac_sha256_internal.h"

static const uint8_t k_sha256_digest_info_prefix[] = {
    0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20
};
static const uint8_t k_sha384_digest_info_prefix[] = {
    0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30
};
static const uint8_t k_sha512_digest_info_prefix[] = {
    0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40
};

typedef struct signature_digest_spec_s {
    ano_crypto_hash_algorithm_t hash_algorithm;
    const uint8_t *prefix;
    size_t prefix_len;
    size_t digest_len;
    size_t signature_len;
    int is_rsa;
} signature_digest_spec_t;

static ano_crypto_status_t signature_spec(
    ano_crypto_signature_algorithm_t algorithm,
    signature_digest_spec_t *out)
{
    if (out == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    ano_crypto_internal_zeroize(out, sizeof(*out));
    switch (algorithm) {
    case ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256:
        out->hash_algorithm = ANO_CRYPTO_HASH_ALG_SHA256;
        out->prefix = k_sha256_digest_info_prefix;
        out->prefix_len = sizeof(k_sha256_digest_info_prefix);
        out->digest_len = ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE;
        out->signature_len = ANO_CRYPTO_RSA2048_SIGNATURE_SIZE;
        out->is_rsa = 1;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384:
        out->hash_algorithm = ANO_CRYPTO_HASH_ALG_SHA384;
        out->prefix = k_sha384_digest_info_prefix;
        out->prefix_len = sizeof(k_sha384_digest_info_prefix);
        out->digest_len = ANO_CRYPTO_HASH_SHA384_DIGEST_SIZE;
        out->signature_len = ANO_CRYPTO_RSA2048_SIGNATURE_SIZE;
        out->is_rsa = 1;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512:
        out->hash_algorithm = ANO_CRYPTO_HASH_ALG_SHA512;
        out->prefix = k_sha512_digest_info_prefix;
        out->prefix_len = sizeof(k_sha512_digest_info_prefix);
        out->digest_len = ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE;
        out->signature_len = ANO_CRYPTO_RSA2048_SIGNATURE_SIZE;
        out->is_rsa = 1;
        return ANO_CRYPTO_OK;
    case ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256:
        out->hash_algorithm = ANO_CRYPTO_HASH_ALG_SHA256;
        out->digest_len = ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE;
        out->signature_len = ANO_CRYPTO_ECDSA_P256_SIGNATURE_SIZE;
        out->is_rsa = 0;
        return ANO_CRYPTO_OK;
    default:
        return ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM;
    }
}

static int ranges_overlap(const void *a, size_t a_len, const void *b, size_t b_len)
{
    uintptr_t ap;
    uintptr_t bp;
    if (a_len == 0u || b_len == 0u || a == 0 || b == 0) {
        return 0;
    }
    ap = (uintptr_t)a;
    bp = (uintptr_t)b;
    if (ap > UINTPTR_MAX - a_len || bp > UINTPTR_MAX - b_len) {
        return 1;
    }
    return ap < bp + b_len && bp < ap + a_len;
}

static ano_crypto_status_t encode_rsa_message(
    const signature_digest_spec_t *spec,
    const uint8_t *message,
    size_t message_len,
    uint8_t encoded[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE])
{
    uint8_t digest[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t digest_written = 0u;
    size_t ps_len;
    ano_crypto_status_t st;

    if (spec == 0 || spec->is_rsa == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_hash_digest(
        spec->hash_algorithm, message, message_len,
        digest, sizeof(digest), &digest_written);
    if (st != ANO_CRYPTO_OK || digest_written != spec->digest_len) {
        ano_crypto_internal_zeroize(digest, sizeof(digest));
        return st == ANO_CRYPTO_OK ? ANO_CRYPTO_ERR_INTERNAL_ERROR : st;
    }
    ps_len = ANO_CRYPTO_RSA2048_SIGNATURE_SIZE - 3u - spec->prefix_len - spec->digest_len;
    encoded[0] = 0x00u;
    encoded[1] = 0x01u;
    memset(encoded + 2u, 0xff, ps_len);
    encoded[2u + ps_len] = 0x00u;
    memcpy(encoded + 3u + ps_len, spec->prefix, spec->prefix_len);
    memcpy(encoded + 3u + ps_len + spec->prefix_len, digest, spec->digest_len);
    ano_crypto_internal_zeroize(digest, sizeof(digest));
    return ANO_CRYPTO_OK;
}

static int constant_time_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        diff |= (uint8_t)(a[i] ^ b[i]);
    }
    return diff == 0u ? 1 : 0;
}

static ano_crypto_status_t sign_rsa(
    ano_crypto_csp_handle_t private_key,
    const signature_digest_spec_t *spec,
    const uint8_t *message,
    size_t message_len,
    uint8_t *out_signature)
{
    uint8_t encoded[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE];
    uint8_t blind[8];
    const ano_crypto_rsa_key_internal_t *key = 0;
    ano_crypto_status_t st;

    st = encode_rsa_message(spec, message, message_len, encoded);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(encoded, sizeof(encoded));
        return st;
    }
    st = ano_crypto_internal_platform_entropy_get(blind, sizeof(blind));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(encoded, sizeof(encoded));
        ano_crypto_internal_zeroize(blind, sizeof(blind));
        return st;
    }
    st = ano_crypto_internal_csp_lock_rsa(private_key, 1, &key);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_rsa_private(key, encoded, blind, out_signature);
        ano_crypto_internal_csp_unlock_rsa();
    }
    ano_crypto_internal_zeroize(encoded, sizeof(encoded));
    ano_crypto_internal_zeroize(blind, sizeof(blind));
    return st;
}

static ano_crypto_status_t sign_ecdsa(
    ano_crypto_csp_handle_t private_key,
    const uint8_t *message,
    size_t message_len,
    uint8_t *out_signature)
{
    uint8_t digest[32];
    const ano_crypto_ec_p256_key_internal_t *key = 0;
    ano_crypto_status_t st;

    st = ano_crypto_internal_sha256_digest(message, message_len, digest);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(digest, sizeof(digest));
        return st;
    }
    st = ano_crypto_internal_csp_lock_ec_p256(private_key, 1, &key);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_ecdsa_p256_sign_digest(key, digest, out_signature);
        ano_crypto_internal_csp_unlock_ec_p256();
    }
    ano_crypto_internal_zeroize(digest, sizeof(digest));
    return st;
}

ano_crypto_status_t ano_crypto_signature_sign(
    ano_crypto_csp_handle_t private_key,
    ano_crypto_signature_algorithm_t algorithm,
    const uint8_t *message,
    size_t message_len,
    uint8_t *out_signature,
    size_t out_signature_len,
    size_t *out_written)
{
    signature_digest_spec_t spec;
    ano_crypto_status_t st;

    if (out_written == 0) {
        if (out_signature != 0 && out_signature_len != 0u) {
            ano_crypto_internal_zeroize(out_signature, out_signature_len);
        }
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if ((out_signature != 0 && out_signature_len != 0u &&
         ranges_overlap(out_written, sizeof(*out_written), out_signature, out_signature_len)) ||
        ranges_overlap(out_written, sizeof(*out_written), message, message_len)) {
        if (out_signature != 0 && out_signature_len != 0u) {
            ano_crypto_internal_zeroize(out_signature, out_signature_len);
        }
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_written = 0u;
    if (message == 0 && message_len != 0u) {
        if (out_signature != 0 && out_signature_len != 0u) {
            ano_crypto_internal_zeroize(out_signature, out_signature_len);
        }
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = signature_spec(algorithm, &spec);
    if (st != ANO_CRYPTO_OK) {
        if (out_signature != 0 && out_signature_len != 0u) {
            ano_crypto_internal_zeroize(out_signature, out_signature_len);
        }
        return st;
    }
    if (out_signature == 0 && out_signature_len == 0u) {
        *out_written = spec.signature_len;
        return ANO_CRYPTO_OK;
    }
    if (out_signature == 0 || out_signature_len < spec.signature_len) {
        if (out_signature != 0 && out_signature_len != 0u) {
            ano_crypto_internal_zeroize(out_signature, out_signature_len);
        }
        return out_signature == 0 ? ANO_CRYPTO_ERR_INVALID_ARGUMENT : ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }
    if (ranges_overlap(message, message_len, out_signature, out_signature_len)) {
        ano_crypto_internal_zeroize(out_signature, out_signature_len);
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out_signature, out_signature_len);
        return st;
    }
    if (spec.is_rsa != 0) {
        st = sign_rsa(private_key, &spec, message, message_len, out_signature);
    } else {
        st = sign_ecdsa(private_key, message, message_len, out_signature);
    }
    if (st == ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED) {
        ano_crypto_internal_zeroize(out_signature, out_signature_len);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out_signature, out_signature_len);
        return st;
    }
    *out_written = spec.signature_len;
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t verify_rsa(
    ano_crypto_csp_handle_t key_handle,
    const signature_digest_spec_t *spec,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *signature)
{
    uint8_t expected[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE];
    uint8_t recovered[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE];
    const ano_crypto_rsa_key_internal_t *key = 0;
    ano_crypto_status_t st;
    int equal;

    st = encode_rsa_message(spec, message, message_len, expected);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(expected, sizeof(expected));
        return st;
    }
    st = ano_crypto_internal_csp_lock_rsa(key_handle, 0, &key);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_rsa_public(key, signature, recovered);
        ano_crypto_internal_csp_unlock_rsa();
    }
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(expected, sizeof(expected));
        ano_crypto_internal_zeroize(recovered, sizeof(recovered));
        return st;
    }
    equal = constant_time_equal(expected, recovered, sizeof(expected));
    ano_crypto_internal_zeroize(expected, sizeof(expected));
    ano_crypto_internal_zeroize(recovered, sizeof(recovered));
    return equal != 0 ? ANO_CRYPTO_OK : ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
}

static ano_crypto_status_t verify_ecdsa(
    ano_crypto_csp_handle_t key_handle,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *signature)
{
    uint8_t digest[32];
    const ano_crypto_ec_p256_key_internal_t *key = 0;
    ano_crypto_status_t st;

    st = ano_crypto_internal_sha256_digest(message, message_len, digest);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(digest, sizeof(digest));
        return st;
    }
    st = ano_crypto_internal_csp_lock_ec_p256(key_handle, 0, &key);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_ecdsa_p256_verify_digest(key, digest, signature);
        ano_crypto_internal_csp_unlock_ec_p256();
    }
    ano_crypto_internal_zeroize(digest, sizeof(digest));
    return st;
}

ano_crypto_status_t ano_crypto_signature_verify(
    ano_crypto_csp_handle_t key_handle,
    ano_crypto_signature_algorithm_t algorithm,
    const uint8_t *message,
    size_t message_len,
    const uint8_t *signature,
    size_t signature_len)
{
    signature_digest_spec_t spec;
    ano_crypto_status_t st;

    if ((message == 0 && message_len != 0u) || signature == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = signature_spec(algorithm, &spec);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (signature_len != spec.signature_len) {
        return ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
    }
    if (ranges_overlap(message, message_len, signature, signature_len)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    if (spec.is_rsa != 0) {
        return verify_rsa(key_handle, &spec, message, message_len, signature);
    }
    return verify_ecdsa(key_handle, message, message_len, signature);
}
