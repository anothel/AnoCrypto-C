# AnoCrypto-C Source Manifest

Status: P1-14 draft
Date: 2026-07-08

## Purpose

This manifest records the materials that are allowed to influence the AnoCrypto-C private C99 codebase.
It is intentionally conservative: implementation code, test vectors, file names, function names, constants, and build layout must not be copied from materials whose ownership or license is unclear.

## Baseline allowed inputs

| ID | Source | Classification | Allowed use | Exclusions / notes | Status |
|---|---|---|---|---|---|
| SRC-0001 | User-provided anocrypto-c project direction and P0 skeleton request | User-owned project instruction | Repository skeleton, API direction, TODO language, boundary decisions | Do not infer ownership of any external/private materials from this instruction alone. | Allowed |
| SRC-0002 | User-owned anocrypto C99 source pack documents uploaded to the GPT project | User-owned project planning material | Requirements, architecture, API/ABI rules, evidence checklist, clean-room process | Use as requirements only; do not import external/private code or vectors. | Allowed |
| SRC-0003 | ISO C99 language rules and common C ABI practice | Public standard / public knowledge | C syntax, include guards, `extern "C"`, `stdint.h`, `stddef.h`, opaque types, explicit enum values | This skeleton does not include normative text from the standard. | Allowed |
| SRC-0004 | CMake package/install conventions | Public documentation / public knowledge | Build target, install/export skeleton, package config skeleton | Keep package names in the anocrypto-c naming family only. | Allowed |
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
| P1-001 | anocrypto-c P0 skeleton generated in this repository | User-owned project artifact | Extended into P1 foundation skeleton | No third-party code imported. |
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

## P1-7 Platform Entropy Provider Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-1601 | Linux `getrandom(2)` manual page from the Linux man-pages project | Public OS API documentation | Linux platform entropy provider boundary using `getrandom(..., flags=0)` | Documentation input only; no external implementation code imported. |
| SRC-1602 | Microsoft documentation for `BCryptGenRandom` | Public OS API documentation | Windows platform entropy provider boundary using the system-preferred RNG flag | Documentation input only; no external implementation code imported. |
| SRC-1603 | Apple Developer documentation for `SecRandomCopyBytes` | Public OS API documentation | Apple platform entropy provider boundary using `kSecRandomDefault` | Documentation input only; no external implementation code imported. |
| DER-6001 | Fresh C99 platform entropy provider seam written in this repository | User-owned generated source | Internal provider name/status function and platform read wrapper | Entropy bytes must never be logged or written to evidence. |
| DER-6002 | Fresh C99 platform entropy provider tests written in this repository | User-owned generated test code | Provider availability/status smoke test and failure zeroization behavior | Test logs may include provider name and status only. |

## P1-7 Clean-room Notes

- No third-party RNG wrapper, entropy abstraction, or random-service implementation code was imported.
- Platform API documentation influenced boundary selection only.
- The OS provider seam is internal and does not expose a public random-service API.
- The public HMAC_DRBG API remains explicit-entropy based for deterministic tests and validation-boundary review.
- Entropy bytes, generated random bytes, DRBG state, key material, private keys, and tokens must not be logged or included in evidence artifacts.
- Provider smoke tests are engineering checks only and are not entropy-quality proof for every customer environment.

## P1-8 Internal Random-Service/DRBG Helper Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-8001 | Fresh C99 internal HMAC_DRBG-SHA-256 instantiate/reseed helper written in this repository | User-owned generated source code | Connect internal entropy source seam to DRBG core for future random-service review | No public random API added. No third-party RNG wrapper or DRBG helper code imported. |
| DER-8002 | Fresh C99 `drbg_entropy_helper_tests.c` written in this repository | User-owned generated test code | Verify helper-driven instantiate/reseed matches explicit-entropy DRBG behavior and zeroizes context on helper failure | Tests do not print entropy, seed material, generated bytes, or deterministic byte streams. |
| DER-8003 | `docs/design/C99_RANDOM_SERVICE_API_DRAFT.md` written for this repository | User-owned design draft | Record candidate public random-service direction before exposing ABI | Draft only; not a committed public API contract. |

## P1-8 Clean-room Notes

- No external random-service, entropy adapter, DRBG helper, or RNG wrapper implementation code was imported.
- The internal helper uses the existing user-owned HMAC_DRBG-SHA-256 core and entropy adapter seam.
- Deterministic callback source bytes in tests are local synthetic fixtures and are not operational entropy.
- Helper tests compare generated outputs in memory only and do not print or persist generated bytes.
- Public `ano_crypto_random_*` APIs remain TODO pending API/ABI and validation-boundary review.

