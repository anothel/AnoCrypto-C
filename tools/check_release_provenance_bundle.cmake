# Verify an unsigned AnoCrypto-C P1-32 engineering provenance bundle.
# This checker validates path safety, complete hash coverage, release identity,
# source-file hashes, source SBOM bindings, ABI/public-header inputs, and the
# explicit unsigned/BLOCKED release boundary. It does not verify a signature or
# authorize external release.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_PROVENANCE_BUNDLE_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_BUNDLE_DIR
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")

if(NOT IS_DIRECTORY "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}" OR
   IS_SYMLINK "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}")
    message(FATAL_ERROR
        "Provenance bundle directory is missing or unsafe: ${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}")
endif()
if(NOT DEFINED ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT OR
   ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT STREQUAL "")
    set(ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/../RELEASE_PROVENANCE_CHECK.txt")
endif()
get_filename_component(_check_output_dir
    "${ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_check_output_dir}")

set(_required_files
    EXPORTED_SYMBOLS.txt
    HOSTED_ARTIFACT_DISPOSITION.txt
    PUBLIC_HEADERS.sha256
    RELEASE_ARTIFACTS.sha256
    RELEASE_AUTHORIZATION.md
    RELEASE_PROVENANCE.txt
    SBOM_STATUS.txt
    SIGNING_REQUEST.txt
    SIGNING_STATUS.txt
    SOURCE_FILES.sha256
    SOURCE_SBOM.spdx.json
    SOURCE_SBOM_SUMMARY.txt
    anocrypto-c-source.tar.gz)
foreach(_relative IN LISTS _required_files)
    set(_full "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Required provenance file is missing or unsafe: ${_relative}")
    endif()
endforeach()

file(GLOB _bundle_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}"
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/*")
list(SORT _bundle_files)
foreach(_relative IN LISTS _bundle_files)
    if(_relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "[/\\\\]")
        message(FATAL_ERROR "Unsafe provenance bundle path: ${_relative}")
    endif()
    if(IS_SYMLINK "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_relative}")
        message(FATAL_ERROR "Provenance bundle rejects symlinks: ${_relative}")
    endif()
endforeach()

set(_manifest "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_ARTIFACTS.sha256")
file(STRINGS "${_manifest}" _manifest_lines)
set(_manifest_paths "")
foreach(_line IN LISTS _manifest_lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
        continue()
    endif()
    if(NOT _line MATCHES "^([0-9a-fA-F][0-9a-fA-F]*)[ ][ ]([^ ].*)$")
        message(FATAL_ERROR "Malformed provenance hash line: ${_line}")
    endif()
    string(TOLOWER "${CMAKE_MATCH_1}" _expected_hash)
    set(_relative "${CMAKE_MATCH_2}")
    string(LENGTH "${_expected_hash}" _hash_length)
    if(NOT _hash_length EQUAL 64 OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "[/\\\\]" OR
       _relative STREQUAL "RELEASE_ARTIFACTS.sha256")
        message(FATAL_ERROR "Unsafe provenance manifest entry: ${_line}")
    endif()
    if(_relative IN_LIST _manifest_paths)
        message(FATAL_ERROR "Duplicate provenance manifest entry: ${_relative}")
    endif()
    set(_full "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Manifest file is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_full}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Provenance hash mismatch: ${_relative}")
    endif()
    list(APPEND _manifest_paths "${_relative}")
endforeach()

set(_expected_manifest_paths "${_bundle_files}")
list(REMOVE_ITEM _expected_manifest_paths "RELEASE_ARTIFACTS.sha256")
list(SORT _expected_manifest_paths)
list(SORT _manifest_paths)
if(NOT _manifest_paths STREQUAL _expected_manifest_paths)
    message(FATAL_ERROR
        "Provenance manifest does not cover exactly the files in the bundle")
endif()

function(_ano_read_key _path _key _output)
    file(STRINGS "${_path}" _matches REGEX "^${_key}=")
    list(LENGTH _matches _count)
    if(NOT _count EQUAL 1)
        message(FATAL_ERROR "Expected one ${_key}= entry in ${_path}")
    endif()
    list(GET _matches 0 _line)
    string(REGEX REPLACE "^[^=]*=" "" _value "${_line}")
    set(${_output} "${_value}" PARENT_SCOPE)
endfunction()

set(_provenance "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_PROVENANCE.txt")
foreach(_key IN ITEMS
        policy_schema release_version package_version expected_tag actual_tag
        tag_status source_commit worktree_state commit_status abi_line public_header_count
        public_symbol_count source_file_count source_archive
        source_archive_sha256 source_hash_manifest source_hash_manifest_sha256
        source_sbom source_sbom_format source_sbom_sha256
        public_header_manifest public_header_manifest_sha256
        exported_symbol_manifest exported_symbol_manifest_sha256
        hosted_artifact_disposition hosted_artifact_disposition_sha256
        signing_status release_authorization external_release_authorized
        generated_utc result)
    _ano_read_key("${_provenance}" "${_key}" "_prov_${_key}")
endforeach()

if(NOT _prov_policy_schema STREQUAL ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA OR
   NOT _prov_release_version STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _prov_package_version STREQUAL ANO_CRYPTO_RELEASE_PACKAGE_VERSION OR
   NOT _prov_expected_tag STREQUAL ANO_CRYPTO_RELEASE_TAG OR
   NOT _prov_abi_line STREQUAL ANO_CRYPTO_RELEASE_ABI_LINE OR
   NOT _prov_public_header_count STREQUAL ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT OR
   NOT _prov_public_symbol_count STREQUAL ANO_CRYPTO_RELEASE_SYMBOL_COUNT)
    message(FATAL_ERROR "Release identity does not match the provenance record")
endif()
if(NOT _prov_source_archive STREQUAL "anocrypto-c-source.tar.gz" OR
   NOT _prov_source_hash_manifest STREQUAL "SOURCE_FILES.sha256" OR
   NOT _prov_source_sbom STREQUAL "SOURCE_SBOM.spdx.json" OR
   NOT _prov_source_sbom_format STREQUAL "SPDX-2.3-JSON" OR
   NOT _prov_public_header_manifest STREQUAL "PUBLIC_HEADERS.sha256" OR
   NOT _prov_exported_symbol_manifest STREQUAL "EXPORTED_SYMBOLS.txt" OR
   NOT _prov_hosted_artifact_disposition STREQUAL "HOSTED_ARTIFACT_DISPOSITION.txt")
    message(FATAL_ERROR "Provenance record references an unexpected artifact name")
endif()
if(NOT _prov_signing_status STREQUAL "UNSIGNED" OR
   NOT _prov_release_authorization STREQUAL "TODO" OR
   NOT _prov_external_release_authorized STREQUAL "NO" OR
   NOT _prov_result STREQUAL "PASS")
    message(FATAL_ERROR
        "P1-32 provenance must remain unsigned and not externally authorized")
endif()
if(NOT _prov_generated_utc MATCHES
   "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$")
    message(FATAL_ERROR "Provenance generated_utc is malformed")
endif()
if(NOT (_prov_actual_tag STREQUAL "not-recorded" OR
        _prov_actual_tag STREQUAL ANO_CRYPTO_RELEASE_TAG))
    message(FATAL_ERROR "Provenance actual tag is neither absent nor expected")
endif()
if(NOT (_prov_worktree_state STREQUAL "CLEAN" OR
        _prov_worktree_state STREQUAL "DIRTY" OR
        _prov_worktree_state STREQUAL "UNKNOWN"))
    message(FATAL_ERROR "Provenance worktree state is invalid")
endif()
if(_prov_source_commit STREQUAL "unknown")
    if(NOT _prov_commit_status STREQUAL "UNVERIFIED")
        message(FATAL_ERROR "Unknown source commit has an unexpected status")
    endif()
elseif(_prov_worktree_state STREQUAL "CLEAN")
    if(NOT _prov_commit_status STREQUAL "RECORDED_CLEAN_NOT_ATTESTED")
        message(FATAL_ERROR "Clean worktree commit has an unexpected status")
    endif()
elseif(_prov_worktree_state STREQUAL "DIRTY")
    if(NOT _prov_commit_status STREQUAL "RECORDED_DIRTY_NOT_ATTESTED")
        message(FATAL_ERROR "Dirty worktree commit has an unexpected status")
    endif()
elseif(NOT _prov_commit_status STREQUAL "RECORDED_WORKTREE_UNKNOWN_NOT_ATTESTED")
    message(FATAL_ERROR "Unknown worktree commit has an unexpected status")
endif()
if(_prov_actual_tag STREQUAL "not-recorded" AND
   NOT _prov_tag_status STREQUAL "UNVERIFIED")
    message(FATAL_ERROR "Absent actual tag must remain UNVERIFIED")
endif()
if(_prov_actual_tag STREQUAL ANO_CRYPTO_RELEASE_TAG AND
   (NOT _prov_tag_status STREQUAL "STRING_MATCH_NOT_ATTESTED" OR
    NOT _prov_worktree_state STREQUAL "CLEAN" OR
    _prov_source_commit STREQUAL "unknown"))
    message(FATAL_ERROR
        "Recorded tag string requires a clean, non-attested worktree record")
endif()

foreach(_binding IN ITEMS
        "anocrypto-c-source.tar.gz|${_prov_source_archive_sha256}"
        "SOURCE_FILES.sha256|${_prov_source_hash_manifest_sha256}"
        "SOURCE_SBOM.spdx.json|${_prov_source_sbom_sha256}"
        "PUBLIC_HEADERS.sha256|${_prov_public_header_manifest_sha256}"
        "EXPORTED_SYMBOLS.txt|${_prov_exported_symbol_manifest_sha256}"
        "HOSTED_ARTIFACT_DISPOSITION.txt|${_prov_hosted_artifact_disposition_sha256}")
    string(REPLACE "|" ";" _parts "${_binding}")
    list(GET _parts 0 _relative)
    list(GET _parts 1 _expected)
    string(LENGTH "${_expected}" _expected_length)
    if(NOT _expected_length EQUAL 64)
        message(FATAL_ERROR "Malformed provenance hash binding: ${_relative}")
    endif()
    file(SHA256 "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_relative}" _actual)
    if(NOT _actual STREQUAL _expected)
        message(FATAL_ERROR "Provenance record hash mismatch: ${_relative}")
    endif()
endforeach()

# The copied ABI and public-header manifests must be exactly the project-owned
# release inputs used by the current source tree.
file(SHA256
    "${ANO_CRYPTO_SOURCE_DIR}/release/PUBLIC_HEADERS.sha256" _source_public_headers_hash)
file(SHA256
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/PUBLIC_HEADERS.sha256" _bundle_public_headers_hash)
if(NOT _source_public_headers_hash STREQUAL _bundle_public_headers_hash)
    message(FATAL_ERROR "Provenance public-header manifest differs from the source tree")
endif()
file(SHA256
    "${ANO_CRYPTO_SOURCE_DIR}/abi/${ANO_CRYPTO_RELEASE_ABI_LINE}_EXPORTED_SYMBOLS.txt"
    _source_symbols_hash)
file(SHA256
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/EXPORTED_SYMBOLS.txt" _bundle_symbols_hash)
if(NOT _source_symbols_hash STREQUAL _bundle_symbols_hash)
    message(FATAL_ERROR "Provenance exported-symbol list differs from the source tree")
endif()

file(STRINGS
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/PUBLIC_HEADERS.sha256" _header_hash_lines)
set(_header_paths "")
foreach(_line IN LISTS _header_hash_lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
        continue()
    endif()
    if(NOT _line MATCHES "^([0-9a-fA-F][0-9a-fA-F]*)[ ][ ](.+)$")
        message(FATAL_ERROR "Malformed public-header hash line: ${_line}")
    endif()
    string(TOLOWER "${CMAKE_MATCH_1}" _expected_hash)
    set(_relative "${CMAKE_MATCH_2}")
    string(LENGTH "${_expected_hash}" _hash_length)
    if(NOT _hash_length EQUAL 64 OR
       NOT _relative MATCHES "^include/anocrypto/[A-Za-z0-9_.-]+[.]h$" OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "\\\\")
        message(FATAL_ERROR "Unsafe public-header hash entry: ${_line}")
    endif()
    if(_relative IN_LIST _header_paths)
        message(FATAL_ERROR "Duplicate public-header hash entry: ${_relative}")
    endif()
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Public header is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_full}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Public-header hash differs from the source tree: ${_relative}")
    endif()
    list(APPEND _header_paths "${_relative}")
endforeach()
list(LENGTH _header_paths _header_count)
if(NOT "${_header_count}" STREQUAL "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}")
    message(FATAL_ERROR "Public-header manifest count differs from release identity")
endif()
file(GLOB _current_public_headers
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
    "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto/*.h")
list(SORT _current_public_headers)
list(SORT _header_paths)
if(NOT _header_paths STREQUAL _current_public_headers)
    message(FATAL_ERROR
        "Public-header manifest does not cover exactly the installed header set")
endif()

file(STRINGS
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/EXPORTED_SYMBOLS.txt" _symbol_lines)
set(_symbol_names "")
foreach(_line IN LISTS _symbol_lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "" OR _line MATCHES "^#")
        continue()
    endif()
    if(NOT _line MATCHES "^ano_crypto_[A-Za-z0-9_]+$")
        message(FATAL_ERROR "Malformed public symbol entry: ${_line}")
    endif()
    if(_line IN_LIST _symbol_names)
        message(FATAL_ERROR "Duplicate public symbol entry: ${_line}")
    endif()
    list(APPEND _symbol_names "${_line}")
endforeach()
list(LENGTH _symbol_names _symbol_count)
if(NOT "${_symbol_count}" STREQUAL "${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}")
    message(FATAL_ERROR "Exported-symbol count differs from release identity")
endif()
set(_sorted_symbol_names "${_symbol_names}")
list(SORT _sorted_symbol_names)
if(NOT _symbol_names STREQUAL _sorted_symbol_names)
    message(FATAL_ERROR "Exported-symbol list must be sorted")
endif()

# Verify every source hash against the current reviewed source tree and bind
# the same paths/checksums into the SPDX JSON inventory.
set(_source_hash_manifest
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/SOURCE_FILES.sha256")
file(STRINGS "${_source_hash_manifest}" _source_hash_lines)
set(_source_paths "")
set(_source_sha1_values "")
set(_source_record_inputs "")
file(READ "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/SOURCE_SBOM.spdx.json" _sbom_text)
foreach(_line IN LISTS _source_hash_lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
        continue()
    endif()
    if(NOT _line MATCHES "^([0-9a-fA-F][0-9a-fA-F]*)[ ][ ](.+)$")
        message(FATAL_ERROR "Malformed source hash line: ${_line}")
    endif()
    string(TOLOWER "${CMAKE_MATCH_1}" _expected_hash)
    set(_relative "${CMAKE_MATCH_2}")
    string(LENGTH "${_expected_hash}" _hash_length)
    if(NOT _hash_length EQUAL 64 OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "\\\\")
        message(FATAL_ERROR "Unsafe source hash entry: ${_line}")
    endif()
    if(_relative IN_LIST _source_paths)
        message(FATAL_ERROR "Duplicate source hash entry: ${_relative}")
    endif()
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Source hash entry is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_full}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "Source hash differs from current source tree: ${_relative}")
    endif()
    file(SHA1 "${_full}" _sha1)
    list(APPEND _source_sha1_values "${_sha1}")
    list(APPEND _source_record_inputs "${_relative}|${_expected_hash}|${_sha1}")
    list(APPEND _source_paths "${_relative}")
endforeach()
list(LENGTH _source_paths _source_file_count)
if(NOT "${_source_file_count}" STREQUAL "${_prov_source_file_count}")
    message(FATAL_ERROR "Source file count differs between manifest and provenance")
endif()

# Bind each SPDX file record to one exact path, SPDX identifier, SHA-256,
# SHA-1, and CONTAINS relationship. Separate path/hash substring checks would
# not detect checksums moved between otherwise valid file records.
list(SORT _source_record_inputs)
set(_spdx_record_index 0)
foreach(_record IN LISTS _source_record_inputs)
    math(EXPR _spdx_record_index "${_spdx_record_index} + 1")
    string(REPLACE "|" ";" _parts "${_record}")
    list(GET _parts 0 _relative)
    list(GET _parts 1 _sha256)
    list(GET _parts 2 _sha1)
    string(SUBSTRING "${_sha256}" 0 12 _short_hash)
    set(_spdx_id "SPDXRef-File-${_spdx_record_index}-${_short_hash}")
    string(REPLACE "\\" "\\\\" _json_path "./${_relative}")
    string(REPLACE "\"" "\\\"" _json_path "${_json_path}")
    string(CONCAT _expected_file_record
        "    {\n"
        "      \"fileName\": \"${_json_path}\",\n"
        "      \"SPDXID\": \"${_spdx_id}\",\n"
        "      \"checksums\": [\n"
        "        {\n"
        "          \"algorithm\": \"SHA256\",\n"
        "          \"checksumValue\": \"${_sha256}\"\n"
        "        },\n"
        "        {\n"
        "          \"algorithm\": \"SHA1\",\n"
        "          \"checksumValue\": \"${_sha1}\"\n"
        "        }\n"
        "      ],\n"
        "      \"licenseConcluded\": \"NOASSERTION\",\n"
        "      \"licenseInfoInFiles\": [\"NOASSERTION\"],\n"
        "      \"copyrightText\": \"NOASSERTION\"\n"
        "    }")
    string(FIND "${_sbom_text}" "${_expected_file_record}" _record_position)
    if(_record_position EQUAL -1)
        message(FATAL_ERROR
            "Source SBOM file record is missing or not hash-bound: ${_relative}")
    endif()
    string(CONCAT _expected_relationship
        "    {\n"
        "      \"spdxElementId\": \"SPDXRef-Package-AnoCrypto-C\",\n"
        "      \"relationshipType\": \"CONTAINS\",\n"
        "      \"relatedSpdxElement\": \"${_spdx_id}\"\n"
        "    }")
    string(FIND "${_sbom_text}" "${_expected_relationship}" _relationship_position)
    if(_relationship_position EQUAL -1)
        message(FATAL_ERROR
            "Source SBOM package relationship is missing: ${_relative}")
    endif()
endforeach()

# Verify that the source archive contains exactly the reviewed source paths,
# only regular files/directories, and the same bytes recorded by the source
# hash manifest. Member names are validated before extraction.
set(_source_archive
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/anocrypto-c-source.tar.gz")
file(SIZE "${_source_archive}" _source_archive_size)
if(_source_archive_size GREATER 268435456)
    message(FATAL_ERROR "Source archive exceeds the 256 MiB engineering review limit")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar tf "${_source_archive}"
    RESULT_VARIABLE _archive_list_result
    OUTPUT_VARIABLE _archive_list_output
    ERROR_VARIABLE _archive_list_error
    TIMEOUT 120)
if(NOT _archive_list_result EQUAL 0)
    message(FATAL_ERROR
        "Unable to list source archive members: ${_archive_list_error}")
endif()
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar tvf "${_source_archive}"
    RESULT_VARIABLE _archive_verbose_result
    OUTPUT_VARIABLE _archive_verbose_output
    ERROR_VARIABLE _archive_verbose_error
    TIMEOUT 120)
if(NOT _archive_verbose_result EQUAL 0)
    message(FATAL_ERROR
        "Unable to inspect source archive member types: ${_archive_verbose_error}")
endif()
foreach(_output_var IN ITEMS _archive_list_output _archive_verbose_output)
    string(REPLACE "\r\n" "\n" ${_output_var} "${${_output_var}}")
    string(REPLACE "\r" "\n" ${_output_var} "${${_output_var}}")
    string(REGEX REPLACE "\n$" "" ${_output_var} "${${_output_var}}")
endforeach()
string(REPLACE "\n" ";" _archive_members "${_archive_list_output}")
string(REPLACE "\n" ";" _archive_verbose_lines "${_archive_verbose_output}")
list(LENGTH _archive_members _archive_member_count)
list(LENGTH _archive_verbose_lines _archive_verbose_count)
if(NOT _archive_member_count EQUAL _archive_verbose_count)
    message(FATAL_ERROR "Source archive member/type listing count differs")
endif()
foreach(_verbose IN LISTS _archive_verbose_lines)
    if(_verbose STREQUAL "")
        continue()
    endif()
    string(SUBSTRING "${_verbose}" 0 1 _member_type)
    if(NOT (_member_type STREQUAL "-" OR _member_type STREQUAL "d"))
        message(FATAL_ERROR
            "Source archive contains a non-regular member: ${_verbose}")
    endif()
endforeach()
set(_archive_file_paths "")
foreach(_member IN LISTS _archive_members)
    if(_member STREQUAL "")
        continue()
    endif()
    string(STRIP "${_member}" _stripped_member)
    if(NOT _stripped_member STREQUAL _member OR
       _member MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _member MATCHES "\\\\")
        message(FATAL_ERROR "Unsafe source archive member: ${_member}")
    endif()
    if(_member STREQUAL "anocrypto-c" OR _member STREQUAL "anocrypto-c/")
        continue()
    endif()
    if(NOT _member MATCHES "^anocrypto-c/(.+)$")
        message(FATAL_ERROR "Source archive member has an unexpected root: ${_member}")
    endif()
    set(_relative "${CMAKE_MATCH_1}")
    if(_relative MATCHES "/$")
        continue()
    endif()
    string(TOLOWER "${_relative}" _relative_lower)
    if(_relative_lower MATCHES "\\.(sig|asc|pem|key|p12|pfx|jks|keystore|token|tsq|tsr|ed25519)$")
        message(FATAL_ERROR
            "Credential/signature-like file is forbidden in the source archive: ${_relative}")
    endif()
    if(_relative IN_LIST _archive_file_paths)
        message(FATAL_ERROR "Duplicate source archive file member: ${_relative}")
    endif()
    list(APPEND _archive_file_paths "${_relative}")
endforeach()
list(SORT _archive_file_paths)
set(_sorted_source_paths "${_source_paths}")
list(SORT _sorted_source_paths)
if(NOT _archive_file_paths STREQUAL _sorted_source_paths)
    message(FATAL_ERROR
        "Source archive file members differ from SOURCE_FILES.sha256")
endif()
set(_archive_extract_dir
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}.archive-check")
file(REMOVE_RECURSE "${_archive_extract_dir}")
file(MAKE_DIRECTORY "${_archive_extract_dir}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar xzf "${_source_archive}"
    WORKING_DIRECTORY "${_archive_extract_dir}"
    RESULT_VARIABLE _archive_extract_result
    OUTPUT_VARIABLE _archive_extract_output
    ERROR_VARIABLE _archive_extract_error
    TIMEOUT 120)
if(NOT _archive_extract_result EQUAL 0)
    file(REMOVE_RECURSE "${_archive_extract_dir}")
    message(FATAL_ERROR
        "Unable to extract reviewed source archive: ${_archive_extract_error}")
endif()
set(_archive_root "${_archive_extract_dir}/anocrypto-c")
if(NOT IS_DIRECTORY "${_archive_root}" OR IS_SYMLINK "${_archive_root}")
    file(REMOVE_RECURSE "${_archive_extract_dir}")
    message(FATAL_ERROR "Extracted source archive root is missing or unsafe")
endif()
foreach(_record IN LISTS _source_record_inputs)
    string(REPLACE "|" ";" _parts "${_record}")
    list(GET _parts 0 _relative)
    list(GET _parts 1 _expected_hash)
    set(_extracted "${_archive_root}/${_relative}")
    if(NOT EXISTS "${_extracted}" OR IS_DIRECTORY "${_extracted}" OR
       IS_SYMLINK "${_extracted}")
        file(REMOVE_RECURSE "${_archive_extract_dir}")
        message(FATAL_ERROR
            "Extracted source archive file is missing or unsafe: ${_relative}")
    endif()
    file(SHA256 "${_extracted}" _extracted_hash)
    if(NOT _extracted_hash STREQUAL _expected_hash)
        file(REMOVE_RECURSE "${_archive_extract_dir}")
        message(FATAL_ERROR
            "Source archive content differs from SOURCE_FILES.sha256: ${_relative}")
    endif()
    ano_crypto_detect_sensitive_signing_material_file(
        "${_extracted}" ON _archive_sensitive_reason)
    if(_archive_sensitive_reason)
        file(REMOVE_RECURSE "${_archive_extract_dir}")
        message(FATAL_ERROR
            "Sensitive signing material found in source archive: ${_relative} (${_archive_sensitive_reason})")
    endif()
endforeach()
file(REMOVE_RECURSE "${_archive_extract_dir}")

list(SORT _source_sha1_values)
string(REPLACE ";" "" _verification_input "${_source_sha1_values}")
string(SHA1 _package_verification_code "${_verification_input}")
foreach(_required_sbom_text IN ITEMS
        "\"spdxVersion\": \"SPDX-2.3\""
        "\"dataLicense\": \"CC0-1.0\""
        "\"SPDXID\": \"SPDXRef-DOCUMENT\""
        "\"versionInfo\": \"${ANO_CRYPTO_RELEASE_VERSION}\""
        "\"packageVerificationCodeValue\": \"${_package_verification_code}\""
        "\"licenseConcluded\": \"NOASSERTION\""
        "\"licenseInfoInFiles\": [\"NOASSERTION\"]"
        "dependency discovery, vulnerability analysis, and license conclusions were not performed")
    string(FIND "${_sbom_text}" "${_required_sbom_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "Source SBOM is missing required text: ${_required_sbom_text}")
    endif()
endforeach()
string(FIND "${_sbom_text}" "${ANO_CRYPTO_SOURCE_DIR}" _absolute_source_position)
if(NOT _absolute_source_position EQUAL -1)
    message(FATAL_ERROR "Source SBOM leaks an absolute source path")
endif()
string(REGEX MATCHALL "\"SPDXID\": \"SPDXRef-File-[^\"]+\""
    _sbom_file_ids "${_sbom_text}")
list(LENGTH _sbom_file_ids _sbom_file_count)
if(NOT "${_sbom_file_count}" STREQUAL "${_source_file_count}")
    message(FATAL_ERROR "Source SBOM file count differs from the source manifest")
endif()

file(READ "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/SBOM_STATUS.txt" _sbom_status)
foreach(_text IN ITEMS
        "Status: GENERATED_SOURCE_INVENTORY"
        "Format: SPDX-2.3 JSON"
        "Dependency discovery: not performed"
        "Vulnerability analysis: not performed"
        "External dependency completeness: not claimed")
    string(FIND "${_sbom_status}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "SBOM status is missing boundary text: ${_text}")
    endif()
endforeach()

file(READ "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/SIGNING_STATUS.txt" _signing_status)
foreach(_text IN ITEMS
        "Status: UNSIGNED"
        "Detached signature present: no"
        "Cryptographic verification performed: no"
        "Signer identity approved: no (UNPROVISIONED)"
        "Timestamp trust source approved: no (UNPROVISIONED)"
        "Private keys or signing tokens included: no"
        "External release authorization implied: no")
    string(FIND "${_signing_status}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "Signing status is missing boundary text: ${_text}")
    endif()
endforeach()
    file(READ "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/SIGNING_REQUEST.txt" _signing_request)
    foreach(_text IN ITEMS
        "subject=${ANO_CRYPTO_RELEASE_SIGNING_SUBJECT}"
        "algorithm=${ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM}"
        "signature_format=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING}"
        "verification_tool=${ANO_CRYPTO_RELEASE_SIGNING_TOOL_FAMILY}"
        "signer_policy_id=${ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID}"
        "timestamp_policy=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT}"
        "production_signer=UNPROVISIONED"
        "timestamp_trust_source=UNPROVISIONED"
        "handoff_status=BLOCKED"
        "status=UNSIGNED")
    string(FIND "${_signing_request}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "Signing request is missing boundary text: ${_text}")
    endif()
endforeach()

file(READ "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/RELEASE_AUTHORIZATION.md" _authorization)
foreach(_text IN ITEMS
        "Status: TODO"
        "External release authorized: NO"
        "Reviewer: TODO"
        "Recorded worktree state: `${_prov_worktree_state}`"
        "actual hosted review, tag, production signer/timestamp provisioning, and signing remain TODO")
    string(FIND "${_authorization}" "${_text}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "Release authorization is missing boundary text: ${_text}")
    endif()
endforeach()

set(_disposition
    "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt")
_ano_read_key("${_disposition}" "release_version" _disposition_release)
_ano_read_key("${_disposition}" "expected_tag" _disposition_tag)
_ano_read_key("${_disposition}" "workflow_commit" _disposition_commit)
_ano_read_key("${_disposition}" "reviewer" _disposition_reviewer)
_ano_read_key("${_disposition}" "reviewed_utc" _disposition_reviewed_utc)
_ano_read_key("${_disposition}" "status" _disposition_status)
_ano_read_key("${_disposition}" "result" _disposition_result)
if(NOT _disposition_release STREQUAL ANO_CRYPTO_RELEASE_VERSION OR
   NOT _disposition_tag STREQUAL ANO_CRYPTO_RELEASE_TAG)
    message(FATAL_ERROR "Hosted artifact disposition has a different release identity")
endif()
if(_disposition_status STREQUAL "PASS")
    if(_disposition_reviewer STREQUAL "TODO" OR
       NOT _disposition_reviewed_utc MATCHES
           "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$" OR
       NOT _disposition_result STREQUAL "PASS" OR
       _disposition_commit STREQUAL "unknown" OR
       NOT _disposition_commit STREQUAL _prov_source_commit OR
       NOT _prov_worktree_state STREQUAL "CLEAN")
        message(FATAL_ERROR "Completed hosted disposition lacks reviewer/date/commit binding")
    endif()
elseif(NOT (_disposition_status STREQUAL "TODO" AND
             _disposition_result STREQUAL "TODO" AND
             _disposition_reviewer STREQUAL "TODO" AND
             _disposition_reviewed_utc STREQUAL "TODO"))
    message(FATAL_ERROR "Hosted artifact disposition must be a complete PASS or explicit TODO")
endif()

# This P1-32 bundle generator does not support signature artifacts. Reject
# signature/key/credential-looking files rather than silently treating them as
# verified release inputs.
foreach(_relative IN LISTS _bundle_files)
    string(TOLOWER "${_relative}" _lower)
    if(_lower MATCHES "\\.(sig|asc|pem|key|p12|pfx|jks|keystore|token|tsq|tsr|ed25519)$")
        message(FATAL_ERROR "Unsupported signing or credential artifact in bundle: ${_relative}")
    endif()
endforeach()

foreach(_relative IN LISTS _bundle_files)
    if(_relative STREQUAL "anocrypto-c-source.tar.gz")
        continue()
    endif()
    ano_crypto_detect_sensitive_signing_material_file(
        "${ANO_CRYPTO_PROVENANCE_BUNDLE_DIR}/${_relative}"
        ON _bundle_sensitive_reason)
    if(_bundle_sensitive_reason)
        message(FATAL_ERROR
            "Sensitive signing material found in provenance bundle: ${_relative} (${_bundle_sensitive_reason})")
    endif()
endforeach()

list(LENGTH _manifest_paths _manifest_file_count)
string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT}"
    "AnoCrypto-C P1-32 release provenance check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Expected tag: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Recorded source commit: ${_prov_source_commit}\n"
    "Recorded worktree state: ${_prov_worktree_state}\n"
    "Recorded actual tag: ${_prov_actual_tag}\n"
    "ABI line: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "Source files: ${_source_file_count}\n"
    "Public headers: ${_header_count}\n"
    "Public symbols: ${_symbol_count}\n"
    "Source archive member/content binding: PASS\n"
    "SPDX source inventory: PASS\n"
    "SPDX file path/SHA-256/SHA-1 binding: PASS\n"
    "SPDX package verification code: ${_package_verification_code}\n"
    "Manifest-covered bundle files: ${_manifest_file_count}\n"
    "Hosted artifact disposition: ${_disposition_status}\n"
    "Signing status: UNSIGNED\n"
    "Release authorization: TODO\n"
    "External release authorized: NO\n"
    "Hash/path/sensitive-marker verification: PASS\n"
    "Limit: JSON schema validation, dependency discovery, vulnerability analysis, cryptographic signature verification, and external-release approval were not performed.\n"
    "Result: PASS\n")

message(STATUS
    "AnoCrypto-C release provenance bundle verified (${_source_file_count} source files, ${_manifest_file_count} bound artifacts)")
