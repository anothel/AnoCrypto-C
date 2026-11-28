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

For a complete standalone C99 project, start with the
[minimal installed SDK example](../../examples/installed_consumer_minimal/).
Copy its [`CMakeLists.txt`](../../examples/installed_consumer_minimal/CMakeLists.txt)
and [`main.c`](../../examples/installed_consumer_minimal/main.c) into an empty
directory and follow the [README quick start](../../README.md#minimal-installed-sdk-consumer).
The example makes one SHA-256 request and prints only status messages. The
[Core PoC](../../examples/core_poc/) covers more SDK operations, and the
[install smoke test](../../README.md#install-smoke-test) checks SDK installation.

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

### Package selection and version errors

Use a fresh build directory when changing SDK installations. CMake caches
package discovery, so a build directory configured with a previous SDK can
keep using that SDK. Quote absolute paths so prefixes containing spaces are
passed as one argument.

Use `CMAKE_PREFIX_PATH` to give CMake an installation prefix to search. To
point directly to the package configuration, set `AnoCryptoC_DIR` to the
directory containing `AnoCryptoCConfig.cmake`. For an SDK installed with
`CMAKE_INSTALL_LIBDIR=lib`, for example:

```sh
cmake -S . -B build-prefix "-DCMAKE_PREFIX_PATH=/opt/AnoCrypto SDK"
cmake -S . -B build-config "-DAnoCryptoC_DIR=/opt/AnoCrypto SDK/lib/cmake/AnoCryptoC"
cmake --build build-config
```

For other layouts, use the installed SDK's actual library directory. Check
the resolved `AnoCryptoC_DIR` entry in the chosen build's `CMakeCache.txt`
against the intended config directory. CMake can search elsewhere when an
explicit directory is missing or its package version is incompatible.
Treat a resolved path outside the intended SDK as a configuration error.

For a trusted Linux shared-library build, inspect `ldd build-config/my_consumer`
for the resolved `libanocrypto_c.so` entry. Canonicalize the reported path, check
that it resolves to the intended SDK library, and compare its SHA-256 with the
expected artifact digest when one is available. The
[minimal consumer smoke runner](../../tools/run_minimal_consumer_smoke.cmake)
shows how to verify package selection and loader resolution together.

The [exact-version probe](../../tests/package_version/exact/) requests CMake
package version `0.1.0` and checks that the loaded library reports runtime
version `0.1.0-p3-10-restricted-sha1-ocsp-certid`. These version checks answer
which versions the package and library declare. Verify the resolved paths and
digest for artifact identity. ABI compatibility requires separate evidence.

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

## Thread-safety integration notes

- Build with `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` for the current intended global-state locking configuration.
- Initialize the module once at process startup when possible.
- Do not mutate the same hash, HMAC, or DRBG context from multiple threads. Use separate contexts or external serialization.
- HMAC key-handle import, destroy, state query, and compute-with-key are guarded by the internal CSP lock in the current P1 slice.
- Review `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` before relying on any concurrency behavior in a customer PoC.

## PoC example

The `examples/core_poc` project is the reference v0 customer integration smoke. It intentionally uses only public headers and the installed CMake package.

```sh
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/path/to/anocrypto-c-prefix
cmake --build build-core-poc
./build-core-poc/anocrypto_c_core_poc
```

The example prints status-only success messages. It does not print raw key material, entropy bytes, nonce bytes, DRBG state, or generated DRBG bytes.

## Current limitations

- SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 are implemented. See
  [`ac_hash.h`](../../include/anocrypto/ac_hash.h) and
  [`ac_hmac.h`](../../include/anocrypto/ac_hmac.h).
- Hash_DRBG remains ABI-reserved and unimplemented.
- The public `ano_crypto_random_*` service is available only when a supported
  platform entropy provider and a process-identity marker are both available.
  Prediction-resistance requests remain unsupported. See
  [`ac_random.h`](../../include/anocrypto/ac_random.h).
- HMAC key handles are in-memory only.
- Persistent keystore, key export, key wrapping, and multi-process CSP behavior are not finalized.
- Public build identity currently uses a versioned struct and remains a tracked ABI TODO before ABI freeze.
