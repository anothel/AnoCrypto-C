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

file(RELATIVE_PATH _manifest_relative
    "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}" "${ANO_CRYPTO_HOSTED_ARTIFACT_MANIFEST}")
if(_manifest_relative MATCHES "(^|/)\\.\\.(/|$)" OR IS_ABSOLUTE "${_manifest_relative}")
    message(FATAL_ERROR "Hosted artifact manifest must remain under the bundle root")
endif()

foreach(_metadata IN ITEMS
        "${ANO_CRYPTO_HOSTED_LANE}"
        "${ANO_CRYPTO_HOSTED_PLATFORM}"
        "${ANO_CRYPTO_HOSTED_ARCH}"
        "${ANO_CRYPTO_HOSTED_COMPILER}"
        "${ANO_CRYPTO_HOSTED_COMMIT}")
    if(_metadata MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Hosted artifact metadata contains a forbidden separator")
    endif()
endforeach()

if(DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES AND
   NOT ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES STREQUAL "")
    file(REMOVE_RECURSE "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    file(MAKE_DIRECTORY "${ANO_CRYPTO_HOSTED_ARTIFACT_ROOT}")
    set(_copied_names "")
    foreach(_source IN LISTS ANO_CRYPTO_HOSTED_ARTIFACT_SOURCE_FILES)
        get_filename_component(_source_abs "${_source}" ABSOLUTE)
        if(NOT EXISTS "${_source_abs}" OR IS_DIRECTORY "${_source_abs}" OR
           IS_SYMLINK "${_source_abs}")
            message(FATAL_ERROR "Hosted artifact source file is missing or unsafe: ${_source}")
        endif()
        get_filename_component(_name "${_source_abs}" NAME)
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
endif()

function(_ano_reject_nul _path _relative)
    file(READ "${_path}" _hex HEX)
    string(LENGTH "${_hex}" _hex_length)
    math(EXPR _last "${_hex_length} - 2")
    if(_last GREATER_EQUAL 0)
        foreach(_offset RANGE 0 ${_last} 2)
            string(SUBSTRING "${_hex}" ${_offset} 2 _byte)
            if(_byte STREQUAL "00")
                message(FATAL_ERROR "Hosted artifact must be text-only: ${_relative}")
            endif()
        endforeach()
    endif()
endfunction()

function(_ano_scan_sensitive_markers _path _relative)
    file(READ "${_path}" _content)
    string(TOLOWER "${_content}" _content_lower)
    foreach(_forbidden IN ITEMS
            "-----begin private key-----"
            "-----begin rsa private key-----"
            "-----begin ec private key-----"
            "-----begin openssh private key-----"
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
    if(_relative MATCHES "(^/|(^|/)\\.\\.(/|$))" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Unsafe hosted artifact path: ${_relative}")
    endif()
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
    "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V1\n"
    "policy_schema=${ANO_CRYPTO_RELEASE_POLICY_SCHEMA}\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "abi_line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "lane=${ANO_CRYPTO_HOSTED_LANE}\n"
    "platform=${ANO_CRYPTO_HOSTED_PLATFORM}\n"
    "architecture=${ANO_CRYPTO_HOSTED_ARCH}\n"
    "compiler=${ANO_CRYPTO_HOSTED_COMPILER}\n"
    "commit=${ANO_CRYPTO_HOSTED_COMMIT}\n"
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
    "result=PASS\n")

message(STATUS
    "Created hosted artifact manifest for ${ANO_CRYPTO_HOSTED_LANE} (${_file_count} files)")
