#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;
static void compare_case(const char *name, const uint8_t *password, size_t password_len,
                         const uint8_t *salt, size_t salt_len, void *workspace)
{
    uint8_t actual[32];
    uint8_t expected[32];
    uint8_t diff = 0u;
    size_t i;
    ano_crypto_status_t st;
    memset(workspace, 0xa5, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    st = ano_crypto_scrypt_derive_key(password, password_len, salt, salt_len,
        workspace, ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE, actual);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr,"FAIL %s: AnoCrypto-C %s\n",name,ano_crypto_status_name(st)); ++failures; return;
    }
    for (i = 0u; i < ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE; ++i) {
        if (((const uint8_t *)workspace)[i] != 0u) {
            fprintf(stderr,"FAIL %s: workspace not zeroized\n",name); ++failures;
            ano_crypto_csp_zeroize(actual,sizeof(actual));
            return;
        }
    }
    if (EVP_PBE_scrypt((const char *)password, password_len, salt, salt_len,
        ANO_CRYPTO_SCRYPT_N, ANO_CRYPTO_SCRYPT_R, ANO_CRYPTO_SCRYPT_P,
        ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE, expected, sizeof(expected)) != 1) {
        fprintf(stderr,"FAIL %s: OpenSSL scrypt\n",name); ++failures;
        ano_crypto_csp_zeroize(actual,sizeof(actual));
        return;
    }
    for (i=0u;i<sizeof(actual);++i) diff=(uint8_t)(diff|(uint8_t)(actual[i]^expected[i]));
    if (diff != 0u) { fprintf(stderr,"FAIL %s: mismatch\n",name); ++failures; }
    OPENSSL_cleanse(expected,sizeof(expected));
    ano_crypto_csp_zeroize(actual,sizeof(actual));
}
int main(void)
{
    static const uint8_t empty[] = "";
    static const uint8_t password_a[]="password";
    static const uint8_t salt_a[]="NaCl";
    static const uint8_t password_b[]={0x00,0x01,0xfe,0xff,0x42};
    static const uint8_t salt_b[]={0xa5,0x5a,0x00,0xff,0x10,0x20,0x30,0x40};
    static const uint8_t alias_bytes[]="alias-input";
    void *workspace=malloc(ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE);
    if (workspace==0) return 1;
    if (ano_crypto_module_init(0)!=ANO_CRYPTO_OK) {
        free(workspace);
        return 1;
    }
    compare_case("empty",empty,0u,empty,0u,workspace);
    compare_case("text",password_a,sizeof(password_a)-1u,salt_a,sizeof(salt_a)-1u,workspace);
    compare_case("binary",password_b,sizeof(password_b),salt_b,sizeof(salt_b),workspace);
    compare_case("aliased inputs",alias_bytes,sizeof(alias_bytes)-1u,alias_bytes,sizeof(alias_bytes)-1u,workspace);
    if (ano_crypto_module_shutdown()!=ANO_CRYPTO_OK) ++failures;
    free(workspace);
    if (failures!=0) { fprintf(stderr,"scrypt_openssl_differential_tests: %d failure(s)\n",failures); return 1; }
    printf("scrypt_openssl_differential_tests: PASS provider=OpenSSL %s comparisons=4\n",OpenSSL_version(OPENSSL_VERSION));
    return 0;
}
