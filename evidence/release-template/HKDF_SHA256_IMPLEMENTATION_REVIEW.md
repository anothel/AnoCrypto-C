# HKDF-SHA-256 Implementation Review

- Version slice: P3-02
- Review status: TODO
- Reviewer: TODO
- Date: TODO
- Source revision: TODO

## API and ownership

- [ ] `ac_kdf.h` exposes exactly one generic combined extract-and-expand function.
- [ ] AnoCrypto-C contains no `SKF1` framing, file workflow, nonce, chunking, or product-orchestration code.
- [ ] AES-GCM, scrypt, SHA-1, RSA, ECDSA, and high-level product APIs are not added in this slice.

## RFC 5869 behavior

- [ ] RFC 5869 test cases 1, 2, and 3 pass.
- [ ] Empty IKM, empty salt, empty info, and zero-length output behavior matches the frozen API gate.
- [ ] Output lengths 1, 32, and 8160 pass; 8161 is rejected.
- [ ] Invalid pointer ranges, output/input overlap, and module-state failures clear the complete structurally valid output range.
- [ ] Read-only input ranges may alias each other.

## Differential and downstream evidence

- [ ] OpenSSL 3.x differential validation is test-only and passes through 8160 bytes.
- [ ] The exact 28-byte AnoSecureKit `SKF1` v1 info label and fixed compatibility vector pass.
- [ ] The AnoSecureKit Community/Enterprise dependency identity conflict remains `OPEN_DOWNSTREAM_SYNC_DECISION`.
- [ ] Enterprise adapter wiring remains `EXTERNAL_CONFIRMATION_REQUIRED` outside this repository.

## ABI and package

- [ ] All 69 `ANOCRYPTO_C_0` symbols remain unchanged.
- [ ] `ano_crypto_hkdf_sha256` is the only `ANOCRYPTO_C_1` symbol.
- [ ] Current public surface is 15 headers and 70 symbols.
- [ ] Current and retained header consumers, install smoke, PoC smoke, and ELF symbol-version checks pass.

## External status

- [ ] KCMVP remains `DEFERRED`.
- [ ] P1-34 hosted execution/review remains `NOT_RUN` / `UNREVIEWED`.
- [ ] Repository signing remains `UNSIGNED` and external release authorization remains `NO`.

Notes: TODO
