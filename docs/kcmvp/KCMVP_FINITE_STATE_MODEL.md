# KCMVP Finite-state Model

Status: `P2-03 FROZEN_INTERNAL_POLICY`; selected integrity transition is not implemented.

Document ID: `AC-KCMVP-FSM-P2-03-R1`

## 1. States

The public numeric state values and names remain unchanged on ABI line `ANOCRYPTO_C_0`.

| State | Meaning | Cryptographic services |
|---|---|---|
| `UNINITIALIZED` | Library loaded, startup sequence not completed. | Denied. |
| `SELF_TEST_RUNNING` | Mandatory startup or accepted on-demand sequence is executing. | Denied. |
| `OPERATIONAL` | Mandatory sequence completed successfully. | Allowed subject to each service contract. |
| `ERROR_STATE` | A fatal condition is latched for this process. | Denied permanently for this process. |
| `SHUTDOWN` | Orderly shutdown completed. | Denied until successful reinitialization. |

## 2. Target transition diagram

```text
                         init(NULL)
      +--------------------------------------------------+
      |                                                  v
UNINITIALIZED -------------------------------> SELF_TEST_RUNNING
      |                                            |       |
      | shutdown                                   | PASS  | FATAL FAIL
      v                                            v       v
   SHUTDOWN <------------- shutdown ---------- OPERATIONAL ERROR_STATE
      |                                            |       ^
      | init(NULL)                                 |       |
      +--------------------> SELF_TEST_RUNNING <----+       |
                             on-demand full test            |
                                  |                         |
                                  +------- FATAL FAIL ------+

ERROR_STATE -- shutdown/queries --> ERROR_STATE
ERROR_STATE -- process termination + fresh load --> new UNINITIALIZED instance
```

There is no transition from `ERROR_STATE` to `OPERATIONAL`, `SHUTDOWN`, or `UNINITIALIZED` inside the same process instance.

## 3. Transition table

| From | Event | Preconditions | To | Return/disposition | Current alignment |
|---|---|---|---|---|---|
| `UNINITIALIZED` | `module_init(NULL)` | no fatal latch | `SELF_TEST_RUNNING` | run mandatory sequence | `CURRENT` |
| `UNINITIALIZED` | cryptographic service | none | unchanged | `ANO_CRYPTO_ERR_NOT_INITIALIZED` | `CURRENT` |
| `UNINITIALIZED` | `module_shutdown()` | none | `SHUTDOWN` | `ANO_CRYPTO_OK` | `CURRENT`, not separately tested |
| `SELF_TEST_RUNNING` | all mandatory checks pass | integrity/KATs complete | `OPERATIONAL` | `ANO_CRYPTO_OK` | KAT path `CURRENT`; integrity pending |
| `SELF_TEST_RUNNING` | fatal check fails | failure status available | `ERROR_STATE` | original fatal status, latch | `CURRENT` for structural/KAT; integrity pending |
| `SELF_TEST_RUNNING` | cryptographic service | none | unchanged | denied | service gate `CURRENT`; concurrent timing tests partial |
| `SELF_TEST_RUNNING` | re-entrant init/test | same instance | unchanged or fatal on actual corruption | reject as busy; proposed `RESOURCE_EXHAUSTED` | `PLANNED_IMPLEMENTATION` for on-demand path |
| `OPERATIONAL` | `module_init(NULL)` | already initialized | `OPERATIONAL` | idempotent `ANO_CRYPTO_OK`; no rerun | `CURRENT` |
| `OPERATIONAL` | cryptographic service | valid service inputs | `OPERATIONAL` | service result | `CURRENT` |
| `OPERATIONAL` | accepted on-demand self-test | exclusive transition acquired | `SELF_TEST_RUNNING` | block services and run full sequence | `PLANNED_IMPLEMENTATION` |
| `OPERATIONAL` | ordinary service error | no module corruption | `OPERATIONAL` | service-specific error | `CURRENT/PER_SERVICE` |
| `OPERATIONAL` | fatal internal condition | fatal classifier | `ERROR_STATE` | latch fatal status | `CURRENT` internal hook/selected paths |
| `OPERATIONAL` | `module_shutdown()` | none | `SHUTDOWN` | `ANO_CRYPTO_OK` | `CURRENT` |
| `SHUTDOWN` | `module_init(NULL)` | no fatal latch | `SELF_TEST_RUNNING` | rerun complete startup sequence | `CURRENT`; P2-03 regression added |
| `SHUTDOWN` | cryptographic service | none | unchanged | `ANO_CRYPTO_ERR_NOT_INITIALIZED` | `CURRENT` |
| `SHUTDOWN` | `module_shutdown()` | none | `SHUTDOWN` | idempotent `ANO_CRYPTO_OK` | `CURRENT`, test coverage partial |
| `ERROR_STATE` | cryptographic service/init/on-demand test | fatal latch set | `ERROR_STATE` | `ANO_CRYPTO_ERR_FATAL_STATE` | init/service gate `CURRENT`; direct self-test target pending |
| `ERROR_STATE` | query API | valid output pointer | `ERROR_STATE` | query result | `CURRENT` |
| `ERROR_STATE` | `module_shutdown()` | none | `ERROR_STATE` | acknowledge without clearing latch | `CURRENT` |
| `ERROR_STATE` | terminate process and load clean artifact | external operator action | new `UNINITIALIZED` instance | new process lifecycle | operational policy |

