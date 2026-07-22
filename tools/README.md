# anocrypto-c Tools

This directory contains engineering helper scripts for the C99 codebase.

## P2-05 evidence preparation and assembly

Creates a local P2-05 release-evidence draft while retaining the reviewed P1-33 release-engineering boundary and the P2-02/P2-03/P2-04 candidate policies:

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
  -DANO_CRYPTO_EVIDENCE_DIR=build/prepared-release-evidence/p2-09 \
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
  -DANO_CRYPTO_RUN_VALIDATION_CANDIDATE_CHECK=ON \
  -DANO_CRYPTO_RUN_SELF_TEST_INTEGRITY_CHECK=ON \
  -DANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON \
  -P tools/collect_release_evidence.cmake
```

The final assembly step consumes only that hash-bound tree:

```sh
cmake \
  -DANO_CRYPTO_BUILD_DIR=build \
  -DANO_CRYPTO_PREPARED_EVIDENCE_DIR=build/prepared-release-evidence/p2-09 \
  -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p2-09 \
  -P tools/assemble_release_evidence.cmake
```

P2-05 output retains the P1-33 build/test/package records, P2-02 boundary policy, P2-03 self-test/integrity policy, P2-04 entropy/DRBG policy, and P2-05 CSP/zeroization policy and compiler capture and copies verified `release-provenance/` plus key-free `release-signing-handoff/`. The source inventory does not perform dependency discovery, vulnerability analysis, or license conclusions. Current state remains `UNSIGNED`, hosted review `UNREVIEWED`, handoff `BLOCKED`, external authorization `NO`. `SENSITIVE_ARTIFACT_SCAN.txt` checks evidence, installed SDK, and PoC package for prohibited credential/signature artifacts.

## `check_validation_candidate_policy.cmake`

Checks the P2-02 candidate identifier, shared-library boundary, selected Ubuntu/GCC/CMake configuration, disabled test-only features, explicit hosted blocker, and the unchanged 14-header/69-symbol `ANOCRYPTO_C_0` public boundary.

```sh
cmake --build build --target ano_crypto_validation_candidate_policy_check
```

This static check does not prove execution on OE-01. The optional configure-time gate `-DANO_CRYPTO_ENFORCE_VALIDATION_CANDIDATE=ON` must be used on the exact selected environment and fails closed on environment or option drift.

## `check_self_test_integrity_policy.cmake`

Checks P2-03 policy IDs, the selected-but-unimplemented HMAC-SHA-256 single-shared-object integrity design, finite-state/recovery records, current startup KAT/state code, 14-header/69-symbol ABI guards, and unchanged hosted/release blockers.

```sh
cmake --build build --target ano_crypto_self_test_integrity_policy_check
```

The output `SELF_TEST_INTEGRITY_POLICY_CHECK.txt` is a claims-boundary and consistency record. It is not runtime module-integrity evidence.

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


## P2-02 retained release compatibility, rehearsal, and hosted artifact tools

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

## P2-02 retained release provenance and offline-signing handoff tools

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

## P2-04 entropy/DRBG policy check

```sh
cmake -DANO_CRYPTO_SOURCE_DIR=. -DANO_CRYPTO_OUTPUT_FILE=build/ENTROPY_DRBG_POLICY_CHECK.txt -P tools/check_entropy_drbg_policy.cmake
```

The check binds `EDP-P2-04-R1`, `ENT-OE01-LINUX-GETRANDOM-R1`, the frozen implementation limits, 14/69/`ANOCRYPTO_C_0`, and the unchanged hosted/release blocker state. It is engineering evidence only.


## P2-05 CSP/zeroization policy and compiler review

```sh
cmake --build build --target ano_crypto_csp_zeroization_policy_check
cmake --build build --target ano_crypto_zeroization_compiler_review
ctest --test-dir build -R "csp_(shutdown|fatal|zeroization)" --output-on-failure
```

`check_csp_zeroization_policy.cmake` binds `CZP-P2-05-R1`, current CSP ownership/lifecycle documentation, module-owned shutdown/fatal cleanup, 14 headers, 69 symbols, `ANOCRYPTO_C_0`, and unchanged hosted/signing/release blockers. `capture_zeroization_compiler_review.cmake` records compiler/target/build/library-hash metadata and disassembly markers for local human review. It does not establish exact OE-01 execution, physical memory remanence elimination, optimizer universality, or formal validation.

## P2-07 submission skeleton

- `check_submission_document_policy.cmake`: validates the controlled document set, staging allow-list, ABI and retained blocker/claim states.
- `create_submission_skeleton_package.cmake`: stages only allow-listed S0 documents and writes a SHA-256 manifest/status record.
- `check_submission_skeleton_package.cmake`: verifies the staged hashes, file types, count and non-authorization status.
- `run_submission_skeleton_package_smoke.cmake`: creates and verifies the internal skeleton package.


## P2-08 pre-consultation rehearsal

- `check_preconsultation_rehearsal_policy.cmake`: validates policy `PCR-P2-08-R1`, 10 Priority-1 agendas, 20 unresolved decisions, status/claim blockers, and unchanged 14/69/`ANOCRYPTO_C_0`.
- `create_preconsultation_rehearsal_package.cmake`: stages only nine allowlisted status-only documents and writes a SHA-256 manifest/status file.
- `check_preconsultation_rehearsal_package.cmake`: rejects source, binary, credential-like material, unexpected files, hash drift, external-consultation claims, or authorization drift.
- `run_preconsultation_rehearsal_package_smoke.cmake`: creates and verifies the internal rehearsal package.

## P2-09 cumulative closeout

- `check_p2_closeout_policy.cmake`: validates `P2C-P2-09-R1`, cumulative status/claims/exit controls, unchanged 14/69/`ANOCRYPTO_C_0`, and retained blocker/authorization states.
- `create_preconsultation_closeout_package.cmake`: stages the 17-document status-only closeout allowlist and writes a SHA-256 manifest/status record.
- `check_preconsultation_closeout_package.cmake`: verifies hashes, file count/type, non-disclosure, blocker states, and non-authorization.
- `run_preconsultation_closeout_package_smoke.cmake`: creates and checks the controlled internal closeout pack.

## P2-10 assisted pre-consultation rehearsal

- `check_live_rehearsal_policy.cmake`: validates `LAR-P2-10-R1`, ten rehearsed agenda groups, nine retained gaps, six institution-contact preconditions, unchanged 14/69/`ANOCRYPTO_C_0`, and retained hosted/signing/authorization blockers.
- `create_live_rehearsal_package.cmake`: stages only the 13-document status-only assisted-rehearsal allowlist and writes a SHA-256 manifest/status record.
- `check_live_rehearsal_package.cmake`: verifies hashes, file count/type, non-disclosure, independent-human `NOT_RUN`, external-consultation `NOT_CONDUCTED`, and conditional no-go contact status.
- `run_live_rehearsal_package_smoke.cmake`: creates and verifies the assisted-rehearsal package.

The assisted rehearsal is not an independent human review or institution consultation. It authorizes neither implementation nor external contact.

## P2-11 independent human rehearsal handoff

- `check_human_rehearsal_handoff_policy.cmake`: validates `HRH-P2-11-R1`, 17 controlled documents, ten agenda rows, reviewer/authorization pending states, unchanged 14/69/`ANOCRYPTO_C_0`, and retained hosted/signing blockers.
- `create_human_rehearsal_handoff_package.cmake`: stages the status-only reviewer handoff and writes a SHA-256 manifest/status record.
- `check_human_rehearsal_handoff_package.cmake`: verifies allowlisted files, hashes, status markers, and source/binary/credential exclusions.
- `run_human_rehearsal_handoff_package_smoke.cmake`: creates and checks the package without assigning a reviewer or performing a live rehearsal.
\n## P2-12 consultation contact readiness

- `check_consultation_contact_readiness_policy.cmake`: validates `CCR-P2-12-R1`, dated public-source controls, five unselected institution candidates, pending human/owner/contact states, unchanged 14/69/`ANOCRYPTO_C_0`, and retained hosted/signing blockers.
- `create_consultation_contact_readiness_package.cmake`: stages the controlled 15-document status-only contact-readiness pack.
- `check_consultation_contact_readiness_package.cmake`: verifies hashes, file count, pending states, and prohibited artifact/sensitive-marker exclusions.
- `run_consultation_contact_readiness_package_smoke.cmake`: creates and validates the pack in one test.

## P3-00 downstream requirement intake

P3-00 established the fail-closed intake gate and is retained as historical design documentation. Its active package/policy tools were superseded by P3-01 after all four exact source sets were imported.

## P3-01 product-driven requirement analysis

- `check_downstream_requirement_analysis_policy.cmake`: validates four imported source sets, one recorded identity conflict, 20 accepted requirement rows, eight approved AnoCrypto-C gaps, selected P3-02 HKDF-SHA-256 work, the frozen P3-01 14/69/`ANOCRYPTO_C_0` baseline, and retained hosted/signing blockers.
- `create_downstream_requirement_analysis_package.cmake`: stages the 14 allowlisted status-only analysis documents and writes a SHA-256 manifest/status record.
- `check_downstream_requirement_analysis_package.cmake`: verifies the status record, document hashes, file count and source/binary exclusion.
- `run_downstream_requirement_analysis_package_smoke.cmake`: creates and validates the P3-01 package in one test.

## P3-02 HKDF-SHA-256 additive API/ABI

- `check_hkdf_sha256_policy.cmake`: preserves the frozen P3-02 one-function C99/RFC 5869/`SKF1` evidence while also confirming that its ABI-0/1 symbols remain intact in the current 18-header/85-symbol P3-10 surface and external blockers remain unchanged.
- `check_symbol_versions.cmake`: verifies ELF default symbol versions for every retained ABI-0 name and the new HKDF ABI-1 name.
- `load_public_surface_state.cmake`: lets retained P1/P2/P3-01 policy checks preserve their frozen snapshots while current checks validate the P3-02 additive surface.

## P3-03 AES-256-GCM one-shot additive API/ABI

- `check_aes256_gcm_policy.cmake`: validates the two-function C99 API, fixed AES-256/12-byte nonce/16-byte tag profile, authenticate-before-release and failure-clearing rules, exact AnoSecureKit v1 compatibility fixtures, retained ABI-0/1 lines, two-symbol `ANOCRYPTO_C_2` line, current 18-header/85-symbol P3-10 identity, and unchanged external blockers.
- `check_symbol_versions.cmake`: verifies ELF default symbol versions for all retained ABI-0/1 names and the two new ABI-2 names.
- `load_public_surface_state.cmake`: preserves frozen P1/P2/P3-01 and P3-02 snapshots while current checks validate the P3-05 additive surface.


## P3-04 AES-256-GCM streaming additive API/ABI

- `check_aes256_gcm_streaming_policy.cmake`: validates the seven-function opaque-context API, explicit state transitions, unverified pre-final decrypt-output policy, exact SKT1 primitive compatibility, retained ABI-0/1/2 lines, seven-symbol `ANOCRYPTO_C_3` line, current 18-header/85-symbol identity, and unchanged blockers.
- `load_public_surface_state.cmake`: validates the retained `ANOCRYPTO_C_3` snapshot within the current `ANOCRYPTO_C_7` surface.

## P3-05 fixed-profile scrypt additive API/ABI

- `check_scrypt_policy.cmake`: validates the one-function fixed-profile API, caller-owned bounded workspace, zeroization rules, exact SKP1 primitive evidence, one-symbol `ANOCRYPTO_C_4`, current 18-header/85-symbol identity, and unchanged blockers.
- `load_public_surface_state.cmake`: validates retained ABI-0/1/2/3 lines and the current `ANOCRYPTO_C_7` surface.
