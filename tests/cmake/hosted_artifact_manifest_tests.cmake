cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_TEST_WORK_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/input")
file(WRITE "${ANO_CRYPTO_TEST_WORK_DIR}/input/status.txt"
    "lane_status=PASS\ncomparison_count=7\n")
file(WRITE "${ANO_CRYPTO_TEST_WORK_DIR}/input/review.md"
    "# Review\n\nStatus-only fixture. No generated values are retained.\n")
set(_bundle "${ANO_CRYPTO_TEST_WORK_DIR}/bundle")

function(_run_expect _expected_success _description)
    execute_process(COMMAND ${ARGN}
        RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr
        TIMEOUT 120)
    if(_expected_success AND NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed unexpectedly:\n${_stdout}${_stderr}")
    elseif(NOT _expected_success AND _result EQUAL 0)
        message(FATAL_ERROR "${_description} unexpectedly succeeded")
    endif()
endfunction()

function(_expect_rejection _diagnostic)
    execute_process(COMMAND ${ARGN}
        RESULT_VARIABLE _result ERROR_VARIABLE _stderr OUTPUT_QUIET TIMEOUT 30)
    string(FIND "${_stderr}" "${_diagnostic}" _position)
    if(NOT _result STREQUAL "1" OR _position EQUAL -1)
        message(FATAL_ERROR "Expected ${_diagnostic}, got ${_result}:\n${_stderr}")
    endif()
endfunction()

function(_require_text _path _text)
    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "${_path} is missing expected text: ${_text}")
    endif()
endfunction()

function(_replace_text _path _from _to)
    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_from}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "${_path} is missing replacement input: ${_from}")
    endif()
    string(REPLACE "${_from}" "${_to}" _content "${_content}")
    file(WRITE "${_path}" "${_content}")
endfunction()

function(_create_directory_alias _target _alias _out_result)
    if(WIN32)
        file(TO_NATIVE_PATH "${_target}" _target_native)
        file(TO_NATIVE_PATH "${_alias}" _alias_native)
        execute_process(
            COMMAND cmd.exe /d /c mklink /J "${_alias_native}" "${_target_native}"
            RESULT_VARIABLE _result OUTPUT_QUIET ERROR_QUIET TIMEOUT 30)
    else()
        file(CREATE_LINK "${_target}" "${_alias}" SYMBOLIC RESULT _result)
    endif()
    set(${_out_result} "${_result}" PARENT_SCOPE)
endfunction()

function(_remove_directory_alias _alias)
    if(WIN32)
        file(TO_NATIVE_PATH "${_alias}" _alias_native)
        execute_process(
            COMMAND cmd.exe /d /c rmdir "${_alias_native}"
            RESULT_VARIABLE _result OUTPUT_QUIET ERROR_QUIET TIMEOUT 30)
        if(NOT _result EQUAL 0)
            message(FATAL_ERROR "Failed to remove hosted-artifact test junction")
        endif()
    else()
        file(REMOVE "${_alias}")
    endif()
endfunction()

function(_write_create_wrapper _path _root _sources _lane _platform _arch _compiler
        _commit _origin _execution _workflow_ref)
    file(WRITE "${_path}"
        "set(ANO_CRYPTO_SOURCE_DIR [==[${ANO_CRYPTO_SOURCE_DIR}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARTIFACT_ROOT [==[${_root}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES [==[${_sources}]==])\n"
        "set(ANO_CRYPTO_HOSTED_LANE [==[${_lane}]==])\n"
        "set(ANO_CRYPTO_HOSTED_PLATFORM [==[${_platform}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARCH [==[${_arch}]==])\n"
        "set(ANO_CRYPTO_HOSTED_COMPILER [==[${_compiler}]==])\n"
        "set(ANO_CRYPTO_HOSTED_COMMIT [==[${_commit}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN [==[${_origin}]==])\n"
        "set(ANO_CRYPTO_HOSTED_EXECUTION_STATUS [==[${_execution}]==])\n"
        "set(ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT [==[${_commit}]==])\n"
        "set(ANO_CRYPTO_HOSTED_WORKFLOW_REF [==[${_workflow_ref}]==])\n")
    list(LENGTH ARGN _review_argument_count)
    if(_review_argument_count EQUAL 3)
        list(GET ARGN 0 _reviewer)
        list(GET ARGN 1 _review_date)
        list(GET ARGN 2 _disposition)
        file(APPEND "${_path}"
            "set(ANO_CRYPTO_HOSTED_REVIEW_STATUS REVIEWED)\n"
            "set(ANO_CRYPTO_HOSTED_REVIEWER [==[${_reviewer}]==])\n"
            "set(ANO_CRYPTO_HOSTED_REVIEW_DATE [==[${_review_date}]==])\n"
            "set(ANO_CRYPTO_HOSTED_DISPOSITION [==[${_disposition}]==])\n")
    elseif(NOT _review_argument_count EQUAL 0)
        message(FATAL_ERROR "Reviewed hosted fixture needs reviewer/date/disposition")
    endif()
    file(APPEND "${_path}"
        "include([==[${ANO_CRYPTO_SOURCE_DIR}/tools/create_hosted_artifact_manifest.cmake]==])\n")
