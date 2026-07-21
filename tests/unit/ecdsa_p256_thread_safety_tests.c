#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/ecdsa_p256_test_vectors.h"

#if defined(_WIN32)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
typedef HANDLE test_thread_t; typedef DWORD (WINAPI *test_thread_entry_t)(LPVOID);
# define TEST_THREAD_RETURN DWORD WINAPI
# define TEST_THREAD_ARG LPVOID
#else
# include <pthread.h>
# include <sched.h>
typedef pthread_t test_thread_t; typedef void *(*test_thread_entry_t)(void *);
# define TEST_THREAD_RETURN void *
# define TEST_THREAD_ARG void *
#endif

#define THREAD_COUNT 4u
#define ITERATIONS 3u

typedef struct worker_s{ano_crypto_csp_handle_t private_key;ano_crypto_csp_handle_t public_key;volatile int started;unsigned failures;int race_mode;ano_crypto_status_t race_status;}worker_t;
static int create_thread(test_thread_t *t,test_thread_entry_t e,void *a){
#if defined(_WIN32)
*t=CreateThread(NULL,0u,e,a,0u,NULL);return *t==NULL?-1:0;
#else
return pthread_create(t,NULL,e,a);
#endif
}
static int join_thread(test_thread_t t){
#if defined(_WIN32)
DWORD r=WaitForSingleObject(t,INFINITE);CloseHandle(t);return r==WAIT_OBJECT_0?0:-1;
#else
return pthread_join(t,NULL);
#endif
}
static void yield_thread(void){
#if defined(_WIN32)
Sleep(0u);
#else
(void)sched_yield();
#endif
}
static TEST_THREAD_RETURN worker_main(TEST_THREAD_ARG arg){worker_t*w=(worker_t*)arg;uint8_t sig[64];size_t written=0u;unsigned i;w->started=1;
 if(w->race_mode){memset(sig,0xa5,sizeof(sig));w->race_status=ano_crypto_signature_sign(w->private_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,(const uint8_t*)"sample",6u,sig,sizeof(sig),&written);if(w->race_status==ANO_CRYPTO_OK){if(written!=64u||memcmp(sig,ECDSA_P256_SIG_SAMPLE,64u)!=0||ano_crypto_signature_verify(w->public_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,(const uint8_t*)"sample",6u,sig,64u)!=ANO_CRYPTO_OK)w->failures++;}else if(w->race_status==ANO_CRYPTO_ERR_INVALID_ARGUMENT){size_t j;if(written!=0u)w->failures++;for(j=0u;j<64u;++j)if(sig[j]!=0u){w->failures++;break;}}else w->failures++;
#if defined(_WIN32)
 return 0u;
#else
 return NULL;
#endif
 }
 for(i=0u;i<ITERATIONS;++i){memset(sig,0,sizeof(sig));written=0u;if(ano_crypto_signature_sign(w->private_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,(const uint8_t*)"sample",6u,sig,sizeof(sig),&written)!=ANO_CRYPTO_OK||written!=64u||memcmp(sig,ECDSA_P256_SIG_SAMPLE,64u)!=0||ano_crypto_signature_verify(w->public_key,ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256,(const uint8_t*)"sample",6u,sig,64u)!=ANO_CRYPTO_OK)w->failures++;}
#if defined(_WIN32)
 return 0u;
#else
 return NULL;
#endif
}
int main(void){ano_crypto_csp_handle_t priv=0,pub=0;test_thread_t threads[THREAD_COUNT],race_thread;worker_t workers[THREAD_COUNT],race;unsigned i;if(ano_crypto_module_init(NULL)!=ANO_CRYPTO_OK)return 1;if(ecdsa_import_private(&priv)!=ANO_CRYPTO_OK||ecdsa_import_public(&pub)!=ANO_CRYPTO_OK)return 1;memset(workers,0,sizeof(workers));for(i=0;i<THREAD_COUNT;++i){workers[i].private_key=priv;workers[i].public_key=pub;if(create_thread(&threads[i],worker_main,&workers[i])!=0)return 1;}for(i=0;i<THREAD_COUNT;++i)if(join_thread(threads[i])!=0||workers[i].failures)return 1;memset(&race,0,sizeof(race));race.private_key=priv;race.public_key=pub;race.race_mode=1;if(create_thread(&race_thread,worker_main,&race)!=0)return 1;while(!race.started)yield_thread();if(ano_crypto_key_destroy(priv)!=ANO_CRYPTO_OK)return 1;if(join_thread(race_thread)!=0||race.failures)return 1;if(ano_crypto_key_destroy(pub)!=ANO_CRYPTO_OK)return 1;if(ano_crypto_module_shutdown()!=ANO_CRYPTO_OK)return 1;puts("ECDSA P-256 thread and destroy-race tests passed");return 0;}
