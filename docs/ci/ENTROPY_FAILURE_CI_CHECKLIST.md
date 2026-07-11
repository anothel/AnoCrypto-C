# Entropy Failure-Injection CI Checklist

Status: P1-18 retained engineering checklist.

## Default Build Lanes

The normal Linux, macOS, and Windows lanes must use:

- `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF`;
- platform entropy provider enabled where supported;
- unit, negative, KAT, install smoke, public-surface, and policy checks;
- no entropy-byte or generated-byte logging.

## Dedicated Failure-Injection Lane

A Linux-only engineering lane may use:

```sh
cmake -S . -B build-entropy-failure \
  -DANO_CRYPTO_BUILD_TESTS=ON \
  -DANO_CRYPTO_BUILD_EXAMPLES=OFF \
  -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=ON \
  -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=OFF \
  -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=OFF
cmake --build build-entropy-failure --parallel
ctest --test-dir build-entropy-failure -L entropy-failure-injection --output-on-failure
```

This lane verifies that deterministic hooks do not depend on real entropy and that failure/partial-write outputs are zeroized. It is not a release or PoC binary lane.

## Clean Release/PoC Boundary

PoC staging must configure a separate clean build with:

- tests off;
- examples off for the SDK build;
- entropy test hooks off;
- internal symbol hiding on;
- shared library enabled.

The package check must reject installed headers containing internal entropy/test-hook tokens. On Unix-like systems with `nm`, it must reject exported `ano_crypto_internal_*` or entropy test-hook symbols.

## Evidence Review

Review these outputs:

- `ENTROPY_HEALTH_CHECK.txt`;
- `ENTROPY_HEALTH_REVIEW_NOTES.txt`;
- `ctest.log` test names/status only;
- `POC_PACKAGE_SMOKE.log` and clean-build logs;
- PoC `ENTROPY_HEALTH_PACKAGE_NOTES.txt`.

Do not upload or retain raw entropy, seed material, DRBG state, generated random bytes, private keys, tokens, or customer data.
