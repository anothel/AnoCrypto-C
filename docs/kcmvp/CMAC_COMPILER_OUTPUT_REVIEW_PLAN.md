# CMAC Compiler Output Review Plan

Status: P1-29 engineering review plan.

## Scope

The build may capture disassembly and metadata for the following private/public
CMAC paths when a suitable disassembler is available:

- private init;
- private update;
- private final;
- private tag comparison;
- public one-shot verify.

The capture contains compiler identity, version, target, build type, library
hash, symbol-marker presence, and whole-library mnemonic counts. It does not
execute CMAC with operational material and does not retain keys, messages,
tags, subkeys, chaining state, or AES schedules.

## Required human review

A qualified reviewer should record, per compiler and target:

1. artifact hash and source revision;
2. compiler flags and optimization level;
3. whether the relevant symbols were retained or inlined;
4. CMAC subkey-doubling control flow and memory access;
5. final-block complete/incomplete selection behavior;
6. AES primitive call boundaries and cleanup paths;
7. tag-comparison loop behavior and early-exit absence;
8. limitations, disposition, reviewer, and review date.

Branch and call counts are navigational metadata only. They are not an
automated safety or side-channel verdict.

## Artifact handling

- `CMAC_COMPILER_OUTPUT_REVIEW.txt` may enter internal engineering evidence.
- `CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt` remains an internal review artifact.
- Neither artifact belongs in the installed SDK or customer PoC package.
- Hosted workflow presence is not evidence that an artifact was reviewed.

## Exit condition

P1-29 closes only the capture and policy skeleton. Target-specific conclusions
remain TODO until actual artifacts, toolchains, reviewers, dates, limitations,
and dispositions are retained.
