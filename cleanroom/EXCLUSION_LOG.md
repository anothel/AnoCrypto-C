# anocrypto-c Exclusion Log

Status: P1-8 draft
Date: 2026-07-07

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
