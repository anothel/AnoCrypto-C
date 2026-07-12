# CMAC Hardening CI Checklist

Status: P1-29 engineering checklist.

The P1-29 lanes should confirm:

- all configured CTest cases pass;
- `ano_crypto_cmac_api_negative_tests` covers fail-closed reinit, aliasing,
  failure output clearing, bounded cleanup, and cross-purpose rejection;
- `ano_crypto_aes_cmac_hardening_review` passes without printing values;
- `ano_crypto_aes_cmac_lifecycle_stress_tests` completes 128 lifecycles;
- OpenSSL 3.x remains test-only and reports 1,728 CMAC comparison paths;
- per-thread and shared CMAC-purpose handle concurrency passes where supported;
- ASan/UBSan diagnostics remain clean in the selected lane;
- `ano_crypto_cmac_hardening_policy_check` passes;
- the clean Release exported-symbol baseline remains 69 names;
- install and PoC smoke tests exclude private CMAC headers/source/tests,
  OpenSSL provider artifacts, and compiler disassembly;
- evidence output contains status and metadata only.

Compiler-output capture should run for reviewed Release-style compiler lanes.
A missing disassembler may be reported as `NOT_CAPTURED` only where the lane
does not require capture.

Workflow presence is not evidence that Windows, macOS, 32-bit, big-endian, or
compiler-output artifacts were executed or reviewed. Actual run identifiers,
hashes, toolchains, dates, limitations, and reviewer disposition must be
recorded separately.
