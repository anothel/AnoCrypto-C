#include <stdio.h>
#include <anocrypto/anocrypto.h>

int main(void)
{
    ano_crypto_status_t st;
    ano_crypto_build_identity_t identity;
    ano_crypto_module_state_t state;

    st = ano_crypto_build_identity(&identity);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "build identity failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    printf("%s %s\n", identity.product_name, identity.version);
    printf("compiler=%s target=%s commit=%s\n",
           identity.compiler, identity.target, identity.git_commit);

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_ERR_NOT_INITIALIZED) {
        fprintf(stderr, "service gate before init returned unexpected status: %s\n",
                ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_get_state(&state);
    if (st != ANO_CRYPTO_OK || state != ANO_CRYPTO_MODULE_STATE_OPERATIONAL) {
        fprintf(stderr, "module state check failed\n");
        return 1;
    }

    st = ano_crypto_module_require_operational();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "service gate after init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module shutdown failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    puts("AnoCrypto-C install smoke: PASS");
    return 0;
}
