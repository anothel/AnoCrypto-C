# AnoCrypto-C Development Workflow

Status: P2-02 validation-readiness workflow; P2-03 is next.

## Purpose

This document explains the working flow used for AnoCrypto-C after the C99 reboot. The goal is to keep implementation, public ABI, test evidence, clean-room records, and customer PoC packaging moving in the same order instead of adding algorithms without review gates.

## Current phase map

| Phase | Main goal | Exit signal |
|---|---|---|
| P0 Foundation | Repository, clean-room records, C ABI skeleton, module lifecycle, self-test, build identity, install smoke | A consumer can build, install, link, initialize, self-test, and shut down the module. |
| P1 Core slice | Small algorithm slice, KATs, negative tests, entropy/DRBG/CSP seams, CI/reproducibility notes, API/ABI review, customer PoC package | The SHA-256/SHA-384/SHA-512 hash and HMAC slice, HMAC_DRBG-SHA-256, random service, CSP lifecycle, and supporting evidence have repeatable tests and explicit scope records. |
| P1 Scope lock | Decide the next validation-target candidate scope before expanding code | Algorithm scope, service matrix, differential-validation strategy, side-channel review checklist, and customer intake inputs are recorded. |
| P1 Evidence/package hardening | Source provenance, package manifest review, evidence generation, offline signing handoff, and clean release rehearsal | Evidence and PoC packages have explicit status/hash records; synthetic/local rehearsal is separated from actual hosted review without storing credentials or claiming production signing. |
| Parallel commercial preparation | Customer PoC hardening, toolchain support matrix, and SecureKit/PKI boundary planning | Customer-facing work remains separate from the validation-readiness sequence and cannot authorize release. |
| P2 Validation readiness | Gap consolidation, boundary/environment freeze, mechanism closure, secure review, submission skeleton, and pre-consultation rehearsal | Submission-facing artifacts are prepared from reviewed evidence and retained external decisions, not ad-hoc logs. |

## Default task loop

Each task should follow this loop:

1. Confirm scope and naming.
   - Product spelling: `AnoCrypto-C`.
   - Repository slug: `anocrypto-c`.
   - CMake package: `AnoCryptoC`.
   - Target: `AnoCryptoC::AnoCryptoC`.
   - Library output: `anocrypto_c`.
   - Public C prefix: `ano_crypto_`.
   - Macro prefix: `ANO_CRYPTO_`.

2. Change the smallest useful surface.
   - Prefer documents, tests, and evidence plumbing before expanding public ABI.
   - Do not expose public struct layouts unless there is an explicit ABI reason.
   - Keep output data in output parameters and return `ano_crypto_status_t`.

3. Add or update tests/evidence.
   - Unit/KAT/negative tests when code changes.
   - Install smoke when package behavior changes.
   - PoC smoke when customer-package behavior changes.
   - Public-surface check when naming, public headers, or symbols change.
   - Thread-safety policy check when synchronization code, CSP/key handle behavior, or concurrency wording changes.
   - Entropy/health policy check when provider, failure handling, DRBG seed acquisition, health-test, or hook boundaries change.
   - Entropy operational policy check when source classification, provider probe, restart/fork behavior, or process-boundary handling changes.
   - Random-service policy check when public random lifecycle, provider availability, source-backed ownership, output clearing, or reseed behavior changes.
   - Differential/portability policy check when a reference provider, C representation assumption, endian/alignment handling, or hosted architecture lane changes.
   - Release-signing handoff and verifier regressions when signature subject/encoding, signer identity, timestamp, role separation, or authorization policy changes.

4. Update clean-room and claims records.
   - Record public standards and user-owned documents used as inputs.
   - Record excluded categories without reproducing sensitive content.
   - Do not add secrets, entropy captures, generated secret bytes, private keys, tokens, or customer data.
   - Do not make stronger security or validation claims than the evidence supports.

5. Re-run local validation.
   - Configure.
   - Build.
   - CTest.
   - Public-surface check.
   - Thread-safety policy check.
   - Install smoke.
   - PoC package smoke when enabled.
   - Evidence target.
   - Release-style symbol scan when visibility policy is relevant.

6. Package outputs.
   - Create a full changed ZIP.
   - Create a patch against the previous step.
   - Include one recommended commit title that represents the complete task, because each task is committed as one commit.

## P1-12 placement

P1-12 is the scope-lock planning step after the P1-11 customer PoC package skeleton. It intentionally avoids adding block cipher code. The desired output is a set of review documents and evidence placeholders that make the next implementation step safer.

P1-12 deliverables:

