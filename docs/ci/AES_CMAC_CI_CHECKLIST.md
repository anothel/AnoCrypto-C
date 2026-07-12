# AES-CMAC CI Checklist

Status: P1-29 engineering policy checklist.

The retained implementation lanes and P1-29 hardening lanes must confirm:

- public `ac_cmac.h` contains exactly eight reviewed functions;
- `ANO_CRYPTO_KEY_TYPE_CMAC` is distinct from the AES encryption purpose;
- startup and extended NIST example KATs pass;
- negative and cross-purpose CSP tests pass;
- OpenSSL 3.x is linked only into the differential test executable;
- install and customer PoC smoke compute and verify a full tag without printing it;
- internal CMAC context, source, tests, and provider artifacts are not packaged;
- the clean Release ABI-0 baseline contains exactly 69 names;
- the P1-27 to P1-28 symbol delta remains the eight reviewed CMAC functions
  only, and P1-28 to P1-29 has no public symbol delta;
- context integrity, failed reinitialization, alias, short/oversized cleanup, and
  output-zeroization regressions pass;
- lifecycle stress completes 128 iterations and differential validation reports
  1,728 CMAC comparisons;
- native-thread concurrency covers per-thread and shared read-only CMAC handles;
- compiler-output capture remains a manual target-specific review aid;
- key, message, tag, subkey, and intermediate values are absent from logs/evidence;
- claims and clean-room policy checks continue to pass.

See `docs/ci/CMAC_HARDENING_CI_CHECKLIST.md` for the P1-29-specific lanes and artifact interpretation boundary.
