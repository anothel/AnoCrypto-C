cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT)
  set(ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT "${ANO_CRYPTO_SOURCE_DIR}/build/policy-check/RESTRICTED_SHA1_OCSP_CERTID_POLICY_CHECK.txt")
endif()
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RESTRICTED_SHA1_OCSP_POLICY.cmake")
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
  include/anocrypto/ac_ocsp_certid_hash.h
  src/internal/ac_sha1_internal.h src/hash/sha1.c src/hash/ocsp_certid_sha1.c
  abi/ANOCRYPTO_C_7_EXPORTED_SYMBOLS.txt
  abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt abi/anocrypto_c.map
  tests/kat/restricted_sha1_ocsp_certid_kat_tests.c
  tests/negative/restricted_sha1_ocsp_certid_negative_tests.c
  tests/differential/restricted_sha1_ocsp_certid_openssl_differential_tests.c
  docs/design/C99_RESTRICTED_SHA1_OCSP_CERTID_API_GATE.md
  docs/integration/ANOPKI_OCSP_CERTID_SHA1_COMPATIBILITY.md
  docs/product/P3_10_IMPLEMENTATION_TEST_DELTA.md
  evidence/release-template/RESTRICTED_SHA1_OCSP_CERTID_IMPLEMENTATION_REVIEW.md)
  require_file("${_file}")
endforeach()
if(NOT ANO_CRYPTO_RESTRICTED_SHA1_PUBLIC_FUNCTION_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_DIGEST_SIZE EQUAL 20 OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_PUBLIC_HEADER_COUNT EQUAL 18 OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_ABI7_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_CURRENT_SYMBOL_COUNT EQUAL 85 OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_CURRENT_ABI_LINE STREQUAL "ANOCRYPTO_C_7")
  list(APPEND _violations "frozen restricted SHA-1 API/ABI policy values drifted")
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
   NOT ANO_CRYPTO_ABI7_SYMBOL_COUNT EQUAL 1 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_7")
  list(APPEND _violations "current additive public identity mismatch")
endif()
if(NOT ANO_CRYPTO_RESTRICTED_SHA1_KCMVP_LANE STREQUAL "DEFERRED" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_HOSTED_STATUS STREQUAL "NOT_RUN" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_HOSTED_REVIEW STREQUAL "UNREVIEWED" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_REPOSITORY_SIGNING STREQUAL "UNSIGNED" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_SIGNING_HANDOFF STREQUAL "BLOCKED" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_EXTERNAL_AUTHORIZATION STREQUAL "NO" OR
   NOT ANO_CRYPTO_RESTRICTED_SHA1_PROVENANCE_STATUS STREQUAL "CONTENT_RECONCILED_PROVENANCE_OPEN")
  list(APPEND _violations "external status values drifted")
endif()
foreach(_text IN ITEMS
  "ANO_CRYPTO_OCSP_CERTID_SHA1_DIGEST_SIZE 20u"
  "ano_crypto_ocsp_certid_sha1_digest"
  "not a generic hash algorithm"
  "issuer-key hashing only")
  require_text("include/anocrypto/ac_ocsp_certid_hash.h" "${_text}")
