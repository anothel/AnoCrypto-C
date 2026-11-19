/* Standalone fixture: compile the real lifecycle/lock code, replacing only
 * startup work and CSP cleanup with controlled dependencies. No library hooks.
 * Real KATs and byte-clearing behavior remain covered by library-backed tests. */
#include <stdio.h>
#include <stdlib.h>
#include <anocrypto/ac_module.h>
#include <anocrypto/ac_selftest.h>
#include "ac_internal.h"
#include "ac_csp_internal.h"
#include "ac_thread_internal.h"

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expr); exit(1); \
} } while (0)

#if ANO_CRYPTO_ENABLE_THREAD_SAFETY && (defined(_WIN32) || defined(__unix__) || defined(__APPLE__))
#define HAVE_RACE_THREADS 1
#ifdef _WIN32
#include <windows.h>
static CRITICAL_SECTION gate;
static CONDITION_VARIABLE changed;
typedef HANDLE test_thread;
#define THREAD_RESULT DWORD WINAPI
#define THREAD_ARG LPVOID
static void gate_init(void) { InitializeCriticalSection(&gate); InitializeConditionVariable(&changed); }
static void gate_lock(void) { EnterCriticalSection(&gate); }
static void gate_unlock(void) { LeaveCriticalSection(&gate); }
static void gate_wait(void) { CHECK(SleepConditionVariableCS(&changed, &gate, INFINITE)); }
static void gate_signal(void) { WakeAllConditionVariable(&changed); }
#else
#include <pthread.h>
static pthread_mutex_t gate = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t changed = PTHREAD_COND_INITIALIZER;
typedef pthread_t test_thread;
#define THREAD_RESULT void *
#define THREAD_ARG void *
static void gate_init(void) { }
static void gate_lock(void) { CHECK(pthread_mutex_lock(&gate) == 0); }
static void gate_unlock(void) { CHECK(pthread_mutex_unlock(&gate) == 0); }
static void gate_wait(void) { CHECK(pthread_cond_wait(&changed, &gate) == 0); }
static void gate_signal(void) { CHECK(pthread_cond_broadcast(&changed) == 0); }
#endif
static unsigned entered;
static unsigned released[2];
static int pause_startup;
static int fail_old;
#endif
static unsigned cleanup_calls;
static unsigned startup_calls;

void ano_crypto_internal_csp_zeroize_all_unlocked(void)
{
    ++cleanup_calls;
}

ano_crypto_status_t ano_crypto_selftest_run(uint32_t flags)
{
    CHECK((flags & ANO_CRYPTO_SELFTEST_FLAG_STARTUP) != 0u);
#if defined(HAVE_RACE_THREADS)
    if (pause_startup) {
        unsigned call;
        int fail;
        gate_lock();
        call = entered++;
        CHECK(call < 2u);
        gate_signal();
        while (!released[call]) gate_wait();
        fail = fail_old && call == 0u;
        gate_unlock();
        /* Model a mandatory failure's real sticky-error side effect. */
        if (fail) {
            ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_SELF_TEST_FAILED);
            return ANO_CRYPTO_ERR_SELF_TEST_FAILED;
        }
        return ANO_CRYPTO_OK;
    }
#endif
    ++startup_calls;
    return ANO_CRYPTO_OK;
}

static void expect_state(ano_crypto_module_state_t expected)
{
    ano_crypto_module_state_t state;
    CHECK(ano_crypto_module_get_state(&state) == ANO_CRYPTO_OK);
    if (state != expected) {
        fprintf(stderr, "state=%s expected=%s\n", ano_crypto_module_state_name(state),
                ano_crypto_module_state_name(expected));
        exit(1);
    }
}

