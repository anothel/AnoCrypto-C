# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton. The repository slug may remain `anocrypto-c`, but the product/brand spelling is `AnoCrypto-C`.

Current status: P1-29 engineering slice retaining SHA-256, SHA-384, SHA-512 and their HMAC variants, HMAC_DRBG-SHA-256, the public random lifecycle, AES-128/192/256, AES-CBC without padding, and AES-CMAC with full 16-byte tags through dedicated CSP key purposes. The repository includes startup and extended public-source KATs, negative and OpenSSL 3.x test-only differential validation, C99 portability checks, AES lifecycle hardening, compiler-output capture for manual review, and a reviewed 69-symbol ABI-0 baseline. P1-29 adds private CMAC state/lifecycle/alias cleanup hardening, 128-iteration stress, expanded differential and shared-handle concurrency coverage, and compiler-output capture without widening the public API. Actual hosted Linux 32-bit, Windows, macOS, big-endian, and target-specific compiler/side-channel artifacts still require review; evidence, install, and PoC outputs remain engineering drafts.

Implemented processing scope:

- SHA-256, SHA-384, and SHA-512 one-shot and streaming APIs
- HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 one-shot and streaming APIs
- HMAC computation for all three SHA-2 variants using an in-memory HMAC key handle
- AES-128, AES-192, and AES-256 table-free portable primitive used through CSP-backed block-cipher keys
- AES-CBC without padding, caller-provided 16-byte IV, complete blocks only, exact in-place support, and partial-overlap rejection
- Startup and extended public-source AES/CBC KATs plus negative and OpenSSL test-only differential coverage
- P1-26 AES private-context integrity guards, fail-closed reinitialization, 128-iteration lifecycle stress, unaligned byte-buffer/streaming/in-place portability regression, and shared/per-thread CSP AES concurrency coverage
- P1-28 AES-CMAC with AES-128/192/256 dedicated key-purpose handles, full 16-byte tags, one-shot compute/verify, streaming compute, startup/extended KATs, negative tests, and OpenSSL test-only differential coverage
- P1-29 AES-CMAC private state guards, fail-closed reinitialization, bounded uninit and non-overlap rules, lifecycle stress, shared-handle concurrency, 1,728 comparison differential corpus, and manual compiler-output review capture
- AES Release compiler-output capture with compiler/target/library metadata and a manual-review-only interpretation boundary
- HMAC_DRBG-SHA-256 deterministic core API
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- Public `ano_crypto_random_*` lifecycle backed by the reviewed platform entropy seam and HMAC_DRBG-SHA-256
- Bidirectional cross-API context separation between explicit-entropy DRBG and random-service generate/reseed operations
- Process-change detection for source-backed random contexts, requiring explicit reseed before generation
- Startup self-test KATs for SHA-256/SHA-384/SHA-512, their HMAC variants, and HMAC_DRBG-SHA-256
- Extended KAT executables for all implemented SHA-2/HMAC variants, HMAC key-handle paths, and HMAC_DRBG-SHA-256 public vectors
- Deterministic OpenSSL 3.x test-only differential comparisons for SHA-256/384/512, HMAC, AES-CBC, and AES-CMAC paths
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
- Release evidence draft collection with source archive hash, public-surface check, reproducible-build notes, install smoke logs, PoC package smoke logs, scope-review notes, SBOM/signing placeholder status, and evidence manifest hashes
- Limited internal locking for module/self-test/CSP global state
- Thread-safety support matrix, cross-platform Win32/pthread CSP/HMAC test, status-only platform review, CI checklist, and policy-check targets
- CI matrix skeleton for Linux, macOS, and Windows build/test/install-smoke paths
- API/ABI review checklist, stored 69-symbol baseline, and build-identity ABI-0 field-order baseline
- Release-style internal symbol hiding option for supported compilers
- Customer PoC package v0 staging, package manifest/review records, and standalone public-API example smoke
- Development workflow, service scope matrix, differential validation strategy, AES/CBC implementation policy, side-channel checklist, customer PoC intake questionnaire, SBOM/signing plan, and evidence manifest policy drafts

Skeleton-only / TODO scope:

- Hash_DRBG-SHA-256 ABI placeholder
- Direct noise-source selection, source-specific startup/continuous health tests, thresholds, and recovery policy
- Cross-platform hosted-CI review of process-change behavior and unsupported-process-marker policy
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Actual hosted Linux 32-bit, Windows, macOS, thread/random/differential artifacts, and any intended big-endian target; Clang sanitizer runtime and ThreadSanitizer policy review
- Byte-identical source archive and binary reproducibility rules
- Complete SBOM generation and detached-signature workflow
- PoC package legal/evaluation notice and customer-specific support boundary
- Human review of captured AES compiler output and target-specific side-channel behavior; CMAC target-specific review, CTR counter policy, AEAD, padding helpers, GMAC, and domestic-algorithm decisions

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-29
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-29
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
cmake --install build --prefix /tmp/anocrypto-c-p1-29
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-29
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

Generated files include public-surface, thread-safety, concurrency/sanitizer, thread-platform, build-identity ABI, SHA-512-family, differential/portability, AES hardening/compiler-output, AES-CMAC implementation, random-service, lifecycle, entropy/health, entropy-operational, clean Release exported-symbol, reproducibility, package-smoke, and hash-manifest records under `build/evidence/p1-29/`. Differential logs retain provider/version/status/comparison-count metadata only.

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

## Important limitations

- This is not an official validation result.
- Hash_DRBG is not implemented. AES-128/192/256, CBC without padding, and full-tag AES-CMAC are implemented; CTR, AEAD, padding helpers, GMAC, tag truncation, and additional block ciphers remain outside the current scope.
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
- `abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt` stores the current 69-name ABI-0 public symbol baseline.
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
