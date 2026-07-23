# AnoCrypto-C

Current runtime baseline: `0.1.0-p3-10-restricted-sha1-ocsp-certid`, source commit `98f544a574f70b387c300f171243b4e87174937a`, with 18 public headers and 85 exported symbols across `ANOCRYPTO_C_0` through `ANOCRYPTO_C_7`. Engineering Phase 1 is closed after local AnoSecureKit and AnoPKI Enterprise parity/no-fallback checkpoints. Commercialization is `UNDECIDED`; customer/external release remains `BLOCKED`/`NOT_AUTHORIZED`; hosted qualification is `NOT_RUN/UNREVIEWED`; repository signing remains `UNSIGNED`; KCMVP remains `DEFERRED` and not validated. No next product implementation is selected.

Implemented processing scope:

- RFC 5869 HKDF-SHA-256 combined extract-and-expand with 0..8160-byte output and exact AnoSecureKit SKF1 info-byte evidence
- AES-256-GCM one-shot encrypt/decrypt using a 32-byte BLOCK_CIPHER CSP key, fixed 12-byte nonce, fixed 16-byte tag, empty AAD/payload support, exact in-place payload operation, and authenticated-output-only decryption
- AES-256-GCM streaming encrypt/decrypt with caller-owned opaque contexts, explicit AAD/payload/final states, arbitrary chunk boundaries, exact in-place processing, and unverified pre-final decrypt output requiring caller buffering and failure wiping
- Fixed-profile scrypt key derivation with `N=32768`, `r=8`, `p=1`, 32-byte output, caller-owned bounded workspace, complete workspace zeroization, and exact `SKP1` v1 primitive compatibility evidence
- NIST AES-256-GCM KATs, streaming state/chunk/finalization tests, corruption/overlap/output-clearing tests, exact SKT1/SKF1/SKP1 v1 primitive fixtures, and OpenSSL test-only differential coverage
- RSA-2048/e=65537 PKCS#1 v1.5 message signing and verification with SHA-256/384/512, algorithm-native component import, CRT exponent blinding, public fault check, and additive ABI-5
- ECDSA P-256 SHA-256 message signing and verification with fixed-width component import, deterministic RFC 6979 nonces, raw 64-byte `r || s` signatures, and additive ABI-6
- SHA-256, SHA-384, and SHA-512 one-shot and streaming APIs
- HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 one-shot and streaming APIs
- HMAC computation for all three SHA-2 variants using an in-memory HMAC key handle
- AES-128, AES-192, and AES-256 table-free portable primitive used through CSP-backed block-cipher keys
- AES-CBC without padding, caller-provided 16-byte IV, complete blocks only, exact in-place support, and partial-overlap rejection
- Startup and extended public-source AES/CBC KATs plus negative and OpenSSL test-only differential coverage
- P1-26 AES private-context integrity guards, fail-closed reinitialization, 128-iteration lifecycle stress, unaligned byte-buffer/streaming/in-place portability regression, and shared/per-thread CSP AES concurrency coverage
- P1-28 AES-CMAC with AES-128/192/256 dedicated key-purpose handles, full 16-byte tags, one-shot compute/verify, streaming compute, startup/extended KATs, negative tests, and OpenSSL test-only differential coverage
- P1-29 AES-CMAC private state guards, fail-closed reinitialization, bounded uninit and non-overlap rules, lifecycle stress, shared-handle concurrency, 1,728 comparison differential corpus, and manual compiler-output review capture
- P1-30 release identity, P1-29 public-header snapshot, normalized source-surface comparison, old-header/current-library runtime compatibility, installed-package compatibility smoke, and hashed hosted-artifact bundle manifests
- P1-31 unsigned release provenance, deterministic SPDX 2.3 JSON source inventory, source/archive/public-header/export hash binding, signing-request placeholder, authorization TODO record, hosted provenance lane, and PoC exclusion checks
- P1-32 key-free offline signing handoff, Ed25519/RFC 3161 verifier policy, role-separation checks, opt-in ephemeral Debug fixtures, and fail-closed production-provisioning status
- P1-33 clean-checkout synthetic tag rehearsal, tag/commit/HEAD and clean-worktree checks, hosted/synthetic provenance separation, required lane review records, and fail-closed negative regressions
- AES Release compiler-output capture with compiler/target/library metadata and a manual-review-only interpretation boundary
- HMAC_DRBG-SHA-256 deterministic core API
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- Public `ano_crypto_random_*` lifecycle backed by the reviewed platform entropy seam and HMAC_DRBG-SHA-256
- Bidirectional cross-API context separation between explicit-entropy DRBG and random-service generate/reseed operations
- Process-change detection for source-backed random contexts, requiring explicit reseed before generation
- Startup self-test KATs for SHA-256/SHA-384/SHA-512, their HMAC variants, and HMAC_DRBG-SHA-256
- Extended KAT executables for all implemented SHA-2/HMAC variants, HMAC key-handle paths, and HMAC_DRBG-SHA-256 public vectors
- Deterministic OpenSSL 3.x test-only differential comparisons for SHA-256/384/512, HMAC, HKDF-SHA-256, fixed-profile scrypt, AES-256-GCM, AES-CBC, AES-CMAC, RSA, and P-256 ECDSA paths
- Restricted one-shot SHA-1 for OCSP CertID issuer Name DER and subjectPublicKey BIT STRING content only, with startup KAT, misuse-containment policy, and exact OpenSSL test-only OCSP comparison
- SHA-512-family representation, endian, variable-chunk, and intentionally unaligned-input portability regression
- CMake C99 representation guard requiring 8-bit bytes, 32-bit `uint32_t`, 64-bit `uint64_t`, and `size_t` no wider than `uint64_t`
- Negative API tests for hash/HMAC/DRBG/CSP/key argument handling, context lifecycle behavior, handle exhaustion, stale handle rejection, and DRBG helper failure cleanup
- Internal entropy adapter seam with callback and platform OS provider tests
- Dedicated-build deterministic entropy provider hook for success, failure, and partial-write negative paths; disabled by default and absent from public headers
- Internal startup/continuous duplicate-block health-test candidates with operation-local or module-fatal policy wiring
- Fatal health-test failure coverage for DRBG context zeroization and sticky module `ERROR_STATE` transition
- Internal source classification and health-test ownership metadata that keeps OS-provider output outside the raw-noise health skeleton
- Status-only OS-provider operational probe with temporary-buffer zeroization
- Private process identity guard for source-backed DRBG state; generation after process change requires reseed and clears output
- Release evidence draft collection with a bound source archive, source-file hashes, SPDX 2.3 JSON source inventory, an unsigned release-provenance bundle, public-surface check, reproducible-build notes, install/PoC smoke logs, verified key-free signing-handoff records, explicit unsigned/blocked status, and evidence manifest hashes
- Limited internal locking for module/self-test/CSP global state
- Thread-safety support matrix, cross-platform Win32/pthread CSP/HMAC test, status-only platform review, CI checklist, and policy-check targets
- CI matrix skeleton for Linux, macOS, and Windows build/test/install-smoke paths
- API/ABI review checklist, stored 69-symbol baseline, and build-identity ABI-0 field-order baseline
- Release-style internal symbol hiding option for supported compilers
- P2-06 secure-coding policy `SCP-P2-06-R1`, controlled findings register, compiler/CPU review matrix, and status-only operational review
- Linux GNU/Clang Release hardening overlay: FORTIFY 3, strong stack protector, stack-clash protection, full RELRO/BIND_NOW, and non-executable stack
- Local GCC/Clang warning/static-analysis review with the exact OE-01 GCC 13.3.0 human disposition kept separate and `NOT_RUN`
- Customer PoC package v0 staging, package manifest/review records, and standalone public-API example smoke
- Development workflow, service scope matrix, differential validation strategy, AES/CBC implementation policy, side-channel checklist, customer PoC intake questionnaire, SBOM/signing plan, and evidence manifest policy drafts

