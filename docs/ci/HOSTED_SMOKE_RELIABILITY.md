# Hosted smoke investigation

Scope: the two downstream Debug failures in PR 3 at `dafc5dc0a9e850f105b1677b8d7245ada776ff0a`, workflow run [44311394765](https://github.com/anothel/AnoCrypto-C/actions/runs/44311394765). The checked-out PR merge was `343f8bfeb3c0ab8de883be38723e3400e0861058`, based on main `b3d5e6551b2bcc38b39602592476146e4b81b9b0`.

Both complete original job logs were downloaded and inspected (Ubuntu job 134826160561, 1,186 lines; Windows job 134826160737, 1,213 lines). Both passed 92/92 CTests. Ubuntu passed its installed consumer and failed creating the PoC package; Windows built the installed consumer and failed launching it. The original artifacts did not include the nested PoC logs.

## Confirmed Windows cause

The direct PowerShell launch omitted `install-prefix/bin` from PATH. The consumer imports `anocrypto_c.dll`, installed in that directory, rather than alongside the consumer. The loader fails before C `main`, so adding C-level assertion messages would not expose this failure.

In [probe run 44313849692](https://github.com/anothel/AnoCrypto-C/actions/runs/44313849692), pinned unchanged main and PR 2 (`2110708252c988441594f08c5f7adb4262aa43df`) both returned `Exit code 0xc0000135` (STATUS_DLL_NOT_FOUND). Adding only the installed SDK bin directory made both same executables return 0 with the install-smoke PASS message. Jobs 134834645453 and 134834644946 retain those native diagnostics. This is a pre-existing workflow defect, unaffected by PR 2 or PR 3. Build-tree tests pass because their executables and DLL share a directory; the tagged compatibility wrapper already supplies the installed runtime directory.

## Ubuntu evidence and limit

The original `POC_PACKAGE_CREATE.log` was neither printed nor uploaded, and its runner workspace is unavailable. The outer log alone cannot establish its child failure. On the same hosted GNU 15.2.0 toolchain, unchanged main and PR 2 both passed Debug PoC packaging, including the clean SDK, 85-export baseline, release-compatibility/provenance gates, and packaged example. This repeated in probe runs 44313849692 and 44314588803. Local GCC 14.2 main also passed before changes and after the preceding policy targets.

Therefore the original Ubuntu child failure remains unconfirmed. Passing reproductions do not establish a transient or environmental cause. PR 2 does not change either package script or the platform-smoke workflow steps; its CMake changes concern RSA test expectations, and its workflow change enables entropy only in the differential lane. PR 3 changes two documentation passages and no smoke code. The hidden-diagnostic defect is already present on main; the original package failure cannot yet be classified more narrowly.

## Focused changes

- Prepend the installed SDK bin directory for the Windows workflow launch, report its native exit code, and propagate failures.
- Print captured stdout/stderr and native results before the existing fatal errors in PoC creation and the installed/PoC wrappers. Persistent log files remain available. Compare process results to the exact success string `0`.
- Upload nested package logs on failure in the existing CI matrix.
- Add standalone fault-injection regression coverage for nested configure stdout/stderr, build, export-baseline, and install failures. Each failure must remain fatal and its deepest diagnostic must reach the parent output.
- Add independent hosted Linux/Windows Debug/Release full-test and installed-SDK checks, Linux PoC checks, and Windows negative loader checks. Existing policy-gated CI runs unchanged apart from the Windows environment and diagnostics. The diagnostic regression runs separately so the existing indexed release-evidence test order and 102-test assertion remain intact.

The exploratory pinned-baseline workflow is retained in commit history only. No cryptographic implementation, public API/ABI, assertion, security policy, release identity, signing authorization, or release gate is changed. PRs 1–3 are untouched; this work is a separate Draft PR.

## Validation

Local GCC 14.2 Debug and Release: 92/92 existing CTests each, plus the four independent failure-injection cases, installed package with current and P1-29 header consumers, PoC clean SDK, 85-symbol baseline, provenance/compatibility checks, and packaged example passed. The new diagnostic regression failed before the logging changes and passes afterward. A temporary locally unstaged deletion triggered the existing provenance gate during development; staging the intended deletion resolved it without changing that gate.

Hosted final-branch results and any remaining blockers are recorded in the Draft PR. Main still has the two public-surface wording failures owned by PR 3 and the entropy-disabled RSA failures owned by PR 2; this branch deliberately does not incorporate either independent change. Keep the PR Draft pending review and required CI. External release remains `EXTERNAL_RELEASE_NOT_AUTHORIZED`.
