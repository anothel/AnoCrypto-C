# Phase 2 initialization/shutdown race hardening — Issue #8

Baseline: `548e903d5cb506c2ad1d18bf205fce316e28d128` (main).
Branch: `fix/module-init-shutdown-race`.
External release: `EXTERNAL_RELEASE_NOT_AUTHORIZED`.
Independent security approval: outstanding.

## Reproduction and fix

A deterministic native-thread fixture compiles the actual module state machine
and lock backend. It pauses startup work at its dependency boundary using a
separate mutex/condition variable, lets shutdown finish, and then resumes the
old init. Startup work and cleanup are controlled fixture dependencies; there
are no sleep-based scheduling assumptions and no production test hooks.

Against the original main source, each schedule below exited 1 with
`old init result=ANO_CRYPTO_OK`, violating the expected `NOT_INITIALIZED` status:

1. Old init paused; shutdown completed; old init resumed.
2. Old init paused; shutdown; new init paused; old init resumed before new init.
3. Old init paused; shutdown; new init completed; old init resumed afterward.

A mutation retaining only a final `SELF_TEST_RUNNING` check still failed case 2.
The corrected code captures a unique uint64 startup generation under the global
lock and requires matching generation and state before publishing success.
Saturation refuses new startup with existing `RESOURCE_EXHAUSTED`, avoiding ABA
through wraparound. Cancellation returns existing `NOT_INITIALIZED` and is not
fatal. Mandatory failures, including those from obsolete startup, remain fatal.

Additional schedules retain an externally latched fatal state while old init
resumes, and a mandatory failure from old init after a newer init succeeds.
Successful cancellation is followed by a fresh successful startup. The
thread-safety-OFF and unavailable-Threads builds exercise only serialized startup/shutdown/reinit.

## Verification commands

Use CMake 3.16+ and a C99 compiler; this session used GCC 14.2.0 and CMake 3.31.6
on Linux x86_64. The local CMake tools live in /workspace/.anocrypto-tools/bin.

```sh
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel 4
ctest --test-dir build-debug --output-on-failure --timeout 180 -j 4
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target ano_crypto_collect_release_evidence --parallel 4
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_SANITIZER_MODE=address-undefined
cmake --build build-asan --parallel 4
ctest --test-dir build-asan -R 'module_state|csp_|thread_|rsa_thread|ecdsa_p256_thread|service_gate' --output-on-failure --timeout 180 -j 2
cmake -S . -B build-off -DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF
cmake --build build-off --target ano_crypto_module_state_tests ano_crypto_csp_shutdown_zeroization_tests ano_crypto_csp_fatal_zeroization_tests
ctest --test-dir build-off -R 'module_state_tests|csp_(shutdown|fatal)_zeroization_tests' --output-on-failure
```

For a direct ThreadSanitizer fixture build:

```sh
cc -std=c99 -Wall -Wextra -Wpedantic -pthread -fsanitize=thread -g \
  -DANO_CRYPTO_ENABLE_THREAD_SAFETY=1 -Iinclude -Isrc/internal \
  tests/unit/module_init_shutdown_race_tests.c src/core/module_state.c \
  src/core/thread_lock.c src/core/status.c -o module-race-tsan
for scenario in 0 1 2 3 4; do ./module-race-tsan "$scenario" || exit; done
```

The same direct fixture build with original main's module_state.c reproduces
cases 0, 1 and 2 as assertion failures. A version with only the state check also
fails scenario 1, demonstrating why the generation comparison is necessary.

## Evidence and limits

Local validation: Debug full suite 102/102; focused ASan+UBSan 17/17
outside the ptrace sandbox with LeakSanitizer enabled; all five deterministic
schedules under ThreadSanitizer; Release thread-safety-OFF focused tests 3/3;
clean hidden-symbol Release ABI 85/85, public-surface and release compatibility
policy checks passed. Public-surface policy retains one existing tracked TODO.
The Release CTest evidence target covered all 102 test indices. The first full
evidence assembly correctly refused a dirty source tree; assembly then passed
against the clean initial fix commit. Final follow-up commit and hosted results
are recorded in the PR/final handoff.
The existing module-state CTest registration remains index 1, and all 102
release-evidence test registrations and ten evidence ranges remain intact. The
fixture is a build dependency of that original target. It is not installed and
adds no library symbols. Existing library-backed module/CSP tests are retained.
No workflows, production algorithm implementations or public headers changed.

Review the standalone state-machine dependency fixture together with real KAT,
CSP zeroization and service-gating evidence; it does not itself prove secret
byte erasure. A first local ASan run was blocked at LeakSanitizer teardown by the
ptrace sandbox; the rerun outside that sandbox passed with checks enabled.
The lifecycle generation saturation path is verified by source inspection, not
2^64 actual startup cycles. Windows/macOS native scheduling must be verified by
hosted CI. Emergency lock-failure cleanup and broad operation/shutdown barriers
remain separate review subjects. No independent security approval is claimed.

Automated code review identified fixture/backend drift when CMake Threads was
unavailable. A configuration with CMAKE_DISABLE_FIND_PACKAGE_Threads=TRUE
reproduced the fixture incorrectly claiming concurrent coverage while the
library selected the no-op fallback. The fixture now mirrors the library's
backend selection; that configuration must report concurrent schedules not run.
This automated review is not independent human security approval.
