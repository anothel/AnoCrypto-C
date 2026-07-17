# KCMVP Claims and Status Summary

| Area | Current status |
|---|---|
| Product | `AnoCrypto-C` C99 cryptographic module candidate SDK |
| Engineering version | `0.1.0-p2-10-assisted-rehearsal` |
| Public boundary | 14 headers, 69 symbols, `ANOCRYPTO_C_0` |
| Candidate environment | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| P2 internal readiness sequence | `COMPLETE_LOCAL_CLOSEOUT` |
| P2-10 assisted rehearsal | `COMPLETE_LOCAL_ASSISTED` |
| Independent human live rehearsal | `NOT_RUN` |
| Institution-contact decision | `CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION` |
| Runtime integrity | `NOT_IMPLEMENTED` |
| Exact OE-01 evidence | `NOT_RUN` |
| External consultation | `NOT_CONDUCTED` |
| Institution response | `NOT_RECEIVED` |
| Formal institution test/validation | `NOT_STARTED` |
| P1-34 actual hosted execution | `BLOCKED_EXTERNAL`, `NOT_RUN`, `UNREVIEWED` |
| Repository signing | `UNSIGNED` |
| Offline signing handoff | `BLOCKED` |
| Submission authorization | `NO` |
| External release authorization | `NO` |

## Permitted wording

- KCMVP-oriented candidate module.
- KCMVP validation-readiness engineering work completed locally through P2 closeout.
- Controlled internal pre-consultation and assisted-rehearsal packs prepared.
- Source-backed assisted desk rehearsal completed; named human rehearsal remains pending.
- Local engineering tests and evidence passed with stated environment and limitations.
- External confirmation, institution test, formal validation, and release authorization remain pending.

## Prohibited wording

- KCMVP validated, certified, approved, institution-tested, or independently rehearsed.
- Claims of completed production suitability, completed independent audit, formal security assurance, or impossibility of misuse.
- Universal constant-time or side-channel resistance.
- Actual Windows/macOS/hosted success when jobs did not run.
- Runtime module integrity completed.
- Entropy source formally accepted.
- Repository or release signed.
- Submission or customer release approved.

## Implemented candidate services

- lifecycle, startup engineering self-tests, sticky error state, build/version identity;
- SHA-256/384/512;
- HMAC-SHA-256/384/512;
- HMAC_DRBG-SHA-256 and Linux OS-provider-backed random service;
- AES-128/192/256 primitive and CBC without padding;
- AES-CMAC full 16-byte tag;
- in-memory purpose-separated HMAC, block-cipher, and CMAC CSP handles.

The list is an engineering implementation inventory, not an accepted validation scope.

## Deferred or excluded

- AES-CTR/GCM/GMAC and generic AEAD;
- KDF and public-key APIs;
- key persistence, export, wrapping, platform keystore, and multi-process sharing;
- full PKI or application security workflows;
- hardware acceleration/dispatch;
- automatic VM clone/snapshot or suspend/resume freshness handling;
- production signing, timestamp, external submission, and release.
