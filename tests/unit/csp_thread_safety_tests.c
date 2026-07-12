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
    ano_crypto_csp_handle_t shared_hmac_handle;
    ano_crypto_csp_handle_t shared_aes_handle;
    ano_crypto_csp_handle_t shared_cmac_handle;
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

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    return diff == 0u;
}

static TEST_THREAD_RETURN worker_main(TEST_THREAD_ARG arg)
{
    worker_result_t *result = (worker_result_t *)arg;
    static const uint8_t hmac_key[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t aes_key[16] = {
        0x10u,0x11u,0x12u,0x13u,0x14u,0x15u,0x16u,0x17u,
        0x18u,0x19u,0x1au,0x1bu,0x1cu,0x1du,0x1eu,0x1fu
    };
    static const uint8_t msg[] = { 't', 'h', 'r', 'e', 'a', 'd' };
    static const uint8_t iv[16] = {
        0xa0u,0xa1u,0xa2u,0xa3u,0xa4u,0xa5u,0xa6u,0xa7u,
        0xa8u,0xa9u,0xaau,0xabu,0xacu,0xadu,0xaeu,0xafu
    };
    static const uint8_t plaintext[16] = {
        0x00u,0x01u,0x02u,0x03u,0x04u,0x05u,0x06u,0x07u,
        0x08u,0x09u,0x0au,0x0bu,0x0cu,0x0du,0x0eu,0x0fu
    };
    unsigned int i;
    unsigned int iterations = result->shared_mode != 0
        ? SHARED_ITERATIONS_PER_THREAD
        : ITERATIONS_PER_THREAD;

    for (i = 0u; i < iterations; ++i) {
        ano_crypto_csp_handle_t hmac_handle = result->shared_hmac_handle;
        ano_crypto_csp_handle_t aes_handle = result->shared_aes_handle;
        ano_crypto_csp_handle_t cmac_handle = result->shared_cmac_handle;
        uint8_t mac[ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE];
        uint8_t cmac_tag[ANO_CRYPTO_CMAC_TAG_SIZE];
        uint8_t ciphertext[16];
        uint8_t recovered[16];
        size_t written = 0u;
        ano_crypto_status_t st;

        if (result->shared_mode == 0) {
            st = ano_crypto_key_import_secret(
                ANO_CRYPTO_KEY_TYPE_HMAC,
                hmac_key,
                sizeof(hmac_key),
                ANO_CRYPTO_KEY_FLAG_NONE,
                &hmac_handle);
            if (st != ANO_CRYPTO_OK) {
                result->failures += 1u;
                continue;
            }
            st = ano_crypto_key_import_secret(
                ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                aes_key,
                sizeof(aes_key),
                ANO_CRYPTO_KEY_FLAG_NONE,
                &aes_handle);
            if (st != ANO_CRYPTO_OK) {
                result->failures += 1u;
                (void)ano_crypto_key_destroy(hmac_handle);
                continue;
            }
            st = ano_crypto_key_import_secret(
                ANO_CRYPTO_KEY_TYPE_CMAC,
                aes_key,
                sizeof(aes_key),
                ANO_CRYPTO_KEY_FLAG_NONE,
                &cmac_handle);
            if (st != ANO_CRYPTO_OK) {
                result->failures += 1u;
                (void)ano_crypto_key_destroy(aes_handle);
                (void)ano_crypto_key_destroy(hmac_handle);
                continue;
            }
        }

        st = ano_crypto_hmac_compute_with_key(
            ANO_CRYPTO_HASH_ALG_SHA256,
            hmac_handle,
            msg,
            sizeof(msg),
            mac,
            sizeof(mac),
            &written);
        if (st != ANO_CRYPTO_OK || written != sizeof(mac)) {
            result->failures += 1u;
        }

        written = 0u;
        st = ano_crypto_cmac_compute_with_key(
            cmac_handle,
            msg,
            sizeof(msg),
            cmac_tag,
            sizeof(cmac_tag),
            &written,
            ANO_CRYPTO_CMAC_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != sizeof(cmac_tag)) {
            result->failures += 1u;
        } else {
            st = ano_crypto_cmac_verify_with_key(
                cmac_handle, msg, sizeof(msg),
                cmac_tag, sizeof(cmac_tag),
                ANO_CRYPTO_CMAC_FLAG_NONE);
            if (st != ANO_CRYPTO_OK) {
                result->failures += 1u;
            }
        }

        written = 0u;
        st = ano_crypto_cipher_crypt_with_key(
            ANO_CRYPTO_CIPHER_ALG_AES,
            ANO_CRYPTO_CIPHER_MODE_CBC,
            ANO_CRYPTO_CIPHER_DIRECTION_ENCRYPT,
            aes_handle,
            iv,
            sizeof(iv),
            plaintext,
            sizeof(plaintext),
            ciphertext,
            sizeof(ciphertext),
            &written,
            ANO_CRYPTO_CIPHER_FLAG_NONE);
        if (st != ANO_CRYPTO_OK || written != sizeof(ciphertext)) {
            result->failures += 1u;
        } else {
            written = 0u;
            st = ano_crypto_cipher_crypt_with_key(
                ANO_CRYPTO_CIPHER_ALG_AES,
                ANO_CRYPTO_CIPHER_MODE_CBC,
                ANO_CRYPTO_CIPHER_DIRECTION_DECRYPT,
                aes_handle,
                iv,
                sizeof(iv),
                ciphertext,
                sizeof(ciphertext),
                recovered,
                sizeof(recovered),
                &written,
                ANO_CRYPTO_CIPHER_FLAG_NONE);
            if (st != ANO_CRYPTO_OK || written != sizeof(recovered) ||
                !bytes_equal(recovered, plaintext, sizeof(recovered))) {
                result->failures += 1u;
            }
        }

        (void)ano_crypto_csp_zeroize(mac, sizeof(mac));
        (void)ano_crypto_csp_zeroize(cmac_tag, sizeof(cmac_tag));
        (void)ano_crypto_csp_zeroize(ciphertext, sizeof(ciphertext));
        (void)ano_crypto_csp_zeroize(recovered, sizeof(recovered));

        if (result->shared_mode == 0) {
            if (ano_crypto_key_destroy(cmac_handle) != ANO_CRYPTO_OK) {
                result->failures += 1u;
            }
            if (ano_crypto_key_destroy(aes_handle) != ANO_CRYPTO_OK) {
                result->failures += 1u;
            }
            if (ano_crypto_key_destroy(hmac_handle) != ANO_CRYPTO_OK) {
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

static int run_worker_group(
    int shared_mode,
    ano_crypto_csp_handle_t shared_hmac_handle,
    ano_crypto_csp_handle_t shared_aes_handle,
    ano_crypto_csp_handle_t shared_cmac_handle)
{
    test_thread_t threads[THREAD_COUNT];
    worker_result_t results[THREAD_COUNT];
    unsigned int i;
    unsigned int created = 0u;
    unsigned int failures = 0u;

    for (i = 0u; i < THREAD_COUNT; ++i) {
        results[i].failures = 0u;
        results[i].shared_hmac_handle = shared_hmac_handle;
        results[i].shared_aes_handle = shared_aes_handle;
        results[i].shared_cmac_handle = shared_cmac_handle;
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

    if (created != THREAD_COUNT) failures += 1u;
    if (failures != 0u) {
        fprintf(stderr, "threaded CSP/HMAC/AES/CMAC worker failures: %u\n", failures);
        return 1;
    }
    return 0;
}

int main(void)
{
    static const uint8_t shared_hmac_key[] = { 's', 'h', 'a', 'r', 'e', 'd' };
    static const uint8_t shared_aes_key[16] = {
        0x20u,0x21u,0x22u,0x23u,0x24u,0x25u,0x26u,0x27u,
        0x28u,0x29u,0x2au,0x2bu,0x2cu,0x2du,0x2eu,0x2fu
    };
    ano_crypto_csp_handle_t shared_hmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t shared_aes_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_handle_t shared_cmac_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        fprintf(stderr, "module init failed: %s\n", ano_crypto_status_name(st));
        return 1;
    }

    if (run_worker_group(
            0,
            ANO_CRYPTO_CSP_INVALID_HANDLE,
            ANO_CRYPTO_CSP_INVALID_HANDLE,
            ANO_CRYPTO_CSP_INVALID_HANDLE) != 0) {
        return 1;
    }

    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_HMAC,
        shared_hmac_key,
        sizeof(shared_hmac_key),
        ANO_CRYPTO_KEY_FLAG_NONE,
        &shared_hmac_handle);
    if (st != ANO_CRYPTO_OK) return 1;
    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
        shared_aes_key,
        sizeof(shared_aes_key),
        ANO_CRYPTO_KEY_FLAG_NONE,
        &shared_aes_handle);
    if (st != ANO_CRYPTO_OK) {
        (void)ano_crypto_key_destroy(shared_hmac_handle);
        return 1;
    }
    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_CMAC,
        shared_aes_key,
        sizeof(shared_aes_key),
        ANO_CRYPTO_KEY_FLAG_NONE,
        &shared_cmac_handle);
    if (st != ANO_CRYPTO_OK) {
        (void)ano_crypto_key_destroy(shared_aes_handle);
        (void)ano_crypto_key_destroy(shared_hmac_handle);
        return 1;
    }

    if (run_worker_group(
            1, shared_hmac_handle, shared_aes_handle, shared_cmac_handle) != 0) {
        (void)ano_crypto_key_destroy(shared_cmac_handle);
        (void)ano_crypto_key_destroy(shared_aes_handle);
        (void)ano_crypto_key_destroy(shared_hmac_handle);
        return 1;
    }

    if (ano_crypto_key_destroy(shared_cmac_handle) != ANO_CRYPTO_OK ||
        ano_crypto_key_destroy(shared_aes_handle) != ANO_CRYPTO_OK ||
        ano_crypto_key_destroy(shared_hmac_handle) != ANO_CRYPTO_OK) {
        return 1;
    }
    if (ano_crypto_module_shutdown() != ANO_CRYPTO_OK) return 1;

    puts("anocrypto-c csp_thread_safety_tests: PASS (HMAC/AES/CMAC values not printed)");
    return 0;
}
