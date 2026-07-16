# Verify a key-free AnoCrypto-C offline-signing handoff.
cmake_minimum_required(VERSION 3.16)
foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_SIGNING_HANDOFF_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SIGNING_HANDOFF_DIR
    "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")

set(_required_files
    APPROVED_RELEASE_SIGNERS.txt
    APPROVED_TIMESTAMP_POLICY.txt
    HOSTED_ARTIFACT_DISPOSITION.txt
    RELEASE_ARTIFACTS.sha256
    RELEASE_AUTHORIZATION.md
    RELEASE_PROVENANCE.txt
    SIGNING_HANDOFF_FILES.sha256
    SIGNING_HANDOFF_STATUS.txt
    SIGNING_POLICY.txt
    SIGNING_REQUEST.txt)
foreach(_relative IN LISTS _required_files)
    set(_full "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Required signing-handoff file is missing or unsafe: ${_relative}")
    endif()
endforeach()
file(GLOB _actual_files RELATIVE
    "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}" "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/*")
list(SORT _actual_files)
foreach(_relative IN LISTS _actual_files)
    if(IS_DIRECTORY "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}" OR
       IS_SYMLINK "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}")
        message(FATAL_ERROR "Signing handoff rejects directories and symlinks: ${_relative}")
    endif()
endforeach()
set(_expected_files "${_required_files}")
list(SORT _expected_files)
if(NOT _actual_files STREQUAL _expected_files)
    message(FATAL_ERROR "Signing handoff contains an unexpected or missing file")
endif()
foreach(_relative IN LISTS _actual_files)
    if(_relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "[/\\\\]" OR
       _relative MATCHES "[.](sig|asc|pem|key|p12|pfx|tsr|tsq|ed25519)$")
        message(FATAL_ERROR "Unsafe/signature-like file in unsigned handoff: ${_relative}")
    endif()
endforeach()

