#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include "ac_internal.h"
#include "ac_process_guard_internal.h"

uint64_t ano_crypto_internal_process_marker_current(void)
{
#if defined(_WIN32)
    return (uint64_t)GetCurrentProcessId();
#elif defined(__unix__) || defined(__APPLE__)
    return (uint64_t)getpid();
#else
    return 0u;
#endif
}

ano_crypto_status_t ano_crypto_internal_process_guard_refresh(
    ano_crypto_internal_process_guard_t *guard)
{
    uint64_t marker;

    if (guard == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(guard, sizeof(*guard));
    marker = ano_crypto_internal_process_marker_current();
    if (marker == 0u) {
        return ANO_CRYPTO_OK;
    }

    guard->process_marker = marker;
    guard->active = 1u;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_process_guard_init(
    ano_crypto_internal_process_guard_t *guard)
{
    return ano_crypto_internal_process_guard_refresh(guard);
}

ano_crypto_status_t ano_crypto_internal_process_guard_check_marker(
    const ano_crypto_internal_process_guard_t *guard,
    uint64_t current_marker)
{
    if (guard == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (guard->active == 0u) {
        return ANO_CRYPTO_OK;
    }

    if (current_marker == 0u) {
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    if (guard->process_marker != current_marker) {
        return ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED;
    }

    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_process_guard_check(
    const ano_crypto_internal_process_guard_t *guard)
{
    if (guard == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (guard->active == 0u) {
        return ANO_CRYPTO_OK;
    }

    return ano_crypto_internal_process_guard_check_marker(
        guard,
        ano_crypto_internal_process_marker_current());
}

void ano_crypto_internal_process_guard_reset(
    ano_crypto_internal_process_guard_t *guard)
{
    if (guard != 0) {
        ano_crypto_internal_zeroize(guard, sizeof(*guard));
    }
}
