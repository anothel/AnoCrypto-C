cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/HKDF_SHA256_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_HKDF_SHA256_POLICY.cmake")
set(_violations "")
function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    set(_violations "${_violations};missing ${rel}" PARENT_SCOPE)
  endif()
endfunction()
function(require_text rel text)
  file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" _content)
  string(FIND "${_content}" "${text}" _found)
  if(_found EQUAL -1)
    set(_violations "${_violations};${rel} missing ${text}" PARENT_SCOPE)
  endif()
endfunction()
foreach(_file IN ITEMS
  include/anocrypto/ac_kdf.h src/kdf/hkdf_sha256.c
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/hkdf_sha256_kat_tests.c tests/negative/hkdf_sha256_api_negative_tests.c
  tests/differential/hkdf_sha256_openssl_differential_tests.c
  docs/design/C99_HKDF_SHA256_API_GATE.md
  docs/integration/ANOSECUREKIT_SKF1_HKDF_COMPATIBILITY.md
  docs/product/P3_02_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/HKDF_SHA256_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
file(GLOB _headers "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(LENGTH _headers _header_count)
foreach(_pair IN ITEMS
  "abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt|69"
  "abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt|1"
  "abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt|70")
  string(REPLACE "|" ";" _parts "${_pair}")
  list(GET _parts 0 _path)
  list(GET _parts 1 _expected)
  file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/${_path}" _lines)
  set(_count 0)
  foreach(_line IN LISTS _lines)
    string(STRIP "${_line}" _line)
    if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
      math(EXPR _count "${_count}+1")
    endif()
  endforeach()
  if(NOT _count EQUAL _expected)
    list(APPEND _violations "${_path} count ${_count}, expected ${_expected}")
  endif()
endforeach()
if(NOT _header_count EQUAL 15 OR NOT ANO_CRYPTO_RELEASE_SYMBOL_COUNT STREQUAL "70" OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_1")
  list(APPEND _violations "current public identity mismatch")
endif()
if(NOT ANO_CRYPTO_HKDF_PUBLIC_FUNCTION_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_HKDF_MAX_OUTPUT_SIZE EQUAL 8160 OR
   NOT ANO_CRYPTO_HKDF_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_HKDF_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_HKDF_CURRENT_SYMBOL_COUNT EQUAL 70 OR
   NOT ANO_CRYPTO_HKDF_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_1")
  list(APPEND _violations "frozen HKDF API/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_HKDF_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_HKDF_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_HKDF_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_HKDF_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_HKDF_EXTERNAL_AUTHORIZATION STREQUAL "NO")
  list(APPEND _violations "retained external blocker/status values drifted")
endif()
require_text("include/anocrypto/ac_kdf.h" "ANO_CRYPTO_HKDF_SHA256_MAX_OUTPUT_SIZE 8160u")
require_text("include/anocrypto/ac_kdf.h" "ano_crypto_hkdf_sha256")
require_text("abi/anocrypto_c.map" "} ANOCRYPTO_C_0;")
require_text("docs/integration/ANOSECUREKIT_SKF1_HKDF_COMPATIBILITY.md" "${ANO_CRYPTO_HKDF_SKF1_INFO_HEX}")
require_text("CMakeLists.txt" "tests/kat/hkdf_sha256_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/hkdf_sha256_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/hkdf_sha256_openssl_differential_tests.c")
require_text("tools/create_poc_package.cmake" "HKDF_SHA256_PACKAGE_NOTES.txt")
require_text("tools/collect_release_evidence.cmake" "HKDF_SHA256_POLICY_CHECK.txt")

file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_kdf.h" _hkdf_header)
string(REGEX MATCHALL
  "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_hkdf_[A-Za-z0-9_]+"
  _hkdf_api_matches "${_hkdf_header}")
list(LENGTH _hkdf_api_matches _hkdf_api_count)
if(NOT _hkdf_api_count EQUAL 1)
  list(APPEND _violations "public HKDF API must contain exactly one reviewed function; found ${_hkdf_api_count}")
endif()

file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/kdf/hkdf_sha256.c" _hkdf_source)
if(_hkdf_source MATCHES "openssl/|EVP_KDF|OpenSSL::Crypto" OR
   _hkdf_header MATCHES "openssl/|EVP_KDF|OpenSSL::Crypto")
  list(APPEND _violations "product HKDF implementation or public header depends on OpenSSL")
endif()
if(_hkdf_source MATCHES "SKF1|anosecurekit file sealing v1" OR
   _hkdf_header MATCHES "SKF1|anosecurekit file sealing v1")
  list(APPEND _violations "product HKDF implementation contains AnoSecureKit format/workflow logic")
endif()
get_filename_component(_out_dir "${ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
file(WRITE "${ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_HKDF_POLICY_ID}\nHeaders: ${_header_count}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "HKDF-SHA-256 policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_HKDF_POLICY_CHECK_OUTPUT}"
  "Hosted: ${ANO_CRYPTO_HKDF_HOSTED_STATUS}/${ANO_CRYPTO_HKDF_HOSTED_REVIEW}\nSigning: ${ANO_CRYPTO_HKDF_REPOSITORY_SIGNING}\nExternal authorization: ${ANO_CRYPTO_HKDF_EXTERNAL_AUTHORIZATION}\nResult: PASS\n")
message(STATUS "HKDF-SHA-256 policy check passed")
