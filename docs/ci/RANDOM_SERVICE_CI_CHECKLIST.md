# Public Random Service CI Checklist

Status: P1-19 engineering checklist.

## Default provider lanes

Run on Linux, Windows, and Apple reviewed provider branches:

1. Configure with OS entropy provider enabled and test hooks disabled.
2. Build with strict warnings.
3. Run `ano_crypto_random_service_tests`.
4. Confirm context-size, init, generate, reseed, and uninit succeed.
5. Confirm unsupported prediction-resistance requests fail and clear output.
6. Confirm explicit DRBG contexts are rejected by random generate/reseed and random contexts are rejected by explicit DRBG generate/reseed.
7. On POSIX, confirm inherited state requires reseed in the child.
8. Run installed-package and PoC smoke flows.
9. Run `ano_crypto_random_service_policy_check`.

## Provider-disabled lane

Configure with:

```text
ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF
ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF
```

Confirm:

- `ano_crypto_random_is_available()` returns zero;
- context size returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` and zero;
- init returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` and clears storage;
- unrelated SHA-256, HMAC, DRBG, CSP, install, and policy tests still pass.

## Failure-injection lane

Configure tests with entropy test hooks enabled and the real provider disabled.

Confirm:

- deterministic init/generate/reseed succeeds;
- complete provider failure clears init storage;
- partial-write provider failure clears init storage;
- manual reseed failure returns `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`;
- a retry can succeed after the provider recovers;
- no generated or deterministic provider bytes are printed or retained.

## Clean Release and PoC checks

- Test hooks are off.
- Internal symbols are hidden where supported.
- Installed headers contain no internal entropy, source-class, process-guard, or hook type.
- Exported public symbols remain equal to the P1-18 public symbol-name baseline.
- The PoC package includes `RANDOM_SERVICE_PACKAGE_NOTES.txt`.
- The PoC example prints only status and byte count.

## Evidence checks

- `RANDOM_SERVICE_CHECK.txt` passes.
- `RANDOM_SERVICE_REVIEW_NOTES.txt` describes lifecycle and limitations.
- `ctest.log` includes the random-service lifecycle test.
- Evidence contains no provider output, entropy, nonce seed material, DRBG state, or generated random bytes.

## Hosted-run TODO

Actual hosted-run results must be reviewed before claiming cross-platform support beyond the configured engineering matrix.
