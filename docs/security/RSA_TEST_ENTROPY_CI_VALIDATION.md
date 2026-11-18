# RSA test reliability and entropy CI validation

Date: 2026-11-18. Base: `main`, `b3d5e6551b2bcc38b39602592476146e4b81b9b0`.
Branch: `fix/rsa-test-entropy-ci-v2`.

## Scope and security boundary

The unavailable local commit `fc76cde50fd84485a8075f3a363b4bfb085ea051` was not present in this checkout or replayed. Draft PR #1 was inspected read-only; its CSP lifecycle implementation is not included here.

The KAT helper used an exit-code macro that returned 1 on an API error, while its caller interpreted 1 as success. Explicit boolean failure returns now propagate signing and verification errors. Regression assertions invoke that same helper with invalid handles and, in dedicated Debug builds, failed and partially failed entropy reads.

RSA test expectations are private compile definitions derived from the configured provider and supported platform, rather than a runtime probe. Unexpected entropy failures in enabled builds remain failures. Disabled builds require `NOT_IMPLEMENTED`, zero bytes written, and full output-capacity clearing, while retaining known-vector verification and size-query coverage. Nonzero output-length sentinels and oversized failure buffers protect these assertions.

The RSA thread test uses an event on Windows and a mutex/condition variable on POSIX instead of a volatile notification. Enabled runs retain successful concurrent signing, exact signature comparison, verification, and sign/destroy outcomes. Disabled runs check concurrent verification and fail-closed signing during destruction. Disabled signing returns before the private-key lock; only enabled runs exercise private signing against destruction. A race run permits either valid scheduler ordering and does not prove that both occurred.

The required OpenSSL differential CI lane enables real OS entropy. Provider-disabled and 32-bit lanes retain entropy OFF. The existing dedicated entropy-failure-injection label now also selects the RSA KAT, including all three SHA-2 signing failure cases. No checks were removed, skipped, made optional, or given success overrides.

Production sources, public headers, ABI baselines, blinding, fault checks, status precedence, and release authorization policies are unchanged. **External release remains blocked.** This change does not supply release approval or resolve PR #1.

## Reproduction and regression evidence

- Before changes, entropy OFF plus OpenSSL reproduced failures in `ano_crypto_rsa_api_negative_tests`, `ano_crypto_rsa_thread_safety_tests`, and `ano_crypto_rsa_openssl_differential_tests`. The KAT incorrectly passed.
- A temporary C harness included the actual original KAT helper and called it with an invalid private handle. It exited 1 with `RSA KAT swallowed an API error`, after the helper reported `NOT_IMPLEMENTED`. Those regression assertions now run inside the existing KAT executable, preserving all CTest/evidence numbering.
- Two temporary Linux interposition probes against the final entropy-OFF KAT simulated leaving the length unchanged and leaving byte 256 uncleared. Both caused a nonzero KAT exit. These probes were not added to production or CI.

## Local environment and exact commands

Linux x86_64; GCC 14.2.0; CMake/CTest 3.31.6; OpenSSL 3.5.7.
Use `export PATH=/workspace/.anocrypto-tools/bin:$PATH` in this environment.
All test configurations use the default tests/examples/strict-warnings settings (ON) unless overridden below. Build with `cmake --build <directory> --parallel 4` after configuration.

| Configuration | Configure options after `cmake -S . -B <directory>` | Test command | Final result |
| --- | --- | --- | --- |
| `build-rsa-release` | `-DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON` | `ctest --test-dir build-rsa-release --output-on-failure -j 2` | 102/102 |
| `build-rsa-before-off` (reconfigured with final source) | Same as Release, with `-DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF` | `ctest --test-dir build-rsa-before-off --output-on-failure -j 2` | 102/102 |
| `build-rsa-asan` | `-DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=ON -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=ON -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=OFF -DANO_CRYPTO_SANITIZER_MODE=address-undefined` | `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir build-rsa-asan --output-on-failure -j 2` | 105/105; leak detection enabled |
| `build-rsa-hooks-off` | `-DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=ON -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=ON -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=OFF` | `ctest --test-dir build-rsa-hooks-off -L entropy-failure-injection --output-on-failure` | 4/4, including RSA forced/partial failures |
| `build-rsa-lock-off` | `-DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF` | `ctest --test-dir build-rsa-lock-off -R '^ano_crypto_(rsa_|csp_|current_header|p129_header|build_identity)' --output-on-failure` | 12/12 under caller serialization |
| `build-rsa-openssl-unavailable` | `-DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=OFF -DCMAKE_DISABLE_FIND_PACKAGE_OpenSSL=ON` | `ctest --test-dir build-rsa-openssl-unavailable --output-on-failure -j 2` | 92/92; optional provider absent |

