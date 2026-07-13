# Verify and index status-only bundles downloaded from one hosted CI run.
# Bundle integrity and presence do not replace human review of platform results.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_HOSTED_ARTIFACT_ROOT
        ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR
        ANO_CRYPTO_WORKFLOW_COMMIT
        ANO_CRYPTO_WORKFLOW_REF)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_HOSTED_ARTIFACT_ROOT
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/hosted_artifact_path_helpers.cmake")

function(_ano_require_safe_relative_path _path _description)
    string(FIND "${_path}" "\\" _backslash)
    if(_path STREQUAL "" OR NOT _backslash EQUAL -1 OR
       _path MATCHES "^/" OR _path MATCHES "^[A-Za-z]:" OR
       _path MATCHES "(^|/)[.][.](/|$)" OR
       _path MATCHES "(^|/)[.](/|$)" OR _path MATCHES "//" OR
       _path MATCHES "[:|]")
        message(FATAL_ERROR "Unsafe ${_description}: ${_path}")
    endif()
endfunction()

ano_crypto_reject_hosted_root_alias(
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "Hosted artifact input root")
if(EXISTS "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}" OR
   IS_SYMLINK "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}")
    ano_crypto_reject_hosted_root_alias(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}" "Hosted artifact output root")
endif()
ano_crypto_get_hosted_candidate_real_path(
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" _input_real)
ano_crypto_get_hosted_candidate_real_path(
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}" _output_real)
file(RELATIVE_PATH _output_from_input "${_input_real}" "${_output_real}")
file(RELATIVE_PATH _input_from_output "${_output_real}" "${_input_real}")
if(_output_from_input STREQUAL "." OR
   NOT _output_from_input MATCHES "(^|/)[.][.](/|$)" OR
   NOT _input_from_output MATCHES "(^|/)[.][.](/|$)")
    message(FATAL_ERROR "Hosted artifact input and output directories must not overlap")
endif()
foreach(_metadata IN ITEMS "${ANO_CRYPTO_WORKFLOW_COMMIT}" "${ANO_CRYPTO_WORKFLOW_REF}")
    if(_metadata MATCHES "[|;\r\n]" OR _metadata STREQUAL "unknown")
        message(FATAL_ERROR "Hosted workflow identity is invalid")
    endif()
endforeach()
if(ANO_CRYPTO_WORKFLOW_REF MATCHES "^refs/tags/" AND
   NOT ANO_CRYPTO_WORKFLOW_REF STREQUAL "refs/tags/${ANO_CRYPTO_RELEASE_TAG}")
    message(FATAL_ERROR "Hosted workflow tag ref does not match expected release metadata")
endif()

foreach(_role IN ITEMS SECURITY_REVIEWER PRODUCT_RELEASE_OWNER)
    if(NOT DEFINED ANO_CRYPTO_${_role}_IDENTITY OR
       ANO_CRYPTO_${_role}_IDENTITY STREQUAL "")
        set(ANO_CRYPTO_${_role}_IDENTITY "UNPROVISIONED")
    endif()
    string(TOLOWER "${ANO_CRYPTO_${_role}_IDENTITY}" _role_normalized)
    if(NOT ANO_CRYPTO_${_role}_IDENTITY MATCHES "^[A-Za-z0-9_.@:+-]+$" OR
       _role_normalized MATCHES "^(todo|unknown|unreviewed|none)$")
        message(FATAL_ERROR "${_role} identity is invalid")
    endif()
    if(_role_normalized STREQUAL "unprovisioned")
        set(ANO_CRYPTO_${_role}_IDENTITY "UNPROVISIONED")
    endif()
    set(_${_role}_NORMALIZED "${_role_normalized}")
endforeach()
if(NOT ANO_CRYPTO_SECURITY_REVIEWER_IDENTITY STREQUAL "UNPROVISIONED" AND
   NOT ANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY STREQUAL "UNPROVISIONED" AND
   _SECURITY_REVIEWER_NORMALIZED STREQUAL _PRODUCT_RELEASE_OWNER_NORMALIZED)
    message(FATAL_ERROR
        "Security Reviewer and Product Release Owner identities must be different")
endif()
if(NOT ANO_CRYPTO_SECURITY_REVIEWER_IDENTITY STREQUAL "UNPROVISIONED" AND
   NOT ANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY STREQUAL "UNPROVISIONED")
    set(_role_separation_status "PASS")
else()
    set(_role_separation_status "UNPROVISIONED")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}")

