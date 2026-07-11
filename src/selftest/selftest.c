#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_build.h>
#include <anocrypto/ac_selftest.h>
#include "ac_aes_internal.h"
#include "ac_drbg_internal.h"
#include "ac_hmac_internal.h"
#include "ac_internal.h"
#include "ac_kat_manifest.h"
#include "ac_sha256_internal.h"
#include "ac_sha512_internal.h"
#include "ac_thread_internal.h"

static ano_crypto_status_t g_selftest_last_status = ANO_CRYPTO_ERR_NOT_INITIALIZED;
static uint32_t g_selftest_last_flags = 0u;


static void ano_crypto_selftest_set_last_flags(uint32_t flags)
{
    if (ano_crypto_internal_global_lock() == ANO_CRYPTO_OK) {
        g_selftest_last_flags = flags;
        ano_crypto_internal_global_unlock();
    } else {
        g_selftest_last_flags = flags;
    }
}

static void ano_crypto_selftest_set_last_status(ano_crypto_status_t status)
{
    if (ano_crypto_internal_global_lock() == ANO_CRYPTO_OK) {
        g_selftest_last_status = status;
        ano_crypto_internal_global_unlock();
    } else {
        g_selftest_last_status = status;
    }
}

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


static ano_crypto_status_t ano_crypto_selftest_sha512_family_kat(void)
{
    static const uint8_t msg_abc[] = { 'a', 'b', 'c' };
    static const uint8_t sha384_empty[48] = {
        0x38u, 0xb0u, 0x60u, 0xa7u, 0x51u, 0xacu, 0x96u, 0x38u,
        0x4cu, 0xd9u, 0x32u, 0x7eu, 0xb1u, 0xb1u, 0xe3u, 0x6au,
        0x21u, 0xfdu, 0xb7u, 0x11u, 0x14u, 0xbeu, 0x07u, 0x43u,
        0x4cu, 0x0cu, 0xc7u, 0xbfu, 0x63u, 0xf6u, 0xe1u, 0xdau,
        0x27u, 0x4eu, 0xdeu, 0xbfu, 0xe7u, 0x6fu, 0x65u, 0xfbu,
        0xd5u, 0x1au, 0xd2u, 0xf1u, 0x48u, 0x98u, 0xb9u, 0x5bu
    };
    static const uint8_t sha384_abc[48] = {
        0xcbu, 0x00u, 0x75u, 0x3fu, 0x45u, 0xa3u, 0x5eu, 0x8bu,
        0xb5u, 0xa0u, 0x3du, 0x69u, 0x9au, 0xc6u, 0x50u, 0x07u,
        0x27u, 0x2cu, 0x32u, 0xabu, 0x0eu, 0xdeu, 0xd1u, 0x63u,
        0x1au, 0x8bu, 0x60u, 0x5au, 0x43u, 0xffu, 0x5bu, 0xedu,
        0x80u, 0x86u, 0x07u, 0x2bu, 0xa1u, 0xe7u, 0xccu, 0x23u,
        0x58u, 0xbau, 0xecu, 0xa1u, 0x34u, 0xc8u, 0x25u, 0xa7u
    };
    static const uint8_t sha512_empty[64] = {
        0xcfu, 0x83u, 0xe1u, 0x35u, 0x7eu, 0xefu, 0xb8u, 0xbdu,
        0xf1u, 0x54u, 0x28u, 0x50u, 0xd6u, 0x6du, 0x80u, 0x07u,
        0xd6u, 0x20u, 0xe4u, 0x05u, 0x0bu, 0x57u, 0x15u, 0xdcu,
        0x83u, 0xf4u, 0xa9u, 0x21u, 0xd3u, 0x6cu, 0xe9u, 0xceu,
        0x47u, 0xd0u, 0xd1u, 0x3cu, 0x5du, 0x85u, 0xf2u, 0xb0u,
        0xffu, 0x83u, 0x18u, 0xd2u, 0x87u, 0x7eu, 0xecu, 0x2fu,
        0x63u, 0xb9u, 0x31u, 0xbdu, 0x47u, 0x41u, 0x7au, 0x81u,
        0xa5u, 0x38u, 0x32u, 0x7au, 0xf9u, 0x27u, 0xdau, 0x3eu
    };
    static const uint8_t sha512_abc[64] = {
        0xddu, 0xafu, 0x35u, 0xa1u, 0x93u, 0x61u, 0x7au, 0xbau,
        0xccu, 0x41u, 0x73u, 0x49u, 0xaeu, 0x20u, 0x41u, 0x31u,
        0x12u, 0xe6u, 0xfau, 0x4eu, 0x89u, 0xa9u, 0x7eu, 0xa2u,
        0x0au, 0x9eu, 0xeeu, 0xe6u, 0x4bu, 0x55u, 0xd3u, 0x9au,
        0x21u, 0x92u, 0x99u, 0x2au, 0x27u, 0x4fu, 0xc1u, 0xa8u,
        0x36u, 0xbau, 0x3cu, 0x23u, 0xa3u, 0xfeu, 0xebu, 0xbdu,
        0x45u, 0x4du, 0x44u, 0x23u, 0x64u, 0x3cu, 0xe8u, 0x0eu,
        0x2au, 0x9au, 0xc9u, 0x4fu, 0xa5u, 0x4cu, 0xa4u, 0x9fu
    };
    uint8_t out[ANO_CRYPTO_SHA512_DIGEST_SIZE];
    ano_crypto_internal_sha512_ctx_t ctx;
    ano_crypto_status_t st;

    st = ano_crypto_internal_sha384_digest(0, 0u, out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha384_empty, sizeof(sha384_empty))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    st = ano_crypto_internal_sha384_digest(msg_abc, sizeof(msg_abc), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha384_abc, sizeof(sha384_abc))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    ano_crypto_internal_sha384_init(&ctx);
    st = ano_crypto_internal_sha512_update(&ctx, msg_abc, 1u);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha512_update(&ctx, msg_abc + 1u, 2u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha384_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha384_abc, sizeof(sha384_abc))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_sha512_digest(0, 0u, out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha512_empty, sizeof(sha512_empty))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    st = ano_crypto_internal_sha512_digest(msg_abc, sizeof(msg_abc), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha512_abc, sizeof(sha512_abc))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    ano_crypto_internal_sha512_init(&ctx);
    st = ano_crypto_internal_sha512_update(&ctx, msg_abc, 1u);
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha512_update(&ctx, msg_abc + 1u, 2u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_sha512_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, sha512_abc, sizeof(sha512_abc))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_zeroize(out, sizeof(out));
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_hmac_sha512_family_kat(void)
{
    static const uint8_t key_case1[20] = {
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu
    };
    static const uint8_t data_case1[] = { 'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e' };
    static const uint8_t key_case2[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t data_case2[] = {
        'w','h','a','t',' ','d','o',' ','y','a',' ','w','a','n','t',' ',
        'f','o','r',' ','n','o','t','h','i','n','g','?'
    };
    static const uint8_t hmac384_case1[48] = {
        0xafu, 0xd0u, 0x39u, 0x44u, 0xd8u, 0x48u, 0x95u, 0x62u,
        0x6bu, 0x08u, 0x25u, 0xf4u, 0xabu, 0x46u, 0x90u, 0x7fu,
        0x15u, 0xf9u, 0xdau, 0xdbu, 0xe4u, 0x10u, 0x1eu, 0xc6u,
        0x82u, 0xaau, 0x03u, 0x4cu, 0x7cu, 0xebu, 0xc5u, 0x9cu,
        0xfau, 0xeau, 0x9eu, 0xa9u, 0x07u, 0x6eu, 0xdeu, 0x7fu,
        0x4au, 0xf1u, 0x52u, 0xe8u, 0xb2u, 0xfau, 0x9cu, 0xb6u
    };
    static const uint8_t hmac384_case2[48] = {
        0xafu, 0x45u, 0xd2u, 0xe3u, 0x76u, 0x48u, 0x40u, 0x31u,
        0x61u, 0x7fu, 0x78u, 0xd2u, 0xb5u, 0x8au, 0x6bu, 0x1bu,
        0x9cu, 0x7eu, 0xf4u, 0x64u, 0xf5u, 0xa0u, 0x1bu, 0x47u,
        0xe4u, 0x2eu, 0xc3u, 0x73u, 0x63u, 0x22u, 0x44u, 0x5eu,
        0x8eu, 0x22u, 0x40u, 0xcau, 0x5eu, 0x69u, 0xe2u, 0xc7u,
        0x8bu, 0x32u, 0x39u, 0xecu, 0xfau, 0xb2u, 0x16u, 0x49u
    };
    static const uint8_t hmac512_case1[64] = {
        0x87u, 0xaau, 0x7cu, 0xdeu, 0xa5u, 0xefu, 0x61u, 0x9du,
        0x4fu, 0xf0u, 0xb4u, 0x24u, 0x1au, 0x1du, 0x6cu, 0xb0u,
        0x23u, 0x79u, 0xf4u, 0xe2u, 0xceu, 0x4eu, 0xc2u, 0x78u,
        0x7au, 0xd0u, 0xb3u, 0x05u, 0x45u, 0xe1u, 0x7cu, 0xdeu,
        0xdau, 0xa8u, 0x33u, 0xb7u, 0xd6u, 0xb8u, 0xa7u, 0x02u,
        0x03u, 0x8bu, 0x27u, 0x4eu, 0xaeu, 0xa3u, 0xf4u, 0xe4u,
        0xbeu, 0x9du, 0x91u, 0x4eu, 0xebu, 0x61u, 0xf1u, 0x70u,
        0x2eu, 0x69u, 0x6cu, 0x20u, 0x3au, 0x12u, 0x68u, 0x54u
    };
    static const uint8_t hmac512_case2[64] = {
        0x16u, 0x4bu, 0x7au, 0x7bu, 0xfcu, 0xf8u, 0x19u, 0xe2u,
        0xe3u, 0x95u, 0xfbu, 0xe7u, 0x3bu, 0x56u, 0xe0u, 0xa3u,
        0x87u, 0xbdu, 0x64u, 0x22u, 0x2eu, 0x83u, 0x1fu, 0xd6u,
        0x10u, 0x27u, 0x0cu, 0xd7u, 0xeau, 0x25u, 0x05u, 0x54u,
        0x97u, 0x58u, 0xbfu, 0x75u, 0xc0u, 0x5au, 0x99u, 0x4au,
        0x6du, 0x03u, 0x4fu, 0x65u, 0xf8u, 0xf0u, 0xe6u, 0xfdu,
        0xcau, 0xeau, 0xb1u, 0xa3u, 0x4du, 0x4au, 0x6bu, 0x4bu,
        0x63u, 0x6eu, 0x07u, 0x0au, 0x38u, 0xbcu, 0xe7u, 0x37u
    };
    uint8_t out[ANO_CRYPTO_HMAC_SHA512_MAC_SIZE];
    ano_crypto_internal_hmac_sha512_family_ctx_t ctx;
    ano_crypto_status_t st;

    st = ano_crypto_internal_hmac_sha512_family_compute(
        ANO_CRYPTO_HASH_ALG_SHA384,
        key_case1, sizeof(key_case1), data_case1, sizeof(data_case1), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac384_case1, sizeof(hmac384_case1))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    st = ano_crypto_internal_hmac_sha512_family_init(
        &ctx, ANO_CRYPTO_HASH_ALG_SHA384, key_case2, sizeof(key_case2));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_update(&ctx, data_case2, 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_update(
            &ctx, data_case2 + 9u, sizeof(data_case2) - 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac384_case2, sizeof(hmac384_case2))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    st = ano_crypto_internal_hmac_sha512_family_compute(
        ANO_CRYPTO_HASH_ALG_SHA512,
        key_case1, sizeof(key_case1), data_case1, sizeof(data_case1), out);
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac512_case1, sizeof(hmac512_case1))) {
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    st = ano_crypto_internal_hmac_sha512_family_init(
        &ctx, ANO_CRYPTO_HASH_ALG_SHA512, key_case2, sizeof(key_case2));
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_update(&ctx, data_case2, 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_update(
            &ctx, data_case2 + 9u, sizeof(data_case2) - 9u);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_hmac_sha512_family_final(&ctx, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, hmac512_case2, sizeof(hmac512_case2))) {
        ano_crypto_internal_zeroize(&ctx, sizeof(ctx));
        ano_crypto_internal_zeroize(out, sizeof(out));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_zeroize(out, sizeof(out));
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_selftest_aes_cbc_kat(void)
{
    static const uint8_t plaintext[16] = {
        0x00u,0x11u,0x22u,0x33u,0x44u,0x55u,0x66u,0x77u,
        0x88u,0x99u,0xaau,0xbbu,0xccu,0xddu,0xeeu,0xffu
    };
    static const uint8_t keys[3][32] = {
        {0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,
         0x08u,0x09u,0x0au,0x0bu,0x0cu,0x0du,0x0eu,0x0fu},
        {0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,
         0x08u,0x09u,0x0au,0x0bu,0x0cu,0x0du,0x0eu,0x0fu,
         0x10u,0x11u,0x12u,0x13u,0x14u,0x15u,0x16u,0x17u},
        {0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,
         0x08u,0x09u,0x0au,0x0bu,0x0cu,0x0du,0x0eu,0x0fu,
         0x10u,0x11u,0x12u,0x13u,0x14u,0x15u,0x16u,0x17u,
         0x18u,0x19u,0x1au,0x1bu,0x1cu,0x1du,0x1eu,0x1fu}
    };
    static const size_t key_lens[3] = {16u, 24u, 32u};
    static const uint8_t expected[3][16] = {
        {0x69u,0xc4u,0xe0u,0xd8u,0x6au,0x7bu,0x04u,0x30u,0xd8u,0xcdu,0xb7u,0x80u,0x70u,0xb4u,0xc5u,0x5au},
        {0xddu,0xa9u,0x7cu,0xa4u,0x86u,0x4cu,0xdfu,0xe0u,0x6eu,0xafu,0x70u,0xa0u,0xecu,0x0du,0x71u,0x91u},
        {0x8eu,0xa2u,0xb7u,0xcau,0x51u,0x67u,0x45u,0xbfu,0xeau,0xfcu,0x49u,0x90u,0x4bu,0x49u,0x60u,0x89u}
    };
    static const uint8_t cbc_key[16] = {
        0x2bu,0x7eu,0x15u,0x16u,0x28u,0xaeu,0xd2u,0xa6u,
        0xabu,0xf7u,0x15u,0x88u,0x09u,0xcfu,0x4fu,0x3cu
    };
    static const uint8_t cbc_iv[16] = {
        0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,
        0x08u,0x09u,0x0au,0x0bu,0x0cu,0x0du,0x0eu,0x0fu
    };
    static const uint8_t cbc_plaintext[16] = {
        0x6bu,0xc1u,0xbeu,0xe2u,0x2eu,0x40u,0x9fu,0x96u,
        0xe9u,0x3du,0x7eu,0x11u,0x73u,0x93u,0x17u,0x2au
    };
    static const uint8_t cbc_ciphertext[16] = {
        0x76u,0x49u,0xabu,0xacu,0x81u,0x19u,0xb2u,0x46u,
        0xceu,0xe9u,0x8eu,0x9bu,0x12u,0xe9u,0x19u,0x7du
    };
    ano_crypto_internal_aes_ctx_t ctx;
    uint8_t out[16];
    uint8_t block[16];
    size_t i;
    ano_crypto_status_t st;

    for (i = 0u; i < 3u; ++i) {
        st = ano_crypto_internal_aes_init(&ctx, keys[i], key_lens[i]);
        if (st == ANO_CRYPTO_OK) {
            st = ano_crypto_internal_aes_encrypt_block(&ctx, plaintext, out);
        }
        if (st != ANO_CRYPTO_OK || !bytes_equal(out, expected[i], sizeof(out))) {
            ano_crypto_internal_aes_uninit(&ctx);
            ano_crypto_internal_zeroize(out, sizeof(out));
            return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
        }
        st = ano_crypto_internal_aes_decrypt_block(&ctx, out, block);
        if (st != ANO_CRYPTO_OK || !bytes_equal(block, plaintext, sizeof(block))) {
            ano_crypto_internal_aes_uninit(&ctx);
            ano_crypto_internal_zeroize(out, sizeof(out));
            ano_crypto_internal_zeroize(block, sizeof(block));
            return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
        }
        ano_crypto_internal_aes_uninit(&ctx);
    }

    st = ano_crypto_internal_aes_init(&ctx, cbc_key, sizeof(cbc_key));
    for (i = 0u; i < sizeof(block); ++i) {
        block[i] = (uint8_t)(cbc_plaintext[i] ^ cbc_iv[i]);
    }
    if (st == ANO_CRYPTO_OK) {
        st = ano_crypto_internal_aes_encrypt_block(&ctx, block, out);
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(out, cbc_ciphertext, sizeof(out))) {
        ano_crypto_internal_aes_uninit(&ctx);
        ano_crypto_internal_zeroize(out, sizeof(out));
        ano_crypto_internal_zeroize(block, sizeof(block));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }
    st = ano_crypto_internal_aes_decrypt_block(&ctx, cbc_ciphertext, block);
    if (st == ANO_CRYPTO_OK) {
        for (i = 0u; i < sizeof(block); ++i) {
            block[i] = (uint8_t)(block[i] ^ cbc_iv[i]);
        }
    }
    if (st != ANO_CRYPTO_OK || !bytes_equal(block, cbc_plaintext, sizeof(block))) {
        ano_crypto_internal_aes_uninit(&ctx);
        ano_crypto_internal_zeroize(out, sizeof(out));
        ano_crypto_internal_zeroize(block, sizeof(block));
        return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
    }

    ano_crypto_internal_aes_uninit(&ctx);
    ano_crypto_internal_zeroize(out, sizeof(out));
    ano_crypto_internal_zeroize(block, sizeof(block));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_run(uint32_t flags)
{
    ano_crypto_status_t st;

    ano_crypto_selftest_set_last_flags(flags);

    /* High test-only flag values are never part of the installed/public ABI. */
    if ((flags & 0x80000000u) != 0u) {
        ano_crypto_selftest_set_last_status(ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_selftest_check_build_identity();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_check_kat_manifest();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_sha256_kat();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_hmac_sha256_kat();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_sha512_family_kat();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_hmac_sha512_family_kat();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_selftest_aes_cbc_kat();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    st = ano_crypto_internal_hmac_drbg_sha256_selftest();
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_selftest_set_last_status(st);
        ano_crypto_internal_module_mark_error(st);
        return st;
    }

    ano_crypto_selftest_set_last_status(ANO_CRYPTO_OK);
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_status(
    ano_crypto_status_t *out_status)
{
    ano_crypto_status_t st;

    if (out_status == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_status = g_selftest_last_status;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_selftest_get_last_flags(
    uint32_t *out_flags)
{
    ano_crypto_status_t st;

    if (out_flags == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    *out_flags = g_selftest_last_flags;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}
