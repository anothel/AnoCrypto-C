#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
typedef HANDLE test_thread_t;
#  define TEST_THREAD_RETURN DWORD WINAPI
#  define TEST_THREAD_ARG LPVOID
#else
#  include <pthread.h>
typedef pthread_t test_thread_t;
#  define TEST_THREAD_RETURN void *
#  define TEST_THREAD_ARG void *
#endif

#define THREAD_COUNT 4u
#define ITERATIONS_PER_THREAD 100u
#define SHARED_ITERATIONS_PER_THREAD 200u

typedef struct worker_result_s {
    unsigned int failures;
    ano_crypto_csp_handle_t shared_handle;
    int shared_mode;
} worker_result_t;

static int create_thread(test_thread_t *out_thread,
                         TEST_THREAD_RETURN (*entry)(TEST_THREAD_ARG),
                         void *arg)
{
#if defined(_WIN32)
    *out_thread = CreateThread(0, 0u, entry, arg, 0u, 0);
    return (*out_thread != 0) ? 0 : -1;
#else
    return pthread_create(out_thread, 0, entry, arg);
#endif
}

static int join_thread(test_thread_t thread)
{
#if defined(_WIN32)
    DWORD wait_result = WaitForSingleObject(thread, INFINITE);
    if (wait_result != WAIT_OBJECT_0) {
        CloseHandle(thread);
        return -1;
    }
    CloseHandle(thread);
    return 0;
#else
    return pthread_join(thread, 0);
#endif
}

static TEST_THREAD_RETURN worker_main(TEST_THREAD_ARG arg)
{
    worker_result_t *result = (worker_result_t *)arg;
    static const uint8_t key[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t msg[] = { 't', 'h', 'r', 'e', 'a', 'd' };
    unsigned int i;
    unsigned int iterations = result->shared_mode != 0
        ? SHARED_ITERATIONS_PER_THREAD
        : ITERATIONS_PER_THREAD;

    for (i = 0u; i < iterations; ++i) {
        ano_crypto_csp_handle_t handle = result->shared_handle;
        uint8_t mac[ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE];
        size_t written = 0u;
        ano_crypto_status_t st;

        if (result->shared_mode == 0) {
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

        (void)ano_crypto_csp_zeroize(mac, sizeof(mac));

        if (result->shared_mode == 0) {
            st = ano_crypto_key_destroy(handle);
            if (st != ANO_CRYPTO_OK) {
                result->failures += 1u;
            }
        }
    }

#if defined(_WIN32)
    return 0u;
#else
    return 0;
#endif
}

static int run_worker_group(int shared_mode, ano_crypto_csp_handle_t shared_handle)
{
    test_thread_t threads[THREAD_COUNT];
    worker_result_t results[THREAD_COUNT];
    unsigned int i;
    unsigned int created = 0u;
    unsigned int failures = 0u;

    for (i = 0u; i < THREAD_COUNT; ++i) {
        results[i].failures = 0u;
        results[i].shared_handle = shared_handle;
        results[i].shared_mode = shared_mode;
        if (create_thread(&threads[i], worker_main, &results[i]) != 0) {
            fprintf(stderr, "thread creation failed\n");
            break;
        }
        created += 1u;
    }

    for (i = 0u; i < created; ++i) {
        if (join_thread(threads[i]) != 0) {
            fprintf(stderr, "thread join failed\n");
            failures += 1u;
        }
        failures += results[i].failures;
    }

    if (created != THREAD_COUNT) {
        failures += 1u;
    }

    if (failures != 0u) {
        fprintf(stderr, "threaded CSP/HMAC worker failures: %u\n", failures);
        return 1;
    }

    return 0;
}

int main(void)
{
    static const uint8_t shared_key[] = { 's', 'h', 'a', 'r', 'e', 'd' };
    ano_crypto_csp_handle_t shared_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    if (run_worker_group(0, ANO_CRYPTO_CSP_INVALID_HANDLE) != 0) {
        return 1;
    }

    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_HMAC,
        shared_key,
        sizeof(shared_key),
        ANO_CRYPTO_KEY_FLAG_NONE,
        &shared_handle);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "shared key import failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    if (run_worker_group(1, shared_handle) != 0) {
        (void)ano_crypto_key_destroy(shared_handle);
        return 1;
    }

    st = ano_crypto_key_destroy(shared_handle);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "shared key destroy failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module shutdown failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    puts("anocrypto-c csp_thread_safety_tests: PASS");
    return 0;
}
