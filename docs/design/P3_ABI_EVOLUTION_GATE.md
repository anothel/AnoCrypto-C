# P3 API and ABI Evolution Gate

Status: `NO_CHANGE_APPROVED`

## Retained baseline

- public headers: 14;
- exported symbols: 69;
- ABI line: `ANOCRYPTO_C_0`.

P3-00 changes no public declaration or exported symbol.

## Decision options for a future accepted gap

1. private implementation using the existing API;
2. additive symbol in the current ABI line after compatibility review;
3. additive new header/symbol set under a separately versioned symbol line;
4. downstream adapter change with no AnoCrypto-C API change;
5. reject or defer the requirement.

A new ABI line such as `ANOCRYPTO_C_1` is not pre-approved. It may be selected
only after exact downstream requirements and compatibility impact are known.

## Required evidence before public expansion

- API ownership and misuse-boundary review;
- old/current header consumers;
- symbol-version and source-surface comparison;
- context size/layout and lifecycle rules;
- status, buffer, alias and failure-zeroization semantics;
- install/export and downstream adapter integration;
- release evidence and baseline update approval.
