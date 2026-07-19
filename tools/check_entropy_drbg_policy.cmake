cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR OR ANO_CRYPTO_SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "ANO_CRYPTO_SOURCE_DIR is required")
endif()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
if(NOT DEFINED ANO_CRYPTO_ENTROPY_DRBG_POLICY_CHECK_OUTPUT OR
   ANO_CRYPTO_ENTROPY_DRBG_POLICY_CHECK_OUTPUT STREQUAL "")
    set(ANO_CRYPTO_ENTROPY_DRBG_POLICY_CHECK_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/ENTROPY_DRBG_POLICY_CHECK.txt")
endif()
get_filename_component(_output "${ANO_CRYPTO_ENTROPY_DRBG_POLICY_CHECK_OUTPUT}" ABSOLUTE)
get_filename_component(_output_dir "${_output}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")

function(require_file _relative)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" OR
       IS_DIRECTORY "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
        message(FATAL_ERROR "Required P2-04 file is missing: ${_relative}")
    endif()
endfunction()

function(require_text _relative _needle)
    require_file("${_relative}")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _text)
    string(FIND "${_text}" "${_needle}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR
            "P2-04 policy text is missing from ${_relative}: ${_needle}")
    endif()
endfunction()

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/load_public_surface_state.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_ENTROPY_DRBG_POLICY.cmake")

foreach(_required_var IN ITEMS
        ANO_CRYPTO_ENTROPY_DRBG_POLICY_ID
        ANO_CRYPTO_ENTROPY_DRBG_POLICY_STATUS
        ANO_CRYPTO_ENTROPY_DRBG_FORMAL_ACCEPTANCE
        ANO_CRYPTO_ENTROPY_DRBG_CANDIDATE_ID
        ANO_CRYPTO_ENTROPY_PROVIDER_PROFILE_ID
        ANO_CRYPTO_ENTROPY_PROVIDER_ID
        ANO_CRYPTO_ENTROPY_SOURCE_CLASS
        ANO_CRYPTO_ENTROPY_HEALTH_TEST_OWNER
        ANO_CRYPTO_DRBG_MECHANISM
        ANO_CRYPTO_DRBG_RESEED_INTERVAL_REQUESTS)
    if(NOT DEFINED ${_required_var} OR "${${_required_var}}" STREQUAL "")
        message(FATAL_ERROR "Entropy/DRBG policy variable is missing: ${_required_var}")
    endif()
endforeach()

if(NOT ANO_CRYPTO_ENTROPY_DRBG_POLICY_ID STREQUAL "EDP-P2-04-R1" OR
   NOT ANO_CRYPTO_ENTROPY_DRBG_POLICY_STATUS STREQUAL "FROZEN_INTERNAL_POLICY" OR
   NOT ANO_CRYPTO_ENTROPY_DRBG_FORMAL_ACCEPTANCE STREQUAL "EXTERNAL_CONFIRMATION_REQUIRED" OR
   NOT ANO_CRYPTO_ENTROPY_DRBG_CANDIDATE_ID STREQUAL ANO_CRYPTO_VALIDATION_CANDIDATE_ID OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_PROFILE_ID STREQUAL "ENT-OE01-LINUX-GETRANDOM-R1" OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_ID STREQUAL "linux-getrandom" OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_API STREQUAL "getrandom" OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_FLAGS STREQUAL "0" OR
   NOT ANO_CRYPTO_ENTROPY_SOURCE_CLASS STREQUAL "EXTERNAL_CONDITIONED_RBG" OR
   NOT ANO_CRYPTO_ENTROPY_HEALTH_TEST_OWNER STREQUAL "EXTERNAL_PROVIDER" OR
   NOT ANO_CRYPTO_ENTROPY_MODULE_HEALTH_TEST_ON_PROVIDER_OUTPUT STREQUAL "DISABLED" OR
   NOT ANO_CRYPTO_ENTROPY_DIRECT_NOISE_SOURCE_SCOPE STREQUAL "OUT_OF_SCOPE_INITIAL_CANDIDATE" OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_RETRY_EINTR STREQUAL "ON" OR
   NOT ANO_CRYPTO_ENTROPY_PROVIDER_COMPLETE_FILL STREQUAL "ON" OR
   NOT ANO_CRYPTO_DRBG_MECHANISM STREQUAL "HMAC_DRBG_SHA256" OR
   NOT ANO_CRYPTO_DRBG_INSTANTIATE_ENTROPY_BYTES STREQUAL "32" OR
   NOT ANO_CRYPTO_DRBG_INSTANTIATE_NONCE_BYTES STREQUAL "16" OR
   NOT ANO_CRYPTO_DRBG_RESEED_ENTROPY_BYTES STREQUAL "32" OR
   NOT ANO_CRYPTO_DRBG_MAX_REQUEST_BYTES STREQUAL "65536" OR
   NOT ANO_CRYPTO_DRBG_RESEED_INTERVAL_REQUESTS STREQUAL "281474976710656" OR
   NOT ANO_CRYPTO_DRBG_PREDICTION_RESISTANCE STREQUAL "UNSUPPORTED" OR
   NOT ANO_CRYPTO_DRBG_PROCESS_CHANGE_POLICY STREQUAL "RESEED_REQUIRED" OR
   NOT ANO_CRYPTO_DRBG_VM_CLONE_POLICY STREQUAL "DISCARD_AND_REINITIALIZE" OR
   NOT ANO_CRYPTO_EXPLICIT_DRBG_ENTROPY_OWNER STREQUAL "CALLER" OR
   NOT ANO_CRYPTO_ENTROPY_DRBG_GENERATED_BYTES_LOGGED STREQUAL "NO")
    message(FATAL_ERROR "Frozen P2-04 entropy/DRBG policy values have drifted")
