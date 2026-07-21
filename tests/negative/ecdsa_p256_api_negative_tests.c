#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/ecdsa_p256_test_vectors.h"

#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"ECDSA negative line %d\n",__LINE__);return 1;}}while(0)
static int all_zero(const uint8_t *p,size_t n){size_t i;uint8_t v=0;for(i=0;i<n;++i)v|=p[i];return v==0;}

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
    REQUIRE(ano_crypto_module_shutdown()==ANO_CRYPTO_OK);
    puts("ECDSA P-256 API negative tests passed"); return 0;
}
