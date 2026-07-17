# AnoPKI Community Collection Request

Provide a clean ZIP and CURRENT ONLY pack from the exact revision consumed by
AnoPKI Enterprise.

Required evidence:

- repository/branch/commit/tag and Community version;
- backend-neutral PKI operation contracts;
- OpenSSL adapter implementation;
- CSR, certificate, CRL and OCSP code paths;
- algorithm/profile policy and key-provider boundary;
- positive, negative and golden fixtures;
- service/CLI/API compatibility tests;
- document index and current/active/next queue.

The collection must allow product workflow and cryptographic primitive work to
be separated. Do not copy OpenSSL call lists as an AnoCrypto-C API proposal.
