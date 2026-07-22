cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/ECDSA_P256_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_ECDSA_P256_POLICY.cmake")
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
  src/csp/key.c src/signature/ecdsa_p256.c src/signature/signature.c
  abi/ANOCRYPTO_C_6_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/ecdsa_p256_sha256_kat_tests.c
  tests/negative/ecdsa_p256_api_negative_tests.c
  tests/differential/ecdsa_p256_openssl_differential_tests.c
  tests/unit/ecdsa_p256_thread_safety_tests.c
  docs/design/C99_ECDSA_P256_SHA256_API_GATE.md
  docs/integration/ANOPKI_ECDSA_P256_COMPATIBILITY.md
  docs/product/P3_09_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/ECDSA_P256_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_ECDSA_PUBLIC_FUNCTION_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ECDSA_CURVE STREQUAL "P-256" OR
   NOT ANO_CRYPTO_ECDSA_DIGEST STREQUAL "SHA-256" OR
   NOT ANO_CRYPTO_ECDSA_COMPONENT_SIZE EQUAL 32 OR
   NOT ANO_CRYPTO_ECDSA_SIGNATURE_SIZE EQUAL 64 OR
   NOT ANO_CRYPTO_ECDSA_PUBLIC_HEADER_COUNT EQUAL 17 OR
   NOT ANO_CRYPTO_ECDSA_ABI6_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ECDSA_CURRENT_SYMBOL_COUNT EQUAL 84 OR
   NOT ANO_CRYPTO_ECDSA_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_6")
  list(APPEND _violations "frozen ECDSA API/algorithm/ABI policy values drifted")
endif()
if(NOT ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT EQUAL 18 OR
   NOT ANO_CRYPTO_CURRENT_SYMBOL_COUNT EQUAL 85 OR
   NOT ANO_CRYPTO_ABI0_SYMBOL_COUNT EQUAL 69 OR
   NOT ANO_CRYPTO_ABI1_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI2_SYMBOL_COUNT EQUAL 2 OR
   NOT ANO_CRYPTO_ABI3_SYMBOL_COUNT EQUAL 7 OR
   NOT ANO_CRYPTO_ABI4_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_ABI5_SYMBOL_COUNT EQUAL 3 OR
   NOT ANO_CRYPTO_ABI6_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_7")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_ECDSA_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_ECDSA_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_ECDSA_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_ECDSA_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_ECDSA_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_ECDSA_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_ECDSA_PROVENANCE_STATUS STREQUAL "CONTENT_RECONCILED_PROVENANCE_OPEN")
  list(APPEND _violations "external status values drifted")
endif()
foreach(_text IN ITEMS
  "ANO_CRYPTO_EC_P256_COMPONENT_SIZE 32u"
  "ano_crypto_ec_p256_key_import")
  require_text("include/anocrypto/ac_key.h" "${_text}")
endforeach()
foreach(_text IN ITEMS
  "ANO_CRYPTO_ECDSA_P256_SIGNATURE_SIZE 64u"
  "ANO_CRYPTO_SIGNATURE_ECDSA_P256_SHA256 = 4"
  "RFC 6979 HMAC-SHA-256"
  "raw r || s")
  require_text("include/anocrypto/ac_signature.h" "${_text}")
endforeach()
require_text("abi/ANOCRYPTO_C_6_EXPORTED_SYMBOLS.txt" "ano_crypto_ec_p256_key_import")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_7")
require_text("tests/kat/ecdsa_p256_sha256_kat_tests.c" "ECDSA RFC6979 signature mismatch")
require_text("tests/differential/ecdsa_p256_openssl_differential_tests.c" "CSR proof-of-possession")
require_text("tests/differential/ecdsa_p256_openssl_differential_tests.c" "OCSP response signing TBS")
require_text("docs/design/C99_ECDSA_P256_SHA256_API_GATE.md" "does not claim a complete target-specific constant-time verdict")
require_text("docs/product/P3_09_IMPLEMENTATION_TEST_DELTA.md" "complete target-specific constant-time verdict")
require_text("evidence/release-template/ECDSA_P256_IMPLEMENTATION_REVIEW.md" "no complete target-specific constant-time verdict claimed")
require_text("CMakeLists.txt" "tests/kat/ecdsa_p256_sha256_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/ecdsa_p256_api_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/ecdsa_p256_openssl_differential_tests.c")
require_text("CMakeLists.txt" "tests/unit/ecdsa_p256_thread_safety_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_key.h" _key_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_signature.h" _signature_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/signature/ecdsa_p256.c" _ecdsa_source)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/signature/signature.c" _signature_source)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_ec_p256_key_import" _api "${_key_header}")
list(LENGTH _api _api_count)
if(NOT _api_count EQUAL 1)
  list(APPEND _violations "public P3-09 API must add exactly one function; found ${_api_count}")
endif()
set(_production "${_key_header}\n${_signature_header}\n${_ecdsa_source}\n${_signature_source}")
if(_ecdsa_source MATCHES "while[ \t\r\n]*[(][ \t\r\n]*carry")
  list(APPEND _violations "production ECDSA carry propagation must use fixed-length loops")
endif()
if(_production MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "production ECDSA implementation or public header depends on OpenSSL")
endif()
set(_production_impl "${_ecdsa_source}\n${_signature_source}")
if(_production_impl MATCHES "PEM_|X509_|OCSP_|PKCS8|SubjectPublicKeyInfo|SEC1|SPKI")
  list(APPEND _violations "production ECDSA primitive contains downstream PKI/container logic")
endif()
get_filename_component(_dir "${ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_dir}")
file(WRITE "${ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_ECDSA_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "ECDSA P-256 policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_ECDSA_POLICY_CHECK_OUTPUT}"
  "Profile: ${ANO_CRYPTO_ECDSA_CURVE}/${ANO_CRYPTO_ECDSA_DIGEST},component=${ANO_CRYPTO_ECDSA_COMPONENT_SIZE},signature=${ANO_CRYPTO_ECDSA_SIGNATURE_SIZE}\n"
  "Nonce: ${ANO_CRYPTO_ECDSA_NONCE}\nRepresentation: ${ANO_CRYPTO_ECDSA_SIGNATURE_REPRESENTATION}\n"
  "Hosted: ${ANO_CRYPTO_ECDSA_HOSTED_STATUS}/${ANO_CRYPTO_ECDSA_HOSTED_REVIEW}\n"
  "Signing: ${ANO_CRYPTO_ECDSA_REPOSITORY_SIGNING}/${ANO_CRYPTO_ECDSA_SIGNING_HANDOFF}\n"
  "External authorization: ${ANO_CRYPTO_ECDSA_EXTERNAL_AUTHORIZATION}\n"
  "Provenance: ${ANO_CRYPTO_ECDSA_PROVENANCE_STATUS}\nResult: PASS\n")
message(STATUS "ECDSA P-256 P3-09 policy check passed")
