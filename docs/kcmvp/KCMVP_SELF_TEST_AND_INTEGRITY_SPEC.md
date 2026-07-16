# KCMVP Self-test and Module-integrity Specification

Status: `P2-03 FROZEN_INTERNAL_DESIGN`; integrity mechanism is `SELECTED_CANDIDATE_NOT_IMPLEMENTED` and remains `EXTERNAL_CONFIRMATION_REQUIRED`.

Document ID: `AC-KCMVP-STI-P2-03-R1`

## 1. Purpose and claims boundary

This document defines the intended self-test, integrity, fatal-state, and recovery behavior for the frozen validation candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`.

It is an internal design baseline for consultation and implementation planning. It does **not** state that:

- the selected integrity design has been accepted by a test or validation institution;
- a runtime module-integrity test is implemented;
- the placeholder public flag proves an integrity test ran;
- official validation, audit, or production release authorization is complete.

The current runtime has startup algorithm KATs, build-identity and KAT-manifest structural checks, an operational service gate, and a sticky process-local error state. It does not yet authenticate the installed shared-object bytes.

## 2. Normative terms and status

The keywords `MUST`, `MUST NOT`, `SHOULD`, and `MAY` describe the internal target design. Items marked `CURRENT` describe observed code. Items marked `PLANNED_IMPLEMENTATION` are not yet implemented. Items marked `EXTERNAL_CONFIRMATION_REQUIRED` depend on institution guidance.

| Area | P2-03 disposition |
|---|---|
| Startup algorithm KAT sequence | `CURRENT / READY_FOR_INTERNAL_REVIEW` |
| Build-identity and KAT-manifest structural checks | `CURRENT / SUPPORTING_CHECKS` |
| Module-integrity authentication | `SELECTED_CANDIDATE_NOT_IMPLEMENTED` |
| Startup service gating | `CURRENT / READY_FOR_INTERNAL_REVIEW` |
| Sticky fatal error state | `CURRENT / READY_FOR_INTERNAL_REVIEW` |
| Orderly shutdown then in-process reinitialization | `CURRENT / FROZEN_INTERNAL_POLICY` |
| In-process recovery from fatal error | `PROHIBITED_INTERNAL_POLICY` |
| Public on-demand self-test state transition | `PLANNED_IMPLEMENTATION` |
| Entropy/direct-noise-source conditional tests | `P2-04 / EXTERNAL_CONFIRMATION_REQUIRED` |
| Institution acceptance of test classification and integrity mechanism | `EXTERNAL_CONFIRMATION_REQUIRED` |

## 3. Test classification

### 3.1 Startup checks and self-tests

`ano_crypto_module_init(NULL)` MUST transition from `UNINITIALIZED` or `SHUTDOWN` to `SELF_TEST_RUNNING`. Cryptographic services MUST remain unavailable until all mandatory checks complete successfully.

Target execution order:

| Order | ID | Classification | Current status | Failure disposition |
|---:|---|---|---|---|
| 1 | `ST-INT-001` | Module-integrity authentication over the exact candidate shared object | `NOT_IMPLEMENTED` | `ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED`, sticky `ERROR_STATE` |
| 2 | `ST-STR-001` | Build-identity structure and required field sanity | `CURRENT` | sticky `ERROR_STATE` |
| 3 | `ST-STR-002` | KAT-manifest structure and required field sanity | `CURRENT` | sticky `ERROR_STATE` |
| 4 | `ST-KAT-SHA2` | SHA-256, SHA-384, SHA-512 known-answer tests | `CURRENT` | `ANO_CRYPTO_ERR_SELF_TEST_FAILED`, sticky `ERROR_STATE` |
| 5 | `ST-KAT-HMAC` | HMAC-SHA-256, HMAC-SHA-384, HMAC-SHA-512 known-answer tests | `CURRENT` | same |
| 6 | `ST-KAT-AES-CBC` | AES-128/192/256 primitive and CBC-path known-answer tests | `CURRENT` | same |
| 7 | `ST-KAT-CMAC` | AES-CMAC full-tag known-answer test | `CURRENT` | same |
| 8 | `ST-KAT-DRBG` | HMAC_DRBG-SHA-256 known-answer test | `CURRENT` | same |

`ST-STR-*` checks support startup diagnostics but MUST NOT be described as cryptographic module-integrity authentication.

### 3.2 Conditional tests

P2-03 does not create conditional tests that are not applicable to the frozen service scope.

| Event/service | Conditional-test disposition | Reason/owner |
|---|---|---|
| Public-key generation or key-pair consistency | `OUT_OF_SCOPE` | No public-key algorithm or key-generation service exists. |
| Symmetric/HMAC/CMAC key import | `NOT_A_SELF_TEST` | Length, purpose, stale-handle, and lifecycle checks are service validation; P2-05 owns CSP evidence. |
| HMAC_DRBG instantiate/reseed/generate | `OPERATIONAL_CHECKS_PRESENT` | Provider/reseed/process guards are not reclassified as KATs. P2-04 owns final limits and failure policy. |
| Direct physical noise source health tests | `NOT_APPLICABLE_CURRENT_CANDIDATE / EXTERNAL_CONFIRMATION_REQUIRED` | OE-01 consumes the Linux OS RNG and has no project-owned direct noise source. P2-04 owns final interpretation. |
| OS entropy provider failure | `P2-04` | Fatal versus retry/reseed treatment must be finalized with provider responsibility. |
| Module load or explicit on-demand request | `STARTUP_OR_ON_DEMAND_SELF_TEST` | Target behavior is described below. |

### 3.3 On-demand self-test

The existing public symbol `ano_crypto_selftest_run(uint32_t flags)` is retained without API or ABI change. The P2-03 target semantics are:

1. In `OPERATIONAL`, an accepted on-demand request transitions the module to `SELF_TEST_RUNNING`, blocks cryptographic services, runs the same mandatory integrity/structural/KAT sequence, and returns to `OPERATIONAL` only on complete success.
2. In `UNINITIALIZED` or `SHUTDOWN`, a direct on-demand request returns `ANO_CRYPTO_ERR_NOT_INITIALIZED` and does not run internal cryptographic checks.
3. In `ERROR_STATE`, it returns `ANO_CRYPTO_ERR_FATAL_STATE`; it cannot recover the process.
4. A concurrent or re-entrant request while `SELF_TEST_RUNNING` is rejected. The implementation owner must use a non-fatal busy status consistent with the existing status ABI; the proposed status is `ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED`.
5. Any on-demand integrity or KAT failure enters sticky `ERROR_STATE`.

Current code runs the check sequence without managing these on-demand state transitions. This is `PLANNED_IMPLEMENTATION`, not a completed behavior.

### 3.4 Public self-test flags

The numeric values of the four existing flags are ABI-stable and remain unchanged.

- `ANO_CRYPTO_SELFTEST_FLAG_DEFAULT` and `ANO_CRYPTO_SELFTEST_FLAG_STARTUP` identify request context; they do not select a reduced test set.
- `ANO_CRYPTO_SELFTEST_FLAG_KAT_PLACEHOLDER` and `ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER` are legacy ABI names. Their presence in `last_flags` MUST NOT be presented as evidence that a KAT or integrity mechanism exists beyond the actually executed sequence.
- The target implementation MUST reject bits outside the published mask with `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- Mandatory startup and on-demand checks MUST NOT be disabled by caller flags.

