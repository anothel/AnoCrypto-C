# C99 Self-Test Design Skeleton

## Status

P1 foundation only. The current self-test verifies build identity availability and KAT manifest skeleton wiring.

## Startup Flow

```text
module_init(NULL)
  -> SELF_TEST_RUNNING
  -> build identity skeleton check
  -> KAT manifest skeleton check
  -> OPERATIONAL or ERROR_STATE
```

## KAT Manifest Rules

- Add the manifest entry before importing or writing a vector fixture.
- Record source provenance in `cleanroom/SOURCE_MANIFEST.md`.
- Use only public-source or user-owned test vectors.
- Do not include secrets, private keys, tokens, or customer material.

## TODO(P1)

- Add per-algorithm KAT runner interface.
- Add negative self-test path evidence.
- Add self-test logs suitable for release evidence.
- Define conditional self-tests for DRBG and key lifecycle events.
