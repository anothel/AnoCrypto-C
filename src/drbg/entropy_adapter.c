#include <stddef.h>
#include <stdint.h>

#if defined(ANO_CRYPTO_USE_LINUX_GETRANDOM)
#include <errno.h>
#include <sys/random.h>
#endif

#if defined(ANO_CRYPTO_USE_WINDOWS_BCRYPTGENRANDOM)
#include <windows.h>
#include <bcrypt.h>
#endif

#if defined(ANO_CRYPTO_USE_APPLE_SECRANDOMCOPYBYTES)
#include <Security/SecRandom.h>
#endif

#include "ac_entropy_internal.h"
#include "ac_internal.h"

#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
typedef struct ano_crypto_internal_entropy_test_hook_state_s {
    uint32_t mode;
    uint8_t seed;
    uint8_t next;
    size_t partial_len;
} ano_crypto_internal_entropy_test_hook_state_t;

static ano_crypto_internal_entropy_test_hook_state_t g_entropy_test_hook;

static int ano_crypto_internal_entropy_test_hook_is_active(void)
{
    return g_entropy_test_hook.mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_OFF ? 1 : 0;
}

ano_crypto_status_t ano_crypto_internal_entropy_test_hook_configure(
    uint32_t mode,
    uint8_t seed,
    size_t partial_len)
{
    if (mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_OFF &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_REPEAT &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL &&
        mode != ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (partial_len > ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    ano_crypto_internal_zeroize(&g_entropy_test_hook, sizeof(g_entropy_test_hook));
    g_entropy_test_hook.mode = mode;
    g_entropy_test_hook.seed = seed;
    g_entropy_test_hook.next = seed;
    g_entropy_test_hook.partial_len = partial_len;
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_entropy_test_hook_reset(void)
{
    ano_crypto_internal_zeroize(&g_entropy_test_hook, sizeof(g_entropy_test_hook));
}

static ano_crypto_status_t ano_crypto_internal_entropy_test_hook_read(
    uint8_t *out,
    size_t out_len)
{
    size_t i;
    size_t partial_len;

    switch (g_entropy_test_hook.mode) {
    case ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_SEQUENCE:
        for (i = 0u; i < out_len; ++i) {
            out[i] = g_entropy_test_hook.next;
            ++g_entropy_test_hook.next;
        }
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_DETERMINISTIC_REPEAT:
        for (i = 0u; i < out_len; ++i) {
            out[i] = (uint8_t)(g_entropy_test_hook.seed + (uint8_t)i);
        }
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_FAIL:
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;

    case ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_PARTIAL_FAIL:
        partial_len = g_entropy_test_hook.partial_len;
        if (partial_len > out_len) {
            partial_len = out_len;
        }
        for (i = 0u; i < partial_len; ++i) {
            out[i] = (uint8_t)(g_entropy_test_hook.seed + (uint8_t)i);
        }
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;

    case ANO_CRYPTO_INTERNAL_ENTROPY_TEST_HOOK_OFF:
    default:
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }
}
#endif

ano_crypto_status_t ano_crypto_internal_entropy_source_init_none(
    ano_crypto_internal_entropy_source_t *source)
{
    if (source == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE;
    source->source_class = ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_NONE;
    source->health_test_owner = ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_NONE;
    source->restart_policy = ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_NONE;
    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback_common(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len,
    uint32_t source_class,
    uint32_t health_test_owner)
{
    if (source == 0 || read_fn == 0 || min_request_len == 0u ||
        min_request_len > ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK;
    source->source_class = source_class;
    source->min_request_len = min_request_len;
    source->read_fn = read_fn;
    source->user_context = user_context;
    source->health_test_owner = health_test_owner;
    source->restart_policy = ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_CALLER_DEFINED;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len)
{
    return ano_crypto_internal_entropy_source_init_callback_common(
        source,
        read_fn,
        user_context,
        min_request_len,
        ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_CALLBACK_UNSPECIFIED,
        ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_CALLER);
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback_noise_candidate(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len)
{
    return ano_crypto_internal_entropy_source_init_callback_common(
        source,
        read_fn,
        user_context,
        min_request_len,
        ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_RAW_NOISE_TEST_CANDIDATE,
        ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_MODULE_ENGINEERING_CANDIDATE);
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len)
{
    if (source == 0 || min_request_len == 0u ||
        min_request_len > ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS;
    source->source_class = ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG;
    source->min_request_len = min_request_len;
    source->health_test_owner = ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER;
    source->restart_policy = ANO_CRYPTO_INTERNAL_ENTROPY_RESTART_POLICY_PROVIDER_PER_CALL;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os_reserved(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len)
{
    return ano_crypto_internal_entropy_source_init_os(source, min_request_len);
}

ano_crypto_status_t ano_crypto_internal_entropy_source_get_policy(
    const ano_crypto_internal_entropy_source_t *source,
    uint32_t *out_source_class,
    uint32_t *out_health_test_owner,
    uint32_t *out_restart_policy)
{
    if (source == 0 || out_source_class == 0 || out_health_test_owner == 0 ||
        out_restart_policy == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_source_class = source->source_class;
    *out_health_test_owner = source->health_test_owner;
    *out_restart_policy = source->restart_policy;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_probe_operational(
    const ano_crypto_internal_entropy_source_t *source)
{
    uint8_t probe[ANO_CRYPTO_INTERNAL_ENTROPY_OPERATIONAL_PROBE_LEN];
    ano_crypto_status_t st;

    if (source == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (source->kind != ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS ||
        source->source_class != ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (source->min_request_len > sizeof(probe)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_internal_platform_entropy_get(probe, sizeof(probe));
    ano_crypto_internal_zeroize(probe, sizeof(probe));
    return st;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_attach_health_test(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_health_t *health,
    uint32_t failure_policy)
{
    if (source == 0 || health == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (source->kind != ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (source->source_class != ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_RAW_NOISE_TEST_CANDIDATE ||
        source->health_test_owner != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_MODULE_ENGINEERING_CANDIDATE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (failure_policy != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_OPERATION_ONLY &&
        failure_policy != ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_MODULE_FATAL) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    source->health = health;
    source->health_failure_policy = failure_policy;
    return ANO_CRYPTO_OK;
}

int ano_crypto_internal_platform_entropy_is_available(void)
{
#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
    if (ano_crypto_internal_entropy_test_hook_is_active()) {
        return 1;
    }
#endif

#if defined(ANO_CRYPTO_USE_LINUX_GETRANDOM) || \
    defined(ANO_CRYPTO_USE_WINDOWS_BCRYPTGENRANDOM) || \
    defined(ANO_CRYPTO_USE_APPLE_SECRANDOMCOPYBYTES)
    return 1;
#else
    return 0;
#endif
}

const char *ano_crypto_internal_platform_entropy_provider_name(void)
{
#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
    if (ano_crypto_internal_entropy_test_hook_is_active()) {
        return "internal-test-hook";
    }
#endif

#if defined(ANO_CRYPTO_USE_LINUX_GETRANDOM)
    return "linux-getrandom";
#elif defined(ANO_CRYPTO_USE_WINDOWS_BCRYPTGENRANDOM)
    return "windows-bcryptgenrandom";
#elif defined(ANO_CRYPTO_USE_APPLE_SECRANDOMCOPYBYTES)
    return "apple-secrandomcopybytes";
#else
    return "unsupported";
#endif
}

static ano_crypto_status_t platform_entropy_get_impl(uint8_t *out, size_t out_len)
{
#if defined(ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS)
    if (ano_crypto_internal_entropy_test_hook_is_active()) {
        return ano_crypto_internal_entropy_test_hook_read(out, out_len);
    }
#endif

#if defined(ANO_CRYPTO_USE_LINUX_GETRANDOM)
    size_t offset = 0u;

    while (offset < out_len) {
        const size_t remaining = out_len - offset;
        ssize_t nread = getrandom(out + offset, remaining, 0u);
        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        if (nread == 0) {
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        offset += (size_t)nread;
    }

    return ANO_CRYPTO_OK;
#elif defined(ANO_CRYPTO_USE_WINDOWS_BCRYPTGENRANDOM)
    size_t offset = 0u;

    while (offset < out_len) {
        const size_t remaining = out_len - offset;
        ULONG chunk = remaining > (size_t)0xffffffffUL ? (ULONG)0xffffffffUL : (ULONG)remaining;
        NTSTATUS status = BCryptGenRandom(NULL, out + offset, chunk, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (!BCRYPT_SUCCESS(status)) {
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        offset += (size_t)chunk;
    }

    return ANO_CRYPTO_OK;
#elif defined(ANO_CRYPTO_USE_APPLE_SECRANDOMCOPYBYTES)
    if (SecRandomCopyBytes(kSecRandomDefault, out_len, out) != 0) {
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }
    return ANO_CRYPTO_OK;
#else
    (void)out;
    (void)out_len;
    return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
#endif
}

ano_crypto_status_t ano_crypto_internal_platform_entropy_get(
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_status_t st;

    if (out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (out_len > ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (!ano_crypto_internal_platform_entropy_is_available()) {
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    st = platform_entropy_get_impl(out, out_len);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out, out_len);
        if (st == ANO_CRYPTO_ERR_NOT_IMPLEMENTED) {
            return st;
        }
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    return ANO_CRYPTO_OK;
}

static ano_crypto_status_t ano_crypto_internal_entropy_apply_health_test(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_status_t st;

    if (source->health == 0) {
        return ANO_CRYPTO_OK;
    }

    st = ano_crypto_internal_entropy_health_evaluate(source->health, out, out_len);
    if (st == ANO_CRYPTO_OK) {
        return ANO_CRYPTO_OK;
    }

    ano_crypto_internal_zeroize(out, out_len);
    if (source->health_failure_policy ==
        ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_FAILURE_MODULE_FATAL) {
        ano_crypto_internal_module_mark_error(ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED);
    }

    return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_get(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_status_t st;

    if (source == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (out_len > ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (source->min_request_len != 0u && out_len < source->min_request_len) {
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    switch (source->kind) {
    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK:
        if (source->read_fn == 0) {
            ano_crypto_internal_zeroize(out, out_len);
            return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
        }
        st = source->read_fn(source->user_context, out, out_len);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(out, out_len);
            return st;
        }
        break;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS:
        st = ano_crypto_internal_platform_entropy_get(out, out_len);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(out, out_len);
            return st;
        }
        break;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE:
    default:
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }

    return ano_crypto_internal_entropy_apply_health_test(source, out, out_len);
}
