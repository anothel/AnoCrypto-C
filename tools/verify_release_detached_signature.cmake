# Verify one raw Ed25519 detached signature over the exact release handoff
# subject. Production external-release authorization remains outside this tool.
cmake_minimum_required(VERSION 3.16)
foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_SIGNING_HANDOFF_DIR
        ANO_CRYPTO_DETACHED_SIGNATURE_FILE
        ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE
        ANO_CRYPTO_APPROVED_SIGNER_RECORD
        ANO_CRYPTO_OPENSSL_EXECUTABLE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SIGNING_HANDOFF_DIR "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_DETACHED_SIGNATURE_FILE "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE "${ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_APPROVED_SIGNER_RECORD "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_OPENSSL_EXECUTABLE "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")
if(NOT DEFINED ANO_CRYPTO_ALLOW_TEST_SIGNER_RECORD)
    set(ANO_CRYPTO_ALLOW_TEST_SIGNER_RECORD OFF)
endif()
if(NOT DEFINED ANO_CRYPTO_REQUIRE_TIMESTAMP)
    set(ANO_CRYPTO_REQUIRE_TIMESTAMP OFF)
endif()

foreach(_input IN ITEMS
        "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}"
        "${ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE}"
        "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}"
        "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/RELEASE_ARTIFACTS.sha256"
        "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_RELEASE_SIGNERS.txt")
    if(NOT EXISTS "${_input}" OR IS_DIRECTORY "${_input}" OR IS_SYMLINK "${_input}")
        message(FATAL_ERROR "Detached-signature verification input is missing or unsafe: ${_input}")
    endif()
endforeach()
if(NOT EXISTS "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" OR IS_DIRECTORY "${ANO_CRYPTO_OPENSSL_EXECUTABLE}")
    message(FATAL_ERROR "Reviewed OpenSSL executable is unavailable")
endif()

set(_handoff_check "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}.signature-handoff-check.txt")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_SIGNING_HANDOFF_DIR=${ANO_CRYPTO_SIGNING_HANDOFF_DIR}
        -DANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT=${_handoff_check}
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_signing_handoff.cmake"
    RESULT_VARIABLE _handoff_result
    OUTPUT_VARIABLE _handoff_stdout
    ERROR_VARIABLE _handoff_stderr
    TIMEOUT 60)
file(REMOVE "${_handoff_check}")
if(NOT _handoff_result EQUAL 0)
    message(FATAL_ERROR "Detached signature requires a valid handoff:\n${_handoff_stdout}${_handoff_stderr}")
endif()

execute_process(
    COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" version
    RESULT_VARIABLE _version_result
    OUTPUT_VARIABLE _version_stdout
    ERROR_VARIABLE _version_stderr
    OUTPUT_STRIP_TRAILING_WHITESPACE
    TIMEOUT 60)
if(NOT _version_result EQUAL 0 OR
   NOT _version_stdout MATCHES "^OpenSSL ([3-9]|[1-9][0-9]+)[.]")
    message(FATAL_ERROR "Detached-signature verifier requires OpenSSL 3.x or later")
endif()

