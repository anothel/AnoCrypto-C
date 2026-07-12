#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;

static int equal_bytes(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t difference = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        difference = (uint8_t)(difference | (uint8_t)(a[i] ^ b[i]));
    }
    return difference == 0u;
}

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n", name,
                ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++failures;
    }
}

static void run_vector(
    const char *name,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *message,
    size_t message_len,
    const uint8_t expected[ANO_CRYPTO_CMAC_TAG_SIZE])
{
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t *ctx = 0;
    size_t ctx_size = 0u;
    size_t written = 0u;
    size_t offset = 0u;
    size_t chunk = 1u;
    ano_crypto_status_t st;

    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_CMAC, key, key_len,
        ANO_CRYPTO_KEY_FLAG_NONE, &handle);
    expect_status(name, st, ANO_CRYPTO_OK);
    if (st != ANO_CRYPTO_OK) {
        return;
    }

    st = ano_crypto_cmac_compute_with_key(
        handle, message, message_len, tag, sizeof(tag), &written,
        ANO_CRYPTO_CMAC_FLAG_NONE);
    expect_status(name, st, ANO_CRYPTO_OK);
    if (st == ANO_CRYPTO_OK &&
        (written != sizeof(tag) || !equal_bytes(tag, expected, sizeof(tag)))) {
        fprintf(stderr, "FAIL %s one-shot result mismatch\n", name);
        ++failures;
    }

    st = ano_crypto_cmac_verify_with_key(
        handle, message, message_len, expected, ANO_CRYPTO_CMAC_TAG_SIZE,
        ANO_CRYPTO_CMAC_FLAG_NONE);
    expect_status(name, st, ANO_CRYPTO_OK);

    memcpy(altered, expected, sizeof(altered));
    altered[sizeof(altered) - 1u] ^= 0x01u;
    st = ano_crypto_cmac_verify_with_key(
        handle, message, message_len, altered, sizeof(altered),
        ANO_CRYPTO_CMAC_FLAG_NONE);
    expect_status(name, st, ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);

    st = ano_crypto_cmac_context_size(&ctx_size);
    expect_status(name, st, ANO_CRYPTO_OK);
    if (st == ANO_CRYPTO_OK) {
        ctx = (uint8_t *)calloc(1u, ctx_size);
    }
    if (ctx == 0) {
        fprintf(stderr, "FAIL %s context allocation\n", name);
        ++failures;
    } else {
        st = ano_crypto_cmac_init(
            ctx, ctx_size, handle, ANO_CRYPTO_CMAC_FLAG_NONE);
        expect_status(name, st, ANO_CRYPTO_OK);
        while (st == ANO_CRYPTO_OK && offset < message_len) {
            size_t take = chunk;
            if (take > message_len - offset) {
                take = message_len - offset;
            }
            st = ano_crypto_cmac_update(ctx, message + offset, take);
            offset += take;
            chunk = (chunk % 7u) + 1u;
        }
        if (message_len == 0u && st == ANO_CRYPTO_OK) {
            st = ano_crypto_cmac_update(ctx, 0, 0u);
        }
        expect_status(name, st, ANO_CRYPTO_OK);
        if (st == ANO_CRYPTO_OK) {
            written = 0u;
            st = ano_crypto_cmac_final(ctx, tag, sizeof(tag), &written);
            expect_status(name, st, ANO_CRYPTO_OK);
            if (st == ANO_CRYPTO_OK &&
                (written != sizeof(tag) || !equal_bytes(tag, expected, sizeof(tag)))) {
                fprintf(stderr, "FAIL %s streaming result mismatch\n", name);
                ++failures;
            }
        }
        (void)ano_crypto_cmac_uninit(ctx, ctx_size);
        free(ctx);
    }

    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));
    expect_status(name, ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
}