## 4. Selected candidate integrity mechanism

### 4.1 Candidate identity

Candidate ID: `INT-HMAC-SHA256-SINGLE-SO-R1`

Disposition: `SELECTED_CANDIDATE_NOT_IMPLEMENTED / EXTERNAL_CONFIRMATION_REQUIRED`.

The candidate retains the P2-02 single-file shared-library boundary. It uses a full-length HMAC-SHA-256 value embedded in a dedicated fixed-size internal metadata region of `libanocrypto_c.so.0.1.0`.

### 4.2 Canonical measurement model

The planned build tool creates a fixed-format internal record containing:

- format magic and version;
- validation-candidate ID;
- runtime version;
- measured file length;
- integrity-key identifier;
- full 32-byte expected HMAC tag;
- fixed reserved bytes for forward-compatible internal format control.

The HMAC input is a canonical framing header plus the exact shared-object bytes, with only the expected-tag field treated as zero bytes during calculation. The integrity key and all other metadata bytes are inside the measured content. No companion runtime file, OpenSSL dependency, customer configuration, or network service is introduced.

Runtime verification MUST:

1. resolve the exact loaded shared-object path and reject ambiguous or non-regular-file input;
2. validate record magic, version, offsets, lengths, candidate ID, and runtime version;
3. read and measure the exact file with bounded allocation/streaming and checked conversions;
4. use the internal HMAC-SHA-256 implementation before public services become operational;
5. compare the full tag without early-exit data-dependent comparison;
6. zeroize temporary HMAC state, key copies, and tag buffers;
7. return only status and non-sensitive metadata to evidence;
8. enter sticky `ERROR_STATE` on missing, unreadable, malformed, truncated, modified, or mismatched content.

