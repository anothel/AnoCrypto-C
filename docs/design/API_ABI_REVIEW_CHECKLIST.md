# AnoCrypto-C API/ABI Review Checklist

Status: P1-12 draft.

## Review scope

This checklist covers the public C headers under `include/anocrypto/`, the CMake installed package, the exported public symbol surface, and the installed-package PoC consumer for the current engineering slice.

## Naming and package identity

| Item | Expected | P1-12 status |
|---|---|---|
| Product spelling | `AnoCrypto-C` | OK |
| Repository/archive slug | `anocrypto-c` | OK |
| CMake package | `AnoCryptoC` | OK |
| Imported target | `AnoCryptoC::AnoCryptoC` | OK |
| Library target/file | `anocrypto_c` | OK |
| Include root | `include/anocrypto/` | OK |
| C API prefix | `ano_crypto_` | OK |
| Macro prefix | `ANO_CRYPTO_` | OK |

## Header ABI checklist

| Rule | P1-12 status | Notes |
|---|---|---|
| Public cryptographic contexts do not expose internal struct layout. | OK | Hash/HMAC/DRBG contexts use caller-provided opaque buffers and size APIs. |
| Public enum values are explicit. | OK | Current public enums use fixed numeric values. |
| Processing APIs return `ano_crypto_status_t`. | OK | Name/status query helpers are documented exceptions. |
| Outputs are returned through output parameters. | OK | Digest/MAC/DRBG APIs use output buffers and written lengths. |
| Context size APIs exist where caller-provided storage is required. | OK | Hash, HMAC, and DRBG expose context-size queries. |
| Buffer length errors are explicit. | OK | Negative tests cover representative short-buffer and invalid-argument cases. |
| Public context mutation is not claimed thread-safe. | OK | Current support is limited to selected global state. |
| Public build identity struct layout is frozen. | TODO | `ano_crypto_build_identity_t` is the only public struct layout; consider field getters before ABI freeze. |
| Symbol visibility policy is finalized. | Partial | `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS` hides internal symbols for no-test release-style builds on supported ELF/Mach-O compilers; internal test-hook builds may still expose internal symbols for internal-unit tests. |
| Static library support policy is finalized. | TODO | Shared-library boundary remains the first candidate boundary. |

## Per-header review notes

| Header | P1-12 review note |
|---|---|
| `anocrypto.h` | Umbrella header only; no extra ABI. |
| `ac_status.h` | Status enum values are explicit and append-only before internal error sentinel. |
| `ac_version.h` | Version queries are state-independent. |
| `ac_build.h` | Public struct layout is tracked as an ABI TODO. |
| `ac_module.h` | Module config remains opaque; module state enum values are explicit. |
| `ac_selftest.h` | Test-only force flag is gated behind test-hook macro. |
| `ac_hash.h` | Opaque buffer context; SHA-384/SHA-512 remain placeholders. |
| `ac_hmac.h` | Opaque buffer context and in-memory HMAC key handle compute path. |
| `ac_drbg.h` | Opaque buffer context; explicit entropy input; public random-service API remains deferred. |
| `ac_csp.h` | Handle is integer-opaque to callers; state enum values are explicit. |
| `ac_key.h` | Import/destroy/state query only; export and persistence are deferred. |

## Required local checks

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_public_surface_check
cmake --install build --prefix /tmp/anocrypto-c-p1-12
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-12
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
cmake --build build --target ano_crypto_poc_package_smoke
```

On Linux, inspect exported symbols with:

```sh
nm -D --defined-only build/libanocrypto_c.so | awk '{print $3}' | sort
```

Expected exported public names should use the `ano_crypto_` prefix. Internal `ano_crypto_internal_*` symbols should be hidden in release-style builds with `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON`; test-hook builds may report them as tracked warnings.

## Before public ABI freeze

1. Decide whether to replace `ano_crypto_build_identity_t` with field-by-field getters.
2. Decide whether to add a stricter version script/export map per platform.
3. Decide whether static libraries are supported and what ABI commitments apply.
4. Add ABI baseline comparison after symbol policy is fixed.
5. Keep unsupported algorithms as explicit placeholder returns until implementation and evidence are reviewed.
