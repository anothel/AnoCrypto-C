# anocrypto-c Tools

This directory contains engineering helper scripts for the C99 codebase.

## P1-33 evidence preparation and assembly

Creates a local P1-33 release-evidence draft from prepared prerequisite outputs:

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cmake --build build --target ano_crypto_collect_release_evidence
```

The target serializes clean Release symbol/runtime-dependency inspection, install smoke, PoC smoke, tagged compatibility, compiler review, full CTest, provenance preparation, signing-handoff verification, and status-log preparation. `assemble_release_evidence.cmake` then verifies the complete evidence manifest and copies only the prepared tree. The final collector launches no configure, build, CTest, package, policy, or status-test subprocess.

`collect_release_evidence.cmake` is the internal status-preparation script. Direct use requires those outputs already exist and should write a prepared directory:

```sh
cmake \
  -DANO_CRYPTO_SOURCE_DIR=. \
  -DANO_CRYPTO_BUILD_DIR=build \
  -DANO_CRYPTO_EVIDENCE_DIR=build/prepared-release-evidence/p1-33 \
  -DANO_CRYPTO_PREPARED_PROVENANCE_DIR=build/release-provenance-evidence \
  -DANO_CRYPTO_PREPARED_SIGNING_HANDOFF_DIR=build/release-signing-handoff \
  -DANO_CRYPTO_PREPARED_SIGNING_HANDOFF_CHECK=build/release-signing-handoff-check.txt \
  -DANO_CRYPTO_BUILD_CONFIG=Release \
  -DANO_CRYPTO_MULTI_CONFIG_BUILD=OFF \
  -DANO_CRYPTO_OPENSSL_DIFFERENTIAL_AVAILABLE=ON \
  -DANO_CRYPTO_RUN_CTEST=OFF \
  -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
  -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
  -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON \
  -DANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON \
  -P tools/collect_release_evidence.cmake
```

The final assembly step consumes only that hash-bound tree:

```sh
cmake \
  -DANO_CRYPTO_BUILD_DIR=build \
  -DANO_CRYPTO_PREPARED_EVIDENCE_DIR=build/prepared-release-evidence/p1-33 \
  -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-33 \
  -P tools/assemble_release_evidence.cmake
