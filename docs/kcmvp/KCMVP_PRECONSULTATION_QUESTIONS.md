# KCMVP Pre-consultation Questions

Status: P2-02 question pack against internally frozen candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`. All proposed answers remain provisional until retained external confirmation is received.

## Use and Recording Rule

For every consultation response, retain:

- institution/organization and respondent role;
- question ID and exact answer or referenced document/version;
- date, scope assumptions, and confidentiality classification;
- decision owner and resulting repository/document action;
- unresolved ambiguity marked `EXTERNAL_CONFIRMATION_REQUIRED`.

Do not record keys, entropy samples, customer data, private infrastructure details, signing credentials, or unpublished exploit material.

## Priority A — Confirm the Frozen Boundary and Environment Before P2-03

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-001 | Testing institution | For the software candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`, is the code and module-owned state in `libanocrypto_c.so.0.1.0` an acceptable logical/physical boundary, with headers, loader, libc, kernel, applications, tests, tools, and PoC outside? | Freezes the physical/logical boundary. | The internal candidate is shared-library only; tests, OpenSSL, hooks, tools, PoC, AnoSecureKit, AnoPKI, and signing infrastructure are outside. Formal acceptance is pending. |
| PCQ-002 | Testing/validation institution | Should static and shared builds be treated as separate module variants, and should the initial scope exclude static linkage? | Avoids ambiguous binaries and duplicated evidence. | Prefer one shared-library configuration first; static remains deferred. |
| PCQ-003 | Testing institution | What exact OS, CPU, compiler, compiler-version, and build-option combinations must be declared as separate operating environments or module variants? | Defines P2-02 evidence targets and cost. | OE-01 is internally frozen to Ubuntu Server 24.04.4 LTS x86_64, GCC 13.3.0, CMake 3.28.3, Release shared build. Immutable image/CPU/kernel/package capture and actual candidate execution are pending. |
| PCQ-003A | Testing institution | For Ubuntu 24.04 LTS, which kernel patch level, glibc package, CPU model/microcode, VM/container/hypervisor, and OS image identifiers must be separately declared or retested? | Determines the immutable evidence boundary and future maintenance cost. | Current plan captures all values per run but treats the 24.04/GCC13 family as one internal candidate until advised otherwise. |
| PCQ-003B | Testing institution | Is GCC 13.3.0 plus generic x86-64, LTO OFF, and no `-march=native` sufficiently specific, and which compiler/linker/package updates require a new candidate or change-impact review? | Freezes compiler-output and maintenance policy. | Any compiler or optimization drift currently requires reviewed candidate revision. |
| PCQ-004 | Testing institution | Can the initial scope be limited to SHA-256/384/512, HMAC-SHA-2, HMAC_DRBG-SHA-256, AES-128/192/256 CBC without padding, and AES-CMAC full tag? | Prevents feature expansion before scope acceptance. | Prefer the implemented narrow scope; no new algorithm/API in P2. |
| PCQ-005 | Testing institution | Must a reserved but unimplemented algorithm identifier such as Hash_DRBG be removed from the submitted configuration, or may it remain as an unsupported enum value? | Affects submitted API description without requiring an ABI break. | Keep public ABI unchanged and describe it as unsupported unless the institution requires a configuration-specific exclusion. |
| PCQ-006 | Testing/validation institution | Which target security level and assurance interpretation are appropriate for this software module and commercial plan? | Drives all document and mechanism requirements. | No level is treated as accepted until confirmed. |
| PCQ-007 | Testing institution | Are AnoSecureKit/PKI workflows correctly outside the candidate cryptographic module, with only primitive cryptographic services in scope? | Preserves product-layer separation. | Current answer is yes, subject to confirmation. |

