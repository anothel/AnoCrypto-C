# C99 Public-Key Key-Handle Architecture Gate

Status: `APPROVED_P3_07_DESIGN_ONLY`

## Scope and freeze

P3-07 freezes the architecture and a candidate future C99 contract for local
software public-key objects. It adds no public header, declaration, type, enum,
constant, function, exported symbol, symbol-version line, implementation, or
runtime-version change.

The retained implementation baseline remains:

- version `0.1.0-p3-05-scrypt-fixed-profile`;
- 16 public headers;
- 80 exported symbols;
- `ANOCRYPTO_C_0` through `ANOCRYPTO_C_4`;
- P1-29 source/ABI compatibility.

`ANOCRYPTO_C_5` is not introduced or reserved by this gate.

## Source and product evidence

The architecture is derived from the exact AnoPKI Community revision
`ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5`, the retained exact Enterprise
identity record at `68b379e7684c9d8b44c53728949774088352a9d9`, and the P3-07 CURRENT ONLY pack.
The Community commit and relevant operation/provider/test paths were re-read at
the exact revision. The Enterprise source ZIP hash and collection hash were
cross-checked against the supplied SHA-256 records. Raw Enterprise source ZIP
bytes were not mounted in the P3-07 execution environment, so byte-for-byte
recomputation of that archive is `EXTERNAL_CONFIRMATION_REQUIRED`.

The exact product evidence confirms:

- certificate, CRL, and OCSP signing through RSA and P-256 ECDSA keys;
- SHA-256 signing for CRL and OCSP response paths;
- SHA-256/384/512 certificate-signature selection;
- CSR proof-of-possession verification;
- issuer certificate/private-key binding checks;
- fail-closed key parse, algorithm mismatch, binding mismatch, provider
  unavailability, production exportability, and no-fallback behavior;
- SHA-1 only as the current default OCSP CertID digest, not as an issuance
  signature target.

## Decision summary

| Decision | Disposition |
|---|---|
| Reuse the existing opaque `ano_crypto_csp_handle_t` identity | `APPROVED` |
| Internally tag each asymmetric handle with role, algorithm, and purpose | `APPROVED` |
| Separate public/private public typedefs and duplicate lifecycle APIs | `DEFERRED_NOT_NEEDED` |
| Reuse `ano_crypto_key_destroy`, `ano_crypto_key_get_state`, and CSP validity/state APIs | `APPROVED` |
| Parse PEM, DER, ASN.1, PKCS#8, or SPKI in AnoCrypto-C | `DEFERRED_DOWNSTREAM_OWNED` |
| Import normalized algorithm-native components | `APPROVED` |
| Message-based sign/verify API | `APPROVED` |
| Public digest-based sign/verify API | `DEFERRED_NO_CURRENT_PRODUCT_NEED` |
| RSA PKCS#1 v1.5 hashing and canonical DigestInfo construction in AnoCrypto-C | `APPROVED` |
| ECDSA primitive signature as fixed-width raw `r || s` | `APPROVED` |
| ECDSA DER encoding/decoding in AnoCrypto-C | `DEFERRED_DOWNSTREAM_OWNED` |
| Public asymmetric metadata query in the first RSA slice | `DEFERRED_NO_CURRENT_PRODUCT_NEED` |
| Key export, persistence, wrapping, or cloning | `DEFERRED_NOT_APPROVED` |
| HSM/KMS/PKCS#11 and `key_ref` routing | `DEFERRED_ENTERPRISE_OWNED` |
| Smallest reasonable RSA implementation budget: three new symbols | `APPROVED_CANDIDATE_BUDGET` |
| Reserve a future ABI line | `DEFERRED_TO_IMPLEMENTATION_GATE` |
| Next slice: RSA PKCS#1 v1.5 SHA-2 plus RSA import/handle implementation | `APPROVED_P3_08_SELECTION` |

## Repository ownership

### AnoCrypto-C may own

- normalized in-memory RSA and P-256 key objects;
- role/algorithm/purpose binding;
- component consistency validation;
- non-exportable module-owned handle lifecycle;
- message hashing and generic RSA/ECDSA sign/verify primitives;
- private-key zeroization and stale-handle rejection;
- stable C ABI and algorithm evidence.

### AnoPKI retains

- PEM/DER/ASN.1, PKCS#8, SPKI, PKCS#10, X.509, CRL, and OCSP codecs;
- construction of exact TBS bytes;
- signatureAlgorithm identifiers and object embedding;
- ECDSA ASN.1 DER signature conversion;
- certificate/profile/chain/EKU/time/OCSP policy and normalized results;
- comparison of product objects and compatibility fixtures.

