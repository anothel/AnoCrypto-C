# Concurrency and Sanitizer CI Checklist

Status: P1-29 retained engineering checklist.

## Selected lanes

- Linux GCC Debug with `ANO_CRYPTO_SANITIZER_MODE=address-undefined` is the required P1-21 sanitizer lane.
- Clang supports the same CMake mode, but its hosted runtime is a follow-up review item rather than a required P1-21 lane.
- Linux, macOS, and Windows normal thread-safety-ON lanes run the cross-platform CSP/HMAC/AES/CMAC concurrency test when the reviewed backend is available.
- A Linux thread-safety-OFF lane confirms that the build and status artifact remain explicit about caller serialization.

AddressSanitizer and UndefinedBehaviorSanitizer are selected because they can run against the complete current CTest set without requiring production secrets or special fixtures. ThreadSanitizer is not enabled in P1-21; toolchain/runtime compatibility and false-positive handling remain TODO.

## Required checks

1. Configure fails for an unknown sanitizer mode.
2. Sanitizer flags are accepted only by the reviewed GCC/Clang-family branch.
3. Full CTest passes in the required GCC address/undefined lane.
4. `ano_crypto_csp_thread_safety_tests` runs on Windows using native thread APIs and on POSIX-like systems using pthreads.
5. `ano_crypto_thread_platform_review` records backend name, availability, concurrency-test presence, and sanitizer mode only.
6. Logs contain no key bytes, MAC bytes, entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data.
7. Clean Release and PoC builds use sanitizer mode `none` unless a separately reviewed diagnostic package is requested.
8. The public exported-symbol baseline remains unchanged.

## Hosted artifact review

Workflow configuration is not proof that hosted jobs passed. Download and review each `hosted-thread-platform-review-*` artifact and the sanitizer job logs. Record OS, compiler, backend, availability, test result, and unresolved limitation without copying sensitive material.