#if defined(HAVE_RACE_THREADS)
static THREAD_RESULT initialize(THREAD_ARG arg)
{
    ano_crypto_status_t *result = (ano_crypto_status_t *)arg;
    *result = ano_crypto_module_init(0);
    return 0;
}
static void start(test_thread *thread, ano_crypto_status_t *result)
{
#ifdef _WIN32
    *thread = CreateThread(0, 0, initialize, result, 0, 0);
    CHECK(*thread != 0);
#else
    CHECK(pthread_create(thread, 0, initialize, result) == 0);
#endif
}
static void join(test_thread thread)
{
#ifdef _WIN32
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(CloseHandle(thread));
#else
    CHECK(pthread_join(thread, 0) == 0);
#endif
}
static void await_start(unsigned count)
{
    gate_lock();
    while (entered < count) gate_wait();
    gate_unlock();
}
static void release_start(unsigned call)
{
    gate_lock();
    released[call] = 1u;
    gate_signal();
    gate_unlock();
}
static void race(int scenario)
{
    test_thread old_thread, new_thread;
    ano_crypto_status_t old_result = ANO_CRYPTO_ERR_INTERNAL_ERROR;
    ano_crypto_status_t new_result = ANO_CRYPTO_ERR_INTERNAL_ERROR;
    const int with_new = scenario == 1 || scenario == 2 || scenario == 4;
    gate_init();
    pause_startup = 1;
    fail_old = scenario == 4;
    start(&old_thread, &old_result);
    await_start(1u);
    expect_state(ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING);
    /* A competing init while startup is current must not start more work. */
    CHECK(ano_crypto_module_init(0) == ANO_CRYPTO_ERR_FATAL_STATE);
    CHECK(ano_crypto_module_shutdown() == ANO_CRYPTO_OK);
    CHECK(cleanup_calls == 1u);
    expect_state(ANO_CRYPTO_MODULE_STATE_SHUTDOWN);
    CHECK(ano_crypto_module_require_operational() == ANO_CRYPTO_ERR_NOT_INITIALIZED);
    if (with_new) {
        start(&new_thread, &new_result);
        await_start(2u);
        if (scenario != 1) {
            release_start(1u);
            join(new_thread);
            CHECK(new_result == ANO_CRYPTO_OK);
            expect_state(ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
        }
    }
    if (scenario == 3) {
        ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_SELF_TEST_FAILED);
    }
    release_start(0u);
    join(old_thread);
    fprintf(stdout, "scenario %d: old init result=%s\n", scenario, ano_crypto_status_name(old_result));
    if (scenario >= 3) {
        CHECK(old_result == (scenario == 4 ? ANO_CRYPTO_ERR_SELF_TEST_FAILED : ANO_CRYPTO_ERR_FATAL_STATE));
        expect_state(ANO_CRYPTO_MODULE_STATE_ERROR_STATE);
        CHECK(cleanup_calls >= 2u);
        CHECK(ano_crypto_module_shutdown() == ANO_CRYPTO_OK);
        CHECK(ano_crypto_module_init(0) == ANO_CRYPTO_ERR_FATAL_STATE);
        CHECK(ano_crypto_module_require_operational() == ANO_CRYPTO_ERR_FATAL_STATE);
    } else {
        CHECK(old_result == ANO_CRYPTO_ERR_NOT_INITIALIZED);
        expect_state(scenario == 0 ? ANO_CRYPTO_MODULE_STATE_SHUTDOWN :
                     scenario == 1 ? ANO_CRYPTO_MODULE_STATE_SELF_TEST_RUNNING :
                                     ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
        if (scenario == 1) {
            CHECK(ano_crypto_module_require_operational() == ANO_CRYPTO_ERR_NOT_INITIALIZED);
            release_start(1u);
            join(new_thread);
            CHECK(new_result == ANO_CRYPTO_OK);
        }
        if (scenario == 0) {
            CHECK(ano_crypto_module_require_operational() == ANO_CRYPTO_ERR_NOT_INITIALIZED);
            pause_startup = 0;
            CHECK(ano_crypto_module_init(0) == ANO_CRYPTO_OK);
            CHECK(startup_calls == 1u);
        }
        expect_state(ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    }
}
#endif
int main(int argc, char **argv)
{
    int scenario = argc == 2 ? atoi(argv[1]) : -1;
#if defined(HAVE_RACE_THREADS)
    if (scenario >= 0 && scenario <= 4) {
        race(scenario);
        puts("module init/shutdown deterministic race: PASS");
        return 0;
    }
#else
    (void)scenario;
    puts("thread-safety OFF/unsupported: concurrent schedules not run; serialized contract only");
#endif
    CHECK(ano_crypto_module_init(0) == ANO_CRYPTO_OK);
    CHECK(ano_crypto_module_shutdown() == ANO_CRYPTO_OK);
    CHECK(ano_crypto_module_init(0) == ANO_CRYPTO_OK);
    CHECK(startup_calls == 2u && cleanup_calls == 1u);
    expect_state(ANO_CRYPTO_MODULE_STATE_OPERATIONAL);
    puts("serialized shutdown/reinit: PASS");
    return 0;
}
