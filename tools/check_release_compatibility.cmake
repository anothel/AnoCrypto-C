# AnoCrypto-C P1-31 release identity and P1-29 source/ABI compatibility check.
# Engineering guardrail only; universal ABI compatibility is not established.
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT)
    set(ANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/RELEASE_COMPATIBILITY_CHECK.txt")
endif()
if(NOT DEFINED ANO_CRYPTO_ACTUAL_RELEASE_TAG)
    set(ANO_CRYPTO_ACTUAL_RELEASE_TAG "")
endif()
if(NOT DEFINED ANO_CRYPTO_ACTUAL_RELEASE_COMMIT)
    set(ANO_CRYPTO_ACTUAL_RELEASE_COMMIT "")
endif()
if(NOT DEFINED ANO_CRYPTO_REQUIRE_ACTUAL_RELEASE_TAG)
    set(ANO_CRYPTO_REQUIRE_ACTUAL_RELEASE_TAG OFF)
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_output_dir "${ANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")

set(_identity_file "${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
if(NOT EXISTS "${_identity_file}")
    message(FATAL_ERROR "Release identity file is missing: ${_identity_file}")
endif()
include("${_identity_file}")

foreach(_required
        ANO_CRYPTO_RELEASE_POLICY_SCHEMA
        ANO_CRYPTO_RELEASE_PACKAGE_VERSION
        ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA
        ANO_CRYPTO_RELEASE_SBOM_FORMAT
        ANO_CRYPTO_RELEASE_SIGNING_MODE
        ANO_CRYPTO_RELEASE_VERSION
        ANO_CRYPTO_RELEASE_PRERELEASE
        ANO_CRYPTO_RELEASE_TAG
        ANO_CRYPTO_RELEASE_FINGERPRINT
        ANO_CRYPTO_RELEASE_EVIDENCE_SLICE
        ANO_CRYPTO_RELEASE_ABI_LINE
        ANO_CRYPTO_RELEASE_SYMBOL_COUNT
        ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT
        ANO_CRYPTO_RELEASE_PACKAGE_COMPATIBILITY
        ANO_CRYPTO_COMPAT_BASELINE_VERSION
        ANO_CRYPTO_COMPAT_BASELINE_TAG)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Release identity field is missing: ${_required}")
    endif()
endforeach()
if(NOT ANO_CRYPTO_RELEASE_POLICY_SCHEMA STREQUAL "1")
    message(FATAL_ERROR "Unsupported release identity policy schema")
endif()
if(NOT ANO_CRYPTO_RELEASE_PACKAGE_COMPATIBILITY STREQUAL "SameMinorVersion")
    message(FATAL_ERROR "P1-31 package compatibility policy must be SameMinorVersion")
endif()
if(NOT ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA STREQUAL "1" OR
   NOT ANO_CRYPTO_RELEASE_SBOM_FORMAT STREQUAL "SPDX-2.3-JSON" OR
   NOT ANO_CRYPTO_RELEASE_SIGNING_MODE STREQUAL "UNSIGNED")
    message(FATAL_ERROR "P1-31 provenance/SBOM/signing identity policy is inconsistent")
endif()
foreach(_metadata IN ITEMS
        "${ANO_CRYPTO_ACTUAL_RELEASE_TAG}"
        "${ANO_CRYPTO_ACTUAL_RELEASE_COMMIT}")
    if(_metadata MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Actual release metadata contains a forbidden separator")
    endif()
endforeach()

set(_baseline_root
    "${ANO_CRYPTO_SOURCE_DIR}/abi/baselines/${ANO_CRYPTO_COMPAT_BASELINE_VERSION}")
set(_baseline_include "${_baseline_root}/include/anocrypto")
set(_current_include "${ANO_CRYPTO_SOURCE_DIR}/include/anocrypto")
set(_baseline_hashes "${_baseline_root}/PUBLIC_HEADER_SHA256.txt")
set(_baseline_symbols "${_baseline_root}/EXPORTED_SYMBOLS.txt")
set(_current_header_hashes "${ANO_CRYPTO_SOURCE_DIR}/release/PUBLIC_HEADERS.sha256")
set(_current_symbols
    "${ANO_CRYPTO_SOURCE_DIR}/abi/${ANO_CRYPTO_RELEASE_ABI_LINE}_EXPORTED_SYMBOLS.txt")

foreach(_path IN ITEMS
        "${_baseline_root}/README.md"
        "${_baseline_hashes}"
        "${_baseline_symbols}"
        "${_current_header_hashes}"
        "${_current_symbols}")
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}")
        message(FATAL_ERROR "Compatibility input is missing: ${_path}")
    endif()
endforeach()

function(_ano_normalize _input _output)
    set(_value "${_input}")
    string(REGEX REPLACE "[ \t\r\n]+" " " _value "${_value}")
    string(STRIP "${_value}" _value)
    set(${_output} "${_value}" PARENT_SCOPE)
endfunction()

function(_ano_append_safe _list_var _value)
    string(REPLACE ";" "<ANO_SEMICOLON>" _safe "${_value}")
    set(_items "${${_list_var}}")
    list(APPEND _items "${_safe}")
    set(${_list_var} "${_items}" PARENT_SCOPE)
endfunction()

function(_ano_collect_api_manifest _include_dir _manifest_file _prefix)
    file(GLOB _headers LIST_DIRECTORIES false "${_include_dir}/*.h")
    list(SORT _headers)
    if(NOT _headers)
        message(FATAL_ERROR "No public headers found in ${_include_dir}")
    endif()

    set(_header_names "")
    set(_functions "")
    set(_types "")
    set(_constants "")

    foreach(_header IN LISTS _headers)
        get_filename_component(_header_name "${_header}" NAME)
        list(APPEND _header_names "${_header_name}")
        file(READ "${_header}" _content)

        # Remove comments and preprocessor directives before declaration parsing.
        # This avoids counting ANO_CRYPTO_API macro definitions as functions.
        string(REGEX REPLACE "/\\*([^*]|\\*+[^*/])*\\*+/" " " _parsed "${_content}")
        string(REGEX REPLACE "//[^\r\n]*" " " _parsed "${_parsed}")
        string(REGEX REPLACE "(^|\n)[ \t]*#[^\r\n]*(\r?\n|$)" "\n" _parsed "${_parsed}")

        string(REGEX MATCHALL
            "ANO_CRYPTO_API[ \t\r\n]+[^;{}#]*ano_crypto_[A-Za-z0-9_]+[ \t\r\n]*\\([^;{}]*\\)[ \t\r\n]*;"
            _decls "${_parsed}")
        foreach(_decl IN LISTS _decls)
            _ano_normalize("${_decl}" _normalized)
            _ano_append_safe(_functions "${_normalized}")
        endforeach()

        string(REGEX MATCHALL
            "typedef[ \t\r\n]+enum[^{;]+\\{[^}]+\\}[ \t\r\n]+[A-Za-z0-9_]+[ \t\r\n]*;"
            _enum_blocks "${_parsed}")
        foreach(_block IN LISTS _enum_blocks)
            _ano_normalize("${_block}" _normalized)
            _ano_append_safe(_types "${_normalized}")
        endforeach()

        string(REGEX MATCHALL
            "typedef[ \t\r\n]+struct[^{;]+\\{[^}]+\\}[ \t\r\n]+[A-Za-z0-9_]+[ \t\r\n]*;"
            _struct_blocks "${_parsed}")
        foreach(_block IN LISTS _struct_blocks)
            _ano_normalize("${_block}" _normalized)
            _ano_append_safe(_types "${_normalized}")
        endforeach()

        string(REGEX MATCHALL
            "typedef[ \t\r\n]+[^;{}]+[ \t\r\n]+ano_crypto_[A-Za-z0-9_]+[ \t\r\n]*;"
            _simple_typedefs "${_parsed}")
        foreach(_typedef IN LISTS _simple_typedefs)
            _ano_normalize("${_typedef}" _normalized)
            _ano_append_safe(_types "${_normalized}")
        endforeach()

        file(STRINGS "${_header}" _define_lines REGEX "^[ \t]*#define[ \t]+ANO_CRYPTO_")
        foreach(_line IN LISTS _define_lines)
            _ano_normalize("${_line}" _normalized)
            if(_normalized MATCHES "^#define ANO_CRYPTO_API($| )")
                continue()
            endif()
            if(_normalized MATCHES "^#define ANO_CRYPTO_[A-Z0-9_]+_H($| )")
                continue()
            endif()
            if(_normalized MATCHES "^#define ANO_CRYPTO_VERSION_(PRERELEASE|STRING)($| )")
                continue()
            endif()
            _ano_append_safe(_constants "${_normalized}")
        endforeach()
    endforeach()

    list(FILTER _header_names EXCLUDE REGEX "^$")
    list(FILTER _functions EXCLUDE REGEX "^$")
    list(FILTER _types EXCLUDE REGEX "^$")
    list(FILTER _constants EXCLUDE REGEX "^$")
    list(REMOVE_DUPLICATES _functions)
    list(REMOVE_DUPLICATES _types)
    list(REMOVE_DUPLICATES _constants)
    list(SORT _functions)
    list(SORT _types)
    list(SORT _constants)
    list(SORT _header_names)

    list(LENGTH _header_names _header_count)
    list(LENGTH _functions _function_count)
    list(LENGTH _types _type_count)
    list(LENGTH _constants _constant_count)

    file(WRITE "${_manifest_file}"
        "AnoCrypto-C normalized public API manifest\n"
        "scope=${_prefix}\n"
        "header_count=${_header_count}\n"
        "function_count=${_function_count}\n"
        "type_count=${_type_count}\n"
        "constant_count=${_constant_count}\n\n"
        "[headers]\n")
    foreach(_item IN LISTS _header_names)
        file(APPEND "${_manifest_file}" "${_item}\n")
    endforeach()
    file(APPEND "${_manifest_file}" "\n[functions]\n")
    foreach(_item IN LISTS _functions)
        string(REPLACE "<ANO_SEMICOLON>" ";" _decoded "${_item}")
        file(APPEND "${_manifest_file}" "${_decoded}\n")
    endforeach()
    file(APPEND "${_manifest_file}" "\n[types]\n")
    foreach(_item IN LISTS _types)
        string(REPLACE "<ANO_SEMICOLON>" ";" _decoded "${_item}")
        file(APPEND "${_manifest_file}" "${_decoded}\n")
    endforeach()
    file(APPEND "${_manifest_file}" "\n[constants]\n")
    foreach(_item IN LISTS _constants)
        string(REPLACE "<ANO_SEMICOLON>" ";" _decoded "${_item}")
        file(APPEND "${_manifest_file}" "${_decoded}\n")
    endforeach()

    set(${_prefix}_HEADER_COUNT "${_header_count}" PARENT_SCOPE)
    set(${_prefix}_FUNCTION_COUNT "${_function_count}" PARENT_SCOPE)
    set(${_prefix}_TYPE_COUNT "${_type_count}" PARENT_SCOPE)
    set(${_prefix}_CONSTANT_COUNT "${_constant_count}" PARENT_SCOPE)
endfunction()

function(_ano_verify_hash_manifest _root _manifest _expected_count _description)
    file(STRINGS "${_manifest}" _hash_lines)
    set(_seen "")
    foreach(_line IN LISTS _hash_lines)
        string(STRIP "${_line}" _line)
        if(_line STREQUAL "")
            continue()
        endif()
        string(REGEX MATCH "^([0-9a-fA-F]+)[ \t][ \t]+(.+)$" _matched "${_line}")
        if(NOT _matched)
            message(FATAL_ERROR "Malformed ${_description} hash line: ${_line}")
        endif()
        string(TOLOWER "${CMAKE_MATCH_1}" _expected_hash)
        set(_relative_path "${CMAKE_MATCH_2}")
        if(_relative_path MATCHES "(^/|(^|/)\\.\\.(/|$))")
            message(FATAL_ERROR "Unsafe ${_description} hash path: ${_relative_path}")
        endif()
        if(_relative_path IN_LIST _seen)
            message(FATAL_ERROR "Duplicate ${_description} hash path: ${_relative_path}")
        endif()
        set(_full_path "${_root}/${_relative_path}")
        if(NOT EXISTS "${_full_path}" OR IS_DIRECTORY "${_full_path}" OR IS_SYMLINK "${_full_path}")
            message(FATAL_ERROR "${_description} file is missing or unsafe: ${_relative_path}")
        endif()
        file(SHA256 "${_full_path}" _actual_hash)
        if(NOT _actual_hash STREQUAL _expected_hash)
            message(FATAL_ERROR "${_description} hash mismatch: ${_relative_path}")
        endif()
        list(APPEND _seen "${_relative_path}")
    endforeach()
    list(LENGTH _seen _count)
    if(NOT "${_count}" STREQUAL "${_expected_count}")
        message(FATAL_ERROR
            "${_description} hash count ${_count} does not match expected count ${_expected_count}")
    endif()
endfunction()

function(_ano_read_symbol_list _path _output)
    file(STRINGS "${_path}" _lines)
    set(_symbols "")
    foreach(_line IN LISTS _lines)
        string(STRIP "${_line}" _line)
        if(NOT _line STREQUAL "" AND NOT _line MATCHES "^#")
            list(APPEND _symbols "${_line}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES _symbols)
    list(SORT _symbols)
    set(${_output} "${_symbols}" PARENT_SCOPE)
endfunction()

function(_ano_require_text _relative _needle)
    set(_path "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_path}")
        message(FATAL_ERROR "Required release policy file is missing: ${_relative}")
    endif()
    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_needle}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR "Required release text '${_needle}' is missing from ${_relative}")
    endif()
