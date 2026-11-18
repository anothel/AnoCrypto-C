#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
#include "../fixtures/rsa_test_helpers.h"

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"RSA differential failure line %d\n",__LINE__); return 1; } } while (0)

static EVP_PKEY *make_pkey(int private_key)
{
    OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();
    OSSL_PARAM *params = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey = NULL;
    BIGNUM *n = BN_bin2bn(RSA_KAT_N, sizeof(RSA_KAT_N), NULL);
    BIGNUM *e = BN_bin2bn(RSA_KAT_E, sizeof(RSA_KAT_E), NULL);
    BIGNUM *d = NULL, *p = NULL, *q = NULL, *dp = NULL, *dq = NULL, *qi = NULL;
    int ok = bld != NULL && n != NULL && e != NULL;
    if (ok) ok = OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, n) == 1 &&
                 OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, e) == 1;
    if (private_key && ok) {
        d=BN_bin2bn(RSA_KAT_D,sizeof(RSA_KAT_D),NULL); p=BN_bin2bn(RSA_KAT_P,sizeof(RSA_KAT_P),NULL);
        q=BN_bin2bn(RSA_KAT_Q,sizeof(RSA_KAT_Q),NULL); dp=BN_bin2bn(RSA_KAT_DP,sizeof(RSA_KAT_DP),NULL);
        dq=BN_bin2bn(RSA_KAT_DQ,sizeof(RSA_KAT_DQ),NULL); qi=BN_bin2bn(RSA_KAT_QINV,sizeof(RSA_KAT_QINV),NULL);
        ok = d&&p&&q&&dp&&dq&&qi &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_D, d)==1 &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR1, p)==1 &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_FACTOR2, q)==1 &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT1, dp)==1 &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_EXPONENT2, dq)==1 &&
             OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_COEFFICIENT1, qi)==1;
    }
    if (ok) params=OSSL_PARAM_BLD_to_param(bld);
    if (params) ctx=EVP_PKEY_CTX_new_from_name(NULL,"RSA",NULL);
    if (ctx && EVP_PKEY_fromdata_init(ctx)==1 &&
        EVP_PKEY_fromdata(ctx, &pkey, private_key?EVP_PKEY_KEYPAIR:EVP_PKEY_PUBLIC_KEY, params)==1) {
    } else { EVP_PKEY_free(pkey); pkey=NULL; }
    EVP_PKEY_CTX_free(ctx); OSSL_PARAM_free(params); OSSL_PARAM_BLD_free(bld);
    BN_free(n);BN_free(e);BN_free(d);BN_free(p);BN_free(q);BN_free(dp);BN_free(dq);BN_free(qi);
    return pkey;
}

static const EVP_MD *md_for(ano_crypto_signature_algorithm_t alg)
{
    if (alg==ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256) return EVP_sha256();
    if (alg==ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384) return EVP_sha384();
    return EVP_sha512();
}

static int openssl_sign(EVP_PKEY *key, const EVP_MD *md, const unsigned char *msg,size_t msg_len,unsigned char sig[256])
{
    EVP_MD_CTX *ctx=EVP_MD_CTX_new(); size_t len=256u; int ok=ctx!=NULL;
    if(ok) ok=EVP_DigestSignInit(ctx,NULL,md,NULL,key)==1;
    if(ok) ok=EVP_DigestSign(ctx,sig,&len,msg,msg_len)==1 && len==256u;
    EVP_MD_CTX_free(ctx); return ok;
}
static int openssl_verify(EVP_PKEY *key,const EVP_MD *md,const unsigned char *msg,size_t msg_len,const unsigned char sig[256])
{
    EVP_MD_CTX *ctx=EVP_MD_CTX_new(); int ok=ctx!=NULL;
    if(ok) ok=EVP_DigestVerifyInit(ctx,NULL,md,NULL,key)==1;
    if(ok) ok=EVP_DigestVerify(ctx,sig,256u,msg,msg_len)==1;
    EVP_MD_CTX_free(ctx); return ok;
}

int main(void)
{
    static const unsigned char m1[]="CSR proof-of-possession";
    static const unsigned char m2[]="certificate issue TBS";
    static const unsigned char m3[]="CRL signing TBS";
    static const unsigned char m4[]="OCSP response signing TBS";
    const unsigned char *messages[]={m1,m2,m3,m4};
    const size_t lengths[]={sizeof(m1)-1u,sizeof(m2)-1u,sizeof(m3)-1u,sizeof(m4)-1u};
    const ano_crypto_signature_algorithm_t algorithms[]={
        ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
        ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384,
        ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512};
    EVP_PKEY *ossl_priv=make_pkey(1),*ossl_pub=make_pkey(0);
    rsa_test_components_t c; ano_crypto_csp_handle_t priv=0,pub=0;
    unsigned char a[256],b[256]; size_t written; size_t i,j; unsigned comparisons=0;
    ano_crypto_status_t st;
    REQUIRE(ossl_priv&&ossl_pub);
    REQUIRE(ano_crypto_module_init(NULL)==ANO_CRYPTO_OK);
    rsa_test_private_components(&c); REQUIRE(rsa_test_import(&c,0,&priv)==ANO_CRYPTO_OK);
    rsa_test_public_components(&c); REQUIRE(rsa_test_import(&c,0,&pub)==ANO_CRYPTO_OK);
    for(i=0;i<3u;++i) for(j=0;j<4u;++j){
        written=99u;
        memset(a,0xa5,sizeof(a));
        st=ano_crypto_signature_sign(priv,algorithms[i],messages[j],lengths[j],a,sizeof(a),&written);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
        REQUIRE(st==ANO_CRYPTO_OK);
        REQUIRE(written==256u && openssl_verify(ossl_pub,md_for(algorithms[i]),messages[j],lengths[j],a));
#else
        {
            size_t k;
            REQUIRE(st==ANO_CRYPTO_ERR_NOT_IMPLEMENTED && written==0u);
            for(k=0u;k<sizeof(a);++k) REQUIRE(a[k]==0u);
        }
#endif
        REQUIRE(openssl_sign(ossl_priv,md_for(algorithms[i]),messages[j],lengths[j],b));
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
        REQUIRE(memcmp(a,b,256u)==0);
#endif
        REQUIRE(openssl_verify(ossl_pub,md_for(algorithms[i]),messages[j],lengths[j],b));
        REQUIRE(ano_crypto_signature_verify(pub,algorithms[i],messages[j],lengths[j],b,256u)==ANO_CRYPTO_OK);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
        comparisons += 3u;
#else
        comparisons += 1u;
#endif
    }
    REQUIRE(ano_crypto_key_destroy(priv)==ANO_CRYPTO_OK); REQUIRE(ano_crypto_key_destroy(pub)==ANO_CRYPTO_OK);
    REQUIRE(ano_crypto_module_shutdown()==ANO_CRYPTO_OK);
    EVP_PKEY_free(ossl_priv);EVP_PKEY_free(ossl_pub);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
    printf("OpenSSL RSA differential comparisons passed: %u (full signing and verification)\n",comparisons);
#else
    printf("OpenSSL RSA differential comparisons passed: %u (verification only; 12 unavailable-entropy signing rejections)\n",comparisons);
#endif
    return 0;
}
