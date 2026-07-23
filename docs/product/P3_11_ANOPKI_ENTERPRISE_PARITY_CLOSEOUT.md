# P3-11 AnoPKI Enterprise Parity Closeout

Status: `COMPLETE`

## Scope

AnoPKI Enterprise commit `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec` connects the P3-10 public AnoCrypto-C SDK
below existing AnoPKI codecs/workflows.

## Frozen inputs

- Enterprise parent: `c3f7b679782f52c90311b7b7670b577ee972c1f6`;
- Community exact baseline: `ab9d76597df93ac1ac8b7938f4d25ba64f59f8dc`;
- AnoCrypto-C: `0.1.0-p3-10-restricted-sha1-ocsp-certid` / `98f544a574f70b387c300f171243b4e87174937a`;
- pinned SDK DLL SHA-256: `7dd56b00f38699509e90a62968ec0ff2c37a2735e4479c0df4a2c60f8b2e26cd`.

## Completed evidence

- eight PKI operations: 8/8;
- approved algorithms: RSA SHA-256/384/512, P-256 SHA-256 and restricted OCSP
  CertID SHA-1;
- Enterprise/AnoCrypto-C CTest: 15/15, zero skipped;
- Enterprise/OpenSSL regression: 15/15;
- exact Community baseline: 14/14;
- AnoCrypto-C: 102/102;
- no `libcrypto` or `libssl` runtime dependency in the AnoCrypto profile;
- malformed, unsupported, mismatched and missing-dependency paths fail closed;
- capability reporting and `fallback_enabled:false` match implementation.

## Artifact bindings

- source ZIP: `fefae0c7c3fa340c782010ef4ca46cf8db6b923f437497cc6a7d4e1b54a798d2`;
- patch: `c822f58dc497d78f4173a43f8b8d8eef8e4c06c54db8f52745fa01a2183f66ac`;
- validation: `ee3060381cd19d52c4bc78b55cfbcb93d9d04c44cd61dcdf18df10146774d837`.

## Limits

The checkpoint is local engineering evidence. It is not hosted qualification,
production-key-provider evidence, customer release approval, KCMVP validation,
or a race-clean claim.
