# AnoCrypto-C Exclusion Log

Status: P1-23 draft
Date: 2026-07-11

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
| EX-2004 | Public random-service API commitment through the PoC package before provider/reseed/fork-safety/API review | API/support risk | Resolved by P1-19 gate | The PoC now exercises the reviewed lifecycle but retains explicit operational and validation limitations. |
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
| EX-2404 | Behavior that silently treats SHA-384/SHA-512 as implemented without startup KAT and evidence updates | Evidence/scope risk | Excluded | `sha2_reserved_api_tests` and `ano_crypto_sha2_scope_policy_check` were the historical P1-15 guardrails; P1-22 supersedes them with implementation/KAT policy checks. |
| EX-2405 | Logs containing HMAC key bytes, entropy input, nonce material, DRBG state, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details | Sensitive information risk | Excluded | P1-15 reserved-scope tests and package notes use status-only output. |


## P1-16 Additional Exclusions

| ID | Material | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2501 | Third-party random-service or OS RNG wrapper code with unclear ownership | Code provenance risk | Excluded | P1-16 adds only user-owned reserved placeholders. |
| EX-2502 | Entropy samples, seed material, nonce seed material, DRBG generated bytes, or DRBG state captures | Sensitive information risk | Excluded | Evidence must remain status-only. |
| EX-2503 | Provider-specific health-test logic copied from non-public or unclear sources | Provenance and validation risk | Excluded | Health-test design remains TODO. |
| EX-2504 | Customer platform entropy logs or deployment-specific RNG diagnostics | Sensitive deployment information risk | Excluded | Customer-specific data must not enter repository evidence. |

## P1-17 Additional Exclusions

| ID | Material | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2601 | Actual operating-system entropy captures or entropy fixtures | Sensitive material and misleading test evidence | Excluded | Provider tests record status only and use synthetic hook data for deterministic branches. |
| EX-2602 | Third-party or non-public startup/continuous health-test code, thresholds, constants, or terminology copied without review | Provenance and validation risk | Excluded | P1-17 adds only a fresh bounded duplicate-block engineering skeleton. |
| EX-2603 | Entropy failure-injection hooks in installed headers, ordinary Release builds, or customer PoC binaries | Public ABI and security-boundary risk | Excluded | Hook compilation requires tests and is disabled in the clean PoC build. |
| EX-2604 | Logs/evidence containing entropy material, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data | Sensitive information risk | Excluded | P1-17 policy, tests, package notes, and evidence review require status-only output. |
| EX-2605 | Treating duplicate-block candidates or CI failure lanes as a finalized health-test design or official validation result | Claims risk | Excluded | Final source-specific health testing and thresholds remain TODO / confirmation required. |
| EX-2606 | Recovery/reset API from sticky module `ERROR_STATE` without lifecycle and validation-boundary review | State-machine/API risk | Deferred | P1-17 keeps fatal health failure sticky and requires restart or future reviewed recovery design. |

## P1-18 Additional Exclusions

| ID | Material / behavior | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2701 | Applying the P1-17 duplicate-block candidate to OS-provider output and describing it as a raw-noise health test | Misplaced test and misleading evidence risk | Excluded | OS APIs are classified as external conditioned providers. |
| EX-2702 | Third-party process/fork guard code, at-fork handlers, or random-state recovery code with unclear ownership | Code provenance and lifecycle risk | Excluded | P1-18 uses a fresh small internal process-marker check only. |
| EX-2703 | Public ABI exposure of source classes, health owners, restart policies, raw-noise candidate types, or process guards | ABI/support risk | Excluded | These remain internal implementation metadata. |
| EX-2704 | Provider output, seed material, nonce material, DRBG state, generated random output, private keys, tokens, or customer data in tests/evidence/packages | Sensitive information risk | Excluded | P1-18 tests and records remain status-only. |
| EX-2705 | Claiming automatic fork/process protection for public explicit-entropy DRBG contexts | Support boundary risk | Excluded | The guard applies only to internally source-backed contexts. |
| EX-2706 | Enabling public random generation before context ownership, process-marker behavior, reseed/prediction-resistance policy, and validation-boundary review | API and operational risk | Resolved by P1-19 gate | P1-19 implements the service with explicit ownership/reseed limits; hosted review and final validation mapping remain TODO. |
| EX-2707 | Treating a provider operational probe as entropy-quality evidence | Evidence risk | Excluded | The probe checks call-path/status only and clears temporary storage. |