function(_ano_read_key _path _key _output)
    file(STRINGS "${_path}" _matches REGEX "^${_key}=")
    list(LENGTH _matches _count)
    if(NOT _count EQUAL 1)
        message(FATAL_ERROR "Expected one ${_key}= entry in ${_path}")
    endif()
    list(GET _matches 0 _line)
    string(REGEX REPLACE "^[^=]*=" "" _value "${_line}")
    if(_value MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Unsafe ${_key}= value in ${_path}")
    endif()
    set(${_output} "${_value}" PARENT_SCOPE)
endfunction()

file(SHA256 "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}" _external_signer_hash)
file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_RELEASE_SIGNERS.txt" _bound_signer_hash)
if(NOT _external_signer_hash STREQUAL _bound_signer_hash)
    message(FATAL_ERROR "Approved signer record is not the record bound into the handoff")
endif()
foreach(_key IN ITEMS status record_scope signer_policy_id signer_role algorithm
        public_key_format fingerprint_algorithm approved_public_key_fingerprint
        key_custody revocation_status)
    _ano_read_key("${ANO_CRYPTO_APPROVED_SIGNER_RECORD}" "${_key}" "_signer_${_key}")
endforeach()
if(NOT _signer_status STREQUAL "APPROVED" OR
   NOT _signer_signer_policy_id STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID OR
   NOT _signer_signer_role STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_ROLE OR
   NOT _signer_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM OR
   NOT _signer_public_key_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FORMAT OR
   NOT _signer_fingerprint_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FINGERPRINT OR
   NOT _signer_revocation_status STREQUAL "ACTIVE")
    message(FATAL_ERROR "Approved signer record is incompatible with the signing policy")
endif()
if(_signer_record_scope STREQUAL "PRODUCTION")
    if(NOT _signer_key_custody STREQUAL ANO_CRYPTO_RELEASE_SIGNING_KEY_CUSTODY)
        message(FATAL_ERROR "Production signer custody does not match policy")
    endif()
elseif(_signer_record_scope STREQUAL "TEST_FIXTURE")
    if(NOT ANO_CRYPTO_ALLOW_TEST_SIGNER_RECORD OR
       NOT _signer_key_custody STREQUAL "EPHEMERAL_TEST_ONLY")
        message(FATAL_ERROR "Test signer record requires explicit opt-in and ephemeral-test custody")
    endif()
else()
    message(FATAL_ERROR "Unsupported signer record scope")
endif()
string(LENGTH "${_signer_approved_public_key_fingerprint}" _fingerprint_len)
if(NOT _fingerprint_len EQUAL 64 OR
   NOT _signer_approved_public_key_fingerprint MATCHES "^[0-9a-fA-F]+$")
    message(FATAL_ERROR "Approved signer fingerprint is malformed")
endif()
string(TOLOWER "${_signer_approved_public_key_fingerprint}" _approved_fingerprint)

file(READ "${ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE}" _public_pem)
ano_crypto_detect_sensitive_signing_material_text(
    "${_public_pem}" OFF _public_key_sensitive_reason)
if(_public_key_sensitive_reason)
    message(FATAL_ERROR "Public-key input contains private-key material")
endif()
string(REGEX MATCHALL "-----BEGIN [A-Z0-9 ]+-----" _pem_begins "${_public_pem}")
list(LENGTH _pem_begins _pem_begin_count)
string(REGEX MATCHALL "-----END [A-Z0-9 ]+-----" _pem_ends "${_public_pem}")
list(LENGTH _pem_ends _pem_end_count)
if(NOT _pem_begin_count EQUAL 1 OR NOT _pem_end_count EQUAL 1 OR
   NOT _public_pem MATCHES "-----BEGIN PUBLIC KEY-----" OR
   NOT _public_pem MATCHES "-----END PUBLIC KEY-----")
    message(FATAL_ERROR "Signer public-key file must contain exactly one SubjectPublicKeyInfo PEM block")
endif()

if(DEFINED ANO_CRYPTO_SIGNATURE_VERIFY_WORK_DIR AND
   NOT ANO_CRYPTO_SIGNATURE_VERIFY_WORK_DIR STREQUAL "")
    get_filename_component(_work_root "${ANO_CRYPTO_SIGNATURE_VERIFY_WORK_DIR}" ABSOLUTE)
else()
    get_filename_component(_signature_parent "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}" DIRECTORY)
    set(_work_root "${_signature_parent}")
endif()
if(EXISTS "${_work_root}" AND
   (NOT IS_DIRECTORY "${_work_root}" OR IS_SYMLINK "${_work_root}"))
    message(FATAL_ERROR "Signature verification work root is unsafe")
endif()
file(MAKE_DIRECTORY "${_work_root}")
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _work_nonce)
set(_work "${_work_root}/.anocrypto-signature-verify-${_work_nonce}")
if(EXISTS "${_work}")
    message(FATAL_ERROR "Signature verification work directory collision")
endif()
file(MAKE_DIRECTORY "${_work}")
set(_public_der "${_work}/signer-public.der")
execute_process(
    COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkey -pubin
        -in "${ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE}" -outform DER -out "${_public_der}"
    RESULT_VARIABLE _pkey_result
    OUTPUT_VARIABLE _pkey_stdout
    ERROR_VARIABLE _pkey_stderr
    TIMEOUT 60)
if(NOT _pkey_result EQUAL 0)
    file(REMOVE_RECURSE "${_work}")
    message(FATAL_ERROR "Signer public key is not a reviewed public key:\n${_pkey_stdout}${_pkey_stderr}")
endif()
file(SIZE "${_public_der}" _public_der_size)
file(READ "${_public_der}" _public_der_hex HEX)
string(TOLOWER "${_public_der_hex}" _public_der_hex)
string(SUBSTRING "${_public_der_hex}" 0 24 _public_der_prefix)
if(NOT _public_der_size EQUAL 44 OR
   NOT _public_der_prefix STREQUAL "302a300506032b6570032100")
    file(REMOVE_RECURSE "${_work}")
    message(FATAL_ERROR "Signer public key is not an Ed25519 SubjectPublicKeyInfo value")
