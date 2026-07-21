#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>
#include "../fixtures/rsa_test_helpers.h"

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
typedef HANDLE test_thread_t;
typedef DWORD (WINAPI *test_thread_entry_t)(LPVOID);
#  define TEST_THREAD_RETURN DWORD WINAPI
#  define TEST_THREAD_ARG LPVOID
#else
#  include <pthread.h>
#  include <sched.h>
typedef pthread_t test_thread_t;
typedef void *(*test_thread_entry_t)(void *);
#  define TEST_THREAD_RETURN void *
#  define TEST_THREAD_ARG void *
#endif

#define THREAD_COUNT 4u
#define ITERATIONS 3u

typedef struct worker_s {
    ano_crypto_csp_handle_t private_key;
    ano_crypto_csp_handle_t public_key;
    volatile int started;
    unsigned int failures;
    int race_mode;
    ano_crypto_status_t race_status;
} worker_t;

static int create_thread(test_thread_t *t, test_thread_entry_t entry, void *arg)
{
#if defined(_WIN32)
    *t = CreateThread(NULL, 0u, entry, arg, 0u, NULL);
    return *t == NULL ? -1 : 0;
#else
    return pthread_create(t, NULL, entry, arg);
#endif
}
static int join_thread(test_thread_t t)
{
#if defined(_WIN32)
    DWORD r = WaitForSingleObject(t, INFINITE);
    CloseHandle(t);
    return r == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_join(t, NULL);
#endif
}
static void yield_thread(void)
{
#if defined(_WIN32)
    Sleep(0u);
#else
    (void)sched_yield();
#endif
}

static TEST_THREAD_RETURN worker_main(TEST_THREAD_ARG arg)
{
    worker_t *w = (worker_t *)arg;
    uint8_t signature[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE];
    size_t written = 0u;
    unsigned int i;
    w->started = 1;
    if (w->race_mode) {
        memset(signature, 0xa5, sizeof(signature));
        w->race_status = ano_crypto_signature_sign(
            w->private_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
            RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
            signature, sizeof(signature), &written);
        if (w->race_status == ANO_CRYPTO_OK) {
            if (written != sizeof(signature) ||
                memcmp(signature, RSA_KAT_SIG_SHA256, sizeof(signature)) != 0 ||
                ano_crypto_signature_verify(
                    w->public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                    RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                    signature, sizeof(signature)) != ANO_CRYPTO_OK) {
                w->failures++;
            }
        } else if (w->race_status == ANO_CRYPTO_ERR_INVALID_ARGUMENT) {
            size_t j;
            if (written != 0u) w->failures++;
            for (j = 0u; j < sizeof(signature); ++j) {
                if (signature[j] != 0u) { w->failures++; break; }
            }
        } else {
            w->failures++;
        }
#if defined(_WIN32)
        return 0u;
#else
        return NULL;
#endif
    }
    for (i = 0u; i < ITERATIONS; ++i) {
        memset(signature, 0, sizeof(signature));
        written = 0u;
        if (ano_crypto_signature_sign(
                w->private_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                signature, sizeof(signature), &written) != ANO_CRYPTO_OK ||
            written != sizeof(signature) ||
            memcmp(signature, RSA_KAT_SIG_SHA256, sizeof(signature)) != 0 ||
            ano_crypto_signature_verify(
                w->public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                signature, sizeof(signature)) != ANO_CRYPTO_OK) {
            w->failures++;
        }
    }
#if defined(_WIN32)
    return 0u;
#else
    return NULL;
#endif
}

int main(void)
{
    rsa_test_components_t c;
    ano_crypto_csp_handle_t private_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t public_key = ANO_CRYPTO_CSP_INVALID_HANDLE;
    test_thread_t threads[THREAD_COUNT];
    worker_t workers[THREAD_COUNT];
    test_thread_t race_thread;
    worker_t race_worker;
    unsigned int i;

    if (ano_crypto_module_init(NULL) != ANO_CRYPTO_OK) return 1;
    rsa_test_private_components(&c);
    if (rsa_test_import(&c, 0u, &private_key) != ANO_CRYPTO_OK) return 1;
    rsa_test_public_components(&c);
    if (rsa_test_import(&c, 0u, &public_key) != ANO_CRYPTO_OK) return 1;

    memset(workers, 0, sizeof(workers));
    for (i = 0u; i < THREAD_COUNT; ++i) {
        workers[i].private_key = private_key;
        workers[i].public_key = public_key;
        if (create_thread(&threads[i], worker_main, &workers[i]) != 0) return 1;
    }
    for (i = 0u; i < THREAD_COUNT; ++i) {
        if (join_thread(threads[i]) != 0 || workers[i].failures != 0u) return 1;
    }

    memset(&race_worker, 0, sizeof(race_worker));
    race_worker.private_key = private_key;
    race_worker.public_key = public_key;
    race_worker.race_mode = 1;
    if (create_thread(&race_thread, worker_main, &race_worker) != 0) return 1;
    while (!race_worker.started) yield_thread();
    if (ano_crypto_key_destroy(private_key) != ANO_CRYPTO_OK) return 1;
    if (join_thread(race_thread) != 0 || race_worker.failures != 0u) return 1;
    if (ano_crypto_key_destroy(public_key) != ANO_CRYPTO_OK) return 1;
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 1;
    puts("RSA thread and destroy-race tests passed");
    return 0;
}
