cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
  set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT)
  set(ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/AES_HARDENING_CHECK.txt")
endif()
get_filename_component(_out_dir "${ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
set(_violations "")

function(require_file p)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${p}")
    list(APPEND _violations "missing ${p}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()
function(require_text p n)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${p}")
    list(APPEND _violations "missing ${p}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${p}" _c)
    string(FIND "${_c}" "${n}" _i)
    if(_i EQUAL -1)
      list(APPEND _violations "${p} missing marker: ${n}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()

foreach(_p IN ITEMS
  src/cipher/aes.c
  src/cipher/cipher.c
  src/internal/ac_aes_internal.h
  tests/operational/aes_cbc_hardening_review.c
  tests/stress/aes_cbc_lifecycle_stress_tests.c
  tests/differential/aes_cbc_openssl_differential_tests.c
  tests/unit/csp_thread_safety_tests.c
  tools/capture_aes_compiler_review.cmake
  docs/design/AES_CBC_HARDENING_POLICY.md
  docs/kcmvp/AES_COMPILER_OUTPUT_REVIEW_PLAN.md
  docs/ci/AES_HARDENING_CI_CHECKLIST.md
  evidence/release-template/AES_HARDENING_REVIEW.md
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt)
  require_file("${_p}")
endforeach()

require_text("src/internal/ac_aes_internal.h" "ANO_CRYPTO_INTERNAL_AES_CTX_MAGIC")
require_text("src/internal/ac_aes_internal.h" "round_key_size")
require_text("src/cipher/aes.c" "failed reinitialization")
require_text("src/cipher/cipher.c" "reinitialization is fail-closed")
require_text("tests/differential/aes_cbc_openssl_differential_tests.c" "AES_DIFFERENTIAL_CASES_PER_KEY 96u")
require_text("tests/operational/aes_cbc_hardening_review.c" "values_not_printed=1")
require_text("tests/stress/aes_cbc_lifecycle_stress_tests.c" "AES_LIFECYCLE_ITERATIONS 128u")
require_text("tools/capture_aes_compiler_review.cmake" "Manual review required: yes")
require_text("docs/design/AES_CBC_HARDENING_POLICY.md" "No public API widening")
require_text("docs/ci/AES_HARDENING_CI_CHECKLIST.md" "Workflow presence is not evidence")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_cipher.h" _cipher_header)
string(REGEX MATCHALL "ANO_CRYPTO_API" _api_markers "${_cipher_header}")
list(LENGTH _api_markers _api_count)
if(NOT _api_count EQUAL 8)
  list(APPEND _violations "ac_cipher.h public API marker count changed from reviewed value 8 to ${_api_count}")
endif()

file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt" _symbols)
set(_symbol_count 0)
foreach(_line IN LISTS _symbols)
  string(STRIP "${_line}" _trimmed)
  if(NOT _trimmed STREQUAL "" AND NOT _trimmed MATCHES "^#")
    math(EXPR _symbol_count "${_symbol_count} + 1")
  endif()
endforeach()
if(NOT _symbol_count EQUAL 61)
  list(APPEND _violations "ABI-0 exported-symbol baseline must remain 61 for P1-26; found ${_symbol_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/cipher/aes.c" _aes_source)
if(_aes_source MATCHES "sbox_table|SBOX_TABLE|\\[[ ]*256[ ]*\\]")
  list(APPEND _violations "AES source contains a table-shaped S-box artifact")
endif()

file(WRITE "${ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT}"
  "AnoCrypto-C P1-26 AES/CBC hardening policy check\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT}" "Result: FAIL\n")
  foreach(v IN LISTS _violations)
    file(APPEND "${ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT}" "- ${v}\n")
  endforeach()
  message(FATAL_ERROR "AES/CBC hardening policy check failed")
endif()
file(APPEND "${ANO_CRYPTO_AES_HARDENING_CHECK_OUTPUT}"
  "Result: PASS\n"
  "Public cipher API functions: 8\n"
  "Public export baseline: 61 symbols\n"
  "Differential corpus: 96 deterministic cases per AES key size, one-shot/streaming/in-place encrypt/decrypt\n"
  "Lifecycle stress iterations: 128\n"
  "Compiler-output interpretation: manual review required\n"
  "Sensitive values in policy output: none\n")
message(STATUS "AnoCrypto-C AES/CBC hardening policy check passed")
