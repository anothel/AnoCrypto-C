# KCMVP Candidate Algorithm Scope

Status: P2-02 current engineering scope; accepted validation scope remains pending.

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

## P1-29 CMAC hardening boundary

P1-29 retains the P1-28 AES-CMAC algorithm and service scope. It adds only
implementation-hardening and evidence coverage. The public CMAC API remains
eight functions, the key-purpose enum remains unchanged, and the clean Release
baseline remains 69 symbols. This does not establish a final validation scope
or target-specific side-channel conclusion.


## P2-02 scope confirmation

P2-02 adds no algorithm, mode, key purpose, header, or public function. The implemented scope remains unchanged and is bound to the internally frozen OE-01 shared-library candidate. Selection of the initial submitted algorithm set is `EXTERNAL_CONFIRMATION_REQUIRED`; unimplemented and deferred services remain outside the candidate configuration unless a separate gate approves them.
