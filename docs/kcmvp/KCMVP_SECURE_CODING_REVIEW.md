# KCMVP Secure-Coding Review — P2-06

Status: local engineering review under `SCP-P2-06-R1`. Exact OE-01 execution,
independent review, and formal institution acceptance remain pending.

## 1. Scope and claims boundary

The review covers the C99 product sources under `src/`, the 14 public headers,
build configuration, local compiler diagnostics, static analysis, ELF hardening,
existing sanitizer regressions, and security-sensitive lifecycle paths. It does
not establish that the module is formally validated, universally constant-work,
fault resistant, power/EM resistant, or safe on every compiler and CPU.

The review retains the P2-02 candidate
`AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`. Local GCC 14.2 and Clang 17 runs are
engineering inputs only and are not substitutes for an exact Ubuntu 24.04 / GCC
13.3.0 retained run.

## 2. Review methods

| Method | Local result | Meaning |
|---|---|---|
| Manual source review | `COMPLETE_LOCAL` | Reviewed input validation, integer/length checks, state/lifecycle handling, cleanup, error paths, and secret-dependent control/memory candidates. |
| High-risk API scan | PASS | Product source contains no `gets`, unbounded string copy/format APIs, process-spawn APIs, non-cryptographic PRNG APIs, or dynamic allocation. |
| GCC 14 strict compile | PASS | `-Wall -Wextra -Wpedantic` plus conversion, shadow, prototype, format, VLA/alloca, and related diagnostics. |
| GCC 14 `-fanalyzer` | `PASS_WITH_REVIEWED_FALSE_POSITIVE` | One SHA-256 message-schedule diagnostic was manually reviewed; the first 16 words are initialized before expansion. No source defect was accepted from that report. |
| Clang 17 strict compile | PASS after one fix | An explicit `ANO_CRYPTO_CSP_STATE_INVALID` switch case was added; no public API changed. |
| Clang 17 static analyzer | PASS | Product sources produced no analyzer diagnostics under the local Linux provider/thread configuration. |
| ASan/UBSan regression | Retained | Existing engineering lane remains part of regression evidence; hosted results are separate. |
| ELF hardening review | Implemented locally | Release Linux GNU/Clang builds enable FORTIFY 3, strong stack protector, stack-clash protection, full RELRO/BIND_NOW, and non-executable stack. |

## 3. Secure-coding checklist disposition

| Topic | Disposition | Notes |
|---|---|---|
| C99-only core | PASS | No C++ runtime, exceptions, RTTI, or STL in the product library. |
| Public/private boundary | PASS | Internal headers and test hooks remain outside installed SDK/public ABI. |
| Allocation discipline | PASS | Product source uses fixed/caller-owned storage; no heap allocation. |
| Input and output lengths | PASS_WITH_LIMITATIONS | Public APIs validate null/length combinations, size queries, complete blocks, and output capacity; formal proof is not claimed. |
| Integer overflow | PASS_WITH_REVIEW | Hash length accumulation and buffer arithmetic contain explicit limits/checks; target-specific proof remains out of scope. |
| Aliasing/overlap | PASS_WITH_DOCUMENTED_SCOPE | Exact in-place CBC is allowed; partial overlap is rejected; other APIs retain their documented rules. |
| Initialization/reinitialization | PASS | Failed initialization clears private contexts; stale handles use generation checks. |
| Error-state handling | PASS_WITH_OPEN_ITEM | Fatal state is sticky; lock-acquisition failure cannot safely sweep the CSP table and requires process termination/external review. |
| Sensitive cleanup | PASS_WITH_LIMITATIONS | Explicit volatile-store zeroization and P2-05 table cleanup are implemented; registers, crash dumps, swap, hibernation, and physical remanence are not universally controlled. |
| Authentication comparison | SOURCE_CANDIDATE | Full 16-byte CMAC verification accumulates all differences and does not exit on mismatch position. Exact OE-01 compiler/timing review remains open. |
| Diagnostics/logging | PASS | Product code does not log secrets; evidence remains status/length/hash only. |
| Build hardening | IMPLEMENTED_LOCAL | P2-06 Linux Release overlay is machine-checked; exact OE-01 verification remains `NOT_RUN`. |

## 4. Implemented remediation

1. Added an explicit `ANO_CRYPTO_CSP_STATE_INVALID` branch to the internal CSP
   state normalization switch, resolving the Clang strict diagnostic.
2. Expanded the non-MSVC strict warning set without enabling `-Werror`.
3. Added `ANO_CRYPTO_ENABLE_RELEASE_HARDENING`, enabled by default, with a
   Linux GNU/Clang Release overlay:
   - `_FORTIFY_SOURCE=3`;
   - `-fstack-protector-strong`;
   - `-fstack-clash-protection`;
   - `-Wl,-z,relro,-z,now,-z,noexecstack`.
4. The P2-02 enforce-candidate path now fails closed if this hardening overlay is
   disabled.
5. Added source policy, ELF hardening capture, findings register, compiler/CPU
   matrix, and status-only operational review.

These changes do not add a public function, type, enum value, header, algorithm,
or exported symbol.

## 5. Open items

- Exact OE-01 GCC 13.3.0 warning/analyzer/disassembly/hardening capture is
  `NOT_RUN`.
- Independent human compiler-output review has no named reviewer/disposition.
- No statistical timing test, cache-profiling campaign, power/EM test, or fault
  injection has been completed.
- CET/IBT/shadow-stack enablement is not selected because deployment support and
  institution expectations require confirmation.
- Runtime module integrity remains `SELECTED_CANDIDATE_NOT_IMPLEMENTED`.
- Forced termination, debugger attachment, core dumps, swap, hibernation, and VM
  snapshots require operator/environment controls in P2-07.

## 6. Allowed wording

Allowed:

- source-level fixed-work candidate;
- table-free AES source implementation;
- local strict-compiler/static-analysis review passed with recorded limitations;
- selected-OE and external side-channel review pending.

Not allowed:

- completion of a constant-work certification assessment;
- side-channel resistant on all targets;
- fault resistant;
- subject to a completed independent audit or formal validation;
- actual hosted review completed.
