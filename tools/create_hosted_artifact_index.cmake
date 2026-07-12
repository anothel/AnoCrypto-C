# Verify and index status-only bundles downloaded from one hosted CI run.
# Bundle integrity and presence do not replace human review of platform results.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_HOSTED_ARTIFACT_ROOT
        ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR)
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

if(NOT IS_DIRECTORY "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    message(FATAL_ERROR
        "Hosted artifact root is unavailable: ${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
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
foreach(_manifest IN LISTS _manifests)
    get_filename_component(_bundle_root "${_manifest}" DIRECTORY)
    file(STRINGS "${_manifest}" _lines)
    set(_lane "")
    set(_platform "")
    set(_release_version "")
    set(_commit "")
    foreach(_line IN LISTS _lines)
        if(_line MATCHES "^lane=(.*)$")
            set(_lane "${CMAKE_MATCH_1}")
        elseif(_line MATCHES "^platform=(.*)$")
            set(_platform "${CMAKE_MATCH_1}")
        elseif(_line MATCHES "^release_version=(.*)$")
            set(_release_version "${CMAKE_MATCH_1}")
        elseif(_line MATCHES "^commit=(.*)$")
            set(_commit "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    if(_lane STREQUAL "" OR _platform STREQUAL "" OR
       _release_version STREQUAL "" OR _commit STREQUAL "")
        message(FATAL_ERROR "Hosted manifest is missing required metadata: ${_manifest}")
    endif()
    if(NOT _release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION)
        message(FATAL_ERROR
            "Hosted bundle release version mismatch: ${_manifest} (${_release_version})")
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_HOSTED_ARTIFACT_ROOT=${_bundle_root}
            -DANO_CRYPTO_EXPECTED_HOSTED_LANE=${_lane}
            -DANO_CRYPTO_EXPECTED_HOSTED_PLATFORM=${_platform}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_hosted_artifact_manifest.cmake"
        RESULT_VARIABLE _verify_result
        OUTPUT_VARIABLE _verify_stdout
        ERROR_VARIABLE _verify_stderr)
    if(NOT _verify_result EQUAL 0)
        message(FATAL_ERROR
            "Hosted bundle verification failed for ${_manifest}:\n${_verify_stdout}${_verify_stderr}")
    endif()

    set(_key "${_lane}|${_platform}")
    if(_key IN_LIST _bundle_keys)
        message(FATAL_ERROR "Duplicate hosted lane/platform bundle: ${_key}")
    endif()
    list(APPEND _bundle_keys "${_key}")
    file(RELATIVE_PATH _bundle_relative
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_bundle_root}")
    string(REPLACE "|" "<PIPE>" _record
        "${_lane}|${_platform}|${_commit}|${_bundle_relative}")
    list(APPEND _bundle_records "${_record}")
endforeach()

set(_required_keys
    "platform-thread-safety-ON|Linux"
    "platform-thread-safety-ON|macOS"
    "platform-thread-safety-ON|Windows"
    "clean-release-symbol-compatibility|Linux"
    "clean-release-symbol-compatibility|macOS"
    "clean-release-symbol-compatibility|Windows"
    "tagged-release-compatibility|Linux"
    "tagged-release-compatibility|macOS"
    "tagged-release-compatibility|Windows"
    "linux-32bit-portability|Linux"
    "linux-s390x-big-endian|Linux"
    "aes-cmac-compiler-output-gcc|Linux"
    "aes-cmac-compiler-output-clang|Linux"
    "openssl-differential|Linux")
set(_missing_keys "")
foreach(_required_key IN LISTS _required_keys)
    if(NOT _required_key IN_LIST _bundle_keys)
        list(APPEND _missing_keys "${_required_key}")
    endif()