### 4.3 Key ownership and lifecycle

- The integrity-test key is **not** a release-signing key, timestamp credential, customer key, entropy seed, or general service key.
- A per-validation/release-binary key is generated and injected by a controlled offline build step.
- The key MUST NOT be stored in the source tree, ordinary CI variables, logs, PoC output, evidence, source archives, or signing handoff.
- The runtime binary necessarily contains material sufficient to perform the HMAC verification. This limitation is explicit and requires institution acceptance.
- The key is a module CSP and MUST be entered into the P2-05 CSP inventory with creation, injection, storage, use, temporary-copy, and zeroization paths.
- Key custody, approved build roles, backup/escrow, reuse versus rotation, and destruction require a controlled P2-07 configuration-management procedure.

### 4.4 Deployment and reproducibility implications

- The installed shared object must not be stripped, patched, rewritten, prelinked, or modified after integrity finalization unless the tag is regenerated through the authorized build process.
- Packaging must preserve exact bytes and permissions.
- A per-build secret means independent bit-for-bit reproduction requires authorized access to the same controlled key and inputs; otherwise the rebuild is expected to differ.
- Debug, sanitizer, test-hook, and differential binaries are not the validation candidate and require separate non-production integrity fixtures if implementation tests need them.
- Runtime path resolution and file-read permissions are part of the operating assumptions and must be tested on OE-01.

### 4.5 Alternatives retained for consultation

The following are not selected for implementation in P2-03 but remain consultation alternatives:

| Alternative | Reason not selected now |
|---|---|
| Detached HMAC tag file | Expands the P2-02 single-file delivery/boundary and creates replacement/permission coupling. |
| Embedded public-key signature verification | Would introduce an internal public-key verification algorithm and signing design outside the current no-new-algorithm constraint. |
| Unkeyed SHA-256 digest only | Does not provide the same authentication property and is not assumed acceptable. |
| OS package-manager signature only | External package trust does not directly define the module's own startup self-test behavior. |

The test/validation institution must confirm whether the selected HMAC design, key treatment, one-file canonicalization, and execution order are acceptable. No implementation begins solely because this candidate is documented.

## 5. Fatal and recoverable failure policy

Failure severity is determined by the operation context and state transition, not only by the numeric status name.

### 5.1 Fatal module failures

The following are fatal for the loaded process:

- module-integrity missing/read/parse/length/tag failure;
- any mandatory startup or on-demand algorithm KAT failure;
- required startup structural metadata failure;
- corruption of module-global state or inability to preserve its synchronization invariant;
- a P2-04 entropy/health failure only if that later specification classifies it as fatal.

