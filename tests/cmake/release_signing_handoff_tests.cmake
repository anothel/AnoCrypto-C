cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_TEST_WORK_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")
get_filename_component(_work_parent "${ANO_CRYPTO_TEST_WORK_DIR}" DIRECTORY)
set(_source_with_slash "${ANO_CRYPTO_SOURCE_DIR}/")
set(_work_with_slash "${ANO_CRYPTO_TEST_WORK_DIR}/")
string(FIND "${_source_with_slash}" "${_work_with_slash}" _work_contains_source)
if(_work_parent STREQUAL ANO_CRYPTO_TEST_WORK_DIR OR _work_contains_source EQUAL 0)
    message(FATAL_ERROR "Signing test work directory must not be a filesystem root or contain the source tree")
endif()
file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c")

function(_run_expect _expected_success _description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr
        TIMEOUT 120)
    if(_expected_success AND NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed unexpectedly:\n${_stdout}${_stderr}")
    elseif(NOT _expected_success AND _result EQUAL 0)
        message(FATAL_ERROR "${_description} unexpectedly succeeded")
    endif()
endfunction()

function(_require_text _path _text)
    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "${_path} omits required text: ${_text}")
    endif()
endfunction()

set(_file_list "${ANO_CRYPTO_TEST_WORK_DIR}/SOURCE_ARCHIVE_CONTENTS.txt")
file(WRITE "${_file_list}" "LICENSE\nREADME.md\n")
configure_file("${ANO_CRYPTO_SOURCE_DIR}/LICENSE"
    "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c/LICENSE" COPYONLY)
configure_file("${ANO_CRYPTO_SOURCE_DIR}/README.md"
    "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c/README.md" COPYONLY)
set(_archive "${ANO_CRYPTO_TEST_WORK_DIR}/anocrypto-c-source.tar.gz")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_archive}" anocrypto-c
    WORKING_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/archive"
    RESULT_VARIABLE _archive_result
    OUTPUT_VARIABLE _archive_stdout
    ERROR_VARIABLE _archive_stderr
    TIMEOUT 60)
if(NOT _archive_result EQUAL 0)
    message(FATAL_ERROR "Signing fixture source archive creation failed: ${_archive_stdout}${_archive_stderr}")
endif()

