# CSP lifecycle PR 1: hosted CI failure classification

Compared base `b3d5e6551b2bcc38b39602592476146e4b81b9b0` with
PR implementation `f3be28d7e7c15d98a6c465f07557dd8ed4afc4de`.
Investigation date: 2026-11-16. No runtime, workflow, assertion, public API/ABI,
release-status or certification-claim change is made by this classification.
PR 1 remains Draft; these findings do not authorize merge or bypass a check.

## Hosted evidence

Both push run `43787295311` and PR run `43787343129` show the same seven failed
jobs. The PR run tests synthetic merge `0ebeeeea09e254b37d99066579972f62974ab3d0`
into the specified base. Logs were read for every failed PR job. Run/job links
use `https://github.com/anothel/AnoCrypto-C/actions/runs/43787343129/job/ID`.

| Job ID | Failed job / exact step | Failure point | Classification |
| --- | --- | --- | --- |
| 133037821260 | Linux Debug locks ON / Public surface check | check_public_surface.cmake:220 | Pre-existing |
| 133037821386 | Linux Debug locks OFF / Public surface check | same gate | Pre-existing |
| 133037821323 | macOS Debug / Public surface check | same gate | Pre-existing |
| 133037821434 | Windows Debug / Public surface check | same gate | Pre-existing |
| 133037821456 | Linux 32-bit / Run 32-bit test suite | rsa_api_negative_tests.c:23 and rsa_thread_safety_tests | Pre-existing entropy/configuration mismatch |
| 133037821454 | Linux provider disabled / Run provider-disabled tests | same two tests | Pre-existing entropy/configuration mismatch |
| 133037821383 | Linux OpenSSL differential / Run status-only differential validation | rsa_openssl_differential_tests.c:89 | Pre-existing entropy/configuration mismatch |

The 32-bit and disabled-provider lanes each report 89/91 passing tests. The
OpenSSL lane reports 9/10 differential tests passing. There is no observed
RSA numeric mismatch, OpenSSL signing failure, or compiler arithmetic failure
in these logs. The failed differential assertion is AnoCrypto's signing call,
before the OpenSSL verify/sign/comparison assertions.

At the investigation snapshot, hosted ASan/UBSan, s390x selected portability,
random lifecycle, all three production export lanes and all three installed
compatibility lanes passed. GCC compiler capture was still running in manifest
verification (job 133037821284); this is pending, not a diagnosed failure.
Downstream evidence/index jobs cannot establish success while prerequisites fail.

## Causes and source evidence

Public-surface gate: its literal substring scan (tools/check_public_surface.cmake:
28-57, 98-104) flags two unchanged documents:
`docs/product/ANOPKI_REQUIREMENT_TRACE.md:45`, which explicitly denies a release
assurance claim, and `docs/roadmap/P3_PRODUCT_DRIVEN_ROADMAP.md:34`, describing
historical internal review. Base and PR generate exactly the same two violation
paths. Test builds' additional internal-export warnings are not the fatal cause;
production export checks pass. The CSP patch adds one private predicate, so its
extra internal-symbol warning in test builds is expected, not a public ABI change.

RSA: all three affected workflow lanes explicitly set
`ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF` (ci.yml:353, 255, 754).
`src/signature/signature.c:150` requests eight bytes of platform entropy for
private-operation blinding. Unavailable entropy returns NOT_IMPLEMENTED
(`src/drbg/entropy_adapter.c:373-375`) before key lookup at signature.c:156 or
private RSA arithmetic. The outer signing path clears output and leaves
out_written zero. Blinding must not be disabled or replaced with fixed bytes.

- Negative test line 23 signs with a public handle and expects
  UNSUPPORTED_PARAMETER. Entropy is requested before key-role validation, so the
  unavailable provider masks that later error with NOT_IMPLEMENTED.
- The concurrency worker (rsa_thread_safety_tests.c:104-114) requires successful
  signing. Every ordinary worker instead receives NOT_IMPLEMENTED. The main
  function returns 1 after joining failed workers, before its destroy-race phase.
- Differential line 89 likewise requires successful signing with a private key;
  it receives NOT_IMPLEMENTED. This does not implicate the OpenSSL provider.

The relevant RSA implementation, tests, workflow and policy/doc files have no
diff between base and PR. A 64-bit local reproduction yields the same failure
set and exact status, so word size is not required to trigger these failures.
Native 32-bit local execution is unavailable: `gcc -m32` cannot link because
Scrt1.o, crti.o and 32-bit libgcc are absent. Actual hosted 32-bit logs provide
the platform evidence; broader 32-bit correctness remains a separate question.

## Reproduction commands/results

Local environment: Linux x86_64, GCC 14.2.0, CMake 3.31.6, OpenSSL 3.5.7.
Hosted affected Linux jobs: GCC 15.2.0; differential reference OpenSSL 3.5.5.
Artifacts are under /tmp. Existing source and tests were not modified.

