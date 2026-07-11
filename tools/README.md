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
  -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-18 \
  -DANO_CRYPTO_RUN_CTEST=ON \
  -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
  -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
  -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON \
  -DANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON \
  -P tools/collect_release_evidence.cmake
```

P1-18 output includes source-file hashes, a source archive SHA-256, binary hashes, CTest output, public-surface check output, thread-safety policy check output, reproducible-build notes, scope-review notes, SBOM/signing placeholder status, an evidence manifest, install-prefix smoke logs, and optional customer PoC package smoke logs, random-service boundary checks, entropy/health policy checks, and entropy/health review notes, entropy operational policy checks, and entropy operational review notes.

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

The generated package contains a separately rebuilt clean SDK prefix, the public `examples/core_poc` integration example, selected docs, package manifest/review records, SBOM/signing placeholder status files, thread-safety/SHA-2/random-boundary notes, entropy/health boundary notes, and entropy operational policy notes.

## `run_poc_package_smoke.cmake`

Creates the PoC package, configures the packaged example as a standalone consumer with `find_package(AnoCryptoC)`, builds it, and runs it.

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The generated directories are engineering drafts only. Review them before sharing because they can contain local paths, compiler identifiers, and test logs.


## `ano_crypto_sha2_scope_policy_check`

Runs local policy checks for the P1-16 SHA-384/SHA-512 reserved scope decision, KAT plan, reserved-API test, KAT manifest status, evidence hook, and PoC package note.


## `check_random_boundary_policy.cmake`

Runs local policy checks for the P1-16 reserved public random-service boundary, fail-closed placeholders, DRBG/entropy boundary docs, evidence hook, and PoC package note.

```sh
cmake --build build --target ano_crypto_random_boundary_policy_check
```

## `check_entropy_health_policy.cmake`

Runs local policy checks for the P1-17 entropy failure-injection and health-test boundary, including test-hook defaults/guards, internal-only placement, negative/error-state tests, dedicated CI lane, evidence outputs, and clean PoC build settings.

```sh
cmake --build build --target ano_crypto_entropy_health_policy_check
```

The script writes `policy-check/ENTROPY_HEALTH_CHECK.txt`. It is an engineering guardrail and does not record entropy bytes or establish a finalized health-test policy.

## `check_entropy_operational_policy.cmake`

Runs local policy checks for P1-18 source classification, external OS-provider health ownership, status-only operational probing, raw-noise health-test placement, source-backed process-change reseed behavior, evidence outputs, and clean public/PoC boundaries.

```sh
cmake --build build --target ano_crypto_entropy_operational_policy_check
```

The script writes `policy-check/ENTROPY_OPERATIONAL_POLICY_CHECK.txt`. It is an engineering guardrail and does not record provider output or establish entropy quality.
