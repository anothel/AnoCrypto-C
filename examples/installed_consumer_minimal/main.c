/*
 * Copy only this file and CMakeLists.txt into an empty directory. From there,
 * set CMAKE_PREFIX_PATH to the root of the separately installed SDK and run:
 *
 * cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/installed-sdk -DCMAKE_BUILD_TYPE=Release
 * cmake --build build
 * ./build/anocrypto_c_minimal_consumer
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <anocrypto/anocrypto.h>

int main(void)
{
    static const uint8_t input[] = {0x61, 0x62, 0x63}; /* "abc" */
    static const uint8_t expected_digest[] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    uint8_t digest[ANO_CRYPTO_HASH_SHA256_DIGEST_SIZE];
    size_t written = 0;
    ano_crypto_status_t hash_status;
    ano_crypto_status_t shutdown_status;
    const ano_crypto_status_t init_status = ano_crypto_module_init(NULL);
    int result = EXIT_SUCCESS;

    if (init_status != ANO_CRYPTO_OK) {
        fprintf(stderr, "Module initialization failed (status %d)\n", (int)init_status);
        return EXIT_FAILURE;
    }

    hash_status = ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                                        input, sizeof(input),
                                        digest, sizeof(digest), &written);
    shutdown_status = ano_crypto_module_shutdown();

    if (hash_status != ANO_CRYPTO_OK) {
        fprintf(stderr, "SHA-256 failed (status %d)\n", (int)hash_status);
        result = EXIT_FAILURE;
    } else if (written != sizeof(expected_digest)) {
        fputs("SHA-256 returned an unexpected digest length\n", stderr);
        result = EXIT_FAILURE;
    } else if (memcmp(digest, expected_digest, sizeof(expected_digest)) != 0) {
        fputs("SHA-256 known-answer check failed\n", stderr);
        result = EXIT_FAILURE;
    }

    if (shutdown_status != ANO_CRYPTO_OK) {
        fprintf(stderr, "Module shutdown failed (status %d)\n", (int)shutdown_status);
        result = EXIT_FAILURE;
    }

    if (result == EXIT_SUCCESS) {
        puts("SHA-256 and module shutdown succeeded");
    }
    return result;
}
