# C99 Random Operational Portability Policy

Status: P2-02 target-bound engineering policy. OE-01 selects the Linux `getrandom` adapter; entropy responsibility and final evidence remain P2-04/`EXTERNAL_CONFIRMATION_REQUIRED`.

## Purpose

This document fixes the current operational boundary for source-backed `ano_crypto_random_*` contexts. It does not claim automatic protection against every execution-environment duplication event and is not official validation evidence.

For candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`, the public random service is built with the internal Linux `getrandom` adapter enabled. The Linux kernel RNG remains an external operating-environment provider, not a project-owned direct noise source. Windows and Apple provider branches remain outside the initial candidate.

## Current service prerequisites

The public random service is available only when both conditions are true:

1. a reviewed platform entropy-provider branch is enabled; and
2. a nonzero process-identity marker can be obtained.

If the process marker is unavailable, `ano_crypto_random_is_available()` reports false and context-size/init operations fail closed with `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`. The explicit-entropy DRBG API remains caller-managed and is not given this automatic process boundary.

## Process creation and inheritance

On the selected Linux branch, and on separately implemented Windows and Apple engineering branches, source-backed contexts record a private process marker. A process-marker change blocks generation with `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED`; requested output is cleared. A successful source-backed reseed refreshes the marker.

This detects ordinary process identity changes such as POSIX `fork()`. It is not a general VM, container, checkpoint, or suspend detector.

## VM snapshot, restore, and clone

OE-01 does not claim automatic VM snapshot/restore, clone, container-checkpoint, or hypervisor migration detection. AnoCrypto-C does not detect those events. When an application or orchestrator knows that execution state has been restored or duplicated, it must discard and reinitialize every inherited public random context before further generation. For a clone, independent reinitialization in each clone is required by this project policy; merely continuing from duplicated DRBG state is not accepted.

A successful call to `ano_crypto_random_reseed()` can refresh a context only when the application can establish that the context was not duplicated into another concurrently usable instance. When duplication is possible or uncertain, uninitialize and create a new context instead.

## Suspend and resume

AnoCrypto-C does not automatically detect suspend/resume. Applications with a threat model that treats long suspension, host migration, or resume as a state-freshness boundary should discard and reinitialize public random contexts after resume. A less strict product policy may choose explicit reseed after resume, but that choice must be documented by the consuming product and must not be presented as automatic module behavior.

## Reseed interval and failure recovery

The HMAC_DRBG core enforces the SP 800-90A Rev. 1 maximum reseed interval used by the implementation (`2^48` generate requests). The final allowed request succeeds; the next request returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and clears output. A failed provider reseed does not bypass the limit. A later successful reseed resets the counter and restores generation.

The dedicated reseed-counter test hook is compiled only when `ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=ON`; that option requires tests and is forbidden in release and PoC builds.

## Test and evidence handling

Lifecycle stress and hosted platform review retain only statuses, provider identifiers, counts, and policy metadata. Generated output is transiently zeroized and is not printed, uploaded, or stored in evidence.

## Standards references

- NIST SP 800-90A Revision 1, deterministic random bit generator mechanisms and reseeding behavior.
- NIST SP 800-90C final, random bit generator construction and randomness-source boundaries.

These references guide design review only. AnoCrypto-C remains in an engineering and KCMVP-readiness stage.
