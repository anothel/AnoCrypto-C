# AnoSecureKit SKF1 HKDF Compatibility Evidence

Status: `EXACT_INFO_BYTES_VERIFIED_P3_02`

## Exact downstream source

- AnoSecureKit Community revision: `694459ebe497d15ba75ef76a52fa7c36ddd7bcce`
- Community source archive SHA-256: `951f523d9444450e3894f72fae5a45594441530b559dc3d7b4766258ee414484`
- Relevant implementation: `src/file_crypto.cpp`, `derive_file_key`
- Format contract: `docs/FORMAT.md`, SKF1 raw-key file format
- Fixture contract: `tests/fixtures/README.md`, `skf1-known-file.hex`

The exact v1 info bytes are 28 ASCII bytes without a terminating NUL:

```text
text: anosecurekit file sealing v1
hex:  616e6f7365637572656b69742066696c65207365616c696e67207631
sha256: b283fd0a8878585cccefd357fe71ae66a094a69a0baabe146290e5ac55591946
```

## Exact compatibility vector

- IKM: 32 zero bytes
- Salt: `000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f`
- Info: exact 28 bytes above
- Output length: 32
- PRK: `416c5392b9f36df188e90eb14d17bf0da190bfdb7f1f4956e6e566a569c8b15c`
- OKM/file key: `b2b5cce70684ba7d441b1525b91a44e66784aa349ebc4da767511993d663c26b`

`tests/kat/hkdf_sha256_kat_tests.c` pins this case using explicit byte values.
The evidence proves generic HKDF compatibility only. It does not implement or
claim full SKF1 compatibility, AES-GCM availability, file parsing, or workflow
parity.

## Open downstream identity item

AnoSecureKit Enterprise's vendored Community identity and the separately
supplied Community release identity remain different. The 181/182 normalized
path match and the unrelated secure-wipe export decoration difference do not
change these HKDF info bytes, but exact dependency lineage remains
`OPEN_DOWNSTREAM_SYNC_DECISION`. It is not resolved by P3-02.

Enterprise adapter wiring for the new primitive is
`EXTERNAL_CONFIRMATION_REQUIRED` in the downstream repository and is not part
of this AnoCrypto-C source change.