## P1-19 Additional Exclusions

| ID | Material / behavior | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2801 | Third-party random-service, DRBG wrapper, fork-recovery, or provider code with unclear ownership | Code provenance risk | Excluded | P1-19 uses fresh project-owned C99 code over existing user-owned seams. |
| EX-2802 | Provider output, entropy/nonce seed material, DRBG state, generated random bytes, private keys, tokens, customer data, or deployment details in logs/evidence/packages | Sensitive information risk | Excluded | Tests and smoke programs report status and byte counts only. |
| EX-2803 | Public provider callbacks, source classes, health-test internals, process markers, or test-hook types | ABI/support risk | Excluded | These remain internal and are scanned out of installed headers and clean PoC packages. |
| EX-2804 | Automatic post-fork generation without a successful child reseed | State-reuse risk | Excluded | A detected process change returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and clears output. |
| EX-2805 | Accepting caller-entropy DRBG contexts through the public random-service API | Cross-API ownership risk | Excluded | Source-backed context marking separates the two lifecycle families. |
| EX-2806 | Prediction-resistance, VM snapshot/restore detection, suspend/resume detection, or unsupported-platform availability claims | Support/claims risk | Deferred | Current APIs reject prediction-resistance and document the remaining operational gaps. |
| EX-2807 | Treating the P1-19 implementation or CI results as official validation evidence or broad platform assurance | Claims risk | Excluded | Hosted-run review and final validation-target mapping remain TODO. |
| EX-2808 | Retaining deterministic hook output or operating-system random output as a test fixture | Sensitive/misleading evidence risk | Excluded | Failure-injection tests compare transient behavior only and retain no output artifact. |

## P1-20 Additional Exclusions

| ID | Material / behavior | Risk | Decision | Notes |
|---|---|---|---|---|
| EX-2901 | Third-party lifecycle stress, reseed-limit hook, process-marker, VM checkpoint, or symbol-baseline code with unclear ownership | Code provenance risk | Excluded | P1-20 uses fresh project-owned C99 and CMake tooling. |
| EX-2902 | Random lifecycle or hosted-review artifacts containing generated bytes, entropy, seed material, DRBG state, private keys, tokens, or customer data | Sensitive information risk | Excluded | Artifacts contain statuses, provider identifiers, capability flags, counts, and policy only. |
| EX-2903 | Enabling `ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS` in ordinary, Release, evidence consumer, or PoC SDK builds | ABI and test-boundary risk | Excluded | The option requires tests and clean Release/PoC configurations explicitly set it OFF. |
| EX-2904 | Silently operating the public random service when process identity cannot be obtained | State-duplication risk | Excluded | P1-20 fails closed with the service unavailable. |
| EX-2905 | Continuing from DRBG state known to have been duplicated by VM snapshot/restore or clone | State-reuse risk | Excluded | Project policy requires discard and independent reinitialization of each instance. |
| EX-2906 | Claiming automatic VM/checkpoint/suspend detection | Support and claims risk | Excluded | No host-specific detection interface is implemented. |
| EX-2907 | Automatically rewriting the exported-symbol baseline after an ABI difference | ABI governance risk | Excluded | Missing or unexpected symbols fail the check and require explicit review. |
| EX-2908 | Treating 53-symbol name equality as complete ABI compatibility or official validation evidence | Claims risk | Excluded | Calling convention, type size, layout, semantics, and platform metadata remain separate review items. |

## P1-21 Additional Exclusions

