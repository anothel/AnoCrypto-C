# Create a key-free offline-signing handoff from one verified provenance bundle.
# The output contains policy, hashes, and readiness metadata only. It never
# creates or accepts a private key, detached signature, timestamp response, or
# external-release authorization.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_PROVENANCE_BUNDLE_DIR
        ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_BUNDLE_DIR
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")

if(NOT DEFINED ANO_CRYPTO_APPROVED_SIGNER_RECORD OR
   ANO_CRYPTO_APPROVED_SIGNER_RECORD STREQUAL "")
    set(ANO_CRYPTO_APPROVED_SIGNER_RECORD
        "${ANO_CRYPTO_SOURCE_DIR}/release/APPROVED_RELEASE_SIGNERS.txt")
endif()
if(NOT DEFINED ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY OR
   ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY STREQUAL "")
    set(ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY
        "${ANO_CRYPTO_SOURCE_DIR}/release/APPROVED_TIMESTAMP_POLICY.txt")
endif()
get_filename_component(ANO_CRYPTO_APPROVED_SIGNER_RECORD
    "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY
    "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}" ABSOLUTE)

foreach(_input IN ITEMS
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_ARTIFACTS.sha256"
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_PROVENANCE.txt"
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt"
        "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}"
        "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}")
    if(NOT EXISTS "${_input}" OR IS_DIRECTORY "${_input}" OR IS_SYMLINK "${_input}")
        message(FATAL_ERROR "Signing-handoff input is missing or unsafe: ${_input}")
    endif()
endforeach()

# Refuse to derive a handoff from an unverified provenance bundle.
set(_provenance_check "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}.provenance-check.txt")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}
        -DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_provenance_check}
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_bundle.cmake"
    RESULT_VARIABLE _provenance_result
    OUTPUT_VARIABLE _provenance_stdout
    ERROR_VARIABLE _provenance_stderr)
if(NOT _provenance_result EQUAL 0)
    message(FATAL_ERROR
        "Signing handoff requires a valid provenance bundle:\n${_provenance_stdout}${_provenance_stderr}")
endif()
file(REMOVE "${_provenance_check}")

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

function(_ano_append_block_reason _reason)
    if(_block_reasons STREQUAL "")
        set(_next "${_reason}")
    else()
        set(_next "${_block_reasons},${_reason}")
    endif()
    set(_block_reasons "${_next}" PARENT_SCOPE)
endfunction()

set(_provenance "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_PROVENANCE.txt")
foreach(_key IN ITEMS release_version expected_tag actual_tag source_commit
        worktree_state signing_status external_release_authorized result)
    _ano_read_key("${_provenance}" "${_key}" "_prov_${_key}")
endforeach()
set(_disposition "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt")
foreach(_key IN ITEMS release_version expected_tag workflow_commit status result)
    _ano_read_key("${_disposition}" "${_key}" "_disp_${_key}")
endforeach()
set(_signer "${ANO_CRYPTO_APPROVED_SIGNER_RECORD}")
foreach(_key IN ITEMS status record_scope signer_policy_id signer_role algorithm
        public_key_format fingerprint_algorithm approved_public_key_fingerprint
        key_custody revocation_status)
    _ano_read_key("${_signer}" "${_key}" "_signer_${_key}")
endforeach()
set(_timestamp "${ANO_CRYPTO_APPROVED_TIMESTAMP_POLICY}")
foreach(_key IN ITEMS status record_scope format timestamp_subject requirement
        tsa_policy_oid trust_anchor_fingerprint trust_anchor_sha256 revocation_review)
    _ano_read_key("${_timestamp}" "${_key}" "_timestamp_${_key}")
endforeach()

if(NOT _prov_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _prov_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _prov_signing_status STREQUAL "UNSIGNED" OR
   NOT _prov_external_release_authorized STREQUAL "NO" OR
   NOT _prov_result STREQUAL "PASS")
    message(FATAL_ERROR "Provenance identity/signing boundary is incompatible with P1-32 handoff")
endif()

set(_block_reasons "")
if(NOT _prov_worktree_state STREQUAL "CLEAN")
    _ano_append_block_reason("WORKTREE_NOT_CLEAN")
endif()
string(LENGTH "${_prov_source_commit}" _commit_length)
if(NOT (_commit_length EQUAL 40 OR _commit_length EQUAL 64) OR
   NOT _prov_source_commit MATCHES "^[0-9a-fA-F]+$")
    _ano_append_block_reason("SOURCE_COMMIT_NOT_REVIEWABLE")
endif()
if(NOT _prov_actual_tag STREQUAL ANO_CRYPTO_RELEASE_TAG)
    _ano_append_block_reason("EXPECTED_TAG_NOT_RECORDED")
