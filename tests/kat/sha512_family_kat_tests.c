#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }
    return diff == 0u;
}

static void fill_repeat(uint8_t *out, size_t out_len, uint8_t value)
{
    size_t i;
    for (i = 0u; i < out_len; ++i) {
        out[i] = value;
    }
}

static void fill_sequence(uint8_t *out, size_t out_len, uint8_t start)
{
    size_t i;
    for (i = 0u; i < out_len; ++i) {
        out[i] = (uint8_t)(start + (uint8_t)i);
    }
}

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
    static const uint8_t sha384_long[48] = {
        0x09u, 0x33u, 0x0cu, 0x33u, 0xf7u, 0x11u, 0x47u, 0xe8u,
        0x3du, 0x19u, 0x2fu, 0xc7u, 0x82u, 0xcdu, 0x1bu, 0x47u,
        0x53u, 0x11u, 0x1bu, 0x17u, 0x3bu, 0x3bu, 0x05u, 0xd2u,
        0x2fu, 0xa0u, 0x80u, 0x86u, 0xe3u, 0xb0u, 0xf7u, 0x12u,
        0xfcu, 0xc7u, 0xc7u, 0x1au, 0x55u, 0x7eu, 0x2du, 0xb9u,
        0x66u, 0xc3u, 0xe9u, 0xfau, 0x91u, 0x74u, 0x60u, 0x39u
    };
    static const uint8_t sha384_million[48] = {
        0x9du, 0x0eu, 0x18u, 0x09u, 0x71u, 0x64u, 0x74u, 0xcbu,
        0x08u, 0x6eu, 0x83u, 0x4eu, 0x31u, 0x0au, 0x4au, 0x1cu,
        0xedu, 0x14u, 0x9eu, 0x9cu, 0x00u, 0xf2u, 0x48u, 0x52u,
        0x79u, 0x72u, 0xceu, 0xc5u, 0x70u, 0x4cu, 0x2au, 0x5bu,
        0x07u, 0xb8u, 0xb3u, 0xdcu, 0x38u, 0xecu, 0xc4u, 0xebu,
        0xaeu, 0x97u, 0xddu, 0xd8u, 0x7fu, 0x3du, 0x89u, 0x85u
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
    static const uint8_t sha512_long[64] = {
        0x8eu, 0x95u, 0x9bu, 0x75u, 0xdau, 0xe3u, 0x13u, 0xdau,
        0x8cu, 0xf4u, 0xf7u, 0x28u, 0x14u, 0xfcu, 0x14u, 0x3fu,
        0x8fu, 0x77u, 0x79u, 0xc6u, 0xebu, 0x9fu, 0x7fu, 0xa1u,
        0x72u, 0x99u, 0xaeu, 0xadu, 0xb6u, 0x88u, 0x90u, 0x18u,
        0x50u, 0x1du, 0x28u, 0x9eu, 0x49u, 0x00u, 0xf7u, 0xe4u,
        0x33u, 0x1bu, 0x99u, 0xdeu, 0xc4u, 0xb5u, 0x43u, 0x3au,
        0xc7u, 0xd3u, 0x29u, 0xeeu, 0xb6u, 0xddu, 0x26u, 0x54u,
        0x5eu, 0x96u, 0xe5u, 0x5bu, 0x87u, 0x4bu, 0xe9u, 0x09u
    };
    static const uint8_t sha512_million[64] = {
        0xe7u, 0x18u, 0x48u, 0x3du, 0x0cu, 0xe7u, 0x69u, 0x64u,
        0x4eu, 0x2eu, 0x42u, 0xc7u, 0xbcu, 0x15u, 0xb4u, 0x63u,
        0x8eu, 0x1fu, 0x98u, 0xb1u, 0x3bu, 0x20u, 0x44u, 0x28u,
        0x56u, 0x32u, 0xa8u, 0x03u, 0xafu, 0xa9u, 0x73u, 0xebu,
        0xdeu, 0x0fu, 0xf2u, 0x44u, 0x87u, 0x7eu, 0xa6u, 0x0au,
        0x4cu, 0xb0u, 0x43u, 0x2cu, 0xe5u, 0x77u, 0xc3u, 0x1bu,
        0xebu, 0x00u, 0x9cu, 0x5cu, 0x2cu, 0x49u, 0xaau, 0x2eu,
        0x4eu, 0xadu, 0xb2u, 0x17u, 0xadu, 0x8cu, 0xc0u, 0x9bu
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
    static const uint8_t hmac384_case3[48] = {
        0x88u, 0x06u, 0x26u, 0x08u, 0xd3u, 0xe6u, 0xadu, 0x8au,
        0x0au, 0xa2u, 0xacu, 0xe0u, 0x14u, 0xc8u, 0xa8u, 0x6fu,
        0x0au, 0xa6u, 0x35u, 0xd9u, 0x47u, 0xacu, 0x9fu, 0xebu,
        0xe8u, 0x3eu, 0xf4u, 0xe5u, 0x59u, 0x66u, 0x14u, 0x4bu,
        0x2au, 0x5au, 0xb3u, 0x9du, 0xc1u, 0x38u, 0x14u, 0xb9u,
        0x4eu, 0x3au, 0xb6u, 0xe1u, 0x01u, 0xa3u, 0x4fu, 0x27u
    };
    static const uint8_t hmac384_case4[48] = {
        0x3eu, 0x8au, 0x69u, 0xb7u, 0x78u, 0x3cu, 0x25u, 0x85u,
        0x19u, 0x33u, 0xabu, 0x62u, 0x90u, 0xafu, 0x6cu, 0xa7u,
        0x7au, 0x99u, 0x81u, 0x48u, 0x08u, 0x50u, 0x00u, 0x9cu,
        0xc5u, 0x57u, 0x7cu, 0x6eu, 0x1fu, 0x57u, 0x3bu, 0x4eu,
        0x68u, 0x01u, 0xddu, 0x23u, 0xc4u, 0xa7u, 0xd6u, 0x79u,
        0xccu, 0xf8u, 0xa3u, 0x86u, 0xc6u, 0x74u, 0xcfu, 0xfbu
    };
    static const uint8_t hmac384_case7[48] = {
        0x66u, 0x17u, 0x17u, 0x8eu, 0x94u, 0x1fu, 0x02u, 0x0du,
        0x35u, 0x1eu, 0x2fu, 0x25u, 0x4eu, 0x8fu, 0xd3u, 0x2cu,
        0x60u, 0x24u, 0x20u, 0xfeu, 0xb0u, 0xb8u, 0xfbu, 0x9au,
        0xdcu, 0xceu, 0xbbu, 0x82u, 0x46u, 0x1eu, 0x99u, 0xc5u,
        0xa6u, 0x78u, 0xccu, 0x31u, 0xe7u, 0x99u, 0x17u, 0x6du,
        0x38u, 0x60u, 0xe6u, 0x11u, 0x0cu, 0x46u, 0x52u, 0x3eu
    };
    static const uint8_t hmac384_case6[48] = {
        0x4eu, 0xceu, 0x08u, 0x44u, 0x85u, 0x81u, 0x3eu, 0x90u,
        0x88u, 0xd2u, 0xc6u, 0x3au, 0x04u, 0x1bu, 0xc5u, 0xb4u,
        0x4fu, 0x9eu, 0xf1u, 0x01u, 0x2au, 0x2bu, 0x58u, 0x8fu,
        0x3cu, 0xd1u, 0x1fu, 0x05u, 0x03u, 0x3au, 0xc4u, 0xc6u,
        0x0cu, 0x2eu, 0xf6u, 0xabu, 0x40u, 0x30u, 0xfeu, 0x82u,
        0x96u, 0x24u, 0x8du, 0xf1u, 0x63u, 0xf4u, 0x49u, 0x52u
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
    static const uint8_t hmac512_case3[64] = {
        0xfau, 0x73u, 0xb0u, 0x08u, 0x9du, 0x56u, 0xa2u, 0x84u,
        0xefu, 0xb0u, 0xf0u, 0x75u, 0x6cu, 0x89u, 0x0bu, 0xe9u,
        0xb1u, 0xb5u, 0xdbu, 0xddu, 0x8eu, 0xe8u, 0x1au, 0x36u,
        0x55u, 0xf8u, 0x3eu, 0x33u, 0xb2u, 0x27u, 0x9du, 0x39u,
        0xbfu, 0x3eu, 0x84u, 0x82u, 0x79u, 0xa7u, 0x22u, 0xc8u,
        0x06u, 0xb4u, 0x85u, 0xa4u, 0x7eu, 0x67u, 0xc8u, 0x07u,
        0xb9u, 0x46u, 0xa3u, 0x37u, 0xbeu, 0xe8u, 0x94u, 0x26u,
        0x74u, 0x27u, 0x88u, 0x59u, 0xe1u, 0x32u, 0x92u, 0xfbu
    };
    static const uint8_t hmac512_case4[64] = {
        0xb0u, 0xbau, 0x46u, 0x56u, 0x37u, 0x45u, 0x8cu, 0x69u,
        0x90u, 0xe5u, 0xa8u, 0xc5u, 0xf6u, 0x1du, 0x4au, 0xf7u,
        0xe5u, 0x76u, 0xd9u, 0x7fu, 0xf9u, 0x4bu, 0x87u, 0x2du,
        0xe7u, 0x6fu, 0x80u, 0x50u, 0x36u, 0x1eu, 0xe3u, 0xdbu,
        0xa9u, 0x1cu, 0xa5u, 0xc1u, 0x1au, 0xa2u, 0x5eu, 0xb4u,
        0xd6u, 0x79u, 0x27u, 0x5cu, 0xc5u, 0x78u, 0x80u, 0x63u,
        0xa5u, 0xf1u, 0x97u, 0x41u, 0x12u, 0x0cu, 0x4fu, 0x2du,
        0xe2u, 0xadu, 0xebu, 0xebu, 0x10u, 0xa2u, 0x98u, 0xddu
    };
    static const uint8_t hmac512_case7[64] = {
        0xe3u, 0x7bu, 0x6au, 0x77u, 0x5du, 0xc8u, 0x7du, 0xbau,
        0xa4u, 0xdfu, 0xa9u, 0xf9u, 0x6eu, 0x5eu, 0x3fu, 0xfdu,
        0xdeu, 0xbdu, 0x71u, 0xf8u, 0x86u, 0x72u, 0x89u, 0x86u,
        0x5du, 0xf5u, 0xa3u, 0x2du, 0x20u, 0xcdu, 0xc9u, 0x44u,
        0xb6u, 0x02u, 0x2cu, 0xacu, 0x3cu, 0x49u, 0x82u, 0xb1u,
        0x0du, 0x5eu, 0xebu, 0x55u, 0xc3u, 0xe4u, 0xdeu, 0x15u,
        0x13u, 0x46u, 0x76u, 0xfbu, 0x6du, 0xe0u, 0x44u, 0x60u,
        0x65u, 0xc9u, 0x74u, 0x40u, 0xfau, 0x8cu, 0x6au, 0x58u
    };
    static const uint8_t hmac512_case6[64] = {
        0x80u, 0xb2u, 0x42u, 0x63u, 0xc7u, 0xc1u, 0xa3u, 0xebu,
        0xb7u, 0x14u, 0x93u, 0xc1u, 0xddu, 0x7bu, 0xe8u, 0xb4u,
        0x9bu, 0x46u, 0xd1u, 0xf4u, 0x1bu, 0x4au, 0xeeu, 0xc1u,
        0x12u, 0x1bu, 0x01u, 0x37u, 0x83u, 0xf8u, 0xf3u, 0x52u,
        0x6bu, 0x56u, 0xd0u, 0x37u, 0xe0u, 0x5fu, 0x25u, 0x98u,
        0xbdu, 0x0fu, 0xd2u, 0x21u, 0x5du, 0x6au, 0x1eu, 0x52u,
        0x95u, 0xe6u, 0x4fu, 0x73u, 0xf6u, 0x3fu, 0x0au, 0xecu,
        0x8bu, 0x91u, 0x5au, 0x98u, 0x5du, 0x78u, 0x65u, 0x98u
    };

static const uint8_t msg_abc[] = { 'a', 'b', 'c' };
static const uint8_t msg_long[] = {
    'a','b','c','d','e','f','g','h','b','c','d','e','f','g','h','i',
    'c','d','e','f','g','h','i','j','d','e','f','g','h','i','j','k',
    'e','f','g','h','i','j','k','l','f','g','h','i','j','k','l','m',
    'g','h','i','j','k','l','m','n','h','i','j','k','l','m','n','o',
    'i','j','k','l','m','n','o','p','j','k','l','m','n','o','p','q',
    'k','l','m','n','o','p','q','r','l','m','n','o','p','q','r','s',
    'm','n','o','p','q','r','s','t','n','o','p','q','r','s','t','u'
};

static void hash_one_shot_expect(
    const char *name,
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *input,
    size_t input_len,
    const uint8_t *expected,
    size_t expected_len)
{
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t written = 0u;

    memset(out, 0, sizeof(out));
    expect_status(name,
                  ano_crypto_hash_digest(algorithm, input, input_len,
                                         out, sizeof(out), &written),
                  ANO_CRYPTO_OK);
    if (written != expected_len || !bytes_equal(out, expected, expected_len)) {
        fprintf(stderr, "FAIL %s: digest mismatch or length %lu\n",
                name, (unsigned long)written);
        ++g_failures;
    }
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
}

static void hash_stream_expect(
    const char *name,
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *input,
    size_t input_len,
    const uint8_t *expected,
    size_t expected_len)
{
    uint8_t ctx[512];
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    static const size_t chunk_pattern[] = { 1u, 7u, 31u, 64u, 127u };
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk_index = 0u;

    expect_status("hash context size", ano_crypto_hash_context_size(algorithm, &ctx_size), ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL %s: context size %lu\n", name, (unsigned long)ctx_size);
        ++g_failures;
        return;
    }
    expect_status("hash stream init", ano_crypto_hash_init(ctx, ctx_size, algorithm), ANO_CRYPTO_OK);
    while (offset < input_len) {
        size_t take = chunk_pattern[chunk_index % (sizeof(chunk_pattern) / sizeof(chunk_pattern[0]))];
        if (take > input_len - offset) {
            take = input_len - offset;
        }
        expect_status("hash stream update", ano_crypto_hash_update(ctx, input + offset, take), ANO_CRYPTO_OK);
        offset += take;
        ++chunk_index;
    }
    expect_status("hash stream final", ano_crypto_hash_final(ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
    if (written != expected_len || !bytes_equal(out, expected, expected_len)) {
        fprintf(stderr, "FAIL %s: streaming digest mismatch or length %lu\n",
                name, (unsigned long)written);
        ++g_failures;
    }
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
    (void)ano_crypto_csp_zeroize(ctx, sizeof(ctx));
}

static void hash_million_a_expect(
    const char *name,
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *expected,
    size_t expected_len)
{
    uint8_t ctx[512];
    uint8_t block[1000];
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t i;

    fill_repeat(block, sizeof(block), (uint8_t)'a');
    expect_status("million-a context size", ano_crypto_hash_context_size(algorithm, &ctx_size), ANO_CRYPTO_OK);
    if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
        fprintf(stderr, "FAIL %s: context size %lu\n", name, (unsigned long)ctx_size);
        ++g_failures;
        return;
    }
    expect_status("million-a init", ano_crypto_hash_init(ctx, ctx_size, algorithm), ANO_CRYPTO_OK);
    for (i = 0u; i < 1000u; ++i) {
        expect_status("million-a update", ano_crypto_hash_update(ctx, block, sizeof(block)), ANO_CRYPTO_OK);
    }
    expect_status("million-a final", ano_crypto_hash_final(ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
    if (written != expected_len || !bytes_equal(out, expected, expected_len)) {
        fprintf(stderr, "FAIL %s: million-a mismatch or length %lu\n",
                name, (unsigned long)written);
        ++g_failures;
    }
    (void)ano_crypto_csp_zeroize(block, sizeof(block));
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
    (void)ano_crypto_csp_zeroize(ctx, sizeof(ctx));
}

static void test_sha512_family_hash_kats(void)
{
    hash_one_shot_expect("sha384 empty", ANO_CRYPTO_HASH_ALG_SHA384,
                         0, 0u, sha384_empty, sizeof(sha384_empty));
    hash_one_shot_expect("sha384 abc", ANO_CRYPTO_HASH_ALG_SHA384,
                         msg_abc, sizeof(msg_abc), sha384_abc, sizeof(sha384_abc));
    hash_one_shot_expect("sha384 long", ANO_CRYPTO_HASH_ALG_SHA384,
                         msg_long, sizeof(msg_long), sha384_long, sizeof(sha384_long));
    hash_stream_expect("sha384 long streaming", ANO_CRYPTO_HASH_ALG_SHA384,
                       msg_long, sizeof(msg_long), sha384_long, sizeof(sha384_long));
    hash_million_a_expect("sha384 million-a", ANO_CRYPTO_HASH_ALG_SHA384,
                          sha384_million, sizeof(sha384_million));

    hash_one_shot_expect("sha512 empty", ANO_CRYPTO_HASH_ALG_SHA512,
                         0, 0u, sha512_empty, sizeof(sha512_empty));
    hash_one_shot_expect("sha512 abc", ANO_CRYPTO_HASH_ALG_SHA512,
                         msg_abc, sizeof(msg_abc), sha512_abc, sizeof(sha512_abc));
    hash_one_shot_expect("sha512 long", ANO_CRYPTO_HASH_ALG_SHA512,
                         msg_long, sizeof(msg_long), sha512_long, sizeof(sha512_long));
    hash_stream_expect("sha512 long streaming", ANO_CRYPTO_HASH_ALG_SHA512,
                       msg_long, sizeof(msg_long), sha512_long, sizeof(sha512_long));
    hash_million_a_expect("sha512 million-a", ANO_CRYPTO_HASH_ALG_SHA512,
                          sha512_million, sizeof(sha512_million));
}

static void hmac_expect(
    const char *name,
    ano_crypto_hash_algorithm_t algorithm,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *data,
    size_t data_len,
    const uint8_t *expected,
    size_t expected_len,
    int streaming)
{
    uint8_t ctx[512];
    uint8_t out[ANO_CRYPTO_HASH_SHA512_DIGEST_SIZE];
    size_t ctx_size = 0u;
    size_t written = 0u;

    memset(out, 0, sizeof(out));
    if (streaming == 0) {
        expect_status(name,
                      ano_crypto_hmac_compute(algorithm, key, key_len, data, data_len,
                                              out, sizeof(out), &written),
                      ANO_CRYPTO_OK);
    } else {
        size_t split = data_len > 9u ? 9u : data_len;
        expect_status("hmac context size", ano_crypto_hmac_context_size(algorithm, &ctx_size), ANO_CRYPTO_OK);
        if (ctx_size == 0u || ctx_size > sizeof(ctx)) {
            fprintf(stderr, "FAIL %s: context size %lu\n", name, (unsigned long)ctx_size);
            ++g_failures;
            return;
        }
        expect_status("hmac stream init", ano_crypto_hmac_init(ctx, ctx_size, algorithm, key, key_len), ANO_CRYPTO_OK);
        expect_status("hmac stream update first", ano_crypto_hmac_update(ctx, data, split), ANO_CRYPTO_OK);
        expect_status("hmac stream update second", ano_crypto_hmac_update(ctx, data + split, data_len - split), ANO_CRYPTO_OK);
        expect_status("hmac stream final", ano_crypto_hmac_final(ctx, out, sizeof(out), &written), ANO_CRYPTO_OK);
        (void)ano_crypto_csp_zeroize(ctx, sizeof(ctx));
    }

    if (written != expected_len || !bytes_equal(out, expected, expected_len)) {
        fprintf(stderr, "FAIL %s: MAC mismatch or length %lu\n",
                name, (unsigned long)written);
        ++g_failures;
    }
    (void)ano_crypto_csp_zeroize(out, sizeof(out));
}

static void test_hmac_sha512_family_kats(void)
{
    static const uint8_t key_case1[20] = {
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu, 0x0bu,
        0x0bu, 0x0bu, 0x0bu, 0x0bu
    };
    static const uint8_t data_case1[] = { 'H','i',' ','T','h','e','r','e' };
    static const uint8_t key_case2[] = { 'J','e','f','e' };
    static const uint8_t data_case2[] = {
        'w','h','a','t',' ','d','o',' ','y','a',' ','w','a','n','t',' ',
        'f','o','r',' ','n','o','t','h','i','n','g','?'
    };
    static const uint8_t data_case6[] = {
        'T','e','s','t',' ','U','s','i','n','g',' ','L','a','r','g','e','r',' ',
        'T','h','a','n',' ','B','l','o','c','k','-','S','i','z','e',' ','K','e','y',
        ' ','-',' ','H','a','s','h',' ','K','e','y',' ','F','i','r','s','t'
    };
    static const uint8_t data_case7[] = {
        'T','h','i','s',' ','i','s',' ','a',' ','t','e','s','t',' ','u','s','i','n','g',' ',
        'a',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','b','l','o','c','k','-','s','i','z','e',' ',
        'k','e','y',' ','a','n','d',' ','a',' ','l','a','r','g','e','r',' ','t','h','a','n',' ','b','l','o','c','k','-','s','i','z','e',' ',
        'd','a','t','a','.',' ','T','h','e',' ','k','e','y',' ','n','e','e','d','s',' ','t','o',' ','b','e',' ',
        'h','a','s','h','e','d',' ','b','e','f','o','r','e',' ','b','e','i','n','g',' ','u','s','e','d',' ',
        'b','y',' ','t','h','e',' ','H','M','A','C',' ','a','l','g','o','r','i','t','h','m','.'
    };
    uint8_t key_case3[20];
    uint8_t data_case3[50];
    uint8_t key_case4[25];
    uint8_t data_case4[50];
    uint8_t key_long[131];

    fill_repeat(key_case3, sizeof(key_case3), 0xaau);
    fill_repeat(data_case3, sizeof(data_case3), 0xddu);
    fill_sequence(key_case4, sizeof(key_case4), 1u);
    fill_repeat(data_case4, sizeof(data_case4), 0xcdu);
    fill_repeat(key_long, sizeof(key_long), 0xaau);

    hmac_expect("hmac384 case1", ANO_CRYPTO_HASH_ALG_SHA384,
                key_case1, sizeof(key_case1), data_case1, sizeof(data_case1),
                hmac384_case1, sizeof(hmac384_case1), 0);
    hmac_expect("hmac384 case2 streaming", ANO_CRYPTO_HASH_ALG_SHA384,
                key_case2, sizeof(key_case2), data_case2, sizeof(data_case2),
                hmac384_case2, sizeof(hmac384_case2), 1);
    hmac_expect("hmac384 case3", ANO_CRYPTO_HASH_ALG_SHA384,
                key_case3, sizeof(key_case3), data_case3, sizeof(data_case3),
                hmac384_case3, sizeof(hmac384_case3), 0);
    hmac_expect("hmac384 case4", ANO_CRYPTO_HASH_ALG_SHA384,
                key_case4, sizeof(key_case4), data_case4, sizeof(data_case4),
                hmac384_case4, sizeof(hmac384_case4), 0);
    hmac_expect("hmac384 case6", ANO_CRYPTO_HASH_ALG_SHA384,
                key_long, sizeof(key_long), data_case6, sizeof(data_case6),
                hmac384_case6, sizeof(hmac384_case6), 0);
    hmac_expect("hmac384 case7", ANO_CRYPTO_HASH_ALG_SHA384,
                key_long, sizeof(key_long), data_case7, sizeof(data_case7),
                hmac384_case7, sizeof(hmac384_case7), 0);

    hmac_expect("hmac512 case1", ANO_CRYPTO_HASH_ALG_SHA512,
                key_case1, sizeof(key_case1), data_case1, sizeof(data_case1),
                hmac512_case1, sizeof(hmac512_case1), 0);
    hmac_expect("hmac512 case2 streaming", ANO_CRYPTO_HASH_ALG_SHA512,
                key_case2, sizeof(key_case2), data_case2, sizeof(data_case2),
                hmac512_case2, sizeof(hmac512_case2), 1);
    hmac_expect("hmac512 case3", ANO_CRYPTO_HASH_ALG_SHA512,
                key_case3, sizeof(key_case3), data_case3, sizeof(data_case3),
                hmac512_case3, sizeof(hmac512_case3), 0);
    hmac_expect("hmac512 case4", ANO_CRYPTO_HASH_ALG_SHA512,
                key_case4, sizeof(key_case4), data_case4, sizeof(data_case4),
                hmac512_case4, sizeof(hmac512_case4), 0);
    hmac_expect("hmac512 case6", ANO_CRYPTO_HASH_ALG_SHA512,
                key_long, sizeof(key_long), data_case6, sizeof(data_case6),
                hmac512_case6, sizeof(hmac512_case6), 0);
    hmac_expect("hmac512 case7", ANO_CRYPTO_HASH_ALG_SHA512,
                key_long, sizeof(key_long), data_case7, sizeof(data_case7),
                hmac512_case7, sizeof(hmac512_case7), 0);

    (void)ano_crypto_csp_zeroize(key_case3, sizeof(key_case3));
    (void)ano_crypto_csp_zeroize(data_case3, sizeof(data_case3));
    (void)ano_crypto_csp_zeroize(key_case4, sizeof(key_case4));
    (void)ano_crypto_csp_zeroize(data_case4, sizeof(data_case4));
    (void)ano_crypto_csp_zeroize(key_long, sizeof(key_long));
}

int main(void)
{
    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    test_sha512_family_hash_kats();
    test_hmac_sha512_family_kats();
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "sha512_family_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("sha512_family_kat_tests: PASS");
    return 0;
}
