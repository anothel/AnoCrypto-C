# AnoCrypto-C Exclusion Log

Status: P0 skeleton draft
Date: 2026-07-07

## Purpose

This log records materials and categories that must not influence the AnoCrypto-C implementation, tests, documentation wording, file names, function names, constants, or build structure.

## Exclusion rules

| ID | Excluded material/category | Risk type | Required action | Notes |
|---|---|---|---|---|
| EXC-0001 | Materials with unclear ownership, unclear license, or unclear right to analyze | IP / clean-room risk | Do not inspect for implementation details; do not copy names, layout, comments, constants, tests, vectors, or build structure. | Use only after explicit written permission and manifest update. |
| EXC-0002 | Private employer, customer, vendor, or third-party code/documents | Confidentiality / trade-secret risk | Exclude from design input and repository contents. | Record only path/risk/action if accidentally discovered. |
| EXC-0003 | Internal certification submissions, test reports, vulnerability reports, or non-public assessment artifacts | Confidentiality / regulatory process risk | Exclude from design input and repository contents. | Use public standards and public vectors instead. |
| EXC-0004 | Secret keys, seeds, entropy captures, private scalars, tokens, certificate private keys | Secret material risk | Do not store, log, print, or include in tests. Delete from repo history if accidentally committed. | Test fixtures must use public non-secret vectors only. |
| EXC-0005 | Legacy names outside the AnoCrypto-C naming plan | Brand/API contamination risk | Do not introduce into new source, headers, examples, package names, or CMake targets. | Historical mention only if a future migration document explicitly needs it. |
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
