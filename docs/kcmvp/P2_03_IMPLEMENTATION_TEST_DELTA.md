# P2-03 Implementation and Test Delta Register

Status: controlled gap list following the P2-03 design freeze. Items are not complete merely because the specification exists.

## Status taxonomy

- `READY_TO_IMPLEMENT_AFTER_GATE`
- `EXTERNAL_CONFIRMATION_REQUIRED`
- `PLANNED_TEST`
- `CURRENT_PARTIAL`
- `DEFERRED_TO_P2_04`
- `DEFERRED_TO_P2_05`
- `DEFERRED_TO_P2_06`
- `OUT_OF_SCOPE`

## Implementation deltas

| ID | Delta | Files/components likely affected | Public ABI | Gate/status | Completion evidence |
|---|---|---|---|---|---|
| `P2-03-IMP-001` | Implement `INT-HMAC-SHA256-SINGLE-SO-R1` internal metadata parser, canonical reader, HMAC verification, constant-time tag compare, and zeroization. | new internal integrity component; `module_state.c`; build finalization tool | No change expected | `EXTERNAL_CONFIRMATION_REQUIRED` | selected-OE pass/tamper evidence; compiler review; no new export |
| `P2-03-IMP-002` | Make public on-demand self-test acquire state ownership, block services, and restore `OPERATIONAL` only on complete success. | `module_state.c`, `selftest.c`, internal interface | No change | `READY_TO_IMPLEMENT_AFTER_GATE` | concurrent/state tests |
| `P2-03-IMP-003` | Reject every unknown self-test flag bit while preserving the published numeric mask. | `selftest.c` | No change | `READY_TO_IMPLEMENT_AFTER_GATE` | negative flag tests and compatibility check |
| `P2-03-IMP-004` | Retain first fatal cause or document an explicitly reviewed replacement precedence. | `module_state.c` | No change | `READY_TO_IMPLEMENT_AFTER_GATE` | multi-error test in isolated process |
| `P2-03-IMP-005` | Classify internal global-lock failure as fatal without unsafe unlocked mutation; design a testable fail-closed path. | thread abstraction/module state | No change | `DEFERRED_TO_P2_06` | secure-coding review plus fault-injection test |
| `P2-03-IMP-006` | Add controlled integrity-key generation/injection/finalization tooling with no secret output. | release/build tooling, protected procedure | No change | `EXTERNAL_CONFIRMATION_REQUIRED` | key-free logs, role review, artifact hash binding |
| `P2-03-IMP-007` | Bind integrity metadata to candidate ID, runtime version, exact file length, and one fixed internal format version. | release candidate record and new metadata format | No change | `READY_TO_IMPLEMENT_AFTER_GATE` | parser/canonicalization tests |
| `P2-03-IMP-008` | Add installation rule that preserves finalized bytes and rejects post-finalization modification in the controlled release flow. | install/package/provenance tooling | No change | `READY_TO_IMPLEMENT_AFTER_GATE` | install artifact identity and tamper regression |
| `P2-03-IMP-009` | Add integrity key to formal CSP and zeroization inventory. | P2-05 documents/tests | No change | `DEFERRED_TO_P2_05` | CSP inventory row and target evidence |
| `P2-03-IMP-010` | Finalize OS entropy/health failure transitions without conflating them with startup KATs. | entropy/random state paths | No change by default | `DEFERRED_TO_P2_04` | P2-04 provider/failure matrix |

## Test deltas

| ID | Test | Expected result | Status |
|---|---|---|---|
| `P2-03-TST-001` | Valid finalized shared object | integrity check PASS, then startup KATs, then `OPERATIONAL` | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-03-TST-002` | One measured byte modified | integrity failure and sticky `ERROR_STATE` | `PLANNED_TEST` |
| `P2-03-TST-003` | Tag byte modified | same | `PLANNED_TEST` |
| `P2-03-TST-004` | Metadata magic/version/offset/length malformed | fail closed without out-of-bounds access | `PLANNED_TEST` |
| `P2-03-TST-005` | File missing, replaced, symlinked, unreadable, or path ambiguous | fail closed according to frozen OE-01 path policy | `PLANNED_TEST` |
| `P2-03-TST-006` | Wrong candidate ID or runtime version | integrity failure | `PLANNED_TEST` |
| `P2-03-TST-007` | Startup KAT forced failure in dedicated Debug test build | sticky `ERROR_STATE`; no values logged | `CURRENT_PARTIAL` |
| `P2-03-TST-008` | On-demand full test from `OPERATIONAL` | temporary service denial; PASS restores operation | `PLANNED_TEST` |
| `P2-03-TST-009` | Concurrent self-test requests | one owner; others receive non-fatal busy result | `PLANNED_TEST` |
| `P2-03-TST-010` | Direct self-test from `UNINITIALIZED`, `SHUTDOWN`, `ERROR_STATE` | not-initialized/not-fatal-state semantics as specified | `PLANNED_TEST` |
| `P2-03-TST-011` | Orderly shutdown then reinit | complete startup sequence reruns and operation resumes | `IMPLEMENTED_P2_03_REGRESSION` |
| `P2-03-TST-012` | Init while operational | idempotent success without self-test rerun | `CURRENT_PARTIAL` |
| `P2-03-TST-013` | Fatal then shutdown then init | fatal latch remains; no in-process recovery | `CURRENT_PARTIAL` |
| `P2-03-TST-014` | Unknown self-test flag bits | `UNSUPPORTED_PARAMETER`, no fatal transition | `CURRENT_PARTIAL` |
| `P2-03-TST-015` | Query availability in every state | only documented read-only APIs remain available | `PLANNED_TEST` |
| `P2-03-TST-016` | Sanitizer/fuzz parser corpus for integrity record | no memory/length/overflow defects | `DEFERRED_TO_P2_06` |
| `P2-03-TST-017` | OE-01 compiler output review for compare/zeroization/parser | reviewed disposition, no universal claim | `DEFERRED_TO_P2_06` |

## Documentation deltas after consultation

- record accepted integrity mechanism or required alternative;
- record exact security level and self-test classifications;
- resolve HMAC key acceptance, custody, rotation, and reproducibility;
- update P2-02 module boundary if an external file or verifier becomes required;
- convert this register into implementation work items, one controlled commit per accepted slice;
- do not mark P1-34 actual hosted evidence complete while billing remains blocked.