set(_manifest "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/SIGNING_HANDOFF_FILES.sha256")
file(STRINGS "${_manifest}" _lines)
set(_manifest_paths "")
foreach(_line IN LISTS _lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
        continue()
    endif()
    if(NOT _line MATCHES "^([0-9a-fA-F]+)[ ][ ]([^ ].*)$")
        message(FATAL_ERROR "Malformed signing-handoff hash line: ${_line}")
    endif()
    string(TOLOWER "${CMAKE_MATCH_1}" _expected_hash)
    set(_relative "${CMAKE_MATCH_2}")
    string(LENGTH "${_expected_hash}" _hash_len)
    if(NOT _hash_len EQUAL 64 OR
       _relative STREQUAL "SIGNING_HANDOFF_FILES.sha256" OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "[/\\\\]" OR
       _relative IN_LIST _manifest_paths)
        message(FATAL_ERROR "Unsafe or duplicate signing-handoff manifest entry: ${_line}")
    endif()
    set(_full "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Signing-handoff manifest file is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_full}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Signing-handoff hash mismatch: ${_relative}")
    endif()
    list(APPEND _manifest_paths "${_relative}")
endforeach()
set(_covered "${_actual_files}")
list(REMOVE_ITEM _covered "SIGNING_HANDOFF_FILES.sha256")
list(SORT _covered)
list(SORT _manifest_paths)
if(NOT _manifest_paths STREQUAL _covered)
    message(FATAL_ERROR "Signing-handoff manifest coverage is incomplete")
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
function(_ano_append_block_reason _reason)
    if(_recomputed_reasons STREQUAL "")
        set(_next "${_reason}")
    else()
        set(_next "${_recomputed_reasons},${_reason}")
    endif()
    set(_recomputed_reasons "${_next}" PARENT_SCOPE)
endfunction()

set(_request "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/SIGNING_REQUEST.txt")
foreach(_key IN ITEMS release_version expected_tag source_commit subject subject_sha256
        provenance_sha256 hosted_disposition_sha256 approved_signer_record_sha256
        approved_timestamp_policy_sha256 algorithm signature_encoding signature_filename
        signer_policy_id timestamp_requirement security_reviewer_identity
        security_reviewer_identity_status product_release_owner_identity
        product_release_owner_identity_status role_separation_status readiness block_reasons
        external_release_authorized)
    _ano_read_key("${_request}" "${_key}" "_request_${_key}")
endforeach()
set(_policy "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/SIGNING_POLICY.txt")
foreach(_key IN ITEMS policy_schema policy_id subject algorithm signature_encoding
        signature_filename public_key_format fingerprint_algorithm verification_tool
        openssl_min_major signer_policy_id timestamp_format timestamp_subject
        timestamp_requirement authorization_quorum authorization_roles
        github_attestation_role private_key_in_repository private_key_in_ci)
    _ano_read_key("${_policy}" "${_key}" "_policy_${_key}")
endforeach()
if(NOT _policy_policy_schema STREQUAL ANO_CRYPTO_RELEASE_SIGNING_POLICY_SCHEMA OR
   NOT _policy_policy_id STREQUAL ANO_CRYPTO_RELEASE_SIGNING_POLICY_ID OR
   NOT _policy_subject STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SUBJECT OR
   NOT _policy_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM OR
   NOT _policy_signature_encoding STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING OR
   NOT _policy_signature_filename STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_FILENAME OR
   NOT _policy_public_key_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FORMAT OR
   NOT _policy_fingerprint_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_PUBLIC_KEY_FINGERPRINT OR
   NOT _policy_verification_tool STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TOOL_FAMILY OR
   NOT _policy_openssl_min_major STREQUAL ANO_CRYPTO_RELEASE_SIGNING_OPENSSL_MIN_MAJOR OR
   NOT _policy_signer_policy_id STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID OR
   NOT _policy_timestamp_format STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT OR
   NOT _policy_timestamp_subject STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_SUBJECT OR
   NOT _policy_timestamp_requirement STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT OR
   NOT _policy_authorization_quorum STREQUAL ANO_CRYPTO_RELEASE_SIGNING_AUTHORIZATION_QUORUM OR
   NOT _policy_authorization_roles STREQUAL ANO_CRYPTO_RELEASE_SIGNING_AUTHORIZATION_ROLES OR
   NOT _policy_github_attestation_role STREQUAL ANO_CRYPTO_RELEASE_SIGNING_GITHUB_ATTESTATION_ROLE OR
   NOT _policy_private_key_in_repository STREQUAL "FORBIDDEN" OR
   NOT _policy_private_key_in_ci STREQUAL "FORBIDDEN")
    message(FATAL_ERROR "Signing policy file does not match the source-owned policy")
endif()
if(NOT _request_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _request_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _request_subject STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SUBJECT OR
   NOT _request_algorithm STREQUAL ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM OR
   NOT _request_signature_encoding STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING OR
   NOT _request_signature_filename STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_FILENAME OR
   NOT _request_signer_policy_id STREQUAL ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID OR
   NOT _request_timestamp_requirement STREQUAL ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_REQUIREMENT OR
   NOT _request_external_release_authorized STREQUAL "NO")
    message(FATAL_ERROR "Signing request identity/policy mismatch")
endif()
foreach(_role IN ITEMS security_reviewer product_release_owner)
    set(_role_identity "${_request_${_role}_identity}")
    set(_role_identity_status "${_request_${_role}_identity_status}")
    if(NOT _role_identity MATCHES "^[A-Za-z0-9_.@:+-]+$" OR
       NOT _role_identity_status MATCHES "^(RECORDED|UNPROVISIONED)$")
        message(FATAL_ERROR "Signing request contains an invalid ${_role} identity state")
    endif()
endforeach()
string(TOLOWER "${_request_security_reviewer_identity}" _security_reviewer_identity_normalized)
string(TOLOWER "${_request_product_release_owner_identity}" _product_release_owner_identity_normalized)
foreach(_identity_normalized IN ITEMS
        "${_security_reviewer_identity_normalized}"
        "${_product_release_owner_identity_normalized}")
    if(_identity_normalized MATCHES "^(todo|tbd|unknown|placeholder|none)$")
        message(FATAL_ERROR "Signing request uses a placeholder approval identity")
    endif()
endforeach()
if(NOT _request_security_reviewer_identity STREQUAL "UNPROVISIONED" AND
   NOT _request_product_release_owner_identity STREQUAL "UNPROVISIONED" AND
   _security_reviewer_identity_normalized STREQUAL _product_release_owner_identity_normalized)
    message(FATAL_ERROR
        "Security Reviewer and Product Release Owner identities must be different")
endif()
if(_security_reviewer_identity_normalized STREQUAL "unprovisioned")
    set(_expected_security_reviewer_identity_status "UNPROVISIONED")
else()
    set(_expected_security_reviewer_identity_status "RECORDED")
endif()
if(_product_release_owner_identity_normalized STREQUAL "unprovisioned")
    set(_expected_product_release_owner_identity_status "UNPROVISIONED")
else()
    set(_expected_product_release_owner_identity_status "RECORDED")
endif()
if(_expected_security_reviewer_identity_status STREQUAL "RECORDED" AND
   _expected_product_release_owner_identity_status STREQUAL "RECORDED")
    set(_expected_role_separation_status "PASS")
else()
    set(_expected_role_separation_status "UNPROVISIONED")
endif()
if(NOT _request_security_reviewer_identity_status STREQUAL _expected_security_reviewer_identity_status OR
   NOT _request_product_release_owner_identity_status STREQUAL _expected_product_release_owner_identity_status OR
   NOT _request_role_separation_status STREQUAL _expected_role_separation_status)
    message(FATAL_ERROR "Signing request role identity state is inconsistent")
endif()

foreach(_binding IN ITEMS
        "RELEASE_ARTIFACTS.sha256|${_request_subject_sha256}"
        "RELEASE_PROVENANCE.txt|${_request_provenance_sha256}"
        "HOSTED_ARTIFACT_DISPOSITION.txt|${_request_hosted_disposition_sha256}"
        "APPROVED_RELEASE_SIGNERS.txt|${_request_approved_signer_record_sha256}"
        "APPROVED_TIMESTAMP_POLICY.txt|${_request_approved_timestamp_policy_sha256}")
    string(REPLACE "|" ";" _parts "${_binding}")
    list(GET _parts 0 _relative)
    list(GET _parts 1 _expected_hash)
    string(LENGTH "${_expected_hash}" _hash_len)
    if(NOT _hash_len EQUAL 64 OR NOT _expected_hash MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR "Malformed request hash binding: ${_relative}")
    endif()
    file(SHA256 "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Signing request hash binding mismatch: ${_relative}")
    endif()
endforeach()

set(_provenance "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/RELEASE_PROVENANCE.txt")
foreach(_key IN ITEMS release_version expected_tag actual_tag source_commit worktree_state
        signing_status external_release_authorized result)
    _ano_read_key("${_provenance}" "${_key}" "_prov_${_key}")
endforeach()
set(_disposition "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt")
foreach(_key IN ITEMS release_version expected_tag workflow_commit review_status
        disposition result)
    _ano_read_key("${_disposition}" "${_key}" "_disp_${_key}")
endforeach()
set(_signer "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_RELEASE_SIGNERS.txt")
foreach(_key IN ITEMS status record_scope signer_policy_id signer_role algorithm
        public_key_format fingerprint_algorithm approved_public_key_fingerprint
        key_custody revocation_status)
    _ano_read_key("${_signer}" "${_key}" "_signer_${_key}")
endforeach()
set(_timestamp "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/APPROVED_TIMESTAMP_POLICY.txt")
foreach(_key IN ITEMS status record_scope format timestamp_subject requirement
        tsa_policy_oid trust_anchor_fingerprint trust_anchor_sha256 revocation_review)
    _ano_read_key("${_timestamp}" "${_key}" "_timestamp_${_key}")
endforeach()

set(_recomputed_reasons "")
if(_expected_security_reviewer_identity_status STREQUAL "UNPROVISIONED")
    _ano_append_block_reason("SECURITY_REVIEWER_IDENTITY_UNPROVISIONED")
endif()
if(_expected_product_release_owner_identity_status STREQUAL "UNPROVISIONED")
    _ano_append_block_reason("PRODUCT_RELEASE_OWNER_IDENTITY_UNPROVISIONED")
endif()
if(NOT _prov_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _prov_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _prov_signing_status STREQUAL "UNSIGNED" OR
   NOT _prov_external_release_authorized STREQUAL "NO" OR
   NOT _prov_result STREQUAL "PASS")
    message(FATAL_ERROR "Handoff provenance boundary is invalid")
endif()
if(NOT _prov_worktree_state STREQUAL "CLEAN")
    _ano_append_block_reason("WORKTREE_NOT_CLEAN")
endif()
string(LENGTH "${_prov_source_commit}" _commit_length)
if(NOT (_commit_length EQUAL 40 OR _commit_length EQUAL 64) OR
   NOT _prov_source_commit MATCHES "^[0-9a-fA-F]+$")
    _ano_append_block_reason("SOURCE_COMMIT_NOT_REVIEWABLE")
endif()
if(NOT _request_source_commit STREQUAL _prov_source_commit)
    message(FATAL_ERROR "Signing request source commit does not match provenance")
endif()
if(NOT _prov_actual_tag STREQUAL ANO_CRYPTO_RELEASE_TAG)
    _ano_append_block_reason("EXPECTED_TAG_NOT_RECORDED")
endif()
if(NOT _disp_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _disp_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _disp_review_status STREQUAL "REVIEWED" OR
   NOT _disp_disposition STREQUAL "ACCEPTED_WITH_LIMITATIONS" OR
   NOT _disp_result STREQUAL "BLOCKED")
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
if(_recomputed_reasons STREQUAL "")
    set(_recomputed_readiness "READY_FOR_OFFLINE_SIGNING")
    set(_recomputed_reasons "NONE")
else()
    set(_recomputed_readiness "BLOCKED")
endif()
if(NOT _request_readiness STREQUAL _recomputed_readiness OR
   NOT _request_block_reasons STREQUAL _recomputed_reasons)
    message(FATAL_ERROR "Signing-handoff readiness metadata was not derived from its bound inputs")
endif()

file(READ "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/SIGNING_HANDOFF_STATUS.txt" _status_text)
foreach(_required_text IN ITEMS
        "Readiness: ${_recomputed_readiness}"
        "Block reasons: ${_recomputed_reasons}"
        "Detached signature present: no"
        "Cryptographic signature verification performed: no"
        "RFC3161 timestamp response present: no"
        "Security Reviewer identity: ${_request_security_reviewer_identity}"
        "Security Reviewer identity status: ${_expected_security_reviewer_identity_status}"
        "Product Release Owner identity: ${_request_product_release_owner_identity}"
        "Product Release Owner identity status: ${_expected_product_release_owner_identity_status}"
        "Role separation status: ${_expected_role_separation_status}"
        "External release authorized: NO"
        "Result: PASS")
    string(FIND "${_status_text}" "${_required_text}" _found)
    if(_found EQUAL -1)
        message(FATAL_ERROR "Signing-handoff status omits required boundary: ${_required_text}")
    endif()
endforeach()
file(READ "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/RELEASE_AUTHORIZATION.md" _authorization_text)
foreach(_required_text IN ITEMS
        "Status: BLOCKED"
        "Security Reviewer identity: ${_request_security_reviewer_identity}"
        "Security Reviewer identity status: ${_expected_security_reviewer_identity_status}"
        "Product Release Owner identity: ${_request_product_release_owner_identity}"
        "Product Release Owner identity status: ${_expected_product_release_owner_identity_status}"
        "Role separation status: ${_expected_role_separation_status}"
        "External release authorized: NO"
        "Signing credentials included: no")
    string(FIND "${_authorization_text}" "${_required_text}" _found)
    if(_found EQUAL -1)
        message(FATAL_ERROR "Signing-handoff authorization record is unsafe")
    endif()
endforeach()

foreach(_relative IN LISTS _actual_files)
    ano_crypto_detect_sensitive_signing_material_file(
        "${ANO_CRYPTO_SIGNING_HANDOFF_DIR}/${_relative}"
        ON _handoff_sensitive_reason)
    if(_handoff_sensitive_reason)
        message(FATAL_ERROR
            "Signing handoff contains sensitive material: ${_relative} (${_handoff_sensitive_reason})")
    endif()
endforeach()

if(DEFINED ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT AND
   NOT ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT STREQUAL "")
    get_filename_component(_check_parent "${ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_check_parent}")
    file(WRITE "${ANO_CRYPTO_SIGNING_HANDOFF_CHECK_OUTPUT}"
        "AnoCrypto-C P2-02 retained release signing handoff check\n"
        "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
        "Signing policy: ${ANO_CRYPTO_RELEASE_SIGNING_POLICY_ID}\n"
        "Readiness: ${_recomputed_readiness}\n"
        "Block reasons: ${_recomputed_reasons}\n"
        "Role separation: ${_expected_role_separation_status}\n"
        "Manifest coverage: PASS\n"
        "Hash bindings: PASS\n"
        "Private signing material present: no\n"
        "External release authorized: NO\n"
        "Result: PASS\n")
endif()
message(STATUS "AnoCrypto-C release signing handoff check passed (${_recomputed_readiness})")
