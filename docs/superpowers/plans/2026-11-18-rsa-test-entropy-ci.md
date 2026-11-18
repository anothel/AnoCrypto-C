# RSA test entropy CI implementation plan

> Execute inline using superpowers:executing-plans. The user's task authorizes implementation, verification, commit, push, and a new Draft PR.

**Goal:** Repair RSA KAT error propagation and accurately exercise RSA with and without blinding entropy.
**Architecture:** Keep production cryptography and public headers unchanged. Set test expectations from the configured, supported OS entropy provider; retain verification and exact fail-closed assertions when entropy is unavailable. Require real entropy for full OpenSSL differential signing in CI.
**Tech stack:** C99, CMake/CTest, GitHub Actions, OpenSSL 3.
**Spec:** User request in this chat; base main b3d5e6551b2bcc38b39602592476146e4b81b9b0.

## Global constraints

- Work branch fix/rsa-test-entropy-ci-v2; new Draft PR targeting main.
- Do not modify or merge PR #1, import its unrelated CSP fix, or recover/replay the lost commit.
- Preserve public API/ABI, secure RSA blinding, error precedence, zeroization, and all release/security gates.
- External release remains blocked; failures must be reported without bypasses.

## Review focus

- Unexpected entropy failure must fail tests in entropy-enabled builds.
- Provider-disabled signing must clear the entire output and write zero bytes for all RSA SHA-2 algorithms.
- Verification and size queries remain usable without entropy.
- Destroy races must accept only the documented outcomes for each configuration.
- Differential results must distinguish full bidirectional signing from verification-only coverage.

### Task 1: KAT propagation and configuration-aware tests

Files: CMakeLists.txt; tests/kat/rsa_pkcs1_v15_kat_tests.c; tests/negative/rsa_api_negative_tests.c; tests/unit/rsa_thread_safety_tests.c; tests/differential/rsa_openssl_differential_tests.c.

- [x] Reproduce baseline entropy-OFF failures and KAT false success.
- [x] Use a regression harness invoking the actual KAT helper; prove it fails before fixing the helper, then integrate the assertions into the existing KAT executable to preserve evidence test numbering.
- [x] Make every helper API error return boolean failure; preserve main's exit-code convention.
- [x] Provide explicit supported-provider expectations through private test compile definitions.
- [x] Assert known-vector verification and exact signing success/failure plus output clearing in each configuration.
- [x] Cover forced and partial entropy failures using existing Debug-only hooks.
- [x] Run focused RSA tests with entropy ON, OFF, and failure hooks.

### Task 2: CI and final validation

Files: .github/workflows/ci.yml; docs/security/RSA_TEST_ENTROPY_CI_VALIDATION.md.

- [x] Enable real OS entropy in required full differential CI; retain provider-disabled coverage and add RSA failure injection to the hooks lane.
- [x] Run full Release suites with entropy ON/OFF and OpenSSL; ASan/UBSan; lock-disabled; optional/required unavailable OpenSSL configuration; native 32-bit if available.
- [x] Run production exported-symbol baseline, installed compatibility, RSA policy, and unchanged public-surface policy gate.
- [x] Review the complete diff and record exact results, limitations, and release blockers.
- [ ] Commit, push the requested branch, create and attach a separate Draft PR, and inspect hosted CI (publication status in the task report).
