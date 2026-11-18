# Hosted smoke reliability implementation plan

Goal: diagnose the two reported hosted failures, fix confirmed defects, and keep failures observable without weakening any checks.

Scope: branch from main; separate Draft PR; do not modify PRs 1–3 or release authorization. Use CMake 3.16-compatible scripts and existing CI.

- [x] Inspect complete original job logs and scripts; compare main and PR 2.
- [x] Reproduce unchanged main and PR 2 on hosted Linux and Windows. Windows native loader status is 0xc0000135; installed bin on PATH resolves it. Ubuntu currently passes on both.
- [x] Add failing regression cases for nested configure/build/export-check diagnostics and failure propagation.
- [x] Print captured output and native results at each affected package boundary; retain logs on CI failure.
- [x] Add installed bin to Windows smoke process PATH; retain native exit diagnostics.
- [x] Replace exploratory workflow with focused Debug/Release installed-package and Linux PoC regression checks, independent of unrelated main policy failures.
- [ ] Run full local CTest, package checks, hosted regressions, and unchanged required CI; document exact outcomes and unresolved causes.

Review focus: nested nonzero exits stay fatal; stdout/stderr stay available; loader failures report native status; SDK is loaded from installed prefix; no assertions, policy gates, crypto implementation, public API/ABI, or release authorization change.

Review: keep diagnostics standalone to preserve the existing indexed 102-test evidence suite. Verified final PoC remains test 102. New CTest invocation uses cmake -E chdir for CMake 3.16 compatibility. Independent review found no further material issues.
