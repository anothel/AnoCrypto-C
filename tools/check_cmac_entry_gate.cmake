cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT)
  set(ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT
      "${ANO_CRYPTO_BUILD_DIR}/policy-check/CMAC_ENTRY_GATE_CHECK.txt")
endif()

get_filename_component(_out_dir "${ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
set(_violations "")

function(require_file path)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${path}")
    list(APPEND _violations "missing ${path}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()

function(require_text path marker)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${path}")
    list(APPEND _violations "missing ${path}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${path}" _content)
    string(FIND "${_content}" "${marker}" _index)
    if(_index EQUAL -1)
      list(APPEND _violations "${path} missing marker: ${marker}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

foreach(_path IN ITEMS
    docs/design/CMAC_SELECTION_DECISION.md
    docs/design/C99_CMAC_API_GATE.md
    docs/kcmvp/CMAC_KAT_AND_EVIDENCE_PLAN.md
    docs/ci/CMAC_ENTRY_GATE_CI_CHECKLIST.md
    evidence/release-template/CMAC_ENTRY_GATE_REVIEW.md
    docs/roadmap/C99_PRIORITY_TODO.md
    include/anocrypto/ac_version.h
    tools/create_poc_package.cmake
    tools/run_poc_package_smoke.cmake
    tools/collect_release_evidence.cmake
    .github/workflows/ci.yml
    abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
  require_file("${_path}")
endforeach()

require_text("docs/design/CMAC_SELECTION_DECISION.md" "narrow AES-CMAC service rather than AES-CTR")
require_text("docs/design/CMAC_SELECTION_DECISION.md" "dedicated CMAC key purpose")
require_text("docs/design/C99_CMAC_API_GATE.md" "ano_crypto_cmac_verify_with_key")
require_text("docs/design/C99_CMAC_API_GATE.md" "61-symbol ABI-0 baseline")
require_text("docs/kcmvp/CMAC_KAT_AND_EVIDENCE_PLAN.md" "SP 800-38B")
require_text("docs/ci/CMAC_ENTRY_GATE_CI_CHECKLIST.md" "Workflow presence is not implementation evidence")
require_text("include/anocrypto/ac_version.h" "0.1.0-p1-27-cmac-entry-gate")
require_text("CMakeLists.txt" "ano_crypto_cmac_entry_gate_check")
require_text("tools/collect_release_evidence.cmake" "CMAC_ENTRY_GATE_CHECK.txt")
require_text("tools/create_poc_package.cmake" "CMAC_ENTRY_GATE_PACKAGE_NOTES.txt")
require_text("tools/run_poc_package_smoke.cmake" "CMAC_ENTRY_GATE_PACKAGE_NOTES.txt")
require_text(".github/workflows/ci.yml" "ano_crypto_cmac_entry_gate_check")

foreach(_forbidden IN ITEMS
    include/anocrypto/ac_cmac.h
    src/mac/cmac.c
    src/internal/ac_cmac_internal.h
    tests/kat/cmac_kat_tests.c
    tests/negative/cmac_api_negative_tests.c
    tests/differential/cmac_openssl_differential_tests.c)
  if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_forbidden}")
    list(APPEND _violations "P1-27 gate unexpectedly contains implementation artifact: ${_forbidden}")
  endif()
endforeach()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_key.h" _key_header)
if(_key_header MATCHES "ANO_CRYPTO_KEY_TYPE_CMAC")
  list(APPEND _violations "P1-27 gate must not add ANO_CRYPTO_KEY_TYPE_CMAC before implementation review")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cipher.h" _cipher_header)
if(_cipher_header MATCHES "ANO_CRYPTO_CIPHER_MODE_CTR|ano_crypto_cipher_.*ctr")
  list(APPEND _violations "P1-27 selected CMAC; unreviewed CTR public API or mode was found")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/CMakeLists.txt" _cmake_content)
if(_cmake_content MATCHES "include/anocrypto/ac_cmac\\.h|src/(mac|cipher)/cmac\\.c")
  list(APPEND _violations "P1-27 CMake unexpectedly wires a CMAC public header or implementation source")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _trimmed)
  if(NOT _trimmed STREQUAL "" AND NOT _trimmed MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count} + 1")
    if(_trimmed MATCHES "^ano_crypto_cmac_")
      list(APPEND _violations "P1-27 baseline unexpectedly exports ${_trimmed}")
    endif()
  endif()
endforeach()
if(NOT _symbol_count EQUAL 61)
  list(APPEND _violations "P1-27 CMAC gate must retain the 61-symbol baseline; found ${_symbol_count}")
endif()

file(WRITE "${ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT}"
  "AnoCrypto-C P1-27 CMAC entry gate policy check\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT}" "Result: FAIL\n")
  foreach(_violation IN LISTS _violations)
    file(APPEND "${ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT}" "- ${_violation}\n")
  endforeach()
  message(FATAL_ERROR "CMAC entry gate policy check failed")
endif()

file(APPEND "${ANO_CRYPTO_CMAC_ENTRY_GATE_CHECK_OUTPUT}"
  "Result: PASS\n"
  "Selected next candidate: AES-CMAC\n"
  "Deferred mode: AES-CTR\n"
  "Public CMAC headers/functions: 0\n"
  "CMAC implementation/test source: 0\n"
  "Dedicated CMAC key purpose: planned, not yet added\n"
  "Public export baseline: 61 symbols\n"
  "Sensitive values in policy output: none\n")
message(STATUS "AnoCrypto-C CMAC entry gate policy check passed")
