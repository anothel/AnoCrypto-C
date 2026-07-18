# AES-256-GCM P3-03 Implementation Review

- API: two one-shot functions in `ac_aead.h`
- key/nonce/tag: 32/12/16 bytes fixed
- decryption: authentication verified before plaintext output
- failure: no partial output; accepted output range cleared
- implementation dependency: existing AnoCrypto-C AES core and CSP handles
- OpenSSL: test-only differential provider, not linked by production code
- downstream evidence: exact SKT1/SKF1/SKP1 v1 primitive fixtures
- ABI: two symbols on additive `ANOCRYPTO_C_2`
- external claims: none; KCMVP/hosted/signing/release states unchanged
