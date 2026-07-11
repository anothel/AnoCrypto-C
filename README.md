# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton. The repository slug may remain `anocrypto-c`, but the product/brand spelling is `AnoCrypto-C`.

Current status: P1-19 foundation with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, expanded KATs and negative tests, internal platform entropy provider seam, dedicated-build entropy failure injection, internal health-test candidates, explicit OS-provider classification, source-backed process-change reseed guard, and an implemented public random-service lifecycle on reviewed Linux, Windows, and Apple provider branches. Evidence, install, and PoC outputs remain engineering drafts.

Implemented processing scope:

- SHA-256 one-shot and streaming API
- HMAC-SHA-256 one-shot and streaming API
- HMAC-SHA-256 compute using an in-memory HMAC key handle
- HMAC_DRBG-SHA-256 deterministic core API
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- Public `ano_crypto_random_*` lifecycle backed by the reviewed platform entropy seam and HMAC_DRBG-SHA-256
- Bidirectional cross-API context separation between explicit-entropy DRBG and random-service generate/reseed operations
- Process-change detection for source-backed random contexts, requiring explicit reseed before generation
- Startup self-test KATs for SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256
- Extended KAT executables for SHA-256, HMAC-SHA-256, HMAC key handle path, and HMAC_DRBG-SHA-256 public vectors
- Negative API tests for hash/HMAC/DRBG/CSP/key argument handling, context lifecycle behavior, handle exhaustion, stale handle rejection, and DRBG helper failure cleanup
- Internal entropy adapter seam with callback and platform OS provider tests
- Dedicated-build deterministic entropy provider hook for success, failure, and partial-write negative paths; disabled by default and absent from public headers
- Internal startup/continuous duplicate-block health-test candidates with operation-local or module-fatal policy wiring
- Fatal health-test failure coverage for DRBG context zeroization and sticky module `ERROR_STATE` transition
- Internal source classification and health-test ownership metadata that keeps OS-provider output outside the raw-noise health skeleton
- Status-only OS-provider operational probe with temporary-buffer zeroization
- Private process identity guard for source-backed DRBG state; generation after process change requires reseed and clears output
- Release evidence draft collection with source archive hash, public-surface check, reproducible-build notes, install smoke logs, PoC package smoke logs, scope-review notes, SBOM/signing placeholder status, and evidence manifest hashes
- Limited internal locking for module/self-test/CSP global state
- Thread-safety support matrix, CI checklist, and policy-check target
- CI matrix skeleton for Linux, macOS, and Windows build/test/install-smoke paths
- API/ABI review checklist for public headers, package identity, and symbol-surface TODOs
- Release-style internal symbol hiding option for supported compilers
- Customer PoC package v0 staging, package manifest/review records, and standalone public-API example smoke
- Development workflow, service scope matrix, differential validation strategy, block-cipher selection gate, side-channel checklist, customer PoC intake questionnaire, SBOM/signing plan, and evidence manifest policy drafts

Skeleton-only / TODO scope:

- SHA-384/SHA-512 ABI placeholders with P1-15 reserved-scope regression tests
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- Hash_DRBG-SHA-256 ABI placeholder
- Direct noise-source selection, source-specific startup/continuous health tests, thresholds, and recovery policy
- Cross-platform hosted-CI review of process-change behavior and unsupported-process-marker policy
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Windows-specific concurrent CSP/HMAC coverage and sanitizer lanes
- Byte-identical source archive and binary reproducibility rules
- Complete SBOM generation and detached-signature workflow
- PoC package legal/evaluation notice and customer-specific support boundary
- Block cipher/mode implementation, pending P1-12 scope decision review

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-19
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-19
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
cmake --install build --prefix /tmp/anocrypto-c-p1-19
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-19
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

Generated files include `SOURCE_ARCHIVE.sha256`, `BINARY_FILES.sha256`, `PUBLIC_SURFACE_CHECK.txt`, `THREAD_SAFETY_CHECK.txt`, `SHA2_SCOPE_CHECK.txt`, `RANDOM_SERVICE_CHECK.txt`, `RANDOM_SERVICE_REVIEW_NOTES.txt`, `ENTROPY_HEALTH_CHECK.txt`, `ENTROPY_HEALTH_REVIEW_NOTES.txt`, `ENTROPY_OPERATIONAL_POLICY_CHECK.txt`, `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`, `REPRODUCIBLE_BUILD_NOTES.txt`, `SCOPE_REVIEW_NOTES.txt`, `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST.sha256`, `ctest.log`, installed-package smoke logs, and PoC package smoke logs when the default P1-19 target is used.

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
- `docs/design/SHA2_384_512_SCOPE_DECISION.md` records the P1-15 SHA-384/SHA-512 reserved scope decision
- `docs/kcmvp/SHA2_384_512_KAT_PLAN.md` records the P1-15 KAT/evidence plan
- `ano_crypto_sha2_scope_policy_check` verifies SHA-384/SHA-512 reserved-scope wiring
- `tests/negative/sha2_reserved_api_tests.c` verifies size/name metadata and NOT_IMPLEMENTED processing behavior
- Evidence workflow now writes `SHA2_SCOPE_CHECK.txt` and `SHA2_SCOPE_REVIEW_NOTES.txt`
- PoC package now writes `SHA2_SCOPE_PACKAGE_NOTES.txt`
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

## Important limitations

- This is not an official validation result.
- SHA-384, SHA-512, Hash_DRBG, and block cipher processing APIs are not implemented yet.
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
- P1-14 thread-safety support-matrix additions are retained.
- P1-15 SHA-384/SHA-512 scope additions do not add new cryptographic algorithm implementations.
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
