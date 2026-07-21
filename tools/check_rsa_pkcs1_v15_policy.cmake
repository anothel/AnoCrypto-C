cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/RSA_PKCS1_V15_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RSA_PKCS1_V15_POLICY.cmake")
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
  include/anocrypto/ac_key.h include/anocrypto/ac_signature.h
  src/csp/key.c src/signature/rsa.c src/signature/signature.c
  abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_1_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_2_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_3_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_4_EXPORTED_SYMBOLS.txt abi/ANOCRYPTO_C_5_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/rsa_pkcs1_v15_kat_tests.c tests/negative/rsa_api_negative_tests.c
  tests/differential/rsa_openssl_differential_tests.c
  docs/design/C99_RSA_PKCS1_V15_SHA2_API_GATE.md
  docs/integration/ANOPKI_RSA_PKCS1_V15_COMPATIBILITY.md
  docs/product/P3_08_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/RSA_PKCS1_V15_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_RSA_PUBLIC_FUNCTION_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_RSA_MODULUS_BITS EQUAL 2048 OR
   NOT ANO_CRYPTO_RSA_PUBLIC_EXPONENT EQUAL 65537 OR
   NOT ANO_CRYPTO_RSA_SIGNATURE_SIZE EQUAL 256 OR
   NOT ANO_CRYPTO_RSA_PUBLIC_HEADER_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_RSA_ABI5_SYMBOL_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_RSA_CURRENT_SYMBOL_COUNT EQUAL 83 OR
   NOT ANO_CRYPTO_RSA_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_5")
  list(APPEND _violations "frozen RSA API/algorithm/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 83 OR
   NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69 OR NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_ABI4_SYMBOL_COUNT EQUAL 1 OR NOT ANO_CRYPTO_ABI5_SYMBOL_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_5")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_RSA_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_RSA_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_RSA_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_RSA_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_RSA_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_RSA_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_RSA_PROVENANCE_STATUS STREQUAL "CONTENT_RECONCILED_PROVENANCE_OPEN")
  list(APPEND _violations "external status values drifted")
endif()
foreach(_text IN ITEMS
  "ANO_CRYPTO_RSA2048_MODULUS_SIZE 256u" "ANO_CRYPTO_RSA2048_PRIME_SIZE 128u"
  "ano_crypto_rsa_key_import")
  require_text("include/anocrypto/ac_key.h" "${_text}")
endforeach()
# Signature declarations live in the dedicated header.
foreach(_text IN ITEMS
  "ANO_CRYPTO_RSA2048_SIGNATURE_SIZE 256u"
  "ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA256"
  "ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA384"
  "ANO_CRYPTO_SIGNATURE_RSA_PKCS1_V15_SHA512"
  "ano_crypto_signature_sign" "ano_crypto_signature_verify")
  require_text("include/anocrypto/ac_signature.h" "${_text}")
endforeach()
require_text("abi/ANOCRYPTO_C_5_EXPORTED_SYMBOLS.txt" "ano_crypto_rsa_key_import")
require_text("abi/ANOCRYPTO_C_5_EXPORTED_SYMBOLS.txt" "ano_crypto_signature_sign")
require_text("abi/ANOCRYPTO_C_5_EXPORTED_SYMBOLS.txt" "ano_crypto_signature_verify")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_5")
require_text("tests/differential/rsa_openssl_differential_tests.c" "EVP_PKEY_fromdata")
require_text("tests/kat/rsa_pkcs1_v15_kat_tests.c" "RSA_KAT_SIG_SHA512")
require_text("CMakeLists.txt" "tests/kat/rsa_pkcs1_v15_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/rsa_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/rsa_openssl_differential_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_key.h" _key_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_signature.h" _signature_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/signature/rsa.c" _rsa_source)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/signature/signature.c" _signature_source)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_(rsa_key_import|signature_(sign|verify))" _api "${_key_header}\n${_signature_header}")
list(LENGTH _api _api_count)
if(NOT _api_count EQUAL 3)
  list(APPEND _violations "public P3-08 API must contain exactly three functions; found ${_api_count}")
endif()
set(_production "${_key_header}\n${_signature_header}\n${_rsa_source}\n${_signature_source}")
if(_production MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "production RSA implementation or public header depends on OpenSSL")
endif()
if(_production MATCHES "PEM_|X509_|OCSP_|PKCS8|SubjectPublicKeyInfo")
  list(APPEND _violations "production RSA primitive contains downstream PKI/container logic")
endif()
get_filename_component(_dir "${ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_dir}")
file(WRITE "${ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_RSA_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "RSA PKCS#1 v1.5 policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_RSA_POLICY_CHECK_OUTPUT}"
  "Profile: RSA-${ANO_CRYPTO_RSA_MODULUS_BITS},e=${ANO_CRYPTO_RSA_PUBLIC_EXPONENT},signature=${ANO_CRYPTO_RSA_SIGNATURE_SIZE}\n"
  "Ingress: ${ANO_CRYPTO_RSA_KEY_INGRESS}\nSign model: ${ANO_CRYPTO_RSA_SIGN_MODEL}\n"
  "Private operation: ${ANO_CRYPTO_RSA_PRIVATE_OPERATION}\n"
  "Hosted: ${ANO_CRYPTO_RSA_HOSTED_STATUS}/${ANO_CRYPTO_RSA_HOSTED_REVIEW}\n"
  "Signing: ${ANO_CRYPTO_RSA_REPOSITORY_SIGNING}/${ANO_CRYPTO_RSA_SIGNING_HANDOFF}\n"
  "External authorization: ${ANO_CRYPTO_RSA_EXTERNAL_AUTHORIZATION}\n"
  "Provenance: ${ANO_CRYPTO_RSA_PROVENANCE_STATUS}\nResult: PASS\n")
message(STATUS "RSA PKCS#1 v1.5 P3-08 policy check passed")
