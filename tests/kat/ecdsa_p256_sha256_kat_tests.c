#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/ecdsa_p256_test_vectors.h"

#define CHECK(x) do { ano_crypto_status_t s_=(x); if(s_!=ANO_CRYPTO_OK){fprintf(stderr,"ECDSA KAT line %d: %s\n",__LINE__,ano_crypto_status_name(s_));return 1;} } while(0)

static int run_case(ano_crypto_csp_handle_t private_key, ano_crypto_csp_handle_t public_key,
                    const uint8_t *message, size_t message_len, const uint8_t expected[64])
{
    uint8_t signature[64]; size_t written=0u;
    memset(signature,0xa5,sizeof(signature));
    CHECK(ano_crypto_signature_sign(private_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                                    message,message_len,signature,sizeof(signature),&written));
    if(written!=64u || memcmp(signature,expected,64u)!=0){fprintf(stderr,"ECDSA RFC6979 signature mismatch\n");return 0;}
    CHECK(ano_crypto_signature_verify(public_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                                      message,message_len,signature,written));
    CHECK(ano_crypto_signature_verify(private_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                                      message,message_len,signature,written));
    return 1;
}

int main(void)
{
    ano_crypto_csp_handle_t private_key=0,public_key=0; size_t required=0u;
    CHECK(ano_crypto_module_init(NULL));
    CHECK(ecdsa_import_private(&private_key)); CHECK(ecdsa_import_public(&public_key));
    CHECK(ano_crypto_signature_sign(private_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,
                                    NULL,0u,NULL,0u,&required));
    if(required!=64u) return 1;
    if(!run_case(private_key,public_key,(const uint8_t*)"sample",6u,ECDSA_P256_SIG_SAMPLE)) return 1;
    if(!run_case(private_key,public_key,(const uint8_t*)"test",4u,ECDSA_P256_SIG_TEST)) return 1;
    CHECK(ano_crypto_key_destroy(private_key)); CHECK(ano_crypto_key_destroy(public_key));
    CHECK(ano_crypto_module_shutdown());
    puts("ECDSA P-256 SHA-256 RFC 6979 KATs passed"); return 0;
}
