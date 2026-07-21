cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/AES256_GCM_STREAMING_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_AES256_GCM_STREAMING_POLICY.cmake")
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
  include/anocrypto/ac_aead.h src/aead/aes256_gcm.c src/internal/ac_aead_internal.h
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/aes256_gcm_streaming_tests.c
  tests/negative/aes256_gcm_streaming_negative_tests.c
  tests/differential/aes256_gcm_streaming_openssl_differential_tests.c
  docs/design/C99_AES256_GCM_STREAMING_API_GATE.md
  docs/integration/ANOSECUREKIT_AES256_GCM_STREAMING_COMPATIBILITY.md
  docs/product/P3_04_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/AES256_GCM_STREAMING_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_AES256_GCM_STREAMING_PUBLIC_FUNCTION_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_KEY_SIZE EQUAL 32 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_NONCE_SIZE EQUAL 12 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_TAG_SIZE EQUAL 16 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_PUBLIC_HEADER_COUNT EQUAL 16 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_CURRENT_SYMBOL_COUNT EQUAL 79 OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_3" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_DECRYPT_OUTPUT_POLICY STREQUAL "UNVERIFIED_UNTIL_FINAL_CALLER_BUFFERING_REQUIRED" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_CONTEXT_STORAGE_POLICY STREQUAL "MALLOC_EQUIVALENT_ALIGNMENT_REQUIRED" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_OUT_WRITTEN_POLICY STREQUAL "DISJOINT_FROM_CONTEXT_INPUT_OUTPUT")
  list(APPEND _violations "frozen streaming API/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 83 OR
   NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_ABI4_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI5_SYMBOL_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_5")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_AES256_GCM_STREAMING_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_AES256_GCM_STREAMING_DOWNSTREAM_IDENTITY_CONFLICT STREQUAL "OPEN_DOWNSTREAM_SYNC_DECISION")
  list(APPEND _violations "retained external status values drifted")
endif()
foreach(_name IN ITEMS
  ano_crypto_aes256_gcm_stream_context_size
  ano_crypto_aes256_gcm_stream_init
  ano_crypto_aes256_gcm_stream_aad_update
  ano_crypto_aes256_gcm_stream_update
  ano_crypto_aes256_gcm_stream_encrypt_final
  ano_crypto_aes256_gcm_stream_decrypt_final
  ano_crypto_aes256_gcm_stream_uninit)
  require_text("include/anocrypto/ac_aead.h" "${_name}")
  require_text("abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt" "${_name}")
endforeach()
require_text("include/anocrypto/ac_aead.h" "UNVERIFIED")
require_text("include/anocrypto/ac_aead.h" "buffer")
require_text("include/anocrypto/ac_aead.h" "malloc/calloc/realloc")
require_text("include/anocrypto/ac_aead.h" "out_written must not overlap")
require_text("tests/negative/aes256_gcm_streaming_negative_tests.c" "misaligned context")
require_text("tests/negative/aes256_gcm_streaming_negative_tests.c" "out-written overlaps input")
require_text("tests/negative/aes256_gcm_streaming_negative_tests.c" "null encrypt tag")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_3")
require_text("CMakeLists.txt" "tests/kat/aes256_gcm_streaming_tests.c")
require_text("CMakeLists.txt" "tests/negative/aes256_gcm_streaming_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/aes256_gcm_streaming_openssl_differential_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_aead.h" _header)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+(size_t|ano_crypto_status_t)[ \t\r\n]+ano_crypto_aes256_gcm_stream_[A-Za-z0-9_]+" _matches "${_header}")
list(LENGTH _matches _api_count)
if(NOT _api_count EQUAL 7)
  list(APPEND _violations "public streaming API must contain exactly seven functions; found ${_api_count}")
endif()
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/aead/aes256_gcm.c" _source)
if(_source MATCHES "openssl/|EVP_|OpenSSL::Crypto" OR _header MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "product streaming implementation or public header depends on OpenSSL")
endif()
if(_source MATCHES "SKT1|SKF1|SKP1" OR _header MATCHES "SKT1|SKF1|SKP1")
  list(APPEND _violations "product primitive contains AnoSecureKit format/workflow logic")
endif()
get_filename_component(_out_dir "${ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")
file(WRITE "${ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "AES-256-GCM streaming policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_AES256_GCM_STREAMING_POLICY_CHECK_OUTPUT}"
  "Decrypt output policy: ${ANO_CRYPTO_AES256_GCM_STREAMING_DECRYPT_OUTPUT_POLICY}\nContext storage policy: ${ANO_CRYPTO_AES256_GCM_STREAMING_CONTEXT_STORAGE_POLICY}\nOut-written policy: ${ANO_CRYPTO_AES256_GCM_STREAMING_OUT_WRITTEN_POLICY}\nHosted: ${ANO_CRYPTO_AES256_GCM_STREAMING_HOSTED_STATUS}/${ANO_CRYPTO_AES256_GCM_STREAMING_HOSTED_REVIEW}\nSigning: ${ANO_CRYPTO_AES256_GCM_STREAMING_REPOSITORY_SIGNING}\nExternal authorization: ${ANO_CRYPTO_AES256_GCM_STREAMING_EXTERNAL_AUTHORIZATION}\nDownstream identity conflict: ${ANO_CRYPTO_AES256_GCM_STREAMING_DOWNSTREAM_IDENTITY_CONFLICT}\nResult: PASS\n")
message(STATUS "AES-256-GCM streaming policy check passed")