endif()
file(SHA256 "${_public_der}" _actual_fingerprint)
if(NOT _actual_fingerprint STREQUAL _approved_fingerprint)
    file(REMOVE_RECURSE "${_work}")
    message(FATAL_ERROR "Signer public-key fingerprint does not match the approved record")
endif()

file(SIZE "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}" _signature_size)
if(NOT _signature_size EQUAL 64)
    file(REMOVE_RECURSE "${_work}")
    message(FATAL_ERROR "Raw Ed25519 detached signature must be exactly 64 bytes")
endif()
set(_subject "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${ANO_CRYPTO_RELEASE_SIGNING_SUBJECT}")
execute_process(
    COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" pkeyutl -verify -rawin -pubin
        -inkey "${ANO_CRYPTO_SIGNER_PUBLIC_KEY_FILE}"
        -in "${_subject}"
        -sigfile "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}"
    RESULT_VARIABLE _verify_result
    OUTPUT_VARIABLE _verify_stdout
    ERROR_VARIABLE _verify_stderr
    TIMEOUT 60)
if(NOT _verify_result EQUAL 0)
    file(REMOVE_RECURSE "${_work}")
    message(FATAL_ERROR "Ed25519 detached signature verification failed")
endif()
file(SHA256 "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}" _signature_hash)
file(SHA256 "${_subject}" _subject_hash)

set(_timestamp_status "NOT_VERIFIED")
set(_timestamp_policy_oid "not-recorded")
if(ANO_CRYPTO_REQUIRE_TIMESTAMP)
    foreach(_required_timestamp
            ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE
            ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE
            ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY)
        if(NOT DEFINED ${_required_timestamp} OR "${${_required_timestamp}}" STREQUAL "")
            file(REMOVE_RECURSE "${_work}")
            message(FATAL_ERROR "${_required_timestamp} is required when timestamp verification is enabled")
        endif()
    endforeach()
    get_filename_component(ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE "${ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE}" ABSOLUTE)
    get_filename_component(ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE "${ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE}" ABSOLUTE)
    get_filename_component(ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}" ABSOLUTE)
    foreach(_input IN ITEMS
            "${ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE}"
            "${ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE}"
            "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}"
            "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_TIMESTAMP_POLICY.txt")
        if(NOT EXISTS "${_input}" OR IS_DIRECTORY "${_input}" OR IS_SYMLINK "${_input}")
            file(REMOVE_RECURSE "${_work}")
            message(FATAL_ERROR "Timestamp verification input is missing or unsafe: ${_input}")
        endif()
    endforeach()
    file(SHA256 "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}" _external_timestamp_policy_hash)
    file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_TIMESTAMP_POLICY.txt" _bound_timestamp_policy_hash)
    if(NOT _external_timestamp_policy_hash STREQUAL _bound_timestamp_policy_hash)
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Timestamp policy is not the record bound into the handoff")
    endif()
    foreach(_key IN ITEMS status record_scope format timestamp_subject requirement
            tsa_policy_oid trust_anchor_fingerprint trust_anchor_sha256 revocation_review)
        _ano_read_key("${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}" "${_key}" "_tsa_${_key}")
    endforeach()
    if(NOT _tsa_status STREQUAL "APPROVED" OR
       NOT (_tsa_record_scope STREQUAL "PRODUCTION" OR
            (_tsa_record_scope STREQUAL "TEST_FIXTURE" AND ANO_CRYPTO_ALLOW_TEST_SIGNER_RECORD)) OR
       NOT _tsa_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT OR
       NOT _tsa_timestamp_subject STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_SUBJECT OR
       NOT _tsa_requirement STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT OR
       NOT _tsa_trust_anchor_fingerprint STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_TRUST_ANCHOR_FINGERPRINT OR
       NOT _tsa_revocation_review STREQUAL "CURRENT" OR
       NOT _tsa_tsa_policy_oid MATCHES "^[0-9]+([.][0-9]+)+$")
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Approved timestamp policy is incompatible with the release policy")
    endif()
    string(LENGTH "${_tsa_trust_anchor_sha256}" _tsa_hash_len)
    if(NOT _tsa_hash_len EQUAL 64 OR
       NOT _tsa_trust_anchor_sha256 MATCHES "^[0-9a-fA-F]+$")
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Timestamp trust-anchor fingerprint is malformed")
    endif()
    string(TOLOWER "${_tsa_trust_anchor_sha256}" _approved_tsa_hash)

    file(READ "${ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE}" _ca_pem)
    ano_crypto_detect_sensitive_signing_material_text(
        "${_ca_pem}" OFF _ca_sensitive_reason)
    string(REGEX MATCHALL "-----BEGIN [A-Z0-9 ]+-----" _ca_begins "${_ca_pem}")
    list(LENGTH _ca_begins _ca_begin_count)
    string(REGEX MATCHALL "-----END [A-Z0-9 ]+-----" _ca_ends "${_ca_pem}")
    list(LENGTH _ca_ends _ca_end_count)
    if(_ca_sensitive_reason OR
       NOT _ca_begin_count EQUAL 1 OR NOT _ca_end_count EQUAL 1 OR
       NOT _ca_pem MATCHES "-----BEGIN CERTIFICATE-----" OR
       NOT _ca_pem MATCHES "-----END CERTIFICATE-----")
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Timestamp trust input must contain exactly one certificate and no private key")
    endif()
    set(_ca_der "${_work}/timestamp-trust-anchor.der")
    execute_process(
        COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" x509
            -in "${ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE}" -outform DER -out "${_ca_der}"
        RESULT_VARIABLE _x509_result
        OUTPUT_VARIABLE _x509_stdout
        ERROR_VARIABLE _x509_stderr
        TIMEOUT 60)
    if(NOT _x509_result EQUAL 0)
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Timestamp trust anchor is not a valid certificate")
    endif()
    file(SHA256 "${_ca_der}" _actual_tsa_hash)
    if(NOT _actual_tsa_hash STREQUAL _approved_tsa_hash)
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "Timestamp trust-anchor fingerprint does not match policy")
    endif()
    execute_process(
        COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ts -verify
            -data "${ANO_CRYPTO_DETACHED_SIGNATURE_FILE}"
            -in "${ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE}"
            -CAfile "${ANO_CRYPTO_TIMESTAMP_TRUST_ANCHOR_FILE}"
        RESULT_VARIABLE _ts_verify_result
        OUTPUT_VARIABLE _ts_verify_stdout
        ERROR_VARIABLE _ts_verify_stderr
        TIMEOUT 60)
    if(NOT _ts_verify_result EQUAL 0)
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "RFC3161 timestamp response verification failed")
    endif()
    execute_process(
        COMMAND "${ANO_CRYPTO_OPENSSL_EXECUTABLE}" ts -reply
            -in "${ANO_CRYPTO_TIMESTAMP_RESPONSE_FILE}" -text
        RESULT_VARIABLE _ts_text_result
        OUTPUT_VARIABLE _ts_text_stdout
        ERROR_VARIABLE _ts_text_stderr
        TIMEOUT 60)
    if(NOT _ts_text_result EQUAL 0 OR
       NOT _ts_text_stdout MATCHES "Policy OID:[ \t]*${_tsa_tsa_policy_oid}([\r\n]|$)")
        file(REMOVE_RECURSE "${_work}")
        message(FATAL_ERROR "RFC3161 timestamp policy OID does not match approved policy")
    endif()
    set(_timestamp_status "VERIFIED")
    set(_timestamp_policy_oid "${_tsa_tsa_policy_oid}")