Fatal handling MUST:

- latch the first fatal status or a separately documented most-severe status policy;
- transition to `ERROR_STATE`;
- reject cryptographic processing with `ANO_CRYPTO_ERR_FATAL_STATE`;
- keep version/build/state/last-error/self-test-result queries available;
- avoid outputting keys, test vectors, computed tags, expected tags, entropy, or intermediate state;
- require process termination and clean reload for recovery.

### 5.2 Recoverable operation failures

Ordinary parameter, buffer, unsupported-service, lifecycle, authentication, caller-data integrity, resource, and reseed-required results do not by themselves enter module `ERROR_STATE`, unless a separate service specification identifies state corruption.

Examples include:

- invalid argument or unsupported parameter;
- insufficient output/context buffer;
- use before initialization or after orderly shutdown;
- stale/destroyed/wrong-purpose CSP handle;
- incomplete block, prohibited overlap, or output-length error;
- `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` when P2-04 allows caller reseed;
- authentication failure on caller-supplied data in a future approved service.

## 6. Restart and operator action

- Calling `ano_crypto_module_init(NULL)` while already `OPERATIONAL` is idempotent and does not rerun startup tests.
- Orderly `ano_crypto_module_shutdown()` transitions to `SHUTDOWN`.
- `SHUTDOWN -> SELF_TEST_RUNNING -> OPERATIONAL` reinitialization is allowed in the same process and reruns the complete startup sequence.
- Shutdown called in `ERROR_STATE` may acknowledge cleanup but MUST NOT clear the fatal latch or report recovery.
- `ERROR_STATE` has no in-process reset, override, retry, or service bypass.
- Operator action after fatal state is: stop the consuming process, preserve status-only diagnostics, investigate the exact artifact/environment, reinstall or replace through an authorized process, then start a fresh process.
- Repeated automatic restart loops without artifact/environment diagnosis are not recommended operator behavior.

## 7. Evidence rules

Permitted records:

- test/check ID and class;
- status code/name;
- module version, candidate ID, artifact path class, file length, and artifact SHA-256 for evidence binding;
- compiler, OS, CPU, commit, reviewer, UTC time, and disposition;
- whether the test ran, passed, failed, or was not implemented.

Prohibited records:

- integrity key, expected or computed HMAC tag;
- algorithm KAT expected/computed values;
- HMAC/DRBG/AES internal state;
- customer keys/data, entropy, random output, IV, plaintext, ciphertext, digest, MAC, or CMAC values;
- production signing or timestamp private material.

## 8. Implementation gate

Implementation of `INT-HMAC-SHA256-SINGLE-SO-R1` requires all of the following:

1. recorded consultation disposition or explicit internal authorization to prototype without claiming acceptance;
2. reviewed binary-format/canonicalization design;
3. key-custody and build-role procedure;
4. tamper, truncation, wrong-file, path, permission, malformed-record, and tag-mismatch tests;
5. selected-OE execution and packaging retention evidence;
6. CSP/zeroization integration with P2-05;
7. secure-coding and compiler review integration with P2-06;
8. unchanged public 14-header/69-symbol `ANOCRYPTO_C_0` boundary unless a separate gate approves otherwise.

## 9. References and external confirmation

- KISA, 암호모듈 시험·검증절차: the applicant selects an appropriate security level, prepares submissions, and the test institution evaluates against module requirements.
- KISA, 암호모듈 제출물 작성 안내서 개정(2025.9.), published 2025-09-17.
- `KS X ISO/IEC 19790:2015` and `KS X ISO/IEC 24759:2015`, as identified by the current KCMVP program materials.

The full standard/guide interpretation is not embedded in this repository. Exact integrity-test acceptance, conditional-test classification, security level, required evidence, and operator wording remain `EXTERNAL_CONFIRMATION_REQUIRED`.