Skeleton-only / TODO scope:

- Hash_DRBG-SHA-256 ABI placeholder
- Actual module-byte integrity authentication, controlled integrity-key injection, binary finalization, and tamper evidence
- Direct noise-source selection, source-specific startup/continuous health tests, thresholds, and recovery policy
- Cross-platform hosted-CI review of process-change behavior and unsupported-process-marker policy
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Actual hosted Linux 32-bit, Windows, macOS, thread/random/differential artifacts, and any intended big-endian target; Clang sanitizer runtime and ThreadSanitizer policy review
- Byte-identical source archive and binary reproducibility rules
- Dependency/component discovery, vulnerability and license review, production signer/key-custody provisioning, RFC 3161 service/trust-anchor provisioning, actual release signing/timestamping, and external authorization
- PoC package legal/evaluation notice and customer-specific support boundary
- Exact OE-01 compiler/CPU and human disposition, approved statistical timing/cache methodology if required, power/EM/fault applicability, padding helpers, GMAC, additional scrypt profiles, and domestic-algorithm decisions


## P2-01 readiness consolidation

P2-01 introduces no cryptographic service, public function, public type, header, enum value, or exported symbol. Its controlled documents are:

- `docs/kcmvp/KCMVP_READINESS_GAP_P2.md`
- `docs/kcmvp/KCMVP_PRECONSULTATION_QUESTIONS.md`
- `docs/kcmvp/KCMVP_EVIDENCE_TRACEABILITY_MATRIX.md`
- `docs/kcmvp/KCMVP_SUBMISSION_DOCUMENT_INVENTORY.md`
- `docs/roadmap/P2_VALIDATION_READINESS_ROADMAP.md`
- `docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md`

The CMake package template filename is normalized to `cmake/AnoCryptoCConfig.cmake.in` so the repository matches its existing case-sensitive CMake and policy references. This packaging-source correction does not change public ABI.

## P2-02 boundary and operating-environment freeze

P2-02 freezes one internal candidate for design, test planning, and pre-consultation. It does not claim institution acceptance or actual hosted execution. Controlled outputs are:

- `docs/kcmvp/KCMVP_MODULE_BOUNDARY_SPEC.md`;
- `docs/kcmvp/KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`;
- `release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake`;
- `tools/check_validation_candidate_policy.cmake`;
- CMake option `ANO_CRYPTO_ENFORCE_VALIDATION_CANDIDATE=ON`;
- evidence record `VALIDATION_CANDIDATE_BOUNDARY_CHECK.txt`.

