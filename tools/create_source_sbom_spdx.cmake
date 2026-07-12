# Generate a deterministic SPDX 2.3 JSON source inventory for the exact files
# selected for an AnoCrypto-C source archive. This is a source-file SBOM input
# record; it does not perform dependency discovery, license conclusions, or
# vulnerability analysis.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_SOURCE_FILE_LIST
        ANO_CRYPTO_SBOM_OUTPUT)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SOURCE_FILE_LIST
    "${ANO_CRYPTO_SOURCE_FILE_LIST}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_SBOM_OUTPUT "${ANO_CRYPTO_SBOM_OUTPUT}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")

if(NOT EXISTS "${ANO_CRYPTO_SOURCE_FILE_LIST}" OR
   IS_DIRECTORY "${ANO_CRYPTO_SOURCE_FILE_LIST}" OR
   IS_SYMLINK "${ANO_CRYPTO_SOURCE_FILE_LIST}")
    message(FATAL_ERROR
        "Source-file list is missing or unsafe: ${ANO_CRYPTO_SOURCE_FILE_LIST}")
endif()

if(NOT DEFINED ANO_CRYPTO_SBOM_CREATED_UTC OR
   ANO_CRYPTO_SBOM_CREATED_UTC STREQUAL "")
    string(TIMESTAMP ANO_CRYPTO_SBOM_CREATED_UTC "%Y-%m-%dT%H:%M:%SZ" UTC)
endif()
if(NOT ANO_CRYPTO_SBOM_CREATED_UTC MATCHES
   "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$")
    message(FATAL_ERROR
        "ANO_CRYPTO_SBOM_CREATED_UTC must use YYYY-MM-DDThh:mm:ssZ")
endif()
if(NOT DEFINED ANO_CRYPTO_SBOM_CREATOR OR
   ANO_CRYPTO_SBOM_CREATOR STREQUAL "")
    set(ANO_CRYPTO_SBOM_CREATOR
        "Tool: AnoCrypto-C CMake source-inventory generator")
endif()
if(ANO_CRYPTO_SBOM_CREATOR MATCHES "[\r\n]")
    message(FATAL_ERROR "SBOM creator must be one line")
endif()

function(_ano_json_escape _input _output)
    set(_value "${_input}")
    string(REPLACE "\\" "\\\\" _value "${_value}")
    string(REPLACE "\"" "\\\"" _value "${_value}")
    string(REPLACE "\n" "\\n" _value "${_value}")
    string(REPLACE "\r" "\\r" _value "${_value}")
    string(REPLACE "\t" "\\t" _value "${_value}")
    set(${_output} "${_value}" PARENT_SCOPE)
endfunction()

file(STRINGS "${ANO_CRYPTO_SOURCE_FILE_LIST}" _source_files)
set(_reviewed_files "")
foreach(_relative IN LISTS _source_files)
    string(STRIP "${_relative}" _stripped_relative)
    if(_stripped_relative STREQUAL "")
        continue()
    endif()
    if(NOT _stripped_relative STREQUAL _relative OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "\\\\")
        message(FATAL_ERROR "Unsafe source-file path in SBOM input: ${_relative}")
    endif()
    if(_relative IN_LIST _reviewed_files)
        message(FATAL_ERROR "Duplicate source-file path in SBOM input: ${_relative}")
    endif()
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "SBOM source file is missing or unsafe: ${_relative}")
    endif()
    list(APPEND _reviewed_files "${_relative}")
endforeach()
list(SORT _reviewed_files)
list(LENGTH _reviewed_files _file_count)
if(_file_count EQUAL 0)
    message(FATAL_ERROR "Source-file list is empty")
endif()

set(_sha1_values "")
set(_file_records "")
set(_relationship_records "")
set(_index 0)
foreach(_relative IN LISTS _reviewed_files)
    math(EXPR _index "${_index} + 1")
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    file(SHA256 "${_full}" _sha256)
    file(SHA1 "${_full}" _sha1)
    list(APPEND _sha1_values "${_sha1}")
    string(SUBSTRING "${_sha256}" 0 12 _short_hash)
    set(_spdx_id "SPDXRef-File-${_index}-${_short_hash}")
    _ano_json_escape("./${_relative}" _json_file_name)
    list(APPEND _file_records
        "    {\n      \"fileName\": \"${_json_file_name}\",\n      \"SPDXID\": \"${_spdx_id}\",\n      \"checksums\": [\n        {\n          \"algorithm\": \"SHA256\",\n          \"checksumValue\": \"${_sha256}\"\n        },\n        {\n          \"algorithm\": \"SHA1\",\n          \"checksumValue\": \"${_sha1}\"\n        }\n      ],\n      \"licenseConcluded\": \"NOASSERTION\",\n      \"licenseInfoInFiles\": [\"NOASSERTION\"],\n      \"copyrightText\": \"NOASSERTION\"\n    }")
    list(APPEND _relationship_records
        "    {\n      \"spdxElementId\": \"SPDXRef-Package-AnoCrypto-C\",\n      \"relationshipType\": \"CONTAINS\",\n      \"relatedSpdxElement\": \"${_spdx_id}\"\n    }")
endforeach()