Required-provider rejection also passed: configuration with `-DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON -DCMAKE_DISABLE_FIND_PACKAGE_OpenSSL=ON` failed with the expected missing-OpenSSL diagnostic.

Supplemental commands:

```sh
ctest --test-dir build-rsa-release -R '^ano_crypto_rsa_openssl_differential_tests$' --verbose
ctest --test-dir build-rsa-before-off -R '^ano_crypto_rsa_openssl_differential_tests$' --verbose
ctest --test-dir build-rsa-release -R '^ano_crypto_rsa_thread_safety_tests$' --repeat until-fail:20 --output-on-failure
ctest --test-dir build-rsa-before-off -R '^ano_crypto_rsa_thread_safety_tests$' --repeat until-fail:20 --output-on-failure
```

Both 20-run race repetitions passed. Entropy ON completed 36 differential comparisons across SHA-256/384/512 and four PKI messages: OpenSSL verifies AnoCrypto signatures, signatures match exactly, and AnoCrypto verifies OpenSSL signatures. Entropy OFF completed 12 verification-only comparisons and 12 exact fail-closed signing assertions; it does not claim working private signing. OpenSSL also verifies each reference signature. The existing release-evidence parser still requires the 36-comparison result.

## API/ABI and policy checks

```sh
cmake -S . -B build-rsa-production -DCMAKE_BUILD_TYPE=Release \
  -DANO_CRYPTO_BUILD_TESTS=OFF -DANO_CRYPTO_BUILD_EXAMPLES=OFF \
  -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF \
  -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF \
  -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
cmake --build build-rsa-production --parallel 4 --target \
  ano_crypto_exported_symbol_baseline_check \
  ano_crypto_release_compatibility_policy_check ano_crypto_rsa_pkcs1_v15_policy_check
cmake --build build-rsa-release --target \
  ano_crypto_install_package_smoke ano_crypto_tagged_release_compatibility_smoke
cmake --build build-rsa-release --target \
  ano_crypto_random_service_policy_check ano_crypto_random_lifecycle_policy_check \
  ano_crypto_entropy_operational_policy_check \
  ano_crypto_differential_portability_policy_check ano_crypto_concurrency_sanitizer_policy_check
```

All passed: clean shared-library 85-symbol baseline, 85 public declarations, installed-package/PoC/current-header/retained P1-29-header compatibility, and the listed security policies. `git diff --check` passed.

An independent read-only review found two assertion gaps (zero length initialization and signature-sized failure buffers); both were corrected and the full suites rerun. Minor deferred diagnostic issue: intentional invalid-handle probes emit the helper's existing `RSA KAT failure` text even when the regression test passes. CTest uses the exit code; these expected diagnostics do not indicate a failing test.

## Remaining blockers and hosted status

`cmake --build build-rsa-production --target ano_crypto_public_surface_check` still fails on blocked wording in the unchanged `docs/product/ANOPKI_REQUIREMENT_TRACE.md` and `docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md`. This is the same pre-existing blocker recorded in Draft PR #1. The gate remains blocking and was not weakened.

A native `cc -m32` link probe failed because `Scrt1.o`, `crti.o`, and 32-bit libgcc are absent. Native 32-bit, Windows, macOS, and s390x results must come from hosted CI. Local results do not establish those platform results.

Hosted checks are pending at initial publication. The new Draft PR and task report carry the current run links and exact job status. Required hosted checks, review, the public-surface gate, and existing release authorization requirements remain blocking. Do not merge or externally release on the basis of this validation record.