## Priority B — Self-test, Integrity, and Error State

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-008 | Testing institution | Which implemented algorithms/services require startup KATs, and which require conditional tests on key generation, instantiation, reseed, or other events? | Finalizes P2-03 test classification. | Startup KATs exist for all implemented families; conditional classification is incomplete. |
| PCQ-009 | Testing institution | What module-integrity mechanisms are acceptable for the selected shared-library environments, and what key/verification material may reside inside or outside the module? | Current integrity check is not a final mechanism. | Select only after boundary and operating environments are frozen. |
| PCQ-010 | Testing institution | Is integrity verification required at every load/startup, and how should dynamically loaded libraries, package managers, and signed OS binaries affect the design? | Determines startup flow and deployment guidance. | `EXTERNAL_CONFIRMATION_REQUIRED`. |
| PCQ-011 | Testing institution | Which failures must enter a persistent fatal state, which may remain operation-local, and what restart or reinstallation action is expected? | Finalizes state machine and operator guidance. | Current fatal self-test/selected health failures are sticky until process restart. |
| PCQ-012 | Testing institution | Which non-cryptographic queries may remain available in fatal state, such as version, build identity, state, and last-error queries? | Prevents service-availability mismatch. | Current state/status/build/version queries are candidates; exact set needs approval. |
| PCQ-013 | Testing institution | May shutdown acknowledge a sticky fatal state without clearing it, and is process termination/reload an acceptable recovery boundary? | Matches current implementation behavior. | Current module does not recover from fatal state in-process. |

## Priority C — Entropy and DRBG

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-014 | Testing institution / entropy specialist | For OE-01, may Linux `getrandom()` be treated as an external conditioned OS provider whose noise-source health tests remain outside AnoCrypto-C, and what kernel/provider evidence is required? | Determines whether a direct noise source is required. | OE-01 uses the internal `linux-getrandom` adapter; AnoCrypto-C does not claim ownership of kernel noise sources or apply its raw-noise engineering skeleton to OS output. |
| PCQ-015 | Testing institution | Is a status-only operational provider probe acceptable, and what failure, retry, and zeroization evidence is required? | Finalizes provider startup/operation policy. | Status-only probe exists; no provider bytes are logged. |
| PCQ-016 | Testing institution / entropy specialist | If a direct noise source becomes necessary, which startup/continuous tests, thresholds, sample sizes, failure policy, and recovery behavior are expected? | Avoids inventing unsupported thresholds. | No direct source has been selected; remain `EXTERNAL_CONFIRMATION_REQUIRED`. |
| PCQ-017 | Testing institution | Are current HMAC_DRBG instantiate/reseed/generate limits and explicit reseed-required behavior suitable, and which exact limits must be documented/tested? | Closes P2-04 lifecycle evidence. | Consolidate after accepted limits are confirmed. |
| PCQ-018 | Testing institution | Is prediction resistance required for any submitted service, or may requests remain unsupported? | Prevents accidental API or entropy expansion. | Current public behavior rejects unsupported prediction-resistance requests. |
| PCQ-019 | Testing institution | Is fork/process-change detection with mandatory reseed sufficient on POSIX targets, and how should unsupported process-marker environments be declared? | Defines supported process model. | Current source-backed random service fails closed when safe marker behavior is unavailable. |
| PCQ-020 | Testing institution | What policy and evidence are expected for VM clone/snapshot, container cloning, suspend/resume, and hibernation? | These are current integration gaps. | Prefer explicit unsupported/consumer-action wording unless a testable platform mechanism is selected. |

## Priority D — CSP, Zeroization, and Side-channel Review

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-021 | Testing institution | Which values in the current implementation must be listed as CSPs, including imported keys, expanded schedules, HMAC pads, CMAC subkeys, DRBG V/K, entropy buffers, and intermediate state? | Defines formal CSP inventory. | P2-05 will enumerate conservatively and request confirmation. |
| PCQ-022 | Testing institution | Is in-memory-only, non-exportable key storage with purpose-separated opaque handles an acceptable initial scope? | Avoids persistence/wrapping expansion. | Export, persistence, wrapping, and platform keystore integration remain outside scope. |
| PCQ-023 | Testing institution | What evidence is accepted for zeroization under optimizing compilers, and are compiler output plus lifecycle/failure tests sufficient as engineering inputs? | Defines closure criteria. | Target/compiler-specific human disposition is planned; universal guarantees are not claimed. |
| PCQ-024 | Testing institution | What process/OS memory behavior is explicitly outside module zeroization responsibility after buffer clearing or process termination? | Prevents overclaiming OS/hardware behavior. | Requires operating-environment wording. |
| PCQ-025 | Testing institution / side-channel specialist | What side-channel review depth is expected for table-free portable AES, HMAC/DRBG, CMAC comparison, and compiler-specific output at the selected level? | Sizes P2-06 and external review. | Compiler captures exist; timing/cache/fault review remains incomplete. |
| PCQ-026 | Testing institution | Are shared read-only key handles with internal locking acceptable, and what caller serialization rules must be documented for mutable contexts? | Freezes thread-safety model. | Global state and handle tables are locked; mutable operation contexts are caller-owned. |

