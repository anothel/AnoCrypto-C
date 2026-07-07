#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

int main(void)
{
    const char *version = 0;
    ano_crypto_module_state_t state;
    ano_crypto_build_identity_t identity;
    size_t size = 0u;
    ano_crypto_status_t st;

    st = ano_crypto_version_string(&version);
    if (st != ANO_CRYPTO_OK || version == 0) {
        fprintf(stderr, "version query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_build_identity(&identity);
    if (st != ANO_CRYPTO_OK || identity.product_name == 0) {
        fprintf(stderr, "build identity query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_hash_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "hash context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_hmac_context_size(ANO_CRYPTO_HASH_ALG_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "hmac context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_drbg_context_size(ANO_CRYPTO_DRBG_ALG_HMAC_SHA256, &size);
    if (st != ANO_CRYPTO_OK || size == 0u) {
        fprintf(stderr, "drbg context size query failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_get_state(&state);
    if (st != ANO_CRYPTO_OK || state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        fprintf(stderr, "unexpected module state after init\n");
        return 1;
    }

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module shutdown failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    printf("AnoCrypto-C install smoke: PASS (%s)\n", version);
    return 0;
}