endif()
if(NOT _disp_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _disp_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _disp_status STREQUAL "PASS" OR
   NOT _disp_result STREQUAL "PASS")
    _ano_append_block_reason("HOSTED_ARTIFACT_DISPOSITION_INCOMPLETE")
endif()
if(NOT _disp_workflow_commit STREQUAL _prov_source_commit)
    _ano_append_block_reason("HOSTED_COMMIT_MISMATCH")
endif()
if(NOT _signer_status STREQUAL "APPROVED" OR
   NOT _signer_record_scope STREQUAL "PRODUCTION" OR
   NOT _signer_signer_policy_id STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID OR
   NOT _signer_signer_role STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_ROLE OR
   NOT _signer_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM OR
   NOT _signer_public_key_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FORMAT OR
   NOT _signer_fingerprint_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FINGERPRINT OR
   NOT _signer_key_custody STREQUAL ANO_CRYPTO_RELEASE_SIGNING_KEY_CUSTODY OR
   NOT _signer_revocation_status STREQUAL "ACTIVE")
    _ano_append_block_reason("PRODUCTION_SIGNER_NOT_APPROVED")
endif()
string(LENGTH "${_signer_approved_public_key_fingerprint}" _signer_fp_len)
if(NOT _signer_fp_len EQUAL 64 OR
   NOT _signer_approved_public_key_fingerprint MATCHES "^[0-9a-fA-F]+$")
    _ano_append_block_reason("PRODUCTION_SIGNER_FINGERPRINT_UNAVAILABLE")
endif()
if(NOT _timestamp_status STREQUAL "APPROVED" OR
   NOT _timestamp_record_scope STREQUAL "PRODUCTION" OR
   NOT _timestamp_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT OR
   NOT _timestamp_timestamp_subject STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_SUBJECT OR
   NOT _timestamp_requirement STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT OR
   NOT _timestamp_trust_anchor_fingerprint STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_TRUST_ANCHOR_FINGERPRINT OR
   NOT _timestamp_revocation_review STREQUAL "CURRENT" OR
   NOT _timestamp_tsa_policy_oid MATCHES "^[0-9]+([.][0-9]+)+$")
    _ano_append_block_reason("TIMESTAMP_POLICY_NOT_APPROVED")
endif()
string(LENGTH "${_timestamp_trust_anchor_sha256}" _timestamp_hash_len)
if(NOT _timestamp_hash_len EQUAL 64 OR
   NOT _timestamp_trust_anchor_sha256 MATCHES "^[0-9a-fA-F]+$")
    _ano_append_block_reason("TIMESTAMP_TRUST_ANCHOR_UNAVAILABLE")
endif()

if(_block_reasons STREQUAL "")
    set(_readiness "READY_FOR_OFFLINE_SIGNING")
    set(_block_reasons "NONE")
else()
    set(_readiness "BLOCKED")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}")
foreach(_copy IN ITEMS
        RELEASE_ARTIFACTS.sha256
        RELEASE_PROVENANCE.txt
        HOSTED_ARTIFACT_DISPOSITION.txt)
    configure_file(
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_copy}"
        "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/${_copy}" COPYONLY)