endforeach()

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
    set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
    if(IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Hosted artifact index rejects symlinks: ${_relative}")
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
if(DEFINED ANO_CRYPTO_WORKFLOW_COMMIT AND
   NOT ANO_CRYPTO_WORKFLOW_COMMIT STREQUAL "")
    set(_workflow_commit "${ANO_CRYPTO_WORKFLOW_COMMIT}")
else()
    set(_workflow_commit "unknown")
endif()
if(DEFINED ANO_CRYPTO_WORKFLOW_REF AND NOT ANO_CRYPTO_WORKFLOW_REF STREQUAL "")
    set(_workflow_ref "${ANO_CRYPTO_WORKFLOW_REF}")
else()
    set(_workflow_ref "unknown")
endif()
list(LENGTH _manifests _bundle_count)
list(LENGTH _artifact_files _artifact_file_count)

set(_index_file
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_INDEX.txt")
file(WRITE "${_index_file}"
    "AnoCrypto-C P1-30 hosted artifact index\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag metadata: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Workflow run id: ${_run_id}\n"
    "Workflow commit: ${_workflow_commit}\n"
    "Workflow ref: ${_workflow_ref}\n"
    "Verified bundles: ${_bundle_count}\n"
    "Indexed files: ${_artifact_file_count}\n"
    "Required bundle categories: verified below\n"
    "Human platform/content review: TODO\n"
    "Sensitive cryptographic values intentionally indexed: no\n\n"
    "Bundles:\n")
list(SORT _bundle_records)
foreach(_record IN LISTS _bundle_records)
    string(REPLACE "<PIPE>" "|" _decoded "${_record}")
    file(APPEND "${_index_file}" "- ${_decoded}\n")
endforeach()

if(_missing_keys)
    file(APPEND "${_index_file}" "\nMissing required lane/platform categories:\n")
    foreach(_missing IN LISTS _missing_keys)
        file(APPEND "${_index_file}" "- ${_missing}\n")
    endforeach()
    file(APPEND "${_index_file}" "Result: FAIL\n")
    message(FATAL_ERROR "Hosted artifact set is incomplete; see ${_index_file}")
endif()
file(APPEND "${_index_file}"
    "\nManifest re-verification: PASS\n"
    "Required lane/platform presence: PASS\n"
    "Result: PASS\n"
    "Limit: hashes and presence do not constitute human platform, ABI, or side-channel review.\n")

file(WRITE
    "${ANO_CRYPTO_HOSTED_ARTIFACT_OUTPUT_DIR}/HOSTED_ARTIFACT_REVIEW.md"
    "# Hosted Artifact Review Record\n\n"
    "Release: `${ANO_CRYPTO_RELEASE_VERSION}`  \n"
    "Expected tag metadata: `${ANO_CRYPTO_RELEASE_TAG}`  \n"
    "Workflow run: `${_run_id}`  \n"
    "Commit: `${_workflow_commit}`  \n"
    "Ref: `${_workflow_ref}`\n\n"
    "All required bundle manifests were re-verified and indexed. A reviewer must complete the table before support, compatibility, or side-channel wording changes.\n\n"
    "| Area | Reviewer/date | Result | Limitations |\n"
    "|---|---|---|---|\n"
    "| Linux 64-bit | TODO | TODO | TODO |\n"
    "| Linux 32-bit | TODO | TODO | TODO |\n"
    "| Linux s390x big-endian | TODO | TODO | TODO |\n"
    "| macOS | TODO | TODO | TODO |\n"
    "| Windows | TODO | TODO | TODO |\n"
    "| GCC/Clang compiler output | TODO | TODO | No automated timing/cache conclusion |\n"
    "| Differential validation | TODO | TODO | Test-only provider |\n"
    "| P1-29 headers/P1-30 library | TODO | TODO | Toolchain-specific |\n"
    "| Tag/commit/release provenance | TODO | TODO | Expected metadata is not a signature |\n\n"
    "Do not attach keys, entropy, seed material, DRBG state, generated random bytes, AES values, CMAC tags, private keys, signing credentials, tokens, customer data, or internal deployment details.\n")

message(STATUS
    "AnoCrypto-C hosted artifact index created (${_bundle_count} verified bundles)")
