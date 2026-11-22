#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/ecdsa_p256_test_vectors.h"

#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"ECDSA negative line %d\n",__LINE__);return 1;}}while(0)
static int all_zero(const uint8_t *p,size_t n){size_t i;uint8_t v=0;for(i=0;i<n;++i)v|=p[i];return v==0;}

static int test_csp_exhaustion(void)
{
    enum { CSP_CAPACITY = 16 };
    static const uint8_t sample[] = { 's', 'a', 'm', 'p', 'l', 'e' };
    ano_crypto_csp_handle_t handles[CSP_CAPACITY] = { ANO_CRYPTO_CSP_INVALID_HANDLE };
    ano_crypto_csp_handle_t extra;
    ano_crypto_csp_handle_t stale;
    size_t i;
    int is_valid;

    for (i = 0u; i < CSP_CAPACITY; ++i) {
        REQUIRE(ecdsa_import_public(&handles[i]) == ANO_CRYPTO_OK);
        REQUIRE(handles[i] != ANO_CRYPTO_CSP_INVALID_HANDLE);
    }

    extra = handles[0];
    REQUIRE(ecdsa_import_public(&extra) == ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED);
    REQUIRE(extra == ANO_CRYPTO_CSP_INVALID_HANDLE);
    extra = handles[0];
    REQUIRE(ecdsa_import_private(&extra) == ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED);
    REQUIRE(extra == ANO_CRYPTO_CSP_INVALID_HANDLE);

    for (i = 0u; i < CSP_CAPACITY; ++i) {
        is_valid = 0;
        REQUIRE(ano_crypto_csp_handle_is_valid(handles[i], &is_valid) == ANO_CRYPTO_OK &&
                is_valid == 1);
    }
    REQUIRE(ano_crypto_signature_verify(
        handles[0], ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256, sample, sizeof(sample),
        ECDSA_P256_SIG_SAMPLE, sizeof(ECDSA_P256_SIG_SAMPLE)) == ANO_CRYPTO_OK);

    stale = handles[0];
    REQUIRE(ano_crypto_key_destroy(stale) == ANO_CRYPTO_OK);
    REQUIRE(ecdsa_import_public(&handles[0]) == ANO_CRYPTO_OK);
    REQUIRE(handles[0] != ANO_CRYPTO_CSP_INVALID_HANDLE && handles[0] != stale);
    is_valid = 0;
    REQUIRE(ano_crypto_csp_handle_is_valid(handles[0], &is_valid) == ANO_CRYPTO_OK &&
            is_valid == 1);
    is_valid = 1;
    REQUIRE(ano_crypto_csp_handle_is_valid(stale, &is_valid) == ANO_CRYPTO_OK &&
            is_valid == 0);
    REQUIRE(ano_crypto_key_destroy(stale) == ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_signature_verify(
        stale, ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256, sample, sizeof(sample),
        ECDSA_P256_SIG_SAMPLE, sizeof(ECDSA_P256_SIG_SAMPLE)) == ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_signature_verify(
        handles[0], ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256, sample, sizeof(sample),
        ECDSA_P256_SIG_SAMPLE, sizeof(ECDSA_P256_SIG_SAMPLE)) == ANO_CRYPTO_OK);

    for (i = 0u; i < CSP_CAPACITY; ++i) {
        REQUIRE(ano_crypto_key_destroy(handles[i]) == ANO_CRYPTO_OK);
    }
    return 0;
}

int main(void)
{
    ano_crypto_csp_handle_t priv=0,pub=0,bad=123u; uint8_t x[32],y[32],d[32],sig[80],msg[8]={1,2,3}; size_t written=99u;
    static const uint8_t n[32]={0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xbc,0xe6,0xfa,0xad,0xa7,0x17,0x9e,0x84,0xf3,0xb9,0xca,0xc2,0xfc,0x63,0x25,0x51};
    REQUIRE(ano_crypto_module_init(NULL)==ANO_CRYPTO_OK);
    memcpy(x,ECDSA_P256_X,32);memcpy(y,ECDSA_P256_Y,32);memcpy(d,ECDSA_P256_D,32);
    REQUIRE(ano_crypto_ec_p256_key_import(NULL,32,y,32,NULL,0,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT && bad==0);
    REQUIRE(ano_crypto_ec_p256_key_import(x,31,y,32,NULL,0,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    x[0]^=1u; REQUIRE(ano_crypto_ec_p256_key_import(x,32,y,32,NULL,0,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT); x[0]^=1u;
    memset(d,0,32); REQUIRE(ano_crypto_ec_p256_key_import(x,32,y,32,d,32,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    memcpy(d,n,32); REQUIRE(ano_crypto_ec_p256_key_import(x,32,y,32,d,32,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    memcpy(d,ECDSA_P256_D,32); d[31]^=1u; REQUIRE(ano_crypto_ec_p256_key_import(x,32,y,32,d,32,0,&bad)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_ec_p256_key_import(x,32,y,32,NULL,0,ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE,&bad)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    REQUIRE(ecdsa_import_private(&priv)==ANO_CRYPTO_OK); REQUIRE(ecdsa_import_public(&pub)==ANO_CRYPTO_OK);
    memset(sig,0xa5,sizeof(sig)); written=99u;
    REQUIRE(ano_crypto_signature_sign(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64,&written)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    REQUIRE(written==0u && all_zero(sig,64));
    memset(sig,0xa5,sizeof(sig)); written=99u;
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,63,&written)==ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    REQUIRE(written==0u && all_zero(sig,63));
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64,NULL)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,NULL,1,sig,64,&written)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,sig,3,sig,64,&written)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64,&written)==ANO_CRYPTO_OK);
    REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,63)==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    sig[0]^=1u; REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64)==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED); sig[0]^=1u;
    memset(sig,0,64); REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64)==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    memcpy(sig,n,32);memcpy(sig+32,ECDSA_P256_SIG_SAMPLE+32,32); REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,(const uint8_t*)"sample",6,sig,64)==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    REQUIRE(ano_crypto_key_destroy(priv)==ANO_CRYPTO_OK); REQUIRE(ano_crypto_key_destroy(pub)==ANO_CRYPTO_OK);
    REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,msg,3,sig,64)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(test_csp_exhaustion() == 0);
    REQUIRE(ano_crypto_module_shutdown()==ANO_CRYPTO_OK);
    puts("ECDSA P-256 API negative tests passed"); return 0;
}
