# C99 Self-test Design

Status: P2-03 implementation-alignment record. Normative candidate policy is in:

- `docs/kcmvp/KCMVP_SELF_TEST_AND_INTEGRITY_SPEC.md`;
- `docs/kcmvp/KCMVP_FINITE_STATE_MODEL.md`;
- `docs/kcmvp/KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md`;
- `docs/kcmvp/P2_03_IMPLEMENTATION_TEST_DELTA.md`.

## Current startup flow

`ano_crypto_module_init(NULL)` transitions the module to `SELF_TEST_RUNNING`, invokes the current internal self-test sequence, and enters `OPERATIONAL` only when every implemented check passes.

Current implemented sequence:

1. Build-identity structure sanity check.
2. KAT-manifest structure sanity check.
3. SHA-256/384/512 KATs.
4. HMAC-SHA-256/384/512 KATs.
5. AES-128/192/256 primitive and CBC-path KATs.
6. AES-CMAC full-tag KAT.
7. HMAC_DRBG-SHA-256 KAT.

The build-identity and KAT-manifest checks are supporting structural checks. They are not module-byte authentication.

## Integrity status

The public `ANO_CRYPTO_SELFTEST_FLAG_INTEGRITY_PLACEHOLDER` value is retained for ABI compatibility. The current runtime does not authenticate the installed shared-object bytes, and the flag MUST NOT be used as evidence that a module-integrity test ran.

P2-03 selects `INT-HMAC-SHA256-SINGLE-SO-R1` as an internal candidate only. It remains:

```text
SELECTED_CANDIDATE_NOT_IMPLEMENTED
EXTERNAL_CONFIRMATION_REQUIRED
```

No integrity key, tag, binary metadata record, runtime verifier, or tamper test is introduced in P2-03.

## Failure behavior

An implemented startup structural/KAT failure records status and enters sticky `ERROR_STATE`. Cryptographic processing is denied for the remainder of the process. Orderly `SHUTDOWN` can be reinitialized in-process and reruns startup checks; fatal `ERROR_STATE` cannot be reset in-process.

## Service gate

Public cryptographic processing APIs require `OPERATIONAL`. Self-tests use internal primitive helpers so startup does not expose public cryptographic services before the gate opens.

## On-demand gap

The public `ano_crypto_selftest_run()` symbol currently executes the internal sequence but does not own the full `OPERATIONAL -> SELF_TEST_RUNNING -> OPERATIONAL/ERROR_STATE` transition. P2-03 specifies the target behavior and records implementation/tests as open deltas rather than claiming completion.

## Conditional-test boundary

- Public-key pairwise-consistency tests are out of scope because no public-key algorithm or key-generation service exists.
- Symmetric/HMAC/CMAC key-import validation is service/CSP behavior, not silently relabeled as a cryptographic self-test.
- Entropy-provider/direct-source health-test classification remains P2-04 and `EXTERNAL_CONFIRMATION_REQUIRED`.

## Evidence boundary

Current KAT and state tests are engineering evidence only. They are not institution test results or formal validation evidence. Logs and documents must not include keys, entropy, DRBG state, random output, expected/computed KAT values, integrity key, or expected/computed integrity tag.
