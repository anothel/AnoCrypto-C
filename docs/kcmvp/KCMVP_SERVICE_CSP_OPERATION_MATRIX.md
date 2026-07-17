# KCMVP Service / CSP / Operation Matrix — P2-05

Status: `FROZEN_INTERNAL_POLICY`; formal classification is pending external consultation.

| Service / API path | CSP input or state | Module-owned copy | Caller-owned state | Completion / failure cleanup | Shutdown / fatal behavior | Scope disposition |
|---|---|---|---|---|---|---|
| HMAC one-shot raw-key API | Caller key bytes | None retained | Stack HMAC context and temporary pads/digest | Internal temporaries cleared before return | Caller input remains caller-owned | Candidate service; raw-key integration responsibility applies |
| HMAC handle API | `CSP-001` | CSP table plus bounded stack snapshot | Internal one-shot context | Snapshot/context cleared; table survives until destroy/shutdown/fatal | Table slot becomes `ZEROIZED` | Candidate service |
| HMAC streaming | Caller key bytes | None retained | `CSP-101` | Final/failure/uninit clears full context | Module cannot locate abandoned context | Candidate service; explicit uninit required |
| AES-CBC handle init/update | `CSP-002` | CSP table plus bounded stack snapshot | `CSP-102` expanded schedule and chain | Snapshot cleared after init; failures/uninit clear context | Table wiped; caller context still requires uninit | Candidate service |
| AES-CMAC handle init/update | `CSP-003` | CSP table plus bounded stack snapshot | `CSP-103` schedule/subkeys/chain/pending | Snapshot and context cleared on final/failure/uninit | Table wiped; caller context still requires uninit | Candidate service |
| Explicit HMAC_DRBG | Caller entropy/nonce plus `CSP-104` | None retained globally | DRBG Key/V/state in caller context | Failure/uninstantiate clears context; transient update state cleared | Caller must uninstantiate; process/VM policy from P2-04 | Candidate low-level service with integration caveat |
| Source-backed random service | External conditioned provider plus `CSP-104` | None retained globally | DRBG state in caller context | Provider/seed temporaries and uninit path clear | Caller must random-uninit | Candidate service; provider acceptance pending |
| Module startup KAT | Public synthetic KAT material; secret-derived local state | No operational key retained | Stack/local contexts | Cleared before return | Fatal state triggers table wipe | Engineering startup service; integrity still pending |
| Future integrity check | Future `CSP-004` | Not implemented | Not implemented | Required by P2-03 delta | Required before fatal exposure | `SELECTED_CANDIDATE_NOT_IMPLEMENTED` |
| `ano_crypto_key_destroy` | CSP handle | Clears table key and metadata | None | State `DESTROYED` | Allowed after shutdown/fatal `ZEROIZED` tombstone | Implemented |
| `ano_crypto_module_shutdown` | All live table CSPs | Clears all live slots | Does not track caller contexts | State `ZEROIZED` | Reinit allowed only after orderly shutdown | Implemented P2-05 |
| Fatal transition | All live table CSPs | Clears all live slots when lock acquired | Does not track caller contexts | Sticky `ERROR_STATE` | Fresh process/load required | Implemented P2-05 with lock-failure limitation |
| Export/persist/wrap | None | None | None | Not implemented | Not applicable | `OUT_OF_SCOPE_INITIAL_CANDIDATE` |