### Enterprise retains

- `key_ref` syntax and provider selection;
- HSM/KMS/PKCS#11 routing and non-exportable provider custody;
- authorization, retry/idempotency, evidence, audit, packaging, and release
  decisions;
- explicit profile capability reporting and no-fallback enforcement.

AnoCrypto-C local handles are not a substitute for an external provider handle.
An HSM or KMS that performs signing without exporting private components remains
outside this local import API.

## Handle model

### Common opaque identity

Future asymmetric keys use the existing `ano_crypto_csp_handle_t`. A separate
public/private typedef would still be an integer token in C, would not prevent
misuse, and would require duplicate lifecycle functions. Runtime metadata is the
enforcement boundary.

Each live asymmetric object is immutable and internally records:

- role: `ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY` or
  `ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY`;
- algorithm family: RSA or EC P-256;
- permitted purpose: verify for a public handle, sign for a private handle;
- normalized component sizes and implementation metadata;
- generation and lifecycle state.

A copied 64-bit token is an alias to the same object, not a cloned key and not a
second ownership grant. Destroying the object through any alias invalidates all
aliases. No copy/clone/export function is approved. The caller already knows the role,
algorithm, and normalized public metadata supplied at import. P3-08 therefore
does not need a public asymmetric-metadata query symbol. The core enforces its
bound metadata on every use, while the adapter retains redacted product
metadata.

### Internal storage

The current 128-byte symmetric CSP slot is not large enough for RSA. A later
implementation must use a separate internal asymmetric storage family or an
equivalent bounded redesign while preserving existing handle behavior. The
public token format remains opaque. The implementation must not expose pointers,
heap addresses, table indexes, or provider-native objects.

Production heap allocation is not pre-approved by P3-07. P3-08 must freeze the
bounded storage strategy, capacity, allocation-failure behavior, and complete
zeroization evidence before implementation.

### Lifecycle

- Import requires an operational module and returns an invalid handle on every
  failure.
- A live handle is immutable.
- `ano_crypto_key_destroy()` remains the single public destroy operation.
- Destroy is allowed outside `OPERATIONAL`, as for existing CSP cleanup.
- A generation change prevents stale-handle reuse.
- Orderly shutdown invalidates and zeroizes every live asymmetric private-key
  object; public objects and metadata are also cleared.
- Fatal transition follows the same fail-closed cleanup direction as existing
  CSP state. Any retained lock-failure limitation remains explicit rather than
  being converted into a stronger claim.
- Reinitialization never revives an old handle.

### Concurrent operation lease

With thread safety enabled on a reviewed platform branch, a future operation
must acquire an internal immutable-object lease before leaving the handle-table
lock. New leases fail once destroy or module cleanup marks the object closing.
A destroy racing with an operation either:

1. observes no acquired lease and invalidates the operation as stale; or
2. waits for the already-acquired operation lease to finish, then zeroizes and
   completes destruction.

This avoids a private-key use-after-free and avoids copying an entire private key
into untracked operation memory. Independent sign/verify calls using the same
immutable handle may run concurrently only after this lease model and its tests
are implemented. With thread safety disabled, the caller serializes all handle
and module operations.

## Algorithm-native key ingress

All multi-byte integers are unsigned big-endian. Non-zero integers use minimal
encoding with no redundant leading zero octet. Empty, zero, wrapping, or
non-canonical component ranges are rejected. Read-only input ranges may alias
each other; the output-handle pointer may not overlap any non-empty input range.
AnoCrypto-C reads components only during import and retains only the normalized
internal key object. The caller remains responsible for clearing caller-owned
private component buffers.

### RSA

One future RSA import operation is preferred for both roles.

Public role requires:

- modulus `n`;
- public exponent `e`;
- private component lengths equal to zero.

Private role requires:

- `n`, `e`, private exponent `d`, prime `p`, and prime `q`;
- exactly two primes;
- consistency validation including `n = p * q` and exponent relationships;
- derivation and validation of CRT values inside the module;
- rejection of inconsistent public/private material;
- distinct probable-prime `p` and `q`, `gcd(e, lcm(p-1,q-1)) = 1`, and
  `d * e = 1 mod lcm(p-1,q-1)` validation.

Passing the public `n` and `e` that came from the issuer certificate together
with the private components allows the import operation to enforce the
key/public-material binding after AnoPKI performs its codec work. Multi-prime RSA
and encoded key containers are deferred.

The exact accepted modulus sizes and exponent allow-list are `TODO_P3_08`; the
current product fixtures positively establish RSA-2048, while broader size
support requires its own vector and policy evidence.

