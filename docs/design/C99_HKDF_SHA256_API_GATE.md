# C99 HKDF-SHA-256 API Gate

Status: `APPROVED_AND_IMPLEMENTED_P3_02`

## Frozen public surface

P3-02 adds exactly one public header and one public symbol:

```c
ano_crypto_status_t ano_crypto_hkdf_sha256(
    const uint8_t *ikm, size_t ikm_len,
    const uint8_t *salt, size_t salt_len,
    const uint8_t *info, size_t info_len,
    uint8_t *out, size_t out_len);
```

- Header: `include/anocrypto/ac_kdf.h`
- New symbol: `ano_crypto_hkdf_sha256`
- Retained symbols: all 69 names in `ANOCRYPTO_C_0`
- Additive line: the single new symbol in `ANOCRYPTO_C_1`
- Current surface: 15 headers, 70 exported symbols

Separate public extract and expand entry points were rejected because no accepted
product operation requires them and they would widen the ABI without benefit.
The implementation still follows RFC 5869 extract and expand internally.

## Semantics

- Empty IKM is a valid zero-length HMAC message.
- Empty salt means 32 zero octets, as specified by RFC 5869.
- Empty info is valid.
- Zero output validates arguments and module state, writes nothing, and succeeds.
- Output length is `0..8160`; 8161 is `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- Non-empty ranges require non-NULL pointers and non-wrapping `uintptr_t` ranges.
- Read-only inputs may alias each other.
- Non-empty output may not overlap IKM, salt, or info.
- Once the output range is structurally valid, every failure clears the complete
  requested output range. NULL or wrapping output ranges cannot be cleared.
- The caller owns a writable buffer of exactly `out_len` bytes; no allocation is
  performed by AnoCrypto-C.

## Ownership boundary

AnoCrypto-C owns only the generic HKDF-SHA-256 primitive. `SKF1`, its header,
file workflow, chunking, nonce rules, and application policies remain owned by
AnoSecureKit. No AnoSecureKit format or orchestration code is implemented here.