endif()

set(_public_headers "${ANO_CRYPTO_CURRENT_PUBLIC_HEADERS}")
set(_public_header_count "${ANO_CRYPTO_RETAINED_PUBLIC_HEADER_COUNT}")
file(STRINGS "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_0_EXPORTED_SYMBOLS.txt"
    _symbol_lines REGEX "^[A-Za-z_][A-Za-z0-9_]*$")
list(LENGTH _symbol_lines _symbol_count)
if(NOT _public_header_count EQUAL 14 OR NOT _symbol_count EQUAL 69 OR
   NOT ANO_CRYPTO_RELEASE_ABI_LINE STREQUAL "ANOCRYPTO_C_3")
    message(FATAL_ERROR
        "P2-04 public ABI changed: headers=${_public_header_count}, symbols=${_symbol_count}, ABI=${ANO_CRYPTO_RELEASE_ABI_LINE}")
endif()

require_text("src/internal/ac_entropy_internal.h"
    "ANO_CRYPTO_INTERNAL_ENTROPY_CLASS_EXTERNAL_CONDITIONED_RBG")
require_text("src/internal/ac_entropy_internal.h"
    "ANO_CRYPTO_INTERNAL_ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER")
require_text("src/drbg/entropy_adapter.c" "getrandom(out + offset, remaining, 0u)")
require_text("src/drbg/entropy_adapter.c" "errno == EINTR")
require_text("src/drbg/entropy_adapter.c" "offset += (size_t)nread")
require_text("src/drbg/entropy_adapter.c"
    "source->kind != ANO_CRYPTO_INTERNAL_ENTROPY_SOURCE_KIND_CALLBACK")
require_text("src/drbg/drbg.c"
    "uint8_t entropy_input[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_ENTROPY_INPUT_SIZE]")
require_text("src/drbg/drbg.c"
    "uint8_t nonce[ANO_CRYPTO_DRBG_HMAC_SHA256_MIN_NONCE_SIZE]")
require_text("src/internal/ac_drbg_internal.h"
    "ANO_CRYPTO_INTERNAL_HMAC_DRBG_RESEED_INTERVAL 281474976710656ULL")
require_text("src/drbg/drbg.c"
    "ctx->reseed_counter > ANO_CRYPTO_INTERNAL_HMAC_DRBG_RESEED_INTERVAL")
require_text("src/drbg/drbg.c" "ano_crypto_internal_process_guard_check")
require_text("src/drbg/random.c" "return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER")
require_text("tests/negative/random_reseed_limit_tests.c" "last allowed generate")
require_text("tests/negative/random_service_failure_injection_tests.c"
    "old random state remains usable after manual reseed failure")