list(SORT _sha1_values)
string(REPLACE ";" "" _verification_input "${_sha1_values}")
string(SHA1 _package_verification_code "${_verification_input}")

_ano_json_escape("AnoCrypto-C-${ANO_CRYPTO_RELEASE_VERSION}-source" _json_doc_name)
_ano_json_escape("${ANO_CRYPTO_SBOM_CREATOR}" _json_creator)
_ano_json_escape("${ANO_CRYPTO_RELEASE_VERSION}" _json_version)
set(_document_namespace
    "https://anocrypto.invalid/spdx/AnoCrypto-C/${ANO_CRYPTO_RELEASE_VERSION}/${_package_verification_code}")
_ano_json_escape("${_document_namespace}" _json_namespace)

get_filename_component(_output_dir "${ANO_CRYPTO_SBOM_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
file(WRITE "${ANO_CRYPTO_SBOM_OUTPUT}"
    "{\n"
    "  \"spdxVersion\": \"SPDX-2.3\",\n"
    "  \"dataLicense\": \"CC0-1.0\",\n"
    "  \"SPDXID\": \"SPDXRef-DOCUMENT\",\n"
    "  \"name\": \"${_json_doc_name}\",\n"
    "  \"documentNamespace\": \"${_json_namespace}\",\n"
    "  \"creationInfo\": {\n"
    "    \"created\": \"${ANO_CRYPTO_SBOM_CREATED_UTC}\",\n"
    "    \"creators\": [\"${_json_creator}\"],\n"
    "    \"comment\": \"Engineering source inventory only; dependency discovery, vulnerability analysis, and license conclusions were not performed.\"\n"
    "  },\n"
    "  \"documentDescribes\": [\"SPDXRef-Package-AnoCrypto-C\"],\n"
    "  \"packages\": [\n"
    "    {\n"
    "      \"name\": \"AnoCrypto-C\",\n"
    "      \"SPDXID\": \"SPDXRef-Package-AnoCrypto-C\",\n"
    "      \"versionInfo\": \"${_json_version}\",\n"
    "      \"packageFileName\": \"anocrypto-c-source.tar.gz\",\n"
    "      \"downloadLocation\": \"NOASSERTION\",\n"
    "      \"filesAnalyzed\": true,\n"
    "      \"packageVerificationCode\": {\n"
    "        \"packageVerificationCodeValue\": \"${_package_verification_code}\"\n"
    "      },\n"
    "      \"licenseConcluded\": \"NOASSERTION\",\n"
    "      \"licenseDeclared\": \"NOASSERTION\",\n"
    "      \"copyrightText\": \"NOASSERTION\",\n"
    "      \"primaryPackagePurpose\": \"LIBRARY\",\n"
    "      \"comment\": \"Private C99 cryptographic module engineering source inventory; no external-release authorization is implied.\"\n"
    "    }\n"
    "  ],\n"
    "  \"files\": [\n")

set(_first TRUE)
foreach(_record IN LISTS _file_records)
    if(_first)
        set(_first FALSE)
    else()
        file(APPEND "${ANO_CRYPTO_SBOM_OUTPUT}" ",\n")
    endif()
    file(APPEND "${ANO_CRYPTO_SBOM_OUTPUT}" "${_record}")
endforeach()

file(APPEND "${ANO_CRYPTO_SBOM_OUTPUT}"
    "\n  ],\n"
    "  \"relationships\": [\n"
    "    {\n"
    "      \"spdxElementId\": \"SPDXRef-DOCUMENT\",\n"
    "      \"relationshipType\": \"DESCRIBES\",\n"
    "      \"relatedSpdxElement\": \"SPDXRef-Package-AnoCrypto-C\"\n"
    "    }")
foreach(_record IN LISTS _relationship_records)
    file(APPEND "${ANO_CRYPTO_SBOM_OUTPUT}" ",\n${_record}")
endforeach()
file(APPEND "${ANO_CRYPTO_SBOM_OUTPUT}" "\n  ]\n}\n")

if(DEFINED ANO_CRYPTO_SBOM_SUMMARY_OUTPUT AND
   NOT ANO_CRYPTO_SBOM_SUMMARY_OUTPUT STREQUAL "")
    get_filename_component(_summary_dir "${ANO_CRYPTO_SBOM_SUMMARY_OUTPUT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_summary_dir}")
    file(SHA256 "${ANO_CRYPTO_SBOM_OUTPUT}" _sbom_sha256)
    file(WRITE "${ANO_CRYPTO_SBOM_SUMMARY_OUTPUT}"
        "AnoCrypto-C source SBOM generation summary\n"
        "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
        "Format: SPDX-2.3 JSON\n"
        "Source file count: ${_file_count}\n"
        "Package verification code: ${_package_verification_code}\n"
        "SBOM SHA-256: ${_sbom_sha256}\n"
        "Dependency discovery: not performed\n"
        "Vulnerability analysis: not performed\n"
        "License conclusions: NOASSERTION\n"
        "Result: PASS\n")
endif()

message(STATUS
    "Created SPDX 2.3 source inventory (${_file_count} files): ${ANO_CRYPTO_SBOM_OUTPUT}")
