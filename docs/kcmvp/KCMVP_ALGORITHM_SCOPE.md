# KCMVP Candidate Algorithm Scope

Status: P1-28 engineering draft; not an official validation scope.

| Family | Service | Status and boundary |
|---|---|---|
| Hash | SHA-256, SHA-384, SHA-512 | Implemented one-shot/streaming with startup and extended KATs. |
| MAC | HMAC-SHA-256/384/512 | Implemented raw-key/streaming/CSP-handle paths. |
| MAC | AES-CMAC with AES-128/192/256 | Implemented full 16-byte tags, dedicated CMAC handles, startup/extended KATs, negative and test-only differential coverage. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented deterministic core and source-backed public random lifecycle. |
| DRBG | Hash_DRBG-SHA-256 | Algorithm ID placeholder; not implemented. |
| Block cipher | AES-128/192/256 | Implemented portable primitive behind CSP-backed services. |
| Mode | CBC without padding | Implemented with caller IV and complete blocks only. |
| Mode | CTR/GCM/GMAC | Deferred; requires separate counter/AEAD/authentication policy gate. |
| Other | Additional domestic/public-key algorithms | Deferred pending customer and validation-target scope. |

P1-28 adds one public header, one dedicated key-purpose enum value, and eight
CMAC functions. The reviewed clean Release ABI-0 baseline is 69 names. This
engineering scope does not claim official approval, completed assessment, or
universal side-channel properties.
