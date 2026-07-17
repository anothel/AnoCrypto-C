# KCMVP Pre-consultation Questions

Status: P2-06 question pack against frozen boundary candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`, policies `STP-P2-03-R1`, `EDP-P2-04-R1`, `CZP-P2-05-R1`, and secure-coding policy `SCP-P2-06-R1`. All proposed answers remain provisional until retained external confirmation is received.

## Use and Recording Rule

For every consultation response, retain:

- institution/organization and respondent role;
- question ID and exact answer or referenced document/version;
- date, scope assumptions, and confidentiality classification;
- decision owner and resulting repository/document action;
- unresolved ambiguity marked `EXTERNAL_CONFIRMATION_REQUIRED`.

Do not record keys, entropy samples, customer data, private infrastructure details, signing credentials, or unpublished exploit material.

## Priority A — Confirm the Frozen Boundary and Environment

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
| PCQ-008 | Testing institution | Is the P2-03 startup order—module integrity, structural checks, then SHA-2/HMAC/AES-CBC/AES-CMAC/HMAC_DRBG KATs—acceptable, and must any submitted service use a different order or trigger? | Controls the startup gate and test procedure. | Internally frozen; exact institution acceptance and OE-01 retained execution are pending. |
| PCQ-009 | Testing/validation institution | Is `INT-HMAC-SHA256-SINGLE-SO-R1` acceptable for the single shared-object boundary, including an embedded fixed metadata/tag region and runtime verification material inside the measured binary? | Determines whether the selected candidate may proceed to implementation. | Candidate is fully specified but `SELECTED_CANDIDATE_NOT_IMPLEMENTED`; embedded verification material and its security limitations require explicit acceptance. |
| PCQ-010 | Testing institution | Is the proposed canonical measurement acceptable: full shared-object bytes with only the expected-tag field zeroed, fixed metadata validation, exact loaded-path resolution, and failure on missing/unreadable/modified/truncated content? | Freezes the build/runtime integrity contract and test vectors. | No verifier or tag injection exists yet; alternate detached-HMAC or public-key-signature designs remain consultation options. |
| PCQ-011 | Testing institution | Is process termination plus fresh load the acceptable recovery boundary after integrity/KAT/fatal health failure, while orderly shutdown permits full reinitialization in the same process? | Confirms fatal recovery versus normal lifecycle behavior. | P2-03 freezes this policy internally and adds orderly reinitialization regression coverage. |
| PCQ-012 | Testing institution | May state, status, build identity, version, and self-test-result metadata remain queryable in `ERROR_STATE`, while all cryptographic services and recovery self-tests are rejected? | Prevents service-availability mismatch and sensitive diagnostics. | P2-03 matrix permits status-only non-cryptographic queries; exact accepted set requires confirmation. |
| PCQ-013 | Testing institution | For the existing `ano_crypto_selftest_run()` symbol, is the target on-demand behavior acceptable: serialize in `OPERATIONAL`, block services during `SELF_TEST_RUNNING`, reject re-entrancy, and enter sticky error on failure without adding public API? | Determines the implementation and concurrency tests while preserving ABI. | The symbol exists, but the target state transition is not implemented and is tracked as a P2-03 delta. |
| PCQ-013A | Testing institution | Which P2-03 events are formally conditional self-tests rather than service validation—DRBG instantiate/reseed, OS-provider operational checks, symmetric key import, or none beyond startup/on-demand tests for this scope? | Prevents incorrect classification and duplicate requirements. | P2-03 leaves OS entropy/direct-source interpretation to P2-04 and treats key-import checks as service validation. |
| PCQ-013B | Testing/validation institution | Must integrity-test HMAC key custody, rotation, backup, and destruction be reviewed as a module CSP/configuration-management item, and may the same controlled key reproduce an approved binary? | Affects P2-05 CSP inventory and P2-07 build/release procedure. | Current design requires per-validation/release-binary controlled material and prohibits source/CI/log/evidence disclosure. |

## Priority C — Entropy and DRBG

| ID | Addressee | Question | Why it matters | Current internal position |
|---|---|---|---|---|
| PCQ-014 | Testing institution / entropy specialist | May OE-01 Linux `getrandom(flags=0)` be treated as an external conditioned provider whose raw-noise health tests remain outside AnoCrypto-C, and which kernel/image/provider records are required? | Determines whether `ENT-OE01-LINUX-GETRANDOM-R1` is viable without a direct source. | Internally frozen; formal acceptance is `EXTERNAL_CONFIRMATION_REQUIRED`. |
| PCQ-015 | Testing institution / entropy specialist | How should entropy credit be assigned to the 32-byte entropy request and separate 16-byte nonce request, and is the two-call construction acceptable? | Controls instantiate justification and test documentation. | Input lengths are implemented; no full-bit-per-byte claim is made. |
| PCQ-016 | Testing institution | Are status-only provider probes plus retry/full-fill/output-clearing tests sufficient, or is additional provider-health evidence required? | Defines evidence without retaining sensitive bytes. | Status, lengths, identifiers, and hashes only; provider bytes are prohibited from evidence. |
| PCQ-017 | Testing institution | Should OS-provider failure during init/reseed remain operation-local, or must any provider failure enter sticky module `ERROR_STATE`? | Changes failure classification and operator recovery. | Current code treats OS-provider failure as operation-local; future direct-source fatal failure is separate. |
| PCQ-018 | Testing institution | Are the implemented 65,536-byte per-request maximum, `2^48` reseed-counter interval, unsupported prediction resistance, and no automatic time/byte reseed acceptable for the submitted profile? | Finalizes profile limits without adding API. | Frozen implementation behavior; a stricter operational interval may be adopted after review. |
| PCQ-019 | Testing institution | Is process-marker change plus mandatory reseed sufficient for forked state, and how should unsupported process-marker branches be declared? | Defines supported process model. | Source-backed service fails closed and clears output until successful reseed. |
| PCQ-020 | Testing institution | Is explicit discard-and-independent-reinitialize guidance sufficient for VM/container checkpoint duplication and suspend/resume, or are platform restrictions/tests required? | Automatic detection is not implemented. | These scenarios are not claimed as automatically protected. |
| PCQ-020A | Testing institution | Which events are formally conditional self-tests for this candidate: external-provider operational checks, future direct-source health tests, DRBG instantiate/reseed, or none of those? | Prevents misclassification of service validation as self-test. | P2-04 freezes responsibility but not institution terminology. |
| PCQ-020B | Testing institution / entropy specialist | If a project-owned direct source becomes necessary, what assessment, raw-sample access, startup/continuous tests, thresholds, false-positive analysis, and recovery are required before adding it? | Prevents promoting generic duplicate-block scaffolding. | Direct source is out of the initial candidate and requires a separate gate. |

## Priority D — CSP, Zeroization, and Side-channel Review

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-021 | Testing institution | Which values in the current implementation must be listed as CSPs, including imported keys, expanded schedules, HMAC pads, CMAC subkeys, DRBG V/K, entropy buffers, and intermediate state? | Defines formal CSP inventory. | P2-05 enumerates conservatively; confirmation is still required. |
| PCQ-022 | Testing institution | Is in-memory-only, non-exportable key storage with purpose-separated opaque handles an acceptable initial scope? | Avoids persistence/wrapping expansion. | Export, persistence, wrapping, and platform keystore integration remain outside scope. |
| PCQ-023 | Testing institution | What evidence is accepted for zeroization under optimizing compilers, and are compiler output plus lifecycle/failure tests sufficient as engineering inputs? | Defines closure criteria. | Target/compiler-specific human disposition is planned; universal guarantees are not claimed. |
| PCQ-024 | Testing institution | What process/OS memory behavior is explicitly outside module zeroization responsibility after buffer clearing or process termination? | Prevents overclaiming OS/hardware behavior. | Requires operating-environment wording. |
| PCQ-024A | Testing institution | Is the split between module-owned table wipe and caller-owned explicit final/uninit cleanup acceptable for a shared-library software boundary? | Determines whether caller-abandoned opaque contexts need additional controls or API changes. | Current module does not retain caller context addresses and makes no global cleanup claim for them. |
| PCQ-024B | Testing institution / security specialist | If the module global lock cannot be acquired during a fatal transition, is immediate process termination with no unsafe table traversal an acceptable fail-closed response? | Determines whether an emergency cleanup design is required. | Current fallback latches fatal state; safe module-owned table wipe is guaranteed only when the lock is acquired. |
| PCQ-025 | Testing institution / side-channel specialist | What side-channel review depth is expected for table-free portable AES, HMAC/DRBG, CMAC comparison, and compiler-specific output at the selected level? | Sizes P2-06 and external review. | Compiler captures exist; timing/cache/fault review remains incomplete. |
| PCQ-026 | Testing institution | Are shared read-only key handles with internal locking acceptable, and what caller serialization rules must be documented for mutable contexts? | Freezes thread-safety model. | Global state and handle tables are locked; mutable operation contexts are caller-owned. |

## Priority D2 — Secure Coding and Side-channel Expectations

| ID | Recipient | Question | Why it matters | Current project option / decision impact |
|---|---|---|---|---|
| PCQ-SC-001 | Testing institution | For the software-only OE-01 shared library, what source, selected-binary, timing, and cache evidence is required? | Determines whether P2-06 local review is sufficient or a specialist campaign is mandatory. | Exact OE-01 and empirical testing remain open. |
| PCQ-SC-002 | Testing institution | May public algorithm, key-size class, message length, and operation type vary execution time, or must any be hidden? | Fixes the leakage model and test classes. | Current design treats these as public API metadata. |
| PCQ-SC-003 | Testing institution | Are power/EM, transient-execution, and active-fault assessments applicable to this software boundary and intended level? | Prevents unsupported physical/fault claims and defines external specialist scope. | These areas are currently `NOT_EVALUATED`. |
| PCQ-SC-004 | Testing institution | Are FORTIFY, stack protector, full RELRO/BIND_NOW, NX stack, CET/IBT, or other binary-hardening controls required or recommended? | Freezes final build profile and environment compatibility. | P2-06 selects the first five; CET remains undecided. |

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