- Development workflow plan.
- Service/algorithm scope matrix.
- Differential validation strategy draft.
- Block cipher and mode selection decision record.
- Side-channel review checklist for key-dependent code paths.
- Customer PoC intake questionnaire.
- Evidence workflow notes that point to the new planning artifacts.


## P1-13 placement

P1-13 is the evidence/package manifest hardening step after the P1-12 scope-lock planning step. It intentionally avoids selecting an SBOM generator, signing tool, signer identity, or key-management policy. The output is a set of placeholder status files and hash manifests that make later release-candidate packaging easier to review.

P1-13 deliverables:

- SBOM/signing plan.
- Evidence manifest policy.
- Customer PoC package review manifest.
- Evidence workflow outputs for `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST_README.txt`, and `EVIDENCE_MANIFEST.sha256`.
- PoC package outputs for `PACKAGE_REVIEW_MANIFEST.md`, `SBOM_STATUS.txt`, and `SIGNING_STATUS.txt`.
- PoC package smoke check for required package records.

## P1-14 placement

P1-14 is the thread-safety support-matrix step after the P1-13 evidence/package manifest hardening step. It does not add new cryptographic algorithms or widen the public ABI. The output is a support-boundary draft, CI checklist, policy check target, evidence output, and PoC package note that make current global-state locking easier to review.

P1-14 deliverables:

- Thread-safety support matrix.
- Thread-safety CI checklist.
- Thread-safety policy check target.
- Evidence output for `THREAD_SAFETY_CHECK.txt`.
- PoC package output for `THREAD_SAFETY_PACKAGE_NOTES.txt`.
- CMake guard so the POSIX concurrent CSP/HMAC test only runs when thread safety is enabled and pthread support is available.


## P1-15 placement

P1-15 was the historical SHA-384/SHA-512 reservation step after P1-14. P1-22 supersedes the active reserved behavior by implementing the existing identifiers together with KAT, negative-test, package, and evidence wiring.

P1-15 deliverables:

- SHA-384/SHA-512 scope decision document.
- SHA-384/SHA-512 KAT/evidence plan.
- Public hash digest/block size metadata macros.
- Historical reserved-API negative test, removed after the P1-22 implementation transition.
- P1-22 SHA-512-family implementation policy check target.
- Evidence outputs for `SHA512_FAMILY_CHECK.txt` and `SHA512_FAMILY_REVIEW_NOTES.txt`.
- PoC package output for `SHA512_FAMILY_PACKAGE_NOTES.txt`.

## P1-17 placement

P1-17 is the entropy provider failure-injection and health-test skeleton step after the P1-16 reserved public random boundary. It does not enable customer-facing random generation or select a finalized noise-source health-test policy.

P1-17 deliverables:

- Internal deterministic provider success/failure/partial-write hook guarded by a dedicated test-build option.
- Source failure, partial output, zero-length, and oversized-request negative tests.
- Internal bounded startup/continuous duplicate-block health-test candidates.
- Fatal health failure transition to sticky module `ERROR_STATE` and DRBG context invalidation coverage.
- Dedicated entropy failure CI lane and entropy/health policy check.
- Evidence outputs and PoC clean-build checks proving hooks/internal symbols are not part of the customer SDK.
- Clean-room records and explicit TODOs for source-specific health testing.

## P1-18 placement

P1-18 is the entropy source and operational policy step after the P1-17 failure-injection/health skeleton. It classifies OS APIs as external conditioned random providers, separates raw-noise health ownership from provider output, adds a status-only operational probe, and adds a private process-change reseed guard for source-backed DRBG state. At that step, public random generation was still deferred.

P1-18 deliverables:

- Entropy source class, health-owner, and restart-policy metadata in internal headers only.
- OS-provider operational probe that clears temporary storage and records status only.
- Explicit rejection of health-test attachment to OS-provider and unclassified callback sources.
- Private process identity guard for internally source-backed HMAC_DRBG contexts.
- POSIX fork regression plus deterministic process-marker tests.
- Entropy operational policy check target, CI checklist, evidence records, and PoC package boundary note.
- Public symbol/header scan showing no new internal policy surface.
- Clean-room records for public standards and platform documentation used as design inputs.

## P1-19 placement

P1-19 is the public random-service implementation step after the P1-18 entropy operational gate. It changes behavior behind the P1-16 reserved symbols but does not add an exported public function or expose internal provider/process metadata.

P1-19 deliverables:

- Platform entropy-backed HMAC_DRBG-SHA-256 implementation for `ano_crypto_random_*`.
- Caller-owned opaque context size/init/generate/reseed/uninit lifecycle.
- Source-backed ownership marker that rejects explicit-entropy DRBG contexts.
- Output clearing on random-generation failures and bounded request size.
- Process-change/fork regression requiring a successful child reseed.
- Provider-disabled and deterministic failure/partial-write lanes.
- Installed-package and customer PoC random lifecycle smoke without byte output.
- Random-service policy, evidence, CI, package, and clean-room records.
- Public symbol-name comparison against the P1-18 baseline.


## P1-20 placement

P1-20 hardens the implemented public random lifecycle without adding an algorithm or exported public function. It adds bounded long-running lifecycle coverage, deterministic reseed-limit/failure recovery, fail-closed process-marker availability, explicit VM snapshot/clone/suspend guidance, runner-specific status-only review artifacts, and an ABI-0 clean Release export baseline.

P1-20 deliverables:

- Repeated init/generate/reseed/uninit stress with transient output zeroization.
- Dedicated test-only reseed-counter control compiled out of release and PoC builds.
- Reseed-required boundary, failed-reseed persistence, successful recovery, and overflow regression.
- Public random availability requiring both a reviewed provider and process marker.
- Operational guidance for fork, VM restore/clone, suspend/resume, and duplicated contexts.
- Stored 69-symbol ABI-0 export baseline and clean Release comparison target.
- Hosted runner status artifacts that contain no generated bytes or seed material.
- Policy, evidence, PoC, CI, and clean-room updates.


## P1-21 placement

P1-21 hardens the existing global-state/CSP concurrency boundary without adding an algorithm or exported public function. It replaces the pthread-only concurrency test with a Win32/pthread implementation, adds a status-only thread-platform artifact, selects Linux GCC ASan/UBSan as the required diagnostic lane, and records the build-identity ABI-0 field order.

P1-21 deliverables:

- Cross-platform per-thread and shared-handle HMAC concurrency regression.
- Internal thread backend availability/name metadata for tests and evidence only.
- `ANO_CRYPTO_SANITIZER_MODE` with `none` and `address-undefined` values.
- Required hosted Linux GCC ASan/UBSan full CTest lane.
- Explicit TODO for Clang sanitizer runtime and ThreadSanitizer review.
- ABI-0 build-identity layout baseline and runtime/offset regression.
- Concurrency/sanitizer policy, evidence, PoC, CI, and clean-room records.


## P1-22 placement

P1-22 implements SHA-384/SHA-512 and their HMAC variants behind the existing public algorithm identifiers without adding exported functions. It moves the historical P1-15 reservation into the active engineering slice together with KAT, negative, package, and evidence coverage.

P1-22 deliverables:

- SHA-384/SHA-512 one-shot and streaming implementation.
- HMAC-SHA-384/HMAC-SHA-512 raw-key and CSP-handle paths.
- Public-source startup/extended KATs and negative lifecycle tests.
- Install/PoC smoke across all implemented SHA-2/HMAC variants.
- SHA-512-family policy/evidence records and unchanged 53-symbol export baseline.

## P1-23 placement

P1-23 adds differential-validation automation and C representation/alignment portability hardening without changing the product algorithm or public ABI surface. OpenSSL 3.x is test-only and must remain outside the installed SDK and PoC runtime boundary.

P1-23 deliverables:

- Deterministic OpenSSL 3.x differential test for SHA-256/384/512 and HMAC variants.
- One-shot/streaming, padding/block, long-key, larger-message, and intentionally unaligned coverage.
- C99 compile guards for byte and integer widths plus status-only endian/alignment review.
- Dedicated hosted differential job and Linux GCC `-m32` portability workflow.
- Differential/portability policy target, evidence logs/template, provider provenance, and clean-room records.
- PoC/install checks that exclude OpenSSL headers/libraries, differential sources, reference outputs, and runtime linkage.
- Clean Release comparison matching the stored 69-symbol ABI-0 baseline.

## Planned next sequence

