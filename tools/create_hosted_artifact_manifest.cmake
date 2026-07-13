# Create a deterministic, status-only hosted CI artifact bundle and SHA-256 manifest.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_HOSTED_ARTIFACT_ROOT
        ANO_CRYPTO_HOSTED_LANE
        ANO_CRYPTO_HOSTED_PLATFORM
        ANO_CRYPTO_HOSTED_ARCH
        ANO_CRYPTO_HOSTED_COMPILER)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()
if(NOT DEFINED ANO_CRYPTO_HOSTED_COMMIT OR ANO_CRYPTO_HOSTED_COMMIT STREQUAL "")
    set(ANO_CRYPTO_HOSTED_COMMIT "unknown")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN OR
   ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN STREQUAL "")
    set(ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN "SYNTHETIC_LOCAL")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_EXECUTION_STATUS OR
   ANO_CRYPTO_HOSTED_EXECUTION_STATUS STREQUAL "")
    set(ANO_CRYPTO_HOSTED_EXECUTION_STATUS "NOT_RUN")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT OR
   ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT STREQUAL "")
    set(ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT "${ANO_CRYPTO_HOSTED_COMMIT}")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_WORKFLOW_REF OR
   ANO_CRYPTO_HOSTED_WORKFLOW_REF STREQUAL "")
    set(ANO_CRYPTO_HOSTED_WORKFLOW_REF "unknown")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_REVIEW_STATUS OR
   ANO_CRYPTO_HOSTED_REVIEW_STATUS STREQUAL "")
    set(ANO_CRYPTO_HOSTED_REVIEW_STATUS "UNREVIEWED")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_REVIEWER OR
   ANO_CRYPTO_HOSTED_REVIEWER STREQUAL "")
    set(ANO_CRYPTO_HOSTED_REVIEWER "UNPROVISIONED")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_REVIEW_DATE OR
   ANO_CRYPTO_HOSTED_REVIEW_DATE STREQUAL "")
    set(ANO_CRYPTO_HOSTED_REVIEW_DATE "UNREVIEWED")
endif()
if(NOT DEFINED ANO_CRYPTO_HOSTED_DISPOSITION OR
   ANO_CRYPTO_HOSTED_DISPOSITION STREQUAL "")
    set(ANO_CRYPTO_HOSTED_DISPOSITION "UNREVIEWED")
endif()
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

file(RELATIVE_PATH _manifest_relative
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}")
if(IS_ABSOLUTE "${_manifest_relative}")
    message(FATAL_ERROR "Hosted artifact manifest must remain under the bundle root")
endif()
_ano_require_safe_relative_path("${_manifest_relative}" "hosted artifact manifest path")

