# AES-CMAC CI Checklist

Status: P1-28 engineering policy checklist.

The P1-28 lanes must confirm:

- public `ac_cmac.h` contains exactly eight reviewed functions;
- `ANO_CRYPTO_KEY_TYPE_CMAC` is distinct from the AES encryption purpose;
- startup and extended NIST example KATs pass;
- negative and cross-purpose CSP tests pass;
- OpenSSL 3.x is linked only into the differential test executable;
- install and customer PoC smoke compute and verify a full tag without printing it;
- internal CMAC context, source, tests, and provider artifacts are not packaged;
- the clean Release ABI-0 baseline contains exactly 69 names;
- the P1-27 to P1-28 symbol delta is the eight reviewed CMAC functions only;
- key, message, tag, subkey, and intermediate values are absent from logs/evidence;
- claims and clean-room policy checks continue to pass.
