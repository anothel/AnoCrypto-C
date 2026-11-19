# Thread-Safety CI Checklist

Status: P1-21 draft.

## Purpose

This checklist records the CI lanes and local commands needed to keep the P1-21 thread-safety support matrix aligned with source code, tests, and evidence. It is CI plumbing and engineering evidence only.

## Required local checks

Run these after any change to `src/core/thread_lock.c`, `src/internal/ac_thread_internal.h`, module state, self-test state, CSP/key handle code, or public concurrency wording.

```sh
cmake -S . -B build \
  -DANO_CRYPTO_BUILD_TESTS=ON \
  -DANO_CRYPTO_BUILD_EXAMPLES=ON \
  -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_thread_safety_policy_check
cmake --build build --target ano_crypto_concurrency_sanitizer_policy_check
cmake --build build --target ano_crypto_public_surface_check
cmake --build build --target ano_crypto_poc_package_smoke
cmake --build build --target ano_crypto_collect_release_evidence
```

Also verify the explicit no-thread-safety configuration remains buildable:

```sh
cmake -S . -B build-no-thread-safety \
  -DANO_CRYPTO_BUILD_TESTS=ON \
  -DANO_CRYPTO_BUILD_EXAMPLES=ON \
  -DANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF
cmake --build build-no-thread-safety --parallel
ctest --test-dir build-no-thread-safety --output-on-failure
cmake --build build-no-thread-safety --target ano_crypto_thread_safety_policy_check
```

The OFF lane covers serialized use. Callers must serialize shared state when the native lock is disabled or unavailable.

`ano_crypto_module_state_tests` runs library-backed sequential lifecycle tests
in both configurations. With the native backend it also runs the deterministic
Issue #8 startup cancellation fixture. With thread safety OFF or the lock
backend unavailable, the fixture uses its sequential fallback. The
`ano_crypto_csp_shutdown_zeroization_tests` and
`ano_crypto_csp_fatal_zeroization_tests` entries cover slot cleanup and rejected
admission after the respective transitions. See the
[current lifecycle contract](../design/C99_THREAD_SAFETY_POLICY.md#phase-2-module-initshutdown-contract-issue-8).

## Hosted CI starter lanes

| Lane | OS | Thread safety | Required target/check | Notes |
|---|---|---:|---|---|
| Default Linux | Ubuntu | ON | Build, CTest, policy checks, install smoke, PoC smoke | Primary CI lane for current concurrent CSP/HMAC smoke. |
| Default macOS | macOS | ON | Build, CTest, policy checks, install smoke | Confirms Apple platform branch builds and basic tests pass. |
| Default Windows | Windows | ON | Build, CTest, policy checks, install smoke, native concurrency test | Upload status-only thread-platform artifact; actual result review is required. |
| Linux explicit OFF | Ubuntu | OFF | Build, CTest, thread-safety policy check | Confirms unsupported concurrency configuration remains documented and buildable. |
| Linux evidence draft | Ubuntu | ON | `ano_crypto_collect_release_evidence` | Captures thread-safety, concurrency/sanitizer, ABI, and status-only platform records. |
| Linux GCC sanitizer | Ubuntu | ON | Full CTest with ASan/UBSan | Required P1-21 diagnostic lane; no secret-bearing fixtures or output. |
| Release-style symbol check | Ubuntu | ON | Build with tests OFF and symbol hiding ON | Confirms no `ano_crypto_internal_*` exports in the public-style shared library. |

## Log review

- CTest logs may include status names and test names only.
- Do not print key bytes, entropy bytes, nonce bytes, MAC output, generated DRBG bytes, private keys, tokens, customer data, or internal deployment details.
- For platform entropy tests, record provider/status only.
- For thread tests, record pass/fail and iteration counts only.

## Failure triage

| Symptom | First review point |
|---|---|
| CSP concurrent smoke fails | Check global lock branch, CSP slot lifecycle, handle generation, and snapshot rule. |
| OFF lane unexpectedly runs concurrent CSP/HMAC smoke | Check CMake guard around `ano_crypto_csp_thread_safety_tests`. |
| Public-surface check reports internal exports | Check test hooks and `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS` configuration. |
| Windows concurrency or platform artifact fails | Keep Windows support wording limited and review native thread/lock backend logs. |
| Evidence lacks thread-safety record | Check `tools/collect_release_evidence.cmake` generated `THREAD_SAFETY_CHECK.txt`. |

## TODO

1. Review and retain actual hosted Linux/macOS/Windows artifacts after successful workflow runs.
2. Review Clang sanitizer runtime and ThreadSanitizer before adding them as required lanes.
3. Keep release-style symbol and build-identity ABI checks in release-candidate review.
