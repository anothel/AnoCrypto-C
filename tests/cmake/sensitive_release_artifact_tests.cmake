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
file(REMOVE "${_fixture}")

# Construct assignments in pieces so the repository source fixture does not
# itself retain any prohibited value-shaped marker.
string(CONCAT _entropy_assignment "entropy" "_material=fixture-only")
string(CONCAT _seed_assignment "seed" "_material=fixture-only")
string(CONCAT _drbg_assignment "drbg" "_state=fixture-only")
string(CONCAT _aes_key_assignment "aes" "_key=fixture-only")
string(CONCAT _aes_iv_assignment "aes" "_iv=fixture-only")
string(CONCAT _plaintext_assignment "plain" "text=fixture-only")
string(CONCAT _ciphertext_assignment "cipher" "text=fixture-only")
string(CONCAT _cmac_assignment "cmac" "_tag=fixture-only")
string(CONCAT _generic_token_assignment "to" "ken=fixture-only")
string(CONCAT _actual_signature_assignment "actual" "_signature=fixture-only")
string(CONCAT _timestamp_response_assignment "timestamp" "_response=fixture-only")
string(CONCAT _generic_entropy_assignment "entro" "py=fixture-only")
string(CONCAT _generic_seed_assignment "se" "ed=fixture-only")
string(CONCAT _api_token_assignment "api_" "token=fixture-only")
string(CONCAT _generic_signature_assignment "signa" "ture=fixture-only")
set(_assignment_index 0)
foreach(_assignment IN ITEMS
        "${_entropy_assignment}"
        "${_seed_assignment}"
        "${_drbg_assignment}"
        "${_aes_key_assignment}"
        "${_aes_iv_assignment}"
        "${_plaintext_assignment}"
        "${_ciphertext_assignment}"
        "${_cmac_assignment}"
        "${_generic_token_assignment}"
        "${_actual_signature_assignment}"
        "${_timestamp_response_assignment}"
        "${_generic_entropy_assignment}"
        "${_generic_seed_assignment}"
        "${_api_token_assignment}"
        "${_generic_signature_assignment}")
    math(EXPR _assignment_index "${_assignment_index} + 1")
    set(_fixture "${_evidence_root}/sensitive-${_assignment_index}.txt")
    file(WRITE "${_fixture}" "${_assignment}\n")
    _run_scan_expect(FALSE
        "Sensitive value assignment rejection ${_assignment_index}")
    file(REMOVE "${_fixture}")
endforeach()

string(CONCAT _generic_token_key "to" "ken")
set(_fixture "${_evidence_root}/duplicate-status.txt")
file(WRITE "${_fixture}"
    "${_generic_token_key}=NOT_RUN\n${_generic_token_key}=fixture-only\n")
_run_scan_expect(FALSE "Duplicate safe/secret status assignment rejection")
file(REMOVE "${_fixture}")

set(_fixture "${_evidence_root}/exported-status.txt")
file(WRITE "${_fixture}" "export ${_generic_token_key}=fixture-only\n")
_run_scan_expect(FALSE "Exported generic token assignment rejection")
file(REMOVE "${_fixture}")

string(CONCAT _json_aes_key "aes" "_key")
set(_fixture "${_evidence_root}/sensitive-record.json")
file(WRITE "${_fixture}"
    "{\"${_generic_token_key}\":\"fixture-only\",\"${_json_aes_key}\":\"fixture-only\"}\n")
_run_scan_expect(FALSE "Sensitive JSON record rejection")
file(REMOVE "${_fixture}")

string(CONCAT _actual_signature_key "actual" "_signature")
string(CONCAT _timestamp_response_key "timestamp" "_response")
string(CONCAT _api_token_key "api_" "token")
string(CONCAT _generic_signature_key "signa" "ture")
string(CONCAT _generic_entropy_key "entro" "py")
string(CONCAT _generic_seed_key "se" "ed")
set(_fixture "${_evidence_root}/safe-status.txt")
file(WRITE "${_fixture}"
    "${_generic_token_key}=NOT_RUN\n"
    "export ${_generic_token_key}=ABSENT\n"
    "${_api_token_key}=NONE\n"
    "${_generic_signature_key}=NOT_CREATED\n"
    "${_generic_entropy_key}=NOT_RUN\n"
    "${_generic_seed_key}=ABSENT\n"
    "${_actual_signature_key}=NOT_CREATED\n"
    "set ${_timestamp_response_key}=NOT_CREATED\n")
_run_scan_expect(TRUE "Explicit no-secret status sentinel acceptance")
file(REMOVE "${_fixture}")

file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
message(STATUS
    "Sensitive source-archive/install/PoC/evidence/value-assignment regressions passed")
