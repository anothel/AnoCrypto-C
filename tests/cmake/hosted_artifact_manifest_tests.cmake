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
        RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
    if(_expected_success AND NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed unexpectedly:\n${_stdout}${_stderr}")
    elseif(NOT _expected_success AND _result EQUAL 0)
        message(FATAL_ERROR "${_description} unexpectedly succeeded")
    endif()
endfunction()

function(_write_create_wrapper _path _root _sources)
    file(WRITE "${_path}"
        "set(ANO_CRYPTO_SOURCE_DIR [==[${ANO_CRYPTO_SOURCE_DIR}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARTIFACT_ROOT [==[${_root}]==])\n"
        "set(ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES [==[${_sources}]==])\n"
        "set(ANO_CRYPTO_HOSTED_LANE manifest-self-test)\n"
        "set(ANO_CRYPTO_HOSTED_PLATFORM local)\n"
        "set(ANO_CRYPTO_HOSTED_ARCH test)\n"
        "set(ANO_CRYPTO_HOSTED_COMPILER cmake)\n"
        "set(ANO_CRYPTO_HOSTED_COMMIT fixture)\n"
        "include([==[${ANO_CRYPTO_SOURCE_DIR}/tools/create_hosted_artifact_manifest.cmake]==])\n")
endfunction()

set(_sources
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/status.txt;${ANO_CRYPTO_TEST_WORK_DIR}/input/review.md")
set(_create_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-normal.cmake")
_write_create_wrapper("${_create_wrapper}" "${_bundle}" "${_sources}")
set(_create_command "${CMAKE_COMMAND}" -P "${_create_wrapper}")
set(_verify_command
    "${CMAKE_COMMAND}"
    -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
    -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_bundle}
    -DANO_CRYPTO_EXPECTED_HOSTED_LANE=manifest-self-test
    -DANO_CRYPTO_EXPECTED_HOSTED_PLATFORM=local
    -P ${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake)

_run_expect(TRUE "Hosted artifact manifest fixture creation" ${_create_command})
_run_expect(TRUE "Hosted artifact manifest fixture verification" ${_verify_command})

file(APPEND "${_bundle}/status.txt" "tamper\n")
_run_expect(FALSE "Tampered hosted artifact verification" ${_verify_command})
_run_expect(TRUE "Hosted artifact fixture recreation" ${_create_command})

file(WRITE "${_bundle}/unlisted.txt" "unlisted_status=PASS\n")
_run_expect(FALSE "Unlisted hosted artifact verification" ${_verify_command})
file(REMOVE "${_bundle}/unlisted.txt")
_run_expect(TRUE "Hosted artifact verification after unlisted-file removal" ${_verify_command})

# Construct the marker in two pieces so the source itself does not become a
# false positive for repository-wide sensitive-marker policy checks.
string(CONCAT _forbidden_marker "entropy" "_material=fixture-only")
file(WRITE "${ANO_CRYPTO_TEST_WORK_DIR}/input/forbidden.txt" "${_forbidden_marker}\n")
set(_forbidden_sources
    "${ANO_CRYPTO_TEST_WORK_DIR}/input/status.txt;${ANO_CRYPTO_TEST_WORK_DIR}/input/forbidden.txt")
set(_forbidden_wrapper "${ANO_CRYPTO_TEST_WORK_DIR}/create-forbidden.cmake")
_write_create_wrapper("${_forbidden_wrapper}"
    "${ANO_CRYPTO_TEST_WORK_DIR}/forbidden-bundle" "${_forbidden_sources}")
_run_expect(FALSE "Sensitive-marker hosted artifact creation"
    "${CMAKE_COMMAND}" -P "${_forbidden_wrapper}")

message(STATUS "Hosted artifact create/verify/tamper/unlisted/sensitive-marker regressions passed")