endfunction()

_ano_verify_hash_manifest(
    "${_baseline_root}" "${_baseline_hashes}"
    "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}" "baseline public header")
_ano_verify_hash_manifest(
    "${ANO_CRYPTO_SOURCE_DIR}" "${_current_header_hashes}"
    "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}" "current public header")

set(_baseline_manifest "${_output_dir}/P1_29_PUBLIC_API_NORMALIZED.txt")
set(_current_manifest "${_output_dir}/P1_31_PUBLIC_API_NORMALIZED.txt")
_ano_collect_api_manifest("${_baseline_include}" "${_baseline_manifest}" BASELINE)
_ano_collect_api_manifest("${_current_include}" "${_current_manifest}" CURRENT)
if(NOT "${BASELINE_HEADER_COUNT}" STREQUAL "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}" OR
   NOT "${CURRENT_HEADER_COUNT}" STREQUAL "${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}")
    message(FATAL_ERROR "Public header count differs from the release identity")
endif()
if(NOT "${BASELINE_FUNCTION_COUNT}" STREQUAL "${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}" OR
   NOT "${CURRENT_FUNCTION_COUNT}" STREQUAL "${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}")
    message(FATAL_ERROR "Public function declaration count differs from the 69-symbol ABI line")
endif()

file(READ "${_baseline_manifest}" _baseline_manifest_text)
file(READ "${_current_manifest}" _current_manifest_text)
string(REGEX REPLACE "scope=BASELINE\n" "scope=PUBLIC_API\n"
    _baseline_comparable "${_baseline_manifest_text}")