| ID | Excluded input or behavior | Risk | Decision | Note |
|---|---|---|---|---|
| EX-3001 | Third-party or employer-owned concurrency harnesses, sanitizer suppressions, and Windows synchronization wrappers with unclear rights | Provenance risk | Excluded | P1-21 uses fresh project-owned C99 test code and public platform API concepts only. |
| EX-3002 | Treating workflow YAML or an uploaded status artifact as proof that every hosted platform passed | Evidence/claims risk | Excluded | Actual workflow runs must be reviewed and referenced separately. |
| EX-3003 | Printing HMAC keys, MAC output, entropy, seed material, DRBG state, or generated random bytes to diagnose races or sanitizer failures | Sensitive material disclosure | Excluded | Tests and artifacts record status/backend/mode only. |
| EX-3004 | Enabling sanitizer instrumentation in the clean customer PoC SDK by default | Packaging and performance ambiguity | Excluded | Clean PoC SDK explicitly uses `ANO_CRYPTO_SANITIZER_MODE=none`. |
| EX-3005 | Extending or reordering `ano_crypto_build_identity_t` while retaining ABI version 1 | ABI ambiguity | Excluded | P1-21 records and tests the existing ABI-0 field order. |
| EX-3006 | Claiming ThreadSanitizer or Clang sanitizer coverage without a reviewed compatible runtime result | Evidence overstatement | Excluded | GCC ASan/UBSan is required; other sanitizer lanes remain TODO. |


## P1-22 Additional Exclusions

| ID | Excluded input or behavior | Risk | Decision | Note |
|---|---|---|---|---|
| EX-3101 | Third-party, employer-owned, vendor, or customer SHA-384/SHA-512/HMAC implementation code with unclear rights | Provenance risk | Excluded | P1-22 uses fresh C99 code based only on recorded public standards. |
| EX-3102 | Unreviewed SHA-384/SHA-512 or HMAC vector files, generated fixtures, or copied test harnesses | Test provenance risk | Excluded | Only FIPS 180-4 and RFC 4231 values recorded in the manifest are used. |
| EX-3103 | Partial implementation without startup KAT, extended KAT, negative tests, install/PoC coverage, and evidence updates | Scope/evidence mismatch | Excluded | `ano_crypto_sha512_family_policy_check` requires the complete P1-22 wiring. |
| EX-3104 | Digest values, MAC values, HMAC key bytes, entropy, seed material, DRBG state, generated random values, private keys, tokens, or customer data in logs/evidence/packages | Sensitive information risk | Excluded | Tests and smoke flows emit names, lengths, statuses, and source IDs only. |
| EX-3105 | Claiming official validation, completed KCMVP status, or universal constant-time behavior from P1-22 KAT/CI results | Claims risk | Excluded | P1-22 is engineering implementation and correctness evidence only. |
| EX-3106 | Selecting an external differential provider without license/version/provenance and failure-triage review | Supply-chain and evidence risk | Resolved by P1-23 gate | P1-23 records OpenSSL 3.x provenance, test-only linkage, logging limits, and failure triage; unreviewed additional providers remain excluded. |

## P1-23 Additional Exclusions

| ID | Excluded input or behavior | Risk | Decision | Note |
|---|---|---|---|---|
| EX-3201 | Copying OpenSSL implementation, constants, sample code, or test harness code into AnoCrypto-C | Provenance and maintenance risk | Excluded | OpenSSL is invoked only through public headers and a separately linked test target. |
| EX-3202 | Linking OpenSSL into `anocrypto_c`, installing OpenSSL headers through AnoCrypto-C, or adding it to the customer PoC runtime dependency | Product boundary and licensing risk | Excluded | The provider is test-only and clean Release/PoC builds have tests disabled. |
| EX-3203 | Logging differential digest/MAC bytes, generated HMAC keys/messages, entropy, seed, DRBG state, or random output | Sensitive/evidence expansion risk | Excluded | Status, provider version, comparison count, algorithm/path, and lengths are sufficient for triage. |
| EX-3204 | Automatically rewriting implementation or KAT values after an OpenSSL mismatch | Common-defect and evidence-integrity risk | Excluded | Reproduction, public KAT, path isolation, and source review are required first. |
| EX-3205 | Treating provider agreement as official validation or proof of implementation independence | Claims risk | Excluded | Differential validation is one engineering correctness layer only. |
| EX-3206 | Claiming 32-bit, big-endian, Windows, or macOS support from workflow definitions without reviewing actual runs | Evidence/portability overstatement | Excluded | Hosted and non-native artifacts require explicit review. |
| EX-3207 | Using unreviewed 32-bit OpenSSL packages merely to make the multilib lane pass | Supply-chain ambiguity | Excluded | The initial 32-bit lane disables external differential testing unless a provider package is separately reviewed. |


