# KCMVP Assisted Rehearsal Transcript

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-AR-002` |
| Status | `CONTROLLED_ASSISTED_REHEARSAL` |
| Policy ID | `LAR-P2-10-R1` |
| Institution participant | `NONE` |
| Independent human reviewer | `NONE` |

This transcript contains compact project-position answers rehearsed against the Priority-1 agenda. The questions are not institution questions received during a meeting, and the answers are not institution dispositions.

## A-01 — Boundary and operating environment

**Prompt:** What exactly is the candidate module and which environment is being proposed?

**Rehearsed answer:** The internal candidate is one Linux x86-64 Release shared library, `libanocrypto_c.so.0.1.0`, built for Ubuntu Server 24.04.4 LTS with GCC 13.3.0 and CMake 3.28.3. Module code and module-owned state are inside the shared object. Public headers, loader, libc, kernel, application, test tools, OpenSSL differential provider, PoC, AnoSecureKit, AnoPKI and signing infrastructure are outside. Exact OE-01 execution is not yet run, and the institution must confirm environment and variation granularity.

## A-02 — Security level and authority

**Prompt:** Which validation level are you targeting and who can decide it?

**Rehearsed answer:** No level is currently selected or claimed. The project asks the testing and validation institutions to identify the appropriate level and distinguish which questions are decided by each authority. No engineering document may infer a level before a retained answer and explicit owner decision.

## A-03 — Runtime integrity

**Prompt:** How is the shared library integrity checked?

**Rehearsed answer:** It is not checked by a runtime verifier today. The selected candidate design is HMAC-SHA-256 over the shared object with a defined excluded tag field, but implementation, build-time tag injection, integrity-key custody, tamper tests and accepted measurement semantics are pending. The purpose of consultation is to accept, revise or reject this design before implementation.

## A-04 — Self-tests and fatal recovery

**Prompt:** What happens before services become available and after a fatal failure?

**Rehearsed answer:** Startup KATs gate operational service. Fatal self-test or fatal health failure enters sticky error state and cryptographic services are rejected. The intended fatal recovery boundary is process termination and fresh load. Orderly shutdown may permit a complete same-process reinitialization. The existing on-demand self-test symbol does not yet implement the full proposed state transition, so that remains a controlled delta.

## A-05 — Entropy and DRBG

**Prompt:** Who owns entropy quality and health tests?

**Rehearsed answer:** The internal OE-01 candidate uses Linux `getrandom(flags=0)` as an external conditioned provider. AnoCrypto-C owns request completion, failure handling, zeroization of temporary buffers, DRBG instantiate/reseed/generate policy and process-change handling. It does not claim ownership of raw-noise health tests or full-bit entropy credit. Those responsibilities and accepted evidence require institution and entropy-specialist confirmation.

## A-06 — CSP and zeroization

**Prompt:** What CSPs exist and how are they cleared?

**Rehearsed answer:** The conservative inventory includes imported keys, expanded schedules, HMAC pads, CMAC subkeys, DRBG V/K, entropy/nonce buffers and sensitive intermediate state. Module-owned key slots are wiped on explicit destroy, orderly shutdown and a fatal transition when the global lock is held. Caller-owned opaque contexts require their documented cleanup functions and are not globally tracked. Compiler-specific evidence and exact responsibility limits remain external-confirmation items.

## A-07 — Secure coding and side channels

**Prompt:** What side-channel assurance do you have?

**Rehearsed answer:** The portable AES source avoids secret-indexed S-box/T-table lookup, CMAC verification performs a full-tag comparison, local static/compiler review is complete, and Linux Release hardening is selected. The project does not claim universal constant-time behavior or specialist side-channel resistance. Exact OE-01 binary review, statistical timing/cache work and applicability of power, EM, fault and transient-execution assessment remain open.

## A-08 — Submission documents and transfer

**Prompt:** Are your submission documents final?

**Rehearsed answer:** No. They are controlled internal skeletons that consolidate the current design and evidence references. The project needs current official forms, security-level-specific expectations, initial-versus-later artifact requirements and a protected transfer channel before conversion into a submission package.

## A-09 — Evidence and configuration management

**Prompt:** What evidence is available now?

**Rehearsed answer:** Local engineering evidence includes KAT, negative, lifecycle, concurrency, differential, sanitizer/static review, ABI compatibility, install/PoC, source inventory, provenance and status-only package checks. Actual hosted P1-34 artifacts do not exist because no runner was assigned; hosted status is `NOT_RUN` and review is `UNREVIEWED`. Local or synthetic outputs will not be relabeled as hosted or institution evidence.

## A-10 — Claims, signing and customer PoC

**Prompt:** Can the current package be signed or sent to customers as a validated product?

**Rehearsed answer:** No. Repository signing is `UNSIGNED`, production signer and TSA are unprovisioned, handoff is `BLOCKED`, and external release authorization is `NO`. The PoC is an evaluation package only and legal/evaluation terms remain pending. External wording remains limited to candidate and readiness language until the responsible institution and project owner authorize otherwise.