string(REGEX REPLACE "scope=CURRENT\n" "scope=PUBLIC_API\n"
    _current_comparable "${_current_manifest_text}")
string(SHA256 _baseline_comparable_hash "${_baseline_comparable}")
string(SHA256 _current_comparable_hash "${_current_comparable}")
if(NOT _baseline_comparable_hash STREQUAL _current_comparable_hash)
    message(FATAL_ERROR
        "Current public declarations/types/constants differ from the P1-29 baseline; compare ${_baseline_manifest} and ${_current_manifest}")
endif()

_ano_read_symbol_list("${_baseline_symbols}" _baseline_symbol_list)
_ano_read_symbol_list("${_current_symbols}" _current_symbol_list)
list(LENGTH _baseline_symbol_list _baseline_symbol_count)
list(LENGTH _current_symbol_list _current_symbol_count)
if(NOT _baseline_symbol_list STREQUAL _current_symbol_list)
    message(FATAL_ERROR "Current ABI-0 symbol list differs from the P1-29 compatibility baseline")
endif()
if(NOT "${_current_symbol_count}" STREQUAL "${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}")
    message(FATAL_ERROR "Release identity symbol count does not match the stored ABI list")
endif()

_ano_require_text("include/anocrypto/ac_version.h"
    "#define ANO_CRYPTO_VERSION_PRERELEASE \"${ANO_CRYPTO_RELEASE_PRERELEASE}\"")
