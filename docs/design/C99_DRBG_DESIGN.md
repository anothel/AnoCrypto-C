# C99 DRBG Design Skeleton

## Status

P1 foundation only. The DRBG public API shape is present, but DRBG implementation, entropy adapter, and KAT vectors are TODO.

## Candidate Scope

- HMAC_DRBG-SHA-256
- Hash_DRBG-SHA-256

Final P1 selection is still TODO. The skeleton keeps both IDs so API review can happen before implementation.

## State Model

```text
UNINSTANTIATED -> INSTANTIATED -> ERROR
        |              |
        +--------------+-> UNINSTANTIATED via uninstantiate/zeroize
```

Module state is separate from DRBG instance state. DRBG instantiate, reseed, and generate must require the module to be operational.

## Entropy Adapter Boundary

TODO(P1): add an internal entropy source adapter interface. Do not treat OS RNG calls as the DRBG itself.

## Sensitive Data Rules

- Do not log entropy, seed material, nonce, private scalar, or raw key material.
- Do not include secret material in tests or evidence logs.
- Use public KAT vectors only after recording source provenance in the clean-room manifest.
