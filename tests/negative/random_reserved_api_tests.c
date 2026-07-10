#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int expect_status(ano_crypto_status_t actual, ano_crypto_status_t expected)
{
    return actual == expected ? 0 : 1;
}

static int bytes_are_zero(const uint8_t *buf, size_t len)
{
    size_t i;

    if (buf == 0) {
        return 0;
    }

    for (i = 0u; i < len; ++i) {
        if (buf[i] != 0u) {
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    uint8_t random_ctx[64];
    uint8_t out[32];
    uint8_t additional_input[1] = {0x42u};
    size_t ctx_size = 1234u;
    ano_crypto_status_t st;

    if (ano_crypto_random_is_available() != 0) {
        return 1;
    }

    if (ano_crypto_random_service_name() == 0) {
        return 2;
    }

    if (expect_status(ano_crypto_random_context_size(0), ANO_CRYPTO_ERR_INVALID_ARGUMENT) != 0) {
        return 3;
    }
    st = ano_crypto_random_context_size(&ctx_size);
    if (expect_status(st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED) != 0 || ctx_size != 0u) {
        return 4;
    }

    memset(random_ctx, 0xA5, sizeof(random_ctx));
    memset(out, 0x5A, sizeof(out));

    st = ano_crypto_random_init(
        random_ctx, sizeof(random_ctx), 0, 1u, ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (expect_status(st, ANO_CRYPTO_ERR_INVALID_ARGUMENT) != 0) {
        return 5;
    }

    st = ano_crypto_random_generate(
        random_ctx, out, sizeof(out), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_PREDICTION_RESISTANCE_REQUESTED);
    if (expect_status(st, ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER) != 0 || !bytes_are_zero(out, sizeof(out))) {
        return 6;
    }

    memset(out, 0x5A, sizeof(out));
    st = ano_crypto_random_generate(
        random_ctx, out, sizeof(out), additional_input, sizeof(additional_input), ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (expect_status(st, ANO_CRYPTO_ERR_NOT_INITIALIZED) != 0 || !bytes_are_zero(out, sizeof(out))) {
        return 7;
    }

    st = ano_crypto_module_init(0);
    if (expect_status(st, ANO_CRYPTO_OK) != 0) {
        return 8;
    }

    memset(random_ctx, 0xA5, sizeof(random_ctx));
    st = ano_crypto_random_init(random_ctx, sizeof(random_ctx), 0, 0u, ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (expect_status(st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED) != 0 || !bytes_are_zero(random_ctx, sizeof(random_ctx))) {
        (void)ano_crypto_module_shutdown();
        return 9;
    }

    memset(out, 0x5A, sizeof(out));
    st = ano_crypto_random_generate(
        random_ctx, out, sizeof(out), additional_input, sizeof(additional_input), ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (expect_status(st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED) != 0 || !bytes_are_zero(out, sizeof(out))) {
        (void)ano_crypto_module_shutdown();
        return 10;
    }

    st = ano_crypto_random_reseed(random_ctx, additional_input, sizeof(additional_input), ANO_CRYPTO_RANDOM_FLAG_NONE);
    if (expect_status(st, ANO_CRYPTO_ERR_NOT_IMPLEMENTED) != 0) {
        (void)ano_crypto_module_shutdown();
        return 11;
    }

    memset(random_ctx, 0xA5, sizeof(random_ctx));
    st = ano_crypto_random_uninit(random_ctx, sizeof(random_ctx));
    if (expect_status(st, ANO_CRYPTO_OK) != 0 || !bytes_are_zero(random_ctx, sizeof(random_ctx))) {
        (void)ano_crypto_module_shutdown();
        return 12;
    }

    st = ano_crypto_module_shutdown();
    if (expect_status(st, ANO_CRYPTO_OK) != 0) {
        return 13;
    }

    return 0;
}
