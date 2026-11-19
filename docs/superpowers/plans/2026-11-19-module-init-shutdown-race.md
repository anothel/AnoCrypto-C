# Module initialization / shutdown race implementation plan

Goal: resolve Issue #8 without changing API/ABI or the 102-test evidence index.
Architecture: assign each admitted initialization a non-reused uint64 generation under the global lock. Final success requires both the captured generation and SELF_TEST_RUNNING to remain current. Shutdown preempts old initialization; a new request may initialize normally. Fatal checks and zeroization retain precedence. Saturation rejects new startup rather than wrapping.

- [x] Reproduce shutdown during startup and old-init/shutdown/new-init ABA using production module_state.c with a standalone test-only dependency fixture and deterministic thread barriers. Keep the existing library-backed module tests, and run the fixture under their existing CTest registration so indexing is unchanged.
- [x] Implement the generation/state guard. Preempted successful init returns existing NOT_INITIALIZED; no public status or symbol is added. Keep self-test failure handling and fatal-state precedence.
- [x] Exercise both old/new completion orders, fatal transition and failing startup, plus normal later reinit. Retain real CSP zeroization tests. Thread-safety OFF remains caller-serialized.
- [ ] Run focused/full tests, Debug/Release, ASan+UBSan, clean ABI/policy checks and release-evidence collection. Verify index registration remains unchanged.
- [ ] Document contract, reproduction and limitations; commit and open a Draft PR linked to #8, then verify push and PR hosted runs. No merge or release; EXTERNAL_RELEASE_NOT_AUTHORIZED.

Review focus: newer SELF_TEST_RUNNING is not proof of ownership; stale successful init must not clear a newer fatal state; mandatory self-test failures stay fatal even if obsolete; cancellation is not fatal and must preserve future reinit; generation must not wrap. Emergency lock-failure cleanup remains a separate documented limitation.

Progress: original-source schedules 0/1/2 fail as expected; state-only mutation fails scenario 1. Debug full suite 102/102 and focused ASan+UBSan 17/17 passed (outside ptrace sandbox). TSan five schedules passed. OFF focused 3/3; clean ABI 85 symbols and compatibility/public-surface policies passed. Release evidence all 102 indices passed; final assembly correctly requires a committed clean tree, so rerun follows commit. No security check was disabled.