_ano_require_text("include/anocrypto/ac_version.h"
    "#define ANO_CRYPTO_VERSION_STRING \"${ANO_CRYPTO_RELEASE_VERSION}\"")
_ano_require_text("CMakeLists.txt"
    "include(\"\${CMAKE_CURRENT_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake\")")
_ano_require_text("CMakeLists.txt"
    "COMPATIBILITY \${ANO_CRYPTO_RELEASE_PACKAGE_COMPATIBILITY}")
_ano_require_text("src/core/build_identity.c" "P1-31 C99 engineering slice")
_ano_require_text("README.md" "${ANO_CRYPTO_RELEASE_VERSION}")
_ano_require_text(".github/workflows/ci.yml" "${ANO_CRYPTO_RELEASE_FINGERPRINT}")
_ano_require_text(".github/workflows/ci.yml"
    "build/evidence/${ANO_CRYPTO_RELEASE_EVIDENCE_SLICE}")
_ano_require_text(
    "abi/baselines/${ANO_CRYPTO_COMPAT_BASELINE_VERSION}/include/anocrypto/ac_version.h"
    "#define ANO_CRYPTO_VERSION_STRING \"${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\"")
_ano_require_text("docs/release/TAGGED_RELEASE_COMPATIBILITY_POLICY.md"
    "${ANO_CRYPTO_RELEASE_TAG}")
