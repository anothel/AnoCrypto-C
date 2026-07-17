# AnoCrypto-C P1-33 retained AES-CMAC hardening policy check.
# This is an engineering guardrail, not formal validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT)
  set(ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT
      "${ANO_CRYPTO_BUILD_DIR}/policy-check/AES_CMAC_HARDENING_CHECK.txt")
endif()

get_filename_component(_out_dir "${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
set(_violations "")

function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "required file missing: ${rel}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()

function(require_text rel text)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "required file missing: ${rel}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" _content)
    string(FIND "${_content}" "${text}" _pos)
    if(_pos EQUAL -1)
      list(APPEND _violations "required marker missing from ${rel}: ${text}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

foreach(_file IN ITEMS
    include/anocrypto/ac_cmac.h
    src/internal/ac_aes_internal.h
    src/internal/ac_cmac_internal.h
    src/mac/cmac.c
    tests/negative/cmac_api_negative_tests.c
    tests/operational/aes_cmac_hardening_review.c
    tests/stress/aes_cmac_lifecycle_stress_tests.c
    tests/differential/aes_cmac_openssl_differential_tests.c
    tests/unit/csp_thread_safety_tests.c
    tools/capture_cmac_compiler_review.cmake
    docs/design/AES_CMAC_HARDENING_POLICY.md
    docs/kcmvp/CMAC_COMPILER_OUTPUT_REVIEW_PLAN.md
    docs/ci/CMAC_HARDENING_CI_CHECKLIST.md
    evidence/release-template/AES_CMAC_HARDENING_REVIEW.md
    abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
  require_file("${_file}")
endforeach()

require_text("include/anocrypto/ac_version.h" "0.1.0-p2-05-csp-zeroization")
require_text("src/mac/cmac.c" "One-shot cleanup is unconditional")
require_text("src/mac/cmac.c" "failed reinitialization is")
require_text("src/mac/cmac.c" "ano_crypto_cmac_ranges_overlap")
require_text("src/internal/ac_aes_internal.h" "ano_crypto_internal_aes_context_is_valid")
require_text("src/internal/ac_cmac_internal.h" "ano_crypto_internal_cmac_tags_equal")
require_text("tests/operational/aes_cmac_hardening_review.c" "values_not_printed=1")
require_text("tests/stress/aes_cmac_lifecycle_stress_tests.c" "CMAC_LIFECYCLE_ITERATIONS 128u")
require_text("tests/differential/aes_cmac_openssl_differential_tests.c" "CMAC_DIFFERENTIAL_CASES_PER_KEY 96u")
require_text("tests/differential/aes_cmac_openssl_differential_tests.c" "CMAC_DIFFERENTIAL_PATHS_PER_CASE 6u")
require_text("tests/unit/csp_thread_safety_tests.c" "shared_cmac_handle")
require_text("tools/capture_cmac_compiler_review.cmake" "Manual review required: yes")
require_text("docs/design/AES_CMAC_HARDENING_POLICY.md" "No public API widening")
require_text("docs/design/AES_CMAC_HARDENING_POLICY.md" "loaded library")
require_text("include/anocrypto/ac_cmac.h" "message input and the supplied tag must also")
require_text("docs/design/AES_CMAC_HARDENING_POLICY.md" "supplied tag storage to be disjoint")
require_text("tests/negative/cmac_api_negative_tests.c" "uninit zero-length nonnull")
require_text("docs/ci/CMAC_HARDENING_CI_CHECKLIST.md" "Workflow presence is not evidence")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cmac.h" _header)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_cmac_[A-Za-z0-9_]+" _api_matches "${_header}")
list(LENGTH _api_matches _api_count)
if(NOT _api_count EQUAL 8)
  list(APPEND _violations "public CMAC API must remain exactly 8 functions; found ${_api_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
list(FILTER _symbols EXCLUDE REGEX "^[ \t]*$")
list(LENGTH _symbols _symbol_count)
if(NOT _symbol_count EQUAL 69)
  list(APPEND _violations "ABI-0 exported-symbol baseline must remain 69; found ${_symbol_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/mac/cmac.c" _cmac_source)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cmac.h" _cmac_header)
if(_cmac_source MATCHES "openssl/|EVP_MAC|OpenSSL::Crypto" OR
   _cmac_header MATCHES "openssl/|EVP_MAC|OpenSSL::Crypto")
  list(APPEND _violations "product CMAC implementation or public header depends on OpenSSL")
endif()

file(WRITE "${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}"
  "AnoCrypto-C P1-33 retained AES-CMAC hardening policy check\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "AES-CMAC hardening policy check failed; see ${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_CMAC_HARDENING_CHECK_OUTPUT}"
  "Result: PASS\n"
  "Public CMAC API functions: 8\n"
  "Public export baseline: 69 symbols\n"
  "Lifecycle stress iterations: 128\n"
  "Differential corpus: 96 cases per key size, 6 checked paths per case, 1728 comparisons\n"
  "Concurrency: per-thread and shared CMAC-purpose CSP handles\n"
  "Compiler-output interpretation: manual review required\n"
  "Sensitive values in policy output: none\n")

message(STATUS "AnoCrypto-C AES-CMAC hardening policy check passed")