## P1-9 HMAC_DRBG Boundary and Naming Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-9001 | Fresh deterministic HMAC_DRBG-SHA-256 boundary fixtures generated for this repository from the public SP 800-90A Rev.1 algorithm | User-owned generated test data | Personalization, additional input, and reseed-after-generate regression tests | Not a public KAT source and not validation evidence. Fixture bytes are synthetic test material only. |
| DER-9002 | Fresh C99 HMAC_DRBG request-size boundary test written in this repository | User-owned generated test code | Verify max request acceptance and oversized request rejection without logging generated bytes | No third-party test harness code imported. |
| DER-9003 | AnoCrypto-C naming normalization decision | User-owned project decision | Product/brand spelling, CMake package/target naming, install smoke package lookup | Product name: `AnoCrypto-C`; repo/archive slug may remain `anocrypto-c`; CMake package: `AnoCryptoC`; CMake target: `AnoCryptoC::AnoCryptoC`; library: `anocrypto_c`. |

## P1-9 Clean-room Notes

- No external DRBG implementation source or third-party random-service code was imported.
- Deterministic boundary fixtures are synthetic local regression data derived from the public HMAC_DRBG algorithm and are not operational entropy, seeds, nonces, or generated secrets.
- Test logs must not print entropy input, nonce, personalization, additional input, generated bytes, DRBG state internals, key material, private keys, or tokens.
- The P1-9 CMake naming normalization is a project naming decision, not a copy of an external build structure.


## P1-10 Reproducibility, CI, and ABI Review Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-10001 | Fresh reproducible-build notes written for this repository | User-owned documentation | Record deterministic-source and build-evidence procedure draft | Documents current gaps; does not assert byte-for-byte reproducibility. |
| DER-10002 | Fresh CI matrix skeleton written for this repository | User-owned workflow/documentation | Exercise Linux, Windows, and Apple build/test/install-smoke paths | Skeleton requires review before treating CI output as release evidence. |
| DER-10003 | Fresh API/ABI review checklist written for this repository | User-owned documentation | Header layout, naming, status-return, symbol, install-package, and evidence review | Checklist records TODOs and exceptions instead of making assurance claims. |
| DER-10004 | Fresh repository policy check script written for this repository | User-owned tooling | Local grep-style checks for retired naming and premature assurance wording | The script constructs scan terms internally to avoid reintroducing retired strings in source text. |

## P1-10 Clean-room Notes

- No external implementation source, CI workflow, packaging script, or ABI scanning code was copied.
- The P1-10 additions are documentation, CMake/tooling, and workflow skeleton changes only.
- Reproducibility notes are draft engineering procedures and do not claim deterministic binary output yet.
- CI matrix output, local CTest logs, and symbol lists remain engineering evidence until reviewed.

## P1-11 Customer PoC Package Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-11001 | Fresh `examples/core_poc` C99 integration example written in this repository | User-owned generated source | Customer PoC smoke using installed public headers and `AnoCryptoC::AnoCryptoC` | Uses public demo bytes only; does not print key material, entropy, nonce, DRBG state, or generated DRBG bytes. |
| DER-11002 | Fresh customer PoC package staging and smoke scripts written in this repository | User-owned generated tooling | Stage installed SDK prefix, selected docs, example, manifest, and smoke logs | Scripts generate engineering package drafts only and must be reviewed before sharing. |
| DER-11003 | Fresh SDK integration and customer PoC package documentation written in this repository | User-owned documentation | Record package contents, integration commands, evaluation boundary, and log handling | Does not assert validation completion or broad environment suitability. |
| DER-11004 | P1-11 CI/evidence updates written in this repository | User-owned workflow/documentation | Add build-tree example test and Linux PoC package smoke path | CI output remains engineering evidence pending review. |

## P1-11 Clean-room Notes

- No third-party SDK sample, package script, CMake consumer project, or customer integration template was copied.
- The PoC example uses only public AnoCrypto-C headers and public demo bytes created for this repository.
- The PoC example intentionally does not print raw key material, entropy input, nonce input, DRBG state, MAC bytes, or generated DRBG bytes.
- PoC package manifests and smoke logs must be reviewed for local paths and internal details before external sharing.
- The customer PoC package is an engineering integration draft and is not official validation evidence.

