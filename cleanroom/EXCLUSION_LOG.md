# AnoCrypto-C Exclusion Log

Status: P1-16 draft
Date: 2026-07-08

## Purpose

This log records materials and categories that must not influence the anocrypto-c implementation, tests, documentation wording, file names, function names, constants, or build structure.

## Exclusion rules

| ID | Excluded material/category | Risk type | Required action | Notes |
|---|---|---|---|---|
| EXC-0001 | Materials with unclear ownership, unclear license, or unclear right to analyze | IP / clean-room risk | Do not inspect for implementation details; do not copy names, layout, comments, constants, tests, vectors, or build structure. | Use only after explicit written permission and manifest update. |
| EXC-0002 | Private employer, customer, vendor, or third-party code/documents | Confidentiality / trade-secret risk | Exclude from design input and repository contents. | Record only path/risk/action if accidentally discovered. |
| EXC-0003 | Internal certification submissions, test reports, vulnerability reports, or non-public assessment artifacts | Confidentiality / regulatory process risk | Exclude from design input and repository contents. | Use public standards and public vectors instead. |
| EXC-0004 | Secret keys, seeds, entropy captures, private scalars, tokens, certificate private keys | Secret material risk | Do not store, log, print, or include in tests. Delete from repo history if accidentally committed. | Test fixtures must use public non-secret vectors only. |
| EXC-0005 | Legacy names outside the anocrypto-c naming plan | Brand/API contamination risk | Do not introduce into new source, headers, examples, package names, or CMake targets. | Historical mention only if a future migration document explicitly needs it. |
| EXC-0006 | Assurance claims not yet backed by completed validation or formal review | Claims risk | Do not use in README, docs, comments, samples, release notes, or sales text. | Use KCMVP-oriented preparation language only. |

## Accidental discovery protocol

If excluded material is discovered:

1. Do not reproduce the content.
2. Record only the file path, risk category, and recommended action.
3. Remove it from the working tree if it was copied into the repository.
4. If it was committed, treat it as a repository history cleanup issue.
5. Add a legal/reviewer note before any further use.

## Open exclusions / TODO

- TODO: Define the internal approval process for source manifest changes.
- TODO: Define repository scanning rules for secrets and prohibited terminology.
- TODO: Define whether release evidence may contain logs with absolute build paths.


## P1 Foundation Review Notes

