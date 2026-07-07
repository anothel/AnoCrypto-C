# AnoCrypto-C Source Manifest

Status: P1-6 draft
Date: 2026-07-07

## Purpose

This manifest records the materials that are allowed to influence the AnoCrypto-C private C99 codebase.
It is intentionally conservative: implementation code, test vectors, file names, function names, constants, and build layout must not be copied from materials whose ownership or license is unclear.

## Baseline allowed inputs

| ID | Source | Classification | Allowed use | Exclusions / notes | Status |
|---|---|---|---|---|---|
| SRC-0001 | User-provided AnoCrypto-C project direction and P0 skeleton request | User-owned project instruction | Repository skeleton, API direction, TODO language, boundary decisions | Do not infer ownership of any external/private materials from this instruction alone. | Allowed |
| SRC-0002 | User-owned AnoCrypto C99 source pack documents uploaded to the GPT project | User-owned project planning material | Requirements, architecture, API/ABI rules, evidence checklist, clean-room process | Use as requirements only; do not import external/private code or vectors. | Allowed |
| SRC-0003 | ISO C99 language rules and common C ABI practice | Public standard / public knowledge | C syntax, include guards, `extern "C"`, `stdint.h`, `stddef.h`, opaque types, explicit enum values | This skeleton does not include normative text from the standard. | Allowed |
| SRC-0004 | CMake package/install conventions | Public documentation / public knowledge | Build target, install/export skeleton, package config skeleton | Keep package names in the AnoCrypto-C naming family only. | Allowed |
| SRC-0005 | Future public algorithm specifications and public KAT vectors | Public standards / public test vectors | P1+ algorithm implementation and KAT manifests only after source URL/version/hash are recorded | No vectors are imported in P0. | TODO |

## Materials not used in P0

- Any source code, build scripts, file names, function names, constants, comments, or tests from materials with unclear ownership or license.
- Any private customer, employer, vendor, or third-party material.
- Any secret key, seed, entropy sample, private scalar, token, certificate private key, or internal deployment detail.
- Any marketing or assurance claim beyond KCMVP-oriented preparation language.

## P0 clean-room notes

- This repository is a fresh C99 skeleton.
- The public C API is not compatible with the previous C++20 API and does not attempt to preserve it.
- Cryptographic algorithms are intentionally not implemented in P0.
- Placeholder self-tests only validate framework wiring; they are not algorithm validation evidence.

## TODO before P1 implementation

1. Add a reviewed source entry for each public algorithm specification used.
2. Add a reviewed source entry for every public KAT vector set, including source URL, version/date, and SHA-256 of the local copy.
3. Add a reviewer and approval column if this repository is used by more than one implementer.
4. Add a rule for how generated files are traced back to inputs.


## P1 Foundation Additions

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| P1-001 | AnoCrypto-C P0 skeleton generated in this repository | User-owned project artifact | Extended into P1 foundation skeleton | No third-party code imported. |
| P1-002 | Project C99 API/ABI rules | User-provided design guidance | Naming, status/API style, public header expansion | Abstract requirements only. |
| P1-003 | Project C99 module architecture | User-provided design guidance | Hash/HMAC/DRBG/CSP skeleton directories and state-gate policy | Abstract requirements only. |

## P1 KAT Source Status

No public KAT vector files have been imported yet. `src/selftest/kat_manifest.c` contains placeholder manifest entries only and must not be treated as algorithm evidence.

## P1-1 SHA-256 / HMAC-SHA-256 Implementation Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-1001 | NIST FIPS 180-4, Secure Hash Standard (SHS), final publication page and PDF | Public standard | SHA-256 compression, padding, digest size, IV and round constants | Algorithm specification input only. This repository does not claim official validation. |
| SRC-1002 | RFC 4231, Identifiers and Test Vectors for HMAC-SHA-224/SHA-256/SHA-384/SHA-512 | Public RFC / public test vectors | HMAC-SHA-256 KAT cases 1 and 2 | Public test-vector keys are synthetic KAT material and not operational secrets. |
| SRC-1003 | RFC 2104, HMAC: Keyed-Hashing for Message Authentication | Public RFC | HMAC construction rule using inner/outer pads | Specification input only. No RFC sample implementation code imported. |
| SRC-1004 | NIST CAVP Secure Hashing project page | Public validation-program information | Evidence caveat language: local KATs are informal correctness checks only | KAT pass results do not replace official validation. |