set(_bundle "${ANO_CRYPTO_TEST_WORK_DIR}/bundle")
_run_expect(TRUE "Signing fixture provenance creation"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${_bundle}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-13T00:00:00Z"
    "-DANO_CRYPTO_PROVENANCE_COMMIT=fixture-commit"
    "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=CLEAN"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

function(_create_handoff _output _security_reviewer _product_owner _expected_success)
    set(_args
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${_bundle}"
        "-DANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR=${_output}")
    if(NOT "${_security_reviewer}" STREQUAL "")
        list(APPEND _args
            "-DANO_CRYPTO_SECURITY_REVIEWER_IDENTITY=${_security_reviewer}"
            "-DANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY=${_product_owner}")
    endif()
    list(APPEND _args
        "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_signing_handoff.cmake")
    _run_expect(${_expected_success} "Signing handoff creation" ${_args})
endfunction()

function(_check_handoff _handoff _expected_success _description)
    _run_expect(${_expected_success} "${_description}"
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_SIGNING_HANDOFF_DIR=${_handoff}"
        "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_signing_handoff.cmake")
endfunction()

function(_rewrite_handoff_manifest _handoff)
    file(GLOB _handoff_files LIST_DIRECTORIES false
        RELATIVE "${_handoff}" "${_handoff}/*")
    list(SORT _handoff_files)
    file(WRITE "${_handoff}/SIGNING_HANDOFF_FILES.sha256" "")
    foreach(_relative IN LISTS _handoff_files)
        if(_relative STREQUAL "SIGNING_HANDOFF_FILES.sha256")
            continue()
        endif()
        file(SHA256 "${_handoff}/${_relative}" _hash)
        file(APPEND "${_handoff}/SIGNING_HANDOFF_FILES.sha256"
            "${_hash}  ${_relative}\n")
    endforeach()
endfunction()

set(_unprovisioned_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff-unprovisioned")
_create_handoff("${_unprovisioned_handoff}" "" "" TRUE)
_check_handoff("${_unprovisioned_handoff}" TRUE "Unprovisioned signing handoff verification")
_require_text("${_unprovisioned_handoff}/SIGNING_REQUEST.txt"
    "security_reviewer_identity_status=UNPROVISIONED")
_require_text("${_unprovisioned_handoff}/SIGNING_REQUEST.txt"
    "product_release_owner_identity_status=UNPROVISIONED")
_require_text("${_unprovisioned_handoff}/SIGNING_REQUEST.txt"
    "role_separation_status=UNPROVISIONED")
_require_text("${_unprovisioned_handoff}/SIGNING_HANDOFF_STATUS.txt" "Readiness: BLOCKED")

set(_tampered_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff-tampered")
file(MAKE_DIRECTORY "${_tampered_handoff}")
file(COPY "${_unprovisioned_handoff}/" DESTINATION "${_tampered_handoff}")
file(APPEND "${_tampered_handoff}/RELEASE_PROVENANCE.txt" "tamper\n")
_check_handoff("${_tampered_handoff}" FALSE
    "Copied provenance input tamper rejection")

set(_separated_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff-separated")
_create_handoff("${_separated_handoff}"
    "fixture-security-reviewer" "fixture-product-release-owner" TRUE)
_check_handoff("${_separated_handoff}" TRUE "Separated-role signing handoff verification")
_require_text("${_separated_handoff}/SIGNING_REQUEST.txt" "role_separation_status=PASS")

set(_sensitive_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff-sensitive")
file(MAKE_DIRECTORY "${_sensitive_handoff}")
file(COPY "${_separated_handoff}/" DESTINATION "${_sensitive_handoff}")
string(CONCAT _pgp_private_marker
    "-----BEGIN PGP " "PRIVATE" " KEY BLOCK-----")
file(APPEND "${_sensitive_handoff}/SIGNING_POLICY.txt"
    "${_pgp_private_marker}\nfixture-only\n")
_rewrite_handoff_manifest("${_sensitive_handoff}")
_check_handoff("${_sensitive_handoff}" FALSE
    "Generic private-key armor rejection")

_create_handoff("${ANO_CRYPTO_TEST_WORK_DIR}/handoff-same-role"
    "fixture-reviewer" "FIXTURE-REVIEWER" FALSE)
_create_handoff("${ANO_CRYPTO_TEST_WORK_DIR}/handoff-placeholder-role"
    "TODO" "fixture-product-release-owner" FALSE)

file(MAKE_DIRECTORY "${_separated_handoff}/unexpected")
file(WRITE "${_separated_handoff}/unexpected/private.pem" "fixture-only\n")
_check_handoff("${_separated_handoff}" FALSE "Nested handoff artifact rejection")

set(_sensitive_fixture "${ANO_CRYPTO_TEST_WORK_DIR}/sensitive-marker.txt")
string(CONCAT _rsa_private_marker "-----BEGIN RSA " "PRIVATE" " KEY-----")
file(WRITE "${_sensitive_fixture}" "${_rsa_private_marker}\nfixture-only\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_fixture}" ON _sensitive_reason)
if(NOT _sensitive_reason STREQUAL "private-key PEM marker")
    message(FATAL_ERROR "RSA private-key marker regression was not rejected")
endif()
string(CONCAT _token_assignment "signing" "_token")
file(WRITE "${_sensitive_fixture}" "  export ${_token_assignment} = fixture-only\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_fixture}" ON _sensitive_reason)
if(NOT _sensitive_reason STREQUAL "sensitive assignment")
    message(FATAL_ERROR "Sensitive assignment regression was not rejected")
endif()
file(WRITE "${_sensitive_fixture}" "entropy = 0.09 sec*proc (2 tests)\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_fixture}" ON _sensitive_reason)
if(_sensitive_reason)
    message(FATAL_ERROR "CTest entropy label-time metadata was incorrectly rejected")
endif()
file(WRITE "${_sensitive_fixture}" "signature = 0.12 sec*proc (3 tests)\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_fixture}" ON _sensitive_reason)
if(_sensitive_reason)
    message(FATAL_ERROR "CTest signature label-time metadata was incorrectly rejected")
endif()
file(WRITE "${_sensitive_fixture}" "entropy = fixture-only\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_fixture}" ON _sensitive_reason)
if(NOT _sensitive_reason STREQUAL "sensitive generic value assignment")
    message(FATAL_ERROR "Generic entropy assignment regression was not rejected")
endif()
set(_sensitive_extension_fixture "${ANO_CRYPTO_TEST_WORK_DIR}/fixture.key")
file(WRITE "${_sensitive_extension_fixture}" "fixture-only\n")
ano_crypto_detect_sensitive_signing_material_file(
    "${_sensitive_extension_fixture}" ON _sensitive_reason)
if(NOT _sensitive_reason STREQUAL "credential/signature-like filename")
    message(FATAL_ERROR "Sensitive filename regression was not rejected")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
message(STATUS "Release signing handoff key-free/role-separation regressions passed")
