# Static and generated-artifact policy check for the retained unsigned release
# provenance/SBOM/signing boundary.
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_RELEASE_PROVENANCE_CHECK_OUTPUT)
    set(ANO_CRYPTO_RELEASE_PROVENANCE_CHECK_OUTPUT
        "${ANO_CRYPTO_BUILD_DIR}/policy-check/RELEASE_PROVENANCE_POLICY_CHECK.txt")
endif()
get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_output_dir "${ANO_CRYPTO_RELEASE_PROVENANCE_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/tools/sensitive_signing_material_helpers.cmake")

foreach(_required_field
        ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA
        ANO_CRYPTO_RELEASE_SBOM_FORMAT
        ANO_CRYPTO_RELEASE_SIGNING_MODE)
    if(NOT DEFINED ${_required_field} OR "${${_required_field}}" STREQUAL "")
        message(FATAL_ERROR "Release provenance identity field is missing: ${_required_field}")
    endif()
endforeach()
if(NOT ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA STREQUAL "1" OR
   NOT ANO_CRYPTO_RELEASE_SBOM_FORMAT STREQUAL "SPDX-2.3-JSON" OR
   NOT ANO_CRYPTO_RELEASE_SIGNING_MODE STREQUAL "UNSIGNED")
    message(FATAL_ERROR "Unexpected current release provenance identity policy")
endif()

set(_required_files
    tools/create_source_sbom_spdx.cmake
    tools/create_release_provenance_bundle.cmake
    tools/check_release_provenance_bundle.cmake
    tools/check_release_provenance_policy.cmake
    tools/assemble_release_evidence.cmake
    tools/check_sensitive_release_artifacts.cmake
    tools/sensitive_signing_material_helpers.cmake
    tools/create_release_signing_handoff.cmake
    tools/check_release_signing_handoff.cmake
    tools/verify_release_detached_signature.cmake
    tools/create_hosted_artifact_manifest.cmake
    tools/check_hosted_artifact_manifest.cmake
    tools/create_hosted_artifact_index.cmake
    tools/hosted_artifact_path_helpers.cmake
    tests/cmake/release_provenance_tests.cmake
    tests/cmake/release_signing_handoff_tests.cmake
    tests/cmake/sensitive_release_artifact_tests.cmake
    tests/cmake/release_signing_fixture_tests.cmake
    tests/cmake/tagged_release_rehearsal_tests.cmake
    tests/cmake/hosted_artifact_manifest_tests.cmake
    docs/release/RELEASE_PROVENANCE_AND_SIGNING_POLICY.md
    docs/ci/RELEASE_PROVENANCE_CI_CHECKLIST.md
    evidence/release-template/RELEASE_PROVENANCE_REVIEW.md)
foreach(_relative IN LISTS _required_files)
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Required release provenance file is missing or unsafe: ${_relative}")
    endif()
endforeach()

function(_ano_require_text _relative _needle)
    set(_path "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}")
        message(FATAL_ERROR "Required provenance policy input is missing: ${_relative}")
    endif()
    file(READ "${_path}" _content)
    string(FIND "${_content}" "${_needle}" _position)
    if(_position EQUAL -1)
        message(FATAL_ERROR
            "Required provenance text '${_needle}' is missing from ${_relative}")
    endif()
endfunction()

_ano_require_text("release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake"
    "set(ANO_CRYPTO_RELEASE_SIGNING_MODE \"UNSIGNED\")")
_ano_require_text("CMakeLists.txt" "ano_crypto_release_provenance_policy_check")
_ano_require_text("CMakeLists.txt" "ano_crypto_release_provenance_tests")
_ano_require_text("CMakeLists.txt" "ano_crypto_release_signing_handoff_tests")
_ano_require_text("CMakeLists.txt" "ano_crypto_tagged_release_rehearsal_tests")
_ano_require_text("CMakeLists.txt" "ano_crypto_hosted_artifact_manifest_tests")
_ano_require_text("CMakeLists.txt" "ano_crypto_release_signing_handoff_evidence")
_ano_require_text("CMakeLists.txt" "ANO_CRYPTO_RUN_RELEASE_PROVENANCE_CHECK=ON")
_ano_require_text(".github/workflows/ci.yml" "release-provenance")
_ano_require_text(".github/workflows/ci.yml"
    "ANO_CRYPTO_HOSTED_ARTIFACT_ORIGIN=HOSTED_RUNNER")
