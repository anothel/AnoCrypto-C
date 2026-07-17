# P2-06 Secure-Coding and Side-Channel Implementation / Test Delta

Status: local review/remediation complete; selected-OE and external acceptance
remain open.

## Implemented in P2-06

| ID | Change | Evidence | Public ABI | Status |
|---|---|---|---|---|
| `P2-06-IMP-001` | Explicitly handle internal CSP `INVALID` enum state. | Clang strict compile and regression CTest | None | `IMPLEMENTED` |
| `P2-06-IMP-002` | Expand strict C diagnostics for non-MSVC builds. | GCC/Clang clean compile | None | `IMPLEMENTED` |
| `P2-06-IMP-003` | Add Linux GNU/Clang Release hardening overlay. | ELF capture verifies stack/NX/RELRO/BIND_NOW/stack protector | None | `IMPLEMENTED_LOCAL_ENGINEERING` |
| `P2-06-IMP-004` | Add machine-readable secure-coding/side-channel policy and source guard. | `SECURE_CODING_SIDE_CHANNEL_POLICY_CHECK.txt` | None | `IMPLEMENTED` |
| `P2-06-IMP-005` | Add findings register, compiler/CPU matrix, secure-coding and side-channel reviews. | controlled P2-06 documents | None | `IMPLEMENTED` |
| `P2-06-IMP-006` | Add status-only CMAC verification operational review and sensitive-output boundary. | `SECURE_CODING_POLICY_REVIEW.txt` | None | `IMPLEMENTED` |

## Open implementation/review items

| ID | Item | Owner/gate | Status |
|---|---|---|---|
| `P2-06-GAP-001` | Execute exact OE-01 GCC 13.3.0 strict/analyzer/disassembly/hardening review. | retained selected-OE lane | `NOT_RUN` |
| `P2-06-GAP-002` | Obtain accepted timing/cache methodology and execute if required. | pre-consultation/testing institution | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-GAP-003` | Decide CET/IBT/shadow-stack build policy. | operating-environment/security review | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-GAP-004` | Decide emergency CSP cleanup expectation when the global lock fails. | external secure-coding review | `EXTERNAL_CONFIRMATION_REQUIRED` |
| `P2-06-GAP-005` | Implement and review P2-03 module integrity candidate. | separate accepted design gate | `NOT_IMPLEMENTED` |
| `P2-06-GAP-006` | Define crash/core/swap/hibernation/debugger/VM operator controls. | P2-07 | `DEFERRED_TO_P2_07` |
| `P2-06-GAP-007` | Determine power/EM/fault/transient-execution applicability. | consultation/security-level decision | `NOT_EVALUATED` |
| `P2-06-GAP-008` | Resume actual hosted compiler/platform evidence. | GitHub billing resolution | `BLOCKED_EXTERNAL` |

## Required final regression

- policy and source guard;
- Debug and Release CTest;
- ASan/UBSan where available;
- clean 69-symbol ABI and P1-29 compatibility;
- installed package and customer PoC;
- AES/CMAC/zeroization/compiler-hardening capture;
- provenance, sensitive-artifact scan and evidence manifest;
- patch application and ZIP extraction identity.
