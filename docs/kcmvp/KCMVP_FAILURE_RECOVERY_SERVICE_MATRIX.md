# KCMVP Failure, Recovery, and Service-availability Matrix

Status: P2-03 internal normative matrix. Integrity and some state-transition rows remain implementation gaps.

Document ID: `AC-KCMVP-FRS-P2-03-R1`

## Failure classification

| ID | Failure/event | Expected status | Module transition | Retry in same process | Operator/developer action | Owner/status |
|---|---|---|---|---|---|---|
| `FR-001` | Integrity metadata absent, malformed, truncated, wrong candidate/version, unreadable file, or HMAC mismatch | `ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED` | any startup/on-demand test state -> `ERROR_STATE` | No | Stop process; verify package/provenance; reinstall authorized artifact | P2-03 candidate, `NOT_IMPLEMENTED`, `EXTERNAL_CONFIRMATION_REQUIRED` |
| `FR-002` | Mandatory algorithm KAT mismatch | `ANO_CRYPTO_ERR_SELF_TEST_FAILED` | `SELF_TEST_RUNNING -> ERROR_STATE` | No | Stop process; retain status/artifact identity; investigate build/hardware | Implemented startup behavior |
| `FR-003` | Build identity or KAT manifest structure invalid | integrity/internal status | `SELF_TEST_RUNNING -> ERROR_STATE` | No | Treat as artifact/build corruption | Implemented supporting check |
| `FR-004` | Cryptographic service before init or after shutdown | `ANO_CRYPTO_ERR_NOT_INITIALIZED` | none | Yes, after successful init | Initialize module | Implemented |
| `FR-005` | Cryptographic service after fatal latch | `ANO_CRYPTO_ERR_FATAL_STATE` | remains `ERROR_STATE` | No | Terminate and investigate | Implemented service gate |
| `FR-006` | Init while already operational | `ANO_CRYPTO_OK` | remains `OPERATIONAL` | N/A | No action; does not rerun tests | Implemented |
| `FR-007` | Orderly shutdown | `ANO_CRYPTO_OK` | `OPERATIONAL -> SHUTDOWN` | Yes | Reinit reruns startup tests | Implemented |
| `FR-008` | Reinit after orderly shutdown | startup result | `SHUTDOWN -> SELF_TEST_RUNNING -> OPERATIONAL/ERROR_STATE` | Yes only on complete success | Same as startup | Implemented; P2-03 regression coverage |
| `FR-009` | On-demand self-test requested while operational | full sequence result | `OPERATIONAL -> SELF_TEST_RUNNING -> OPERATIONAL/ERROR_STATE` | Defined by result | Block services during test | `PLANNED_IMPLEMENTATION` |
| `FR-010` | Re-entrant/concurrent self-test | proposed `ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED` | no fatal transition unless corruption detected | Yes after current test completes | Retry later if policy permits | `PLANNED_IMPLEMENTATION` |
| `FR-011` | Unknown self-test flag bits | `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER` | none | Yes with supported flags | Correct caller flags | Partial; current validation is incomplete |
| `FR-012` | Invalid pointer, length, enum, purpose, overlap, or context size | specific parameter/buffer/unsupported status | none | Yes with corrected input | Correct caller input | Implemented per service |
| `FR-013` | Stale, destroyed, wrong-purpose, or exhausted CSP handle | service-specific lifecycle/resource status | none unless corruption found | Usually yes | Use valid handle/lifecycle | Implemented; formal P2-05 evidence pending |
| `FR-014` | DRBG reseed required | `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` | none unless P2-04 says otherwise | Yes after permitted reseed | Reseed or recreate context | P2-04 final policy |
| `FR-015` | OS entropy provider call failure | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Operation-local under current code | Clear failed-init destination or retain existing state after failed manual reseed | Retry/recreate after provider correction; fatality remains externally pending | `FROZEN_INTERNAL_POLICY / EXTERNAL_CONFIRMATION_REQUIRED` |
| `FR-016` | Direct-source health-test failure | not applicable | No selected direct source | No claim | Separate scope/assessment required before introduction | `OUT_OF_SCOPE_INITIAL_CANDIDATE` |
| `FR-017` | Authentication failure on caller data | `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED` | none by default | Yes with valid data/key | Reject data; do not expose comparison detail | Current API mostly reserves status; future service-specific use |
| `FR-018` | Caller-data integrity failure | `ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED` | none unless this is module-integrity context | Context-specific | Reject data | Status context must be documented |
| `FR-019` | Internal global-state lock cannot preserve invariant | `ANO_CRYPTO_ERR_INTERNAL_ERROR` or lock status | target `ERROR_STATE` | No | Stop process | Current behavior needs dedicated review/test |
| `FR-020` | Memory/resource allocation failure in ordinary caller-owned setup | `ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED` | none unless global invariant lost | Maybe | Free resources/retry | Per service/implementation |

## Status-to-fatality rule

A status value is not globally fatal by itself. The same status can have different state consequences based on context. In particular:

- `ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED` during `ST-INT-001` is fatal module-integrity failure.
- A future caller-data integrity failure may be recoverable and must not silently latch module fatal state.
- `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` remains P2-04-owned until provider and recovery policy are frozen.

## Query and diagnostic boundary after fatal failure

Allowed status-only diagnostics:

- module state and state name;
- last fatal status;
- last self-test status and request flags;
- version, build identity, and build fingerprint;
- external artifact/provenance hash collected outside the runtime.

Not allowed:

- expected/computed integrity tags or integrity key identifiers that reveal secret handling;
- KAT expected/computed bytes;
- keys, expanded schedules, entropy, DRBG state, random output, IV, plaintext, ciphertext, digest, HMAC, or CMAC values;
- automatic fatal-latch reset or continued cryptographic processing.
