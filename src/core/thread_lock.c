#include "ac_thread_internal.h"

#if defined(ANO_CRYPTO_ENABLE_THREAD_SAFETY)
#  if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

static INIT_ONCE g_ano_crypto_lock_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION g_ano_crypto_global_lock;

static BOOL CALLBACK ano_crypto_init_global_lock_once(
    PINIT_ONCE init_once,
    PVOID parameter,
    PVOID *context)
{
    (void)init_once;
    (void)parameter;
    (void)context;
    InitializeCriticalSection(&g_ano_crypto_global_lock);
    return TRUE;
}

ano_crypto_status_t ano_crypto_internal_global_lock(void)
{
    if (!InitOnceExecuteOnce(
            &g_ano_crypto_lock_once,
            ano_crypto_init_global_lock_once,
            0,
            0)) {
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    EnterCriticalSection(&g_ano_crypto_global_lock);
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_global_unlock(void)
{
    LeaveCriticalSection(&g_ano_crypto_global_lock);
}

int ano_crypto_internal_thread_safety_available(void)
{
    return 1;
}

const char *ano_crypto_internal_thread_backend_name(void)
{
    return "windows-critical-section";
}

#  elif defined(__unix__) || defined(__APPLE__)
#    include <pthread.h>

static pthread_mutex_t g_ano_crypto_global_lock = PTHREAD_MUTEX_INITIALIZER;

ano_crypto_status_t ano_crypto_internal_global_lock(void)
{
    if (pthread_mutex_lock(&g_ano_crypto_global_lock) != 0) {
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_global_unlock(void)
{
    (void)pthread_mutex_unlock(&g_ano_crypto_global_lock);
}

int ano_crypto_internal_thread_safety_available(void)
{
    return 1;
}

const char *ano_crypto_internal_thread_backend_name(void)
{
    return "pthread-mutex";
}

#  else

/*
 * Unsupported platform fallback.
 *
 * This keeps the source buildable while making the policy explicit in
 * docs/design/C99_THREAD_SAFETY_POLICY.md: this path is not a reviewed
 * thread-safety implementation.
 */
ano_crypto_status_t ano_crypto_internal_global_lock(void)
{
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_global_unlock(void)
{
}

int ano_crypto_internal_thread_safety_available(void)
{
    return 0;
}

const char *ano_crypto_internal_thread_backend_name(void)
{
    return "unsupported-noop";
}

#  endif
#else

ano_crypto_status_t ano_crypto_internal_global_lock(void)
{
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_global_unlock(void)
{
}

int ano_crypto_internal_thread_safety_available(void)
{
    return 0;
}

const char *ano_crypto_internal_thread_backend_name(void)
{
    return "disabled-noop";
}

#endif