_ano_require_text("docs/ci/RELEASE_COMPATIBILITY_CI_CHECKLIST.md"
    "ano_crypto_tagged_release_compatibility_smoke")
_ano_require_text("docs/kcmvp/TAGGED_RELEASE_COMPATIBILITY_EVIDENCE.md"
    "${ANO_CRYPTO_COMPAT_BASELINE_VERSION}")
_ano_require_text("evidence/release-template/RELEASE_COMPATIBILITY_REVIEW.md"
    "Tag/commit/HEAD identity")
_ano_require_text("tools/create_poc_package.cmake"
    "RELEASE_COMPATIBILITY_PACKAGE_NOTES.txt")
_ano_require_text("tests/compat/p1_29_compatibility_probe.c"
    "symbol_count != 69u")
_ano_require_text("tests/cmake/hosted_artifact_manifest_tests.cmake"
    "Sensitive-marker hosted artifact creation")

set(_tag_verification "NOT_REQUESTED")
set(_tag_commit "")
set(_head_commit "")
if(NOT ANO_CRYPTO_ACTUAL_RELEASE_TAG STREQUAL "" AND
   NOT ANO_CRYPTO_ACTUAL_RELEASE_TAG STREQUAL ANO_CRYPTO_RELEASE_TAG)
    message(FATAL_ERROR
        "Actual release tag '${ANO_CRYPTO_ACTUAL_RELEASE_TAG}' does not match expected tag '${ANO_CRYPTO_RELEASE_TAG}'")