foreach(_metadata IN ITEMS
        "${ANO_CRYPTO_HOSTED_LANE}"
        "${ANO_CRYPTO_HOSTED_PLATFORM}"
        "${ANO_CRYPTO_HOSTED_ARCH}"
        "${ANO_CRYPTO_HOSTED_COMPILER}"
        "${ANO_CRYPTO_HOSTED_COMMIT}"
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN}"
        "${ANO_CRYPTO_HOSTED_EXECUTION_STATUS}"
        "${ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT}"
        "${ANO_CRYPTO_HOSTED_WORKFLOW_REF}"
        "${ANO_CRYPTO_HOSTED_REVIEW_STATUS}"
        "${ANO_CRYPTO_HOSTED_REVIEWER}"
        "${ANO_CRYPTO_HOSTED_REVIEW_DATE}"
        "${ANO_CRYPTO_HOSTED_DISPOSITION}")
    if(_metadata MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Hosted artifact metadata contains a forbidden separator")
    endif()
endforeach()
if(NOT ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN MATCHES
       "^(HOSTED_RUNNER|SYNTHETIC_LOCAL)$")
    message(FATAL_ERROR "Hosted artifact origin is invalid")
endif()
if(NOT ANO_CRYPTO_HOSTED_EXECUTION_STATUS MATCHES "^(COMPLETED|NOT_RUN)$")
    message(FATAL_ERROR "Hosted execution status is invalid")
endif()
if((ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN STREQUAL "HOSTED_RUNNER" AND
    NOT ANO_CRYPTO_HOSTED_EXECUTION_STATUS STREQUAL "COMPLETED") OR
   (ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN STREQUAL "SYNTHETIC_LOCAL" AND
    NOT ANO_CRYPTO_HOSTED_EXECUTION_STATUS STREQUAL "NOT_RUN"))
    message(FATAL_ERROR "Hosted artifact origin and execution status disagree")
endif()
if(NOT ANO_CRYPTO_HOSTED_COMMIT STREQUAL ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT)
    message(FATAL_ERROR "Hosted artifact commit and workflow commit disagree")
endif()
if(ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN STREQUAL "HOSTED_RUNNER" AND
   (ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT STREQUAL "unknown" OR
    ANO_CRYPTO_HOSTED_WORKFLOW_REF STREQUAL "unknown"))
    message(FATAL_ERROR "Hosted runner artifacts require workflow commit and ref")
endif()
string(TOUPPER "${ANO_CRYPTO_HOSTED_DISPOSITION}" _disposition_upper)
if(_disposition_upper MATCHES "^(PASS|APPROVED)$")
    message(FATAL_ERROR "Hosted review disposition may not claim PASS or APPROVED")
endif()
if(ANO_CRYPTO_HOSTED_REVIEW_STATUS STREQUAL "UNREVIEWED")
    if(NOT ANO_CRYPTO_HOSTED_REVIEWER STREQUAL "UNPROVISIONED" OR
       NOT ANO_CRYPTO_HOSTED_REVIEW_DATE STREQUAL "UNREVIEWED" OR
       NOT ANO_CRYPTO_HOSTED_DISPOSITION STREQUAL "UNREVIEWED")
        message(FATAL_ERROR "Unreviewed hosted artifacts must retain safe review placeholders")
    endif()
elseif(ANO_CRYPTO_HOSTED_REVIEW_STATUS STREQUAL "REVIEWED")
    string(TOLOWER "${ANO_CRYPTO_HOSTED_REVIEWER}" _reviewer_lower)
    if(NOT ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN STREQUAL "HOSTED_RUNNER" OR
       NOT ANO_CRYPTO_HOSTED_EXECUTION_STATUS STREQUAL "COMPLETED" OR
       NOT ANO_CRYPTO_HOSTED_REVIEWER MATCHES "^[A-Za-z0-9_.@:+-]+$" OR
       _reviewer_lower MATCHES "^(todo|unknown|unreviewed|unprovisioned|none)$" OR
       NOT ANO_CRYPTO_HOSTED_REVIEW_DATE MATCHES
           "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$" OR
       NOT ANO_CRYPTO_HOSTED_DISPOSITION MATCHES
           "^(ACCEPTED_WITH_LIMITATIONS|REJECTED)$")
        message(FATAL_ERROR "Completed hosted review metadata is incomplete or invalid")
    endif()
else()
    message(FATAL_ERROR "Hosted review status is invalid")
endif()

if(DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES AND
   NOT ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES STREQUAL "")
    if(EXISTS "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" OR
       IS_SYMLINK "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
        ano_crypto_reject_hosted_root_alias(
            "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "Hosted artifact output root")
    else()
        get_filename_component(_output_parent
            "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" DIRECTORY)
        if(NOT IS_DIRECTORY "${_output_parent}")
            message(FATAL_ERROR
                "Hosted artifact output parent is unavailable: ${_output_parent}")
        endif()
    endif()
    file(REMOVE_RECURSE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    file(MAKE_DIRECTORY "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    ano_crypto_reject_hosted_root_alias(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "Hosted artifact output root")
    set(_copied_names "")
    foreach(_source IN LISTS ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES)
        get_filename_component(_source_abs "${_source}" ABSOLUTE)
        if(NOT EXISTS "${_source_abs}" OR IS_DIRECTORY "${_source_abs}" OR
           IS_SYMLINK "${_source_abs}")
            message(FATAL_ERROR "Hosted artifact source file is missing or unsafe: ${_source}")
        endif()
        get_filename_component(_name "${_source_abs}" NAME)
        get_filename_component(_source_parent "${_source_abs}" DIRECTORY)
        ano_crypto_require_regular_unaliased_hosted_child(
            "${_source_parent}" "${_name}" "Hosted artifact source file")
        if(_name STREQUAL "HOSTED_ARTIFACT_MANIFEST.txt")
            message(FATAL_ERROR "A source file may not use the manifest filename")
        endif()
        if(_name IN_LIST _copied_names)
            message(FATAL_ERROR "Duplicate hosted artifact basename: ${_name}")
        endif()
        list(APPEND _copied_names "${_name}")
        configure_file("${_source_abs}"
            "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_name}" COPYONLY)
    endforeach()
elseif(NOT IS_DIRECTORY "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    message(FATAL_ERROR "Hosted artifact root does not exist and no source files were supplied")
else()
    ano_crypto_reject_hosted_root_alias(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "Hosted artifact root")
endif()

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

file(GLOB_RECURSE _files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}"
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/*")
list(SORT _files)
set(_reviewed_files "")
foreach(_relative IN LISTS _files)
    set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
    get_filename_component(_full_abs "${_full}" ABSOLUTE)
    if(_full_abs STREQUAL ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST)
        continue()
    endif()
    _ano_require_safe_relative_path("${_relative}" "hosted artifact path")
    ano_crypto_require_regular_unaliased_hosted_child(
        "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_relative}"
        "Hosted artifact file")
    if(NOT _relative MATCHES "\\.(txt|md|json|sha256|asm|disasm)$")
        message(FATAL_ERROR "Hosted artifact file type is not approved: ${_relative}")
    endif()
    file(SIZE "${_full}" _size)
    if(_size GREATER 10485760)
        message(FATAL_ERROR "Hosted artifact file exceeds 10 MiB: ${_relative}")
    endif()
    _ano_reject_nul("${_full}" "${_relative}")
    _ano_scan_sensitive_markers("${_full}" "${_relative}")
    list(APPEND _reviewed_files "${_relative}")
endforeach()

list(LENGTH _reviewed_files _file_count)
if(_file_count EQUAL 0)
    message(FATAL_ERROR "Hosted artifact bundle contains no review files")
endif()

file(WRITE "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}"
    "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V2\n"
    "policy_schema=${ANO_CRYPTO_RELEASE_POLICY_SCHEMA}\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "abi_line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "lane=${ANO_CRYPTO_HOSTED_LANE}\n"
    "platform=${ANO_CRYPTO_HOSTED_PLATFORM}\n"
    "architecture=${ANO_CRYPTO_HOSTED_ARCH}\n"
    "compiler=${ANO_CRYPTO_HOSTED_COMPILER}\n"
    "commit=${ANO_CRYPTO_HOSTED_COMMIT}\n"
    "artifact_origin=${ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN}\n"
    "execution_status=${ANO_CRYPTO_HOSTED_EXECUTION_STATUS}\n"
    "workflow_commit=${ANO_CRYPTO_HOSTED_WORKFLOW_COMMIT}\n"
    "workflow_ref=${ANO_CRYPTO_HOSTED_WORKFLOW_REF}\n"
    "review_status=${ANO_CRYPTO_HOSTED_REVIEW_STATUS}\n"
    "reviewer=${ANO_CRYPTO_HOSTED_REVIEWER}\n"
    "review_date=${ANO_CRYPTO_HOSTED_REVIEW_DATE}\n"
    "disposition=${ANO_CRYPTO_HOSTED_DISPOSITION}\n"
    "file_count=${_file_count}\n")
foreach(_relative IN LISTS _reviewed_files)
    set(_full "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}/${_relative}")
    file(SIZE "${_full}" _size)
    file(SHA256 "${_full}" _sha256)
    file(APPEND "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}"
        "file=${_relative}|size=${_size}|sha256=${_sha256}\n")
endforeach()
file(APPEND "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}"
    "sensitive_value_scan=PASS\n"
    "integrity_status=PASS\n")
ano_crypto_require_regular_unaliased_hosted_child(
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${_manifest_relative}"
    "Hosted artifact manifest")

message(STATUS
    "Created hosted artifact manifest for ${ANO_CRYPTO_HOSTED_LANE} (${_file_count} files)")
