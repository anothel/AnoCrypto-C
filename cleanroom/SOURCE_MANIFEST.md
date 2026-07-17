# AnoCrypto-C Source Manifest

Status: P2-07 current manifest
Date: 2026-07-17

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

## P1-15 SHA-384/SHA-512 Reserved Scope Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-15001 | Fresh `docs/design/SHA2_384_512_SCOPE_DECISION.md` written in this repository | User-owned planning artifact | Records the decision to keep SHA-384/SHA-512 processing reserved in P1-15 | No third-party implementation code imported. |
| DER-15002 | Fresh `docs/kcmvp/SHA2_384_512_KAT_PLAN.md` written in this repository | User-owned planning artifact | Records planned SHA-384/SHA-512 and HMAC-SHA-384/HMAC-SHA-512 vector/evidence classes | Planning only; no new vector data files imported. |
| DER-15003 | Fresh `tests/negative/sha2_reserved_api_tests.c` written in this repository | User-owned generated test code | Verifies SHA-384/SHA-512 metadata queries and `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` processing behavior | Status-only output; no secrets or generated digest bytes are printed. |
| DER-15004 | Fresh `tools/check_sha2_scope_policy.cmake` written in this repository | User-owned tooling | Checks docs/source/evidence/PoC wiring for SHA-384/SHA-512 reserved scope | Engineering guardrail only. |
| DER-15005 | P1-15 updates to PoC package staging and smoke scripts | User-owned tooling | Generate and verify `SHA2_SCOPE_PACKAGE_NOTES.txt` | Package note is a support-boundary summary only. |

## P1-15 Clean-room Notes

- P1-15 adds documentation, tests, CMake/tooling, evidence records, and package notes only for SHA-384/SHA-512 reserved scope.
- No SHA-384/SHA-512 implementation code is imported or added.
- No HMAC-SHA-384/HMAC-SHA-512 implementation code is imported or added.
- No new non-public test vector data is imported.
- No secret key, entropy sample, generated secret byte string, private key, token, customer data, or internal deployment detail is added.


## P1-16 Random-Service Boundary Inputs