_ano_require_text(".github/workflows/ci.yml" "release-signing-handoff")
_ano_require_text(".github/workflows/ci.yml" "SOURCE_SBOM.spdx.json")
_ano_require_text("tools/create_hosted_artifact_manifest.cmake"
    "ANOCRYPTO_HOSTED_ARTIFACT_MANIFEST_V2")
_ano_require_text("tools/create_hosted_artifact_index.cmake"
    "release-signing-handoff|Linux|x86_64|gcc")
_ano_require_text("tools/create_hosted_artifact_index.cmake"
    "github_attestation_role=")
_ano_require_text("tools/create_release_provenance_bundle.cmake" "RELEASE_PROVENANCE.txt")
_ano_require_text("tools/create_release_provenance_bundle.cmake" "worktree_state=")
_ano_require_text("tools/create_release_signing_handoff.cmake" "READY_FOR_OFFLINE_SIGNING")
_ano_require_text("tools/verify_release_detached_signature.cmake" "Raw Ed25519 detached signature must be exactly 64 bytes")
_ano_require_text("tools/verify_release_detached_signature.cmake" "RFC3161 timestamp response verification failed")
_ano_require_text("tools/collect_release_evidence.cmake" "release-provenance")
_ano_require_text("tools/collect_release_evidence.cmake" "SOURCE_SBOM.spdx.json")
_ano_require_text("tools/create_poc_package.cmake" "RELEASE_PROVENANCE_PACKAGE_NOTES.txt")
_ano_require_text("tools/create_poc_package.cmake"
    "The internal source archive and source SBOM are not included")
_ano_require_text("docs/kcmvp/SBOM_SIGNING_PLAN.md" "SPDX 2.3 JSON source inventory")
_ano_require_text("docs/release/RELEASE_PROVENANCE_AND_SIGNING_POLICY.md"
    "Status: P3-08 unsigned engineering policy retaining P1-33 controls")
_ano_require_text("docs/release/RELEASE_PROVENANCE_AND_SIGNING_POLICY.md"
    "External release authorized: NO")
_ano_require_text("docs/ci/RELEASE_PROVENANCE_CI_CHECKLIST.md"
    "ano_crypto_release_provenance_tests")
_ano_require_text("tools/check_release_provenance_bundle.cmake"
    "Source archive member/content binding: PASS")
_ano_require_text("tools/create_source_sbom_spdx.cmake"
    "licenseInfoInFiles")
_ano_require_text("evidence/release-template/RELEASE_PROVENANCE_REVIEW.md"
    "Cryptographic signature verification")
_ano_require_text("README.md" "${ANO_CRYPTO_RELEASE_VERSION}")
_ano_require_text("README.md" "unsigned release-provenance bundle")

# A signing credential or detached signature must never be committed merely to
# make the retained engineering gate pass.
set(_source_entries "")
find_program(_git_executable git)
if(_git_executable)
    execute_process(
        COMMAND "${_git_executable}" -C "${ANO_CRYPTO_SOURCE_DIR}"
            ls-files --cached --others --exclude-standard
        RESULT_VARIABLE _git_files_result
        OUTPUT_VARIABLE _git_files_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 60)
    if("${_git_files_result}" STREQUAL "0")
        string(REPLACE "\r\n" "\n" _git_files_output "${_git_files_output}")
        string(REPLACE "\n" ";" _source_entries "${_git_files_output}")
    endif()
endif()
if(NOT _source_entries)
    file(GLOB_RECURSE _source_entries
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
        "${ANO_CRYPTO_SOURCE_DIR}/*")
