# Downstream Requirement Intake

Status: `ACTIVE_P3_00_INTAKE`

## Purpose

AnoCrypto-C resumes product-driven development by accepting evidence-backed
requirements from AnoSecureKit Enterprise and AnoPKI Enterprise. This work does
not assume that Community product behavior belongs in AnoCrypto-C.

The decision flow is:

```text
downstream product operation
-> backend contract and exact test/fixture evidence
-> required cryptographic capability
-> repository ownership decision
-> existing AnoCrypto-C capability comparison
-> gap decision
-> API/ABI and test gate
-> numbered implementation work item
```

No algorithm or public API is approved merely because another product uses
OpenSSL for an operation.

## Intake source sets

| Source set | Repository | Required authority | Current state |
|---|---|---|---|
| `ASK-COMM` | AnoSecureKit Community | exact current commit/tag, code, provider contract, formats and tests | `NOT_IMPORTED` |
| `ASK-ENT` | AnoSecureKit Enterprise | exact current commit, code, parity/upstream requirement documents and adapter tests | `NOT_IMPORTED` |
| `APKI-COMM` | AnoPKI Community | exact current commit/tag, code, operation contracts, OpenSSL adapter and golden tests | `NOT_IMPORTED` |
| `APKI-ENT` | AnoPKI Enterprise | exact current commit, code, parity/upstream requirements, AnoCrypto-C adapter and no-fallback tests | `NOT_IMPORTED` |

File-Library snippets or historical document packs may help locate files, but
they do not satisfy intake authority. Each source set must provide a current
code snapshot and exact revision identity.

## Acceptance requirements

A downstream requirement is accepted only when it records:

- product and repository;
- exact source revision;
- product operation or format behavior;
- required/optional classification;
- backend-neutral contract;
- current OpenSSL-path implementation location;
- positive, negative and compatibility evidence;
- data/key ownership and failure semantics;
- proposed owning repository;
- whether an existing AnoCrypto-C service already satisfies it.

## Explicit non-decisions

P3-00 does not decide:

- the next algorithm;
- a new public header or symbol;
- a new ABI line;
- Enterprise operation parity;
- Community format changes;
- KCMVP submission scope;
- external consultation or release authorization.
