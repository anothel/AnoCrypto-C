# P2-04 Entropy/DRBG Implementation and Test Delta

Status: internal gap register for `EDP-P2-04-R1`.

| ID | Target policy | Current implementation/evidence | Delta | Disposition |
|---|---|---|---|---|
| EDP-DELTA-001 | Accept OE-01 Linux `getrandom(flags=0)` provider boundary. | Adapter and candidate environment are implemented. | Institution acceptance and exact retained OE evidence absent. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| EDP-DELTA-002 | Assign raw-source health tests to the correct owner. | OS output is classified external-conditioned and rejects health-skeleton attachment. | Required external provider documentation/evidence not selected. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| EDP-DELTA-003 | Keep direct noise source outside initial candidate. | No Release direct-source implementation. | Existing raw-callback health skeleton must remain test-only and must not be cited as final health testing. | `READY_FOR_INTERNAL_REVIEW` |
| EDP-DELTA-004 | Fix instantiate/reseed/generate limits. | 32-byte entropy, 16-byte nonce, 65,536-byte request, `2^48` reseed interval implemented. | Entropy credit, stricter operational interval, and personalization/additional-input profile require confirmation. | `PARTIAL` |
| EDP-DELTA-005 | Prediction resistance policy. | Flag is rejected. | None unless future scope gate approves per-request entropy. | `READY_FOR_INTERNAL_REVIEW` |
| EDP-DELTA-006 | Provider failure mapping. | Init clears context; manual reseed failure preserves old state; module remains operational. | Confirm recoverable versus fatal treatment for selected validation target. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| EDP-DELTA-007 | Fork/process behavior. | Source-backed process marker and reseed-required behavior implemented and tested. | Exact OE-01 retained fork evidence pending. | `PARTIAL` |
| EDP-DELTA-008 | VM/container checkpoint duplication. | No automatic detection. | Automatic support not planned for initial candidate; operator rule must be accepted. | `NOT_IMPLEMENTED` |
| EDP-DELTA-009 | Suspend/resume freshness. | No automatic detection. | Select discard/reinitialize or reviewed reseed-only profile per deployment. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| EDP-DELTA-010 | Explicit-entropy public DRBG integration responsibility. | Caller-managed API accepts minimum-length input. | Submitted service profile and caller entropy obligations require confirmation. | `EXTERNAL_CONFIRMATION_REQUIRED` |
| EDP-DELTA-011 | Status-only entropy/DRBG evidence. | Existing tests plus new policy checker/review record avoid byte disclosure. | Exact OE and human review metadata pending. | `PARTIAL` |
| EDP-DELTA-012 | Actual hosted evidence. | Workflow definitions and synthetic/local checks only. | GitHub Actions billing blocks real runner/artifact execution. | `BLOCKED_EXTERNAL` |

## P2-05 handoff

P2-05 must inventory HMAC_DRBG `Key`, `V`, reseed counter, process marker, temporary
entropy/nonce buffers, personalization/additional input treatment, and zeroization paths.
The process marker is non-secret policy state but belongs in the complete state inventory.
