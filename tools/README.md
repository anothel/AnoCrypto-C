# anocrypto-c Tools

This directory contains engineering helper scripts for the private C99 codebase.

## `collect_release_evidence.cmake`

Creates a local release-evidence draft from a configured build tree:

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cmake --build build --target ano_crypto_collect_release_evidence
```

Or run the script directly:

```sh
cmake \
  -DANO_CRYPTO_SOURCE_DIR=. \
  -DANO_CRYPTO_BUILD_DIR=build \
  -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-20 \
  -DANO_CRYPTO_RUN_CTEST=ON \
  -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
  -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
  -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON \
  -DANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON \
  -P tools/collect_release_evidence.cmake
```

P1-20 output includes source-file hashes, a source archive SHA-256, binary hashes, CTest output, public-surface check output, thread-safety policy check output, reproducible-build notes, scope-review notes, SBOM/signing placeholder status, an evidence manifest, install-prefix smoke logs, and optional customer PoC package smoke logs, random-service implementation checks, entropy/health policy checks, and entropy/health review notes, entropy operational policy checks, and entropy operational review notes.

## `check_public_surface.cmake`

Runs local policy checks for public headers, package identity, retired naming, premature assurance wording, C++ dependency tokens, and Linux exported symbols when a shared object is present.

```sh
cmake --build build --target ano_crypto_public_surface_check
```

## `check_thread_safety_policy.cmake`

Runs local policy checks for the P1-16 thread-safety support matrix, CI checklist, lock source branches, CMake option, concurrent-test guard, evidence hook, and PoC package note.

```sh
cmake --build build --target ano_crypto_thread_safety_policy_check
```

## `create_poc_package.cmake`

Stages a customer PoC package v0 from a configured build tree.

```sh
cmake --build build --target ano_crypto_create_poc_package
```

The generated package contains a separately rebuilt clean SDK prefix, the public `examples/core_poc` integration example, selected docs, package manifest/review records, SBOM/signing placeholder status files, thread-safety/SHA-2/random-service notes, entropy/health boundary notes, and entropy operational policy notes.

## `run_poc_package_smoke.cmake`

Creates the PoC package, configures the packaged example as a standalone consumer with `find_package(AnoCryptoC)`, builds it, and runs it.

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The generated directories are engineering drafts only. Review them before sharing because they can contain local paths, compiler identifiers, and test logs.


## `ano_crypto_sha2_scope_policy_check`

Runs local policy checks for the P1-16 SHA-384/SHA-512 reserved scope decision, KAT plan, reserved-API test, KAT manifest status, evidence hook, and PoC package note.


## `check_random_service_policy.cmake`

Runs local policy checks for the platform entropy-backed HMAC_DRBG random service, source-backed context ownership, process-change reseed behavior, failure output clearing, provider-disabled behavior, evidence hook, and PoC package note.

```sh
cmake --build build --target ano_crypto_random_service_policy_check
```

## `check_entropy_health_policy.cmake`

Runs local policy checks for the P1-17 entropy failure-injection and health-test boundary, including test-hook defaults/guards, internal-only placement, negative/error-state tests, dedicated CI lane, evidence outputs, and clean PoC build settings.

```sh
cmake --build build --target ano_crypto_entropy_health_policy_check
```

The script writes `policy-check/ENTROPY_HEALTH_CHECK.txt`. It is an engineering guardrail and does not record entropy bytes or establish a finalized health-test policy.

## `check_entropy_operational_policy.cmake`

Runs local policy checks for source classification, external OS-provider health ownership, status-only operational probing, raw-noise health-test placement, source-backed process-change reseed behavior, evidence outputs, and clean public/PoC boundaries.

```sh
cmake --build build --target ano_crypto_entropy_operational_policy_check
```

The script writes `policy-check/ENTROPY_OPERATIONAL_POLICY_CHECK.txt`. It is an engineering guardrail and does not record provider output or establish entropy quality.


## P1-20 random lifecycle and export tools

- `check_random_lifecycle_policy.cmake` verifies lifecycle tests, fail-closed process-marker policy, VM guidance, private hook boundaries, and ABI-baseline inputs.
- `check_exported_symbols.cmake` compares one shared library with `abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt`.
- `check_clean_release_symbols.cmake` creates a nested tests-off/hooks-off hidden-symbol Release build and runs the exported-symbol comparison.
- `collect_release_evidence.cmake` stores lifecycle and clean-export reports while excluding the nested temporary build from the final evidence manifest.
- `create_poc_package.cmake` runs the clean export comparison before installing the PoC SDK and includes only status/policy records.