## 4. Service-availability matrix

Legend: `A` allowed, `D` denied, `Q` query/read-only, `T` target behavior not yet implemented.

| API category | UNINITIALIZED | SELF_TEST_RUNNING | OPERATIONAL | ERROR_STATE | SHUTDOWN |
|---|---:|---:|---:|---:|---:|
| Version/build/fingerprint queries | Q | Q | Q | Q | Q |
| Module state/name/last-error queries | Q | Q | Q | Q | Q |
| Self-test last-status/last-flags queries | Q | Q | Q | Q | Q |
| `module_init(NULL)` | A | D | A/idempotent | D | A |
| `module_shutdown()` | A | synchronization-dependent | A | A/no recovery | A/idempotent |
| Public on-demand `selftest_run` | D/T | D/T | A/T | D/T | D/T |
| Hash/HMAC operations | D | D | A | D | D |
| DRBG and public random operations | D | D | A | D | D |
| AES/CBC/CMAC operations | D | D | A | D | D |
| CSP key/handle operations | D | D | A | D | D |
| Installed-package discovery/linking | external build activity | external | external | external | external |

Query availability does not authorize cryptographic processing and does not clear failure state.

## 5. Fatal latch and last error

- The fatal latch is process-local and monotonic from unset to set.
- A fatal transition must store a non-OK status. If no specific status is available, use `ANO_CRYPTO_ERR_INTERNAL_ERROR`.
- P2-03 target policy is to retain the first fatal cause for operator diagnosis. Current code can overwrite it through repeated internal error marking; this is an owned implementation delta.
- `module_get_last_error` reports status only and must not expose key, tag, vector, entropy, or caller data.
- Ordinary service errors do not update the module-global fatal error.

## 6. Concurrency and synchronization

- State transitions and fatal-latch updates require the internal global lock.
- Cryptographic services must call the operational gate before processing.
- An on-demand transition must acquire exclusive state ownership before the first integrity/KAT operation.
- Concurrent callers observing `SELF_TEST_RUNNING` must fail without using partially tested services.
- If the global lock cannot protect state integrity, the failure is fatal; current lock-failure behavior needs a dedicated P2-03 implementation/test review.
- Caller-owned mutable contexts remain subject to their separate serialization rules.

## 7. Recovery model

### Orderly restart

`SHUTDOWN` is not a fatal state. Reinitialization in the same process is allowed and must rerun the complete startup sequence. The allowed orderly path is `SHUTDOWN -> SELF_TEST_RUNNING -> OPERATIONAL` after all mandatory checks pass.

### Fatal recovery

No public or internal reset is authorized after `ERROR_STATE`. Recovery requires:

1. stop the consumer process;
2. capture status-only diagnostics and artifact/environment identity;
3. investigate integrity, installation, hardware/OS, and build provenance;
4. reinstall or replace through an authorized procedure where necessary;
5. start a new process, which begins in `UNINITIALIZED` and reruns all mandatory tests.

## 8. Implementation and test deltas

The authoritative delta list is `docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md`. The principal gaps are:

- actual module-integrity implementation and tamper tests;
- controlled on-demand state transitions and flag validation;
- first-fatal-cause retention;
- lock-failure classification and test strategy;
- complete state/event/service matrix regression coverage.

These gaps are not hidden by the P2-03 policy checker.
