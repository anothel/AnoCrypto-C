# Historical Phase 1 execution prompts — retained source links

Status: `HISTORICAL_REFERENCE_ONLY / NOT_ACTIVE_WORK`

This index is a navigation record, **not a new implementation instruction**.
The Phase 1 P3-10 and P3-11 work is recorded as completed in
[P3 product roadmap](P3_PRODUCT_DRIVEN_ROADMAP.md),
[P3-10 implementation/test delta](../product/P3_10_IMPLEMENTATION_TEST_DELTA.md),
[P3-11 parity closeout](../product/P3_11_ANOPKI_ENTERPRISE_PARITY_CLOSEOUT.md),
and [P3-12 Phase 1 closeout](../product/P3_12_PHASE1_CLOSEOUT.md).

The following **completed next-work prompts** were removed from the active
working tree because the same instructions should not be re-run as current work.
Their exact text remains available in Git history at frozen
`main@4d67dab863ca21abb886cca2669f58ca5bea427c`:

| Historical instruction | Frozen original | Current handling |
| --- | --- | --- |
| P3-10 restricted SHA-1 / OCSP CertID | [Original P3-10 prompt](https://github.com/anothel/AnoCrypto-C/blob/4d67dab863ca21abb886cca2669f58ca5bea427c/docs/roadmap/P3_10_RESTRICTED_SHA1_OCSP_CERTID_NEXT_WORK_PROMPT.md) | P3-10 done; retained design, implementation and security policies remain authoritative within their own original scope |
| P3-11 AnoPKI Enterprise parity | [Original P3-11 prompt](https://github.com/anothel/AnoCrypto-C/blob/4d67dab863ca21abb886cca2669f58ca5bea427c/docs/roadmap/P3_11_ANOPKI_ENTERPRISE_PARITY_NEXT_WORK_PROMPT.md) | P3-11/P3-12 complete at their recorded historical versions; no automatic new downstream validation |

The older [P3-08 prompt](P3_08_RSA_PKCS1_V15_SHA2_NEXT_WORK_PROMPT.md)
is **retained in the working tree** because the historical
[P3-07 implementation/test delta](../product/P3_07_IMPLEMENTATION_TEST_DELTA.md)
explicitly references it. It is not the next authorized development task.

**Current decision boundary:** Engineering Phase 1 is closed. On 2026-11-21 the
owner selected strategic A (internal SDK stability) and authorized one limited
Linux/GCC/Release/shared core verification profile. This does not select or
approve any subsequent code implementation, downstream pin change, KCMVP,
customer release, or merge. Consult [the Master index](https://github.com/anothel/AnoCrypto-C/issues/15)
and [ACC-4](https://anothel.atlassian.net/browse/ACC-4) for later decisions.

**Retention:** Only the two obsolete instruction files are deleted from this
working tree. Their fixed-commit links, Git objects, prior CI/evidence and
upstream histories are not rewritten. This cleanup does **not** delete
security reviews, validation results, recorded approvals, ABI/source contracts,
or other historical evidence.
