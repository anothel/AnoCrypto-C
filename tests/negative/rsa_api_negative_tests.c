#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/rsa_test_helpers.h"

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"RSA negative failure line %d\n",__LINE__); return 1; } } while (0)
static int cleared(const uint8_t *p, size_t n){ size_t i; for(i=0;i<n;++i) if(p[i]!=0u) return 0; return 1; }

int main(void)
{
    rsa_test_components_t c;
    ano_crypto_csp_handle_t priv=0,pub=0,handles[16];
    uint8_t out[256], bad[256], e[3], d[256], dp[128], dq[128];
    size_t written=99u, i;
    ano_crypto_csp_state_t state;
    REQUIRE(ano_crypto_module_init(NULL)==ANO_CRYPTO_OK);
    rsa_test_private_components(&c);
    REQUIRE(rsa_test_import(&c,0,&priv)==ANO_CRYPTO_OK);
    rsa_test_public_components(&c);
    REQUIRE(rsa_test_import(&c,0,&pub)==ANO_CRYPTO_OK);

    memset(out,0xa5,sizeof(out));
    REQUIRE(ano_crypto_signature_sign(pub,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),out,sizeof(out),&written)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    REQUIRE(cleared(out,sizeof(out)) && written==0u);
    memcpy(bad,RSA_KAT_SIG_SHA256,sizeof(bad)); bad[0]^=1u;
    REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),bad,sizeof(bad))==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),RSA_KAT_SIG_SHA256,255u)==ANO_CRYPTO_ERR_AUTHENTICATION_FAILED);
    memset(out,0xa5,31u); written=99u;
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),out,31u,&written)==ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    REQUIRE(cleared(out,31u) && written==0u);
    memset(out,0xa5,sizeof(out));
    memcpy(out,RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE)); written=99u;
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        out,sizeof(RSA_KAT_MESSAGE),out,sizeof(out),&written)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(cleared(out,sizeof(out)) && written==0u);
    memset(out,0xa5,sizeof(out));
    REQUIRE(ano_crypto_signature_sign(priv,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),out,sizeof(out),(size_t *)(void *)out)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(cleared(out,sizeof(out)));
    REQUIRE(ano_crypto_signature_sign(priv,(ano_crypto_signature_algorithm_t)99,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),NULL,0u,&written)==ANO_CRYPTO_ERR_UNSUPPORTED_ALGORITHM);

    REQUIRE(ano_crypto_key_destroy(pub)==ANO_CRYPTO_OK);
    REQUIRE(ano_crypto_signature_verify(pub,ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        RSA_KAT_MESSAGE,sizeof(RSA_KAT_MESSAGE),RSA_KAT_SIG_SHA256,256u)==ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    REQUIRE(ano_crypto_key_get_state(pub,&state)==ANO_CRYPTO_OK && state==ANO_CRYPTO_CSP_STATE_DESTROYED);
    REQUIRE(ano_crypto_key_destroy(priv)==ANO_CRYPTO_OK);

    rsa_test_public_components(&c);
    memcpy(e,RSA_KAT_E,sizeof(e)); e[2]=3u; c.public_exponent=e;
    REQUIRE(rsa_test_import(&c,0,&pub)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER && pub==0);
    rsa_test_private_components(&c);
    memcpy(dp,RSA_KAT_DP,sizeof(dp)); dp[127]^=1u; c.exponent1=dp;
    REQUIRE(rsa_test_import(&c,0,&priv)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER && priv==0);
    rsa_test_private_components(&c);
    memset(d,0,sizeof(d)); d[sizeof(d)-1u]=1u;
    memset(dp,0,sizeof(dp)); dp[sizeof(dp)-1u]=1u;
    memset(dq,0,sizeof(dq)); dq[sizeof(dq)-1u]=1u;
    c.private_exponent=d; c.exponent1=dp; c.exponent2=dq;
    REQUIRE(rsa_test_import(&c,0,&priv)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER && priv==0);
    rsa_test_public_components(&c);
    REQUIRE(rsa_test_import(&c,ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE,&pub)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);
    c.private_exponent=RSA_KAT_D; c.private_exponent_len=sizeof(RSA_KAT_D);
    REQUIRE(rsa_test_import(&c,0,&priv)==ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER);

    rsa_test_public_components(&c);
    for(i=0u;i<16u;++i) REQUIRE(rsa_test_import(&c,0,&handles[i])==ANO_CRYPTO_OK);
    REQUIRE(rsa_test_import(&c,0,&pub)==ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED);
    for(i=0u;i<16u;++i) REQUIRE(ano_crypto_key_destroy(handles[i])==ANO_CRYPTO_OK);
    REQUIRE(ano_crypto_module_shutdown()==ANO_CRYPTO_OK);
    puts("RSA API negative and boundary tests passed");
    return 0;
}
