# Verify a hosted CI artifact bundle against its deterministic SHA-256 manifest.
cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_HOSTED_ARTIFACT_ROOT)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
if(NOT DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST OR
   ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST STREQUAL "")
    set(ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/HOSTED_ARTIFACT_MANIFEST.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_HOSTED_ARTIFACT_ROOT
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST
    "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")
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
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "Hosted artifact root")

file(RELATIVE_PATH _manifest_relative
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}")
if(IS_ABSOLUTE "${_manifest_relative}")
    message(FATAL_ERROR "Hosted artifact manifest must remain under the bundle root")
endif()
_ano_require_safe_relative_path("${_manifest_relative}" "hosted artifact manifest path")
ano_crypto_require_regular_unaliased_hosted_child(
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_manifest_relative}"
    "Hosted artifact manifest")

function(_ano_reject_nul _path _relative)
    file(READ "${_path}" _hex HEX)
    string(LENGTH "${_hex}" _hex_length)
    math(EXPR _last "${_hex_length} - 2")
    if(_last GREATER_EQUAL 0)
        foreach(_offset RANGE 0 ${_last} 2)
            string(SUBSTRING "${_hex}" ${_offset} 2 _byte)
            if(_byte MATCHES "^(0[0-8bBcCeEfF]|1[0-9a-fA-F]|7[fF])$")
                message(FATAL_ERROR "Hosted artifact must be text-only: ${_relative}")
            endif()
        endforeach()
    endif()
endfunction()

function(_ano_scan_sensitive_markers _path _relative)
    file(READ "${_path}" _content)
    string(TOLOWER "${_content}" _content_lower)
    ano_crypto_detect_sensitive_signing_material_text(
        "${_content}" ON _sensitive_reason)
    if(_sensitive_reason)
        message(FATAL_ERROR
            "Hosted artifact contains forbidden signing material: ${_relative}")
    endif()
    foreach(_forbidden IN ITEMS
            "ano_crypto_secret_value="
            "entropy_material="
            "seed_material="
            "drbg_state="
            "random_bytes="
            "private_scalar="
            "token_value=")
        string(FIND "${_content_lower}" "${_forbidden}" _position)
        if(NOT _position EQUAL -1)
            message(FATAL_ERROR
                "Hosted artifact contains a forbidden sensitive-value marker: ${_relative}")
        endif()
    endforeach()
endfunction()

file(STRINGS "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}" _lines)
list(LENGTH _lines _line_count)
if(_line_count LESS 22)
    message(FATAL_ERROR "Hosted artifact manifest is incomplete")
endif()
list(GET _lines 0 _magic)
if(NOT _magic STREQUAL "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V2")
    message(FATAL_ERROR "Hosted artifact manifest magic is invalid")
endif()

set(_listed_files "")
set(_metadata_keys "")
foreach(_field IN ITEMS policy_schema release_version expected_tag abi_line lane platform
                        architecture compiler commit artifact_origin execution_status
                        workflow_commit workflow_ref review_status reviewer review_date
                        disposition file_count sensitive_value_scan integrity_status)
    set(_${_field} "")
endforeach()