endfunction()

set(_sources
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/status.txt;${ANO_CRYPTO_TEST_WORK_DIR}/input/review.md")
set(_create_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-normal.cmake")
_write_create_wrapper("${_create_wrapper}" "${_bundle}" "${_sources}"
    "manifest-self-test" "local" "test" "cmake" "fixture-commit"
    "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
set(_create_command "${CMAKE_COMMAND}" -P "${_create_wrapper}")
set(_verify_command
    "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_bundle}
    -DANO_CRYPTO_EXPECTED_HOSTED_LANE=manifest-self-test
    -DANO_CRYPTO_EXPECTED_HOSTED_PLATFORM=local
    -P ${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake)
set(_manifest "${_bundle}/HOSTED_ARTIFACT_MANIFEST.txt")

_run_expect(TRUE "Hosted artifact manifest fixture creation" ${_create_command})
_run_expect(TRUE "Hosted artifact manifest fixture verification" ${_verify_command})
_require_text("${_manifest}" "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V2")
_require_text("${_manifest}" "artifact_origin=SYNTHETIC_LOCAL")
_require_text("${_manifest}" "execution_status=NOT_RUN")
_require_text("${_manifest}" "review_status=UNREVIEWED")
_require_text("${_manifest}" "disposition=UNREVIEWED")
_require_text("${_manifest}" "integrity_status=PASS")

file(APPEND "${_bundle}/status.txt" "tamper\n")
_expect_rejection("Hosted artifact size mismatch:" ${_verify_command})
_run_expect(TRUE "Hosted artifact fixture recreation" ${_create_command})
_replace_text("${_bundle}/status.txt" "lane_status=PASS" "lane_status=FAIL")
_expect_rejection("Hosted artifact hash mismatch:" ${_verify_command})
_run_expect(TRUE "Hosted artifact recreation after same-size tampering" ${_create_command})

file(WRITE "${_bundle}/unlisted.txt" "unlisted_status=PASS\n")
_run_expect(FALSE "Unlisted hosted artifact verification" ${_verify_command})
file(REMOVE "${_bundle}/unlisted.txt")
_run_expect(TRUE "Hosted artifact verification after unlisted-file removal" ${_verify_command})

_replace_text("${_manifest}" "file=review.md|" "file=../review.md|")
_expect_rejection("Unsafe hosted artifact manifest entry:" ${_verify_command})
_run_expect(TRUE "Hosted artifact fixture recreation after traversal" ${_create_command})

string(ASCII 92 _backslash)
_replace_text("${_manifest}" "file=review.md|" "file=C:${_backslash}review.md|")
_expect_rejection("Unsafe hosted artifact manifest entry:" ${_verify_command})
_run_expect(TRUE "Hosted artifact fixture recreation after drive path" ${_create_command})

file(CREATE_LINK "${_bundle}/status.txt" "${_bundle}/status-link.txt"
    SYMBOLIC RESULT _link_result)
if(_link_result STREQUAL "0")
    _run_expect(FALSE "Symlink hosted artifact verification" ${_verify_command})
    file(REMOVE "${_bundle}/status-link.txt")
else()
    message(STATUS "Symlink regression skipped because this host cannot create one: ${_link_result}")
endif()

file(SHA256 "${_bundle}/status.txt" _root_alias_sentinel_hash)
set(_root_alias "${ANO_CRYPTO_TEST_WORK_DIR}/bundle-root-alias")
_create_directory_alias("${_bundle}" "${_root_alias}" _root_alias_result)
if(_root_alias_result STREQUAL "0")
    _run_expect(FALSE "Aliased hosted artifact root rejection"
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_root_alias}"
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake")
    set(_root_alias_create_wrapper
        "${ANO_CRYPTO_TEST_WORK_DIR}/create-root-alias.cmake")
    _write_create_wrapper("${_root_alias_create_wrapper}" "${_root_alias}"
        "${_sources}" "alias-self-test" "local" "test" "cmake"
        "fixture-commit" "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
    _run_expect(FALSE "Aliased hosted artifact output replacement rejection"
        "${CMAKE_COMMAND}" -P "${_root_alias_create_wrapper}")
    _remove_directory_alias("${_root_alias}")
    if(NOT EXISTS "${_bundle}/status.txt")
        message(FATAL_ERROR "Root-alias cleanup removed the target bundle")
    endif()
    file(SHA256 "${_bundle}/status.txt" _root_alias_sentinel_hash_after)
    if(NOT _root_alias_sentinel_hash_after STREQUAL _root_alias_sentinel_hash)
        message(FATAL_ERROR "Root-alias regression changed the target bundle")
    endif()
else()
    message(STATUS
        "Root-alias regression skipped because this host cannot create one: ${_root_alias_result}")
endif()

set(_alias_target "${ANO_CRYPTO_TEST_WORK_DIR}/alias-target")
file(MAKE_DIRECTORY "${_alias_target}")
configure_file("${_bundle}/status.txt" "${_alias_target}/status.txt" COPYONLY)
file(SHA256 "${_alias_target}/status.txt" _intermediate_alias_sentinel_hash)
set(_intermediate_alias "${_bundle}/aliased")
_create_directory_alias(
    "${_alias_target}" "${_intermediate_alias}" _intermediate_alias_result)
if(_intermediate_alias_result STREQUAL "0")
    _replace_text("${_manifest}" "file=status.txt|" "file=aliased/status.txt|")
    file(REMOVE "${_bundle}/status.txt")
    _run_expect(FALSE "Intermediate hosted artifact path alias rejection"
        ${_verify_command})
    _remove_directory_alias("${_intermediate_alias}")
    if(NOT EXISTS "${_alias_target}/status.txt")
        message(FATAL_ERROR "Intermediate-alias cleanup removed the target file")
    endif()
    file(SHA256 "${_alias_target}/status.txt" _intermediate_alias_sentinel_hash_after)
    if(NOT _intermediate_alias_sentinel_hash_after STREQUAL
       _intermediate_alias_sentinel_hash)
        message(FATAL_ERROR "Intermediate-alias regression changed the target file")
    endif()
    _run_expect(TRUE "Hosted artifact recreation after path-alias regression"
        ${_create_command})
else()
    message(STATUS
        "Intermediate-alias regression skipped because this host cannot create one: ${_intermediate_alias_result}")
endif()

# Refresh both manifest fields so these fixtures reach the content checks.
function(_refresh_listed_status)
    set(_status "${_bundle}/status.txt")
    file(SIZE "${_status}" _size)
    file(SHA256 "${_status}" _hash)
    file(READ "${_manifest}" _contents)
    string(REGEX MATCH "file=status[.]txt[|]size=[0-9]+[|]sha256=[0-9a-f]+"
        _entry "${_contents}")
    _replace_text("${_manifest}" "${_entry}" "file=status.txt|size=${_size}|sha256=${_hash}")
endfunction()

function(_set_listed_status _data)
    file(WRITE "${_bundle}/status.txt" "${_data}")
    _refresh_listed_status()
endfunction()

# Allowed pairs can resemble forbidden bytes if hex is matched at odd offsets.
string(REPEAT "p0" 1024 _boundary_text)
string(ASCII 9 10 13 32 126 128 255 _allowed)
_set_listed_status("${_boundary_text}${_allowed}\n")
_run_expect(TRUE "Allowed text bytes across scanner chunks" ${_verify_command})

foreach(_code IN ITEMS 1 2 3 4 5 6 7 8 11 12 14 15
        16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 127)
    string(ASCII ${_code} _control)
    _set_listed_status("${_boundary_text}${_control}suffix\n")
    _expect_rejection("Hosted artifact must be text-only: status.txt" ${_verify_command})
endforeach()

# An uncompressed tar of the text fixture supplies raw NUL padding on all hosts.
execute_process(COMMAND "${CMAKE_COMMAND}" -E tar cf status.txt --format=gnutar review.md
    WORKING_DIRECTORY "${_bundle}" RESULT_VARIABLE _tar_result TIMEOUT 30)
file(READ "${_bundle}/status.txt" _nul_hex OFFSET 9 LIMIT 1 HEX)
if(NOT _tar_result STREQUAL "0" OR NOT _nul_hex STREQUAL "00")
    message(FATAL_ERROR "Could not create a raw NUL fixture for hosted artifact verification")
endif()
_refresh_listed_status()
_expect_rejection("Hosted artifact must be text-only: status.txt" ${_verify_command})

string(CONCAT _listed_marker "export sign" "ing_token=fixture-only")
_set_listed_status("${_boundary_text}\n${_listed_marker}\n")
_expect_rejection("Hosted artifact contains forbidden signing material:" ${_verify_command})

# Keep the performance regression separate from the small integrity fixtures.
string(REPEAT "00000000: 70 30 7e ff\tcompiler output status line\n" 65536 _large_text)
_set_listed_status("${_large_text}")
unset(_large_text)
execute_process(COMMAND ${_verify_command}
    RESULT_VARIABLE _large_result OUTPUT_QUIET ERROR_VARIABLE _large_error TIMEOUT 30)
if(NOT _large_result STREQUAL "0")
    message(FATAL_ERROR "Large text verification exceeded its budget or failed: ${_large_result}\n${_large_error}")
endif()
_run_expect(TRUE "Hosted artifact recreation after byte regressions" ${_create_command})

string(REPEAT "x" 10485761 _oversized_content)
file(WRITE "${ANO_CRYPTO_TEST_WORK_DIR}/input/oversized.txt" "${_oversized_content}")
unset(_oversized_content)
set(_oversized_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-oversized.cmake")
_write_create_wrapper("${_oversized_wrapper}"
    "${ANO_CRYPTO_TEST_WORK_DIR}/oversized-bundle"
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/oversized.txt"
    "oversized-self-test" "local" "test" "cmake" "fixture-commit"
    "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
_run_expect(FALSE "Oversized hosted artifact creation"
    "${CMAKE_COMMAND}" -P "${_oversized_wrapper}")
file(REMOVE "${ANO_CRYPTO_TEST_WORK_DIR}/input/oversized.txt")

configure_file("${CMAKE_COMMAND}"
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/binary.txt" COPYONLY)
set(_binary_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-binary.cmake")
_write_create_wrapper("${_binary_wrapper}"
    "${ANO_CRYPTO_TEST_WORK_DIR}/binary-bundle"
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/binary.txt"
    "binary-self-test" "local" "test" "cmake" "fixture-commit"
    "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
_run_expect(FALSE "Binary-like hosted artifact creation"
    "${CMAKE_COMMAND}" -P "${_binary_wrapper}")

# Construct sensitive markers in pieces so repository-wide source scans do not
# mistake regression input for retained cryptographic material.
string(CONCAT _entropy_marker "entropy" "_material =fixture-only")
string(CONCAT _aes_key_marker "aes" "_key =fixture-only")
string(CONCAT _aes_iv_marker "aes" "_iv =fixture-only")
string(CONCAT _plaintext_marker "plain" "text =fixture-only")
string(CONCAT _ciphertext_marker "cipher" "text =fixture-only")
string(CONCAT _cmac_marker "cmac" "_tag =fixture-only")
string(CONCAT _token_marker "to" "ken =fixture-only")
string(CONCAT _actual_signature_marker "actual" "_signature=fixture-only")
string(CONCAT _timestamp_response_marker "timestamp" "_response=fixture-only")
string(CONCAT _export_token_marker "export to" "ken=fixture-only")
string(CONCAT _generic_entropy_marker "entro" "py=fixture-only")
string(CONCAT _generic_seed_marker "se" "ed=fixture-only")
string(CONCAT _api_token_marker "api_" "token=fixture-only")
string(CONCAT _generic_signature_marker "signa" "ture=fixture-only")
string(CONCAT _json_token_key "to" "ken")
string(CONCAT _json_aes_key "aes" "_key")
set(_json_sensitive_marker
    "{\"${_json_token_key}\":\"fixture-only\",\"${_json_aes_key}\":\"fixture-only\"}")
set(_marker_index 0)
foreach(_marker IN ITEMS "${_entropy_marker}" "${_aes_key_marker}" "${_aes_iv_marker}"
                         "${_plaintext_marker}" "${_ciphertext_marker}" "${_cmac_marker}"
                          "${_token_marker}" "${_actual_signature_marker}"
                          "${_timestamp_response_marker}" "${_export_token_marker}"
                          "${_json_sensitive_marker}" "${_generic_entropy_marker}"
                          "${_generic_seed_marker}" "${_api_token_marker}"
                          "${_generic_signature_marker}")
    math(EXPR _marker_index "${_marker_index} + 1")
    set(_forbidden_input "${ANO_CRYPTO_TEST_WORK_DIR}/input/forbidden-${_marker_index}.txt")
    file(WRITE "${_forbidden_input}" "${_marker}\n")
    set(_forbidden_wrapper
        "${ANO_CRYPTO_TEST_WORK_DIR}/create-forbidden-${_marker_index}.cmake")
    _write_create_wrapper("${_forbidden_wrapper}"
        "${ANO_CRYPTO_TEST_WORK_DIR}/forbidden-bundle-${_marker_index}"
        "${_forbidden_input}" "sensitive-self-test" "local" "test" "cmake"
        "fixture-commit" "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
    _run_expect(FALSE "Sensitive-marker hosted artifact creation ${_marker_index}"
        "${CMAKE_COMMAND}" -P "${_forbidden_wrapper}")
endforeach()

set(_duplicate_token_input
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/duplicate-token.txt")
string(CONCAT _duplicate_token_key "to" "ken")
file(WRITE "${_duplicate_token_input}"
    "${_duplicate_token_key}=NOT_RUN\n${_duplicate_token_key}=fixture-only\n")
set(_duplicate_token_wrapper
    "${ANO_CRYPTO_TEST_WORK_DIR}/create-duplicate-token.cmake")
_write_create_wrapper("${_duplicate_token_wrapper}"
    "${ANO_CRYPTO_TEST_WORK_DIR}/duplicate-token-bundle"
    "${_duplicate_token_input}" "sensitive-self-test" "local" "test" "cmake"
    "fixture-commit" "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
_run_expect(FALSE "Safe-sentinel sensitive assignment followed by secret rejection"
    "${CMAKE_COMMAND}" -P "${_duplicate_token_wrapper}")

set(_reviewed_bundle "${ANO_CRYPTO_TEST_WORK_DIR}/reviewed-bundle")
set(_reviewed_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-reviewed.cmake")
_write_create_wrapper("${_reviewed_wrapper}" "${_reviewed_bundle}" "${_sources}"
    "review-self-test" "Linux" "x86_64" "gcc" "fixture-commit"
    "HOSTED_RUNNER" "COMPLETED" "refs/heads/fixture"
    "fixture-reviewer" "2026-07-12T00:00:00Z" "ACCEPTED_WITH_LIMITATIONS")
set(_reviewed_create_command "${CMAKE_COMMAND}" -P "${_reviewed_wrapper}")
set(_reviewed_manifest "${_reviewed_bundle}/HOSTED_ARTIFACT_MANIFEST.txt")
set(_reviewed_verify_command
    "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_reviewed_bundle}
    -P ${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake)
_run_expect(TRUE "Reviewed hosted manifest fixture creation"
    ${_reviewed_create_command})
_run_expect(TRUE "Reviewed hosted manifest fixture verification"
    ${_reviewed_verify_command})

string(CONCAT _pass_disposition "disposition=" "PASS")
_replace_text("${_reviewed_manifest}" "disposition=ACCEPTED_WITH_LIMITATIONS"
    "${_pass_disposition}")
_run_expect(FALSE "False PASS hosted review disposition" ${_reviewed_verify_command})
_run_expect(TRUE "Reviewed fixture recreation after false PASS"
    ${_reviewed_create_command})
string(CONCAT _approved_disposition "disposition=" "APPROVED")
_replace_text("${_reviewed_manifest}" "disposition=ACCEPTED_WITH_LIMITATIONS"
    "${_approved_disposition}")
_run_expect(FALSE "False APPROVED hosted review disposition"
    ${_reviewed_verify_command})

_run_expect(TRUE "Reviewed fixture recreation before reviewer check"
    ${_reviewed_create_command})
_replace_text("${_reviewed_manifest}" "reviewer=fixture-reviewer"
    "reviewer=UNPROVISIONED")
_run_expect(FALSE "Missing hosted reviewer identity" ${_reviewed_verify_command})

_run_expect(TRUE "Reviewed fixture recreation before review-date check"
    ${_reviewed_create_command})
_replace_text("${_reviewed_manifest}" "review_date=2026-07-12T00:00:00Z"
    "review_date=UNREVIEWED")
_run_expect(FALSE "Missing hosted review date" ${_reviewed_verify_command})

# Build the exact required tuple set from synthetic/local fixtures. Presence and
# integrity pass, but hosted execution/review must remain NOT_RUN/UNREVIEWED.
set(_download_root "${ANO_CRYPTO_TEST_WORK_DIR}/downloaded")
set(_wrapper_root "${ANO_CRYPTO_TEST_WORK_DIR}/index-wrappers")
file(MAKE_DIRECTORY "${_download_root}" "${_wrapper_root}")
set(_required_tuples
    "platform-thread-safety-ON|Linux|runner-default|cmake-configured"
    "platform-thread-safety-ON|macOS|runner-default|cmake-configured"
    "platform-thread-safety-ON|Windows|runner-default|cmake-configured"
    "clean-release-symbol-compatibility|Linux|runner-default|cmake-configured"
    "clean-release-symbol-compatibility|macOS|runner-default|cmake-configured"
    "clean-release-symbol-compatibility|Windows|runner-default|cmake-configured"
    "tagged-release-compatibility|Linux|runner-default|cmake-configured"
    "tagged-release-compatibility|macOS|runner-default|cmake-configured"
    "tagged-release-compatibility|Windows|runner-default|cmake-configured"
    "linux-32bit-portability|Linux|x86|gcc-m32"
    "linux-s390x-big-endian|Linux|s390x|gcc-s390x-linux-gnu"
    "aes-cmac-compiler-output-gcc|Linux|x86_64|gcc"
    "aes-cmac-compiler-output-clang|Linux|x86_64|clang"
    "openssl-differential|Linux|x86_64|system-default"
    "release-provenance|Linux|x86_64|gcc"
    "release-signing-handoff|Linux|x86_64|gcc")

function(_create_index_bundle _number _tuple _bundle_name)
    string(REPLACE "|" ";" _parts "${_tuple}")
    list(GET _parts 0 _lane)
    list(GET _parts 1 _platform)
    list(GET _parts 2 _arch)
    list(GET _parts 3 _compiler)
    set(_root "${_download_root}/${_bundle_name}")
    set(_wrapper "${_wrapper_root}/create-${_number}-${_bundle_name}.cmake")
    _write_create_wrapper("${_wrapper}" "${_root}"
        "${ANO_CRYPTO_TEST_WORK_DIR}/input/status.txt"
        "${_lane}" "${_platform}" "${_arch}" "${_compiler}"
        "fixture-commit" "SYNTHETIC_LOCAL" "NOT_RUN" "refs/heads/fixture")
    _run_expect(TRUE "Index bundle creation ${_bundle_name}"
        "${CMAKE_COMMAND}" -P "${_wrapper}")
endfunction()

set(_tuple_index 0)
foreach(_tuple IN LISTS _required_tuples)
    math(EXPR _tuple_index "${_tuple_index} + 1")
    _create_index_bundle("${_tuple_index}" "${_tuple}" "bundle-${_tuple_index}")
endforeach()
set(_index_output "${ANO_CRYPTO_TEST_WORK_DIR}/index-output")
set(_index_command
    "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_download_root}
    -DANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR=${_index_output}
    -DANO_CRYPTO_WORKFLOW_RUN_ID=fixture-run
    -DANO_CRYPTO_WORKFLOW_COMMIT=fixture-commit
    -DANO_CRYPTO_WORKFLOW_REF=refs/heads/fixture
    -P ${ANO_CRYPTO_SOURCE_DIR}/tools/create_hosted_artifact_index.cmake)
_run_expect(TRUE "Synthetic hosted artifact index creation" ${_index_command})
_require_text("${_index_output}/HOSTED_ARTIFACT_INDEX.txt"
    "Synthetic local bundles: 16")
_require_text("${_index_output}/HOSTED_ARTIFACT_INDEX.txt"
    "Hosted execution status: NOT_RUN")
_require_text("${_index_output}/HOSTED_ARTIFACT_INDEX.txt"
    "Human review status: UNREVIEWED")
_require_text("${_index_output}/HOSTED_ARTIFACT_INDEX.txt"
    "manifest-sha256")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "handoff_status=BLOCKED")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "workflow_ref=NOT_RUN")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "external_release_authorized=NO")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "required_hosted_tuple_count=0")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "artifact_index_sha256=")
_require_text("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "artifact_files_manifest_sha256=")
configure_file("${_index_output}/HOSTED_ARTIFACT_DISPOSITION.txt"
    "${ANO_CRYPTO_TEST_WORK_DIR}/HOSTED_ARTIFACT_REVIEW_REGRESSION.txt" COPYONLY)
configure_file("${_index_output}/HOSTED_ARTIFACT_INDEX.txt"
    "${ANO_CRYPTO_TEST_WORK_DIR}/HOSTED_ARTIFACT_MATRIX_REGRESSION.txt" COPYONLY)

file(REMOVE_RECURSE "${_download_root}/bundle-16")
_run_expect(FALSE "Missing required hosted tuple" ${_index_command})
_create_index_bundle("16-restored" "release-signing-handoff|Linux|x86_64|gcc" "bundle-16")

_create_index_bundle("duplicate" "release-signing-handoff|Linux|x86_64|gcc"
    "duplicate-bundle")
_run_expect(FALSE "Duplicate hosted tuple" ${_index_command})
file(REMOVE_RECURSE "${_download_root}/duplicate-bundle")

set(_index_manifest "${_download_root}/bundle-1/HOSTED_ARTIFACT_MANIFEST.txt")
_replace_text("${_index_manifest}" "commit=fixture-commit" "commit=wrong-commit")
_run_expect(FALSE "Hosted artifact commit mismatch" ${_index_command})
_create_index_bundle("1-restored" "platform-thread-safety-ON|Linux|runner-default|cmake-configured"
    "bundle-1")

_replace_text("${_index_manifest}" "workflow_ref=refs/heads/fixture"
    "workflow_ref=refs/heads/wrong")
_run_expect(FALSE "Hosted artifact ref mismatch" ${_index_command})
_create_index_bundle("1-restored-ref"
    "platform-thread-safety-ON|Linux|runner-default|cmake-configured" "bundle-1")

set(_wrong_tag_command
    "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_download_root}
    -DANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR=${_index_output}
    -DANO_CRYPTO_WORKFLOW_COMMIT=fixture-commit
    -DANO_CRYPTO_WORKFLOW_REF=refs/tags/wrong-tag
    -P ${ANO_CRYPTO_SOURCE_DIR}/tools/create_hosted_artifact_index.cmake)
_run_expect(FALSE "Hosted artifact tag ref mismatch" ${_wrong_tag_command})

file(WRITE "${_download_root}/unregistered.txt" "status=UNREVIEWED\n")
_run_expect(FALSE "Unregistered hosted artifact index file" ${_index_command})
file(REMOVE "${_download_root}/unregistered.txt")

set(_same_role_command ${_index_command})
# Move -P and its script to the end after appending role inputs.
list(POP_BACK _same_role_command _index_script)
list(POP_BACK _same_role_command _index_flag)
list(APPEND _same_role_command
    -DANO_CRYPTO_SECURITY_REVIEWER_IDENTITY=fixture-approver
    -DANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY=fixture-approver
    "${_index_flag}" "${_index_script}")
_run_expect(FALSE "Conflicting hosted approval roles" ${_same_role_command})

message(STATUS
    "Hosted artifact V2 manifest/index positive, integrity, path, size, binary, sensitive, review, tuple, identity, and role regressions passed")
