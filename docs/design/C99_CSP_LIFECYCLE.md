# C99 CSP / Key Lifecycle Skeleton

## Status

P1 foundation only. Public handle and state enums are present, but handle storage, import/export policy, and key ownership rules are TODO.

## Candidate CSP States

```text
INVALID
ALLOCATED
INITIALIZED
ACTIVE
RESTRICTED
ZEROIZED
DESTROYED
```

## API Direction

- Public callers receive opaque integer handles.
- Raw key buffers are caller input only.
- Key material must not be printed, logged, persisted, or added to test fixtures.
- Export policy must be explicit per algorithm and candidate mode.

## TODO(P1)

- Decide handle table ownership model.
- Define import/export permission matrix.
- Add lifecycle tests for destroy, zeroize, and failure paths.
- Define thread-safety and process-global state policy.
