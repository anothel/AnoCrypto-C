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
    unsigned int failures;
    int race_mode;
    ano_crypto_status_t race_status;
} worker_t;

/* Synchronize the start notification; volatile is not thread synchronization. */
#if defined(_WIN32)
static HANDLE race_started;
#else
static pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER;
static int race_started;
#endif

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
static TEST_THREAD_RETURN worker_main(TEST_THREAD_ARG arg)
{
    worker_t *w = (worker_t *)arg;
    uint8_t signature[ANO_CRYPTO_RSA2048_SIGNATURE_SIZE];
    size_t written = 99u;
    unsigned int i;
    if (w->race_mode) {
#if defined(_WIN32)
        if (!SetEvent(race_started)) w->failures++;
#else
        (void)pthread_mutex_lock(&start_mutex);
        race_started = 1;
        (void)pthread_cond_signal(&start_cond);
        (void)pthread_mutex_unlock(&start_mutex);
#endif
        memset(signature, 0xa5, sizeof(signature));
        w->race_status = ano_crypto_signature_sign(
            w->private_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
            RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
            signature, sizeof(signature), &written);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
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
#else
        /* Entropy rejection precedes the live/destroyed-key check. */
        if (w->race_status == ANO_CRYPTO_ERR_NOT_IMPLEMENTED) {
#endif
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
        ano_crypto_status_t st;
        memset(signature, 0xa5, sizeof(signature));
        written = 99u;
        st = ano_crypto_signature_sign(
                w->private_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                signature, sizeof(signature), &written);
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
        if (st != ANO_CRYPTO_OK ||
            written != sizeof(signature) ||
            memcmp(signature, RSA_KAT_SIG_SHA256, sizeof(signature)) != 0) {
            w->failures++;
        }
#else
        {
            size_t j;
            if (st != ANO_CRYPTO_ERR_NOT_IMPLEMENTED || written != 0u) w->failures++;
            for (j = 0u; j < sizeof(signature); ++j) {
                if (signature[j] != 0u) { w->failures++; break; }
            }
        }
#endif
        if (ano_crypto_signature_verify(
                w->public_key, ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256,
                RSA_KAT_MESSAGE, sizeof(RSA_KAT_MESSAGE),
                RSA_KAT_SIG_SHA256, sizeof(RSA_KAT_SIG_SHA256)) != ANO_CRYPTO_OK) {
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
#if defined(_WIN32)
    race_started = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (race_started == NULL) return 1;
#endif
    if (create_thread(&race_thread, worker_main, &race_worker) != 0) return 1;
#if defined(_WIN32)
    if (WaitForSingleObject(race_started, INFINITE) != WAIT_OBJECT_0) return 1;
#else
    (void)pthread_mutex_lock(&start_mutex);
    while (!race_started) (void)pthread_cond_wait(&start_cond, &start_mutex);
    (void)pthread_mutex_unlock(&start_mutex);
#endif
    if (ano_crypto_key_destroy(private_key) != ANO_CRYPTO_OK) return 1;
    if (join_thread(race_thread) != 0 || race_worker.failures != 0u) return 1;
#if defined(_WIN32)
    CloseHandle(race_started);
#endif
    if (ano_crypto_key_destroy(public_key) != ANO_CRYPTO_OK) return 1;
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 1;
#if defined(ANO_CRYPTO_TEST_RSA_ENTROPY_AVAILABLE)
    puts("RSA signing, verification, and destroy-race tests passed");
#else
    puts("RSA concurrent verification, unavailable-entropy rejection, and destroy-race tests passed");
#endif
    return 0;
}