## Priority E — Tests, Evidence, and Submission Package

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-027 | Testing institution | Which current templates and document revisions should be used for basic design, detailed design, configuration management, and test documents? | Prevents drafting against stale forms. | P2-07 creates controlled skeletons only after confirmation. |
| PCQ-028 | Testing institution | What source, binary, build environment, toolchain, symbol/map, disassembly, and test-vector materials are expected at initial application versus later testing? | Defines submission staging and confidentiality handling. | Current inventory separates internal evidence from customer PoC and public SDK. |
| PCQ-029 | Testing institution | How should requirement-to-code-to-test traceability be structured, and is a repository path plus test/evidence identifier sufficient? | Validates the P2 matrix format. | Current matrix is path-based and status-based. |
| PCQ-030 | Testing institution | Which independent algorithm conformance tests will be run, and which vendor KAT/differential records are useful supporting evidence? | Avoids treating local KATs as institution results. | Local KAT/differential evidence is engineering support only. |
| PCQ-031 | Testing institution | Are actual Windows/macOS/32-bit/big-endian results required for initial scope, or may unsupported/non-target environments remain excluded? | Resolves hosted-evidence priorities. | No actual hosted result exists while P1-34 is blocked. |
| PCQ-032 | Testing institution | What reviewer identity, date, disposition, artifact hash, and retention metadata are expected for vendor evidence? | Aligns evidence manifest policy. | Current model records these fields but lacks actual hosted review. |
| PCQ-033 | Testing institution | Is a source-only SPDX inventory useful, and what dependency/runtime/toolchain/license/vulnerability inventory is expected? | Defines SBOM scope without overstating completeness. | Current SPDX file inventories exact source files only. |
| PCQ-034 | Testing institution | What configuration-management records are required for branch protection, commit/tag control, build reproducibility, issue/change approval, and archive retention? | Closes P2-07 configuration-management document. | Existing provenance and compatibility tooling are inputs, not a complete submitted procedure. |
| PCQ-035 | Testing institution | What secure channel and redaction rules apply when sharing source, evidence, compiler output, or test artifacts during pre-consultation? | Protects proprietary code and sensitive metadata. | Use minimum necessary, status-only records unless a protected submission channel is confirmed. |

## Priority F — Release and Commercial Boundary

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-036 | Testing/validation institution | At what point may the module/version be externally described as under test, accepted, or listed, and what exact wording is permitted before then? | Prevents premature claims. | Continue using candidate/readiness wording only. |
| PCQ-037 | Testing institution | Must the submitted binary be signed/timestamped before testing, and what signer identity or chain requirements apply? | Aligns offline signing handoff with the actual process. | Production signer and TSA remain unprovisioned; no signing action is authorized. |
| PCQ-038 | Testing institution | Can customer evaluation PoC packages be distributed before completion, and what notices or configuration restrictions should accompany them? | Connects validation boundary to business/legal handling. | Technical PoC exists; legal/evaluation terms remain TODO. |
| PCQ-039 | Testing institution | How are post-submission bug fixes, compiler updates, OS patches, and ABI-preserving changes classified for retest or change review? | Defines maintenance branch and configuration management. | P2-07 should include change-impact classification after confirmation. |
| PCQ-040 | Testing/validation institution | Does the current planned separation of testing institution review and validation institution decision match the current process, and which questions must be escalated to the validation institution? | Routes questions correctly. | Record the responsible authority for each answer. |

## Consultation Exit Criteria

Pre-consultation is complete only when the project has retained answers sufficient to:

1. obtain a retained acceptance, revision, or rejection of the internally frozen shared-library boundary and OE-01 environment;
2. select a self-test/integrity/error-state design;
3. assign entropy-source and health-test responsibility;
4. define CSP/zeroization and side-channel evidence expectations;
5. select current submission templates and package contents;
6. convert remaining ambiguity into an owned, dated decision register.
