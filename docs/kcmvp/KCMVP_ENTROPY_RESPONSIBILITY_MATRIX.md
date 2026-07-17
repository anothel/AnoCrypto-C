# KCMVP Entropy Responsibility Matrix — P2-04

Status: internal engineering allocation under `EDP-P2-04-R1`; all institution-dependent
acceptance remains `EXTERNAL_CONFIRMATION_REQUIRED`.

| ID | Component/event | Inside module boundary | Current owner | Current evidence | Gap/status |
|---|---|---:|---|---|---|
| ER-001 | Linux kernel noise sources | No | Linux operating environment | Exact source not visible through module API. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| ER-002 | Kernel entropy accumulation/conditioning | No | Linux operating environment | `getrandom(flags=0)` provider branch and exact OE metadata. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| ER-003 | Kernel/provider startup and continuous health behavior | No | External provider/operating environment | Provider classification and no module health attachment. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| ER-004 | `getrandom()` invocation and complete-read loop | Yes, adapter call path | AnoCrypto-C | `entropy_adapter.c`; provider/unit/failure tests. | `READY_FOR_INTERNAL_REVIEW` |
| ER-005 | Provider output clearing after acquisition failure | Yes | AnoCrypto-C | Adapter and failure-injection tests. | `READY_FOR_INTERNAL_REVIEW` |
| ER-006 | 32-byte instantiate entropy-input acquisition | Yes | AnoCrypto-C call path; entropy quality external | DRBG helper tests and policy review. | `PARTIAL` |
| ER-007 | Separate 16-byte instantiate nonce acquisition | Yes | AnoCrypto-C call path; construction acceptance external | DRBG helper tests and policy review. | `PARTIAL` |
| ER-008 | 32-byte reseed acquisition | Yes | AnoCrypto-C call path; entropy quality external | Random reseed/failure tests. | `PARTIAL` |
| ER-009 | HMAC_DRBG instantiate/reseed/generate state machine | Yes | AnoCrypto-C | KAT, boundary, negative, lifecycle, and reseed-limit tests. | `READY_FOR_INTERNAL_REVIEW` |
| ER-010 | Public source-backed random context | Yes | AnoCrypto-C and caller-owned storage | Random service tests, install consumer, PoC. | `READY_FOR_INTERNAL_REVIEW` |
| ER-011 | Explicit-entropy DRBG input origin and entropy estimate | No module verification | Caller/integrator | Minimum-length and deterministic KAT coverage only. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| ER-012 | Direct project-owned raw-noise source | No | None | No selected implementation. | `OUT_OF_SCOPE` |
| ER-013 | Raw-source RCT/APT or equivalent tests | No selected source | Future source owner | Generic duplicate-block skeleton only. | `NOT_IMPLEMENTED` |
| ER-014 | Fork/process duplication detection | Yes for source-backed contexts | AnoCrypto-C | PID/process-marker and fork tests. | `READY_FOR_INTERNAL_REVIEW` |
| ER-015 | VM snapshot/clone detection | No | Operator/orchestrator | Documented discard/reinitialize rule only. | `NOT_IMPLEMENTED` |
| ER-016 | Container checkpoint/restore detection | No | Operator/orchestrator | Documented discard/reinitialize rule only. | `NOT_IMPLEMENTED` |
| ER-017 | Suspend/resume freshness detection | No | Operator/product integration | Documented assumption only. | `NOT_IMPLEMENTED` |
| ER-018 | Selected OE-01 provider execution | External execution dependency | Project reviewer on exact OE | Candidate profile exists; exact retained run absent. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| ER-019 | Actual hosted provider evidence | External CI account | Project/GitHub | Run `29256531912` produced zero artifacts. | `BLOCKED_EXTERNAL` |

## Provider profiles

| Environment profile | Provider | Initial validation disposition | Health-test allocation | Evidence disposition |
|---|---|---|---|---|
| OE-01 Ubuntu 24.04.4 x86_64/GCC 13 | `linux-getrandom`, flags `0` | `SELECTED_INTERNAL_CANDIDATE` | External provider | Exact retained execution and institution acceptance required. |
| Windows engineering branch | `BCryptGenRandom` | `DEFERRED` | External provider | Actual hosted evidence absent. |
| Apple engineering branch | `SecRandomCopyBytes` | `DEFERRED` | External provider | Actual hosted evidence absent. |
| Callback test seam | Deterministic/failure callback | `TEST_ONLY` | Caller or explicit raw-candidate test owner | Forbidden in Release, install SDK, and PoC. |
| Direct raw-noise source | None | `OUT_OF_SCOPE_INITIAL_CANDIDATE` | Not assigned | Separate scope and validation plan required. |

## Required pre-consultation decisions

- Is the selected OS-provider allocation acceptable for the intended KCMVP module form?
- What kernel/provider documentation and retained test evidence are required?
- Must provider failure latch the module into `ERROR_STATE`?
- Are separate `getrandom` calls for entropy input and nonce acceptable?
- Must the explicit-entropy DRBG API be excluded or more tightly constrained in the
  submitted service profile?