endforeach()
require_text("abi/ANOCRYPTO_C_7_EXPORTED_SYMBOLS.txt" "ano_crypto_ocsp_certid_sha1_digest")
require_text("abi/anocrypto_c.map" "ANOCRYPTO_C_7")
require_text("tests/differential/restricted_sha1_ocsp_certid_openssl_differential_tests.c" "OCSP_cert_to_id")
require_text("docs/design/C99_RESTRICTED_SHA1_OCSP_CERTID_API_GATE.md" "SHA-1 signatures are prohibited")
require_text("docs/product/P3_10_IMPLEMENTATION_TEST_DELTA.md" "No generic SHA-1 hash enum")
require_text("evidence/release-template/RESTRICTED_SHA1_OCSP_CERTID_IMPLEMENTATION_REVIEW.md" "No SHA-1 signature, HMAC, KDF, or password service")
require_text("CMakeLists.txt" "tests/kat/restricted_sha1_ocsp_certid_kat_tests.c")
require_text("CMakeLists.txt" "tests/negative/restricted_sha1_ocsp_certid_negative_tests.c")
require_text("CMakeLists.txt" "tests/differential/restricted_sha1_ocsp_certid_openssl_differential_tests.c")
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_ocsp_certid_hash.h" _restricted_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_hash.h" _hash_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_hmac.h" _hmac_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_kdf.h" _kdf_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/ac_signature.h" _signature_header)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/hash/sha1.c" _sha1_source)
file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/hash/ocsp_certid_sha1.c" _ocsp_source)
string(REGEX MATCHALL "ANO_CRYPTO_API[ \t\r\n]+ano_crypto_status_t[ \t\r\n]+ano_crypto_ocsp_certid_sha1_digest" _api "${_restricted_header}")
list(LENGTH _api _api_count)
if(NOT _api_count EQUAL 1)
  list(APPEND _violations "P3-10 must add exactly one public function; found ${_api_count}")
endif()
if(_hash_header MATCHES "ANO_CRYPTO_HASH_ALG_SHA1")
  list(APPEND _violations "generic SHA-1 hash enum is prohibited")
endif()
set(_other_headers "${_hmac_header}\n${_kdf_header}\n${_signature_header}")
if(_other_headers MATCHES "SHA1|SHA-1|sha1")
  list(APPEND _violations "SHA-1 must not enter public HMAC, KDF, or signature services")
endif()
set(_production "${_restricted_header}\n${_sha1_source}\n${_ocsp_source}")
if(_production MATCHES "openssl/|EVP_|OpenSSL::Crypto")
  list(APPEND _violations "production restricted SHA-1 implementation depends on OpenSSL")
endif()
if(_production MATCHES "PEM_|X509_|OCSP_cert_to_id|OCSP_basic|d2i_OCSP|i2d_OCSP|ASN1_|PKCS8|SubjectPublicKeyInfo|SPKI")
  list(APPEND _violations "production primitive contains downstream OCSP/PKI codec logic")
endif()
get_filename_component(_dir "${ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_dir}")
file(WRITE "${ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT}"
  "Policy: ${ANO_CRYPTO_RESTRICTED_SHA1_POLICY_ID}\nHeaders: ${ANO_CRYPTO_CURRENT_PUBLIC_HEADER_COUNT}\nSymbols: ${ANO_CRYPTO_CURRENT_SYMBOL_COUNT}\nABI: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n")
if(_violations)
  file(APPEND "${ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT}" "Result: FAIL\nViolations:${_violations}\n")
  message(FATAL_ERROR "Restricted SHA-1 OCSP CertID policy check failed: ${_violations}")
endif()
file(APPEND "${ANO_CRYPTO_RESTRICTED_SHA1_POLICY_CHECK_OUTPUT}"
  "Use: ${ANO_CRYPTO_RESTRICTED_SHA1_USE}\nDigest size: ${ANO_CRYPTO_RESTRICTED_SHA1_DIGEST_SIZE}\n"
  "Hosted: ${ANO_CRYPTO_RESTRICTED_SHA1_HOSTED_STATUS}/${ANO_CRYPTO_RESTRICTED_SHA1_HOSTED_REVIEW}\n"
  "Signing: ${ANO_CRYPTO_RESTRICTED_SHA1_REPOSITORY_SIGNING}/${ANO_CRYPTO_RESTRICTED_SHA1_SIGNING_HANDOFF}\n"
  "External authorization: ${ANO_CRYPTO_RESTRICTED_SHA1_EXTERNAL_AUTHORIZATION}\n"
  "Provenance: ${ANO_CRYPTO_RESTRICTED_SHA1_PROVENANCE_STATUS}\nResult: PASS\n")
message(STATUS "Restricted SHA-1 OCSP CertID P3-10 policy check passed")
