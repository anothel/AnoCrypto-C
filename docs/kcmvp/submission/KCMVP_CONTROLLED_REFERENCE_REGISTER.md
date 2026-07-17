# Controlled Reference Register

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-RF-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Clean-room/compliance lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. External public references

| Ref ID | Reference | Revision/date | Use | Status |
|---|---|---|---|---|
| EXT-KISA-001 | KISA KCMVP 시험·검증절차 public page | checked 2026-07-17 | process and claims boundary | Public reference; exact institution procedure remains confirmable. |
| EXT-KISA-002 | 암호모듈 제출물 작성 안내서 개정 | 2025.9, posted 2025-09-17 | document-category and preparation baseline | Latest publicly listed at P2-07 check; exact accepted form/version `EXTERNAL_CONFIRMATION_REQUIRED`. |
| EXT-KS-001 | KS X ISO/IEC 19790:2015 | controlled licensed copy required | security requirements | Full text not copied into repository. |
| EXT-KS-002 | KS X ISO/IEC 24759:2015 | controlled licensed copy required | test requirements | Full text not copied into repository. |
| EXT-LINUX-001 | Linux `getrandom(2)` documentation | target OE revision required | external conditioned-provider interface | Exact kernel/OE behavior evidence pending. |
| EXT-NIST-001 | NIST SP 800-90A/B family | current controlled copies required | DRBG/noise-source terminology and analysis input | Applicability/acceptance remains external. |

## 2. Public test-vector references

Public algorithm vector sources are tracked through `cleanroom/SOURCE_MANIFEST.md` and algorithm KAT plans. Each imported vector set must retain source, version, license/usage basis, normalization method and local fixture hash.

## 3. Internal controlled references

- P2-02 module boundary and operating environment records.
- P2-03 self-test, integrity candidate, finite-state and recovery records.
- P2-04 entropy/provider/DRBG specification and responsibility matrices.
- P2-05 CSP inventory, service lifecycle and zeroization evidence.
- P2-06 secure-coding, side-channel, findings and compiler/CPU records.
- ABI baseline, public-header manifest, release identity and evidence/provenance policies.

## 4. Clean-room and copyright rule

Do not copy full licensed standards, proprietary institution templates, third-party source implementations or restricted test assets into this repository. Record identifiers, revisions, access authority, hash and derived requirement IDs in controlled storage.
