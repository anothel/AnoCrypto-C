# CMAC Entry Gate CI Checklist

Status: P1-27 engineering policy checklist. Workflow presence is not implementation evidence.

The P1-27 policy lane must confirm:

- the CMAC selection, API gate, KAT/evidence plan, and release review template
  are present;
- `include/anocrypto/ac_cmac.h` is absent;
- CMAC implementation and KAT source files are absent;
- `ANO_CRYPTO_KEY_TYPE_CMAC` is not yet present in the public enum;
- no `ano_crypto_cmac_*` public symbol exists;
- AES-CTR has not been added through an unreviewed mode enum or function;
- the stored clean Release ABI-0 baseline remains exactly 61 names;
- install and PoC packages contain the decision note but no internal CMAC
  implementation or fixture;
- claims and sensitive-output policy checks continue to pass.

A later implementation lane must replace the absence checks with KAT,
negative, differential, install, PoC, symbol-delta, and evidence checks.
