cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_TEST_WORK_DIR
        ANO_CRYPTO_OPENSSL_EXECUTABLE
        ANO_CRYPTO_BUILD_TESTS
        ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES
        ANO_CRYPTO_TEST_CONFIGURATION)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_OPENSSL_EXECUTABLE "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ABSOLUTE)

if(NOT ANO_CRYPTO_BUILD_TESTS OR
   NOT ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES OR
   NOT ANO_CRYPTO_TEST_CONFIGURATION STREQUAL "Debug")
    message(FATAL_ERROR
        "Release-signing fixtures require tests enabled, explicit fixture opt-in, and Debug configuration")
endif()
if(NOT EXISTS "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" OR
   IS_DIRECTORY "${ANO_CRYPTO_OPENSSL_EXECUTABLE}")
    message(FATAL_ERROR "OpenSSL executable is unavailable")
endif()

# Run fixture work in a child process so cleanup occurs even when a positive or
# negative regression fails unexpectedly.
if(NOT ANO_CRYPTO_SIGNING_FIXTURE_CHILD)
    set(_source_with_slash "${ANO_CRYPTO_SOURCE_DIR}/")
    set(_work_with_slash "${ANO_CRYPTO_TEST_WORK_DIR}/")
    get_filename_component(_work_parent "${ANO_CRYPTO_TEST_WORK_DIR}" DIRECTORY)
    string(FIND "${_source_with_slash}" "${_work_with_slash}" _work_contains_source)
    if(_work_parent STREQUAL ANO_CRYPTO_TEST_WORK_DIR OR _work_contains_source EQUAL 0)
        message(FATAL_ERROR
            "Fixture work directory must not be a filesystem root or contain the source tree")
    endif()

    function(_expect_fixture_guard _configuration _build_tests _fixture_opt_in)
        set(_probe_work
            "${ANO_CRYPTO_TEST_WORK_DIR}.guard-${_configuration}-${_build_tests}-${_fixture_opt_in}")
        file(REMOVE_RECURSE "${_probe_work}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}"
                "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
                "-DANO_CRYPTO_TEST_WORK_DIR=${_probe_work}"
                "-DANO_CRYPTO_OPENSSL_EXECUTABLE=${ANO_CRYPTO_OPENSSL_EXECUTABLE}"
                "-DANO_CRYPTO_BUILD_TESTS=${_build_tests}"
                "-DANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=${_fixture_opt_in}"
                "-DANO_CRYPTO_TEST_CONFIGURATION=${_configuration}"
                "-DANO_CRYPTO_SIGNING_FIXTURE_CHILD=ON"
                -P "${CMAKE_CURRENT_LIST_FILE}"
            RESULT_VARIABLE _guard_result
            OUTPUT_VARIABLE _guard_stdout
            ERROR_VARIABLE _guard_stderr
            TIMEOUT 60)
        file(REMOVE_RECURSE "${_probe_work}")
        set(_guard_output "${_guard_stdout}${_guard_stderr}")
        if(_guard_result EQUAL 0 OR
           NOT _guard_output MATCHES
               "Release-signing fixtures require tests enabled")
            message(FATAL_ERROR
                "Signing fixture guard did not reject ${_configuration}/tests=${_build_tests}/opt-in=${_fixture_opt_in}")
        endif()
    endfunction()
    foreach(_release_configuration IN ITEMS Release RelWithDebInfo MinSizeRel)
        _expect_fixture_guard("${_release_configuration}" ON ON)
    endforeach()
    _expect_fixture_guard(Debug OFF ON)
    _expect_fixture_guard(Debug ON OFF)

    file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
            "-DANO_CRYPTO_TEST_WORK_DIR=${ANO_CRYPTO_TEST_WORK_DIR}"
            "-DANO_CRYPTO_OPENSSL_EXECUTABLE=${ANO_CRYPTO_OPENSSL_EXECUTABLE}"
            "-DANO_CRYPTO_BUILD_TESTS=ON"
            "-DANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=ON"
            "-DANO_CRYPTO_TEST_CONFIGURATION=Debug"
            "-DANO_CRYPTO_SIGNING_FIXTURE_CHILD=ON"
            -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE _fixture_result
        OUTPUT_VARIABLE _fixture_stdout
        ERROR_VARIABLE _fixture_stderr
        TIMEOUT 180)
    file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
    if(EXISTS "${ANO_CRYPTO_TEST_WORK_DIR}")
        message(FATAL_ERROR "Release-signing fixture cleanup failed")
    endif()
    if(NOT _fixture_result EQUAL 0)
        message(FATAL_ERROR
            "Release-signing fixture regression failed after cleanup:\n${_fixture_stdout}${_fixture_stderr}")
    endif()
    message(STATUS "Release-signing Ed25519/RFC3161 fixture regressions passed; fixture material removed")
    return()
