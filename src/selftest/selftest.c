#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_build.h>
#include <anocrypto/ac_selftest.h>
#include "ac_drbg_internal.h"
#include "ac_hmac_internal.h"
#include "ac_internal.h"
#include "ac_kat_manifest.h"
#include "ac_sha256_internal.h"

static ano_crypto_status_t g_selftest_last_status = ANO_CRYPTO_ERR_NOT_INITIALIZED;
static uint32_t g_selftest_last_flags = 0u;

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    if (a == 0 || b == 0) {
        return 0;
    }

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
}

static void fill_repeat(uint8_t *out, size_t out_len, uint8_t value)
{
    size_t i;

    if (out == 0) {
        return;
    }

    for (i = 0u; i < out_len; ++i) {
        out[i] = value;
    }
}

static ano_crypto_status_t ano_crypto_selftest_check_build_identity(void)
{
    ano_crypto_build_identity_t identity;
    ano_crypto_status_t st;

    st = ano_crypto_build_identity(&identity);
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    if (identity.product_name == 0 || identity.version == 0 || identity.compiler == 0) {
        return ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_check_kat_manifest(void)
{
    size_t count;
    size_t i;

    count = ano_crypto_internal_kat_manifest_count();
    if (count == 0u) {
        return ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
    }

    for (i = 0u; i < count; ++i) {
        const ano_crypto_internal_kat_manifest_entry_t *entry;
        entry = ano_crypto_internal_kat_manifest_get(i);
        if (entry == 0 || entry->selftest_id == 0 || entry->algorithm_name == 0 ||
            entry->source_policy == 0 || entry->status == 0) {
            return ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
        }
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_sha256_kat(void)
{
    static const uint8_t msg_abc[] = { 'a', 'b', 'c' };
    static const uint8_t msg_long[] = {
        'a','b','c','d','b','c','d','e','c','d','e','f','d','e','f','g',
        'e','f','g','h','f','g','h','i','g','h','i','j','h','i','j','k',
        'i','j','k','l','j','k','l','m','k','l','m','n','l','m','n','o',
        'm','n','o','p','n','o','p','q'
    };
    static const uint8_t sha256_empty[ANO_CRYPTO_SHA256_DIGEST_SIZE] = {
        0xe3u, 0xb0u, 0xc4u, 0x42u, 0x98u, 0xfcu, 0x1cu, 0x14u,
        0x9au, 0xfbu, 0xf4u, 0xc8u, 0x99u, 0x6fu, 0xb9u, 0x24u,
        0x27u, 0xaeu, 0x41u, 0xe4u, 0x64u, 0x9bu, 0x93u, 0x4cu,
        0xa4u, 0x95u, 0x99u, 0x1bu, 0x78u, 0x52u, 0xb8u, 0x55u
    };
    static const uint8_t sha256_abc[ANO_CRYPTO_SHA256_DIGEST_SIZE] = {
        0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
        0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
        0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
        0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu
    };
    static const uint8_t sha256_long[ANO_CRYPTO_SHA256_DIGEST_SIZE] = {
        0x24u, 0x8du, 0x6au, 0x61u, 0xd2u, 0x06u, 0x38u, 0xb8u,
        0xe5u, 0xc0u, 0x26u, 0x93u, 0x0cu, 0x3eu, 0x60u, 0x39u,
        0xa3u, 0x3cu, 0xe4u, 0x59u, 0x64u, 0xffu, 0x21u, 0x67u,
        0xf6u, 0xecu, 0xedu, 0xd4u, 0x19u, 0xdbu, 0x06u, 0xc1u
    };
    uint8_t out[ANO_CRYPTO_SHA256_DIGEST_SIZE];
    ano_crypto_internal_sha256_ctx_t ctx;
    ano_crypto_status_t st;

    st = ano_crypto_internal_sha256_digest(0, 0u, out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha256_empty, sizeof(out))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_sha256_digest(msg_abc, sizeof(msg_abc), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha256_abc, sizeof(out))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_sha256_init(&ctx);
    st = ano_crypto_internal_sha256_update(&ctx, msg_abc, 1u);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_update(&ctx, msg_abc + 1u, 2u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha256_abc, sizeof(out))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_sha256_digest(msg_long, sizeof(msg_long), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha256_long, sizeof(out))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_sha256_init(&ctx);
    st = ano_crypto_internal_sha256_update(&ctx, msg_long, 13u);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_update(&ctx, msg_long + 13u, sizeof(msg_long) - 13u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha256_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha256_long, sizeof(out))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_zeroize(out, sizeof(out));
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_hmac_sha256_kat(void)
{
    static const uint8_t key_case1[20] = {
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu
    };
    static const uint8_t data_case1[] = { 'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e' };
    static const uint8_t hmac_case1[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE] = {
        0xb0u, 0x34u, 0x4cu, 0x61u, 0xd8u, 0xdbu, 0x38u, 0x53u,
        0x5cu, 0xa8u, 0xafu, 0xceu, 0xafu, 0x0bu, 0xf1u, 0x2bu,
        0x88u, 0x1du, 0xc2u, 0x00u, 0xc9u, 0x83u, 0x3du, 0xa7u,
        0x26u, 0xe9u, 0x37u, 0x6cu, 0x2eu, 0x32u, 0xcfu, 0xf7u
    };
    static const uint8_t key_case2[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t data_case2[] = {
        'w', 'h', 'a', 't', ' ', 'd', 'o', ' ', 'y', 'a', ' ', 'w', 'a', 'n', 't',
        ' ', 'f', 'o', 'r', ' ', 'n', 'o', 't', 'h', 'i', 'n', 'g', '?'
    };
    static const uint8_t hmac_case2[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE] = {
        0x5bu, 0xdcu, 0xc1u, 0x46u, 0xbfu, 0x60u, 0x75u, 0x4eu,
        0x6au, 0x04u, 0x24u, 0x26u, 0x08u, 0x95u, 0x75u, 0xc7u,
        0x5au, 0x00u, 0x3fu, 0x08u, 0x9du, 0x27u, 0x39u, 0x83u,
        0x9du, 0xecu, 0x58u, 0xb9u, 0x64u, 0xecu, 0x38u, 0x43u
    };
    static const uint8_t data_case6[] = {
        'T','e','s','t',' ','U','s','i','n','g',' ','L','a','r','g','e','r',' ',
        'T','h','a','n',' ','B','l','o','c','k','-','S','i','z','e',' ','K','e','y',
        ' ','-',' ','H','a','s','h',' ','K','e','y',' ','F','i','r','s','t'
    };
    static const uint8_t hmac_case6[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE] = {
        0x60u, 0xe4u, 0x31u, 0x59u, 0x1eu, 0xe0u, 0xb6u, 0x7fu,
        0x0du, 0x8au, 0x26u, 0xaau, 0xcbu, 0xf5u, 0xb7u, 0x7fu,
        0x8eu, 0x0bu, 0xc6u, 0x21u, 0x37u, 0x28u, 0xc5u, 0x14u,
        0x05u, 0x46u, 0x04u, 0x0fu, 0x0eu, 0xe3u, 0x7fu, 0x54u
    };
    uint8_t key_case6[131];
    uint8_t out[ANO_CRYPTO_HMAC_SHA256_MAC_SIZE];
    ano_crypto_internal_hmac_sha256_ctx_t ctx;
    ano_crypto_status_t st;

    st = ano_crypto_internal_hmac_sha256_compute(
        key_case1, sizeof(key_case1), data_case1, sizeof(data_case1), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac_case1, sizeof(out))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_hmac_sha256_compute(
        key_case2, sizeof(key_case2), data_case2, sizeof(data_case2), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac_case2, sizeof(out))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_hmac_sha256_init(&ctx, key_case2, sizeof(key_case2));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_update(&ctx, data_case2, 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_update(&ctx, data_case2 + 9u, sizeof(data_case2) - 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac_case2, sizeof(out))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    fill_repeat(key_case6, sizeof(key_case6), 0xaau);
    st = ano_crypto_internal_hmac_sha256_compute(
        key_case6, sizeof(key_case6), data_case6, sizeof(data_case6), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac_case6, sizeof(out))) {
        ano_crypto_internal_zeroize(key_case6, sizeof(key_case6));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_hmac_sha256_init(&ctx, key_case6, sizeof(key_case6));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_update(&ctx, data_case6, 17u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_update(&ctx, data_case6 + 17u, sizeof(data_case6) - 17u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha256_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac_case6, sizeof(out))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(key_case6, sizeof(key_case6));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_zeroize(key_case6, sizeof(key_case6));
    ano_crypto_internal_zeroize(out, sizeof(out));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_run(uint32_t flags)
{
    ano_crypto_status_t st;

    g_selftest_last_flags = flags;

#if defined(ANO_CRYPTO_ENABLE_TEST_HOOKS)
    if ((flags & ANO_CRYPTO_SELFTEST_FLAG_FORCE_FAILURE_FOR_TEST) != 0u) {
        g_selftest_last_status = ANO_CRYPTO_ERR_SELF_TEST_FAILED;
        ano_crypto_internal_module_mark_error(g_selftest_last_status);
        return g_selftest_last_status;
    }
#else
    if ((flags & 0x80000000u) != 0u) {
        g_selftest_last_status = ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        return g_selftest_last_status;
    }
#endif

    st = ano_crypto_selftest_check_build_identity();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_check_kat_manifest();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_sha256_kat();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_hmac_sha256_kat();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_internal_hmac_drbg_sha256_selftest();
    if (st != ANO_CRYPTO_OK) {
        g_selftest_last_status = st;
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    g_selftest_last_status = ANO_CRYPTO_OK;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_status(
    ano_crypto_status_t *out_status)
{
    if (out_status == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_status = g_selftest_last_status;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_flags(
    uint32_t *out_flags)
{
    if (out_flags == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_flags = g_selftest_last_flags;
    return ANO_CRYPTO_OK;
}
