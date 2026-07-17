# KCMVP Security Findings Register — P2-06

Status: internal engineering register. Entries are not formal vulnerability or
validation dispositions unless explicitly stated.

| ID | Class | Observation | Disposition | Status / next action |
|---|---|---|---|---|
| `P2-06-F-001` | Secure coding / low | Clang strict review reported that `ANO_CRYPTO_CSP_STATE_INVALID` was not explicitly handled in an internal state-normalization switch. | Add explicit case mapping to `INVALID`; no ABI change. | `FIXED_LOCAL` |
| `P2-06-F-002` | Tooling / informational | GCC 14 `-fanalyzer` reported possible uninitialized SHA-256 schedule use. Manual review confirmed words 0–15 are initialized before words 16–63 are expanded. | Retain diagnostic and manual rationale; do not add redundant secret-state initialization solely to silence the analyzer. | `REVIEWED_FALSE_POSITIVE_LOCAL` |
| `P2-06-F-003` | Build hardening / medium | Baseline Linux Release binary had NX stack and partial RELRO but no BIND_NOW or observable stack-protector reference. | Add FORTIFY 3, strong stack protector, stack-clash protection and full RELRO/BIND_NOW/NX overlay. | `REMEDIATED_LOCAL_EXACT_OE_NOT_RUN` |
| `P2-06-F-004` | Side channel / readiness high | Exact OE-01 GCC 13.3.0 disassembly and named human disposition do not exist. | Run immutable OE-01 build; hash binary/disassembly and record reviewer/date/disposition. | `NOT_RUN` |
| `P2-06-F-005` | Side channel / readiness high | No approved statistical timing or cache-leakage campaign has been performed. | Confirm required methodology and execute on selected hardware if required. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-F-006` | Fault/physical / scope | Power, EM, transient-execution and active fault behavior have not been evaluated. | Confirm applicability/security level; do not claim resistance. | `NOT_EVALUATED` |
| `P2-06-F-007` | Fatal cleanup / medium | If the global lock itself fails, fatal state can be latched without safely traversing and wiping the CSP table. | Require process termination; obtain external review on whether an emergency mechanism is needed. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-F-008` | Integrity / readiness high | Selected HMAC-based module integrity verifier is not implemented. | Retain P2-03 implementation gate and tamper-test plan. | `NOT_IMPLEMENTED` |
| `P2-06-F-009` | Memory lifecycle / medium | Caller-owned contexts, crash dumps, swap, hibernation, debugger and VM snapshots are outside automatic global cleanup. | Define operator controls and guidance in P2-07. | `PARTIAL` |
| `P2-06-F-010` | Timing assumption / informational | HMAC key length, message length, output length, algorithm choice and mode affect control flow/work. | Treat these as public API metadata unless consultation requires otherwise; prohibit universal constant-time wording. | `DOCUMENTED_ASSUMPTION` |
| `P2-06-F-011` | Platform hardening / low | CET/IBT/shadow-stack are not selected in the candidate build. | Decide after CPU/OS support and institution expectations are known. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-F-012` | Hosted evidence / external blocker | Actual hosted lanes and review remain unavailable because GitHub Actions billing prevented runner allocation. | Resume P1-34 only after billing resolution; do not substitute local results. | `BLOCKED_EXTERNAL` |

## Closure rule

A finding may move to `CLOSED` only when its code/document change, test or review
artifact, reviewer, date, target identity, and evidence hash are retained. Local
remediation does not close institution-dependent or exact-OE findings.
