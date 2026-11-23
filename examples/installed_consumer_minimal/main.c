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

#include <anocrypto/anocrypto.h>

int main(void)
{
    static const uint8_t input[] = {0x61, 0x62, 0x63}; /* "abc" */
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
    } else if (written != sizeof(digest)) {
        fputs("SHA-256 returned an unexpected digest length\n", stderr);
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
