# KCMVP Public Procedure Source Register

## Control status

| Field | Value |
|---|---|
| Policy ID | `CCR-P2-12-R1` |
| Checked date | `2026-07-17` |
| Status | `PUBLIC_SOURCE_REFRESH_COMPLETE` |
| Official-form conformity | `EXTERNAL_CONFIRMATION_REQUIRED` |
| External consultation | `NOT_CONDUCTED` |

This register records public sources used to prepare contact readiness. It does not replace the current institution instructions, a contract, or an institution response.

## Controlled public sources

| ID | Owner | Public source | Observation used by AnoCrypto-C | Refresh rule |
|---|---|---|---|---|
| PS-01 | KISA | `https://seed.kisa.or.kr/kisa/kcmvp/EgovSummary.do` | Lists current public and private test institutions, contact channels, and KS X ISO/IEC 19790:2015 / 24759:2015 as the stated test and validation standards. | Recheck immediately before contact. |
| PS-02 | KISA | `https://seed.kisa.or.kr/kisa/kcmvp/EgovProcedure.do` | Applicant selects a security level, prepares submissions, applies to a test institution, and may be asked for explanations, test-environment support, automated algorithm results, and development-environment security information. | Recheck immediately before contact or submission. |
| PS-03 | KISA | `https://seed.kisa.or.kr/kisa/kcmvp/reference.do` | Latest listed submission-writing guide is the September 2025 revision. | Download and review the then-current attachment before any formal preparation. |
| PS-04 | KCMVP service | `https://www.kcmvp.or.kr/` | Provides validation-management and pre-validation service entry points. | Confirm account, access, scope, and terms separately. |
| PS-05 | KISA | `https://www.kisa.or.kr/402/form?postSeq=2336` | Describes online management and self-service pre-validation for algorithm conformance and entropy evaluation. | Treat as public service description, not proof of project eligibility or test acceptance. |

## Freshness and authority rules

1. Public webpages can change after this controlled snapshot.
2. An email address or phone number in this register is not permission to transmit internal material.
3. A public pre-validation service does not replace a test-institution agreement or formal validation.
4. Institution-specific instructions received later supersede this internal source interpretation only after they are recorded with date, sender role, scope, and disposition.
5. Source code, binaries, credentials, keys, raw entropy, DRBG state, plaintext, ciphertext, IVs, MACs/tags, signatures, timestamps, and customer data are excluded from the first contact.
