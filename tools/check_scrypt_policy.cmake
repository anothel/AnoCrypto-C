cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/SCRYPT_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_SCRYPT_POLICY.cmake")
set(_violations "")
function(require_file rel)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
    set(_violations "${_violations}" PARENT_SCOPE)
  endif()
endfunction()
function(require_text rel text)
  if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel}")
    list(APPEND _violations "missing ${rel}")
  else()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel}" _content)
    string(FIND "${_content}" "${text}" _found)
    if(_found EQUAL -1)
      list(APPEND _violations "${rel} missing ${text}")
    endif()
  endif()
  set(_violations "${_violations}" PARENT_SCOPE)
endfunction()
foreach(_file IN ITEMS
  include/anocrypto/ac_kdf.h src/kdf/scrypt.c
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_4_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/scrypt_kat_tests.c tests/negative/scrypt_api_negative_tests.c
  tests/differential/scrypt_openssl_differential_tests.c
  docs/design/C99_SCRYPT_FIXED_PROFILE_API_GATE.md
  docs/integration/ANOSECUREKIT_SKP1_SCRYPT_COMPATIBILITY.md
  docs/product/P3_05_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/SCRYPT_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_SCRYPT_PUBLIC_FUNCTION_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_SCRYPT_N EQUAL 32768 OR NOT ANO_CRYPTO_SCRYPT_R EQUAL 8 OR
   NOT ANO_CRYPTO_SCRYPT_P EQUAL 1 OR NOT ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE EQUAL 32 OR
   NOT ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE EQUAL 33556544 OR
   NOT ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE EQUAL 67108864 OR
   NOT ANO_CRYPTO_SCRYPT_ABI4_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_SCRYPT_CURRENT_SYMBOL_COUNT EQUAL 80 OR
   NOT ANO_CRYPTO_SCRYPT_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_4")
  list(APPEND _violations "frozen scrypt API/resource/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 18 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 85 OR
   NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69 OR NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_ABI4_SYMBOL_COUNT EQUAL 1 OR NOT ANO_CRYPTO_ABI5_SYMBOL_COUNT EQUAL 3 OR NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_7")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_SCRYPT_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_SCRYPT_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_SCRYPT_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_SCRYPT_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_SCRYPT_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_SCRYPT_DOWNSTREAM_IDENTITY_CONFLICT STREQUAL "OPEN_DOWNSTREAM_SYNC_DECISION")
  list(APPEND _violations "retained external status values drifted")
endif()
foreach(_text IN ITEMS
  "ANO_CRYPTO_SCRYPT_N 32768u" "ANO_CRYPTO_SCRYPT_R 8u" "ANO_CRYPTO_SCRYPT_P 1u"
  "ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE 32u" "ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE 67108864u"
  "ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE 33556544u" "ano_crypto_scrypt_derive_key"
  "caller-provided byte strings" "workspace" "zeroized")
  require_text("include/anocrypto/ac_kdf.h" "${_text}")
endforeach()
require_text("abi/ANOCRYPTO_C_4_EXPORTED_SYMBOLS.txt" "ano_crypto_scrypt_derive_key")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_4")
require_text("tests/kat/scrypt_kat_tests.c" "known SKP1 password")
require_text("tests/differential/scrypt_openssl_differential_tests.c" "EVP_PBE_scrypt")
require_text("CMakeLists.txt" "tests/kat/scrypt_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/scrypt_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/scrypt_openssl_differential_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_kdf.h" _header)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_scrypt_[A-Za-z0-9_]+" _api "${_header}")
list(LENGTH _api _api_count)
if(NOT _api_count EQUAL 1)
  list(APPEND _violations "public scrypt API must contain exactly one function; found ${_api_count}")
endif()
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/kdf/scrypt.c" _source)
if(_source MATCHES "openssl/|EVP_|OpenSSL::Crypto" OR _header MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "production scrypt implementation or public header depends on OpenSSL")
endif()
if(_source MATCHES "SKP1|SKF1|SKT1" OR _header MATCHES "SKP1|SKF1|SKT1")
  list(APPEND _violations "production primitive contains product format/workflow logic")
endif()
get_filename_component(_dir "${ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_dir}")
file(WRITE "${ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT}" "Policy: ${ANO_CRYPTO_SCRYPT_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "scrypt policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_SCRYPT_POLICY_CHECK_OUTPUT}"
  "Profile: N=${ANO_CRYPTO_SCRYPT_N},r=${ANO_CRYPTO_SCRYPT_R},p=${ANO_CRYPTO_SCRYPT_P},out=${ANO_CRYPTO_SCRYPT_DERIVED_KEY_SIZE}\nWorkspace: ${ANO_CRYPTO_SCRYPT_WORKSPACE_SIZE}/${ANO_CRYPTO_SCRYPT_MAX_MEMORY_SIZE}\nHosted: ${ANO_CRYPTO_SCRYPT_HOSTED_STATUS}/${ANO_CRYPTO_SCRYPT_HOSTED_REVIEW}\nSigning: ${ANO_CRYPTO_SCRYPT_REPOSITORY_SIGNING}\nExternal authorization: ${ANO_CRYPTO_SCRYPT_EXTERNAL_AUTHORIZATION}\nDownstream identity conflict: ${ANO_CRYPTO_SCRYPT_DOWNSTREAM_IDENTITY_CONFLICT}\nResult: PASS\n")
message(STATUS "scrypt P3-05 policy check passed")
