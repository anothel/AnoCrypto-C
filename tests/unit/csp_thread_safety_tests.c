#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <anocrypto/anocrypto.h>

#define THREAD_COUNT 4u
#define ITERATIONS_PER_THREAD 100u

typedef struct worker_result_s {
    unsigned int failures;
} worker_result_t;

static void *worker_main(void *arg)
{
    worker_result_t *result = (worker_result_t *)arg;
    static const uint8_t key[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t msg[] = { 't', 'h', 'r', 'e', 'a', 'd' };
    unsigned int i;

    for (i = 0u; i < ITERATIONS_PER_THREAD; ++i) {
        ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
        uint8_t mac[32];
        size_t written = 0u;
        ano_crypto_status_t st;

        st = ano_crypto_key_import_secret(
            ANO_CRYPTO_KEY_TYPE_HMAC,
            key,
            sizeof(key),
            ANO_CRYPTO_KEY_FLAG_NONE,
            &handle);
        if (st != ANO_CRYPTO_OK) {
            result->failures += 1u;
            continue;
        }

        st = ano_crypto_hmac_compute_with_key(
            ANO_CRYPTO_HASH_ALG_SHA256,
            handle,
            msg,
            sizeof(msg),
            mac,
            sizeof(mac),
            &written);
        if (st != ANO_CRYPTO_OK || written != sizeof(mac)) {
            result->failures += 1u;
        }

        st = ano_crypto_key_destroy(handle);
        if (st != ANO_CRYPTO_OK) {
            result->failures += 1u;
        }
    }

    return 0;
}

int main(void)
{
    pthread_t threads[THREAD_COUNT];
    worker_result_t results[THREAD_COUNT];
    unsigned int i;
    unsigned int failures = 0u;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    for (i = 0u; i < THREAD_COUNT; ++i) {
        results[i].failures = 0u;
        if (pthread_create(&threads[i], 0, worker_main, &results[i]) != 0) {
            fprintf(stderr, "pthread_create failed\n");
            return 1;
        }
    }

    for (i = 0u; i < THREAD_COUNT; ++i) {
        if (pthread_join(threads[i], 0) != 0) {
            fprintf(stderr, "pthread_join failed\n");
            return 1;
        }
        failures += results[i].failures;
    }

    if (failures != 0u) {
        fprintf(stderr, "threaded CSP/HMAC worker failures: %u\n", failures);
        return 1;
    }

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module shutdown failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    return 0;
}
