# Thread-Safety CI Checklist

Status: P1-14 draft.

## Purpose

This checklist records the CI lanes and local commands needed to keep the P1-14 thread-safety support matrix aligned with source code, tests, and evidence. It is CI plumbing and engineering evidence only.

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

The OFF lane is not a concurrency-support lane. It confirms that the build configuration remains explicit and that callers must serialize shared state in that mode.

## Hosted CI starter lanes

| Lane | OS | Thread safety | Required target/check | Notes |
|---|---|---:|---|---|
| Default Linux | Ubuntu | ON | Build, CTest, policy checks, install smoke, PoC smoke | Primary CI lane for current concurrent CSP/HMAC smoke. |
| Default macOS | macOS | ON | Build, CTest, policy checks, install smoke | Confirms Apple platform branch builds and basic tests pass. |
| Default Windows | Windows | ON | Build, CTest, policy checks, install smoke | Confirms Windows lock branch compiles and basic global-state paths run. |
| Linux explicit OFF | Ubuntu | OFF | Build, CTest, thread-safety policy check | Confirms unsupported concurrency configuration remains documented and buildable. |
| Linux evidence draft | Ubuntu | ON | `ano_crypto_collect_release_evidence` | Captures `THREAD_SAFETY_CHECK.txt` with evidence outputs. |
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
| Windows lane builds but lacks concurrent stress coverage | Keep Windows support wording limited until a Windows-specific test is added. |
| Evidence lacks thread-safety record | Check `tools/collect_release_evidence.cmake` generated `THREAD_SAFETY_CHECK.txt`. |

## TODO

1. Add hosted CI artifact retention rules only after log redaction policy is reviewed.
2. Add a Windows-specific concurrent CSP/HMAC test.
3. Add compiler sanitizer lanes after compiler/toolchain policy is selected.
4. Add a release-style symbol check job before release-candidate review.