- No external implementation source files were imported for SHA-2, HMAC, or DRBG.
- No test vectors were imported.
- No secret keys, seeds, entropy samples, tokens, or certificate private keys were added.
- Algorithm processing APIs intentionally return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` until implementation source and test vector provenance are reviewed.

## P1-1 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1001 | Third-party SHA-2/HMAC implementation source code with unclear license or ownership | Code provenance risk | Excluded | P1-1 implementation is fresh C99 code based on public specifications only. |
| EX-1002 | Non-public KCMVP/CAVP submission materials or lab reports | Confidentiality / legal risk | Excluded | Public standards and public vectors only. |
| EX-1003 | Operational HMAC keys, seeds, entropy samples, tokens, private keys | Sensitive information risk | Excluded | Public RFC test-vector keys are allowed only as KAT fixtures. |

## P1-2 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1101 | Third-party KAT runner implementations or external hash/HMAC test harness code | Code provenance risk | Excluded | P1-2 test harnesses are fresh C99 code written in this repository. |
| EX-1102 | Non-public algorithm validation submissions, lab traces, or vendor test logs | Confidentiality / process risk | Excluded | P1-2 uses public vectors and local engineering logs only. |
| EX-1103 | Evidence bundles containing secrets, private keys, tokens, entropy captures, or private deployment details | Sensitive information risk | Excluded | Evidence script output must be reviewed before sharing. |
| EX-1104 | Treating local KAT/CTest pass logs as formal validation evidence | Claims risk | Excluded | Logs are engineering correctness evidence only. |

## P1-3 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1201 | Third-party DRBG implementation source code, including sample code from libraries or public repositories | Code provenance risk | Excluded | P1-3 HMAC_DRBG-SHA-256 implementation is fresh C99 source based on public specifications. |
| EX-1202 | Operational entropy captures, seeds, nonces, generated secrets, or customer random samples | Sensitive information risk | Excluded | DRBG tests use public synthetic vector material only. |
| EX-1203 | Treating public CAVP DRBG test-vector pass logs as a formal validation result | Claims risk | Excluded | The logs are engineering correctness evidence only. |
| EX-1204 | OS entropy source implementation without review of platform boundary and evidence requirements | Design/evidence risk | Deferred | P1-3 implements only the deterministic DRBG core. |

## P1-4 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1301 | Third-party entropy adapter, RNG wrapper, or OS random abstraction source code | Code provenance risk | Excluded | P1-4 adds only a fresh internal seam and OS-reserved stub. |
| EX-1302 | Operational entropy captures, generated random bytes, production seeds, private keys, HMAC keys, or tokens | Sensitive information risk | Excluded | Tests use public synthetic fixtures only and logs must not print bytes. |
| EX-1303 | Third-party keystore, key table, handle manager, or key export implementation source code | Code provenance risk | Excluded | P1-4 CSP table is a fresh in-memory lifecycle slice only. |
| EX-1304 | Treating callback entropy tests as operational entropy-source evidence | Claims/design risk | Excluded | Callback provider is test plumbing; OS entropy provider remains TODO. |


## P1-5 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1401 | Third-party CSP handle-manager, key table, slot allocator, or stale-handle implementation source code | Code provenance risk | Excluded | P1-5 stale-handle/exhaustion tests and handle behavior are fresh code in this repository. |
| EX-1402 | Source archives containing build outputs, generated evidence bundles, local caches, or copied third-party material | Provenance / disclosure risk | Excluded | Evidence source archive is generated from filtered repository source files. |
| EX-1403 | Install smoke logs containing secrets, operational entropy, generated secret bytes, private keys, tokens, customer paths, or internal deployment details | Sensitive information / disclosure risk | Excluded | P1-5 install smoke logs must remain status-only and be reviewed before sharing. |
| EX-1404 | Treating source archive hashes, CTest logs, or install smoke logs as formal validation results | Claims risk | Excluded | They are engineering traceability and correctness inputs only. |


## P1-6 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1501 | Third-party thread wrapper, lock abstraction, concurrent test harness, or CSP synchronization implementation source code | Code provenance risk | Excluded | P1-6 lock seam and tests are fresh code in this repository. |
| EX-1502 | Claiming broad thread-safety for caller-owned hash/HMAC/DRBG contexts | Support/claims risk | Excluded | P1-6 only protects selected global module/self-test/CSP state. |
| EX-1503 | Logs containing thread test key bytes, MAC output, generated DRBG bytes, entropy bytes, private keys, or tokens | Sensitive information risk | Excluded | P1-6 thread tests must remain status-only. |
| EX-1504 | Treating thread smoke tests as formal concurrency assurance or external review completion | Claims risk | Excluded | These are engineering tests only. |

## P1-7 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1601 | Third-party RNG wrappers, entropy abstraction libraries, random-service implementation code, or vendor SDK samples | Code provenance risk | Excluded | P1-7 platform entropy provider seam is fresh C99 code using documented OS API boundaries. |
| EX-1602 | Operational entropy captures, DRBG seeds, generated random bytes, private keys, tokens, customer secrets, or production random samples | Sensitive information risk | Excluded | Provider tests must record status and provider name only. |
| EX-1603 | Claiming entropy quality, validation completion, or platform suitability from a local provider smoke test alone | Claims risk | Excluded | Platform-specific review and evidence remain required before customer or validation-boundary commitments. |
| EX-1604 | Adding a public random byte API before provider policy, DRBG integration, prediction-resistance behavior, and validation-boundary review | API/support risk | Deferred | P1-7 keeps the provider seam internal. |

## P1-8 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1701 | Third-party public random-service API implementations, RNG wrappers, DRBG auto-seeding helpers, or provider abstraction code | Code provenance risk | Excluded | P1-8 helper and tests are fresh C99 code in this repository. |
| EX-1702 | Operational entropy captures, DRBG seeds, generated random bytes, production nonces, private keys, tokens, or customer random samples | Sensitive information risk | Excluded | P1-8 tests must not print or persist entropy/seed/generated output bytes. |
| EX-1703 | Adding public `ano_crypto_random_*` ABI before API/ABI, provider, reseed, fork-safety, and validation-boundary review | API/support risk | Deferred | P1-8 adds a design draft and internal helper only. |
| EX-1704 | Treating helper-driven DRBG tests or local OS entropy smoke tests as formal validation evidence | Claims risk | Excluded | These are engineering correctness and integration checks only. |

## P1-9 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1801 | Third-party HMAC_DRBG implementation source, DRBG test harness code, or random-service implementation code | Code provenance risk | Excluded | P1-9 boundary tests are fresh C99 code in this repository. |
| EX-1802 | Operational entropy captures, DRBG seeds, nonces, personalization strings from customer deployments, generated random bytes, private keys, or tokens | Sensitive information risk | Excluded | P1-9 fixtures are synthetic local regression data and test logs must remain status-only. |
| EX-1803 | Treating local deterministic DRBG boundary tests as formal validation evidence | Claims risk | Excluded | Boundary tests are engineering correctness checks only. |
| EX-1804 | Lowercase-only `anocryptoc` CMake package/target names that conflict with the product naming plan | Naming/API risk | Replaced | Use `AnoCryptoC` package and `AnoCryptoC::AnoCryptoC` target; keep `anocrypto_c` as the library file/target name. |


## P1-10 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-1901 | Third-party CI workflow files, release scripts, packaging jobs, or ABI checkers with unclear license or ownership | Code provenance risk | Excluded | P1-10 workflow and CMake policy checks are fresh skeletons. |
| EX-1902 | Treating reproducible-build notes as proof of byte-identical binaries across hosts | Claims risk | Excluded | Current notes are a draft procedure and gap list only. |
| EX-1903 | Treating exported symbol lists, CTest output, or install smoke logs as formal validation evidence | Claims risk | Excluded | These remain local engineering evidence pending external process review. |
| EX-1904 | Logs containing secret keys, entropy bytes, DRBG seeds, generated secret bytes, private keys, tokens, customer data, or internal deployment details | Sensitive information risk | Excluded | P1-10 evidence review keeps logs status-only and redacts before sharing. |
| EX-1905 | Public header changes that expose cryptographic context internals or freeze unreviewed struct layouts | ABI risk | Deferred or blocked | Public context internals remain hidden; the build identity struct remains a tracked TODO before public ABI freeze. |

## P1-11 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2001 | Third-party SDK integration samples, package scripts, CMake consumer templates, or vendor packaging workflows with unclear ownership | Code provenance risk | Excluded | P1-11 PoC package scripts and example are fresh C99/CMake files written in this repository. |
| EX-2002 | PoC packages containing secrets, entropy captures, generated secret bytes, private keys, tokens, customer data, or internal deployment details | Sensitive information risk | Excluded | P1-11 package smoke and manifest are status/package-structure evidence only and must be reviewed before sharing. |
| EX-2003 | Treating a customer PoC package smoke run as official validation evidence or broad environment suitability proof | Claims risk | Excluded | The package smoke only checks local installed-package integration and public-API smoke behavior. |
| EX-2004 | Public random-service API commitment through the PoC package before provider/reseed/fork-safety/API review | API/support risk | Deferred | The PoC example uses explicit-entropy HMAC_DRBG flow only and does not expose new public random APIs. |
| EX-2005 | Legal/evaluation license language without review | Legal/process risk | Deferred | Package notice text remains TODO pending legal review. |

## P1-12 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2101 | Block cipher, mode, KDF, AEAD, public-key, SecureKit, or PKI implementation code from third-party or unclear sources | Code provenance risk | Excluded | P1-12 adds only planning documents and does not import implementation code. |
| EX-2102 | Starting block cipher implementation before customer/platform scope, public vectors, self-test requirements, CSP mapping, and side-channel review are recorded | Scope/API risk | Deferred | P1-12 creates the decision gate instead of adding algorithm code. |
| EX-2103 | Differential tests based on reference tools or libraries without license/usage review | Provenance/process risk | Deferred | P1-12 records the strategy and requires source/tool review before adding tests. |
| EX-2104 | Customer PoC intake that requests secrets, entropy captures, generated secret bytes, private keys, tokens, customer data, or internal deployment secrets | Sensitive information risk | Excluded | Intake questionnaire is limited to platform, toolchain, deployment model, and algorithm-scope requirements. |
| EX-2105 | Treating side-channel checklist completion as formal leakage-resistance proof | Claims risk | Excluded | Checklist is an engineering review aid and must be paired with evidence and external process as applicable. |
| EX-2106 | Moving AnoSecureKit or AnoPKI workflows into the AnoCrypto-C core | Product-boundary risk | Excluded | P1-12 service matrix keeps application helper and PKI workflows out of the core boundary. |

## P1-13 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2201 | Private signing keys, signing tokens, release-service credentials, or timestamping credentials | Sensitive information risk | Excluded | P1-13 records signing status placeholders only and does not include secret signing material. |
| EX-2202 | Customer PoC packages containing customer secrets, customer data, private keys, entropy captures, generated secret bytes, tokens, or internal deployment details | Sensitive information risk | Excluded | P1-13 package review manifest and smoke checks keep package records structural and status-only. |
| EX-2203 | Third-party SBOM generator outputs or vendor manifests without reviewed tool/configuration provenance | Provenance/process risk | Deferred | P1-13 records SBOM placeholder status and waits for tool and schema review. |
| EX-2204 | Treating SHA-256 manifests or placeholder SBOM/signing status as official validation evidence | Claims risk | Excluded | Manifests are engineering review aids only. |
| EX-2205 | Detached signature files or verification instructions before signing tool, signer identity, and key-management policy are selected | Process risk | Deferred | P1-13 documents future approach without performing signing. |

## P1-14 Additional Exclusions

| ID | Material / Pattern | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2301 | Third-party thread-safety support matrices, lock wrappers, concurrency test harnesses, or CI snippets with unclear ownership | Code/document provenance risk | Excluded | P1-14 docs, scripts, and workflow updates are fresh project-owned material. |
| EX-2302 | Claiming that caller-owned hash, HMAC, or DRBG contexts are concurrently mutable | Support/claims risk | Excluded | P1-14 keeps context serialization as caller responsibility. |
| EX-2303 | Treating POSIX concurrency smoke results as a broad platform support guarantee | Claims risk | Excluded | Current results are engineering evidence tied to specific build configuration and platform branch. |
| EX-2304 | Thread test logs containing key material, entropy input, nonce input, DRBG state, MAC output, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details | Sensitive information risk | Excluded | P1-14 log rules require status-only output. |
| EX-2305 | Enabling POSIX concurrent CSP/HMAC smoke tests when `ANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF` | Test/support risk | Excluded | P1-14 guards that test behind both the build option and pthread availability. |

## P1-15 Additional Exclusions

| ID | Excluded material or behavior | Reason | Action | Notes |
|---|---|---|---|---|
| EX-2401 | Third-party SHA-384/SHA-512 implementation code or snippets with unclear ownership | Code provenance risk | Excluded | P1-15 does not add a SHA-512-family implementation. |
| EX-2402 | Third-party HMAC-SHA-384/HMAC-SHA-512 implementation code or snippets with unclear ownership | Code provenance risk | Excluded | P1-15 keeps these processing APIs reserved. |
| EX-2403 | Unreviewed SHA-384/SHA-512 or HMAC-SHA-384/HMAC-SHA-512 vector files | Test provenance risk | Excluded | P1-15 records only a KAT plan and KAT manifest reserved entries. |
| EX-2404 | Behavior that silently treats SHA-384/SHA-512 as implemented without startup KAT and evidence updates | Evidence/scope risk | Excluded | `sha2_reserved_api_tests` and `ano_crypto_sha2_scope_policy_check` keep the reserved state explicit. |
| EX-2405 | Logs containing HMAC key bytes, entropy input, nonce material, DRBG state, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details | Sensitive information risk | Excluded | P1-15 reserved-scope tests and package notes use status-only output. |


## P1-16 Additional Exclusions

| ID | Material | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2501 | Third-party random-service or OS RNG wrapper code with unclear ownership | Code provenance risk | Excluded | P1-16 adds only user-owned reserved placeholders. |
| EX-2502 | Entropy samples, seed material, nonce seed material, DRBG generated bytes, or DRBG state captures | Sensitive information risk | Excluded | Evidence must remain status-only. |
| EX-2503 | Provider-specific health-test logic copied from non-public or unclear sources | Provenance and validation risk | Excluded | Health-test design remains TODO. |
| EX-2504 | Customer platform entropy logs or deployment-specific RNG diagnostics | Sensitive deployment information risk | Excluded | Customer-specific data must not enter repository evidence. |