endif()
set(_credential_like_files "")
set(_sensitive_source_files "")
foreach(_relative IN LISTS _source_entries)
    if(_relative MATCHES "(^|/)(\\.git|build|build-[^/]*|cmake-build-[^/]*)(/|$)")
        continue()
    endif()
    string(TOLOWER "${_relative}" _lower)
    if(_lower MATCHES "\\.(sig|asc|pem|key|p12|pfx|jks|keystore|token|tsq|tsr|ed25519)$")
        list(APPEND _credential_like_files "${_relative}")
    endif()
    set(_source_file "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    ano_crypto_detect_sensitive_signing_material_file(
        "${_source_file}" ON _sensitive_reason)
    if(_sensitive_reason)
        list(APPEND _sensitive_source_files "${_relative} (${_sensitive_reason})")
    endif()
endforeach()
if(_credential_like_files)
    list(JOIN _credential_like_files ", " _credential_text)
    message(FATAL_ERROR
        "Credential/signature-like files are forbidden in the source tree: ${_credential_text}")
endif()
if(_sensitive_source_files)
    list(JOIN _sensitive_source_files ", " _sensitive_source_text)
    message(FATAL_ERROR
        "Sensitive signing material is forbidden in the source tree: ${_sensitive_source_text}")
endif()

# If the full evidence target has already produced a provenance bundle, verify
# the generated files as part of the policy target. The static policy remains
# runnable before evidence collection.
set(_generated_bundle
    "${ANO_CRYPTO_BUILD_DIR}/evidence/${ANO_CRYPTO_RELEASE_EVIDENCE_SLICE}/release-provenance")
set(_generated_status "NOT_PRESENT")
if(NOT DEFINED ANO_CRYPTO_SKIP_GENERATED_PROVENANCE_CHECK)
    set(ANO_CRYPTO_SKIP_GENERATED_PROVENANCE_CHECK OFF)
endif()
if(NOT ANO_CRYPTO_SKIP_GENERATED_PROVENANCE_CHECK AND
   IS_DIRECTORY "${_generated_bundle}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${_generated_bundle}
            -DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_output_dir}/GENERATED_RELEASE_PROVENANCE_CHECK.txt
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_bundle.cmake"
        RESULT_VARIABLE _generated_result
        OUTPUT_VARIABLE _generated_stdout
        ERROR_VARIABLE _generated_stderr
        TIMEOUT 120)
    if(NOT _generated_result EQUAL 0)
        message(FATAL_ERROR
            "Generated release provenance bundle failed verification:\n${_generated_stdout}${_generated_stderr}")
    endif()
    set(_generated_status "PASS")
endif()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_RELEASE_PROVENANCE_CHECK_OUTPUT}"
    "AnoCrypto-C P3-08 release provenance policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Release version: ${ANO_CRYPTO_RELEASE_VERSION}\n"
    "Provenance schema: ${ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA}\n"
    "SBOM format: ${ANO_CRYPTO_RELEASE_SBOM_FORMAT}\n"
    "Signing mode: ${ANO_CRYPTO_RELEASE_SIGNING_MODE}\n"
    "Public headers: ${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}\n"
    "Public symbols: ${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "Source SBOM generator: present\n"
    "Unsigned provenance bundle generator/verifier: present\n"
    "Tamper/claim/path/disposition regression: configured\n"
    "Credential/signature-like source files: none\n"
    "Generated evidence bundle verification: ${_generated_status}\n"
    "Actual hosted execution: NOT_RUN unless separately supplied\n"
    "Hosted artifact review: UNREVIEWED\n"
    "Offline signature verifier: Ed25519 raw 64-byte over RELEASE_ARTIFACTS.sha256\n"
    "Signer identity binding: SHA-256 of DER SubjectPublicKeyInfo\n"
    "Timestamp verifier: RFC 3161 over detached signature\n"
    "Production signer: UNPROVISIONED\n"
    "Timestamp trust source: UNPROVISIONED\n"
    "Repository signing status: UNSIGNED\n"
    "Signing handoff readiness: BLOCKED\n"
    "External release authorization: NO\n"
    "Result: PASS\n")

message(STATUS "AnoCrypto-C P3-08 release provenance policy check passed")