The selected candidate is shared-library only. Static linkage, Windows, macOS, non-x86-64 architectures, 32-bit, big-endian, musl, mobile/embedded environments, hardware dispatch, VM-clone guarantees, and suspend/resume detection are deferred or unsupported for the initial candidate.

## P2-03 self-test, integrity, state, and recovery design

P2-03 freezes internal design and evidence policy without implementing or claiming an accepted module-integrity mechanism. Controlled outputs are:

- `docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md`;
- `docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md`;
- `docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md`;
- `docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md`;
- `release/ANOCRYPTO_C_SELF_TEST_POLICY.cmake`;
- `tools/check_self_test_integrity_policy.cmake`;
- evidence record `SELF_TEST_INTEGRITY_POLICY_CHECK.txt`.

The selected candidate is `INT-HMAC-SHA256-SINGLE-SO-R1`, which preserves the P2-02 single-shared-object boundary. It requires explicit consultation/implementation gates, protected integrity-key handling, CSP inventory integration, tamper tests, selected-OE execution, and target-specific review. No integrity verifier, key, tag, or binary metadata record is included in this step. P2-04 froze entropy-provider, direct-source health-test, DRBG limit, process/VM/suspend, and failure-policy responsibility. P2-05 now freezes CSP ownership and zeroization evidence; the integrity verifier remains unimplemented.

## P2-10 assisted pre-consultation rehearsal

P2-10 adds a controlled, status-only assisted rehearsal layer without claiming an independent human or institution review. Controlled outputs are:

- `docs/kcmvp/KCMVP_INTERNAL_ASSISTED_REHEARSAL_REPORT.md`;
- `docs/kcmvp/KCMVP_ASSISTED_REHEARSAL_TRANSCRIPT.md`;
- `docs/kcmvp/KCMVP_REHEARSAL_RESPONSE_SCORECARD.md`;
- `docs/kcmvp/KCMVP_REHEARSAL_GAPS_AND_REMEDIATION.md`;
- `docs/kcmvp/KCMVP_EXTERNAL_CONSULTATION_DECISION_GATE.md`;
- `release/ANOCRYPTO_C_LIVE_REHEARSAL_POLICY.cmake`;
- `tools/check_live_rehearsal_policy.cmake`;
- status-only assisted rehearsal package smoke.

The assisted rehearsal covers ten Priority-1 agenda groups. It concludes that the pack is ready for a named human internal rehearsal. Institution contact remains `CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION`.


## P2-11 independent human rehearsal handoff

P2-11 does not pretend that an automated or document-only exercise is an independent human review. It freezes policy `HRH-P2-11-R1` and provides a status-only handoff package for a future named reviewer. Controlled outputs include:

- `docs/kcmvp/KCMVP_HUMAN_REHEARSAL_HANDOFF_PACK_INDEX.md`;
- `docs/kcmvp/KCMVP_INDEPENDENT_HUMAN_REHEARSAL_HANDOFF.md`;
- `docs/kcmvp/KCMVP_NAMED_REVIEWER_EXECUTION_GUIDE.md`;
- `docs/kcmvp/KCMVP_LIVE_REHEARSAL_MINUTES_TEMPLATE.md`;
- `docs/kcmvp/KCMVP_REHEARSAL_OBJECTION_AND_DEFECT_REGISTER.md`;
- `docs/kcmvp/KCMVP_CONSULTATION_AUTHORIZATION_RECORD.md`;
- policy and 17-document handoff-package smoke checks.

The reviewer remains `UNASSIGNED`, the live rehearsal remains `NOT_RUN`, and institution contact remains unauthorized. A completed human rehearsal and separate project-owner decision are required before the consultation gate can change.

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p2-11
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p2-11
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## Core PoC example

Build the example from the build tree:

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/core_poc/anocrypto_c_core_poc
```

Build the example as an installed-package consumer:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p2-11
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p2-11
cmake --build build-core-poc
./build-core-poc/anocrypto_c_core_poc
```

The example prints status-only success messages. It does not print raw key material, entropy input, nonce input, DRBG state, or generated DRBG bytes.

## Customer PoC package v0

Create a local package draft:

```sh
cmake --build build --target ano_crypto_create_poc_package
```

Run the package smoke flow:

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The package smoke target stages an installed SDK prefix, configures the packaged `examples/core_poc` project with `find_package(AnoCryptoC)`, builds it, and runs it.

## Evidence draft collection

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated evidence directory is a local engineering draft. It is not official validation evidence and should be reviewed before sharing.

Generated files include public-surface, thread-safety, concurrency/sanitizer, thread-platform, build-identity ABI, SHA-512-family, differential/portability, AES hardening/compiler-output, AES-CMAC implementation, random-service, lifecycle, entropy/health, entropy-operational, CSP/zeroization policy and compiler-output, clean Release exported-symbol, reproducibility, package-smoke, tagged-rehearsal, hosted-review, signing-handoff, and hash-manifest records under `build/evidence/p2-09/`, including `release-provenance/`, `release-signing-handoff/`, and their separately verified bindings. Differential logs retain provider/version/status/comparison-count metadata only.