## P1-24 block-cipher entry gate

- Used only public standards identifiers and project-owned design requirements.
- Selected FIPS 197 and NIST SP 800-38A as future public vector sources; no vectors or implementation code were copied in P1-24.
- No private certification material, customer material, key, plaintext, ciphertext, or proprietary implementation was used.


## P1-25 AES/CBC exclusions

- No third-party AES implementation, S-box table, key-schedule code, file layout, or test harness was copied into the product source.
- No private KCMVP submission material or customer vectors were used.
- OpenSSL remains a test-only differential provider and is excluded from the library, installed SDK, and customer PoC runtime.
- Keys, expanded schedules, IVs, plaintext, ciphertext, and intermediate AES state are excluded from logs and evidence.

## P1-26 AES/CBC Hardening Exclusions

| ID | Excluded input or behavior | Risk | Decision | Note |
|---|---|---|---|---|
| EX-3401 | Third-party AES hardening patches, context-integrity layouts, compiler analysis scripts, lifecycle/concurrency harnesses, or CI fragments with unclear rights | Provenance risk | Excluded | P1-26 uses fresh project-owned C99 and CMake material. |
| EX-3402 | Treating internal context magic or round-key-size checks as an authentication mechanism or a substitute for memory safety | Security overstatement | Excluded | The checks are bounded defense-in-depth and fail-closed lifecycle guards only. |
| EX-3403 | Claiming universal constant-time, side-channel resistance, or external review from a table-free source design or captured disassembly | Claims and technical risk | Excluded | Compiler, optimization, target architecture, microarchitecture, timing/cache, and fault review require manual evidence. |
| EX-3404 | Logging or packaging AES keys, expanded schedules, IVs, plaintext, ciphertext, intermediate state, differential output bytes, entropy, seed, DRBG state, random output, private keys, signing keys, tokens, or customer data | Sensitive information risk | Excluded | Tests, CI, evidence summaries, install smoke, and PoC remain status/count only. |
| EX-3405 | Shipping OpenSSL, differential test source, internal AES headers/source, lifecycle tests, or compiler disassembly in the customer PoC package | Product boundary and disclosure risk | Excluded | OpenSSL and internal review artifacts remain test/evidence only. |
| EX-3406 | Automatically declaring hosted Windows/macOS/32-bit/big-endian support because workflow YAML exists | Evidence overstatement | Excluded | Actual runs, artifact hashes, reviewer, limitations, and disposition are required. |
| EX-3407 | Widening the P1-25 public cipher API or silently changing the 61-symbol baseline during hardening | ABI governance risk | Excluded | P1-26 retains eight public cipher functions, existing enums/layouts, and 61 public symbols. |
| EX-3408 | Adding CTR, GCM, padding, CMAC/GMAC, hardware acceleration, or domestic algorithms without a separate decision gate | Scope-creep risk | Deferred | P1-27 must select a narrow next scope or pause algorithm expansion. |

## P1-27 CMAC Entry Gate Exclusions

| ID | Excluded material or action | Risk | Action | Note |
|---|---|---|---|---|
| EX-3501 | Third-party CMAC implementation, subkey routine, API surface, tests, build fragments, or fixtures with unclear rights | Provenance risk | Excluded | P1-27 uses only public requirements and fresh project-owned planning material. |
| EX-3502 | Private submission vectors, internal certification documents, or customer CMAC data | Confidentiality and rights risk | Excluded | Only public-source vector planning is allowed. |
| EX-3503 | Reusing generic AES encryption handles for CMAC without explicit purpose authorization | Key-separation and service-scope risk | Excluded | A dedicated CMAC-purpose handle is required before implementation. |
| EX-3504 | Adding `ac_cmac.h`, CMAC source, enum value, function, or symbol during the decision gate | API/ABI governance risk | Excluded | The 61-symbol baseline remains unchanged in P1-27. |
| EX-3505 | Adding AES-CTR without counter construction, uniqueness, exhaustion, clone, and recovery policy | Misuse and state-management risk | Excluded | CTR remains a separate future gate. |
| EX-3506 | Logging CMAC key, message, subkey, intermediate, or tag values | Sensitive-material exposure risk | Excluded | Gate, CI, evidence, and PoC records are status/policy only. |