foreach(_line IN LISTS _lines)
    if(_line STREQUAL "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V2")
        continue()
    elseif(_line MATCHES "^file=([^|]+)\\|size=([0-9]+)\\|sha256=([0-9a-fA-F]+)$")
        set(_relative "${CMAKE_MATCH_1}")
        set(_expected_size "${CMAKE_MATCH_2}")
        string(TOLOWER "${CMAKE_MATCH_3}" _expected_hash)
        string(LENGTH "${_expected_hash}" _hash_length)
        if(NOT _hash_length EQUAL 64)
            message(FATAL_ERROR "Hosted artifact hash has an invalid length: ${_relative}")
        endif()
        _ano_require_safe_relative_path(
            "${_relative}" "hosted artifact manifest entry")
        if(_relative STREQUAL "HOSTED_ARTIFACT_MANIFEST.txt")
            message(FATAL_ERROR "Unsafe hosted artifact manifest path: ${_relative}")
        endif()
        if(_relative IN_LIST _listed_files)
            message(FATAL_ERROR "Duplicate hosted artifact manifest path: ${_relative}")
        endif()
        set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
        ano_crypto_require_regular_unaliased_hosted_child(
            "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_relative}"
            "Hosted artifact file")
        if(NOT _relative MATCHES "\\.(txt|md|json|sha256|asm|disasm)$")
            message(FATAL_ERROR "Hosted artifact file type is not approved: ${_relative}")
        endif()
        file(SIZE "${_full}" _actual_size)
        if(_actual_size GREATER 10485760)
            message(FATAL_ERROR "Hosted artifact file exceeds 10 MiB: ${_relative}")
        endif()
        file(SHA256 "${_full}" _actual_hash)
        if(NOT "${_actual_size}" STREQUAL "${_expected_size}")
            message(FATAL_ERROR "Hosted artifact size mismatch: ${_relative}")
        endif()
        if(NOT _actual_hash STREQUAL _expected_hash)
            message(FATAL_ERROR "Hosted artifact hash mismatch: ${_relative}")
        endif()
        _ano_reject_nul("${_full}" "${_relative}")
        _ano_scan_sensitive_markers("${_full}" "${_relative}")
        list(APPEND _listed_files "${_relative}")
    elseif(_line MATCHES "^([a-z_]+)=(.*)$")
        set(_key "${CMAKE_MATCH_1}")
        set(_value "${CMAKE_MATCH_2}")
        if(NOT _key IN_LIST _metadata_keys)
            if(NOT _key MATCHES "^(policy_schema|release_version|expected_tag|abi_line|lane|platform|architecture|compiler|commit|artifact_origin|execution_status|workflow_commit|workflow_ref|review_status|reviewer|review_date|disposition|file_count|sensitive_value_scan|integrity_status)$")
                message(FATAL_ERROR "Unknown hosted artifact metadata field: ${_key}")
            endif()
            list(APPEND _metadata_keys "${_key}")
            set(_${_key} "${_value}")
        else()
            message(FATAL_ERROR "Duplicate hosted artifact metadata field: ${_key}")
        endif()
    else()
        message(FATAL_ERROR "Malformed hosted artifact manifest line: ${_line}")
    endif()
endforeach()

foreach(_required_key IN ITEMS policy_schema release_version expected_tag abi_line lane platform
                               architecture compiler commit artifact_origin execution_status
                               workflow_commit workflow_ref review_status reviewer review_date
                               disposition file_count sensitive_value_scan integrity_status)
    if(NOT _required_key IN_LIST _metadata_keys)
        message(FATAL_ERROR "Hosted artifact metadata is missing: ${_required_key}")
    endif()
endforeach()
if(NOT _policy_schema STREQUAL ANO_CRYPTO_RELEASE_POLICY_SCHEMA OR
   NOT _release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _abi_line STREQUAL ANO_CRYPTO_RELEASE_ABI_LINE)
    message(FATAL_ERROR "Hosted artifact release identity does not match the source tree")
endif()
if(_lane STREQUAL "" OR _platform STREQUAL "" OR _architecture STREQUAL "" OR
   _compiler STREQUAL "" OR _commit STREQUAL "")
    message(FATAL_ERROR "Hosted artifact lane metadata is incomplete")