```

P1-33 output retains prior build/test/package records and copies verified `release-provenance/` plus key-free `release-signing-handoff/`. The source inventory does not perform dependency discovery, vulnerability analysis, or license conclusions. Current state remains `UNSIGNED`, hosted review `UNREVIEWED`, handoff `BLOCKED`, external authorization `NO`. `SENSITIVE_ARTIFACT_SCAN.txt` checks evidence, installed SDK, and PoC package for prohibited credential/signature artifacts.

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

The generated package contains a separately rebuilt clean SDK prefix, the public `examples/core_poc` integration example, selected docs, package manifest/review records, package-specific SBOM-not-generated and unsigned signing status files, thread-safety/SHA-512-family/random-service notes, entropy/health boundary notes, and entropy operational policy notes.

## `run_poc_package_smoke.cmake`

Creates the PoC package, configures the packaged example as a standalone consumer with `find_package(AnoCryptoC)`, builds it, and runs it.

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The generated directories are engineering drafts only. Review them before sharing because they can contain local paths, compiler identifiers, and test logs.


## `ano_crypto_sha512_family_policy_check`

Runs local policy checks for the SHA-384/SHA-512 and HMAC family implementation, startup/extended KATs, negative API tests, KAT manifest, evidence hook, and PoC package note.

```sh
cmake --build build --target ano_crypto_sha512_family_policy_check
```


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


## P1-21 concurrency and sanitizer tools

- `check_concurrency_sanitizer_policy.cmake` verifies the Win32/pthread concurrency test, thread-platform artifact, sanitizer mode, GCC hosted lane, PoC boundary, and build-identity ABI-0 decision.
- `ano_crypto_thread_platform_review` records backend/availability/test-presence/sanitizer-mode metadata only.
- `ANO_CRYPTO_SANITIZER_MODE=address-undefined` instruments supported GCC/Clang-family diagnostic builds; the required CI lane is Linux GCC.
- Actual hosted artifacts must be reviewed separately before platform support wording changes.


### `check_differential_portability_policy.cmake`

Checks P1-23 OpenSSL 3.x test-only provider selection, deterministic SHA-2/HMAC differential coverage, C99 representation/endian/alignment portability tests, hosted differential/32-bit workflow wiring, evidence records, and PoC product-boundary exclusions.

The evidence collector writes `DIFFERENTIAL_PORTABILITY_CHECK.txt`, `DIFFERENTIAL_VALIDATION.log`, `SHA512_PORTABILITY_REVIEW.log`, and `DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt`. Logs must not contain digest, MAC, key, message, entropy, seed, DRBG state, or random values.


## P1-29 AES-CMAC hardening tools

- `check_cmac_policy.cmake` retains the P1-28 AES-CMAC implementation, API,
  KAT, dedicated CSP-purpose, differential-provider, package, and 69-symbol
  baseline checks under the P1-29 version identity.
- `check_cmac_hardening_policy.cmake` verifies private state guards, overlap
  rules, bounded cleanup, lifecycle stress, concurrency, the 1,728-comparison
  differential corpus, compiler-review wiring, evidence records, and unchanged
  public ABI.
- `capture_cmac_compiler_review.cmake` records compiler, target, library hash,
  symbol markers, aggregate branch/call counts, and disassembly for manual
  target-specific review. It does not make an automated timing or side-channel
  conclusion.

```sh
cmake --build build --target ano_crypto_cmac_policy_check
cmake --build build --target ano_crypto_cmac_hardening_policy_check
cmake --build build --target ano_crypto_cmac_compiler_review
```

CMAC compiler disassembly is internal engineering evidence and is excluded from
the customer PoC package. Status logs must not print key, message, tag, subkey,
AES schedule, or chaining-state values.


## P1-33 release compatibility, rehearsal, and hosted artifact tools

- `check_release_compatibility.cmake` validates central release identity, 14 baseline/current canonical-LF public-header hashes, normalized public declarations/types/constants, and the unchanged 69-symbol ABI line.
- `run_tagged_release_compatibility_smoke.cmake` builds a clean installed package and runs current-header, P1-29-header, exact-version, and incompatible-version consumers.
- `create_hosted_artifact_manifest.cmake` creates a text-only status bundle with release/lane/platform/compiler metadata, sizes, and SHA-256.
- `hosted_artifact_path_helpers.cmake` rejects hosted roots, files, and intermediate paths that resolve through symlinks or junctions.
- `check_hosted_artifact_manifest.cmake` rejects tampering, unlisted files, unsafe/aliased paths, binary/NUL content, and sensitive-value markers.
- `create_hosted_artifact_index.cmake` re-verifies downloaded manifests, checks required lane/platform coverage, and writes an index plus a human-review template.
- `tagged_release_rehearsal_tests.cmake` clones the current commit into an isolated repository, creates only a synthetic local tag, and proves clean identity plus dirty/moved-tag/moved-`HEAD` rejection.

```sh
cmake --build build --target ano_crypto_release_compatibility_policy_check
cmake --build build --target ano_crypto_tagged_release_compatibility_smoke
```

Expected tag metadata is not release provenance until a Git-backed check proves that the tag, supplied commit, and checked-out `HEAD` identify one commit.

## P1-33 release provenance and offline-signing handoff tools

- `create_source_sbom_spdx.cmake` writes a deterministic SPDX 2.3 JSON source inventory for the exact source-archive file list.
- `create_release_provenance_bundle.cmake` binds the source archive, source hashes, source SBOM, public-header manifest, ABI-0 symbol list, hosted disposition, unsigned signing status, fixed signing request, and blocked authorization record.
- `check_release_provenance_bundle.cmake` verifies exact hash coverage, safe paths, release identity, source-tree hashes, source-SBOM bindings, public counts, hosted disposition, and the explicit unsigned/external-release-NO boundary.
- `prepare_release_provenance_evidence.cmake` derives the tracked/unignored source list, creates and verifies provenance, and writes prepared artifacts consumed by final collection.
- `create_release_signing_handoff.cmake` creates a key-free, manifest-bound offline-signing request/status/authorization handoff from verified provenance and source-owned production-policy records.
- `check_release_signing_handoff.cmake` re-derives readiness, verifies hash/policy bindings, enforces role identity separation, and rejects private signing material.
- `check_release_provenance_policy.cmake` checks repository/CI/evidence/PoC/signing integration and rejects signature/key/token-like source files.
- `release_provenance_tests.cmake` covers provenance tampering, unlisted files, false signed/approved claims, unsafe paths, wrong tag metadata, and hosted-disposition completeness.
- `release_signing_handoff_tests.cmake` covers key-free generation, tamper rejection, unprovisioned readiness, and distinct Security Reviewer/Product Release Owner identity enforcement.
- `release_signing_fixture_tests.cmake` provides opt-in Debug-only ephemeral OpenSSL Ed25519/raw-64/SPKI-fingerprint/RFC-3161 positive and negative fixtures, then removes generated material.
- `sensitive_release_artifact_tests.cmake` injects prohibited suffixes, generic private-key armor, token assignments, and timestamp fixtures into synthetic source-archive/install/PoC/evidence roots and requires the shared scanner to fail closed.

```sh
cmake --build build --target ano_crypto_release_provenance_policy_check
ctest --test-dir build -R "ano_crypto_release_(provenance|signing_handoff)_tests" --output-on-failure
```

Offline profile: sign `RELEASE_ARTIFACTS.sha256` with Ed25519; store raw 64-byte `RELEASE_ARTIFACTS.sha256.ed25519`; identify approved public key by SHA-256 of DER SubjectPublicKeyInfo; obtain RFC 3161 timestamp over the signature file. GitHub attestation is optional secondary evidence only.

Production signer/TSA records and required role identities are `UNPROVISIONED`. Actual hosted review, tag authorization, production signature/timestamp generation and verification, Security Reviewer/Product Release Owner approval, legal/product authorization, and external distribution remain incomplete. No key, token, signature, timestamp request, or timestamp response belongs in source, evidence, installed SDK, or PoC package.