## P1-28 AES-CMAC Implementation Exclusions

| ID | Excluded material or action | Risk | Action | Note |
|---|---|---|---|---|
| EX-3601 | Third-party CMAC implementation, subkey-generation source, context layout, tests, or build fragments with unclear rights | Provenance risk | Excluded | P1-28 uses fresh project-owned C99 and recorded public standards/examples. |
| EX-3602 | Private submission vectors, customer CMAC material, or unexplained expected tags | Confidentiality and evidence-integrity risk | Excluded | Only the recorded public NIST examples are used for KATs. |
| EX-3603 | Reusing AES encryption-purpose handles for CMAC or CMAC-purpose handles for CBC | Service authorization and key-purpose ambiguity | Excluded | Cross-purpose use is rejected and covered by negative tests. |
| EX-3604 | Accepting truncated tags, partial output, or mismatch-position reporting | API and authentication ambiguity | Excluded | The initial API accepts exactly 16 bytes and returns `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED` on mismatch. |
| EX-3605 | Linking OpenSSL into the product library, installing provider headers, or shipping differential source/results in the PoC | Product-boundary and licensing risk | Excluded | OpenSSL remains test-only and clean SDK/PoC builds disable provider discovery. |
| EX-3606 | Logging or packaging CMAC key, message, tag, subkey, chaining state, round key, entropy, seed, DRBG state, random output, private key, token, or customer data | Sensitive-material disclosure | Excluded | Tests and evidence emit status/count/provenance metadata only. |
| EX-3607 | Claiming universal constant-time behavior, completed side-channel review, or official validation from KAT/differential results | Claims and technical risk | Excluded | Target-specific compiler, timing/cache, and formal review remain TODO. |
| EX-3608 | Adding CTR, GCM/GMAC, protocol formatting, persistence, export, or hardware dispatch in the CMAC implementation step | Scope-creep risk | Deferred | Each requires a separate reviewed decision gate. |
| EX-3609 | Running nested install/PoC builds and a second filtered CTest inside one evidence-assembly process under an outer build jobserver | Build reliability and deadlock risk | Excluded | CTest, installed-package smoke, and PoC smoke are separate prepared artifacts consumed by the evidence assembler. |


## P1-29 AES-CMAC Hardening Exclusions

| ID | Excluded material or behavior | Risk | Disposition | Note |
|---|---|---|---|---|
| EX-3701 | Third-party CMAC lifecycle, context-integrity, constant-work comparison, concurrency, or compiler-analysis source with unclear rights | Provenance risk | Excluded | P1-29 changes and tests are fresh project-owned C99 and CMake material. |
| EX-3702 | Treating source-level full-tag comparison or disassembly capture as a completed timing/side-channel conclusion | Claims risk | Excluded | Target/compiler-specific human review remains required. |
| EX-3703 | Allowing context, message, output tag, or output-length storage to overlap where mutable state can be corrupted | Memory/state integrity risk | Excluded | Public preflight rejects the reviewed overlap classes. |
| EX-3704 | Trusting oversized `uninit` length to clear beyond the opaque CMAC context | Memory-corruption risk | Excluded | Cleanup is bounded to the queried context extent. |
| EX-3705 | Retaining a prior key schedule after failed CMAC reinitialization | Secret-state lifecycle risk | Excluded | Valid-size reinitialization failure clears the caller context. |
| EX-3706 | Logging or packaging CMAC keys, messages, tags, subkeys, chains, AES schedules, differential outputs, entropy, seed, DRBG state, random bytes, private keys, tokens, or customer data | Sensitive-material disclosure | Excluded | P1-29 logs contain status/count/compiler/target metadata only. |
| EX-3707 | Adding public CMAC functions, tag truncation, CTR, GCM/GMAC, persistence, export, or protocol formatting during hardening | API/scope creep | Excluded or deferred | Public API remains eight functions and ABI-0 remains 69 symbols. |


