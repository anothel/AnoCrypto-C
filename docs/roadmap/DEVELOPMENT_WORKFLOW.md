# AnoCrypto-C Development Workflow

Status: P1-12 draft.

## Purpose

This document explains the working flow used for AnoCrypto-C after the C99 reboot. The goal is to keep implementation, public ABI, test evidence, clean-room records, and customer PoC packaging moving in the same order instead of adding algorithms without review gates.

## Current phase map

| Phase | Main goal | Exit signal |
|---|---|---|
| P0 Foundation | Repository, clean-room records, C ABI skeleton, module lifecycle, self-test, build identity, install smoke | A consumer can build, install, link, initialize, self-test, and shut down the module. |
| P1 Core slice | Small algorithm slice, KATs, negative tests, entropy/DRBG/CSP seams, CI/reproducibility notes, API/ABI review, customer PoC package | The limited SHA-256/HMAC-SHA-256/HMAC_DRBG-SHA-256 slice has repeatable tests and engineering evidence. |
| P1 Scope lock | Decide the next validation-target candidate scope before expanding code | Algorithm scope, service matrix, differential-validation strategy, side-channel review checklist, and customer intake inputs are recorded. |
| P2 Productization | Customer PoC hardening, SBOM/signing placeholders, toolchain support matrix, SecureKit/PKI boundary planning | A customer-specific PoC package can be reviewed without mixing core, SecureKit, and PKI scope. |
| P3 Validation readiness | Gap review with selected platform/toolchain/scope and external preparation | Submission-facing artifacts are prepared from reviewed evidence, not from ad-hoc logs. |

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
   - Install smoke.
   - PoC package smoke when enabled.
   - Evidence target.
   - Release-style symbol scan when visibility policy is relevant.

6. Package outputs.
   - Create a full changed ZIP.
   - Create a patch against the previous step.
   - Include a recommended commit message.

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

## Planned next sequence

| Step | Candidate name | Scope |
|---|---|---|
| P1-13 | SBOM/signing placeholders | Add SBOM/signing status files to evidence and PoC manifest without choosing a signing tool yet. |
| P1-14 | Thread-safety support matrix | Turn internal locking notes into a public support matrix and CI checklist. |
| P1-15 | SHA-384/SHA-512 implementation decision | Decide whether to implement, defer, or source-review SHA-384/SHA-512. |
| P1-16 | Public random API decision | Decide whether to expose `ano_crypto_random_*` or keep DRBG APIs lower-level until entropy policy is reviewed. |
| P2-01 | Block cipher implementation start | Start only after P1-12 selection criteria and customer/KCMVP scope are reviewed. |

## Stop conditions

Stop adding code and update docs first when any of the following happen:

- A public API needs a new exposed struct layout.
- A new algorithm changes the candidate validation boundary.
- A test vector source is unclear.
- A customer request mixes AnoCrypto-C, AnoSecureKit, or AnoPKI responsibilities.
- A log or sample risks printing secret material, entropy bytes, generated secret bytes, private keys, tokens, or customer data.
- A marketing or security statement is stronger than the current evidence.
