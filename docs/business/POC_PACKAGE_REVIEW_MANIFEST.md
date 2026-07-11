# Customer PoC Package Review Manifest

Status: P1-27 draft.

## Purpose

This document defines the review intent for the AnoCrypto-C customer PoC package v0. The package is a customer integration draft for C ABI, installed CMake package lookup, module lifecycle, self-test startup behavior, and SHA-256/SHA-384/SHA-512, their HMAC variants, HMAC_DRBG-SHA-256, and public random-service smoke flows.

## Required package records

The PoC package should contain:

- `README.md`
- `PACKAGE_MANIFEST.md`
- `PACKAGE_REVIEW_MANIFEST.md`
- `SBOM_STATUS.txt`
- `SIGNING_STATUS.txt`
- `THREAD_SAFETY_PACKAGE_NOTES.txt`
- `CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt`
- `SHA512_FAMILY_PACKAGE_NOTES.txt`
- `CMAC_ENTRY_GATE_PACKAGE_NOTES.txt`
- `RANDOM_SERVICE_PACKAGE_NOTES.txt`
- `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt`
- `ENTROPY_HEALTH_PACKAGE_NOTES.txt`
- `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt`
- `CLEAN_BUILD_CONFIGURE.log`
- `CLEAN_BUILD.log`
- `CLEAN_RELEASE_SYMBOL_BASELINE.log`
- `INSTALL_PACKAGE.log`
- `sdk/`
- `examples/core_poc/`
- `docs/`
- `evidence-template/`

## Review before external sharing

Confirm:

- the package manifest includes only package-relative paths
- install logs do not disclose internal deployment details that should be removed
- examples do not print key bytes, entropy bytes, seed material, nonce material, DRBG state, generated DRBG output, private keys, or tokens
- docs do not imply official validation evidence or external review completion
- SBOM status is clearly marked as placeholder unless an actual SBOM is generated later
- signing status is clearly marked as not performed unless detached signature files are added later
- thread-safety package notes match `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` and current build options
- clean SDK logs record tests and both internal hook options off and internal symbol hiding on
- installed headers and shared-library exports do not expose internal entropy, process-guard, reseed-limit, or failure-injection hook symbols
- entropy/health package notes do not imply finalized source-specific health tests
- random-service and lifecycle package notes match the P1-20 process-marker, VM duplication, suspend/resume, and output non-disclosure limits
- package contents match the intended customer PoC scope

## Customer-specific notes

Do not add customer secrets, customer certificates with private keys, production tokens, internal deployment paths, or customer data to this package. Use a separate reviewed customer note for platform, compiler, linker, OS, CPU, and algorithm-scope requirements.

## Open TODO

- Add legal/evaluation notice after legal review.
- Add package archive creation after handoff format is chosen.
- Add detached signature records after signing tool and key-management policy are selected.
- Add generated SBOM records after generator selection.

## P1-14 thread-safety package note

The generated package includes `THREAD_SAFETY_PACKAGE_NOTES.txt`. Review it with `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md` before customer sharing. The note must not imply concurrent mutation support for caller-owned hash, HMAC, or DRBG contexts.

## P1-17 entropy/test-hook package note

Review `ENTROPY_HEALTH_PACKAGE_NOTES.txt`, `CLEAN_BUILD_CONFIGURE.log`, and `CLEAN_BUILD.log` together. The packaged SDK must come from the separate clean build, not the internal failure-injection build. No internal entropy header, deterministic hook API, entropy capture, seed material, generated random bytes, or DRBG state may be included.

## P1-19 entropy operational package note

Review `ENTROPY_OPERATIONAL_PACKAGE_NOTES.txt` together with the clean build logs and installed symbol/header scans. Confirm that no process-guard, source-class, raw-noise candidate, or internal entropy symbol is installed and that the package describes public random generation as implemented only on reviewed provider branches with the documented P1-19 limitations.


## P1-20 lifecycle and export-baseline package note

Review `RANDOM_LIFECYCLE_PACKAGE_NOTES.txt` and `CLEAN_RELEASE_SYMBOL_BASELINE.log` together. Confirm the log reports 53 expected and actual public symbols, both test hooks were disabled, internal symbol hiding was enabled, and no internal test/reseed/process helper entered the installed SDK. Symbol-name equality is a guardrail only and does not replace broader ABI or platform review.


## P1-21 concurrency and sanitizer package boundary

- The package is rebuilt with `ANO_CRYPTO_SANITIZER_MODE=none`, tests/hooks OFF, and internal symbols hidden.
- Internal Win32/pthread test helpers, backend metadata functions, sanitizer flags, and test executables are not installed as public API.
- `CONCURRENCY_SANITIZER_PACKAGE_NOTES.txt` records the required diagnostic lane, hosted-review limits, and build-identity ABI-0 decision.
- Actual hosted artifacts remain separate engineering records and are not bundled as customer assurance.


## P1-22 SHA-512-family package boundary

Review `SHA512_FAMILY_PACKAGE_NOTES.txt`, the packaged example, and clean export-baseline log together. Confirm all three SHA-2 digest and HMAC variants are exercised, digest/MAC values are not printed, and no internal SHA-512 context or implementation header is installed. Independent differential-provider results are not claimed.


## P1-23 differential and portability boundary

- [ ] `DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt` is present.
- [ ] Clean configure records OpenSSL differential tests OFF and provider requirement OFF.
- [ ] Installed headers contain no OpenSSL include or provider API name.
- [ ] Packaged shared libraries and examples do not require `libcrypto`/`libssl` because of the differential lane.
- [ ] Differential and portability test source files are absent.
- [ ] No digest, MAC, generated key/message, entropy, seed, DRBG state, or random output value is present.
- [ ] Hosted 32-bit, Windows, macOS, and big-endian results are not represented as reviewed unless actual referenced artifacts exist.

## P1-27 CMAC entry-gate package boundary

- [ ] `CMAC_ENTRY_GATE_PACKAGE_NOTES.txt` is present.
- [ ] The installed SDK does not contain `ac_cmac.h` or a CMAC public symbol.
- [ ] The package does not contain CMAC implementation, KAT, differential, key, message, subkey, intermediate, or tag material.
- [ ] The note records AES-CMAC as a future candidate and AES-CTR as deferred, not implemented.
- [ ] The clean Release symbol report remains 61 expected and actual names.