## P1-17 entropy failure-injection and health-test skeleton

- P1-10 reproducible build notes, CI matrix skeleton, API/ABI checklist, public-surface checks, and symbol-hiding option retained
- P1-11 customer PoC package v0 staging and smoke retained
- P1-12 scope/validation planning documents retained
- P1-13 SBOM/signing placeholder and evidence/PoC manifest workflow retained
- `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` added as the current public support-boundary draft
- `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md` added for ON/OFF lanes and log review
- `ano_crypto_thread_safety_policy_check` added as an engineering guardrail target
- Evidence workflow now writes `THREAD_SAFETY_CHECK.txt`
- PoC package now writes `THREAD_SAFETY_PACKAGE_NOTES.txt`
- PoC package smoke checks that package manifest/review/status/thread-safety records exist
- Clean-room source manifest and exclusion log updated for P1-14 thread-safety support-matrix additions
- `docs/design/SHA2_384_512_SCOPE_DECISION.md` records the P1-15 reservation history and P1-22 implementation decision
- `docs/kcmvp/SHA2_384_512_KAT_PLAN.md` records the P1-15 KAT/evidence plan
- `ano_crypto_sha512_family_policy_check` verifies SHA-384/SHA-512 implementation, KAT, negative-test, evidence, and PoC wiring
- `tests/kat/sha512_family_kat_tests.c` and `tests/negative/sha512_family_api_negative_tests.c` verify implemented processing and negative boundaries
- Evidence workflow now writes `SHA512_FAMILY_CHECK.txt` and `SHA512_FAMILY_REVIEW_NOTES.txt`
- PoC package now writes `SHA512_FAMILY_PACKAGE_NOTES.txt`
- `include/anocrypto/ac_random.h` was introduced as a reserved boundary in P1-16 and is implemented in P1-19
- `src/drbg/random.c` now drives source-backed HMAC_DRBG-SHA-256 through the reviewed platform entropy seam
- `tests/unit/random_service_tests.c` verifies lifecycle, provider-disabled, cross-API misuse, output clearing, and POSIX fork/reseed behavior
- `docs/design/PUBLIC_RANDOM_API_DECISION.md` records the public random API decision
- `docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md` records DRBG/entropy evidence rules
- `ano_crypto_random_service_policy_check` verifies random-service implementation wiring
- Evidence workflow writes `RANDOM_SERVICE_CHECK.txt` and `RANDOM_SERVICE_REVIEW_NOTES.txt`
- PoC package writes `RANDOM_SERVICE_PACKAGE_NOTES.txt`

## P1-18 entropy source and operational policy

- `docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md` records source classification, health-test ownership, operational probe, failure fatality, and process-change decisions.
- `docs/ci/ENTROPY_OPERATIONAL_CI_CHECKLIST.md` defines normal, failure-injection, clean Release, PoC, and evidence checks.
- OS APIs selected by CMake are classified internally as external conditioned random providers rather than direct raw-noise interfaces.
- P1-17 duplicate-block candidates can attach only to an explicitly classified internal raw-noise engineering callback.
- `ano_crypto_internal_entropy_source_probe_operational()` checks the configured OS provider with a fixed temporary request and clears the temporary buffer before return.
- Source-backed internal HMAC_DRBG state records process identity; a detected process change returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and clears generation output until reseed succeeds.
- `ano_crypto_entropy_operational_policy_check` verifies source, health-owner, process-boundary, evidence, and public-header rules.
- Evidence collection writes `ENTROPY_OPERATIONAL_POLICY_CHECK.txt` and `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`.
- PoC packaging writes `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt` and rejects internal process-guard/source-policy artifacts.
- P1-18 added no public function or installed internal type; P1-19 implements the already-reserved public random-service symbols.

## P1-19 public random service

