# AnoPKI RSA PKCS#1 v1.5 Compatibility

Status: `P3_08_PRIMITIVE_COMPATIBILITY_COMPLETE_ADAPTER_PENDING`

AnoPKI remains responsible for parsing PEM/DER/ASN.1 key containers, extracting
algorithm-native RSA components, constructing CertificationRequestInfo,
TBSCertificate, TBSCertList, and OCSP response bytes, choosing PKI identifiers,
and embedding signatures.

P3-08 provides the generic primitive beneath that boundary:

- import RSA-2048 public/private components with `e=65537`;
- message-based SHA-256/384/512 PKCS#1 v1.5 signing;
- strict verification through public or private handles.

Test-only OpenSSL differential evidence covers the three digest variants across
CSR proof-of-possession, certificate issuance, CRL signing, OCSP response
signing, and responder-certificate verification message labels. Core-produced
signatures verify in OpenSSL, OpenSSL-produced signatures verify in core, and
deterministic PKCS#1 v1.5 signature bytes are identical.

AnoPKI Enterprise adapter composition, issuer-certificate/key binding,
`key_ref`, provider routing, HSM/KMS/PKCS#11, authorization, evidence, and audit
remain separate downstream work. No fallback is introduced by P3-08.