endif()

file(MAKE_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/archive/anocrypto-c")

function(_run_expect _expected_success _description)
    execute_process(
        COMMAND ${ARGN}
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

execute_process(
    COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" version
    RESULT_VARIABLE _openssl_version_result
    OUTPUT_VARIABLE _openssl_version_stdout
    ERROR_VARIABLE _openssl_version_stderr
    OUTPUT_STRIP_TRAILING_WHITESPACE
    TIMEOUT 60)
if(NOT _openssl_version_result EQUAL 0 OR
   NOT _openssl_version_stdout MATCHES "^OpenSSL 3[.]")
    message(FATAL_ERROR
        "Release-signing fixture worker requires OpenSSL 3.x: ${_openssl_version_stdout}${_openssl_version_stderr}")
endif()

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
    message(FATAL_ERROR "Signing fixture archive creation failed: ${_archive_stdout}${_archive_stderr}")
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
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_provenance_bundle.cmake")

set(_signer_private "${ANO_CRYPTO_TEST_WORK_DIR}/signer-private.pem")
set(_signer_public "${ANO_CRYPTO_TEST_WORK_DIR}/signer-public.pem")
set(_signer_public_der "${ANO_CRYPTO_TEST_WORK_DIR}/signer-public.der")
_run_expect(TRUE "Ephemeral Ed25519 private-key creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" genpkey -algorithm ED25519 -out "${_signer_private}")
_run_expect(TRUE "Ephemeral Ed25519 public-key creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkey -in "${_signer_private}"
        -pubout -out "${_signer_public}")
_run_expect(TRUE "Ed25519 SubjectPublicKeyInfo DER creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkey -pubin -in "${_signer_public}"
        -outform DER -out "${_signer_public_der}")
file(SIZE "${_signer_public_der}" _signer_public_der_size)
file(READ "${_signer_public_der}" _signer_public_der_hex HEX)
string(TOLOWER "${_signer_public_der_hex}" _signer_public_der_hex)
string(SUBSTRING "${_signer_public_der_hex}" 0 24 _signer_public_der_prefix)
if(NOT _signer_public_der_size EQUAL 44 OR
   NOT _signer_public_der_prefix STREQUAL "302a300506032b6570032100")
    message(FATAL_ERROR "Fixture signer is not an Ed25519 SubjectPublicKeyInfo value")
endif()
file(SHA256 "${_signer_public_der}" _signer_fingerprint)
set(_signer_record "${ANO_CRYPTO_TEST_WORK_DIR}/APPROVED_RELEASE_SIGNERS.txt")
file(WRITE "${_signer_record}"
    "ANOCRYPTO_APPROVED_RELEASE_SIGNERS_V1\n"
    "status=APPROVED\n"
    "record_scope=TEST_FIXTURE\n"
    "signer_policy_id=ANO-REL-SIGNER-1\n"
    "signer_role=AnoCrypto Release Signer\n"
    "algorithm=ED25519\n"
    "public_key_format=PEM_SUBJECT_PUBLIC_KEY_INFO\n"
    "fingerprint_algorithm=SHA256_SPKI_DER\n"
    "approved_public_key_fingerprint=${_signer_fingerprint}\n"
    "key_custody=EPHEMERAL_TEST_ONLY\n"
    "provisioned_utc=TEST_FIXTURE\n"
    "revocation_status=ACTIVE\n"
    "note=TEST_FIXTURE only; not a production signer.\n")

set(_tsa_root_private "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-root-private.pem")
set(_tsa_root_cert "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-root-cert.pem")
set(_tsa_private "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-private.pem")
set(_tsa_csr "${ANO_CRYPTO_TEST_WORK_DIR}/tsa.csr")
set(_tsa_cert "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-cert.pem")
set(_tsa_ext "${ANO_CRYPTO_TEST_WORK_DIR}/tsa.ext")
file(WRITE "${_tsa_ext}"
    "basicConstraints=critical,CA:FALSE\n"
    "keyUsage=critical,digitalSignature,nonRepudiation\n"
    "extendedKeyUsage=critical,timeStamping\n"
    "subjectKeyIdentifier=hash\n"
    "authorityKeyIdentifier=keyid,issuer\n")
_run_expect(TRUE "Ephemeral RFC3161 root creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" req -x509 -newkey rsa:2048 -nodes
        -keyout "${_tsa_root_private}" -out "${_tsa_root_cert}"
        -subj "/CN=AnoCrypto-C Test TSA Root" -days 2 -sha256
        -addext "basicConstraints=critical,CA:TRUE"
        -addext "keyUsage=critical,keyCertSign,cRLSign")
_run_expect(TRUE "Ephemeral RFC3161 signer request creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" req -new -newkey rsa:2048 -nodes
        -keyout "${_tsa_private}" -out "${_tsa_csr}"
        -subj "/CN=AnoCrypto-C Test TSA")
_run_expect(TRUE "Ephemeral RFC3161 signer certificate creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" x509 -req -in "${_tsa_csr}"
        -CA "${_tsa_root_cert}" -CAkey "${_tsa_root_private}" -CAcreateserial
        -out "${_tsa_cert}" -days 2 -sha256 -extfile "${_tsa_ext}")
set(_tsa_root_der "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-root-cert.der")
_run_expect(TRUE "RFC3161 trust-anchor DER creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" x509 -in "${_tsa_root_cert}"
        -outform DER -out "${_tsa_root_der}")
file(SHA256 "${_tsa_root_der}" _tsa_root_fingerprint)

set(_tsa_policy_oid "1.3.6.1.4.1.55555.1")
set(_timestamp_policy "${ANO_CRYPTO_TEST_WORK_DIR}/APPROVED_TIMESTAMP_POLICY.txt")
file(WRITE "${_timestamp_policy}"
    "ANOCRYPTO_APPROVED_TIMESTAMP_POLICY_V1\n"
    "status=APPROVED\n"
    "record_scope=TEST_FIXTURE\n"
    "format=RFC3161\n"
    "timestamp_subject=RELEASE_ARTIFACTS.sha256.ed25519\n"
    "requirement=REQUIRED_BEFORE_EXTERNAL_RELEASE\n"
    "tsa_policy_oid=${_tsa_policy_oid}\n"
    "trust_anchor_fingerprint=SHA256_CERT_DER\n"
    "trust_anchor_sha256=${_tsa_root_fingerprint}\n"
    "service_owner=TEST_FIXTURE\n"
    "provisioned_utc=TEST_FIXTURE\n"
    "revocation_review=CURRENT\n"
    "note=TEST_FIXTURE only; not a production timestamp policy.\n")

set(_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff")
_run_expect(TRUE "Test-only signing handoff creation"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${_bundle}"
    "-DANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR=${_handoff}"
    "-DANO_CRYPTO_APPROVED_SIGNER_RECORD=${_signer_record}"
    "-DANO_CRYPTO_APPROVED_TIMESTAMP_POLICY=${_timestamp_policy}"
    "-DANO_CRYPTO_SECURITY_REVIEWER_IDENTITY=TEST_FIXTURE-security-reviewer"
    "-DANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY=TEST_FIXTURE-product-release-owner"
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_signing_handoff.cmake")

set(_signature "${ANO_CRYPTO_TEST_WORK_DIR}/RELEASE_ARTIFACTS.sha256.ed25519")
_run_expect(TRUE "Raw Ed25519 detached-signature creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkeyutl -sign -rawin
        -inkey "${_signer_private}"
        -in "${_handoff}/RELEASE_ARTIFACTS.sha256" -out "${_signature}")
file(SIZE "${_signature}" _signature_size)
if(NOT _signature_size EQUAL 64)
    message(FATAL_ERROR "Fixture Ed25519 signature is not raw 64-byte form")
endif()

set(_tsa_serial "${ANO_CRYPTO_TEST_WORK_DIR}/tsa-serial")
set(_tsa_config "${ANO_CRYPTO_TEST_WORK_DIR}/tsa.cnf")
file(TO_CMAKE_PATH "${ANO_CRYPTO_TEST_WORK_DIR}" _tsa_dir)
file(WRITE "${_tsa_serial}" "01\n")
file(WRITE "${_tsa_config}"
    "[tsa]\n"
    "default_tsa=tsa_config\n"
    "[tsa_config]\n"
    "dir=${_tsa_dir}\n"
    "serial=$dir/tsa-serial\n"
    "signer_cert=$dir/tsa-cert.pem\n"
    "certs=$dir/tsa-root-cert.pem\n"
    "signer_key=$dir/tsa-private.pem\n"
    "signer_digest=sha256\n"
    "default_policy=${_tsa_policy_oid}\n"
    "digests=sha256\n"
    "accuracy=secs:1\n"
    "ordering=yes\n"
    "tsa_name=yes\n"
    "ess_cert_id_chain=no\n")
set(_timestamp_query "${ANO_CRYPTO_TEST_WORK_DIR}/signature.tsq")
set(_timestamp_response "${ANO_CRYPTO_TEST_WORK_DIR}/signature.tsr")
_run_expect(TRUE "RFC3161 timestamp request creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ts -query -data "${_signature}"
        -sha256 -cert -out "${_timestamp_query}")
_run_expect(TRUE "RFC3161 timestamp response creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ts -reply -config "${_tsa_config}"
        -section tsa_config -queryfile "${_timestamp_query}" -out "${_timestamp_response}")

function(_verify_signature _expected_success _description _handoff_dir _signature_file
        _public_key _timestamp_policy_file _trust_anchor _timestamp_file _require_timestamp)
    set(_args
        "${CMAKE_COMMAND}"
        "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
        "-DANO_CRYPTO_SIGNING_HANDOFF_DIR=${_handoff_dir}"
        "-DANO_CRYPTO_DETACHED_SIGNATURE_FILE=${_signature_file}"
        "-DANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE=${_public_key}"
        "-DANO_CRYPTO_APPROVED_SIGNER_RECORD=${_signer_record}"
        "-DANO_CRYPTO_OPENSSL_EXECUTABLE=${ANO_CRYPTO_OPENSSL_EXECUTABLE}"
        "-DANO_CRYPTO_ALLOW_TEST_SIGNER_RECORD=ON"
        "-DANO_CRYPTO_REQUIRE_TIMESTAMP=${_require_timestamp}"
        "-DANO_CRYPTO_SIGNATURE_VERIFY_WORK_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/verify-work")
    if(_require_timestamp)
        list(APPEND _args
            "-DANO_CRYPTO_TIMESTAMP_RESPONSE_FILE=${_timestamp_file}"
            "-DANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE=${_trust_anchor}"
            "-DANO_CRYPTO_APPROVED_TIMESTAMP_POLICY=${_timestamp_policy_file}")
    endif()
    list(APPEND _args
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/verify_release_detached_signature.cmake")
    _run_expect(${_expected_success} "${_description}" ${_args})
endfunction()

_verify_signature(TRUE "Raw Ed25519 detached-signature verification"
    "${_handoff}" "${_signature}" "${_signer_public}" "" "" "" OFF)
_verify_signature(TRUE "RFC3161 timestamp verification"
    "${_handoff}" "${_signature}" "${_signer_public}" "${_timestamp_policy}"
    "${_tsa_root_cert}" "${_timestamp_response}" ON)

string(REPEAT "0" 63 _short_signature_bytes)
set(_short_signature "${ANO_CRYPTO_TEST_WORK_DIR}/short.ed25519")
file(WRITE "${_short_signature}" "${_short_signature_bytes}")
_verify_signature(FALSE "63-byte Ed25519 signature rejection"
    "${_handoff}" "${_short_signature}" "${_signer_public}" "" "" "" OFF)
string(REPEAT "0" 64 _malformed_signature_bytes)
set(_malformed_signature "${ANO_CRYPTO_TEST_WORK_DIR}/malformed.ed25519")
file(WRITE "${_malformed_signature}" "${_malformed_signature_bytes}")
_verify_signature(FALSE "Malformed 64-byte Ed25519 signature rejection"
    "${_handoff}" "${_malformed_signature}" "${_signer_public}" "" "" "" OFF)

set(_other_private "${ANO_CRYPTO_TEST_WORK_DIR}/other-private.pem")
set(_other_public "${ANO_CRYPTO_TEST_WORK_DIR}/other-public.pem")
_run_expect(TRUE "Mismatched Ed25519 private-key creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" genpkey -algorithm ED25519 -out "${_other_private}")
_run_expect(TRUE "Mismatched Ed25519 public-key creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkey -in "${_other_private}"
        -pubout -out "${_other_public}")
_verify_signature(FALSE "Signer SPKI fingerprint mismatch rejection"
    "${_handoff}" "${_signature}" "${_other_public}" "" "" "" OFF)

set(_wrong_timestamp_policy "${ANO_CRYPTO_TEST_WORK_DIR}/WRONG_TIMESTAMP_POLICY.txt")
file(READ "${_timestamp_policy}" _wrong_timestamp_policy_text)
string(REPLACE "tsa_policy_oid=${_tsa_policy_oid}"
    "tsa_policy_oid=1.3.6.1.4.1.55555.2"
    _wrong_timestamp_policy_text "${_wrong_timestamp_policy_text}")
file(WRITE "${_wrong_timestamp_policy}" "${_wrong_timestamp_policy_text}")
set(_wrong_policy_handoff "${ANO_CRYPTO_TEST_WORK_DIR}/handoff-wrong-policy")
_run_expect(TRUE "Wrong-policy test handoff creation"
    "${CMAKE_COMMAND}"
    "-DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}"
    "-DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${_bundle}"
    "-DANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR=${_wrong_policy_handoff}"
    "-DANO_CRYPTO_APPROVED_SIGNER_RECORD=${_signer_record}"
    "-DANO_CRYPTO_APPROVED_TIMESTAMP_POLICY=${_wrong_timestamp_policy}"
    "-DANO_CRYPTO_SECURITY_REVIEWER_IDENTITY=TEST_FIXTURE-security-reviewer"
    "-DANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY=TEST_FIXTURE-product-release-owner"
    -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_release_signing_handoff.cmake")
_verify_signature(FALSE "RFC3161 policy OID mismatch rejection"
    "${_wrong_policy_handoff}" "${_signature}" "${_signer_public}" "${_wrong_timestamp_policy}"
    "${_tsa_root_cert}" "${_timestamp_response}" ON)

set(_other_root_private "${ANO_CRYPTO_TEST_WORK_DIR}/other-root-private.pem")
set(_other_root_cert "${ANO_CRYPTO_TEST_WORK_DIR}/other-root-cert.pem")
_run_expect(TRUE "Mismatched RFC3161 trust-anchor creation"
    "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" req -x509 -newkey rsa:2048 -nodes
        -keyout "${_other_root_private}" -out "${_other_root_cert}"
        -subj "/CN=AnoCrypto-C Other Test TSA Root" -days 2 -sha256
        -addext "basicConstraints=critical,CA:TRUE"
        -addext "keyUsage=critical,keyCertSign,cRLSign")
_verify_signature(FALSE "RFC3161 trust-anchor fingerprint mismatch rejection"
    "${_handoff}" "${_signature}" "${_signer_public}" "${_timestamp_policy}"
    "${_other_root_cert}" "${_timestamp_response}" ON)

set(_malformed_timestamp "${ANO_CRYPTO_TEST_WORK_DIR}/malformed.tsr")
file(WRITE "${_malformed_timestamp}" "TEST_FIXTURE malformed RFC3161 response\n")
_verify_signature(FALSE "Malformed RFC3161 timestamp rejection"
    "${_handoff}" "${_signature}" "${_signer_public}" "${_timestamp_policy}"
    "${_tsa_root_cert}" "${_malformed_timestamp}" ON)

message(STATUS "Release-signing fixture worker completed")
