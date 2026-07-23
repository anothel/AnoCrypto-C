# Next Work Prompt — P3-11 AnoPKI Enterprise Parity

Status: `COMPLETE_HISTORICAL_DO_NOT_RUN`

P3-11 completed at AnoPKI Enterprise commit `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec` and was closed by P3-12. This prompt is retained only as a historical execution record. Do not run it as current work.

Continue from AnoCrypto-C `0.1.0-p3-10-restricted-sha1-ocsp-certid` with 18
public headers, 85 symbols, and ABI lines `ANOCRYPTO_C_0` through
`ANOCRYPTO_C_7`.

Task: connect AnoPKI Enterprise RSA-2048 PKCS#1 v1.5 SHA-2, ECDSA P-256
SHA-256, and restricted OCSP CertID SHA-1 through public AnoCrypto-C APIs.

Hard boundaries:

- do not move PEM/DER/ASN.1/CSR/X.509/CRL/OCSP construction into AnoCrypto-C;
- do not add a generic SHA-1 service or use SHA-1 for signatures;
- keep ECDSA DER encoding in AnoPKI;
- keep `key_ref`, provider routing, authorization, evidence, retry, and audit in
  Enterprise;
- fail closed and never silently fall back to OpenSSL;
- do not change AnoCrypto-C public API/ABI in this checkpoint;
- keep hosted, provenance, signing, release, and KCMVP states unchanged;
- do not push, tag, sign, timestamp, or distribute.

Required evidence includes exact CSR proof-of-possession, certificate, CRL,
OCSP response, responder validation, OCSP CertID issuer hashes, algorithm/key
mismatch, wrong signature, unsupported capability, no-output-on-failure,
capability-mask truthfulness, and platform dependency inspection.