| Step | Candidate name | Scope |
|---|---|---|
| P1-15 | SHA-384/SHA-512 reserved scope decision | Completed as reserved-scope/KAT-plan guardrail without implementation. |
| P1-16 | Public random API decision | Completed as a reserved fail-closed boundary that was subsequently implemented in P1-19 without adding public symbols. |
| P1-17 | Entropy failure injection and health-test skeleton | Completed as internal testability/state-transition plumbing; final source-specific policy remains TODO. |
| P1-18 | Entropy source and operational policy review | Completed as source classification, external-provider health ownership, status-only probe, and source-backed process-change reseed guard; direct noise-source thresholds remain TODO. |
| P1-19 | Public random-service implementation | Completed using the existing public symbols, platform entropy-backed HMAC_DRBG-SHA-256, source-backed ownership checks, fork/reseed handling, failure injection, and package/evidence updates. |
| P1-20 | Random lifecycle and portability hardening | Completed with stress and reseed-limit recovery tests, fail-closed marker policy, VM lifecycle guidance, hosted status artifacts, and clean Release ABI-0 symbol checks. |
| P1-21 | Concurrency and sanitizer hardening | Completed with Win32/pthread CSP/HMAC coverage, status-only thread artifacts, a required GCC ASan/UBSan lane, and build-identity ABI-0 layout decision. |
| P1-22 | SHA-384/SHA-512 family implementation | Completed with existing public identifiers, startup/extended KATs, negative tests, install/PoC smoke, and unchanged 53-symbol export baseline. |
| P1-23 | SHA-512-family differential validation and portability | Completed with OpenSSL 3.x test-only deterministic comparisons, C99 representation/alignment checks, hosted artifact wiring, and unchanged public ABI. |
| P1-24 | AES-CBC implementation entry gate | Completed as a no-code selection/API/KAT/CSP/evidence gate. |
| P1-25 | AES-CBC implementation | Completed with AES-128/192/256, CBC without padding, dedicated CSP purpose, KAT/negative/differential/package evidence, and an intentional 61-symbol baseline. |
| P1-26 | AES-CBC hardening | Completed with private-state guards, lifecycle/concurrency/portability regression, expanded differential coverage, and compiler-output capture. |
| P1-27 | AES-CMAC implementation entry gate | Completed as a no-code scope/API/KAT/CSP/evidence gate. |
| P1-28 | AES-CMAC implementation | Completed with AES-128/192/256, full tags, dedicated CMAC-purpose handles, KAT/negative/differential/package evidence, and an intentional 69-symbol baseline. |
| P1-29 | AES-CMAC hardening | Completed with private-state/failure/cleanup/alias hardening, lifecycle/concurrency regression, 1,728 differential paths, and compiler-output capture. |
| P1-30 | Release compatibility baseline and hosted artifact manifests | Completed by freezing the P1-29 public contract, testing old/current headers against the P1-30 package, and requiring manifest-backed hosted review without adding public services. |
| P1-31 | Unsigned release provenance and source SBOM gate | Completed with exact-source SPDX 2.3 JSON inventory, hash-bound unsigned provenance, signing/authorization TODO records, hosted provenance lane, negative regressions, and no public API growth. |
| P1-32 | Offline release-signing handoff | Completed with an Ed25519/RFC 3161 verifier policy, key-free handoff bundle, role separation, opt-in ephemeral Debug fixtures, and fail-closed unprovisioned production state. |
| P1-33 | Clean tagged-release rehearsal and hosted artifact review | Completed with an isolated synthetic tag rehearsal, clean/moved identity checks, explicit hosted lane records, actual/synthetic separation, and fail-closed review regressions. |
| P2-01 | Readiness gap consolidation and pre-consultation pack | Completed with traceability, gap status, document inventory, blocker record, and P2-02 through P2-08 roadmap; no public ABI change. |
| P2-02 | Module boundary and operating environment freeze | Completed with one shared-library candidate, OE-01 configuration record, explicit exclusions, fail-closed CMake enforcement, and no public ABI change. |

## Stop conditions

Stop adding code and update docs first when any of the following happen:

- A public API needs a new exposed struct layout.
- A new algorithm changes the candidate validation boundary.
- A test vector source is unclear.
- A customer request mixes AnoCrypto-C, AnoSecureKit, or AnoPKI responsibilities.
- A log or sample risks printing secret material, entropy bytes, generated secret bytes, private keys, tokens, or customer data.
- A marketing or security statement is stronger than the current evidence.

## P1-29 placement

P1-29 is the AES-CMAC lifecycle and compiler-review hardening step after the
P1-28 implementation. It retains the public API and exported-symbol baseline.

P1-29 deliverables:

- CMAC private-context integrity validation and fail-closed reinitialization;
- overlap and failure-output policy regression;
- bounded cleanup behavior for short and oversized caller lengths;
- 128-cycle lifecycle stress and shared CMAC-purpose handle concurrency;
- 1,728 test-only OpenSSL comparison paths;
- CMAC compiler-output capture and human-review plan;
- policy, evidence, PoC, CI, roadmap, and clean-room alignment.

## P1-31 placement

P1-31 is the unsigned release-provenance and source-inventory step after the P1-30 compatibility baseline. It adds no cryptographic algorithm, public function, header, or symbol.

