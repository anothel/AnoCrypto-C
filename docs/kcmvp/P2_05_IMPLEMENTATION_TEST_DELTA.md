# P2-05 CSP and Zeroization Implementation / Test Delta

Status: P2-05 internal closure register. Formal acceptance remains external.

## Implemented in P2-05

| ID | Change | Code / evidence | Public ABI impact | Status |
|---|---|---|---|---|
| `P2-05-IMP-001` | Clear all live module-owned CSP slots during orderly module shutdown. | `module_state.c`, `csp_handle.c`, shutdown test | None | `IMPLEMENTED` |
| `P2-05-IMP-002` | Clear live module-owned CSP slots before locked fatal-state exposure and during fatal shutdown acknowledgment. | same plus fatal test | None | `IMPLEMENTED` |
| `P2-05-IMP-003` | Preserve generation and expose `ZEROIZED` tombstone so explicit destroy remains possible after shutdown/fatal transition. | CSP state logic and tests | No declaration/symbol change; reviewed semantic tightening | `IMPLEMENTED` |
| `P2-05-IMP-004` | Formal CSP/sensitive-intermediate and service-operation inventories. | P2-05 documents | None | `IMPLEMENTED` |
| `P2-05-IMP-005` | Add machine-checked policy and local compiler-output capture. | release/tool/CMake evidence | None | `IMPLEMENTED_LOCAL_ENGINEERING` |

## Remaining implementation or environment gaps

| ID | Required work | Owner / phase | ABI expectation | Current status |
|---|---|---|---|---|
| `P2-05-GAP-001` | Implement selected P2-03 integrity verifier and inventory its real key/tag/parser state. | integrity implementation gate | No public change expected | `NOT_IMPLEMENTED` |
| `P2-05-GAP-002` | Run and review optimized zeroization output on exact OE-01 GCC 13.3.0. | retained selected-OE lane | None | `NOT_RUN` |
| `P2-05-GAP-003` | Obtain formal CSP classification and acceptable zeroization-evidence method. | pre-consultation/testing institution | None | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-05-GAP-004` | Define core-dump, swap, hibernation, crash, debugger and VM snapshot operator controls. | P2-07/operator docs | None | `PARTIAL` |
| `P2-05-GAP-005` | Decide whether lock-acquisition failure needs an emergency mechanism beyond process termination. | P2-06/external review | Internal only | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-05-GAP-006` | Review compiler register/spill/inlining behavior for all sensitive contexts, not only the primitive loop. | P2-06 | None | `DEFERRED_TO_P2_06` |

## Test and evidence IDs

| ID | Scenario | Expected result | Status |
|---|---|---|---|
| `P2-05-TST-001` | public explicit buffer zeroize | full requested buffer reads zero | Implemented |
| `P2-05-TST-002` | orderly shutdown with active HMAC/CBC/CMAC handles | all states `ZEROIZED`, invalid for use | Implemented |
| `P2-05-TST-003` | explicit destroy after shutdown wipe | `ZEROIZED -> DESTROYED` succeeds once | Implemented |
| `P2-05-TST-004` | reinit after orderly shutdown | old handles remain invalid; new lifecycle allowed | Implemented |
| `P2-05-TST-005` | fatal transition with active handle | state `ZEROIZED`; sticky module error remains | Implemented |
| `P2-05-TST-006` | optimized local compiler capture | artifact captured, manual review required, no sensitive values | Implemented local-only |
| `P2-05-TST-007` | exact OE-01 compiler capture/reviewer disposition | hash-bound retained evidence | `NOT_RUN` |
| `P2-05-TST-008` | full CTest/policy/install/PoC/ABI/evidence regression | no regression and 14/69/ABI unchanged | Required for final P2-05 artifact |
