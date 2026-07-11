# AnoCrypto-C API/ABI Review Checklist

Status: P1-19 draft.

## Review scope

This checklist covers the public C headers under `include/anocrypto/`, the CMake installed package, the exported public symbol surface, and the installed-package PoC consumer for the current engineering slice.

## Naming and package identity

| Item | Expected | P1-16 status |
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

| Rule | P1-16 status | Notes |
|---|---|---|
| Public cryptographic contexts do not expose internal struct layout. | OK | Hash/HMAC/DRBG contexts use caller-provided opaque buffers and size APIs. |
| Public enum values are explicit. | OK | Current public enums use fixed numeric values. |
| Processing APIs return `ano_crypto_status_t`. | OK | Name/status query helpers are documented exceptions. |
| Outputs are returned through output parameters. | OK | Digest/MAC/DRBG APIs use output buffers and written lengths. |
| Context size APIs exist where caller-provided storage is required. | OK | Hash, HMAC, DRBG, and the P1-19 random service expose implemented context-size queries; random returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` only when no reviewed platform provider is compiled in. |
| Buffer length errors are explicit. | OK | Negative tests cover representative short-buffer and invalid-argument cases. |
| Public context mutation is not claimed thread-safe. | OK | Current support is limited to selected global state. |
| Public build identity struct layout is frozen. | TODO | `ano_crypto_build_identity_t` is the only public struct layout; consider field getters before ABI freeze. |
| Symbol visibility policy is finalized. | Partial | `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS` hides internal symbols for no-test release-style builds on supported ELF/Mach-O compilers; internal test-hook builds may still expose internal symbols for internal-unit tests. |
| Static library support policy is finalized. | TODO | Shared-library boundary remains the first candidate boundary. |

## Per-header review notes

| Header | P1-16 review note |
|---|---|
| `anocrypto.h` | Umbrella header only; no extra ABI. |
| `ac_status.h` | Status enum values are explicit and append-only before internal error sentinel. |
| `ac_version.h` | Version queries are state-independent. |
| `ac_build.h` | Public struct layout is tracked as an ABI TODO. |
| `ac_module.h` | Module config remains opaque; module state enum values are explicit. |
| `ac_selftest.h` | Test-only force flag is gated behind test-hook macro. |
| `ac_hash.h` | Opaque buffer context; SHA-384/SHA-512 remain placeholders. |
| `ac_hmac.h` | Opaque buffer context and in-memory HMAC key handle compute path. |
| `ac_drbg.h` | Opaque buffer context; explicit entropy input; implemented public random generation remains separate. |
| `ac_random.h` | P1-19 implemented source-backed HMAC_DRBG-SHA-256 service; caller-owned opaque context, bounded request, failure output clearing, and explicit unsupported flags. |
| `ac_csp.h` | Handle is integer-opaque to callers; state enum values are explicit. |
| `ac_key.h` | Import/destroy/state query only; export and persistence are deferred. |

## Required local checks

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_public_surface_check
cmake --build build --target ano_crypto_thread_safety_policy_check
cmake --build build --target ano_crypto_sha2_scope_policy_check
cmake --build build --target ano_crypto_random_service_policy_check
cmake --build build --target ano_crypto_entropy_health_policy_check
cmake --install build --prefix /tmp/anocrypto-c-p1-19
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-19
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

## P1-14 thread-safety ABI note

The P1-14 support matrix does not add public symbols or public struct layouts. It documents current behavior for existing APIs and keeps caller-owned context serialization outside the library ABI.

## P1-16 random-service ABI note

P1-19 implements the existing `ac_random.h` symbols without adding a public function. Context ownership remains caller-provided opaque storage; source-backed context marking prevents explicit DRBG contexts from being accepted by the random service. Prediction resistance and VM snapshot/restore detection remain unsupported. The public symbol-name baseline must remain unchanged from P1-18.

## P1-17 entropy testability ABI note

- Entropy provider hooks and health-test state remain under `src/internal/` and are not installed.
- `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS` is `OFF` by default and cannot be enabled with `ANO_CRYPTO_BUILD_TESTS=OFF`.
- No public status-enum value or public struct layout was added for the health-test skeleton; internal health failure maps to `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`.
- PoC staging rebuilds with tests/hooks off and internal symbol hiding on, then scans public headers and shared-library exports for internal/test-hook leakage.
- Windows shared-library auto-export is allowed only in internal test builds so existing internal-unit tests can link; it is not enabled in the tests-off Release/PoC configuration.

## P1-19 entropy operational ABI note

- Source class, health owner, restart policy, operational probe, and process guard remain private under `src/internal/` or internal source files.
- No public function, enum value, installed struct, or public macro was added for P1-19.
- The private DRBG context size increased; callers are required to query `ano_crypto_drbg_context_size()` for each build and must not persist or hard-code the size.
- Clean Release/PoC builds keep internal symbol hiding enabled and must not export `ano_crypto_internal_*`, entropy hook, or process-guard names.
- Compare the P1-17 and P1-19 exported public symbol-name lists; any difference requires review before handoff.

## P1-19 random-service ABI note

- No new exported public function is added.
- `ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE` documents the per-call ceiling.
- Private context layout may change before ABI freeze; callers must query size for each build.
- Installed headers expose no provider callback, entropy source class, health-test state, or process-guard type.
- Random-service contexts are mutable caller-owned storage and require external synchronization.