### EC P-256

A later P-256 import operation uses:

- public role: one 65-byte uncompressed SEC1 point `0x04 || X || Y`;
- private role: one 32-byte scalar plus the required 65-byte expected public
  point;
- curve membership, non-infinity, scalar range, and `Q = dG` validation.

Compressed points, other curves, ECDH, encoded key containers, and implicit
curve selection are deferred. This representation decision is approved, but its
implementation is not selected for P3-08.

## Sign and verify model

### Alternative comparison

| Alternative | Benefits | Costs/risks | P3-07 result |
|---|---|---|---|
| message-based sign/verify | module binds hashing, scheme selection, RSA DigestInfo, and primitive operation; prevents caller-supplied digest-length/scheme mismatch; directly consumes AnoPKI-owned exact TBS bytes | caller must provide the complete message/TBS range; no public prehash/streaming shortcut | `APPROVED` |
| digest-based sign/verify | permits independent or streaming hashing before the private-key operation | splits one signature contract across hash and signature APIs, permits wrong-digest/scheme orchestration, and has no positive exact product requirement | `DEFERRED_NO_CURRENT_PRODUCT_NEED` |
| caller-provided DigestInfo or raw RSA | maximum low-level flexibility | exposes unsafe encoding choices and weakens algorithm binding | `REJECTED` |

The exact AnoPKI operations already produce bounded CSR, certificate, CRL, and
OCSP TBS byte sequences, so P3-07 selects the message-based alternative rather
than adding a prehashed public contract.

### Message-based API

A message-based service is approved because the downstream product already owns
exact TBS byte encoding and because keeping hashing inside the primitive reduces
hash/scheme mismatch and RSA DigestInfo construction errors. An empty message is
valid. A public prehashed service is deferred until a positive product
requirement exists.

The future scheme selector is expected to distinguish at least:

- RSA PKCS#1 v1.5 with SHA-256;
- RSA PKCS#1 v1.5 with SHA-384;
- RSA PKCS#1 v1.5 with SHA-512;
- later, ECDSA P-256 with SHA-256.

The exact enum names and numeric values are not frozen by P3-07.

### RSA ownership

AnoCrypto-C owns:

- SHA-2 hashing of the supplied message;
- the canonical RFC 8017 DigestInfo prefix for the selected digest;
- EMSA-PKCS1-v1_5 encoding and strict verification;
- the raw fixed-size RSA signature octet string.

AnoPKI owns the X.509/CSR/CRL/OCSP signatureAlgorithm identifier and must ensure
that its object metadata matches the selected AnoCrypto-C scheme.

### RSA private-operation hardening

P3-08 must freeze and implement a reviewed private-operation hardening policy:

- multiplicative RSA blinding using module-owned approved random service output,
  or a separately justified equivalent;
- fail closed and clear the complete accepted signature output if blinding
  randomness is unavailable;
- use CRT only after validated/derived CRT parameters exist;
- verify the private-operation result with the public exponent before releasing
  a signature, so a CRT computation fault cannot be emitted silently;
- clear blinding values, CRT intermediates, encoded-message buffers, and all
  private operation temporaries before return;
- retain constant-time and compiler-output review as an explicit P3-08 evidence
  item rather than making a side-channel claim in P3-07.

Exact blinding construction, retry bounds, random-service failure mapping, and
compiler/CPU review are `TODO_P3_08_SECURITY_GATE`.

### ECDSA ownership

AnoCrypto-C will use a primitive signature representation of exactly 64 bytes:
32-byte big-endian `r` followed by 32-byte big-endian `s`. AnoPKI converts
between this representation and canonical ASN.1 DER for PKI objects. ECDSA nonce
generation strategy, deterministic signing, and low-`s` policy are
`TODO_P3_09` and are not silently decided by this gate.

## Candidate future C surface

P3-07 adds none of the following. They are candidate names for the next API
review, not ABI authorization. Header placement is `TODO_P3_08`: the review
must compare extending `ac_key.h` with adding one focused signature/public-key
header, and must not treat either choice as pre-approved.

```c
/* Candidate only; no declaration is added in P3-07. */
ano_crypto_status_t ano_crypto_rsa_key_import(/* role, n, e, d, p, q, out */);
ano_crypto_status_t ano_crypto_signature_sign(/* private handle, scheme,
                                                 message, output */);
ano_crypto_status_t ano_crypto_signature_verify(/* public handle, scheme,
                                                   message, signature */);
```

A later ECDSA slice should reuse the sign/verify functions and add only one
algorithm-specific import operation:

```c
/* Candidate only; deferred beyond P3-08. */
ano_crypto_status_t ano_crypto_ec_p256_key_import(/* role, scalar, point, out */);
```

The smallest reasonable candidate symbol budget is therefore:

| Slice | Candidate new symbols | Reused symbols |
|---|---:|---|
| P3-08 RSA | 3 | existing key destroy/state and CSP validity/state APIs |
| Later P-256 ECDSA | 1 | generic sign/verify and lifecycle APIs |
| Combined public-key architecture | 4 | all existing lifecycle helpers |

Combining sign and verify into one direction-flag function was rejected because
it creates incompatible input/output contracts and weakens misuse resistance.
Combining RSA and EC import into one untyped component-array function was
rejected because it makes validation and diagnostics ambiguous.

The P3-08 API review may change candidate names or prove that another symbol is
necessary. No count and no ABI line are pre-approved by P3-07.

## Buffer and failure rules for the future API

### Import

- `out_handle` is required and is set to
  `ANO_CRYPTO_CSP_INVALID_HANDLE` before component validation.
- Every non-empty range requires a non-NULL pointer and a non-wrapping address
  range.
- Read-only component inputs may alias each other.
- `out_handle` may not overlap any component range.
- Malformed, non-canonical, zero, or mathematically inconsistent components
  return `ANO_CRYPTO_ERR_INVALID_ARGUMENT`; unsupported approved-policy sizes
  or exponents return `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- Role, algorithm, or scheme mismatch returns
  `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- No partially initialized object or reusable table residue remains on failure.

### Sign

- `message == NULL` is valid only when `message_len == 0`.
- A size query uses `signature == NULL`, capacity zero, and a non-NULL length
  output; it validates module state, handle role, algorithm, scheme, and message
  range before returning the exact required size.
- RSA required size equals the modulus length. P-256 ECDSA required size is 64.
- A too-small buffer returns `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL`, reports the
  required size, writes no partial signature, and clears the accepted supplied
  capacity.
- Other failures after the output range is accepted clear the entire supplied
  output capacity and report zero bytes written.
- Output may not overlap the message or the length-output object.
- Success writes exactly one complete signature and reports its length.

### Verify

- Message and signature are read-only and may alias each other.
- NULL is valid only for a zero-length message; a signature is always non-empty.
- Wrong handle role, algorithm, or selected scheme is an API mismatch and fails
  with `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- A structurally supplied but cryptographically invalid, modified, truncated, or
  non-canonical signature returns `ANO_CRYPTO_ERR_AUTHENTICATION_FAILED`.
- Verify has no output buffer and never exposes recovered DigestInfo or
  intermediate values.

### General

- Every pointer range is checked for `uintptr_t` wrap before overlap checks.
- No key, component, message, digest, signature, nonce, intermediate, or private
  metadata value is logged.
- No partial output is permitted.

## Evidence plan

### P3-08 RSA

- public RSA signature KATs for each approved modulus/digest combination;
- blinding success/failure, CRT consistency, injected-fault detection, and
  private-temporary zeroization tests;
- RFC 8017 encoding edge cases and strict DigestInfo verification;
- malformed/inconsistent component negatives;
- modified message/signature, wrong key, wrong digest, short/long signature,
  stale handle, role mismatch, module-state, null, overflow, alias, overlap,
  short-buffer, and failure-clearing tests;
- concurrent sign/verify/destroy lease tests;
- OpenSSL test-only differential sign/verify validation;
- exact AnoPKI CSR proof-of-possession, certificate, CRL, and OCSP TBS-byte
  compatibility cases;
- current and P1-29 consumers, install/export, PoC, symbol-version, no-OpenSSL
  production dependency, and full local CTest regression.

### Later P-256 ECDSA

- NIST P-256 signature-generation/verification vectors;
- scalar/point validation and `Q = dG` negatives;
- raw `r || s` boundary and AnoPKI DER round-trip evidence;
- OpenSSL test-only differential and exact certificate/CRL/OCSP fixtures;
- nonce, low-`s`, concurrency, lifecycle, consumer, ABI, install, and PoC gates.

### Restricted SHA-1

The current OCSP CertID default need is reconfirmed, but its misuse-resistant
public API policy is separate. It is not selected for implementation by P3-07.
No SHA-1 issuance or signature service is approved.

## Gate outcome

P3-07 is complete. The architecture is approved for a separate P3-08 API/ABI
review and implementation slice:

`P3-08: RSA PKCS#1 v1.5 SHA-256/384/512 and RSA key-handle import`

P3-08 must review exact names, prototypes, key-size policy, bounded storage, KAT
sources, symbol count, and any additive ABI line before changing public code.