P1-31 deliverables:

- exact source-archive file list and SHA-256 source manifest;
- deterministic SPDX 2.3 JSON source inventory with explicit scope limitations;
- self-contained unsigned provenance bundle and complete hash manifest;
- false-signing, false-authorization, tamper, path, tag, and hosted-disposition regression;
- hosted provenance bundle/index wiring and review template;
- explicit PoC exclusion of internal source/SBOM/signing artifacts;
- unchanged P1-29 public contract and 69-symbol ABI-0 line.

P1-31 does not infer that a tag string, checksum, source SBOM, or uploaded artifact is authenticated or externally approved.

## P1-32 placement

P1-32 is the offline release-signing handoff step after the P1-31 unsigned provenance gate. It adds no cryptographic service, public function, header, or symbol. It defines and verifies release-signing metadata while keeping all production credentials and outputs outside the repository and CI.

P1-32 deliverables:

- exact `RELEASE_ARTIFACTS.sha256` signing subject;
- Ed25519 raw 64-byte detached-signature and SHA-256 SPKI DER fingerprint policy;
- required RFC 3161 timestamp over the detached signature;
- key-free handoff bundle creation and complete manifest/policy verification;
- distinct Security Reviewer and Product Release Owner identity records and same-identity rejection;
- optional secondary-only GitHub artifact attestation classification;
- key-free negative tests plus explicit opt-in Debug-only ephemeral OpenSSL fixtures that remove generated signing/timestamp material;
- production signer/TSA state recorded as unprovisioned, signing as `UNSIGNED`, external release as `NO`, and handoff readiness as `BLOCKED`;
- PoC exclusion of internal provenance/handoff records, keys, tokens, signatures, and timestamp responses.

Actual hosted disposition, tag execution, production signer/TSA provisioning, detached signature/timestamp generation and verification, distinct human approvals, and external-release authorization remain future release operations.

## P1-33 placement

P1-33 follows the P1-32 offline signing handoff. It adds no cryptographic service or public API. It rehearses release identity in an isolated clean checkout and makes hosted review state explicit without creating a real tag or claiming a hosted run.

P1-33 deliverables:

- synthetic local tag in a temporary clean repository only;
- tag, supplied commit, and `HEAD` equality plus clean-worktree enforcement;
- dirty, moved-tag, moved-HEAD, mismatch, unsafe-artifact, and false-approval negative tests;
- fixed required lane/platform review model with artifact and workflow identity metadata;
- explicit `HOSTED_RUNNER` versus `SYNTHETIC_LOCAL` origin and `NOT_RUN`/`UNREVIEWED` state;
- clean-checkout build, CTest, install, PoC, compatibility, provenance, signing-handoff, and evidence reproduction;
- unchanged 14-header, 69-symbol `ANOCRYPTO_C_0` boundary.

Actual hosted review, user-repository tag creation, tag signing, production signing/timestamping, and external release remain blocked operations.


## P2-01 placement

P2-01 follows the P1-33 engineering rehearsal while P1-34 remains externally blocked. It adds no cryptographic algorithm or public API. It consolidates the current state into submission-oriented planning without representing local or synthetic records as actual hosted execution.

P2-01 deliverables:

- current implementation/test/evidence inventory and gap classification;
- requirement-to-code-to-test-to-evidence matrix;
- pre-consultation question pack;
- submission-document inventory;
- P2-02 through P2-08 roadmap and completion gates;
- durable P1-34 billing-blocker record;
- unchanged 14-header, 69-symbol `ANOCRYPTO_C_0` public boundary.

P2-02 is completed as an internal freeze. P2-03 is the next codebase work item. P1-34 resumes only after GitHub billing is resolved and remains a separate actual-hosted-evidence lane.

## P2-02 placement

P2-02 converts the broad P2-01 boundary/environment gaps into one controlled internal candidate:

- `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`;
- Ubuntu Server 24.04.4 LTS x86_64, GCC 13.3.0, CMake 3.28.3;
- Release shared library with thread safety and Linux OS entropy enabled;
- tests, examples, hooks, OpenSSL differential code, signing fixtures, sanitizers, LTO, and native CPU tuning excluded;
- static linkage and all non-selected operating environments deferred or unsupported for the initial candidate.

The candidate is a configuration-management decision, not a validation result. Exact selected-environment execution, immutable image/CPU/kernel/package evidence, institution confirmation, integrity design, and hosted artifact review remain open. P2-03 must now define self-test, module integrity, finite-state, service availability, failure classification, and recovery/operator actions against this boundary.
