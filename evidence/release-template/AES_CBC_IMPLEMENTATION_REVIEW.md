# AES/CBC Implementation Review

- Version slice: P1-25
- AES-128/192/256 primitive KAT: pending evidence run
- CBC-AES128/192/256 KAT: pending evidence run
- Startup self-test: pending evidence run
- Negative and aliasing tests: pending evidence run
- CSP type/length/stale-handle checks: pending evidence run
- OpenSSL test-only differential result: pending evidence run
- Installed SDK/PoC smoke: pending evidence run
- Exported symbol baseline expected: 61
- Raw key and expanded schedule clearing reviewed: pending
- Table-free arithmetic implementation present: yes
- Compiler/target side-channel review: TODO
- Keys/IV/plaintext/ciphertext in evidence: must be no
- Official validation status claimed: no