file(GLOB_RECURSE _manifests
    LIST_DIRECTORIES false
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/HOSTED_ARTIFACT_MANIFEST.txt"
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/*/HOSTED_ARTIFACT_MANIFEST.txt")
list(REMOVE_DUPLICATES _manifests)
list(SORT _manifests)
if(NOT _manifests)
    message(FATAL_ERROR "No hosted artifact manifests were downloaded")
endif()

set(_bundle_records "")
set(_bundle_keys "")
set(_hosted_bundle_keys "")
set(_registered_files "")
set(_hosted_runner_count 0)
set(_synthetic_local_count 0)
foreach(_manifest IN LISTS _manifests)
    get_filename_component(_bundle_root "${_manifest}" DIRECTORY)
    file(RELATIVE_PATH _bundle_relative
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_bundle_root}")
    if(NOT _bundle_relative STREQUAL ".")
        _ano_require_safe_relative_path(
            "${_bundle_relative}" "hosted artifact bundle path")
    endif()
    file(RELATIVE_PATH _manifest_relative
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_manifest}")
    _ano_require_safe_relative_path(
        "${_manifest_relative}" "hosted artifact manifest path")
    ano_crypto_require_regular_unaliased_hosted_child(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_manifest_relative}"
        "Hosted artifact manifest")

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_bundle_root}
            -DANO_CRYPTO_EXPECTED_HOSTED_COMMIT=${ANO_CRYPTO_WORKFLOW_COMMIT}
            -DANO_CRYPTO_EXPECTED_HOSTED_WORKFLOW_REF=${ANO_CRYPTO_WORKFLOW_REF}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake"
        RESULT_VARIABLE _verify_result
        OUTPUT_VARIABLE _verify_stdout
        ERROR_VARIABLE _verify_stderr
        TIMEOUT 120)
    if(NOT _verify_result EQUAL 0)
        message(FATAL_ERROR
            "Hosted bundle verification failed for ${_manifest}:\n${_verify_stdout}${_verify_stderr}")
    endif()

    foreach(_field IN ITEMS lane platform architecture compiler commit artifact_origin
                            execution_status workflow_commit workflow_ref review_status
                            reviewer review_date disposition)
        set(_${_field} "")
    endforeach()
    file(STRINGS "${_manifest}" _lines)
    foreach(_line IN LISTS _lines)
        if(_line MATCHES "^([a-z_]+)=(.*)$")
            set(_key "${CMAKE_MATCH_1}")
            set(_value "${CMAKE_MATCH_2}")
            if(_key MATCHES
               "^(lane|platform|architecture|compiler|commit|artifact_origin|execution_status|workflow_commit|workflow_ref|review_status|reviewer|review_date|disposition)$")
                set(_${_key} "${_value}")
            endif()
        endif()
    endforeach()

    set(_bundle_key "${_lane}|${_platform}|${_architecture}|${_compiler}")
    if(_bundle_key IN_LIST _bundle_keys)
        message(FATAL_ERROR "Duplicate hosted lane/platform/architecture/compiler bundle: ${_bundle_key}")
    endif()
    list(APPEND _bundle_keys "${_bundle_key}")
    if(_artifact_origin STREQUAL "HOSTED_RUNNER")
        math(EXPR _hosted_runner_count "${_hosted_runner_count} + 1")
        list(APPEND _hosted_bundle_keys "${_bundle_key}")
    elseif(_artifact_origin STREQUAL "SYNTHETIC_LOCAL")
        math(EXPR _synthetic_local_count "${_synthetic_local_count} + 1")
    else()
        message(FATAL_ERROR "Hosted bundle has an invalid artifact origin")
    endif()
    if(_review_status STREQUAL "REVIEWED")
        string(TOLOWER "${_reviewer}" _manifest_reviewer_normalized)
        if(ANO_CRYPTO_SECURITY_REVIEWER_IDENTITY STREQUAL "UNPROVISIONED" OR
           NOT _manifest_reviewer_normalized STREQUAL _SECURITY_REVIEWER_NORMALIZED)
            message(FATAL_ERROR
                "Reviewed hosted bundle is not bound to the Security Reviewer identity")
        endif()
    endif()

    file(SHA256 "${_manifest}" _manifest_hash)
    string(REPLACE "|" "<PIPE>" _record
        "${_bundle_key}|${_commit}|${_workflow_ref}|${_artifact_origin}|${_execution_status}|${_review_status}|${_reviewer}|${_review_date}|${_disposition}|${_manifest_hash}|${_bundle_relative}")
    list(APPEND _bundle_records "${_record}")

    file(GLOB_RECURSE _bundle_files
        LIST_DIRECTORIES false RELATIVE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}"
        "${_bundle_root}/*")
    foreach(_relative IN LISTS _bundle_files)
        _ano_require_safe_relative_path("${_relative}" "hosted artifact file path")
        ano_crypto_require_regular_unaliased_hosted_child(
            "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_relative}"
            "Hosted artifact index input")
        list(APPEND _registered_files "${_relative}")
    endforeach()
endforeach()
list(REMOVE_DUPLICATES _registered_files)

set(_required_keys
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
set(_missing_keys "")
set(_required_hosted_count 0)
foreach(_required_key IN LISTS _required_keys)
    if(NOT _required_key IN_LIST _bundle_keys)
        list(APPEND _missing_keys "${_required_key}")
    elseif(_required_key IN_LIST _hosted_bundle_keys)
        math(EXPR _required_hosted_count "${_required_hosted_count} + 1")
    endif()
endforeach()
list(LENGTH _required_keys _required_count)
if(_required_hosted_count EQUAL _required_count)
    set(_hosted_execution_status "COMPLETED")
else()
    set(_hosted_execution_status "NOT_RUN")
endif()

file(GLOB_RECURSE _artifact_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}"
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/*")
list(SORT _artifact_files)
if(NOT _artifact_files)
    message(FATAL_ERROR "No hosted artifact files were downloaded")
endif()
set(_hash_file
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_FILES.sha256")
file(WRITE "${_hash_file}" "")
foreach(_relative IN LISTS _artifact_files)
    _ano_require_safe_relative_path("${_relative}" "hosted artifact index path")
    set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
    ano_crypto_require_regular_unaliased_hosted_child(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_relative}"
        "Hosted artifact index file")
    if(NOT _relative IN_LIST _registered_files)
        message(FATAL_ERROR "Unregistered hosted artifact index file: ${_relative}")
    endif()
    file(SIZE "${_full}" _size)
    if(_size GREATER 10485760)
        message(FATAL_ERROR "Hosted artifact index file exceeds 10 MiB: ${_relative}")
    endif()
    file(SHA256 "${_full}" _hash)
    file(APPEND "${_hash_file}" "${_hash}  ${_relative}\n")
endforeach()

if(DEFINED ANO_CRYPTO_WORKFLOW_RUN_ID AND
   NOT ANO_CRYPTO_WORKFLOW_RUN_ID STREQUAL "")
    set(_run_id "${ANO_CRYPTO_WORKFLOW_RUN_ID}")
else()
    set(_run_id "unknown")
endif()
list(LENGTH _manifests _bundle_count)
list(LENGTH _artifact_files _artifact_file_count)

set(_index_file
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_INDEX.txt")
file(WRITE "${_index_file}"
    "AnoCrypto-C hosted artifact index V2\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Workflow run id: ${_run_id}\n"
    "Workflow commit: ${ANO_CRYPTO_WORKFLOW_COMMIT}\n"
    "Workflow ref: ${ANO_CRYPTO_WORKFLOW_REF}\n"
    "Verified bundles: ${_bundle_count}\n"
    "Hosted runner bundles: ${_hosted_runner_count}\n"
    "Synthetic local bundles: ${_synthetic_local_count}\n"
    "Indexed files: ${_artifact_file_count}\n"
    "Hosted execution status: ${_hosted_execution_status}\n"
    "Human review status: UNREVIEWED\n"
    "Release handoff status: BLOCKED\n"
    "External release authorized: NO\n"
    "Sensitive cryptographic values intentionally indexed: no\n\n"
    "Bundles (lane|platform|architecture|compiler|commit|workflow-ref|origin|execution|review-status|reviewer|review-date|disposition|manifest-sha256|path):\n")
list(SORT _bundle_records)
foreach(_record IN LISTS _bundle_records)
    string(REPLACE "<PIPE>" "|" _decoded "${_record}")
    file(APPEND "${_index_file}" "- ${_decoded}\n")
endforeach()

if(_missing_keys)
    file(APPEND "${_index_file}" "\nMissing required hosted tuples:\n")
    foreach(_missing IN LISTS _missing_keys)
        file(APPEND "${_index_file}" "- ${_missing}\n")
    endforeach()
    file(APPEND "${_index_file}" "Index integrity: FAIL\n")
    message(FATAL_ERROR "Hosted artifact set is incomplete; see ${_index_file}")
endif()
file(APPEND "${_index_file}"
    "\nManifest re-verification: PASS\n"
    "Required tuple presence: PASS\n"
    "Index integrity: PASS\n"
    "Review disposition: UNREVIEWED\n"
    "Release authorization: BLOCKED\n"
    "Limit: hashes and presence do not constitute human platform, ABI, side-channel, signing, or release review.\n")
file(SHA256 "${_index_file}" _index_sha256)
file(SHA256 "${_hash_file}" _artifact_files_manifest_sha256)

set(_disposition_file
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt")
if(_hosted_execution_status STREQUAL "COMPLETED")
    set(_disposition_run_id "${_run_id}")
    set(_disposition_workflow_ref "${ANO_CRYPTO_WORKFLOW_REF}")
else()
    set(_disposition_run_id "NOT_RUN")
    set(_disposition_workflow_ref "NOT_RUN")
endif()
file(WRITE "${_disposition_file}"
    "ANOCRYPTO_HOSTED_ARTIFACT_DISPOSITION_V2\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "workflow_run_id=${_disposition_run_id}\n"
    "workflow_commit=${ANO_CRYPTO_WORKFLOW_COMMIT}\n"
    "workflow_ref=${_disposition_workflow_ref}\n"
    "required_tuple_count=${_required_count}\n"
    "required_hosted_tuple_count=${_required_hosted_count}\n"
    "hosted_runner_bundle_count=${_hosted_runner_count}\n"
    "synthetic_local_bundle_count=${_synthetic_local_count}\n"
    "artifact_index_sha256=${_index_sha256}\n"
    "artifact_files_manifest_sha256=${_artifact_files_manifest_sha256}\n"
    "hosted_execution_status=${_hosted_execution_status}\n"
    "review_status=UNREVIEWED\n"
    "reviewer=UNPROVISIONED\n"
    "review_date=UNREVIEWED\n"
    "disposition=UNREVIEWED\n"
    "security_reviewer_identity=${ANO_CRYPTO_SECURITY_REVIEWER_IDENTITY}\n"
    "product_release_owner_identity=${ANO_CRYPTO_PRODUCT_RELEASE_OWNER_IDENTITY}\n"
    "role_separation_status=${_role_separation_status}\n"
    "signing_status=UNSIGNED\n"
    "handoff_status=BLOCKED\n"
    "external_release_authorized=NO\n"
    "github_attestation_role=${ANO_CRYPTO_RELEASE_SIGNING_GITHUB_ATTESTATION_ROLE}\n"
    "integrity_status=PASS\n"
    "status=UNREVIEWED\n"
    "result=BLOCKED\n"
    "limit=Manifest integrity and lane presence only; actual human review, signing, timestamping, and release authorization are incomplete.\n")

file(WRITE
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_REVIEW.md"
    "# Hosted Artifact Review Record\n\n"
    "Release: `${ANO_CRYPTO_RELEASE_VERSION}`  \n"
    "Expected tag metadata: `${ANO_CRYPTO_RELEASE_TAG}`  \n"
    "Workflow run: `${_run_id}`  \n"
    "Commit: `${ANO_CRYPTO_WORKFLOW_COMMIT}`  \n"
    "Ref: `${ANO_CRYPTO_WORKFLOW_REF}`\n\n"
    "Hosted runner bundles: `${_hosted_runner_count}`  \n"
    "Synthetic local bundles: `${_synthetic_local_count}`  \n"
    "Hosted execution: `${_hosted_execution_status}`  \n"
    "Review status: `UNREVIEWED`  \n"
    "Handoff status: `BLOCKED`  \n"
    "External release authorized: `NO`\n\n"
    "Every manifest and required tuple was checked mechanically. This record is not human approval. Reviewer and review date remain unprovisioned. GitHub attestation is optional secondary metadata only.\n\n"
    "Do not attach keys, entropy, seed material, DRBG state, generated random bytes, AES values, CMAC tags, private keys, signing credentials, signatures, timestamp responses, tokens, customer data, or internal deployment details.\n")

message(STATUS
    "AnoCrypto-C hosted artifact index created (${_bundle_count} bundles, review UNREVIEWED, release BLOCKED)")
