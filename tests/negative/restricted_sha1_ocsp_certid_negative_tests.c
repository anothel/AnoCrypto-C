#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <anocrypto/anocrypto.h>

static int failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++failures;
    }
}

static void expect_zero(const char *name, const uint8_t *buffer, size_t length)
{
    size_t index;
    for (index = 0u; index < length; ++index) {
        if (buffer[index] != 0u) {
            fprintf(stderr, "FAIL %s: byte %lu not cleared\n", name, (unsigned long)index);
            ++failures;
            return;
        }
    }
}

int main(void)
{
    size_t aligned_storage[8];
    uint8_t *storage = (uint8_t *)(void *)aligned_storage;
    uint8_t output[ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE];
    size_t written = 123u;

    (void)memset(output, 0xa5, sizeof(output));
    expect_status("before init",
        ano_crypto_ocsp_certid_sha1_digest(storage, 3u, output, sizeof(output), &written),
        ANO_CRYPTO_ERR_NOT_INITIALIZED);
    expect_zero("before init output", output, sizeof(output));
    if (written != 0u) {
        fprintf(stderr, "FAIL before init written\n");
        ++failures;
    }

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);

    (void)memset(output, 0xa5, sizeof(output));
    written = 123u;
    expect_status("null input nonzero",
        ano_crypto_ocsp_certid_sha1_digest(0, 1u, output, sizeof(output), &written),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("null input output", output, sizeof(output));

    written = 123u;
    expect_status("null output",
        ano_crypto_ocsp_certid_sha1_digest(storage, 3u, 0, sizeof(output), &written),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    if (written != 0u) {
        fprintf(stderr, "FAIL null output written\n");
        ++failures;
    }

    (void)memset(output, 0xa5, sizeof(output));
    expect_status("null written",
        ano_crypto_ocsp_certid_sha1_digest(storage, 3u, output, sizeof(output), 0),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("null written output", output, sizeof(output));

    (void)memset(output, 0xa5, sizeof(output));
    written = 123u;
    expect_status("short output",
        ano_crypto_ocsp_certid_sha1_digest(storage, 3u, output, sizeof(output) - 1u, &written),
        ANO_CRYPTO_ERR_BUFFER_TOO_SMALL);
    expect_zero("short output cleared", output, sizeof(output) - 1u);
    if (written != 0u) {
        fprintf(stderr, "FAIL short output written\n");
        ++failures;
    }

    (void)memset(storage, 0xa5, sizeof(aligned_storage));
    written = 123u;
    expect_status("input output overlap",
        ano_crypto_ocsp_certid_sha1_digest(storage, 32u, storage + 4u,
                                            ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE,
                                            &written),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("overlap output cleared", storage + 4u,
                ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE);

    (void)memset(storage, 0xa5, sizeof(aligned_storage));
    expect_status("written overlaps output",
        ano_crypto_ocsp_certid_sha1_digest(storage + 32u, 8u, storage,
                                            ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE,
                                            (size_t *)(void *)(storage + 8u)),
        ANO_CRYPTO_ERR_INVALID_ARGUMENT);
    expect_zero("written overlap output cleared", storage,
                ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (failures != 0) {
        fprintf(stderr, "restricted_sha1_ocsp_certid_negative_tests: %d failure(s)\n", failures);
        return 1;
    }
    puts("restricted_sha1_ocsp_certid_negative_tests: PASS");
    return 0;
}
