#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

#define CHECK_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "build identity ABI check failed at %s:%d: %s\n", \
                __FILE__, __LINE__, #expr); \
        return 1; \
    } \
} while (0)

int main(void)
{
    ano_crypto_build_identity_t identity;
    ano_crypto_status_t st;

    CHECK_TRUE(ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION == 1u);
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, abi_version) == 0u);
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, product_name) <
               offsetof(ano_crypto_build_identity_t, version));
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, version) <
               offsetof(ano_crypto_build_identity_t, git_commit));
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, git_commit) <
               offsetof(ano_crypto_build_identity_t, build_type));
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, build_type) <
               offsetof(ano_crypto_build_identity_t, compiler));
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, compiler) <
               offsetof(ano_crypto_build_identity_t, target));
    CHECK_TRUE(offsetof(ano_crypto_build_identity_t, target) <
               offsetof(ano_crypto_build_identity_t, notes));
    CHECK_TRUE(sizeof(identity) >= sizeof(uint32_t) + (7u * sizeof(const char *)));

    st = ano_crypto_build_identity(&identity);
    CHECK_TRUE(st == ANO_CRYPTO_OK);
    CHECK_TRUE(identity.abi_version == ANO_CRYPTO_BUILD_IDENTITY_ABI_VERSION);
    CHECK_TRUE(identity.product_name != 0);
    CHECK_TRUE(identity.version != 0);
    CHECK_TRUE(identity.git_commit != 0);
    CHECK_TRUE(identity.build_type != 0);
    CHECK_TRUE(identity.compiler != 0);
    CHECK_TRUE(identity.target != 0);
    CHECK_TRUE(identity.notes != 0);

    puts("anocrypto-c build_identity_abi_tests: PASS");
    return 0;
}