endif()

if(DEFINED ANO_CRYPTO_SIGNATURE_VERIFY_OUTPUT AND
   NOT ANO_CRYPTO_SIGNATURE_VERIFY_OUTPUT STREQUAL "")
    get_filename_component(_output_parent "${ANO_CRYPTO_SIGNATURE_VERIFY_OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_output_parent}")
    file(WRITE "${ANO_CRYPTO_SIGNATURE_VERIFY_OUTPUT}"
        "AnoCrypto-C P1-32 detached-signature verification\n"
        "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
        "Expected tag: ${ANO_CRYPTO_RELEASE_TAG}\n"
        "Subject: ${ANO_CRYPTO_RELEASE_SIGNING_SUBJECT}\n"
        "Subject SHA-256: ${_subject_hash}\n"
        "Algorithm: ${ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM}\n"
        "Signature encoding: ${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING}\n"
        "Signature SHA-256: ${_signature_hash}\n"
        "Signer public-key fingerprint: ${_actual_fingerprint}\n"
        "Signer record scope: ${_signer_record_scope}\n"
        "Detached signature verification: PASS\n"
        "RFC3161 timestamp verification: ${_timestamp_status}\n"
        "Timestamp policy OID: ${_timestamp_policy_oid}\n"
        "External release authorized: NO\n"
        "Result: PASS\n")
endif()
file(REMOVE_RECURSE "${_work}")
message(STATUS "AnoCrypto-C Ed25519 detached signature verification passed")