endif()
if(ANO_CRYPTO_REQUIRE_ACTUAL_RELEASE_TAG)
    if(ANO_CRYPTO_ACTUAL_RELEASE_TAG STREQUAL "" OR
       ANO_CRYPTO_ACTUAL_RELEASE_COMMIT STREQUAL "")
        message(FATAL_ERROR "Tagged release verification requires actual tag and commit inputs")
    endif()
    find_program(_git_executable NAMES git)
    if(NOT _git_executable OR NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/.git")
        message(FATAL_ERROR "Tagged release verification requires a Git work tree")
    endif()
    execute_process(
        COMMAND "${_git_executable}" -C "${ANO_CRYPTO_SOURCE_DIR}" rev-parse HEAD
        RESULT_VARIABLE _head_result OUTPUT_VARIABLE _head_commit
        ERROR_VARIABLE _head_error OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND "${_git_executable}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            rev-parse "refs/tags/${ANO_CRYPTO_ACTUAL_RELEASE_TAG}^{commit}"
        RESULT_VARIABLE _tag_result OUTPUT_VARIABLE _tag_commit
        ERROR_VARIABLE _tag_error OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND "${_git_executable}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            rev-parse "${ANO_CRYPTO_ACTUAL_RELEASE_COMMIT}^{commit}"
        RESULT_VARIABLE _actual_commit_result OUTPUT_VARIABLE _actual_commit
        ERROR_VARIABLE _actual_commit_error OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _head_result EQUAL 0 OR NOT _tag_result EQUAL 0 OR
       NOT _actual_commit_result EQUAL 0)
        message(FATAL_ERROR
            "Tagged release Git resolution failed: ${_head_error}${_tag_error}${_actual_commit_error}")
    endif()
    if(NOT _tag_commit STREQUAL _head_commit OR
       NOT _actual_commit STREQUAL _head_commit)
        message(FATAL_ERROR "Release tag, supplied commit, and checked-out HEAD do not identify one commit")
    endif()
    set(_tag_verification "VERIFIED")
elseif(NOT ANO_CRYPTO_ACTUAL_RELEASE_TAG STREQUAL "")
    set(_tag_verification "TAG_STRING_MATCH_ONLY")
endif()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT}"
    "AnoCrypto-C P1-31 release compatibility check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Policy schema: ${ANO_CRYPTO_RELEASE_POLICY_SCHEMA}\n"
    "Current version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Package version: ${ANO_CRYPTO_RELEASE_PACKAGE_VERSION}\n"
    "Package compatibility: ${ANO_CRYPTO_RELEASE_PACKAGE_COMPATIBILITY}\n"
    "Expected tag: ${ANO_CRYPTO_RELEASE_TAG}\n"
    "Actual tag input: ${ANO_CRYPTO_ACTUAL_RELEASE_TAG}\n"
    "Actual commit input: ${ANO_CRYPTO_ACTUAL_RELEASE_COMMIT}\n"
    "Tag/commit verification: ${_tag_verification}\n"
    "Resolved tag commit: ${_tag_commit}\n"
    "Resolved HEAD commit: ${_head_commit}\n"
    "Compatibility baseline version: ${ANO_CRYPTO_COMPAT_BASELINE_VERSION}\n"
    "Compatibility baseline tag metadata: ${ANO_CRYPTO_COMPAT_BASELINE_TAG}\n"
    "ABI line: ${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "Public header count: ${CURRENT_HEADER_COUNT}\n"
    "Public function declaration count: ${CURRENT_FUNCTION_COUNT}\n"
    "Public type declaration count: ${CURRENT_TYPE_COUNT}\n"
    "Public constant declaration count: ${CURRENT_CONSTANT_COUNT}\n"
    "Exported symbol count: ${_current_symbol_count}\n"
    "Baseline snapshot hash manifest: PASS\n"
    "Current public-header hash manifest: PASS\n"
    "Normalized public API comparison: PASS\n"
    "Stored exported-symbol comparison: PASS\n"
    "Release identity consistency: PASS\n"
    "Limit: untested compiler ABIs, loaders, and complete semantic compatibility are not established.\n"
    "Result: PASS\n")

message(STATUS
    "AnoCrypto-C release compatibility check passed (${_current_symbol_count} symbols, ${CURRENT_FUNCTION_COUNT} function declarations, tag=${_tag_verification})")
