# AnoCrypto-C Source Manifest

Status: P0 skeleton draft
Date: 2026-07-07

## Purpose

This manifest records the materials that are allowed to influence the AnoCrypto-C private C99 codebase.
It is intentionally conservative: implementation code, test vectors, file names, function names, constants, and build layout must not be copied from materials whose ownership or license is unclear.

## Allowed inputs for this P0 skeleton

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