## P1-30 Release Compatibility Exclusions

| ID | Excluded material or behavior | Risk | Disposition | Note |
|---|---|---|---|---|
| EX-3801 | Third-party ABI test suites, copied compatibility shims, or release tooling with unclear rights | Provenance risk | Excluded | P1-30 compatibility tests and CMake scripts are fresh project-owned material. |
| EX-3802 | Installing or packaging the retained P1-29 header baseline, compatibility probe source, or hosted review tooling | Public/package boundary risk | Excluded | Baseline material remains tests/evidence-only. |
| EX-3803 | Treating symbol/declaration equality as universal ABI or semantic compatibility | Claims risk | Excluded | Results are limited to reviewed toolchains and tested paths. |
| EX-3804 | Treating expected tag text as proof that a tag exists, is authorized, or identifies the packaged binary | Provenance risk | Excluded | Tagged Git verification and signing policy are separate steps. |
| EX-3805 | Accepting tampered, unlisted, binary, path-traversal, oversized, or sensitive-value hosted artifacts | Evidence integrity/disclosure risk | Excluded | Create/verify/tamper negative tests and final index checks are required. |
| EX-3806 | Logging keys, entropy, seed material, DRBG state, generated bytes, AES values, CMAC tags, private keys, signing credentials, tokens, customer data, or internal deployment details | Sensitive-material disclosure | Excluded | P1-30 compatibility and hosted logs are status-only. |
| EX-3807 | Adding a new public cryptographic service while freezing the compatibility baseline | Scope/ABI ambiguity | Excluded | P1-30 retains 14 public headers and 69 exports. |

## P1-31 Release Provenance and Signing Exclusions

| ID | Excluded material or behavior | Risk | Disposition | Note |
|---|---|---|---|---|
| EX-3901 | Third-party SBOM/provenance/signing generator source, copied workflow fragments, or release scripts with unclear rights | Provenance and licensing risk | Excluded | P1-31 tooling is fresh project-owned CMake material based on public format concepts. |
| EX-3902 | Private signing keys, certificate private keys, keystores, signing tokens, cloud credentials, or detached signatures committed to make a test pass | Credential and release-integrity risk | Excluded | The source and bundle policy rejects credential/signature-like files. |
| EX-3903 | Treating an expected tag string, commit string, SHA-256 manifest, uploaded artifact, or source SBOM as an authenticated attestation | Claims and supply-chain risk | Excluded | Actual tag/commit proof and cryptographic attestation require separate verified procedures. |
| EX-3904 | Marking signing `SIGNED` or external release `YES` without an approved verifier, signer policy, and release authorization | False-claim risk | Excluded | P1-31 accepts only `UNSIGNED`, authorization `TODO`, and external release `NO`. |
| EX-3905 | Describing the source inventory as a complete dependency, license, or vulnerability SBOM | Scope and compliance risk | Excluded | Dependency discovery, legal conclusions, and vulnerability analysis are explicitly not performed. |
| EX-3906 | Copying the internal source archive, source SBOM, signing request, authorization record, compatibility baseline, or hosted review tooling into the customer PoC | Product-boundary and disclosure risk | Excluded | The PoC receives status/package notes only. |
| EX-3907 | Treating synthetic hosted disposition fixtures as actual Windows/macOS/32-bit/big-endian/compiler review | Evidence overstatement | Excluded | Fixtures verify parser policy only; actual workflow artifacts require reviewer/date disposition. |
| EX-3908 | Logging or packaging keys, entropy, seed, DRBG state, generated random values, AES values, CMAC tags, private scalars, tokens, customer data, or internal deployment details | Sensitive-material disclosure | Excluded | Provenance and SBOM artifacts contain paths, hashes, statuses, counts, and release metadata only. |

## P1-32 Offline Release-Signing Handoff Exclusions