require_text("tests/operational/entropy_drbg_policy_review.c"
    "generated_bytes_logged=no")
require_text("docs/kcmvp/KCMVP_ENTROPY_AND_DRBG_SPEC.md" "EDP-P2-04-R1")
require_text("docs/kcmvp/KCMVP_ENTROPY_RESPONSIBILITY_MATRIX.md"
    "OUT_OF_SCOPE_INITIAL_CANDIDATE")
require_text("docs/kcmvp/KCMVP_ENTROPY_HEALTH_AND_RECOVERY_MATRIX.md"
    "EXTERNAL_CONFIRMATION_REQUIRED")
require_text("docs/kcmvp/P2_04_IMPLEMENTATION_TEST_DELTA.md"
    "BLOCKED_EXTERNAL")
require_text("docs/ci/HOSTED_CI_EXTERNAL_BLOCKER.md" "| Actual hosted lanes | `NOT_RUN` |")
require_text("include/anocrypto/ac_version.h" "0.1.0-p3-04-aes256-gcm-streaming")

foreach(_header IN LISTS _public_headers)
    file(READ "${_header}" _header_text)
    if(_header_text MATCHES
       "ENT-OE01|EDP-P2-04|RAW_NOISE_TEST_CANDIDATE|ENTROPY_HEALTH_OWNER_EXTERNAL_PROVIDER|INTERNAL_HMAC_DRBG_RESEED_INTERVAL")
        message(FATAL_ERROR "P2-04 internal policy token leaked into public header: ${_header}")
    endif()
endforeach()

file(WRITE "${_output}"
"AnoCrypto-C P2-04 Entropy and DRBG Policy Check\n"
"Policy ID: ${ANO_CRYPTO_ENTROPY_DRBG_POLICY_ID}\n"
"Policy status: ${ANO_CRYPTO_ENTROPY_DRBG_POLICY_STATUS}\n"
"Formal acceptance: ${ANO_CRYPTO_ENTROPY_DRBG_FORMAL_ACCEPTANCE}\n"
"Candidate ID: ${ANO_CRYPTO_ENTROPY_DRBG_CANDIDATE_ID}\n"
"Provider profile: ${ANO_CRYPTO_ENTROPY_PROVIDER_PROFILE_ID}\n"
"Provider/source class: ${ANO_CRYPTO_ENTROPY_PROVIDER_ID} / ${ANO_CRYPTO_ENTROPY_SOURCE_CLASS}\n"
"Health-test owner: ${ANO_CRYPTO_ENTROPY_HEALTH_TEST_OWNER}\n"
"Direct noise source: ${ANO_CRYPTO_ENTROPY_DIRECT_NOISE_SOURCE_SCOPE}\n"
"DRBG: ${ANO_CRYPTO_DRBG_MECHANISM}\n"
"Instantiate entropy/nonce: ${ANO_CRYPTO_DRBG_INSTANTIATE_ENTROPY_BYTES}/${ANO_CRYPTO_DRBG_INSTANTIATE_NONCE_BYTES} bytes\n"
"Reseed entropy: ${ANO_CRYPTO_DRBG_RESEED_ENTROPY_BYTES} bytes\n"
"Max request: ${ANO_CRYPTO_DRBG_MAX_REQUEST_BYTES} bytes\n"
"Reseed interval: ${ANO_CRYPTO_DRBG_RESEED_INTERVAL_REQUESTS} requests\n"
"Prediction resistance: ${ANO_CRYPTO_DRBG_PREDICTION_RESISTANCE}\n"
"Process/VM policy: ${ANO_CRYPTO_DRBG_PROCESS_CHANGE_POLICY} / ${ANO_CRYPTO_DRBG_VM_CLONE_POLICY}\n"
"ABI: ${ANO_CRYPTO_RELEASE_ABI_LINE} / ${_public_header_count} headers / ${_symbol_count} symbols\n"
"Hosted evidence: ${ANO_CRYPTO_ENTROPY_DRBG_HOSTED_EVIDENCE_STATUS}\n"
"Result: PASS\n")

message(STATUS "P2-04 entropy/DRBG policy check passed: ${_output}")
