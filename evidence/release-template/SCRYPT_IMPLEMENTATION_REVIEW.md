# P3-05 scrypt Implementation Review

- API count: 1.
- Profile: N=32768, r=8, p=1, output=32.
- Workspace: caller-owned, 33,556,544 bytes, zeroized.
- Production allocator: none.
- Production OpenSSL dependency: none.
- Exact downstream primitive compatibility: required.
- ABI: one symbol on ANOCRYPTO_C_4; ABI-0/1/2/3 retained.
- KCMVP/hosted/signing/release status: unchanged and blocked/deferred as recorded.