endif()
foreach(_metadata IN ITEMS
        "${_lane}" "${_platform}" "${_architecture}" "${_compiler}"
        "${_commit}" "${_artifact_origin}" "${_execution_status}"
        "${_workflow_commit}" "${_workflow_ref}" "${_review_status}"
        "${_reviewer}" "${_review_date}" "${_disposition}")
    if(_metadata MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Hosted artifact metadata contains a forbidden separator")
    endif()
endforeach()
if(NOT _artifact_origin MATCHES "^(HOSTED_RUNNER|SYNTHETIC_LOCAL)$" OR
   NOT _execution_status MATCHES "^(COMPLETED|NOT_RUN)$")
    message(FATAL_ERROR "Hosted artifact origin or execution status is invalid")
endif()
if((_artifact_origin STREQUAL "HOSTED_RUNNER" AND
    NOT _execution_status STREQUAL "COMPLETED") OR
   (_artifact_origin STREQUAL "SYNTHETIC_LOCAL" AND
    NOT _execution_status STREQUAL "NOT_RUN"))
    message(FATAL_ERROR "Hosted artifact origin and execution status disagree")
endif()
if(NOT _commit STREQUAL _workflow_commit)
    message(FATAL_ERROR "Hosted artifact commit and workflow commit disagree")
endif()
if(_artifact_origin STREQUAL "HOSTED_RUNNER" AND
   (_workflow_commit STREQUAL "unknown" OR _workflow_ref STREQUAL "unknown"))
    message(FATAL_ERROR "Hosted runner artifacts require workflow commit and ref")
endif()
string(TOUPPER "${_disposition}" _disposition_upper)
if(_disposition_upper MATCHES "^(PASS|APPROVED)$")
    message(FATAL_ERROR "Hosted review disposition may not claim PASS or APPROVED")
endif()
if(_review_status STREQUAL "UNREVIEWED")
    if(NOT _reviewer STREQUAL "UNPROVISIONED" OR
       NOT _review_date STREQUAL "UNREVIEWED" OR
       NOT _disposition STREQUAL "UNREVIEWED")
        message(FATAL_ERROR "Unreviewed hosted artifacts must retain safe review placeholders")
    endif()
elseif(_review_status STREQUAL "REVIEWED")
    string(TOLOWER "${_reviewer}" _reviewer_lower)
    if(NOT _artifact_origin STREQUAL "HOSTED_RUNNER" OR
       NOT _execution_status STREQUAL "COMPLETED" OR
       NOT _reviewer MATCHES "^[A-Za-z0-9_.@:+-]+$" OR
       _reviewer_lower MATCHES "^(todo|unknown|unreviewed|unprovisioned|none)$" OR
       NOT _review_date MATCHES
           "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$" OR
       NOT _disposition MATCHES "^(ACCEPTED_WITH_LIMITATIONS|REJECTED)$")
        message(FATAL_ERROR "Completed hosted review metadata is incomplete or invalid")
    endif()
else()
    message(FATAL_ERROR "Hosted review status is invalid")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_LANE AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_LANE STREQUAL "" AND
   NOT _lane STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_LANE)
    message(FATAL_ERROR "Hosted artifact lane does not match expected input")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_PLATFORM AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_PLATFORM STREQUAL "" AND
   NOT _platform STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_PLATFORM)
    message(FATAL_ERROR "Hosted artifact platform does not match expected input")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_ARCH AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_ARCH STREQUAL "" AND
   NOT _architecture STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_ARCH)
    message(FATAL_ERROR "Hosted artifact architecture does not match expected input")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_COMPILER AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_COMPILER STREQUAL "" AND
   NOT _compiler STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_COMPILER)
    message(FATAL_ERROR "Hosted artifact compiler does not match expected input")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_COMMIT AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_COMMIT STREQUAL "" AND
   (NOT _commit STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_COMMIT OR
    NOT _workflow_commit STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_COMMIT))
    message(FATAL_ERROR "Hosted artifact commit does not match expected input")
endif()
if(DEFINED ANO_CRYPTO_EXPECTED_HOSTED_WORKFLOW_REF AND
   NOT ANO_CRYPTO_EXPECTED_HOSTED_WORKFLOW_REF STREQUAL "" AND
   NOT _workflow_ref STREQUAL ANO_CRYPTO_EXPECTED_HOSTED_WORKFLOW_REF)
    message(FATAL_ERROR "Hosted artifact workflow ref does not match expected input")
endif()
if(NOT _sensitive_value_scan STREQUAL "PASS" OR
   NOT _integrity_status STREQUAL "PASS")
    message(FATAL_ERROR "Hosted artifact manifest integrity did not pass")
endif()
list(LENGTH _listed_files _actual_count)
if(NOT "${_actual_count}" STREQUAL "${_file_count}")
    message(FATAL_ERROR "Hosted artifact manifest file count mismatch")
endif()

file(GLOB_RECURSE _bundle_files
    LIST_DIRECTORIES false RELATIVE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}"
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/*")
foreach(_relative IN LISTS _bundle_files)
    set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
    _ano_require_safe_relative_path("${_relative}" "hosted artifact bundle path")
    get_filename_component(_full_abs "${_full}" ABSOLUTE)
    if(_full_abs STREQUAL ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST)
        continue()
    endif()
    ano_crypto_require_regular_unaliased_hosted_child(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_relative}"
        "Hosted artifact bundle file")
    if(NOT _relative IN_LIST _listed_files)
        message(FATAL_ERROR "Unlisted hosted artifact file: ${_relative}")
    endif()
endforeach()

message(STATUS
    "Hosted artifact manifest verified for ${_lane} on ${_platform} (${_actual_count} files)")