## P1-1 Clean-room Notes

- SHA-256 and HMAC-SHA-256 were implemented as fresh C99 source in this repository.
- No external SHA/HMAC implementation code was imported.
- Embedded KAT values are limited to public specification/test-vector material.
- HMAC KAT keys are public test vectors only; they must not be reused as operational keys.
- SHA-384, SHA-512, DRBG, CSP/key import, and algorithm validation evidence remain TODO.

## P1-2 Extended KAT and Evidence Tooling Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-1101 | Existing P1-1 SHA-256/HMAC-SHA-256 implementation in this repository | User-owned project artifact | Extended tests and startup self-test coverage | No external implementation code imported. |
| SRC-1102 | NIST FIPS 180-4 SHA-256 example digests already recorded as SRC-1001 | Public standard / public test vectors | Added multi-block and million-`a` SHA-256 KAT coverage | Correctness test input only; not official validation evidence. |
| SRC-1103 | RFC 4231 HMAC-SHA-256 test vectors already recorded as SRC-1002 | Public RFC / public test vectors | Added HMAC-SHA-256 test cases 3, 4, 6, and 7 | Public synthetic keys only; not operational secrets. |
| DER-2001 | Locally generated deterministic SHA-256 chunk-boundary API consistency fixtures | User-owned generated test data | Compare one-shot and streaming APIs for boundary lengths | Not a public KAT source and not validation evidence. |
| DER-2002 | Locally generated deterministic HMAC-SHA-256 chunk-boundary API consistency fixtures | User-owned generated test data | Compare one-shot and streaming APIs for key/message boundary lengths | Not a public KAT source and not validation evidence. |
| DER-2003 | `tools/collect_release_evidence.cmake` written for this repository | User-owned project artifact | Engineering evidence draft collection | Captures local paths and logs; review before sharing. |

## P1-2 Clean-room Notes

- No external SHA-256, HMAC, KAT runner, or evidence script implementation code was imported.
- Public KAT values are limited to already recorded public SHA-256 and HMAC-SHA-256 sources.
- Public HMAC test-vector keys are synthetic test fixtures and must not be used as operational keys.
- Deterministic boundary tests are local API consistency tests and are labeled separately from public KAT sources.
- Evidence drafts may include local paths, compiler identity, and test logs; review before external sharing.

## P1-3 HMAC_DRBG-SHA-256 Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-1201 | NIST SP 800-90A Revision 1, Recommendation for Random Number Generation Using Deterministic Random Bit Generators | Public standard | HMAC_DRBG instantiate, reseed, generate, update state-machine logic | Algorithm specification input only. OS entropy-source integration remains TODO. |
| SRC-1202 | NIST CAVP DRBG Test Vectors package linked from the public CAVP Random Number Generators page | Public test vectors | HMAC_DRBG-SHA-256 no-prediction-resistance with reseed COUNT=0 KAT | Used for informal implementation correctness testing only. Does not replace formal validation. |
| DER-3001 | Fresh C99 DRBG API negative tests written in this repository | User-owned generated test code | Invalid argument, unsupported flag, short entropy, context state, and uninstantiate behavior | No third-party test harness code imported. |

## P1-3 Clean-room Notes

- HMAC_DRBG-SHA-256 was implemented as fresh C99 source in this repository using the public algorithm specification above.
- No external DRBG implementation code was imported.
- Embedded DRBG KAT values are public test-vector material only and are not operational entropy, seed, nonce, or secret material.
- The P1-3 public DRBG API accepts explicit entropy input so the DRBG core can be tested deterministically and later driven by a reviewed entropy adapter.
- OS entropy collection, prediction-resistance requests, and Hash_DRBG-SHA-256 remain TODO.

