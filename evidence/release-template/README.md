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
- Source SBOM status and scope
- Unsigned signing status, key-free handoff readiness, and release authorization state
- Release provenance bundle/check result
- Offline-signing handoff/check result
- Clean tagged-release rehearsal/check result and explicit synthetic origin
- Hosted artifact review matrix with `NOT_RUN`/`UNREVIEWED` state
- Sensitive-artifact scan across evidence, installed SDK, and PoC package
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
- Confirmation that the PoC SDK was rebuilt with entropy/random hooks off, OpenSSL differential tests off, sanitizer mode none, and internal symbol hiding on
- AES/CBC implementation and hardening policy results
- AES lifecycle/portability review log
- AES compiler-output summary and disassembly for manual internal review
- Clean Release 69-symbol baseline result

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
- `docs/kcmvp/SBOM_SIGNING_PLAN.md` records the retained P1-32 Ed25519/RFC-3161 profile plus P1-33 clean-rehearsal/hosted-review, unsigned/BLOCKED, and authorization boundaries.
- `docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md` records manifest generation rules.
- `docs/business/POC_PACKAGE_REVIEW_MANIFEST.md` records customer package review fields.
- `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` records the P1-14 public thread-safety support boundary.
- `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md` records the P1-14 thread-safety CI lanes and log-review rules.
- `tools/check_thread_safety_policy.cmake` checks thread-safety policy/evidence wiring.
- `tools/check_sha512_family_policy.cmake` checks SHA-384/SHA-512 implemented-scope wiring.
- `tools/collect_release_evidence.cmake` consumes prepared prerequisites and writes `release-provenance/`, `release-signing-handoff/`, their check records, `SENSITIVE_ARTIFACT_SCAN.txt`, `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST_README.txt`, `EVIDENCE_MANIFEST.sha256`, and retained algorithm/platform review records.

## Commands

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install smoke:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p2-01
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p2-01
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
- Retain the P1-32 source-inventory/signing profile and P1-33 unsigned/BLOCKED boundary until actual hosted review, production signer/TSA provisioning, distinct role approvals, cryptographic verification, and release authorization complete.
- Add customer PoC legal/evaluation notice after legal review.
- Add reviewed platform CI evidence for Linux, Windows, and Apple entropy provider branches.
- Review actual Windows and POSIX concurrent CSP HMAC/AES artifacts.
- Retain the selected GCC ASan/UBSan lane and review Clang/ThreadSanitizer separately.
- Add archive timestamp normalization if byte-identical source archives become required.
- Review hosted random-service results, VM snapshot/suspend guidance, lifecycle stress, and final validation-boundary mapping.
- Review P1-26 AES/CBC compiler-output, hosted 32-bit/big-endian, and target-specific timing/cache evidence before broader claims or mode expansion.
- Keep OpenSSL differential validation test-only and select any additional provider through a separate provenance/license review.

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
- `SBOM_STATUS.txt` describes the generated source-only inventory and its limitations; `SIGNING_STATUS.txt` remains explicitly `UNSIGNED`; handoff readiness remains `BLOCKED`
- thread-safety check output matches the support matrix and current build option
- evidence and package manifests use intended file paths only


## Historical P1-16 SHA-2 Reservation Review

P1-16/P1-15 reserved-processing records are historical only. For P1-22, confirm they are clearly marked as superseded and review the active `SHA512_FAMILY_CHECK.txt` and `SHA512_FAMILY_REVIEW_NOTES.txt` instead. No digest values, MAC values, key bytes, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data may appear in evidence.

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


## P1-22 Concurrency, Sanitizer, and Build Identity Review

Include and review:

- `CONCURRENCY_SANITIZER_CHECK.txt`;
- `THREAD_PLATFORM_REVIEW.log`;
- `CONCURRENCY_SANITIZER_REVIEW_NOTES.txt`;
- GCC ASan/UBSan CTest result;
- `ano_crypto_build_identity_abi_tests` result;
- actual hosted thread-platform artifacts when available.

Confirm the cross-platform concurrency test uses native Win32 threads or pthreads, the clean Release/PoC sanitizer mode is `none`, the public symbol count remains 53, and build-identity ABI version 1 matches the recorded field order. Clang sanitizer runtime, ThreadSanitizer, and hosted Windows/macOS result review remain TODO until actual artifacts are retained.


## P1-22 SHA-512 Family Review

Review `SHA512_FAMILY_CHECK.txt`, `SHA512_FAMILY_REVIEW_NOTES.txt`, startup/extended KAT results, negative tests, install/PoC smoke, and the clean 69-symbol export baseline. Evidence must not retain digest values, MAC values, HMAC keys, entropy, seed material, DRBG state, or generated random bytes. Review P1-23 differential and portability records separately from KAT provenance.


## P1-23 Differential and Portability Review

Review `DIFFERENTIAL_PORTABILITY_CHECK.txt`, `DIFFERENTIAL_VALIDATION.log`, `SHA512_PORTABILITY_REVIEW.log`, and `DIFFERENTIAL_PORTABILITY_REVIEW_NOTES.txt`. Confirm OpenSSL is test-only and absent from product/installed/PoC runtime dependencies, the public symbol count remains 53, and logs retain only provider/version, architecture/representation metadata, comparison count, and status. Actual hosted 32-bit, Windows, macOS, and big-endian artifacts must not be marked reviewed unless the referenced runs completed successfully.


## P1-26 AES/CBC Hardening Review

Include and review:

- `AES_CBC_IMPLEMENTATION_CHECK.txt`;
- `AES_HARDENING_CHECK.txt`;
- `AES_HARDENING_REVIEW.log`;
- `AES_COMPILER_OUTPUT_REVIEW.txt`;
- `AES_COMPILER_OUTPUT_DISASSEMBLY.txt`;
- `AES_CBC_REVIEW_NOTES.txt`;
- `DIFFERENTIAL_VALIDATION.log`;
- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`;
- actual hosted AES hardening, 32-bit, big-endian, Windows, and macOS artifacts when available.

Confirm that:

- P1-26 retains the eight reviewed public cipher functions and 61-symbol baseline;
- failed reinitialization invalidates the prior caller-owned cipher context;
- lifecycle, unaligned/variable-chunk, exact in-place, CSP destruction-after-init, and HMAC/AES concurrency regression pass;
- OpenSSL differential comparison reports 2,016 AES comparisons without output bytes;
- compiler-output capture is treated as an internal manual-review artifact, not an automated constant-time or side-channel conclusion;
- customer PoC and installed SDK exclude internal AES headers/source, hardening/lifecycle test source, OpenSSL provider artifacts, and compiler disassembly;
- no AES key, IV, plaintext, ciphertext, expanded schedule, intermediate state, entropy, seed, DRBG state, random output, private key, signing key, token, or customer data appears in logs or packages;
- hosted workflow definitions are not marked reviewed until actual artifact hashes, toolchains, reviewers, dates, limitations, and dispositions are recorded.

## P1-28 AES-CMAC Implementation Review

- Review `AES_CMAC_IMPLEMENTATION_CHECK.txt` and `AES_CMAC_REVIEW_NOTES.txt`.
- Confirm public `ac_cmac.h`, the dedicated CMAC key purpose, startup/extended
  KATs, negative tests, test-only differential result, install/PoC smoke, and
  the 69-symbol clean Release baseline.
- Confirm internal CMAC context/source/tests and OpenSSL provider artifacts are
  absent from installed SDK and PoC packages.
- Confirm logs contain no key, message, tag, subkey, intermediate, entropy,
  seed, DRBG, random, private-key, token, or customer values.
- Confirm tag truncation, CTR, GCM/GMAC, persistence, export, protocol formatting,
  and target-specific side-channel conclusions remain deferred.

## P1-29 AES-CMAC Hardening Review

Include `AES_CMAC_HARDENING_CHECK.txt`, hardening/lifecycle status logs, CMAC
compiler-output summary and optional disassembly, and
`AES_CMAC_HARDENING_REVIEW_NOTES.txt`. Confirm no public symbol increase beyond
69, no private/provider artifact in the PoC or installed SDK, and no sensitive
values in logs. Compiler-output capture requires human target-specific review
and is not an automated side-channel conclusion.

## P1-31 release provenance review

Review `RELEASE_PROVENANCE_REVIEW.md` together with:

- `release-provenance/RELEASE_PROVENANCE.txt`;
- `release-provenance/SOURCE_FILES.sha256`;
- `release-provenance/SOURCE_SBOM.spdx.json`;
- `release-provenance/RELEASE_ARTIFACTS.sha256`;
- `release-provenance/HOSTED_ARTIFACT_DISPOSITION.txt`;
- `release-provenance/SIGNING_STATUS.txt`;
- `release-provenance/RELEASE_AUTHORIZATION.md`;
- `RELEASE_PROVENANCE_CHECK.txt` and `RELEASE_PROVENANCE_POLICY_CHECK.txt`.

The records are unsigned engineering evidence. Do not infer dependency completeness, vulnerability analysis, license conclusions, authenticated attestation, completed hosted review, or external-release authorization.

## P1-32 offline-signing handoff review

Review together:

- `release-signing-handoff/SIGNING_POLICY.txt`;
- `release-signing-handoff/SIGNING_REQUEST.txt`;
- `release-signing-handoff/SIGNING_HANDOFF_STATUS.txt`;
- `release-signing-handoff/SIGNING_HANDOFF_FILES.sha256`;
- `release-signing-handoff/RELEASE_AUTHORIZATION.md`;
- `RELEASE_SIGNING_HANDOFF_CHECK.txt`;
- `SENSITIVE_ARTIFACT_SCAN.txt`.

Confirm `RELEASE_ARTIFACTS.sha256` is the only subject; signature profile is Ed25519/raw 64 bytes; approved-key fingerprint is SHA-256 over DER SubjectPublicKeyInfo; RFC 3161 timestamp subject is the detached-signature file; required Security Reviewer and Product Release Owner identities are non-placeholder and different once provisioned; GitHub attestation is secondary only. Current production signer/TSA/role state remains unprovisioned, so repository state is `UNSIGNED`, readiness `BLOCKED`, and external authorization `NO`.

Debug-only OpenSSL fixtures are test-path evidence only. Do not retain fixture key, certificate, signature, timestamp request, or timestamp response files in release evidence.

## P1-33 clean release rehearsal and hosted review

Review together:

- clean-checkout rehearsal summary and tag/commit/`HEAD` identity result;
- dirty-worktree, moved-tag, moved-`HEAD`, commit/ref mismatch regressions;
- hosted required-lane index and per-artifact review records;
- artifact SHA-256, workflow commit/ref, platform, compiler, architecture, reviewer, UTC review date, disposition, and origin fields;
- hosted negative-test result for missing/duplicate lanes, unsafe or unlisted files, hash mismatch, oversized/binary-like files, sensitive markers, incomplete reviewer data, conflicting roles, and false approval state.

Confirm the rehearsal tag exists only in an isolated temporary repository. Mark synthetic/local artifacts `SYNTHETIC_LOCAL`, actual hosted-runner artifacts `HOSTED_RUNNER`, absent hosted execution `NOT_RUN`, and downloaded-but-unreviewed output `UNREVIEWED`. No synthetic result may be represented as hosted `PASS` or `APPROVED`. Signing remains `UNSIGNED`, handoff `BLOCKED`, and external authorization `NO`.
