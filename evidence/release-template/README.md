# anocrypto-c Release Evidence Template

This directory is a release evidence skeleton. It is not a completed release evidence package.

## Required Per-Candidate Artifacts

- Source archive SHA-256
- Binary SHA-256
- Git commit hash
- Compiler identity
- Build options
- Target OS/architecture
- Startup self-test log
- KAT log
- Negative test log
- Install smoke test log
- Known limitations
- Algorithm scope matrix
- Service scope matrix
- Differential validation strategy notes
- Side-channel review checklist status
- Customer PoC intake/scope notes when applicable
- Clean-room source manifest snapshot
- Exclusion log snapshot
- SBOM status
- Signing status
- Reproducible build notes
- Public surface check result
- Thread-safety policy check result
- SHA-384/SHA-512 scope check result
- SHA-384/SHA-512 scope review notes
- CI matrix result summary
- Customer PoC package smoke log
- Customer PoC package manifest review
- Entropy/health policy check result
- Entropy/health review notes
- Confirmation that the PoC SDK was rebuilt with entropy hooks off and internal symbol hiding on

## P1 Evidence Notes

- SHA-256 and HMAC-SHA-256 processing APIs are implemented for the current P1 slice.
- HMAC-SHA-256 can also be computed through an in-memory HMAC key handle lifecycle slice.
- HMAC_DRBG-SHA-256 deterministic core APIs are implemented for the current P1 slice.
- The internal HMAC_DRBG-SHA-256 instantiate/reseed helper from the prior P1 slice remains covered by unit tests.
- Extended KAT tests cover public SHA-256 vectors, RFC 4231 HMAC-SHA-256 cases, the HMAC key handle path, and a public HMAC_DRBG-SHA-256 flow.
- Deterministic chunk-boundary tests cover one-shot versus streaming API consistency.
- Negative tests cover representative module gating, hash/HMAC/DRBG/CSP/key invalid arguments, finalized-context behavior, stale handles, and cleanup paths.
- Entropy adapter and provider tests verify internal seam behavior without storing raw entropy in logs.
- `tools/collect_release_evidence.cmake` creates a local engineering evidence draft from a build tree and can capture source archive SHA-256, public surface check output, reproducible-build notes, installed-package smoke logs, PoC package smoke logs, and scope-review notes.
- `docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md` records the current repeatable-build procedure and unresolved gaps.
- `docs/design/API_ABI_REVIEW_CHECKLIST.md` records the current public header and symbol-surface review checklist.
- `examples/core_poc` records the current public-API customer PoC integration smoke.
- `docs/roadmap/DEVELOPMENT_WORKFLOW.md` records the task flow and next-step plan.
- `docs/kcmvp/SERVICE_SCOPE_MATRIX.md` records implemented, reserved, internal-only, and deferred services.
- `docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md` records allowed comparison sources and logging limits.
- `docs/design/BLOCK_CIPHER_MODE_SELECTION.md` records the block-cipher implementation gate.
- `docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md` records key-dependent code path review questions.
- `docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md` records non-secret customer intake questions.
- `docs/kcmvp/SBOM_SIGNING_PLAN.md` records SBOM/signing placeholders and boundaries.
- `docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md` records manifest generation rules.
- `docs/business/POC_PACKAGE_REVIEW_MANIFEST.md` records customer package review fields.
- `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` records the P1-14 public thread-safety support boundary.
- `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md` records the P1-14 thread-safety CI lanes and log-review rules.
- `tools/check_thread_safety_policy.cmake` checks thread-safety policy/evidence wiring.
- `tools/check_sha2_scope_policy.cmake` checks SHA-384/SHA-512 reserved-scope wiring.
- `tools/collect_release_evidence.cmake` writes `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST_README.txt`, `EVIDENCE_MANIFEST.sha256`, `THREAD_SAFETY_CHECK.txt`, `SHA2_SCOPE_CHECK.txt`, and `SHA2_SCOPE_REVIEW_NOTES.txt` for P1-16 evidence drafts.

