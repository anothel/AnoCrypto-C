# AnoCrypto-C GPT Project Source Replacement Plan

Status: `P3_09_COMPLETE_P3_10_ENTRY_REFRESH_REQUIRED`

Older P1/P2, P3-01/P3-02, and P3-07/P3-08 CURRENT ONLY packs are historical and
must not remain active beside the next replacement pack.

## Current artifact roles

| Artifact | Repository role | Identity |
|---|---|---|
| AnoCrypto-C P3-09 ECDSA source | current core source | `0.1.0-p3-09-ecdsa-p256-sha256`; 17 headers / 84 symbols / ABI-6 |
| AnoCrypto-C P3-08 RSA source | retained prior implementation baseline | 17 headers / 83 symbols / ABI-5 |
| AnoSecureKit Enterprise P3-06 source | downstream parity/no-fallback evidence | not an AnoCrypto-C core replacement |
| exact AnoPKI sources | P3-10 OCSP CertID requirement authority | keep exact revisions and manifests |

## Next replacement pack

Create one `P3-09 Closeout / P3-10 Entry CURRENT ONLY` pack that:

- identifies P3-09 as complete;
- freezes 17 headers, 84 symbols, and ABI lines 0 through 6;
- records RSA and ECDSA core completion with AnoPKI adapter parity pending;
- selects only the restricted SHA-1 OCSP CertID API/ABI gate;
- pre-approves no new function, enum, symbol count, or ABI line;
- preserves all hosted, provenance, signing, KCMVP, and release blockers.

## Authority order

1. exact current source ZIP and committed manifests;
2. newest CURRENT ONLY pack;
3. exact downstream source revisions;
4. completed patches/evidence;
5. historical documents.
