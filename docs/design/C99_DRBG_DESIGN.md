# C99 DRBG Design Skeleton

Status: P1-3 draft.

## Implemented P1-3 slice

- HMAC_DRBG-SHA-256 deterministic core
- caller-provided opaque DRBG context buffer
- instantiate / reseed / generate / uninstantiate lifecycle
- public C API service gate through `ano_crypto_module_require_operational()`
- startup self-test KAT for HMAC_DRBG-SHA-256
- public KAT executable for HMAC_DRBG-SHA-256
- negative tests for entropy length, nonce length, unsupported flags, invalid state, and cleanup

## Explicitly not implemented yet

- OS entropy source adapter
- prediction-resistance requests
- Hash_DRBG-SHA-256
- automatic reseed policy driven by an entropy adapter
- thread-safety policy for shared DRBG contexts
- production random-service API above the DRBG core

## Candidate Scope

| Algorithm | Status | Notes |
|---|---|---|
| HMAC_DRBG-SHA-256 | Implemented P1-3 slice | Deterministic core only; entropy input is explicit. |
| Hash_DRBG-SHA-256 | ABI-reserved | Returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` from processing APIs. |

## State Model

```text
UNINSTANTIATED -> INSTANTIATED -> ERROR
        |              |
        +--------------+-> UNINSTANTIATED via uninstantiate/zeroize
```

Module state is separate from DRBG instance state. DRBG instantiate, reseed, and generate require the module to be operational. DRBG uninstantiate is allowed outside the operational state so callers can clear caller-owned context storage during cleanup.

## P1-3 public API boundary

The DRBG core API accepts explicit entropy input:

```c
ano_crypto_drbg_instantiate(ctx, ctx_len, algorithm,
                            entropy_input, entropy_input_len,
                            nonce, nonce_len,
                            personalization, personalization_len,
                            flags);

ano_crypto_drbg_reseed(ctx,
                       entropy_input, entropy_input_len,
                       additional_input, additional_input_len,
                       flags);
```

This is intentionally a low-level DRBG core interface. A later entropy adapter or random-service layer must collect and assess entropy before calling this core API. P1-3 does not claim that caller-provided entropy is suitable for operational use.

## HMAC_DRBG-SHA-256 policy constants

| Constant | Value | P1-3 meaning |
|---|---:|---|
| `ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE` | 32 bytes | Minimum entropy input accepted by instantiate/reseed. |
| `ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE` | 16 bytes | Minimum nonce accepted by instantiate. |
| `ANO_CRYPTO_DRBG_HMAC_SHA256_MAX_REQUEST_SIZE` | 65536 bytes | Maximum bytes per generate request. |
| `ANO_CRYPTO_DRBG_HMAC_SHA256_OUTPUT_SIZE` | 32 bytes | SHA-256 output size. |

## Entropy Adapter Boundary

TODO(P1/P2): add an internal entropy source adapter interface. Do not treat OS RNG calls as the DRBG itself.

The future adapter should define:

- platform entropy source selection
- entropy input length policy
- failure mapping to `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`
- health-test hooks if applicable
- evidence/logging rules that never record raw entropy
- reseed policy and fork/process-boundary behavior

## Sensitive Data Rules

- Do not log entropy, seed material, nonce, additional input, private scalar, or raw key material.
- Do not include operational secret material in tests or evidence logs.
- Public DRBG KAT vectors are synthetic/public test material only.
- Zeroize DRBG context storage on uninstantiate and on fatal internal paths where possible.

## TODO

1. Add a reviewed entropy adapter design.
2. Decide whether a higher-level `ano_crypto_random_generate()` API belongs in AnoCrypto-C P1 or P2.
3. Add reseed counter and fork-safety tests when the adapter exists.
4. Add install-prefix evidence capture for DRBG KAT logs.
5. Add side-channel review checklist for HMAC_DRBG state transitions.
