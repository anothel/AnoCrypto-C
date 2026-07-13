cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_TEST_WORK_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")

set(_source_archive_root "${ANO_CRYPTO_TEST_WORK_DIR}/source-archive")
set(_install_root "${ANO_CRYPTO_TEST_WORK_DIR}/install-sdk")
set(_poc_root "${ANO_CRYPTO_TEST_WORK_DIR}/customer-poc")
set(_evidence_root "${ANO_CRYPTO_TEST_WORK_DIR}/evidence")
set(_roots
    "${_source_archive_root}"
    "${_install_root}"
    "${_poc_root}"
    "${_evidence_root}")
foreach(_root IN LISTS _roots)
    file(MAKE_DIRECTORY "${_root}")
    file(WRITE "${_root}/status.txt" "status=PASS\n")
endforeach()

function(_run_scan_expect _expected_success _description)
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
            "-DANO_CRYPTO_SENSITIVE_ARTIFACT_ROOTS=${_roots}"
            "-DANO_CRYPTO_SENSITIVE_ARTIFACT_SCAN_OUTPUT=${ANO_CRYPTO_TEST_WORK_DIR}/scan.txt"
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_sensitive_release_artifacts.cmake"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _stdout
        ERROR_VARIABLE _stderr
        TIMEOUT 60)
    if(_expected_success AND NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed unexpectedly:\n${_stdout}${_stderr}")
    elseif(NOT _expected_success AND _result EQUAL 0)
        message(FATAL_ERROR "${_description} unexpectedly succeeded")
    endif()
endfunction()

_run_scan_expect(TRUE "Clean synthetic release-root scan")

set(_fixture "${_source_archive_root}/unexpected.pem")
file(WRITE "${_fixture}" "fixture-only\n")
_run_scan_expect(FALSE "Source-archive credential suffix rejection")
file(REMOVE "${_fixture}")

string(CONCAT _private_marker "-----BEGIN DSA " "PRIVATE" " KEY-----")
set(_fixture "${_install_root}/include.h")
file(WRITE "${_fixture}" "${_private_marker}\nfixture-only\n")
_run_scan_expect(FALSE "Installed-SDK private-key marker rejection")
file(REMOVE "${_fixture}")

string(CONCAT _token_name "signing" "_token")
set(_fixture "${_poc_root}/settings")
file(WRITE "${_fixture}" "export ${_token_name} = fixture-only\n")
_run_scan_expect(FALSE "PoC signing-token assignment rejection")
file(REMOVE "${_fixture}")

set(_fixture "${_evidence_root}/unexpected.tsr")
file(WRITE "${_fixture}" "fixture-only\n")
_run_scan_expect(FALSE "Evidence timestamp fixture rejection")

file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
message(STATUS "Sensitive source-archive/install/PoC/evidence regressions passed")