int main(void)
{
    static const uint8_t message[64] = {
        0x6bu,0xc1u,0xbeu,0xe2u,0x2eu,0x40u,0x9fu,0x96u,0xe9u,0x3du,0x7eu,0x11u,0x73u,0x93u,0x17u,0x2au,
        0xaeu,0x2du,0x8au,0x57u,0x1eu,0x03u,0xacu,0x9cu,0x9eu,0xb7u,0x6fu,0xacu,0x45u,0xafu,0x8eu,0x51u,
        0x30u,0xc8u,0x1cu,0x46u,0xa3u,0x5cu,0xe4u,0x11u,0xe5u,0xfbu,0xc1u,0x19u,0x1au,0x0au,0x52u,0xefu,
        0xf6u,0x9fu,0x24u,0x45u,0xdfu,0x4fu,0x9bu,0x17u,0xadu,0x2bu,0x41u,0x7bu,0xe6u,0x6cu,0x37u,0x10u
    };
    static const uint8_t key128[16] = {
        0x2bu,0x7eu,0x15u,0x16u,0x28u,0xaeu,0xd2u,0xa6u,0xabu,0xf7u,0x15u,0x88u,0x09u,0xcfu,0x4fu,0x3cu
    };
    static const uint8_t key192[24] = {
        0x8eu,0x73u,0xb0u,0xf7u,0xdau,0x0eu,0x64u,0x52u,0xc8u,0x10u,0xf3u,0x2bu,0x80u,0x90u,0x79u,0xe5u,
        0x62u,0xf8u,0xeau,0xd2u,0x52u,0x2cu,0x6bu,0x7bu
    };
    static const uint8_t key256[32] = {
        0x60u,0x3du,0xebu,0x10u,0x15u,0xcau,0x71u,0xbeu,0x2bu,0x73u,0xaeu,0xf0u,0x85u,0x7du,0x77u,0x81u,
        0x1fu,0x35u,0x2cu,0x07u,0x3bu,0x61u,0x08u,0xd7u,0x2du,0x98u,0x10u,0xa3u,0x09u,0x14u,0xdfu,0xf4u
    };
    static const uint8_t tags128[4][16] = {
        {0xbbu,0x1du,0x69u,0x29u,0xe9u,0x59u,0x37u,0x28u,0x7fu,0xa3u,0x7du,0x12u,0x9bu,0x75u,0x67u,0x46u},
        {0x07u,0x0au,0x16u,0xb4u,0x6bu,0x4du,0x41u,0x44u,0xf7u,0x9bu,0xddu,0x9du,0xd0u,0x4au,0x28u,0x7cu},
        {0x7du,0x85u,0x44u,0x9eu,0xa6u,0xeau,0x19u,0xc8u,0x23u,0xa7u,0xbfu,0x78u,0x83u,0x7du,0xfau,0xdeu},
        {0x51u,0xf0u,0xbeu,0xbfu,0x7eu,0x3bu,0x9du,0x92u,0xfcu,0x49u,0x74u,0x17u,0x79u,0x36u,0x3cu,0xfeu}
    };
    static const uint8_t tags192[4][16] = {
        {0xd1u,0x7du,0xdfu,0x46u,0xadu,0xaau,0xcdu,0xe5u,0x31u,0xcau,0xc4u,0x83u,0xdeu,0x7au,0x93u,0x67u},
        {0x9eu,0x99u,0xa7u,0xbfu,0x31u,0xe7u,0x10u,0x90u,0x06u,0x62u,0xf6u,0x5eu,0x61u,0x7cu,0x51u,0x84u},
        {0x3du,0x75u,0xc1u,0x94u,0xedu,0x96u,0x07u,0x04u,0x44u,0xa9u,0xfau,0x7eu,0xc7u,0x40u,0xecu,0xf8u},
        {0xa1u,0xd5u,0xdfu,0x0eu,0xedu,0x79u,0x0fu,0x79u,0x4du,0x77u,0x58u,0x96u,0x59u,0xf3u,0x9au,0x11u}
    };
    static const uint8_t tags256[4][16] = {
        {0x02u,0x89u,0x62u,0xf6u,0x1bu,0x7bu,0xf8u,0x9eu,0xfcu,0x6bu,0x55u,0x1fu,0x46u,0x67u,0xd9u,0x83u},
        {0x28u,0xa7u,0x02u,0x3fu,0x45u,0x2eu,0x8fu,0x82u,0xbdu,0x4bu,0xf2u,0x8du,0x8cu,0x37u,0xc3u,0x5cu},
        {0x15u,0x67u,0x27u,0xdcu,0x08u,0x78u,0x94u,0x4au,0x02u,0x3cu,0x1fu,0xe0u,0x3bu,0xadu,0x6du,0x93u},
        {0xe1u,0x99u,0x21u,0x90u,0x54u,0x9fu,0x6eu,0xd5u,0x69u,0x6au,0x2cu,0x05u,0x6cu,0x31u,0x54u,0x10u}
    };
    static const size_t lengths[4] = {0u, 16u, 20u, 64u};
    size_t i;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    for (i = 0u; i < 4u; ++i) {
        run_vector("AES-128-CMAC", key128, sizeof(key128),
                   lengths[i] == 0u ? 0 : message, lengths[i], tags128[i]);
        run_vector("AES-192-CMAC", key192, sizeof(key192),
                   lengths[i] == 0u ? 0 : message, lengths[i], tags192[i]);
        run_vector("AES-256-CMAC", key256, sizeof(key256),
                   lengths[i] == 0u ? 0 : message, lengths[i], tags256[i]);
    }
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (failures != 0) {
        fprintf(stderr, "AES-CMAC KAT failures: %d\n", failures);
        return 1;
    }
    printf("AES-128/192/256 CMAC KATs passed (tags not printed)\n");
    return 0;
}
