# CSP lifecycle admission fix validation

Base: `b3d5e6551b2bcc38b39602592476146e4b81b9b0`.
Branch: `fix/csp-lifecycle-admission-race`.
Environment: Linux x86_64, GCC 14.2.0, CMake 3.31.6, OpenSSL 3.5.7.

## Boundary and compatibility

Final secret, RSA and P-256 key admission now checks module state while holding
exactly the same global lock as slot insertion and shutdown/fatal zeroization.
The internal state predicate never locks. Rejection precedes allocation,
generation changes and tombstone mutation. Public early state checks remain;
RSA/P-256 preparation and unconditional prepared-key cleanup remain outside the
critical section. No public headers, exports, status codes or handle layouts
change. Secret import additionally invalidates a non-null output handle before
its early gate, matching asymmetric import rejection behavior.

The existing shutdown/fatal tests stage both orderings at the split-phase
admission boundary: successful public imports before the transition, and
initial state checks/prepared keys followed by the transition before internal
insertion. They cover HMAC, block-cipher, CMAC, public/private RSA and
public/private P-256. Rejected outputs, ZEROIZED tombstones and explicit
DESTROYED tombstones are checked. No timing sleeps or shipped test hooks are
needed. Existing test registrations/counts remain unchanged.

## Commands and results

Commands below ran from the repository root. Build artifacts/logs are temporary.

```sh
export PATH=/workspace/.anocrypto-tools/bin:$PATH
ctest --test-dir /tmp/anocrypto-audit-release --output-on-failure --timeout 180 -j 4
```

Before edits: 102/102 baseline tests passed (`/tmp/csp-baseline-release.log`).
After adding the regression tests, before the implementation change:

```sh
cmake --build /tmp/anocrypto-audit-release -j 4
ctest --test-dir /tmp/anocrypto-audit-release -R 'csp_(shutdown|fatal)_zeroization' --output-on-failure --timeout 30
```

Build succeeded; both tests failed as expected: final admission returned OK
instead of NOT_INITIALIZED/FATAL_STATE for all seven cases, with live returned
handles and tombstone damage (`/tmp/csp-red-test.log`). After the fix:

```sh
cmake --build /tmp/anocrypto-audit-release -j 4
ctest --test-dir /tmp/anocrypto-audit-release -R 'csp_|module_state|rsa_|ecdsa_p256' --output-on-failure --timeout 180 -j 4
```

22/22 focused tests passed (`/tmp/csp-focused-test.log`). Fresh full Release:

```sh
cmake -S . -B /tmp/csp-fix-release -DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-fix-release -j 4
ctest --test-dir /tmp/csp-fix-release --output-on-failure --timeout 180 -j 4
```

102/102 tests passed; configure/build succeeded without compiler diagnostics.
Logs: `/tmp/csp-release-{configure,build,test}.log`.

```sh
cmake -S . -B /tmp/csp-fix-asan -DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_SANITIZER_MODE=address-undefined -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=ON -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-fix-asan -j 4
ASAN_OPTIONS=detect_leaks=0 ctest --test-dir /tmp/csp-fix-asan --output-on-failure --timeout 180 -j 2
```

105/105 tests passed; no ASan/UBSan diagnostics. Leak detection is disabled
because this environment's LeakSanitizer previously failed under its tracing
runtime; leak checking is not established. Logs:
`/tmp/csp-asan-{configure,build,test}.log`.

```sh
cmake -S . -B /tmp/csp-fix-nolocks -DCMAKE_BUILD_TYPE=Release -DANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF
cmake --build /tmp/csp-fix-nolocks -j 4
ctest --test-dir /tmp/csp-fix-nolocks -R 'csp_|module_state' --output-on-failure --timeout 180 -j 4
```

11/11 tests passed. This mode still requires caller serialization.
Logs: `/tmp/csp-nolocks-{configure,build,test}.log`.

```sh
cmake --build /tmp/csp-fix-release --target ano_crypto_clean_release_symbol_baseline_check ano_crypto_install_package_smoke ano_crypto_public_surface_check -j 2
git diff --check
```

Independent clean production Release 85-symbol baseline and installed/current/
old-header compatibility checks passed. The combined target command exited 2:
the pre-existing public-surface policy gate still rejects wording in
`docs/product/ANOPKI_REQUIREMENT_TRACE.md` and
`docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md`. The tests-enabled library also
intentionally exposes internal symbols. No check or gate was weakened.
Log: `/tmp/csp-package-policy.log`. Diff whitespace check passed.

A supplemental Linux executable-interposition harness paused real public calls
immediately after the real initial state gate, completed shutdown/fatal, and
resumed import. All ten runs (secret and public/private RSA/P-256, each
transition) returned the expected rejection with invalid handles and no live
returned key. Source `/tmp/csp-public-race.c`; output `/tmp/csp-public-race.log`.

```sh
gcc -std=c99 -I include -I src/internal /tmp/csp-public-race.c -L/tmp/csp-fix-release -Wl,-rpath,/tmp/csp-fix-release -Wl,--export-dynamic -lanocrypto_c -ldl -pthread -o /tmp/csp-public-race
# Each invocation ran with a 10-second subprocess timeout.
for kind in 0 1 2 3 4; do /tmp/csp-public-race "$kind"; /tmp/csp-public-race "$kind" fatal; done
```

## Remaining limits

The lock-failure emergency fatal path, unsupported/no-op locking, caller-owned
operation contexts, and unrelated cryptographic findings are not redesigned.
No Windows/macOS, TSan, or leak-checking result is claimed. Hosted CI/review
remain required before merge. Release-blocked, unsigned and validation status
are unchanged. Independent read-only candidate review found no concrete
admission bypass or compatibility regression.
