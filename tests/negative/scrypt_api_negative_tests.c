#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;
static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) { fprintf(stderr, "FAIL %s: got %s expected %s\n", name,
        ano_crypto_status_name(got), ano_crypto_status_name(expected)); ++failures; }
}
static void expect_cleared(const char *name, const uint8_t *out)
{
    size_t i; for (i=0u;i<32u;++i) if (out[i]!=0u) { fprintf(stderr,"FAIL %s: output not cleared\n",name); ++failures; return; }
}
int main(void)
{
    uint8_t password[16] = {1u};
    uint8_t salt[32] = {2u};
    uint8_t out[32];
    uint8_t overlap_storage[64] = {3u};
    uint8_t *raw = (uint8_t *)malloc(ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE + 8u);
    void *workspace;
    if (raw == 0) return 1;
    workspace = (void *)(((uintptr_t)raw + 3u) & ~(uintptr_t)3u);

    memset(out,0xa5,sizeof(out));
    expect_status("NULL password", ano_crypto_scrypt_derive_key(0,1u,salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,out), ANO_CRYPTO_ERR_INVALID_ARGUMENT); expect_cleared("NULL password",out);
    memset(out,0xa5,sizeof(out));
    expect_status("NULL salt", ano_crypto_scrypt_derive_key(password,sizeof(password),0,1u,workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,out), ANO_CRYPTO_ERR_INVALID_ARGUMENT); expect_cleared("NULL salt",out);
    memset(out,0xa5,sizeof(out));
    expect_status("small workspace", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE-1u,out), ANO_CRYPTO_ERR_BUFFER_TOO_SMALL); expect_cleared("small workspace",out);
    memset(out,0xa5,sizeof(out));
    expect_status("misaligned workspace", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),(uint8_t *)workspace+1u,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,out), ANO_CRYPTO_ERR_INVALID_ARGUMENT); expect_cleared("misaligned workspace",out);
    memset(overlap_storage,0xa5,sizeof(overlap_storage));
    expect_status("output overlaps password", ano_crypto_scrypt_derive_key(overlap_storage,32u,salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,overlap_storage+8u), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_cleared("output overlaps password", overlap_storage+8u);
    memset(workspace,0xa5,ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE);
    expect_status("output overlaps workspace", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,(uint8_t *)workspace), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_cleared("output overlaps workspace", (const uint8_t *)workspace);
    memset(out,0xa5,sizeof(out));
    expect_status("workspace overlaps salt", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),salt,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,out), ANO_CRYPTO_ERR_INVALID_ARGUMENT); expect_cleared("workspace overlaps salt",out);
    memset(out,0xa5,sizeof(out));
    expect_status("not initialized", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,out), ANO_CRYPTO_ERR_NOT_INITIALIZED); expect_cleared("not initialized",out);

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("NULL output", ano_crypto_scrypt_derive_key(password,sizeof(password),salt,sizeof(salt),workspace,ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE,0), ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);
    free(raw);
    if (failures != 0) { fprintf(stderr,"scrypt_api_negative_tests: %d failure(s)\n",failures); return 1; }
    puts("scrypt_api_negative_tests: PASS"); return 0;
}
