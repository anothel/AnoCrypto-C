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

ano_crypto_status_t ano_crypto_internal_entropy_source_init_none(
    ano_crypto_internal_entropy_source_t *source)
{
    if (source == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_callback(
    ano_crypto_internal_entropy_source_t *source,
    ano_crypto_internal_entropy_read_fn read_fn,
    void *user_context,
    size_t min_request_len)
{
    if (source == 0 || read_fn == 0 || min_request_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK;
    source->min_request_len = min_request_len;
    source->read_fn = read_fn;
    source->user_context = user_context;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len)
{
    if (source == 0 || min_request_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    ano_crypto_internal_zeroize(source, sizeof(*source));
    source->kind = ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS;
    source->min_request_len = min_request_len;
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_entropy_source_init_os_reserved(
    ano_crypto_internal_entropy_source_t *source,
    size_t min_request_len)
{
    return ano_crypto_internal_entropy_source_init_os(source, min_request_len);
}

int ano_crypto_internal_platform_entropy_is_available(void)
{
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

ano_crypto_status_t ano_crypto_internal_entropy_source_get(
    const ano_crypto_internal_entropy_source_t *source,
    uint8_t *out,
    size_t out_len)
{
    ano_crypto_status_t st;

    if (source == 0 || out == 0 || out_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
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
            return st == ANO_CRYPTO_OK ? ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED : st;
        }
        return ANO_CRYPTO_OK;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_OS:
        st = ano_crypto_internal_platform_entropy_get(out, out_len);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(out, out_len);
        }
        return st;

    case ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_NONE:
    default:
        ano_crypto_internal_zeroize(out, out_len);
        return ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED;
    }
}
