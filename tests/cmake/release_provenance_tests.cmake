cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_TEST_WORK_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c")

set(_file_list "${ANO_CRYPTO_TEST_WORK_DIR}/SOURCE_ARCHIVE_CONTENTS.txt")
file(WRITE "${_file_list}" "LICENSE\nREADME.md\n")
configure_file(
    "${ANO_CRYPTO_SOURCE_DIR}/LICENSE"
    "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c/LICENSE" COPYONLY)
configure_file(
    "${ANO_CRYPTO_SOURCE_DIR}/README.md"
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
    message(FATAL_ERROR "Fixture archive creation failed: ${_archive_stdout}${_archive_stderr}")
endif()

set(_bundle "${ANO_CRYPTO_TEST_WORK_DIR}/bundle")
set(_check "${ANO_CRYPTO_TEST_WORK_DIR}/RELEASE_PROVENANCE_CHECK.txt")

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

function(_create_bundle _disposition_file)
    set(_args
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${_bundle}"
        "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
        "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
        "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
        "-DANO_CRYPTO_PROVENANCE_COMMIT=fixture-commit"
        "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=CLEAN"
        "-DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_check}")
    if(NOT "${_disposition_file}" STREQUAL "")
        list(APPEND _args
            "-DANO_CRYPTO_HOSTED_ARTIFACT_DISPOSITION_FILE=${_disposition_file}")
    endif()
    list(APPEND _args
        "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")
    _run_expect(TRUE "Release provenance fixture creation" ${_args})
endfunction()

function(_verify_bundle _expected_success _description)
    _run_expect(${_expected_success} "${_description}"
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${_bundle}"
        "-DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_check}"
        "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_bundle.cmake")
endfunction()

function(_rewrite_manifest)
    file(GLOB _files
        LIST_DIRECTORIES false
        RELATIVE "${_bundle}"
        "${_bundle}/*")
    list(SORT _files)
    file(WRITE "${_bundle}/RELEASE_ARTIFACTS.sha256" "")
    foreach(_relative IN LISTS _files)
        if(_relative STREQUAL "RELEASE_ARTIFACTS.sha256")
            continue()
        endif()
        file(SHA256 "${_bundle}/${_relative}" _hash)
        file(APPEND "${_bundle}/RELEASE_ARTIFACTS.sha256"
            "${_hash}  ${_relative}\n")
    endforeach()
endfunction()

function(_refresh_provenance_hash _key _relative)
    file(SHA256 "${_bundle}/${_relative}" _new_hash)
    file(READ "${_bundle}/RELEASE_PROVENANCE.txt" _provenance)
    string(REGEX REPLACE
        "${_key}=[0-9a-fA-F]+"
        "${_key}=${_new_hash}"
        _provenance "${_provenance}")
    file(WRITE "${_bundle}/RELEASE_PROVENANCE.txt" "${_provenance}")
    _rewrite_manifest()
endfunction()

_create_bundle("")
_verify_bundle(TRUE "Unsigned provenance fixture verification")

_run_expect(TRUE "Dirty untagged engineering provenance acceptance"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/dirty-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-DANO_CRYPTO_PROVENANCE_COMMIT=fixture-commit"
    "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=DIRTY"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

# A semantic SBOM mutation is rebound into the outer hashes so that the
# verifier must reject the path/checksum record itself rather than only the
# outer artifact hash.
_create_bundle("")
file(SHA1 "${ANO_CRYPTO_SOURCE_DIR}/LICENSE" _license_sha1)
file(SHA1 "${ANO_CRYPTO_SOURCE_DIR}/README.md" _readme_sha1)
file(READ "${_bundle}/SOURCE_SBOM.spdx.json" _semantic_sbom)
string(REPLACE
    "\"checksumValue\": \"${_license_sha1}\""
    "\"checksumValue\": \"${_readme_sha1}\""
    _semantic_sbom "${_semantic_sbom}")
file(WRITE "${_bundle}/SOURCE_SBOM.spdx.json" "${_semantic_sbom}")
_refresh_provenance_hash("source_sbom_sha256" "SOURCE_SBOM.spdx.json")
_verify_bundle(FALSE "Semantically mismatched SBOM file-record verification")

_create_bundle("")
file(APPEND "${_bundle}/SOURCE_SBOM.spdx.json" "tamper\n")
_verify_bundle(FALSE "Tampered source SBOM verification")

_create_bundle("")
file(WRITE "${_bundle}/unlisted.txt" "unlisted=1\n")
_verify_bundle(FALSE "Unlisted provenance artifact verification")

_create_bundle("")
file(WRITE "${_bundle}/release.sig" "not-a-signature\n")
_rewrite_manifest()
_verify_bundle(FALSE "Unsupported signature-like artifact rejection")

_create_bundle("")
file(READ "${_bundle}/SIGNING_STATUS.txt" _signing_status)
string(REPLACE "Status: UNSIGNED" "Status: SIGNED" _signing_status "${_signing_status}")
file(WRITE "${_bundle}/SIGNING_STATUS.txt" "${_signing_status}")
_rewrite_manifest()
_verify_bundle(FALSE "Unverified signed-status claim")

_create_bundle("")
file(READ "${_bundle}/RELEASE_AUTHORIZATION.md" _authorization)
string(REPLACE "Status: TODO" "Status: APPROVED" _authorization "${_authorization}")
string(REPLACE "External release authorized: NO" "External release authorized: YES"
    _authorization "${_authorization}")
file(WRITE "${_bundle}/RELEASE_AUTHORIZATION.md" "${_authorization}")
_rewrite_manifest()
_verify_bundle(FALSE "Unverified external-release authorization claim")

set(_unsafe_list "${ANO_CRYPTO_TEST_WORK_DIR}/unsafe-files.txt")
file(WRITE "${_unsafe_list}" "../README.md\n")
_run_expect(FALSE "Unsafe source-file path rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/unsafe-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_unsafe_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

set(_content_mismatch_root "${ANO_CRYPTO_TEST_WORK_DIR}/content-mismatch")
file(MAKE_DIRECTORY "${_content_mismatch_root}/anocrypto-c")
configure_file(
    "${ANO_CRYPTO_SOURCE_DIR}/LICENSE"
    "${_content_mismatch_root}/anocrypto-c/LICENSE" COPYONLY)
file(WRITE "${_content_mismatch_root}/anocrypto-c/README.md"
    "archive content does not match the reviewed source tree\n")
set(_content_mismatch_archive
    "${ANO_CRYPTO_TEST_WORK_DIR}/content-mismatch.tar.gz")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_content_mismatch_archive}" anocrypto-c
    WORKING_DIRECTORY "${_content_mismatch_root}"
    RESULT_VARIABLE _content_mismatch_result
    TIMEOUT 60)
if(NOT _content_mismatch_result EQUAL 0)
    message(FATAL_ERROR "Content-mismatch archive fixture creation failed")
endif()
_run_expect(FALSE "Source archive content-binding rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/content-mismatch-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_content_mismatch_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

set(_member_mismatch_root "${ANO_CRYPTO_TEST_WORK_DIR}/member-mismatch")
file(COPY "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c"
    DESTINATION "${_member_mismatch_root}")
file(WRITE "${_member_mismatch_root}/anocrypto-c/EXTRA.txt" "unexpected member\n")
set(_member_mismatch_archive
    "${ANO_CRYPTO_TEST_WORK_DIR}/member-mismatch.tar.gz")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_member_mismatch_archive}" anocrypto-c
    WORKING_DIRECTORY "${_member_mismatch_root}"
    RESULT_VARIABLE _member_mismatch_result
    TIMEOUT 60)
