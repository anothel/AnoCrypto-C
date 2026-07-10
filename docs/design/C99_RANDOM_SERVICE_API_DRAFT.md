# C99 Random Service API Draft

Status: P1-16 reserved boundary draft.

## Purpose

This document captures the future public random-service API direction with a reserved public `ano_crypto_random_*` header in P1-16.

The current implemented boundary remains:

- public DRBG core API with explicit entropy input
- internal platform entropy provider seam
- internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by an entropy source

P1-16 does not claim that random byte generation is implemented, reviewed for a validation boundary, or ready for customer integration.

## Candidate public API shape

The following names are present as reserved ABI placeholders in `include/anocrypto/ac_random.h`. They return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` where processing would otherwise occur.

```c
ano_crypto_status_t ano_crypto_random_context_size(size_t *out_size);
ano_crypto_status_t ano_crypto_random_init(void *ctx, size_t ctx_len,
                                           const uint8_t *personalization,
                                           size_t personalization_len,
                                           uint32_t flags);
ano_crypto_status_t ano_crypto_random_generate(void *ctx,
                                               uint8_t *out,
                                               size_t out_len,
                                               const uint8_t *additional_input,
                                               size_t additional_input_len,
                                               uint32_t flags);
ano_crypto_status_t ano_crypto_random_reseed(void *ctx,
                                             const uint8_t *additional_input,
                                             size_t additional_input_len,
                                             uint32_t flags);
ano_crypto_status_t ano_crypto_random_uninit(void *ctx, size_t ctx_len);
```

## Candidate implementation flow

```text
ano_crypto_random_init
  -> module must be OPERATIONAL
  -> select reviewed entropy source
  -> draw entropy input and nonce through adapter
  -> instantiate HMAC_DRBG-SHA-256
  -> zeroize local seed material

ano_crypto_random_generate
  -> module must be OPERATIONAL
  -> enforce max request size
  -> reseed if policy requires it
  -> generate through DRBG core
  -> do not log output bytes

ano_crypto_random_reseed
  -> module must be OPERATIONAL
  -> draw entropy input through adapter
  -> reseed HMAC_DRBG-SHA-256
  -> zeroize local entropy buffer
```

## P1-8 internal helper retained through P1-16

P1-8 added these internal helper functions in `src/internal/ac_drbg_internal.h`:

```c
ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(...);
ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(...);
```

They are intentionally internal. They are used to test the seam between the
entropy adapter and the HMAC_DRBG-SHA-256 core before committing to a public
random-service ABI.

## Non-goals in P1-16

- no implemented public random byte generation
- no public provider selection API
- no prediction-resistance support
- no fork/process-boundary reseed policy
- no entropy health-test framework
- no customer-facing guarantee of platform entropy suitability
- no formal validation claim

## Sensitive data rules

- Do not log entropy input, nonce, DRBG seed material, generated random bytes,
  private keys, tokens, or production CSP values.
- Evidence may record provider name, status code names, pass/fail, compiler,
  build options, and test names only.
- Internal helper tests must not print random bytes or deterministic seed bytes.

## TODO before public API exposure

1. Review whether public random contexts are caller-owned opaque buffers or
   library-owned handles.
2. Define reseed interval, request-size, additional-input, and personalization
   policies.
3. Define fork/process-boundary behavior.
4. Define prediction-resistance behavior or keep it explicitly unsupported.
5. Add platform CI for Linux, Windows, and Apple provider branches.
6. Add health-test and failure-injection evidence strategy.
7. Review whether `ano_crypto_random_generate()` belongs in the validation
   target boundary or a higher-level AnoSecureKit-style layer.
8. Perform API/ABI review before changing reserved placeholders into an implemented service.

## P1-16 decision document

See `docs/design/PUBLIC_RANDOM_API_DECISION.md` for the reserved-boundary decision and `docs/kcmvp/DRBG_ENTROPY_BOUNDARY_EVIDENCE.md` for evidence handling rules.
