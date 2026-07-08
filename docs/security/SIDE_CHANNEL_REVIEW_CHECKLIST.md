# Side-Channel Review Checklist

Status: P1-13 draft.

## Purpose

This checklist is a pre-implementation and pre-release engineering review aid for key-dependent code paths. It is not a guarantee of leakage resistance and does not claim external review completion.

## When to run this checklist

Run this checklist before merging changes that touch:

- Key import, storage, lookup, use, zeroization, or destruction.
- MAC, DRBG, cipher, AEAD, KDF, or public-key operations.
- Comparison of authentication tags or MAC values.
- Table lookups or branching that may depend on secret data.
- Error paths that may expose different behavior based on secret state.
- Logs, traces, or diagnostics around cryptographic services.

## Review checklist

| Area | Question | Status |
|---|---|---|
| Secret-dependent branches | Does any branch condition depend on key material, DRBG state, private scalar, secret seed, or secret intermediate data? | TODO |
| Secret-dependent memory access | Does any table index, pointer offset, or lookup pattern depend on secret material? | TODO |
| Comparison behavior | Are MAC/tag comparisons handled without early-exit leakage where applicable? | TODO |
| Zeroization | Are secret buffers cleared on success, failure, and early return paths? | TODO |
| Compiler behavior | Is zeroization implemented in a way that should not be optimized away? | TODO |
| Error behavior | Do status codes avoid disclosing secret-dependent internal state? | TODO |
| Logging | Are secret values, entropy bytes, DRBG internal state, private keys, tokens, and customer data absent from logs? | TODO |
| Test fixtures | Are test keys and vectors public deterministic fixtures only? | TODO |
| Platform differences | Are platform-specific entropy, threading, and visibility branches reviewed? | TODO |
| Public claims | Do docs avoid stronger leakage-resistance wording than the evidence supports? | TODO |

## Current P1-12 observations

- SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, and HMAC key handle paths exist as an engineering P1 slice.
- HMAC key material is in-memory only and is not exported by public API.
- Entropy provider tests are status-only and must not log entropy bytes.
- Further side-channel hardening work remains TODO before broader claims are made.

## Evidence record template

For each reviewed change, record:

```text
Review ID:
Commit/range:
Files reviewed:
Service area:
Secret-bearing inputs:
Branches reviewed:
Memory-access patterns reviewed:
Zeroization paths reviewed:
Logging reviewed:
Known limitations:
Follow-up TODOs:
Reviewer:
Date:
```