## Commands

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install smoke:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-19
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-19
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
```

Customer PoC package smoke:

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

Evidence draft:

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

## TODO

- Add KAT vector local-copy hash policy when vectors are stored as separate data files.
- Add CI-produced evidence bundle layout after workflow review.
- Replace SBOM/signing placeholder fields with reviewed generator/signing outputs when tool and key-management policies are selected.
- Add customer PoC legal/evaluation notice after legal review.
- Add reviewed platform CI evidence for Linux, Windows, and Apple entropy provider branches.
- Add Windows-specific concurrent CSP/HMAC coverage.
- Add sanitizer lanes where supported by selected compiler/toolchain policy.
- Add archive timestamp normalization if byte-identical source archives become required.
- Review hosted random-service results, VM snapshot/suspend guidance, lifecycle stress, and final validation-boundary mapping.
- Add first block cipher/mode implementation only after P1-12 scope inputs are reviewed.
- Add differential tests only after a reviewed public/reference source or tool is selected.

## P1-14 Log Review

If present, review entropy/thread/helper test logs for:

- status-only output
- no key bytes
- no MAC/DRBG output bytes
- no entropy bytes
- no seed or nonce bytes
- no broad support claims beyond the documented global-state lock, internal platform entropy provider, and internal DRBG helper policies
- provider logs include provider name/status only and no entropy bytes
- public surface check is PASS or has documented TODO-only warnings
- reproducibility gaps remain marked as TODO
- PoC package manifests and logs do not include secrets, customer data, or internal deployment details
- scope-review notes do not expand the implemented algorithm boundary by implication
- SBOM/signing status files remain clear placeholders unless reviewed generated artifacts are present
- thread-safety check output matches the support matrix and current build option
- evidence and package manifests use intended file paths only


## P1-16 SHA-2 Scope Review

If present, review SHA-384/SHA-512 scope logs for:

- reserved processing APIs return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`
- metadata queries expose only name, digest size, MAC size, and opaque context size
- KAT manifest entries remain `p1-16-reserved-kat-plan` until implementation evidence exists
- no key bytes, entropy bytes, generated secret bytes, private keys, tokens, or customer data appear in logs
- no wording implies SHA-384/SHA-512 processing implementation has been added

## P1-16/P1-19 random-service evidence

- Include `RANDOM_SERVICE_CHECK.txt`.
- Include `RANDOM_SERVICE_REVIEW_NOTES.txt`.
- Confirm `ac_random.h`, `src/drbg/random.c`, and the P1-20 portability policy match the implemented lifecycle and limitations.
- Confirm lifecycle, provider-disabled, failure-injection, and process-change records contain status only and no entropy, nonce seed material, DRBG state, or generated random bytes.

## P1-17 Entropy/Health Review

If present, review entropy failure-injection and health-test records for:

- `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF` in ordinary, evidence, Release, and clean PoC SDK configurations;
- dedicated failure lane only for deterministic success/failure/partial-write coverage;
- full output zeroization after provider or health-test failure;
- sticky module `ERROR_STATE` and DRBG context invalidation after fatal health failure;
- no installed internal entropy headers or exported test-hook symbols;
- no entropy samples, deterministic hook byte sequences, seed material, DRBG state, or generated random bytes in logs/evidence;
- explicit wording that duplicate-block candidates are engineering skeletons, not finalized source-specific tests.

## P1-18 Entropy Operational Review

If present, review:

- `ENTROPY_OPERATIONAL_POLICY_CHECK.txt`;
- `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`;
- OS-provider classification as external conditioned provider output;
- rejection of raw-noise health attachment for OS-provider and unclassified callback sources;
- source-backed process-change result `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and output clearing;
- clean Release/PoC absence of internal process-guard and source-policy symbols;
- status-only evidence with no provider output, seed material, nonce material, DRBG state, or generated random output.

## P1-19 Public Random-Service Review

Review:

- `RANDOM_SERVICE_CHECK.txt`;
- `RANDOM_SERVICE_REVIEW_NOTES.txt`;
- default-provider lifecycle and installed/PoC smoke results;
- provider-disabled behavior returning `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`;
- dedicated deterministic failure/partial-write results;
- process-change behavior requiring successful reseed before generation;
- clean Release public symbol comparison against the P1-18 baseline;
- status-only evidence with no provider output, entropy/nonce seed material, DRBG state, or generated bytes.

Prediction resistance, VM snapshot/restore detection, suspend/resume detection, and final validation-target inclusion remain unsupported or TODO.


## P1-20 Random Lifecycle and Export Review

Include and review:

- `RANDOM_LIFECYCLE_HARDENING_CHECK.txt`;
- `RANDOM_LIFECYCLE_REVIEW_NOTES.txt`;
- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`;
- `EXPORTED_SYMBOL_BASELINE_REVIEW_NOTES.txt`;
- hosted Linux, Windows, and Apple `ano_crypto_random_platform_review` artifacts when available.

Confirm the clean Release uses tests/examples/hooks OFF and internal symbol hiding ON, the expected and actual public symbol counts are both 53, unsupported process-marker platforms fail closed, VM duplication guidance is explicit, and no generated random bytes, entropy, seed material, or DRBG state are retained.


## P1-21 Concurrency, Sanitizer, and Build Identity Review

Include and review:

- `CONCURRENCY_SANITIZER_CHECK.txt`;
- `THREAD_PLATFORM_REVIEW.log`;
- `CONCURRENCY_SANITIZER_REVIEW_NOTES.txt`;
- GCC ASan/UBSan CTest result;
- `ano_crypto_build_identity_abi_tests` result;
- actual hosted thread-platform artifacts when available.

Confirm the cross-platform concurrency test uses native Win32 threads or pthreads, the clean Release/PoC sanitizer mode is `none`, the public symbol count remains 53, and build-identity ABI version 1 matches the recorded field order. Clang sanitizer runtime, ThreadSanitizer, and hosted Windows/macOS result review remain TODO until actual artifacts are retained.