| ID | Excluded material or behavior | Risk | Disposition | Note |
|---|---|---|---|---|
| EX-4001 | Production private keys, seed phrases, key backups, keystores, hardware-token credentials, PINs, signing tokens, cloud credentials, or CI secrets | Credential and release-integrity risk | Excluded | Production signing custody remains offline and unprovisioned; the repository and CI contain no signing credential. |
| EX-4002 | Invented production signer fingerprints, TSA policy OIDs, trust-anchor fingerprints, service owners, approval identities, or revocation records | False identity/trust claim | Excluded | Unprovisioned fields remain explicit `TODO`/`UNPROVISIONED` values and keep handoff readiness blocked. |
| EX-4003 | Production detached signatures, signer public-key files, timestamp requests/responses, or TSA certificate/trust-anchor files committed or packaged as evidence | Artifact authenticity and credential-boundary risk | Excluded | P1-32 defines verification inputs but performs no production signing or timestamp operation. |
| EX-4004 | Recording the same non-placeholder identity as both Security Reviewer and Product Release Owner | Authorization separation risk | Rejected | The handoff creator and checker require distinct recorded identities. |
| EX-4005 | Treating GitHub artifact attestation, an uploaded artifact, a checksum, or expected tag text as the primary signing trust root or external authorization | Supply-chain and claims risk | Excluded | Hosted attestation is optional secondary evidence only; offline signature, timestamp, and human authorization remain separate gates. |
| EX-4006 | Enabling ephemeral signing fixtures in Release-like builds, when tests are disabled, or retaining fixture keys/certificates/signatures/timestamps after testing | Test-material leakage risk | Rejected | Fixtures require explicit opt-in Debug test configuration and remove generated material after the regression. |
| EX-4007 | Marking signing `SIGNED`, handoff `READY_FOR_OFFLINE_SIGNING`, or external release `YES` while production signer, TSA, role identities, hosted disposition, or release authorization remains incomplete | False release claim | Excluded | Current repository state is `UNSIGNED`, `BLOCKED`, and external release `NO`. |
| EX-4008 | Third-party signing scripts, proprietary key-custody procedures, copied CI workflow fragments, private PKI material, or vendor fixtures with unclear rights | Provenance and confidentiality risk | Excluded | P1-32 uses fresh project-owned CMake tooling and public standards/CLI documentation only. |
| EX-4009 | Copying internal provenance/handoff bundles, signer/TSA policy records, release authorization records, keys, tokens, signatures, or timestamp responses into the customer PoC package | Product-boundary and disclosure risk | Excluded | The PoC contains only package-local unsigned status and policy-boundary notes. |

## P1-33 Clean Tagged-Release Rehearsal and Hosted Review Exclusions

| ID | Excluded material or behavior | Risk | Disposition | Note |
|---|---|---|---|---|
| EX-4101 | Creating, moving, signing, or pushing a tag in the user repository during rehearsal | Repository/release integrity risk | Excluded | Synthetic tags exist only in an isolated temporary repository. |
| EX-4102 | Treating synthetic/local fixture output as an actual GitHub-hosted result | Evidence overstatement | Rejected | Synthetic origin cannot satisfy required hosted review; missing execution remains `NOT_RUN`. |
| EX-4103 | Accepting a dirty checkout, moved tag, moved `HEAD`, commit mismatch, or ref/tag mismatch | Provenance integrity risk | Rejected | Expected tag, supplied commit, tag-resolved commit, and clean checked-out `HEAD` must agree. |
| EX-4104 | Accepting missing/duplicate lanes, unlisted files, hash mismatch, traversal, symlink, oversized or unexpected binary-like files, or sensitive markers | Artifact integrity/disclosure risk | Rejected | Hosted review input is complete, manifest-bound, regular-file-only, bounded, and status-only. |
| EX-4105 | Recording hosted `PASS`/`APPROVED` without artifact/workflow identity, reviewer, UTC review date, disposition, and limitations | False review claim | Rejected | Downloaded but undispositioned artifacts remain `UNREVIEWED`. |
| EX-4106 | Assigning the same identity to conflicting Security Reviewer and Product Release Owner approvals | Authorization separation risk | Rejected | Distinct stable non-placeholder identities remain required before release authorization. |
| EX-4107 | Adding actual signatures, timestamp responses, signing credentials, entropy/seed/DRBG state, AES values, CMAC tags, tokens, or customer data to rehearsal/hosted artifacts | Sensitive-material and false-release risk | Excluded | P1-33 retains status/hash metadata only and performs no production signing or timestamping. |