```sh
export PATH=/workspace/.anocrypto-tools/bin:$PATH
git clone --shared --quiet /workspace/AnoCrypto-C /tmp/csp-ci-baseline
git -C /tmp/csp-ci-baseline checkout --quiet b3d5e6551b2bcc38b39602592476146e4b81b9b0
cmake -S /tmp/csp-ci-baseline -B /tmp/csp-ci-base-off -DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_BUILD_EXAMPLES=OFF -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-ci-base-off -j 4
ctest --test-dir /tmp/csp-ci-base-off --output-on-failure --timeout 180 -j 4
cmake -S . -B /tmp/csp-ci-pr-off -DCMAKE_BUILD_TYPE=Debug -DANO_CRYPTO_BUILD_EXAMPLES=OFF -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-ci-pr-off -j 4
ctest --test-dir /tmp/csp-ci-pr-off --output-on-failure --timeout 180 -j 4
```

Both builds succeed; both suites fail exactly the same three RSA tests:
98/101 pass. Logs: /tmp/csp-ci-{base,pr}-{config,build,tests}.log.

Reconfigure each existing build with differential tests OFF:

```sh
cmake -S /tmp/csp-ci-baseline -B /tmp/csp-ci-base-off -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=OFF
cmake --build /tmp/csp-ci-base-off -j 4
ctest --test-dir /tmp/csp-ci-base-off --output-on-failure --timeout 180 -j 4
cmake -S . -B /tmp/csp-ci-pr-off -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=OFF
cmake --build /tmp/csp-ci-pr-off -j 4
ctest --test-dir /tmp/csp-ci-pr-off --output-on-failure --timeout 180 -j 4
```

Both report 89/91 passes, failing negative/concurrency exactly as hosted.
Logs: /tmp/csp-ci-{base,pr}-off-{config,build,tests}.log.

Positive control: change only the entropy setting from the original differential
configuration, on both revisions:

```sh
cmake -S /tmp/csp-ci-baseline -B /tmp/csp-ci-base-off -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-ci-base-off -j 4
ctest --test-dir /tmp/csp-ci-base-off -R 'rsa_(api_negative|thread_safety|openssl_differential)_tests|csp_(shutdown|fatal)_zeroization_tests' --output-on-failure --timeout 180
cmake -S . -B /tmp/csp-ci-pr-off -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=ON -DANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON
cmake --build /tmp/csp-ci-pr-off -j 4
ctest --test-dir /tmp/csp-ci-pr-off -R 'rsa_(api_negative|thread_safety|openssl_differential)_tests|csp_(shutdown|fatal)_zeroization_tests' --output-on-failure --timeout 180
```

Both pass 5/5; logs /tmp/csp-ci-{base,pr}-on-{config,build,tests}.log.
A synthetic-fixture status probe (/tmp/csp-ci-rsa-status.c) on both revisions
confirms provider OFF gives NOT_IMPLEMENTED for public/private signing, cleared
output and zero bytes written; fixture verification still succeeds. Provider ON
gives UNSUPPORTED_PARAMETER for the public handle and successful 256-byte private
signing. No keys, signatures or entropy bytes were printed.

```sh
cmake -DANO_CRYPTO_SOURCE_DIR=/tmp/csp-ci-baseline -DANO_CRYPTO_BUILD_DIR=/tmp/csp-ci-base-off -P /tmp/csp-ci-baseline/tools/check_public_surface.cmake
cmake -DANO_CRYPTO_SOURCE_DIR=/workspace/AnoCrypto-C -DANO_CRYPTO_BUILD_DIR=/tmp/csp-ci-pr-off -P tools/check_public_surface.cmake
```

Both fail identically on the same two document paths (policy logs under
/tmp/csp-ci-{base,pr}-policy.log). No policy was loosened to obtain a pass.

## Separate follow-up PRs

1. Repair the two document/policy conflicts without allowing unsupported claims
   or disabling the gate. Keep this separate from CSP lifecycle locking.
2. Make RSA capability/configuration tests explicit: enabled-provider lanes must
   exercise successful signing/blinding; disabled-provider lanes must assert
   rejection, zero written length and complete output clearing. Resolve and test
   key-role versus entropy-error precedence. Keep 32-bit signing coverage with an
   actual entropy provider, and retain a true provider-disabled lane. Do not skip
   assertions, bless generic errors, or remove blinding.
3. Fix existing RSA KAT error propagation. `CHECK` returns 1 on error; inside
   `run_case()` that is also success. With the provider OFF, direct execution
   prints four signing failures at line 25 and then exits 0 with a success line.
   This is reproducible on base and PR. Correct propagation and independently
   test the test's failure exit; do not mistake its current pass for evidence
   that RSA signing works without entropy.

No PR regression was found among the completed failed checks. Accordingly no
unrelated RSA, workflow or policy fix is folded into the CSP patch. Merge remains
blocked by the failures, pending jobs and required review.