- The existing `ano_crypto_random_*` symbols now implement caller-owned opaque random contexts using HMAC_DRBG-SHA-256.
- `ano_crypto_random_init()` and `ano_crypto_random_reseed()` obtain seed material only through the reviewed internal platform provider seam.
- `ano_crypto_random_generate()` accepts only contexts created by the random-service init path; explicit-entropy DRBG contexts are rejected.
- Requests are limited to `ANO_CRYPTO_RANDOM_MAX_REQUEST_SIZE` bytes and all valid output buffers are cleared on failure.
- A detected process change returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED`; callers must reseed in the new process before retrying.
- Prediction-resistance requests, VM snapshot/restore detection, and suspend/resume detection remain unsupported.
- The dedicated failure-injection lane covers deterministic success, complete failure, partial-write failure, and manual reseed retry without recording generated bytes.
- The install smoke and customer PoC example call the public random lifecycle but print only status and byte counts.

## P2-04 entropy and DRBG responsibility freeze

P2-04 freezes an internal responsibility and evidence model without claiming institution acceptance:

- policy ID `EDP-P2-04-R1`;
- provider profile `ENT-OE01-LINUX-GETRANDOM-R1`;
- Linux `getrandom(flags=0)` classified as an external conditioned provider;
- no direct raw-noise source in the initial candidate;
- generic duplicate-block health scaffolding excluded from conditioned provider output;
- source-backed instantiate uses separate 32-byte entropy and 16-byte nonce requests;
- reseed uses 32 bytes, per-request generation is capped at 65,536 bytes, and the implementation reseed interval is `2^48`;
- prediction resistance, automatic VM-clone detection, and automatic suspend/resume detection remain unsupported;
- fork/process-marker change requires successful reseed before generation;
- no entropy, seed, DRBG state, or generated bytes are retained in evidence.

Controlled outputs are `docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md`, the responsibility and health/recovery matrices, `docs/kcmvp/P2_04_IMPLEMENTATION_TEST_DELTA.md`, `release/ANOCRYPTO_C_ENTROPY_DRBG_POLICY.cmake`, `tools/check_entropy_drbg_policy.cmake`, and status-only operational review evidence.

## Important limitations

- This is not an official validation result.
- Hash_DRBG is not implemented. AES-128/192/256, CBC without padding, full-tag AES-CMAC, and the reviewed AES-256-GCM one-shot/streaming profile are implemented; generic CTR, padding helpers, GMAC, tag truncation, and additional block ciphers remain outside the current scope.
- KAT pass results are correctness checks only and do not replace formal validation.
- The explicit-entropy HMAC_DRBG-SHA-256 API remains caller-managed and separate from the public random service.
- The entropy callback seam is test/internal plumbing and is not an operational entropy source.
- The platform entropy provider seam, operational probe, process guard, and DRBG source helpers are internal; their tests are engineering checks only and do not store provider output.
- HMAC key handles are in-memory only; export, persistence, keystore integration, and full thread-safety support are not finalized.
- Prediction-resistance requests are rejected; a reviewed per-request entropy policy has not been selected.
- Error recovery policy after fatal state remains TODO.
- Public build identity still exposes one versioned struct layout; field getters should be considered before ABI freeze.
- Binary reproducibility across hosts/toolchains is not established yet.
- The dedicated entropy failure-injection build exposes internal test symbols only to internal tests; ordinary, Release, and PoC builds keep the hook disabled, and release-style builds hide internal symbols where supported.
- Customer PoC package v0 is an integration draft and still needs legal/evaluation notice review before external sharing.
- P1-32 defines and tests the offline signing handoff and verification policy, but no production signer, timestamp authority, signature, timestamp response, role approval, tag run, or external-release authorization has been provisioned or completed.
- P1-33 rehearses the tagged-release flow only in an isolated synthetic/local repository. It creates no tag in the user repository and records no synthetic result as a hosted run.
- P1-14 thread-safety support-matrix additions are retained.
- P1-15 was a historical reserved-scope step; P1-22 supersedes that processing boundary with implementation, KAT, negative-test, and evidence coverage.
- P1-16 introduced the random-service boundary; P1-19 implements the existing symbols without adding new public functions.
- This repository does not claim official validation or external review completion.
- P1-17 health-test candidates are engineering skeletons, not finalized noise-source tests or validation evidence.
- Process-change protection applies to source-backed random-service contexts; the public explicit-entropy DRBG API remains caller-managed.
- P1-18 OS-provider classification does not prove provider quality or suitability for every target environment.

### P1-17 entropy boundary additions

- `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS` defaults to `OFF` and cannot be enabled when tests are disabled.
- A dedicated CI lane enables deterministic provider success/failure/partial-write behavior without using real entropy.
- `src/internal/ac_entropy_health_internal.h` and `src/drbg/entropy_health.c` add bounded startup/continuous duplicate-block candidates for state-machine testing only.
- Health-test failure zeroizes the requested output; a source configured with fatal policy also places the module in sticky `ERROR_STATE`.
- Fatal reseed health failure invalidates the affected HMAC_DRBG context by zeroization.
- PoC staging performs a separate clean shared-library build with tests/hooks off and internal symbol hiding on.
- Evidence collection adds `ENTROPY_HEALTH_CHECK.txt` and `ENTROPY_HEALTH_REVIEW_NOTES.txt`.
- Final direct-noise-source tests, thresholds, and recovery policy remain TODO / confirmation required.


## P1-20 random lifecycle and portability hardening

- The public random service now fails closed when either the reviewed platform provider or process marker is unavailable.
- `ano_crypto_random_lifecycle_stress_tests` repeatedly exercises init/generate/reseed/uninit while zeroizing transient output.
- A dedicated tests-only hook verifies the HMAC_DRBG `2^48` reseed-counter boundary, failed-reseed persistence, recovery, and no-wrap behavior.
- Known VM snapshot/restore or clone events require inherited contexts to be discarded and each instance independently reinitialized; automatic detection is not implemented.
- Suspend/resume handling remains a consuming-product policy.
- `abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt` stores the retained 69-name ABI-0 public symbol baseline; `abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt` stores the current 80-name additive surface.
- Clean Release, evidence, and PoC flows compare shared-library exports with that baseline and keep both internal test-hook options disabled.
- Hosted CI collects status-only platform review artifacts; actual hosted results still require review.

## P1-21 concurrency, sanitizer, and build-identity ABI hardening

- `tests/unit/csp_thread_safety_tests.c` now uses Win32 native threads on Windows and pthreads on POSIX-like systems.
- The concurrency regression covers repeated per-thread import/compute/destroy cycles and concurrent compute operations sharing one read-only HMAC handle.
- `ano_crypto_thread_platform_review` emits backend name, availability, concurrency-test presence, and sanitizer mode only.
- `ANO_CRYPTO_SANITIZER_MODE=address-undefined` enables GCC/Clang-family AddressSanitizer and UndefinedBehaviorSanitizer instrumentation; the required hosted lane is Linux GCC.
- Local GCC ASan/UBSan full CTest is part of the P1-21 engineering evidence. Clang runtime and ThreadSanitizer remain separate review items.
- `ano_crypto_build_identity_t` retains ABI version 1 and its recorded field order without adding a public function or symbol.
- `ano_crypto_concurrency_sanitizer_policy_check` verifies source, CI, evidence, PoC, and ABI decision wiring.
- Hosted workflow configuration creates status-only thread artifacts, but actual runner results must be reviewed before widening platform support wording.


## P1-22 SHA-512-family implementation

- Implements SHA-384 and SHA-512 behind the existing algorithm identifiers without adding a public symbol.
- Implements HMAC-SHA-384 and HMAC-SHA-512 for raw-key, streaming, and CSP HMAC key-handle paths.
- Adds startup self-tests and extended public-source FIPS/RFC 4231 KAT coverage.
- Adds negative context/state/output-size/cross-service regression tests.
- Extends install and PoC smoke to all three SHA-2 digest and HMAC variants while suppressing digest/MAC output.
- Replaces the historical reserved-scope policy with `ano_crypto_sha512_family_policy_check`.
- Leaves actual hosted Windows/macOS and non-default architecture review as TODO; P1-23 adds a separate test-only differential and portability layer.


## P1-23 differential validation and SHA-512 portability

- Selects OpenSSL 3.x `libcrypto` as an independent test-only provider for SHA-256/384/512 and HMAC variants.
- Compares AnoCrypto-C one-shot and streaming paths over deterministic padding/block boundaries, long HMAC keys, larger messages, and intentionally unaligned addresses.
- Records only provider/version, comparison count, representation metadata, and pass/fail status; no digest, MAC, key, message, entropy, seed, DRBG state, or random value is logged.
- Adds `ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS` and a CI-only required-provider option; the product target, installed SDK, and customer PoC do not link to OpenSSL because of this lane.
- Adds C99 representation compile guards and `ano_crypto_sha512_portability_review` for endian metadata, variable chunking, and unaligned input.
- Adds a dedicated hosted Linux differential job and a Linux GCC `-m32` portability workflow. Workflow presence does not imply those hosted artifacts have been reviewed.
- Adds `ano_crypto_differential_portability_policy_check`, evidence logs/template, clean-room provenance, and PoC exclusion checks.
- Preserved the then-current public header/API and 53-symbol ABI-0 export baseline.

P1-24 was the implementation entry gate. It added no block-cipher code or public ABI and conditionally approved a narrow AES/CBC slice.

## P1-25 AES-CBC implementation

- Adds the reviewed public `ac_cipher.h` surface with eight exported functions.
- Implements AES-128/192/256 and CBC without padding using CSP-backed non-exportable in-memory keys.
- Allows exact in-place processing and rejects partial overlap, incomplete blocks, invalid IV/key sizes, stale handles, and cross-service contexts.
- Adds startup KATs, extended FIPS 197/SP 800-38A KATs, negative tests, and OpenSSL 3.x test-only differential comparison.
- Extends install and customer PoC smoke without printing keys, IVs, plaintext, ciphertext, or reference results.
- Updates the ABI-0 exported-symbol baseline from 53 to 61 entries after explicit review.
- Leaves compiler-output, microarchitecture-specific side-channel analysis, hosted 32-bit/big-endian evidence, CTR, AEAD, padding helpers, CMAC/GMAC, and domestic algorithms as TODO.

## P1-26 AES/CBC hardening

- Preserves the reviewed eight-function cipher API and 61-symbol ABI-0 baseline.
- Adds private AES context magic and exact expanded-round-key-size guards for AES-128/192/256.
- Makes failed internal or public reinitialization fail closed by clearing an earlier valid schedule once context storage has been validated.
- Adds a 128-iteration CSP key-import/init/destroy/stream/uninit lifecycle stress test and verifies complete context zeroization.
- Extends the cross-platform CSP concurrency test to both HMAC and AES handles in per-thread and shared-read modes.
- Adds status-only portability review for unaligned key/IV/input/output byte buffers, variable complete-block streaming, exact in-place processing, and handle-destroy-after-init ownership.
- Expands the OpenSSL 3.x test-only AES-CBC differential corpus to 2,016 comparisons over 96 deterministic cases per key size, 1 through 64 blocks, one-shot/streaming/in-place, and encrypt/decrypt paths.
- Adds `ano_crypto_aes_compiler_review`, which captures Release-library disassembly and metadata for manual target-specific review without making an automated side-channel verdict.
- Adds hosted workflow structure for GCC/Clang compiler artifacts, 32-bit AES portability, and s390x big-endian cross-run evidence. Workflow presence does not mean those artifacts have been reviewed.

## P1-27 CMAC entry gate

P1-27 selected the narrow AES-CMAC scope and reviewed the API, dedicated key-purpose, KAT, evidence, and package boundaries without adding code or public ABI.

## P1-28 AES-CMAC implementation

- Adds public `ac_cmac.h` with eight reviewed functions.
- Adds `ANO_CRYPTO_KEY_TYPE_CMAC` as a dedicated CSP key purpose distinct from AES encryption keys.
- Implements AES-CMAC with AES-128/192/256, full 16-byte tags, empty-message support, streaming compute, one-shot compute, and one-shot verify.
- Adds startup KAT and all 12 public NIST AES-CMAC examples for AES-128/192/256.
- Adds negative lifecycle, short-buffer, wrong-purpose, stale-handle, and authentication-failure coverage.
- Adds OpenSSL 3.x test-only differential comparison without adding a product or PoC runtime dependency.
- Extends install and PoC smoke while suppressing key, message, tag, subkey, and intermediate values.
- Updates the reviewed clean Release ABI-0 baseline from 61 to 69 symbols, adding only the eight CMAC functions.
- Leaves tag truncation, AES-CTR, GCM/GMAC, protocol formatting, key persistence/export, and target-specific side-channel conclusions deferred.


## P1-29 AES-CMAC hardening

- Retains the eight-function `ac_cmac.h` API and 69-symbol ABI-0 baseline.
- Validates private CMAC state version, key-size class, pending length, and the
  embedded AES schedule before use; marked inconsistent contexts are cleared.
- Makes failed reinitialization fail closed after a valid context-size check.
- Rejects context/input/output/output-length aliasing and clears one-shot output
  after post-preflight failures.
- Bounds `uninit` to the reviewed context extent; short lengths clear only the
  supplied extent and require a correct-size cleanup retry.
- Adds 128 lifecycle iterations, per-thread/shared-handle CMAC concurrency, and
  1,728 test-only OpenSSL CMAC comparisons.
- Captures compiler/target/library metadata and CMAC disassembly for manual
  target-specific review; no completed side-channel conclusion is made.


## P1-30 release compatibility and hosted artifact manifests

- Adds `release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake` as the single reviewed metadata record for the current version, expected tag, fingerprint, evidence slice, ABI line, symbol count, and compatibility baseline.
- Preserves the project-owned P1-29 public headers and 69-symbol list under `abi/baselines/0.1.0-p1-29-cmac-hardening/` with SHA-256 checksums.
- Adds `ano_crypto_release_compatibility_policy_check`, which compares normalized public function declarations, types, non-version constants, and stored exports against the P1-29 baseline.
- Builds and runs a consumer with the preserved P1-29 headers against the then-current P1-30 library, including through the installed CMake package.
- Adds deterministic hosted-artifact bundle creation and verification with release identity, lane/platform/compiler metadata, file sizes, SHA-256 hashes, text-only restrictions, and explicit sensitive-value marker checks.
- Adds create/verify/tamper regression coverage for the artifact manifest.
- Keeps the public API and ABI-0 export set unchanged at 69 symbols.
- Records the expected tag as metadata by default; strict mode requires actual tag/commit inputs and a Git-backed check that the tag, supplied commit, and `HEAD` resolve to one commit.

## P1-31 unsigned release provenance and source SBOM gate

- Retains the P1-29 public-header compatibility baseline, the P1-30 installed-package compatibility consumers, and the 69-symbol `ANOCRYPTO_C_0` export set.
- Adds a deterministic source-file inventory in SPDX 2.3 JSON for the exact files selected for `anocrypto-c-source.tar.gz`.
- Binds the source archive, source-file hashes, source SBOM, current public-header manifest, ABI-0 symbol list, and hosted-artifact disposition into `RELEASE_PROVENANCE.txt` and `RELEASE_ARTIFACTS.sha256`.
- Verifies exact manifest coverage, path safety, source-tree hashes, source-SBOM paths/checksums, package verification code, public-header/symbol counts, and explicit unsigned/TODO authorization state.
- Adds regression cases for tampered SBOM data, unlisted files, false signed/approved claims, unsafe source paths, wrong tag metadata, and incomplete hosted disposition.
- Adds a `release-provenance|Linux` hosted bundle lane and requires it in the hosted artifact index; actual reviewer/date disposition remains TODO until a real hosted run is examined.
- Keeps signing mode `UNSIGNED`, records `External release authorized: NO`, and rejects signature/key/token-like files rather than treating them as verified inputs.
- Keeps the internal source archive, source SBOM, signing request, and release authorization record out of the customer PoC package; the PoC receives only status/policy notes.
- Does not add a public header, public function, cryptographic service, or exported symbol.
- Leaves dependency discovery, vulnerability analysis, license conclusions, actual signature generation/verification, signer identity, timestamping, legal/product authorization, and external sharing approval as TODO items.

## P1-32 offline release-signing handoff

- Retains the P1-31 unsigned provenance bundle, P1-29 public-header baseline, and unchanged 69-symbol `ANOCRYPTO_C_0` public surface.
- Defines `RELEASE_ARTIFACTS.sha256` as the exact detached-signature subject, Ed25519 raw 64-byte signature encoding, and SHA-256 over SPKI DER as the approved public-key fingerprint.
- Requires an RFC 3161 timestamp response over `RELEASE_ARTIFACTS.sha256.ed25519` before external release.
- Generates and verifies a key-free handoff bundle that binds provenance, hosted disposition, signer policy, timestamp policy, signing request, authorization checklist, and complete file hashes.
- Requires distinct Security Reviewer and Product Release Owner identities and treats GitHub artifact attestation only as optional secondary evidence, never the trust root.
- Adds key-free negative regressions plus Debug-only, explicit opt-in ephemeral OpenSSL fixtures for signature and timestamp verification; fixture keys, signatures, and timestamp material are removed after testing.
- Keeps the production signer and timestamp authority `UNPROVISIONED`, handoff readiness `BLOCKED`, signing mode `UNSIGNED`, and external release authorization `NO`.
- Does not perform a hosted run, create or verify a production signature, create a release tag, provision credentials, or authorize external distribution.

## P1-33 clean tagged-release rehearsal and hosted review

- Rehearses expected tag, supplied commit, checked-out `HEAD`, and clean-worktree identity in an isolated clean checkout using a synthetic local tag.
- Rejects dirty worktrees, moved tags, moved `HEAD`, commit/ref mismatches, incomplete required lanes, malformed or unsafe artifacts, and false review claims.
- Records artifact hash, workflow commit/ref, platform, compiler, architecture, reviewer, review date, disposition, and actual-versus-synthetic origin.
- Keeps unavailable hosted results `NOT_RUN` or `UNREVIEWED`; synthetic fixtures prove policy behavior only.
- Retains `RELEASE_ARTIFACTS.sha256` as the only signing subject and keeps production signer/TSA `UNPROVISIONED`, repository `UNSIGNED`, handoff `BLOCKED`, and external release `NO`.
- Adds no public header, function, type, constant, cryptographic service, or exported symbol.


## P2-05 CSP and zeroization evidence closure

P2-05 freezes internal policy `CZP-P2-05-R1` without claiming institution acceptance:

- module-owned CSP table entries for HMAC, block cipher, and CMAC are zeroized on explicit destroy, orderly shutdown, and locked fatal transition;
- shutdown/fatal wipe changes live slots to `ZEROIZED`, invalidates use, and permits one explicit transition to `DESTROYED`;
- caller-owned HMAC, AES-CBC, AES-CMAC, DRBG, and random contexts are not globally tracked and require documented final/uninit cleanup;
- persistence, export, wrapping, platform keystore, and multi-process sharing remain outside the initial candidate;
- local disassembly capture is an engineering input only; exact OE-01 GCC 13.3.0 human review remains `NOT_RUN`;
- lock-acquisition failure can latch fatal state without safely walking the CSP table and requires external/secure-coding review;
- no physical-remanence, universal optimizer, or formal-validation claim is made.
## P2-06 secure coding and target-specific side-channel review

P2-06 freezes internal policy `SCP-P2-06-R1` without claiming universal constant-work behavior, side-channel resistance, fault resistance, completed independent assessment, or formal validation:

- records source, GCC analyzer, Clang strict-warning, and Clang static-analyzer review with limitations and retained findings;
- explicitly handles `ANO_CRYPTO_CSP_STATE_INVALID` in the internal CSP state switch;
- applies the selected Linux Release hardening profile `SCB-P2-06-LINUX-ELF-R1`;
- captures ELF stack/RELRO/BIND_NOW/RWX/TEXTREL/stack-protector status and library hash;
- documents table-free portable AES source and fixed-length CMAC tag-comparison candidates;
- keeps exact OE-01 GCC 13.3.0 review, named reviewer disposition, empirical timing/cache work, and power/EM/fault work open;
- preserves 14 public headers, 69 exported symbols, and `ANOCRYPTO_C_0`.

## P2-07 controlled submission-document skeleton

P2-07 creates the internal document set under `docs/kcmvp/submission/`, controlled by `SDP-P2-07-R1` and `release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt`. Official template conformity, security level, secure delivery channel, institution results, and authorization remain external or unset.

## P2-08 pre-consultation rehearsal

P2-08 completes the planned internal readiness sequence with policy `PCR-P2-08-R1`. It prepares a status-only rehearsal package containing the controlled agenda, evidence walkthrough, question pack, unresolved-decision register, response template, action-impact matrix, readiness register, and submission staging references. The package contains no source, binary, credentials, cryptographic result values, institution answers, or authorization.

```sh
cmake --build build --target ano_crypto_preconsultation_rehearsal_policy_check
cmake --build build --target ano_crypto_preconsultation_rehearsal_package_smoke
```

A captured answer never authorizes code, ABI, algorithm, signing, submission, release, or customer distribution by itself.

## P2-09 cumulative closeout review

P2-09 freezes policy `P2C-P2-09-R1` and consolidates the P2-01 through P2-08 result into a controlled closeout decision, claims summary, open-item/exit-criteria register, next-work gate, and a 17-document status-only pre-consultation closeout package. The package contains no source, binary, credentials, cryptographic result values, institution answers, signature, timestamp response, or authorization.

```sh
cmake --build build --target ano_crypto_p2_closeout_policy_check
cmake --build build --target ano_crypto_preconsultation_closeout_package_smoke
```

P2 closeout permits internal pack review only. It does not authorize external consultation, implementation, formal submission, signing, publication, or release.
