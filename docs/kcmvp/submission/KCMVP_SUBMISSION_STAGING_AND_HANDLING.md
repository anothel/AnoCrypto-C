# Submission Staging and Handling

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-SH-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Security operations lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Staging levels

| Level | Contents | Authorization |
|---|---|---|
| S0 — Internal skeleton | Files in `release/ANOCRYPTO_C_SUBMISSION_STAGING_MANIFEST.txt` | Internal review only |
| S1 — Consultation pack | Approved question set, selected skeleton excerpts, non-sensitive status evidence | Named internal authorization plus approved recipient/channel |
| S2 — Test application package | Institution-required documents/source/binary/evidence | `EXTERNAL_CONFIRMATION_REQUIRED`; separate manifest and approval |
| S3 — Production/customer release | Product package, signatures, timestamp and customer terms | Currently prohibited; release authorization `NO` |

## 2. Routine S0 exclusions

Do not automatically stage:

- product source or private internal headers;
- binaries, object files or disassembly;
- OpenSSL differential code/provider;
- deterministic entropy/failure hooks and signing fixtures;
- raw CTest/evidence trees or local absolute paths;
- keys, tokens, entropy, seed, DRBG state/output, IV, plaintext/ciphertext, digest/MAC/CMAC values;
- integrity key/tag material;
- production signatures/timestamp responses;
- customer or internal deployment data.

## 3. Protected expansion

Source, binary, compiler output, detailed evidence or institution forms may be added only after:

1. recipient and purpose are recorded;
2. exact requirement and delivery channel are confirmed;
3. sensitive-content scan passes;
4. a new package manifest and SHA-256 file are generated;
5. independent reviewer and release authority approve;
6. transfer receipt and retention/destruction responsibilities are recorded.

## 4. Package binding

Every staged package must contain or reference:

- package ID and revision;
- source commit and engineering version;
- included-file manifest with SHA-256;
- classification and recipient;
- owner/reviewer/approver/date;
- excluded-material statement;
- open gaps and claims boundary;
- transfer/receipt record when externally delivered.

## 5. Current blockers

- approved institution secure-delivery channel: `EXTERNAL_CONFIRMATION_REQUIRED`;
- P1-34 actual hosted artifacts: `BLOCKED_EXTERNAL`, zero artifacts;
- production signer/TSA: `UNPROVISIONED`;
- submission/external release authorization: `NO`.
