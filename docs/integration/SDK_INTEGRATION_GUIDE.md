# AnoCrypto-C SDK Integration Guide

## Naming contract

- Product spelling: `AnoCrypto-C`
- Repository slug: `anocrypto-c`
- CMake package: `AnoCryptoC`
- CMake imported target: `AnoCryptoC::AnoCryptoC`
- Library target/file: `anocrypto_c`
- Include root: `include/anocrypto/`
- Public umbrella header: `#include <anocrypto/anocrypto.h>`
- C API prefix: `ano_crypto_`
- Macro prefix: `ANO_CRYPTO_`

## Minimal CMake consumer

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_anocrypto_consumer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(AnoCryptoC CONFIG REQUIRED)

add_executable(my_consumer main.c)
target_link_libraries(my_consumer PRIVATE AnoCryptoC::AnoCryptoC)
```

Configure with the installed SDK prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/anocrypto-c-prefix
cmake --build build
```

## Basic lifecycle pattern

1. Query version/build identity if needed.
2. Call `ano_crypto_module_init(NULL)` once during startup.
3. Treat any non-`ANO_CRYPTO_OK` return as a hard integration failure unless the specific API documents another recovery path.
4. Use cryptographic services only after the module is operational.
5. Destroy CSP handles and uninstantiate DRBG contexts during cleanup.
6. Call `ano_crypto_module_shutdown()` during normal process teardown.

## Status-code pattern

All public processing APIs return `ano_crypto_status_t`. Results are returned through output parameters.

```c
uint8_t digest[32];
size_t written = 0u;
ano_crypto_status_t st;

st = ano_crypto_hash_digest(ANO_CRYPTO_HASH_ALG_SHA256,
                            input,
                            input_len,
                            digest,
                            sizeof(digest),
                            &written);
if (st != ANO_CRYPTO_OK) {
    /* Map or report st without logging secrets. */
}
```

## Buffer and logging rules

- Do not log keys, entropy input, nonce input, DRBG state, generated secret bytes, private keys, tokens, or customer secret material.
- Check all output lengths.
- Treat zero-length input behavior as API-specific.
- Do not assume caller-owned hash, HMAC, or DRBG contexts are safe for concurrent mutation.
- Destroy CSP handles when no longer needed.
- Zeroize caller-owned buffers containing sensitive material when practical.

## PoC example

The `examples/core_poc` project is the reference v0 customer integration smoke. It intentionally uses only public headers and the installed CMake package.

```sh
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/path/to/anocrypto-c-prefix
cmake --build build-core-poc
./build-core-poc/anocrypto_c_core_poc
```

The example prints status-only success messages. It does not print raw key material, entropy bytes, nonce bytes, DRBG state, or generated DRBG bytes.

## Current limitations

- SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 remain ABI placeholders.
- Hash_DRBG remains an ABI placeholder.
- Public `ano_crypto_random_*` APIs are not exposed yet.
- HMAC key handles are in-memory only.
- Persistent keystore, key export, key wrapping, and multi-process CSP behavior are not finalized.
- Public build identity currently uses a versioned struct and remains a tracked ABI TODO before ABI freeze.
