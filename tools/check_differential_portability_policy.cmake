# AnoCrypto-C P1-23 differential-validation and portability policy check.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT)
    set(ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/DIFFERENTIAL_PORTABILITY_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT
    "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")

set(_violations "")
set(_warnings "")

function(ano_crypto_require_file relative_path description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}")
        list(APPEND _violations "missing ${description}: ${relative_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text relative_path needle description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}")
        list(APPEND _violations "missing file for ${description}: ${relative_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
        return()
    endif()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}" _content)
    string(FIND "${_content}" "${needle}" _index)
    if(_index EQUAL -1)
        list(APPEND _violations "missing ${description} in ${relative_path}: ${needle}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_forbid_text relative_path needle description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}")
        return()
    endif()
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${relative_path}" _content)
    string(FIND "${_content}" "${needle}" _index)
    if(NOT _index EQUAL -1)
        list(APPEND _violations "forbidden ${description} in ${relative_path}: ${needle}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

foreach(_required IN ITEMS
    "tests/differential/README.md"
    "tests/differential/sha2_hmac_openssl_differential_tests.c"
    "tests/operational/sha512_portability_review.c"
    "docs/kcmvp/OPENSSL_DIFFERENTIAL_PROVIDER_DECISION.md"
    "docs/design/SHA512_FAMILY_PORTABILITY_POLICY.md"
    "docs/ci/DIFFERENTIAL_PORTABILITY_CI_CHECKLIST.md"
    "evidence/release-template/DIFFERENTIAL_PORTABILITY_REVIEW.md")
    ano_crypto_require_file("${_required}" "P1-23 differential/portability artifact")
endforeach()

ano_crypto_require_text("CMakeLists.txt" "ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS" "optional differential-provider option")
ano_crypto_require_text("CMakeLists.txt" "ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER" "required differential-provider CI option")
ano_crypto_require_text("CMakeLists.txt" "find_package(OpenSSL 3.0 QUIET COMPONENTS Crypto)" "OpenSSL 3.x provider discovery")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha2_hmac_openssl_differential_tests" "differential test target")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_sha512_portability_review" "portability review target")
ano_crypto_require_text("CMakeLists.txt" "ANO_CRYPTO_C99_PORTABILITY_TYPES_OK" "C99 representation compile guard")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_differential_portability_policy_check" "P1-23 policy target")
ano_crypto_require_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "EVP_Q_digest" "OpenSSL EVP digest comparison")
ano_crypto_require_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "EVP_MAC_fetch" "OpenSSL EVP HMAC comparison")
ano_crypto_require_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "message_storage + 1u" "intentionally unaligned differential input")
ano_crypto_require_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "values_logged=no" "status-only differential output marker")
ano_crypto_require_text("tests/operational/sha512_portability_review.c" "unaligned_input=pass" "status-only alignment review")
ano_crypto_require_text("tests/operational/sha512_portability_review.c" "cmake_byte_order" "CMake/runtime byte-order comparison")
ano_crypto_require_text(".github/workflows/ci.yml" "ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON" "required hosted differential lane")
ano_crypto_require_text(".github/workflows/ci.yml" "gcc-multilib" "hosted Linux 32-bit toolchain lane")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "DIFFERENTIAL_VALIDATION.log" "differential evidence log")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "SHA512_PORTABILITY_REVIEW.log" "portability evidence log")
ano_crypto_require_text("tools/create_poc_package.cmake" "DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt" "PoC provider-boundary note")
ano_crypto_require_text("tools/run_poc_package_smoke.cmake" "DIFFERENTIAL_PORTABILITY_PACKAGE_NOTES.txt" "PoC provider-boundary smoke check")
ano_crypto_require_text("cleanroom/SOURCE_MANIFEST.md" "SRC-3201" "OpenSSL provider provenance entry")
ano_crypto_require_text("cleanroom/EXCLUSION_LOG.md" "EX-3202" "OpenSSL product-link exclusion")

ano_crypto_forbid_text("CMakeLists.txt" "target_link_libraries(anocrypto_c PRIVATE OpenSSL::Crypto" "OpenSSL product-library linkage")
if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/CMakeLists.txt")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/CMakeLists.txt" _top_level_cmake)
    if(_top_level_cmake MATCHES "target_link_libraries\\(anocrypto_c[^\\)]*OpenSSL::Crypto")
        list(APPEND _violations "OpenSSL::Crypto appears in the anocrypto_c product target link block")
    endif()
    if(_top_level_cmake MATCHES "target_include_directories\\(anocrypto_c[^\\)]*(OPENSSL|OpenSSL)")
        list(APPEND _violations "OpenSSL include path appears in the anocrypto_c product target include block")
    endif()
endif()
ano_crypto_forbid_text("include/anocrypto/anocrypto.h" "openssl/" "OpenSSL include in installed umbrella header")
ano_crypto_forbid_text("include/anocrypto/ac_hash.h" "openssl/" "OpenSSL include in public hash header")
ano_crypto_forbid_text("include/anocrypto/ac_hmac.h" "openssl/" "OpenSSL include in public HMAC header")
ano_crypto_forbid_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "%02x" "hex output formatting in differential log")
ano_crypto_forbid_text("tests/differential/sha2_hmac_openssl_differential_tests.c" "BIO_dump" "binary output dump in differential log")
ano_crypto_forbid_text("tests/operational/sha512_portability_review.c" "%02x" "hex output formatting in portability log")

if(EXISTS "${ANO_CRYPTO_BUILD_DIR}/CMakeCache.txt")
    file(READ "${ANO_CRYPTO_BUILD_DIR}/CMakeCache.txt" _cache)
    string(FIND "${_cache}" "ANO_CRYPTO_BUILD_TESTS:BOOL=OFF" _tests_off)
    string(FIND "${_cache}" "ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER:BOOL=ON" _provider_required)
    if(NOT _tests_off EQUAL -1 AND NOT _provider_required EQUAL -1)
        list(APPEND _violations "configured build requires the test-only differential provider while tests are disabled")
    endif()
endif()

list(APPEND _warnings "Actual Linux 32-bit, Windows, macOS, and any big-endian artifacts require explicit review before portability support wording changes.")
list(APPEND _warnings "OpenSSL differential agreement is engineering evidence only and does not replace KAT provenance or official validation.")

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}"
    "AnoCrypto-C differential-validation and SHA-512 portability check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- OpenSSL 3.x test-only provider selection and provenance recorded\n"
    "- deterministic SHA-2/HMAC one-shot and streaming differential target present\n"
    "- unaligned input, padding/block boundaries, long keys, and status-only output present\n"
    "- C99 representation and endian/alignment portability checks present\n"
    "- dedicated hosted differential and Linux 32-bit lanes present\n"
    "- evidence and PoC product-boundary wiring present\n"
    "- OpenSSL is absent from public headers and direct product target linkage\n\n")

if(_warnings)
    file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "Warnings / tracked TODOs:\n")
    foreach(_warning IN LISTS _warnings)
        file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "- ${_warning}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "\n")
endif()

if(_violations)
    file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C differential/portability policy check failed; see ${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C differential/portability policy check passed: ${ANO_CRYPTO_DIFFERENTIAL_PORTABILITY_CHECK_OUTPUT}")