| ID | Source | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-16001 | Fresh `include/anocrypto/ac_random.h` written in this repository | User-owned API skeleton | Reserved public random-service boundary | No third-party implementation code imported. |
| DER-16002 | Fresh `src/drbg/random.c` written in this repository | User-owned C99 implementation | Fail-closed random-service placeholders | Processing paths return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`; generated random bytes are not produced. |
| DER-16003 | Historical P1-16 `tests/negative/random_reserved_api_tests.c` | User-owned regression test | Verified the reserved random-service behavior before implementation | Removed in P1-19 and replaced by lifecycle/failure tests. |
| DER-16004 | Fresh `docs/design/PUBLIC_RANDOM_API_DECISION.md` written in this repository | User-owned planning artifact | Records public random API reserved-boundary decision | No external wording copied. |
| DER-16005 | Fresh `docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md` written in this repository | User-owned evidence planning artifact | Records DRBG/entropy evidence boundaries | Prohibits entropy, seed, and generated-byte logging. |
| DER-16006 | Fresh `tools/check_random_service_policy.cmake` written in this repository | User-owned tooling | Verifies random-service boundary wiring | Engineering guardrail only. |

## P1-16 Clean-room Notes

- P1-16 does not import third-party random-service implementation code.
- P1-16 does not store entropy samples, nonce seed material, generated random bytes, private keys, tokens, or customer data.
- P1-16 keeps the internal platform entropy provider as a status-only engineering seam.

## P1-17 Entropy Failure-Injection and Health-Test Inputs

| ID | Source | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-17001 | Fresh internal entropy failure-injection hook written in this repository | User-owned C99 test plumbing | Deterministic success, failure-before-write, and partial-write failure coverage | Compiled only with `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS`; not installed or public ABI. |
| DER-17002 | Fresh internal entropy health-test skeleton written in this repository | User-owned C99 implementation skeleton | Exercise bounded startup/continuous duplicate-block state and failure transitions | Engineering candidate only; not a copied or finalized RCT/APT design. |
| DER-17003 | Fresh entropy negative/unit tests written in this repository | User-owned C99 test code | Cover source failure, partial output, zero length, oversized request, health failure, DRBG invalidation, and module error state | Synthetic in-memory patterns only; no operating-system entropy is stored or printed. |
| DER-17004 | Fresh P1-17 entropy design and CI checklist documents | User-owned documentation | Record test/release boundary, health-test limitations, CI lane, and log/evidence rules | No third-party health-test wording or implementation copied. |
| DER-17005 | Fresh entropy/health policy check and evidence workflow changes | User-owned CMake tooling | Verify hook default/guards, internal-only placement, PoC clean-build settings, and evidence review records | Engineering guardrail only; not official validation evidence. |
| DER-17006 | Fresh PoC clean Release build boundary changes | User-owned CMake tooling | Rebuild SDK with tests/hooks off and internal symbol hiding on; scan headers/exports | Does not package internal entropy headers, hooks, captured entropy, or seed material. |

## P1-17 Clean-room Notes

- No third-party entropy provider, health-test implementation, threshold, test vector, or vendor CI workflow was imported.
- Tests use fresh synthetic byte patterns in memory only; actual entropy output is neither stored as a fixture nor written to evidence.
- The duplicate-block checks are explicitly limited to wiring/state-transition candidates pending selection and review of the actual noise-source boundary.
- Logs and evidence remain status-only and exclude entropy material, seed material, DRBG state, generated secret bytes, private keys, tokens, customer data, and internal deployment details.
- The test hook is internal, compile-time gated, disabled by default, and excluded from clean Release/PoC builds.

## P1-18 Entropy Source and Operational Policy Inputs

| ID | Source | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-18001 | NIST SP 800-90B, public final publication | Public standard | Terminology and review basis for noise-source health tests, startup/continuous testing, and pre-conditioning placement | No implementation code, threshold, or test vector copied. |
| DER-18002 | NIST SP 800-90C, public final publication | Public standard | Review basis for entropy-source failure reporting and RBG construction boundaries | Used as a design reference only. |
| DER-18003 | Linux `getrandom(2)` public manual page | Public platform documentation | Confirm provider call/partial-return/error considerations for the Linux branch | Existing fresh C99 wrapper retained; no third-party source code copied. |
| DER-18004 | Microsoft `BCryptGenRandom` public documentation | Public platform documentation | Confirm system-preferred provider selection and status mapping questions for the Windows branch | Existing fresh C99 wrapper retained. |
| DER-18005 | Apple `SecRandomCopyBytes` public documentation | Public platform documentation | Confirm default provider call and status mapping questions for the Apple branch | Existing fresh C99 wrapper retained. |
| DER-18006 | Fresh internal source-class/health-owner/restart-policy metadata | User-owned C99 implementation | Keep OS-provider output separate from direct raw-noise engineering callbacks | Internal only; no installed ABI. |
| DER-18007 | Fresh process identity guard and unit/fork tests | User-owned C99 implementation/test code | Require reseed after inherited source-backed DRBG state crosses a detected process boundary | Process marker is non-secret metadata; tests log status only. |
| DER-18008 | Fresh P1-18 design, CI, evidence, policy-check, and PoC package updates | User-owned documentation/tooling | Record and enforce the operational boundary | Engineering guardrails only. |

## P1-18 Clean-room Notes

- Public standards and official platform documentation were used to establish terminology and boundary questions only.
- No third-party entropy-source implementation, RCT/APT implementation, cutoff, sample data, process-guard code, or CI workflow was copied.
- No operating-system provider output is stored as a fixture or written to evidence.
- P1-18 process-marker tests use process identifiers as non-secret control metadata and do not treat them as entropy.
- Direct noise-source selection and final source-specific health-test parameters remain TODO / confirmation required.

## P1-19 Public Random-Service Implementation Inputs

| ID | Source | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-19001 | Existing user-owned HMAC_DRBG-SHA-256 core and P1-18 source-backed process guard | User-owned implementation | Implement the previously reserved public random lifecycle without adding a new algorithm | Existing internal interfaces were hardened rather than copied from an external implementation. |
| DER-19002 | Fresh `src/drbg/random.c` P1-19 implementation | User-owned C99 implementation | Platform entropy-backed init/reseed and source-backed generate lifecycle | No provider output, seed material, DRBG state, or generated bytes are logged. |
| DER-19003 | Fresh `tests/unit/random_service_tests.c` | User-owned C99 test code | Lifecycle, invalid input, cross-API context, output clearing, and POSIX process-change regression | Generated bytes are used only in transient local buffers and are not printed or retained. |
| DER-19004 | Fresh `tests/negative/random_service_failure_injection_tests.c` | User-owned C99 test code | Deterministic provider success, failure-before-write, partial-write failure, and reseed retry coverage | Compiled only in the dedicated internal hook build. |
| DER-19005 | Fresh P1-19 random-service design, CI, evidence, policy, install-smoke, and PoC updates | User-owned documentation/tooling | Record and enforce the public lifecycle and support limitations | Engineering guardrails only; not official validation evidence. |
| DER-19006 | Existing public platform API documentation recorded in P1-18 | Public platform documentation | Retain the reviewed Linux, Windows, and Apple provider call boundaries | No platform implementation source code copied. |

## P1-19 Clean-room Notes

- P1-19 adds no third-party random-service, DRBG, process/fork, or provider implementation code.
- The public service is assembled from user-owned internal entropy and HMAC_DRBG-SHA-256 components already present in the repository.
- Tests use transient synthetic hook patterns or operating-system provider output in memory only; output is not stored as a fixture, printed, or included in evidence.
- Prediction resistance, VM snapshot/restore detection, suspend/resume detection, and direct raw-noise health testing remain unsupported or TODO.
- No secret key, entropy capture, seed/nonce material, DRBG state, generated byte string, private key, token, customer data, or internal deployment detail is added to repository artifacts.

## P1-20 Random Lifecycle and Portability Hardening Inputs

| ID | Source | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-20001 | NIST SP 800-90A Revision 1, public final publication | Public standard | Review basis for the existing HMAC_DRBG reseed interval and reseed-required boundary | Design reference only; no third-party implementation code copied. |
| DER-20002 | NIST SP 800-90C final publication | Public standard | Review terminology for RBG construction, source-backed state, and operational duplication questions | Design reference only; no implementation or validation wording copied. |
| DER-20003 | Fresh `tests/stress/random_lifecycle_stress_tests.c` | User-owned C99 test code | Repeated public init/generate/reseed/uninit coverage | Output is transiently zeroized and never printed or stored. |
| DER-20004 | Fresh `tests/negative/random_reseed_limit_tests.c` and private counter hook | User-owned C99 test code/plumbing | Deterministic reseed-limit, failed-reseed persistence, recovery, and no-wrap regression | Compiled only when both dedicated test-hook options are enabled; absent from Release/PoC builds. |
| DER-20005 | Fresh process-marker fail-closed changes and deterministic tests | User-owned C99 implementation/test code | Disable the public source-backed random service when process identity cannot be obtained | Process identifiers are non-secret control metadata and are not used as entropy. |
| DER-20006 | Fresh VM snapshot/clone/suspend policy and hosted status review program | User-owned documentation/C99 test code | Record operational limitations and collect status-only runner metadata | Does not call a VM vendor API or store random output. |
| DER-20007 | Fresh 53-symbol ABI-0 baseline and CMake comparison tooling | User-owned ABI/tooling artifact | Detect missing or unexpected clean Release public symbol names | Baseline changes require explicit API/ABI review; tooling does not silently rewrite it. |
| DER-20008 | Fresh P1-20 CI, evidence, PoC, policy, and roadmap updates | User-owned documentation/tooling | Enforce hook, package, non-disclosure, and review boundaries | Engineering guardrails only; not official validation evidence. |

## P1-20 Clean-room Notes

- No third-party DRBG wrapper, reseed-counter test hook, process guard, VM checkpoint integration, symbol scanner implementation, or hosted CI workflow was copied.
- Public standards were used only to review terminology and the already selected reseed interval.
- Lifecycle and reseed-limit tests use transient in-memory buffers; generated output, provider output, entropy, seed material, and DRBG state are not retained.
- VM snapshot/restore, clone, checkpoint, and suspend/resume behavior is documented as an integration boundary rather than implemented by copying vendor-specific code.
- The exported-symbol baseline contains only public symbol names and no binary, private API, secret, or customer information.

## P1-21 Concurrency, Sanitizer, and Build-Identity ABI Inputs

| ID | Source | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-21001 | ISO C99 language and CMake project configuration already used by this repository | Public standard / project-owned build design | Cross-platform test and sanitizer option plumbing | No third-party implementation copied. |
| SRC-21002 | Public Win32 thread API concepts (`CreateThread`, `WaitForSingleObject`, `CloseHandle`) | Public platform API | Windows branch of the project-owned CSP/HMAC concurrency regression | API use only; no external source code or proprietary test structure copied. |
| SRC-21003 | Public POSIX pthread API concepts | Public platform API | POSIX branch of the same project-owned concurrency regression | API use only. |
| SRC-21004 | GCC/Clang sanitizer command-line interface concepts | Public compiler documentation concept | `address-undefined` diagnostic build mode | Diagnostic tooling only; not a security certification or production build requirement. |
| DER-21005 | Fresh P1-21 concurrency test, status-only platform review, ABI layout regression, policy scripts, docs, CI, evidence, and PoC updates | User-owned generated project material | P1-21 implementation and review guardrails | No keys, MAC output, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data retained. |

## P1-21 Clean-room Notes

- The Win32 and pthread test branches were written specifically for AnoCrypto-C from public API concepts.
- No third-party concurrency harness, sanitizer suppression file, internal company test, or proprietary ABI layout was copied.
- The required sanitizer lane is engineering diagnostics only. Clang runtime and ThreadSanitizer remain TODO pending reviewed hosted results.
- The build-identity ABI file records the existing AnoCrypto-C field order; it does not import an external ABI design or make a broad compatibility claim.


## P1-22 SHA-512-family Implementation Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-22001 | NIST FIPS 180-4 already recorded as SRC-1001 | Public standard and public examples | SHA-384/SHA-512 IVs, round constants, padding, length encoding, and public digest vectors | Specification and public vector input only; no sample implementation imported. |
| SRC-22002 | RFC 4231 already recorded as SRC-1002 | Public RFC and public vectors | HMAC-SHA-384/HMAC-SHA-512 cases 1, 2, 3, 4, 6, and 7 | Synthetic public test keys only; not operational secrets. |
| DER-22001 | Fresh `src/hash/sha512.c` and `src/internal/ac_sha512_internal.h` written in this repository | User-owned C99 implementation | Shared SHA-512-family core and SHA-384 derivation | No third-party implementation code, names, comments, or build structure imported. |
| DER-22002 | Fresh P1-22 hash/HMAC dispatch and startup self-test updates | User-owned project code | Public one-shot/streaming and startup KAT integration | Existing public symbols retained; internal context layout remains private. |
| DER-22003 | Fresh `tests/kat/sha512_family_kat_tests.c` and `tests/negative/sha512_family_api_negative_tests.c` | User-owned generated test code | Public-vector correctness and negative API regression | Test output is status-only; digest and MAC values are not printed. |
| DER-22004 | Fresh P1-22 policy, evidence, PoC, install-smoke, CI, and documentation updates | User-owned project material | Keep implementation, KAT, package, and evidence boundaries aligned | No keys, entropy, seed material, DRBG state, generated random values, private keys, tokens, or customer data retained. |

## P1-22 Clean-room Notes

- P1-15 reserved-scope files and behavior are retained only as historical records; P1-22 replaces the active reserved test and policy with implementation/KAT checks.
- SHA-384/SHA-512 constants and expected outputs are limited to the recorded public specification and RFC sources.
- No external SHA-512-family implementation source, test harness, provider wrapper, or proprietary validation asset was copied.
- P1-23 selects OpenSSL 3.x through a separate provider/license/evidence decision. The dependency remains test-only and no provider output or implementation code is committed.

## P1-23 Differential Validation and Portability Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-3201 | OpenSSL 3.x public headers and `libcrypto` through CMake `OpenSSL::Crypto` | Public open-source test dependency | Independent test-only SHA-2 and HMAC differential provider | OpenSSL 3.0+ is published under Apache License 2.0 by the OpenSSL project. Review the exact packaged license before redistribution. No implementation source is copied. |
| SRC-3202 | OpenSSL public EVP digest and EVP_MAC-HMAC documentation | Public documentation | API usage and provider return-code handling for the differential test executable | Documentation concepts only; no OpenSSL sample or test code copied. |
| DER-3201 | Fresh `tests/differential/sha2_hmac_openssl_differential_tests.c` | User-owned generated C99 test source | In-memory deterministic comparison of public AnoCrypto-C SHA-2/HMAC outputs with OpenSSL 3.x | Prints provider/version/count/status only; output bytes and generated keys/messages are not retained. |
| DER-3202 | Fresh `tests/operational/sha512_portability_review.c` | User-owned generated C99 test source | Alignment, endian, representation, and streaming portability review | Status-only output; no digest, MAC, or key bytes. |
| DER-3203 | Fresh P1-23 provider decision, portability policy, CI checklist, evidence template, and CMake policy tooling | User-owned documentation/tooling | Fix provider provenance, test/product separation, 32-bit lane, and review boundaries | Workflow definitions require actual hosted artifact review before support wording changes. |

## P1-23 Clean-room Notes

- No OpenSSL implementation code, constants, test vectors, test harness source, or result bytes were copied into AnoCrypto-C.
- OpenSSL is a test-only independently built provider and is not linked into the AnoCrypto-C product library, installed SDK, or customer PoC SDK.
- Differential messages and HMAC keys are deterministic local fixtures generated at runtime and are not operational secrets.
- Differential logs contain provider identity, version, comparison count, algorithm/path/length metadata on failure, and status only.
- The Linux 32-bit workflow is a portability lane definition until an actual hosted result is reviewed.
- Differential agreement, sanitizer results, and portability status do not establish official validation, side-channel resistance, or completed KCMVP status.


## P1-24 block-cipher entry gate

- Used only public standards identifiers and project-owned design requirements.
- Selected FIPS 197 and NIST SP 800-38A as future public vector sources; no vectors or implementation code were copied in P1-24.
- No private certification material, customer material, key, plaintext, ciphertext, or proprietary implementation was used.


## P1-25 block-cipher public sources

| ID | Source | Use | Boundary |
|---|---|---|---|
| SRC-1401 | NIST FIPS 197, Advanced Encryption Standard | AES-128/192/256 algorithm definition and cipher examples used for startup/extended KAT transcription. | Public standard; no external implementation code copied. |
| SRC-1402 | NIST SP 800-38A, CBC mode examples | CBC-AES128/192/256 four-block encrypt/decrypt fixtures. | Public standard; no private submission data. |
| SRC-1403 | OpenSSL 3.x public EVP cipher API | Test-only AES-CBC differential provider. | Build/test dependency only; excluded from product library, installed SDK, and PoC runtime. |

## P1-26 AES/CBC Hardening Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| DER-3401 | Fresh internal AES context validation and fail-closed cipher reinitialization changes | User-owned C99 implementation | Harden caller-owned context lifecycle without changing the public API | No external implementation, table, code structure, or private material imported. |
| DER-3402 | Fresh AES hardening, lifecycle stress, unaligned/streaming, and CSP HMAC/AES concurrency tests | User-owned C99 test code | Exercise corrupt/incomplete context rejection, 128 lifecycle iterations, variable chunks, exact in-place operation, and shared read-only key handles | Test data is deterministic and transient; key, IV, plaintext, ciphertext, and intermediate values are not printed or retained. |
| SRC-3403 | OpenSSL 3.x public EVP cipher API through the P1-23 reviewed test-only provider boundary | Public open-source test dependency | Expand independent AES-CBC comparison to 96 deterministic cases per key size and multiple AnoCrypto-C processing paths | No OpenSSL implementation or test harness source is copied. Product and PoC binaries remain independent of OpenSSL. |
| DER-3404 | Fresh compiler-output capture CMake tool and review plan | User-owned tooling/documentation | Retain compiler, target, binary hash, symbol markers, and disassembly for manual AES review | The tool makes no automated constant-time or side-channel conclusion. |
| DER-3405 | Fresh Linux 32-bit and s390x cross-run workflow/toolchain definitions | User-owned build/CI material using public compiler interfaces | Prepare status-only portability evidence lanes | Workflow presence is not a successful hosted result; actual artifacts require review. |
| DER-3406 | Fresh P1-26 policy, evidence, PoC, clean-room, and roadmap updates | User-owned project material | Keep implementation, package, ABI, logging, and claims boundaries aligned | Public cipher API remains eight functions and the clean Release baseline remains 61 symbols. |

## P1-26 Clean-room Notes

- No third-party AES core, S-box table, key-schedule source, compiler-analysis script, concurrency harness, or proprietary test structure was imported.
- Public FIPS 197 and NIST SP 800-38A vectors recorded in P1-25 remain the correctness source; the enlarged OpenSSL comparison corpus is generated locally from deterministic non-secret recipes.
- Compiler-output artifacts contain machine instructions and build metadata for the AnoCrypto-C binary under review, not runtime key/IV/plaintext/ciphertext values. They are internal evidence and require human disposition.
- Keys, expanded schedules, IVs, plaintext, ciphertext, intermediate AES state, entropy, seed material, DRBG state, generated random output, private keys, signing keys, tokens, customer data, and internal deployment information remain excluded from logs and packages.
- P1-26 does not add CTR, GCM, padding, CMAC/GMAC, hardware dispatch, or a new public API.

## P1-27 CMAC Entry Gate Inputs

| ID | Input | Classification | Use | Boundary |
|---|---|---|---|---|
| PUB-3501 | NIST SP 800-38B AES-CMAC specification and public examples | Public standard | Define future CMAC algorithm and KAT planning scope | No third-party implementation code copied. |
| DER-3502 | Existing project-owned AES/CBC, CSP, API/ABI, evidence, PoC, and policy structure | User-owned project material | Derive a narrow CMAC entry gate and dedicated key-purpose requirement | P1-27 adds no CMAC implementation or public ABI. |
| DER-3503 | Fresh P1-27 CMake policy, documentation, evidence-template, PoC-note, roadmap, and clean-room updates | User-owned project material | Keep no-implementation and 61-symbol boundaries mechanically checked | Policy output contains status only and no cryptographic values. |

## P1-27 Clean-room Notes

- The decision uses public algorithm requirements and fresh project-owned text.
- No external CMAC source, subkey-generation routine, KAT harness, function
  naming scheme, internal layout, build structure, or private validation
  material was copied.
- P1-27 records planned public vector provenance but stores no CMAC key,
  message, subkey, intermediate, or tag fixture.
- AES-CTR, truncation, GCM/GMAC, and protocol constructions remain deferred.

## P1-28 AES-CMAC Implementation Inputs

| ID | Input | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-1501 | NIST SP 800-38B and the public AES-CMAC example set for AES-128/192/256 | Public standard and public examples | Algorithm definition, startup KAT, and 12 extended KAT cases | No third-party implementation source or private submission data copied. |
| SRC-1502 | OpenSSL 3.x public EVP_MAC CMAC API documentation and separately linked `libcrypto` | Public documentation and test-only dependency | Independent differential comparison | Product library, installed SDK, and PoC runtime remain independent of OpenSSL. |
| DER-3601 | Fresh `src/mac/cmac.c` and private CMAC context design | User-owned C99 implementation | Full-tag AES-CMAC compute/verify/streaming with dedicated CSP key purpose | No external CMAC implementation, subkey routine, layout, or naming structure copied. |
| DER-3602 | Fresh KAT, negative, differential, install, and PoC tests | User-owned C99 test material | Verify public vectors, lifecycle, wrong-purpose handles, authentication failure, and provider agreement | Keys/messages/tags are public or deterministic test material and are not logged or retained in evidence. |
| DER-3603 | Fresh P1-28 API/ABI, policy, evidence, package, CI, roadmap, and clean-room updates | User-owned project material | Keep the eight-function CMAC API, dedicated key purpose, and 69-symbol baseline mechanically aligned | Evidence contains status, source/provider identity, comparison count, and metadata only. |

| DER-3604 | Fresh standalone CTest and installed-package smoke evidence runners plus evidence-artifact reuse wiring | User-owned CMake tooling | Prevent nested build/test runner coupling while preserving status-only evidence collection | No cryptographic value is added to logs; direct script fallback remains available. |

## P1-28 Clean-room Notes

- CMAC source, internal context, tests, and build wiring were written fresh from public algorithm requirements and existing project-owned C99 conventions.
- The 12 expected tags are transcribed from the recorded public NIST example source; no private or unexplained vector file is used.
- OpenSSL is called only through public APIs in a separately linked test executable; no OpenSSL implementation or test harness source is copied.
- CMAC keys, messages, tags, subkeys, chaining state, AES round keys, entropy, seed material, DRBG state, generated random output, private keys, tokens, customer data, and internal deployment information are excluded from logs, evidence summaries, and customer packages.
- P1-28 does not add tag truncation, CTR, GCM/GMAC, persistence, export, protocol formatting, or a completed side-channel claim.


## P1-29 AES-CMAC Hardening Inputs

| ID | Source | Classification | Use | Boundary |
|---|---|---|---|---|
| DER-3701 | Fresh project-owned CMAC context/lifecycle/alias hardening and C99 tests | User-owned project material | Strengthen private state integrity, cleanup, stress, and concurrency behavior | No third-party CMAC implementation or private validation material copied. |
| DER-3702 | Existing project-owned AES/CMAC API, CSP, CI, evidence, and PoC structure | User-owned project material | Preserve the eight-function CMAC API and 69-symbol baseline while adding hardening evidence | Internal layouts and compiler artifacts remain outside the public/PoC surface. |
| SRC-1502-R1 | OpenSSL 3.x public EVP_MAC CMAC interface used by the existing test-only provider | Public documentation and separately linked test dependency | Expand deterministic differential coverage to 1,728 comparisons | Product library, installed SDK, and customer PoC remain independent of OpenSSL. |

### P1-29 notes

- New operational and lifecycle inputs are generated deterministically at runtime and are not retained as secret fixtures or evidence values.
- Compiler-output capture records binary/compiler/target metadata and disassembly only; it does not execute with or retain CMAC key, message, or tag values.
- No key, tag, subkey, chain, AES schedule, entropy, seed, DRBG state, random output, private key, token, customer data, or internal deployment information is added to logs or packages.


## P1-30 release compatibility inputs

| Input | Classification | Use | Notes |
|---|---|---|---|
| Project-owned `0.1.0-p1-29-cmac-hardening` public headers and exported-symbol list | User/project owned | Frozen compatibility baseline | Exact copies are stored under `abi/baselines/`; no third-party implementation is introduced. |
| Current AnoCrypto-C C99 public headers and CMake package configuration | User/project owned | Normalized source/API and package-version comparison | Version macros are reviewed separately from stable declarations. |
| CMake public documentation and project-authored test harnesses | Public documentation / project owned | Package compatibility, manifest, and negative-test design | No external source code or proprietary fixture is copied. |
| Git tag and commit metadata supplied by CI | Build metadata | Optional tagged-release identity check | Metadata is status-only; tag authorization/signature review remains TODO. |

P1-30 hosted artifacts retain only status, counts, versions, target/compiler metadata, file sizes, and hashes. Cryptographic values and customer/internal deployment data remain excluded.

## P1-31 Release Provenance and Source SBOM Inputs

| ID | Input | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-3901 | SPDX Specification 2.3 public document and JSON field model | Public standard | Select the source-inventory format, package/file checksums, package verification code, and `NOASSERTION` boundaries | No SPDX implementation source, third-party generator code, or private SBOM data copied. |
| SRC-3902 | Public SLSA/in-toto provenance concepts and GitHub artifact-attestation documentation | Public documentation | Distinguish a hash/provenance record from an authenticated attestation and keep hosted attestation as a later policy decision | P1-31 does not claim a SLSA level and does not enable hosted attestation or signing. |
| DER-3901 | Fresh CMake source-SBOM generator and unsigned provenance bundle create/verify tools | User-owned project tooling | Bind the source archive, source-file hashes, public-header manifest, ABI symbol list, hosted disposition, and authorization state | No key, token, credential, signature, or external implementation is included. |
| DER-3902 | Fresh tamper, unlisted-file, unsafe-path, false-signing, false-authorization, tag, and hosted-disposition regression fixtures | User-owned CMake tests | Enforce fail-closed release metadata handling | Fixtures use public repository files and status strings only. |
| DER-3903 | Fresh P1-31 release, CI, evidence, PoC, roadmap, and clean-room documentation | User-owned project material | Keep source inventory, unsigned signing state, and review TODO boundaries explicit | No completed hosted review, signing, validation, or external-release claim is recorded. |

## P1-31 Clean-room Notes

- The SPDX JSON writer was created fresh from public field requirements and existing project-owned CMake conventions.
- No third-party SBOM generator implementation, SLSA generator, signing implementation, workflow fragment, private key, token, signature, or proprietary release process was copied.
- The source SBOM contains repository-relative paths and hashes only; it does not retain source contents, cryptographic values, customer data, or internal deployment information.
- Public documentation is used to distinguish unsigned hash metadata from authenticated provenance; P1-31 does not claim an attestation level.
- The rooted `.gitignore` is fresh project-owned build hygiene: it excludes generated build/hosted-review directories only, keeps ordinary untracked source visible, and avoids treating CI build output as a source change.
- Actual hosted artifact review, signer authorization, cryptographic verification, timestamping, and external-release approval remain TODO.

## P1-32 Offline Release-Signing Handoff Inputs

| ID | Input | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-4001 | RFC 8032 public Ed25519 specification | Public standard | Select Ed25519 and its fixed 64-byte signature representation | Specification concepts only; no implementation or private test material copied. |
| SRC-4002 | RFC 3161 public time-stamp protocol specification | Public standard | Define a timestamp response over the detached signature before external release | Protocol concepts only; no production TSA credential, response, or trust anchor included. |
| SRC-4003 | OpenSSL 3 public `pkey`, `pkeyutl`, and `ts` command documentation | Public documentation | Public-key DER conversion, Ed25519 verification, and RFC 3161 fixture/verification command behavior | Public CLI concepts only; no OpenSSL source, sample key, or vendor workflow copied. OpenSSL is not linked into the product or PoC runtime. |
| SRC-4004 | Public X.509 SubjectPublicKeyInfo/DER concepts, including RFC 5280 and RFC 8410 | Public standards | Define the approved public-key identity as SHA-256 over SPKI DER | Format concepts only; no certificate, key, or external fingerprint imported. |
| SRC-3902-R1 | Public GitHub artifact-attestation documentation already reviewed in P1-31 | Public documentation | Classify hosted attestation as optional secondary evidence | GitHub attestation is not the signing trust root or a substitute for offline signature/timestamp verification. |
| DER-4001 | Fresh P1-32 release-signing policy and unprovisioned signer/timestamp records | User-owned project material | Record exact subject, signature/fingerprint/timestamp formats, role separation, custody boundary, and fail-closed production state | Records contain policy identifiers and `TODO`/`UNPROVISIONED` values only. |
| DER-4002 | Fresh CMake handoff creator, handoff checker, and detached-signature verifier | User-owned project tooling | Bind and verify provenance, signer/TSA policy, authorization roles, signature, and timestamp inputs | Tooling creates no production key, token, signature, or timestamp response. |
| DER-4003 | Fresh key-free regressions and explicit opt-in Debug-only ephemeral OpenSSL fixtures | User-owned project tests | Exercise tamper, identity separation, Ed25519 signature, and RFC 3161 acceptance/rejection paths | Fixture keys, certificates, signatures, and timestamp files are generated only in the test build tree and removed after testing. |
| DER-4004 | Fresh P1-32 CI, evidence, PoC, roadmap, and clean-room updates | User-owned project material | Keep handoff readiness, artifact exclusions, and remaining release operations explicit | No hosted run, tag, production signature, human approval, or external authorization is claimed. |
| DER-4005 | Fresh prepared-status evidence assembler and sensitive-signing-material scanner | User-owned project tooling | Keep the final aggregate collector status-only, hash-bound, fail-closed, and free of key/token fixtures | The assembler launches no child build/test process; source/archive/install/PoC/evidence scans retain no sensitive values. |

## P1-32 Clean-room Notes

- The P1-32 policy and CMake tools were written for AnoCrypto-C from public format and command-interface concepts; no third-party signing implementation or proprietary release procedure was copied.
- The approved subject is exactly `RELEASE_ARTIFACTS.sha256`; the detached signature format is Ed25519 raw 64-byte, and the public-key identity is SHA-256 over SPKI DER.
- RFC 3161 timestamp verification covers `RELEASE_ARTIFACTS.sha256.ed25519`; production TSA policy and trust-anchor values remain unprovisioned.
- Security Reviewer and Product Release Owner identities are separate project authorization roles. GitHub artifact attestation remains optional secondary evidence only.
- Production keys, tokens, signatures, timestamp responses, certificates, trust anchors, approver identities, and credentials are not included. The repository remains unsigned and externally unauthorized.

## P1-33 Clean Tagged-Release Rehearsal and Hosted Review Inputs

| ID | Input | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-4101 | Public Git command documentation for clone, tag, rev-parse, status, and worktree identity | Public documentation | Define isolated synthetic tag creation, tag/commit/`HEAD` resolution, and clean-worktree checks | Command semantics only; no third-party release script, tag, credential, or repository history copied. |
| SRC-3902-R2 | Public GitHub Actions artifact metadata and attestation concepts already reviewed in P1-31/P1-32 | Public documentation | Separate workflow/artifact identity from human review and keep attestation secondary | Workflow presence, hashes, or attestation do not become hosted review or release authorization. |
| DER-4101 | Fresh clean-checkout tagged-release rehearsal and fail-closed identity checks | User-owned project tooling | Exercise synthetic tag, commit, `HEAD`, worktree, and evidence reproduction without modifying the user repository | No user-repository tag, tag signature, history rewrite, push, or production release action is performed. |
| DER-4102 | Fresh hosted artifact review records and negative regressions | User-owned project tooling/tests | Fix required lane/platform combinations and reject incomplete, mismatched, unsafe, or falsely approved artifacts | Fixtures are explicitly synthetic and cannot be recorded as actual hosted execution. |
| DER-4103 | Fresh P1-33 release, CI, evidence, PoC, roadmap, and clean-room documentation | User-owned project material | Keep `NOT_RUN`/`UNREVIEWED`, unsigned/BLOCKED, and actual-versus-synthetic boundaries explicit | No completed hosted review, actual tag, production signature, timestamp, or external authorization is claimed. |

## P1-33 Clean-room Notes

- The rehearsal and hosted-review tooling is fresh project-owned CMake/Git orchestration based on public command and metadata concepts.
- A synthetic local tag is created only in an isolated temporary repository and is never represented as a user-repository or production tag.
- Actual hosted records require artifact hash, workflow commit/ref, platform, compiler, architecture, reviewer, UTC review date, disposition, and `HOSTED_RUNNER` origin.
- Synthetic fixtures remain `SYNTHETIC_LOCAL`; unavailable hosted execution remains `NOT_RUN`; downloaded but undispositioned output remains `UNREVIEWED`.
- Production signer/TSA remain `UNPROVISIONED`; signing remains `UNSIGNED`, handoff `BLOCKED`, and external release `NO`.


## P2-01 Readiness Consolidation Inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-4201 | User-provided AnoCrypto-C P2 transition prompt and CURRENT ONLY source pack | User-owned project instruction/material | Current identity, P1-34 blocker, ABI constraints, P2-01 deliverables, status taxonomy, and roadmap direction | Requirements and project facts only; no external implementation imported. |
| SRC-4202 | KISA public “암호모듈 검증” process overview, checked 2026-07-14 | Public official information | High-level process, testing/validation role separation, and referenced standards checkpoint | No normative standard text or acceptance conclusion copied; exact applicability remains external confirmation. |
| SRC-4203 | KISA public announcement for the “암호모듈 제출물 작성 안내서”, checked 2026-07-14 | Public official information | Basic design, detailed design, configuration-management, and test document category checkpoint | Current templates/revisions and contents remain external confirmation. |
| DER-4204 | P2-01 readiness, traceability, consultation, submission-inventory, roadmap, and blocker documents written in this repository | User-owned generated project artifacts | Validation-readiness consolidation and future work control | Engineering planning only; no institution result or formal approval implied. |
| DER-4205 | Case-sensitive CMake package-template filename normalization | User-owned repository correction | Make the existing `cmake/AnoCryptoCConfig.cmake.in` reference valid on case-sensitive filesystems | No installed package identity, public header, symbol, or ABI change. |

## P2-01 Clean-room Notes

- No algorithm implementation, KAT value, public API, third-party source, or official submission template was copied into the repository.
- Official-source checkpoints are used only to categorize questions and documents; unresolved interpretation is marked `EXTERNAL_CONFIRMATION_REQUIRED`.
- The P1-34 billing failure is retained as an external blocker, not converted into a code defect or successful hosted result.
- P2-01 documents contain status, path, count, hash/commit metadata, and planning information only; no key, entropy, seed, random output, private signing material, token, or customer data is added.

## P2-02 Boundary and Operating-Environment Freeze Inputs

| ID | Source | Classification | Use | Boundary |
|---|---|---|---|---|
| SRC-4301 | KISA public KCMVP overview and current public notice for the revised submission guide, checked 2026-07-14 | Public official metadata | Confirm current standards/process checkpoint and consultation need | No normative standard or attached guide text copied; exact applicability remains external confirmation. |
| SRC-4302 | Ubuntu official 24.04.4 LTS release metadata and Ubuntu package metadata for GCC 13, CMake, and glibc, checked 2026-07-14 | Public official distribution metadata | Select and identify one reproducible initial operating-environment family | Package/source code not copied; exact installed package hashes are TODO for the candidate run. |
| DER-4303 | Existing AnoCrypto-C CMake, runtime source layout, ABI baseline, portability policies, and P2-01 gap/traceability records | User-owned project artifacts | Derive the candidate module boundary, exclusions, interfaces, and configuration | No public API or cryptographic implementation change. |
| DER-4304 | `KCMVP_MODULE_BOUNDARY_SPEC.md`, `KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`, candidate CMake record, and validation-candidate policy checker | Fresh project-owned artifacts | P2-02 internal freeze and fail-closed configuration control | Engineering planning/evidence only; no formal validation acceptance implied. |

## P2-02 Clean-room Notes

- No official submission template, proprietary consultation answer, standard text, third-party implementation, or environment image was copied into the repository.
- Public OS/toolchain metadata is used only to identify an internal candidate; exact image, CPU, kernel, package, and binary evidence must be captured from a future controlled run.
- The selected environment does not expand runtime dependencies to OpenSSL, C++, test providers, or signing infrastructure.
- P2-02 adds no algorithm, public header, function, enum, exported symbol, KAT value, or sensitive material.
- The P1-34 billing blocker remains unchanged and no local result is represented as hosted execution.

## P2-03 Self-test, Integrity, and Recovery Design Inputs

| Record | Input | Source class | Use | Boundary |
|---|---|---|---|---|
| DER-4401 | Existing project-owned `module_state.c`, `selftest.c`, public headers, tests, and P2-01/P2-02 records | Project-owned implementation/evidence | Observe current startup KAT, structural checks, service gate, fatal latch, shutdown/reinit, and public ABI | No external implementation copied. |
| DER-4402 | Public KCMVP process/submission landing information | Public official guidance | Confirm that consultation/submission planning remains external and controlled | No normative standard text copied into code or KAT fixtures. |
| DER-4403 | Project-authored HMAC-SHA-256 single-shared-object integrity candidate | Fresh project-owned design | Consultation and implementation planning only | No key, expected tag, metadata binary, or verifier implementation added. |
| DER-4404 | Finite-state, failure/recovery, policy, checker, roadmap, and evidence records | Fresh project-owned artifacts | P2-03 internal design freeze and fail-closed claims control | Engineering evidence only; no formal acceptance implied. |

## P2-03 Clean-room Notes

- No external proprietary code, binary, module-integrity implementation, private standard text, or confidential test material was used.
- No integrity key, tag, signing key, entropy sample, CSP value, or customer data was added.
- P2-03 adds no algorithm, public header, function, status value, enum value, exported symbol, or public data layout.
- The HMAC-based integrity candidate reuses an already implemented internal primitive only as a future design; implementation remains gated by consultation and controlled key-management decisions.

## P2-04 Entropy and DRBG Responsibility Freeze

| ID | Input / artifact | Classification | Purpose | Boundary |
|---|---|---|---|---|
| DER-45001 | Fresh P2-04 entropy/DRBG specification and matrices | User-owned documentation | Freeze provider/source/health/lifecycle responsibility and evidence boundaries | Internal readiness material; not institution acceptance. |
| DER-45002 | Fresh machine-readable entropy/DRBG policy and checker | User-owned CMake tooling | Detect candidate, limits, ABI, blocker, and claims drift | No secret/provider/generated bytes processed. |
| DER-45003 | Fresh status-only operational review executable | User-owned C99 test code | Report provider identity, classifications, limits, capability, and statuses | No entropy, nonce, DRBG state, or generated bytes logged. |
| DER-45004 | Public Linux `getrandom(2)` documentation and public NIST entropy-source/DRBG guidance | Public official/primary references | High-level design and consultation checkpoint | No third-party source code or normative text copied. |

P2-04 introduces no public API or algorithm and preserves 14 headers, 69 symbols, and `ANOCRYPTO_C_0`. The direct-noise health scaffolding remains test-only engineering code and is not promoted as a validated RCT/APT or applied to conditioned OS output.

## P2-06 secure-coding and side-channel review inputs

| ID | Input | Classification | Usage | Notes |
|---|---|---|---|---|
| SRC-P206-001 | Existing AnoCrypto-C P2-05 source and evidence policy | User-owned project artifact | Source, warning, analyzer, control-flow, zeroization, and build-hardening review | No external implementation source imported. |
| SRC-P206-002 | GCC compiler diagnostics and instrumentation documentation | Public vendor documentation | Warning/analyzer and hardening option interpretation | Documentation only; local GCC 14 output is not OE-01 evidence. |
| SRC-P206-003 | NIST public side-channel methodology and AES implementation-attack publications | Public technical guidance | Threat taxonomy, evidence limitations, and consultation questions | No external crypto implementation or exploit code imported. |
| DER-P206-001 | Locally generated GCC/Clang warning, analyzer, disassembly, ELF, symbol, and hash records | User-owned generated evidence | Internal finding and regression disposition | Status/metadata only; not formal validation evidence. |

P2-06 does not import third-party implementation code, secret-bearing traces, production keys, customer data, or unpublished attack material.