if(NOT _member_mismatch_result EQUAL 0)
    message(FATAL_ERROR "Member-mismatch archive fixture creation failed")
endif()
_run_expect(FALSE "Source archive member-list rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/member-mismatch-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_member_mismatch_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

_run_expect(FALSE "Wrong actual release tag rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/wrong-tag-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-DANO_CRYPTO_PROVENANCE_ACTUAL_TAG=v0.1.0-wrong"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
_run_expect(FALSE "Dirty worktree actual-tag rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/dirty-tag-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-DANO_CRYPTO_PROVENANCE_COMMIT=fixture-commit"
    "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=DIRTY"
    "-DANO_CRYPTO_PROVENANCE_ACTUAL_TAG=${ANO_CRYPTO_RELEASE_TAG}"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

set(_complete_disposition "${ANO_CRYPTO_TEST_WORK_DIR}/complete-disposition.txt")
file(WRITE "${_complete_disposition}"
    "ANOCRYPTO_HOSTED_ARTIFACT_DISPOSITION_V1\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "workflow_run_id=fixture-run\n"
    "workflow_commit=fixture-commit\n"
    "reviewer=fixture-reviewer\n"
    "reviewed_utc=2026-07-12T00:00:00Z\n"
    "platform_review=PASS\n"
    "compiler_output_review=PASS\n"
    "differential_review=PASS\n"
    "tag_commit_review=PASS\n"
    "status=PASS\n"
    "result=PASS\n"
    "limit=Fixture only; no support or validation claim.\n")
_create_bundle("${_complete_disposition}")
_verify_bundle(TRUE "Complete hosted-disposition fixture verification")

set(_incomplete_disposition "${ANO_CRYPTO_TEST_WORK_DIR}/incomplete-disposition.txt")
file(WRITE "${_incomplete_disposition}"
    "ANOCRYPTO_HOSTED_ARTIFACT_DISPOSITION_V1\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "workflow_run_id=fixture-run\n"
    "workflow_commit=fixture-commit\n"
    "reviewer=TODO\n"
    "reviewed_utc=TODO\n"
    "platform_review=PASS\n"
    "compiler_output_review=PASS\n"
    "differential_review=PASS\n"
    "tag_commit_review=PASS\n"
    "status=PASS\n"
    "result=PASS\n"
    "limit=Incomplete fixture.\n")
_run_expect(FALSE "Incomplete hosted disposition rejection"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_OUTPUT_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/incomplete-bundle"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE=${_archive}"
    "-DANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST=${_file_list}"
    "-DANO_CRYPTO_PROVENANCE_CREATED_UTC=2026-07-12T00:00:00Z"
    "-DANO_CRYPTO_PROVENANCE_COMMIT=fixture-commit"
    "-DANO_CRYPTO_PROVENANCE_WORKTREE_STATE=CLEAN"
    "-DANO_CRYPTO_HOSTED_ARTIFACT_DISPOSITION_FILE=${_incomplete_disposition}"
    "-P" "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

message(STATUS
    "Release provenance create/verify/SBOM/archive/worktree/tamper/signing/authorization/path/tag/disposition regressions passed")