endforeach()
configure_file("${_signer}"
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/APPROVED_RELEASE_SIGNERS.txt" COPYONLY)
configure_file("${_timestamp}"
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/APPROVED_TIMESTAMP_POLICY.txt" COPYONLY)

file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/RELEASE_ARTIFACTS.sha256" _subject_hash)
file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/RELEASE_PROVENANCE.txt" _provenance_hash)
file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt" _disposition_hash)
file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/APPROVED_RELEASE_SIGNERS.txt" _signer_record_hash)
file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/APPROVED_TIMESTAMP_POLICY.txt" _timestamp_record_hash)

file(WRITE "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/SIGNING_POLICY.txt"
    "ANOCRYPTO_RELEASE_SIGNING_POLICY_V1\n"
    "policy_schema=${ANO_CRYPTO_RELEASE_SIGNING_POLICY_SCHEMA}\n"
    "policy_id=${ANO_CRYPTO_RELEASE_SIGNING_POLICY_ID}\n"
    "subject=${ANO_CRYPTO_RELEASE_SIGNING_SUBJECT}\n"
    "algorithm=${ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM}\n"
    "signature_encoding=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING}\n"
    "signature_filename=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_FILENAME}\n"
    "public_key_format=${ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FORMAT}\n"
    "fingerprint_algorithm=${ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FINGERPRINT}\n"
    "verification_tool=${ANO_CRYPTO_RELEASE_SIGNING_TOOL_FAMILY}\n"
    "openssl_min_major=${ANO_CRYPTO_RELEASE_SIGNING_OPENSSL_MIN_MAJOR}\n"
    "signer_policy_id=${ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID}\n"
    "timestamp_format=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT}\n"
    "timestamp_subject=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_SUBJECT}\n"
    "timestamp_requirement=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT}\n"
    "authorization_quorum=${ANO_CRYPTO_RELEASE_SIGNING_AUTHORIZATION_QUORUM}\n"
    "authorization_roles=${ANO_CRYPTO_RELEASE_SIGNING_AUTHORIZATION_ROLES}\n"
    "github_attestation_role=${ANO_CRYPTO_RELEASE_SIGNING_GITHUB_ATTESTATION_ROLE}\n"
    "private_key_in_repository=${ANO_CRYPTO_RELEASE_SIGNING_PRIVATE_KEY_IN_REPOSITORY}\n"
    "private_key_in_ci=${ANO_CRYPTO_RELEASE_SIGNING_PRIVATE_KEY_IN_CI}\n")

file(WRITE "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/SIGNING_REQUEST.txt"
    "ANOCRYPTO_OFFLINE_SIGNING_REQUEST_V1\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "source_commit=${_prov_source_commit}\n"
    "subject=RELEASE_ARTIFACTS.sha256\n"
    "subject_sha256=${_subject_hash}\n"
    "provenance_sha256=${_provenance_hash}\n"
    "hosted_disposition_sha256=${_disposition_hash}\n"
    "approved_signer_record_sha256=${_signer_record_hash}\n"
    "approved_timestamp_policy_sha256=${_timestamp_record_hash}\n"
    "algorithm=${ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM}\n"
    "signature_encoding=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING}\n"
    "signature_filename=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_FILENAME}\n"
    "signer_policy_id=${ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID}\n"
    "timestamp_requirement=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT}\n"
    "readiness=${_readiness}\n"
    "block_reasons=${_block_reasons}\n"
    "external_release_authorized=NO\n")

file(WRITE "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/SIGNING_HANDOFF_STATUS.txt"
    "AnoCrypto-C offline signing handoff status\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Readiness: ${_readiness}\n"
    "Block reasons: ${_block_reasons}\n"
    "Detached signature present: no\n"
    "Cryptographic signature verification performed: no\n"
    "RFC3161 timestamp response present: no\n"
    "External release authorized: NO\n"
    "Private signing key accepted by this generator: no\n"
    "Result: PASS\n")

file(WRITE "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/RELEASE_AUTHORIZATION.md"
    "# Release Authorization Handoff\n\n"
    "Status: BLOCKED\n\n"
    "The handoff readiness is `${_readiness}`. This is not external-release authorization.\n\n"
    "Required after offline signing:\n\n"
    "- [ ] Verify the raw Ed25519 detached signature against the approved production signer record.\n"
    "- [ ] Verify an RFC 3161 timestamp response over the detached-signature file using the single approved trust anchor.\n"
    "- [ ] Record Security Reviewer approval.\n"
    "- [ ] Record Product Release Owner approval.\n"
    "- [ ] Reconfirm hosted artifacts, tag, commit, source SBOM scope, and known limitations.\n\n"
    "External release authorized: NO\n"
    "Signing credentials included: no\n")

set(_handoff_manifest
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/SIGNING_HANDOFF_FILES.sha256")
file(GLOB _handoff_files LIST_DIRECTORIES false RELATIVE
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}"
    "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/*")
list(SORT _handoff_files)
file(WRITE "${_handoff_manifest}" "")
foreach(_relative IN LISTS _handoff_files)
    if(_relative STREQUAL "SIGNING_HANDOFF_FILES.sha256")
        continue()
    endif()
    if(IS_SYMLINK "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/${_relative}")
        message(FATAL_ERROR "Signing handoff rejects symlinks: ${_relative}")
    endif()
    file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}/${_relative}" _hash)
    file(APPEND "${_handoff_manifest}" "${_hash}  ${_relative}\n")
endforeach()

if(NOT DEFINED ANO_CRYPTO_VERIFY_SIGNING_HANDOFF)
    set(ANO_CRYPTO_VERIFY_SIGNING_HANDOFF ON)
endif()
if(ANO_CRYPTO_VERIFY_SIGNING_HANDOFF)
    if(DEFINED ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT AND
       NOT ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT STREQUAL "")
        set(_handoff_check "${ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT}")
    else()
        set(_handoff_check "${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}.check.txt")
    endif()
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_SIGNING_HANDOFF_DIR=${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}
            -DANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT=${_handoff_check}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_signing_handoff.cmake"
        RESULT_VARIABLE _check_result
        OUTPUT_VARIABLE _check_stdout
        ERROR_VARIABLE _check_stderr)
    if(NOT _check_result EQUAL 0)
        message(FATAL_ERROR
            "Generated signing handoff failed verification:\n${_check_stdout}${_check_stderr}")
    endif()
endif()
message(STATUS
    "Created key-free release signing handoff (${_readiness}): ${ANO_CRYPTO_SIGNING_HANDOFF_OUTPUT_DIR}")