## P1-12 Scope and Validation Planning Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-12001 | Fresh `docs/roadmap/DEVELOPMENT_WORKFLOW.md` written in this repository | User-owned documentation | Record task flow, phase gates, stop conditions, and planned sequence | Planning document only; no external workflow copied. |
| DER-12002 | Fresh `docs/kcmvp/SERVICE_SCOPE_MATRIX.md` written in this repository | User-owned documentation | Separate implemented services, ABI-reserved services, internal seams, and deferred product-layer features | Does not assert official validation status. |
| DER-12003 | Fresh `docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md` written in this repository | User-owned documentation | Record allowed comparison inputs, generated vector rules, and logging limits | No third-party reference tool is selected yet. |
| DER-12004 | Fresh `docs/design/BLOCK_CIPHER_MODE_SELECTION.md` written in this repository | User-owned documentation | Gate block cipher/mode implementation behind customer and validation-target scope review | No block cipher code or third-party implementation imported. |
| DER-12005 | Fresh `docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md` written in this repository | User-owned documentation | Add a review checklist for key-dependent code paths | Checklist is a review aid only; it does not claim leakage-resistance proof. |
| DER-12006 | Fresh `docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md` written in this repository | User-owned documentation | Collect non-secret customer PoC requirements for platform/toolchain/algorithm scope | Explicitly excludes secrets, entropy captures, private keys, tokens, and customer data. |
| DER-12007 | P1-12 evidence workflow notes written in this repository | User-owned tooling/documentation | Add scope-review notes to local evidence drafts and PoC package documentation set | Engineering evidence only; not official validation evidence. |

## P1-12 Clean-room Notes

- No block cipher, KDF, AEAD, public-key, SecureKit, or PKI implementation code was imported.
- P1-12 adds planning and review documents only, plus evidence/package references to those documents.
- Customer intake explicitly excludes secret material, entropy captures, generated secret bytes, private keys, tokens, customer data, and internal deployment secrets.
- Differential validation remains a strategy draft until a reviewed public/reference source or tool is selected.
- Scope planning does not change the current implemented cryptographic service slice.

## P1-13 SBOM, Signing, and Manifest Planning Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-13001 | Fresh `docs/kcmvp/SBOM_SIGNING_PLAN.md` written in this repository | User-owned documentation | Record SBOM and signing placeholder workflow before selecting tools or key-management policy | No SBOM generator output, private key, token, or signature file is included. |
| DER-13002 | Fresh `docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md` written in this repository | User-owned documentation | Define evidence/package manifest hash rules and review boundaries | Uses SHA-256 manifests as engineering review aids only. |
| DER-13003 | Fresh `docs/business/POC_PACKAGE_REVIEW_MANIFEST.md` written in this repository | User-owned documentation | Record PoC package review fields before customer sharing | Explicitly excludes customer secrets, signing keys, tokens, private keys, entropy captures, and customer data. |
| DER-13004 | P1-13 updates to `tools/collect_release_evidence.cmake` | User-owned tooling | Generate `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, and evidence manifest files | Placeholder status only; no private signing material and no generated SBOM are added. |
| DER-13005 | P1-13 updates to PoC package staging/smoke scripts | User-owned tooling | Generate and verify package manifest, package review manifest, SBOM status, and signing status records | Package smoke checks record presence without creating signatures or collecting secrets. |

## P1-13 Clean-room Notes

- No external SBOM generator output, signing workflow code, release service integration, or vendor package manifest was copied.
- No private signing key, signing token, customer secret, customer data, entropy capture, generated secret bytes, private key, or internal deployment material is included.
- P1-13 adds documentation, placeholder status records, and manifest generation only.
- SBOM and signing remain TODO until tool, format, signer identity, and verification policy are reviewed.

## P1-14 Thread-Safety Support Matrix Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-14001 | Fresh `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` written in this repository | User-owned documentation | Record public API concurrency support boundaries and unsupported cases | Does not claim broad context-level concurrent mutation safety. |
| DER-14002 | Fresh `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md` written in this repository | User-owned documentation | Record CI lanes, local commands, and log-review rules for thread-safety checks | Logs remain status-only and must not include key material, entropy, generated bytes, private keys, tokens, or customer data. |
| DER-14003 | Fresh `tools/check_thread_safety_policy.cmake` written in this repository | User-owned tooling | Check docs/source/build/evidence wiring for P1-14 thread-safety policy | Engineering guardrail only; not official validation evidence. |
| DER-14004 | P1-14 updates to PoC package staging and smoke scripts | User-owned tooling | Generate and verify `THREAD_SAFETY_PACKAGE_NOTES.txt` | Package note is a support-boundary summary only. |
| DER-14005 | P1-14 updates to `.github/workflows/ci.yml` | User-owned workflow skeleton | Add thread-safety ON/OFF matrix handling and policy-check step | Hosted CI output requires review before release-candidate use. |

## P1-14 Clean-room Notes

- No third-party lock wrapper, concurrency harness, CI workflow, or support-matrix text was copied.
- P1-14 adds documentation, CMake/tooling, CI skeleton updates, and evidence/package records only.
- The current support matrix does not make caller-owned hash, HMAC, or DRBG contexts concurrently mutable.
- Thread/concurrency logs must remain status-only and must not print key material, entropy input, nonce input, DRBG state, MAC output, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details.
