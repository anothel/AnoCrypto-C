# AnoCrypto-C P1-33 retained AES-CMAC implementation policy check.
# This is an engineering guardrail, not formal validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_CMAC_CHECK_OUTPUT)
  set(ANO_CRYPTO_CMAC_CHECK_OUTPUT
      "${ANO_CRYPTO_BUILD_DIR}/policy-check/AES_CMAC_IMPLEMENTATION_CHECK.txt")
endif()

get_filename_component(_out_dir "${ANO_CRYPTO_CMAC_CHECK_OUTPUT}" DIRECTORY)
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
    src/internal/ac_cmac_internal.h
    src/mac/cmac.c
    tests/kat/aes_cmac_kat_tests.c
    tests/negative/cmac_api_negative_tests.c
    tests/differential/aes_cmac_openssl_differential_tests.c
    docs/design/CMAC_SELECTION_DECISION.md
    docs/design/C99_CMAC_API_GATE.md
    docs/kcmvp/CMAC_KAT_AND_EVIDENCE_PLAN.md
    docs/ci/AES_CMAC_CI_CHECKLIST.md
    evidence/release-template/AES_CMAC_IMPLEMENTATION_REVIEW.md
    abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
  require_file("${_file}")
endforeach()

require_text("include/anocrypto/ac_version.h" "0.1.0-p2-08-preconsultation-rehearsal")
require_text("include/anocrypto/ac_key.h" "ANO_CRYPTO_KEY_TYPE_CMAC = 7")
require_text("include/anocrypto/anocrypto.h" "#include <anocrypto/ac_cmac.h>")
require_text("include/anocrypto/ac_cmac.h" "ANO_CRYPTO_CMAC_TAG_SIZE 16u")
require_text("src/mac/cmac.c" "ANO_CRYPTO_ERR_AUTHENTICATION_FAILED")
require_text("src/selftest/selftest.c" "ano_crypto_selftest_aes_cmac")
require_text("CMakeLists.txt" "tests/kat/aes_cmac_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/cmac_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/aes_cmac_openssl_differential_tests.c")
require_text("tools/create_poc_package.cmake" "AES_CMAC_PACKAGE_NOTES.txt")
require_text("tools/run_poc_package_smoke.cmake" "AES_CMAC_PACKAGE_NOTES.txt")
require_text("tools/collect_release_evidence.cmake" "AES_CMAC_IMPLEMENTATION_CHECK.txt")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cmac.h" _header)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_cmac_[A-Za-z0-9_]+" _api_matches "${_header}")
list(LENGTH _api_matches _api_count)
if(NOT _api_count EQUAL 8)
  list(APPEND _violations "public CMAC API must contain exactly 8 reviewed functions; found ${_api_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
list(FILTER _symbols EXCLUDE REGEX "^[ \t]*$")
list(LENGTH _symbols _symbol_count)
if(NOT _symbol_count EQUAL 69)
  list(APPEND _violations "ABI-0 exported-symbol baseline must contain 69 names; found ${_symbol_count}")
endif()
foreach(_symbol IN ITEMS
    ano_crypto_cmac_compute_with_key
    ano_crypto_cmac_context_size
    ano_crypto_cmac_final
    ano_crypto_cmac_init
    ano_crypto_cmac_mac_size
    ano_crypto_cmac_uninit
    ano_crypto_cmac_update
    ano_crypto_cmac_verify_with_key)
  list(FIND _symbols "${_symbol}" _symbol_index)
  if(_symbol_index EQUAL -1)
    list(APPEND _violations "ABI baseline missing reviewed CMAC symbol: ${_symbol}")
  endif()
endforeach()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/mac/cmac.c" _cmac_source)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cmac.h" _cmac_header)
if(_cmac_source MATCHES "openssl/|EVP_MAC|OpenSSL::Crypto" OR
   _cmac_header MATCHES "openssl/|EVP_MAC|OpenSSL::Crypto")
  list(APPEND _violations "product CMAC implementation or public header depends on OpenSSL")
endif()

file(WRITE "${ANO_CRYPTO_CMAC_CHECK_OUTPUT}"
  "AnoCrypto-C P1-33 retained AES-CMAC implementation policy check\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_CMAC_CHECK_OUTPUT}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${ANO_CRYPTO_CMAC_CHECK_OUTPUT}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "AES-CMAC implementation policy check failed; see ${ANO_CRYPTO_CMAC_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_CMAC_CHECK_OUTPUT}"
  "Result: PASS\n"
  "Implemented: AES-CMAC with AES-128/192/256 dedicated CSP keys\n"
  "Public API functions: 8\n"
  "Public export baseline: 69 symbols\n"
  "Tag policy: full 16-byte tags only\n"
  "Differential provider boundary: test-only\n"
  "Sensitive values in policy output: none\n")
