cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/AES256_GCM_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_AES256_GCM_POLICY.cmake")
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
  include/anocrypto/ac_aead.h src/aead/aes256_gcm.c
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt
  tests/kat/aes256_gcm_kat_tests.c tests/negative/aes256_gcm_api_negative_tests.c
  tests/differential/aes256_gcm_openssl_differential_tests.c
  docs/design/C99_AES256_GCM_API_GATE.md
  docs/integration/ANOSECUREKIT_AES256_GCM_COMPATIBILITY.md
  docs/product/P3_03_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/AES256_GCM_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_AES256_GCM_PUBLIC_FUNCTION_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_AES256_GCM_KEY_SIZE EQUAL 32 OR
   NOT ANO_CRYPTO_AES256_GCM_NONCE_SIZE EQUAL 12 OR
   NOT ANO_CRYPTO_AES256_GCM_TAG_SIZE EQUAL 16 OR
   NOT ANO_CRYPTO_AES256_GCM_PUBLIC_HEADER_COUNT EQUAL 16 OR
   NOT ANO_CRYPTO_AES256_GCM_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_AES256_GCM_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_AES256_GCM_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_AES256_GCM_CURRENT_SYMBOL_COUNT EQUAL 72 OR
   NOT ANO_CRYPTO_AES256_GCM_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_2" OR
   NOT ANO_CRYPTO_AES256_GCM_DECRYPT_OUTPUT_POLICY STREQUAL "AUTHENTICATED_OUTPUT_ONLY")
  list(APPEND _violations "frozen AES-256-GCM API/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 84 OR
   NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_ABI4_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI5_SYMBOL_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_6")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_AES256_GCM_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_AES256_GCM_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_AES256_GCM_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_AES256_GCM_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_AES256_GCM_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_AES256_GCM_DOWNSTREAM_IDENTITY_CONFLICT STREQUAL "OPEN_DOWNSTREAM_SYNC_DECISION")
  list(APPEND _violations "retained external status values drifted")
endif()
require_text("include/anocrypto/ac_aead.h" "ANO_CRYPTO_AES256_GCM_KEY_SIZE 32u")
require_text("include/anocrypto/ac_aead.h" "ANO_CRYPTO_AES256_GCM_NONCE_SIZE 12u")
require_text("include/anocrypto/ac_aead.h" "ANO_CRYPTO_AES256_GCM_TAG_SIZE 16u")
require_text("include/anocrypto/ac_aead.h" "ano_crypto_aes256_gcm_encrypt")
require_text("include/anocrypto/ac_aead.h" "ano_crypto_aes256_gcm_decrypt")
require_text("include/anocrypto/ac_aead.h" "authenticates before writing plaintext")
require_text("src/aead/aes256_gcm.c" "ANO_CRYPTO_ERR_AUTHENTICATION_FAILED")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_2")
require_text("CMakeLists.txt" "tests/kat/aes256_gcm_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/aes256_gcm_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/aes256_gcm_openssl_differential_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_aead.h" _header)
set(_api_count 0)
foreach(_name IN ITEMS ano_crypto_aes256_gcm_encrypt ano_crypto_aes256_gcm_decrypt)
  string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+${_name}[ \t\r\n]*[(]" _matches "${_header}")
  list(LENGTH _matches _name_count)
  math(EXPR _api_count "${_api_count} + ${_name_count}")
endforeach()
if(NOT _api_count EQUAL 2)
  list(APPEND _violations "public one-shot AES-256-GCM API must retain exactly two functions; found ${_api_count}")
endif()
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/aead/aes256_gcm.c" _source)
if(_source MATCHES "openssl/|EVP_|OpenSSL::Crypto" OR _header MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "product AES-256-GCM implementation or public header depends on OpenSSL")
endif()
if(_source MATCHES "SKT1|SKF1|SKP1" OR _header MATCHES "SKT1|SKF1|SKP1")
  list(APPEND _violations "product primitive contains AnoSecureKit format/workflow logic")
endif()
get_filename_component(_out_dir "${ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
file(WRITE "${ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_AES256_GCM_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "AES-256-GCM policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_AES256_GCM_POLICY_CHECK_OUTPUT}"
  "Output policy: ${ANO_CRYPTO_AES256_GCM_DECRYPT_OUTPUT_POLICY}\nHosted: ${ANO_CRYPTO_AES256_GCM_HOSTED_STATUS}/${ANO_CRYPTO_AES256_GCM_HOSTED_REVIEW}\nSigning: ${ANO_CRYPTO_AES256_GCM_REPOSITORY_SIGNING}\nExternal authorization: ${ANO_CRYPTO_AES256_GCM_EXTERNAL_AUTHORIZATION}\nDownstream identity conflict: ${ANO_CRYPTO_AES256_GCM_DOWNSTREAM_IDENTITY_CONFLICT}\nResult: PASS\n")
message(STATUS "AES-256-GCM policy check passed")