## P1-4 Entropy Adapter and HMAC CSP Handle Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-4001 | Fresh C99 internal entropy adapter seam written in this repository | User-owned generated source | Callback test seam, OS-reserved provider stub, failure zeroization behavior | No OS entropy implementation or third-party code imported. |
| DER-4002 | Fresh C99 in-memory CSP handle table written in this repository | User-owned generated source | HMAC key import/use/destroy lifecycle slice | No keystore, persistence, export, or third-party key-management code imported. |
| SRC-4003 | RFC 4231 HMAC-SHA-256 Test Case 2 already recorded as SRC-1002 | Public RFC / public test vector | HMAC key handle KAT path | Public synthetic key only; not operational secret material. |
| DER-4004 | Fresh C99 CSP/key lifecycle negative tests written in this repository | User-owned generated test code | Invalid handle, oversize key, unsupported flag/type, destroy, shutdown cleanup behavior | Test diagnostics must not print key bytes. |
| DER-4005 | Fresh C99 entropy adapter unit tests written in this repository | User-owned generated test code | Callback success/failure, minimum request rejection, OS-reserved behavior | Test logs record status only, not generated bytes. |

## P1-4 Clean-room Notes

- No external entropy adapter, RNG, keystore, key table, or CSP implementation code was imported.
- The entropy callback seam is test/internal plumbing only and is not an operational entropy source.
- HMAC key handle tests use public synthetic RFC test-vector keys only.
- Raw key material, entropy bytes, DRBG state, and generated secret bytes must not be logged or added to evidence artifacts.
- OS entropy provider, key export, persistence, keystore integration, and thread-safety remain TODO.


## P1-5 Lifecycle and Evidence Additions

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-5001 | Fresh C99 CSP handle exhaustion/stale-handle negative test written in this repository | User-owned generated test code | Fixed-capacity table exhaustion, generation-based stale-handle rejection, slot reuse behavior | Test diagnostics must not print key bytes or handle internals beyond status/state names. |
| DER-5002 | Updated `tools/collect_release_evidence.cmake` written in this repository | User-owned project artifact | Source archive creation, source archive SHA-256, install-prefix smoke log capture | Evidence may include local paths; review before sharing. |
| DER-5003 | Local source archive generated by evidence tooling | User-owned generated artifact | Engineering release-candidate input hash | Archive contents are derived from filtered repository files only. |

## P1-5 Clean-room Notes

- No external CSP handle-manager, evidence-packager, or install-smoke implementation code was imported.
- Source archive and install smoke logs are engineering evidence inputs only.
- Evidence output must be reviewed for local path disclosure and must not contain secrets, entropy bytes, private keys, tokens, or operational key material.
- The fixed table capacity is an implementation detail for P1 testing and must be re-reviewed before ABI or product commitments.


## P1-6 Thread-Safety Policy and Lock Seam Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-5001 | Fresh C99 internal thread lock seam written in this repository | User-owned generated source | Process-wide lock for module, self-test bookkeeping, and CSP global state | POSIX and Windows implementation branches are small platform bindings; unsupported fallback requires review before commitment. |
| DER-5002 | Fresh POSIX concurrent CSP/HMAC test written in this repository | User-owned generated test code | Import/use/destroy HMAC handles across worker threads | Uses public synthetic HMAC fixture bytes only and does not print key material. |
| DER-5003 | Thread-safety policy document written in this repository | User-owned generated document | Defines P1-6 concurrency boundary and TODOs | Does not claim broad per-context thread safety. |

## P1-6 Clean-room Notes

- No third-party lock wrapper, concurrency library, key table, or test harness code was imported.
- The POSIX test uses standard `pthread` APIs as platform interfaces, not copied implementation code.
- The HMAC key bytes used by the concurrent test are public synthetic fixtures only and are not operational secrets.
- Evidence and test logs must not print key material, entropy bytes, generated output, private keys, or tokens.
- P1-6 thread-safety is a limited engineering policy for global state only; public per-context concurrency guarantees remain TODO.
