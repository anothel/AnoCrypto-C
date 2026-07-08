# AnoCrypto-C Core PoC Example

This example is the first customer-facing integration smoke for the AnoCrypto-C C API. It intentionally uses only installed public headers and the `AnoCryptoC::AnoCryptoC` CMake target.

## What it checks

- `find_package(AnoCryptoC CONFIG REQUIRED)` integration
- version and build identity queries
- module initialization and shutdown
- SHA-256 digest flow
- HMAC-SHA-256 flow with an in-memory HMAC key handle
- HMAC_DRBG-SHA-256 instantiate/generate/uninstantiate flow using deterministic public demo input

The example does not print raw key material, entropy input, nonce input, DRBG state, or generated DRBG bytes.

## Build from an installed package

```sh
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/path/to/anocrypto-c-prefix
cmake --build build-core-poc
./build-core-poc/anocrypto_c_core_poc
```

On Windows, the executable may be under a configuration subdirectory such as `Debug` or `Release` depending on the generator.

## Build from the source tree

When `ANO_CRYPTO_BUILD_EXAMPLES=ON`, the top-level build adds this directory and links against the build-tree target.

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/core_poc/anocrypto_c_core_poc
```

## Scope limits

This example is a PoC integration check. It is not official validation evidence, not a performance benchmark, and not a complete application security pattern.
