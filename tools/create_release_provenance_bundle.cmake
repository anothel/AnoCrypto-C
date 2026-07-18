# Create a self-contained, unsigned AnoCrypto-C engineering provenance bundle.
# The bundle binds one source archive, source-file inventory, public-header
# manifest, ABI symbol list, SPDX source SBOM, hosted-review disposition, and
# release-authorization status with SHA-256. It deliberately performs no
# signing and carries no private key, token, or external-release approval.
cmake_minimum_required(VERSION 3.16)

foreach(_required
        ANO_CRYPTO_SOURCE_DIR
        ANO_CRYPTO_PROVENANCE_OUTPUT_DIR
        ANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE
        ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_OUTPUT_DIR
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE
    "${ANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST
    "${ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST}" ABSOLUTE)
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
include("${ANO_CRYPTO_SOURCE_DIR}/release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake")

foreach(_input IN ITEMS
        "${ANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE}"
        "${ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST}"
        "${ANO_CRYPTO_SOURCE_DIR}/release/PUBLIC_HEADERS.sha256"
        "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt")
    if(NOT EXISTS "${_input}" OR IS_DIRECTORY "${_input}" OR IS_SYMLINK "${_input}")
        message(FATAL_ERROR "Provenance input is missing or unsafe: ${_input}")
    endif()
endforeach()

if(NOT DEFINED ANO_CRYPTO_PROVENANCE_CREATED_UTC OR
   ANO_CRYPTO_PROVENANCE_CREATED_UTC STREQUAL "")
    string(TIMESTAMP ANO_CRYPTO_PROVENANCE_CREATED_UTC "%Y-%m-%dT%H:%M:%SZ" UTC)
endif()
if(NOT ANO_CRYPTO_PROVENANCE_CREATED_UTC MATCHES
   "^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z$")
    message(FATAL_ERROR
        "ANO_CRYPTO_PROVENANCE_CREATED_UTC must use YYYY-MM-DDThh:mm:ssZ")
endif()
if(NOT DEFINED ANO_CRYPTO_PROVENANCE_COMMIT OR
   ANO_CRYPTO_PROVENANCE_COMMIT STREQUAL "")
    set(ANO_CRYPTO_PROVENANCE_COMMIT "unknown")
endif()
if(NOT DEFINED ANO_CRYPTO_PROVENANCE_ACTUAL_TAG OR
   ANO_CRYPTO_PROVENANCE_ACTUAL_TAG STREQUAL "")
    set(ANO_CRYPTO_PROVENANCE_ACTUAL_TAG "not-recorded")
endif()
if(NOT DEFINED ANO_CRYPTO_PROVENANCE_WORKTREE_STATE OR
   ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "")
    set(ANO_CRYPTO_PROVENANCE_WORKTREE_STATE "UNKNOWN")
endif()
string(TOUPPER "${ANO_CRYPTO_PROVENANCE_WORKTREE_STATE}"
    ANO_CRYPTO_PROVENANCE_WORKTREE_STATE)
if(NOT (ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "CLEAN" OR
        ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "DIRTY" OR
        ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "UNKNOWN"))
    message(FATAL_ERROR
        "ANO_CRYPTO_PROVENANCE_WORKTREE_STATE must be CLEAN, DIRTY, or UNKNOWN")
endif()
foreach(_metadata IN ITEMS
        "${ANO_CRYPTO_PROVENANCE_COMMIT}"
        "${ANO_CRYPTO_PROVENANCE_ACTUAL_TAG}")
    if(_metadata MATCHES "[|;\r\n]")
        message(FATAL_ERROR "Provenance metadata contains a forbidden separator")
    endif()
endforeach()

file(REMOVE_RECURSE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}")

set(_archive_name "anocrypto-c-source.tar.gz")
set(_archive_out "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/${_archive_name}")
configure_file("${ANO_CRYPTO_PROVENANCE_SOURCE_ARCHIVE}" "${_archive_out}" COPYONLY)

set(_source_hashes "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SOURCE_FILES.sha256")
file(STRINGS "${ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST}" _source_files)
set(_reviewed_source_files "")
foreach(_relative IN LISTS _source_files)
    string(STRIP "${_relative}" _stripped_relative)
    if(_stripped_relative STREQUAL "")
        continue()
    endif()
    if(NOT _stripped_relative STREQUAL _relative OR
       _relative MATCHES "(^/|^[A-Za-z]:[/\\\\]|(^|/)\\.\\.(/|$)|[|;\r\n\t])" OR
       _relative MATCHES "\\\\")
        message(FATAL_ERROR "Unsafe source path in provenance input: ${_relative}")
    endif()
    if(_relative IN_LIST _reviewed_source_files)
        message(FATAL_ERROR "Duplicate source path in provenance input: ${_relative}")
    endif()
    set(_full "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
    if(NOT EXISTS "${_full}" OR IS_DIRECTORY "${_full}" OR IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Provenance source file is missing or unsafe: ${_relative}")
    endif()
    list(APPEND _reviewed_source_files "${_relative}")
endforeach()
list(SORT _reviewed_source_files)
list(LENGTH _reviewed_source_files _source_file_count)
if(_source_file_count EQUAL 0)
    message(FATAL_ERROR "Provenance source-file list is empty")
endif()
file(WRITE "${_source_hashes}" "")
foreach(_relative IN LISTS _reviewed_source_files)
    file(SHA256 "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" _hash)
    file(APPEND "${_source_hashes}" "${_hash}  ${_relative}\n")
endforeach()

configure_file(
    "${ANO_CRYPTO_SOURCE_DIR}/release/PUBLIC_HEADERS.sha256"
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/PUBLIC_HEADERS.sha256" COPYONLY)
configure_file(
    "${ANO_CRYPTO_SOURCE_DIR}/abi/ANOCRYPTO_C_CURRENT_EXPORTED_SYMBOLS.txt"
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/EXPORTED_SYMBOLS.txt" COPYONLY)

set(_sbom "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SOURCE_SBOM.spdx.json")
set(_sbom_summary "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SOURCE_SBOM_SUMMARY.txt")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
        -DANO_CRYPTO_SOURCE_FILE_LIST=${ANO_CRYPTO_PROVENANCE_SOURCE_FILE_LIST}
        -DANO_CRYPTO_SBOM_OUTPUT=${_sbom}
        -DANO_CRYPTO_SBOM_SUMMARY_OUTPUT=${_sbom_summary}
        -DANO_CRYPTO_SBOM_CREATED_UTC=${ANO_CRYPTO_PROVENANCE_CREATED_UTC}
        -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_source_sbom_spdx.cmake"
    RESULT_VARIABLE _sbom_result
    OUTPUT_VARIABLE _sbom_stdout
    ERROR_VARIABLE _sbom_stderr
    TIMEOUT 120)
if(NOT _sbom_result EQUAL 0)
    message(FATAL_ERROR
        "Source SBOM generation failed:\n${_sbom_stdout}${_sbom_stderr}")
endif()

set(_disposition "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/HOSTED_ARTIFACT_DISPOSITION.txt")
if(DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_DISPOSITION_FILE AND
   NOT ANO_CRYPTO_HOSTED_ARTIFACT_DISPOSITION_FILE STREQUAL "")
    get_filename_component(_input_disposition
        "${ANO_CRYPTO_HOSTED_ARTIFACT_DISPOSITION_FILE}" ABSOLUTE)
    if(NOT EXISTS "${_input_disposition}" OR
       IS_DIRECTORY "${_input_disposition}" OR
       IS_SYMLINK "${_input_disposition}")
        message(FATAL_ERROR
            "Hosted artifact disposition is missing or unsafe: ${_input_disposition}")
    endif()
    configure_file("${_input_disposition}" "${_disposition}" COPYONLY)
else()
    file(WRITE "${_disposition}"
        "ANOCRYPTO_HOSTED_ARTIFACT_DISPOSITION_V2\n"
        "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
        "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
        "workflow_run_id=NOT_RUN\n"
        "workflow_commit=unknown\n"
        "workflow_ref=NOT_RUN\n"
        "required_tuple_count=16\n"
        "required_hosted_tuple_count=0\n"
        "hosted_runner_bundle_count=0\n"
        "synthetic_local_bundle_count=0\n"
        "artifact_index_sha256=NOT_RUN\n"
        "artifact_files_manifest_sha256=NOT_RUN\n"
        "hosted_execution_status=NOT_RUN\n"
        "review_status=UNREVIEWED\n"
        "reviewer=UNPROVISIONED\n"
        "review_date=UNREVIEWED\n"
        "disposition=UNREVIEWED\n"
        "security_reviewer_identity=UNPROVISIONED\n"
        "product_release_owner_identity=UNPROVISIONED\n"
        "role_separation_status=UNPROVISIONED\n"
        "signing_status=UNSIGNED\n"
        "handoff_status=BLOCKED\n"
        "external_release_authorized=NO\n"
        "github_attestation_role=OPTIONAL_SECONDARY_NOT_TRUST_ROOT\n"
        "integrity_status=NOT_RUN\n"
        "status=UNREVIEWED\n"
        "result=BLOCKED\n"
        "limit=No actual hosted artifact disposition was supplied to this local engineering bundle.\n")
endif()

set(_hosted_index_input "")
set(_hosted_files_manifest_input "")
if(DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_INDEX_FILE AND
   NOT ANO_CRYPTO_HOSTED_ARTIFACT_INDEX_FILE STREQUAL "")
    get_filename_component(_hosted_index_input
        "${ANO_CRYPTO_HOSTED_ARTIFACT_INDEX_FILE}" ABSOLUTE)
endif()
if(DEFINED ANO_CRYPTO_HOSTED_ARTIFACT_FILES_MANIFEST AND
   NOT ANO_CRYPTO_HOSTED_ARTIFACT_FILES_MANIFEST STREQUAL "")
    get_filename_component(_hosted_files_manifest_input
        "${ANO_CRYPTO_HOSTED_ARTIFACT_FILES_MANIFEST}" ABSOLUTE)
endif()
if((_hosted_index_input STREQUAL "" AND
    NOT _hosted_files_manifest_input STREQUAL "") OR
   (NOT _hosted_index_input STREQUAL "" AND
    _hosted_files_manifest_input STREQUAL ""))
    message(FATAL_ERROR
        "Hosted artifact index and files manifest must be supplied together")
endif()
if(NOT _hosted_index_input STREQUAL "")
    foreach(_hosted_input IN ITEMS
            "${_hosted_index_input}" "${_hosted_files_manifest_input}")
        if(NOT EXISTS "${_hosted_input}" OR
           IS_DIRECTORY "${_hosted_input}" OR
           IS_SYMLINK "${_hosted_input}")
            message(FATAL_ERROR "Hosted artifact binding input is missing or unsafe")
        endif()
    endforeach()
    configure_file("${_hosted_index_input}"
        "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/HOSTED_ARTIFACT_INDEX.txt" COPYONLY)
    configure_file("${_hosted_files_manifest_input}"
        "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/HOSTED_ARTIFACT_FILES.sha256" COPYONLY)
endif()

file(SHA256 "${_archive_out}" _archive_hash)
file(SHA256 "${_source_hashes}" _source_hash_manifest_hash)
file(SHA256 "${_sbom}" _sbom_hash)
file(SHA256
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/PUBLIC_HEADERS.sha256"
    _public_headers_manifest_hash)
file(SHA256
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/EXPORTED_SYMBOLS.txt"
    _exported_symbols_hash)
file(SHA256 "${_disposition}" _disposition_hash)

if(ANO_CRYPTO_PROVENANCE_COMMIT STREQUAL "unknown")
    set(_commit_status "UNVERIFIED")
elseif(ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "CLEAN")
    set(_commit_status "RECORDED_CLEAN_NOT_ATTESTED")
elseif(ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "DIRTY")
    set(_commit_status "RECORDED_DIRTY_NOT_ATTESTED")
else()
    set(_commit_status "RECORDED_WORKTREE_UNKNOWN_NOT_ATTESTED")
endif()
if(ANO_CRYPTO_PROVENANCE_ACTUAL_TAG STREQUAL "not-recorded")
    set(_tag_status "UNVERIFIED")
elseif(ANO_CRYPTO_PROVENANCE_ACTUAL_TAG STREQUAL ANO_CRYPTO_RELEASE_TAG)
    if(NOT ANO_CRYPTO_PROVENANCE_WORKTREE_STATE STREQUAL "CLEAN" OR
       ANO_CRYPTO_PROVENANCE_COMMIT STREQUAL "unknown")
        message(FATAL_ERROR
            "A recorded actual release tag requires a known commit and CLEAN worktree state")
    endif()
    set(_tag_status "STRING_MATCH_NOT_ATTESTED")
else()
    message(FATAL_ERROR
        "Actual provenance tag does not match expected tag metadata")
endif()

file(WRITE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SBOM_STATUS.txt"
    "AnoCrypto-C SBOM status\n"
    "Status: GENERATED_SOURCE_INVENTORY\n"
    "Format: SPDX-2.3 JSON\n"
    "Scope: files selected for anocrypto-c-source.tar.gz\n"
    "Source file count: ${_source_file_count}\n"
    "Dependency discovery: not performed\n"
    "Vulnerability analysis: not performed\n"
    "License conclusions: NOASSERTION\n"
    "External dependency completeness: not claimed\n"
    "Official validation evidence: no\n")

file(WRITE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SIGNING_STATUS.txt"
    "AnoCrypto-C signing status\n"
    "Status: UNSIGNED\n"
    "Signing subject: RELEASE_ARTIFACTS.sha256\n"
    "Detached signature present: no\n"
    "Cryptographic verification performed: no\n"
    "Signer identity approved: no (UNPROVISIONED)\n"
    "Timestamp trust source approved: no (UNPROVISIONED)\n"
    "Private keys or signing tokens included: no\n"
    "External release authorization implied: no\n")

file(WRITE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/SIGNING_REQUEST.txt"
    "ANOCRYPTO_DETACHED_SIGNING_REQUEST_V1\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "subject=${ANO_CRYPTO_RELEASE_SIGNING_SUBJECT}\n"
    "algorithm=${ANO_CRYPTO_RELEASE_SIGNING_ALGORITHM}\n"
    "signature_format=${ANO_CRYPTO_RELEASE_SIGNING_SIGNATURE_ENCODING}\n"
    "verification_tool=${ANO_CRYPTO_RELEASE_SIGNING_TOOL_FAMILY}\n"
    "signer_policy_id=${ANO_CRYPTO_RELEASE_SIGNING_SIGNER_POLICY_ID}\n"
    "timestamp_policy=${ANO_CRYPTO_RELEASE_SIGNING_TIMESTAMP_FORMAT}\n"
    "production_signer=UNPROVISIONED\n"
    "timestamp_trust_source=UNPROVISIONED\n"
    "handoff_status=BLOCKED\n"
    "status=UNSIGNED\n"
    "note=Do not place private keys, tokens, or credentials in this bundle.\n")

file(WRITE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/RELEASE_AUTHORIZATION.md"
    "# Release Authorization Record\n\n"
    "Status: BLOCKED\n\n"
    "Release: `${ANO_CRYPTO_RELEASE_VERSION}`  \n"
    "Expected tag: `${ANO_CRYPTO_RELEASE_TAG}`  \n"
    "Recorded commit: `${ANO_CRYPTO_PROVENANCE_COMMIT}`  \n"
    "Recorded worktree state: `${ANO_CRYPTO_PROVENANCE_WORKTREE_STATE}`\n\n"
    "Required before external release:\n\n"
    "- [ ] Tag, supplied commit, and checked-out HEAD resolve to one reviewed commit.\n"
    "- [ ] Required hosted artifacts have an explicit reviewer/date disposition.\n"
    "- [ ] Source SBOM scope and limitations have been reviewed.\n"
    "- [ ] Production signer identity and timestamp trust source are provisioned and approved.\n"
    "- [ ] The raw Ed25519 detached signature and RFC 3161 timestamp have been verified by the P2-02 retained offline verifier.\n"
    "- [ ] Legal/product approval and customer-sharing scope are recorded.\n\n"
    "Actual release tag generated: NO\n"
    "GitHub attestation role: OPTIONAL_SECONDARY_NOT_TRUST_ROOT\n"
    "External release authorized: NO\n"
    "Reviewer: UNPROVISIONED\n"
    "Review date: UNREVIEWED\n"
    "Limitations: actual hosted review, tag, production signer/timestamp provisioning, and signing remain incomplete.\n")

file(WRITE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/RELEASE_PROVENANCE.txt"
    "ANOCRYPTO_RELEASE_PROVENANCE_V1\n"
    "policy_schema=${ANO_CRYPTO_RELEASE_PROVENANCE_SCHEMA}\n"
    "release_version=${ANO_CRYPTO_RELEASE_VERSION}\n"
    "package_version=${ANO_CRYPTO_RELEASE_PACKAGE_VERSION}\n"
    "expected_tag=${ANO_CRYPTO_RELEASE_TAG}\n"
    "actual_tag=${ANO_CRYPTO_PROVENANCE_ACTUAL_TAG}\n"
    "tag_status=${_tag_status}\n"
    "source_commit=${ANO_CRYPTO_PROVENANCE_COMMIT}\n"
    "worktree_state=${ANO_CRYPTO_PROVENANCE_WORKTREE_STATE}\n"
    "commit_status=${_commit_status}\n"
    "abi_line=${ANO_CRYPTO_RELEASE_ABI_LINE}\n"
    "public_header_count=${ANO_CRYPTO_RELEASE_PUBLIC_HEADER_COUNT}\n"
    "public_symbol_count=${ANO_CRYPTO_RELEASE_SYMBOL_COUNT}\n"
    "source_file_count=${_source_file_count}\n"
    "source_archive=${_archive_name}\n"
    "source_archive_sha256=${_archive_hash}\n"
    "source_hash_manifest=SOURCE_FILES.sha256\n"
    "source_hash_manifest_sha256=${_source_hash_manifest_hash}\n"
    "source_sbom=SOURCE_SBOM.spdx.json\n"
    "source_sbom_format=SPDX-2.3-JSON\n"
    "source_sbom_sha256=${_sbom_hash}\n"
    "public_header_manifest=PUBLIC_HEADERS.sha256\n"
    "public_header_manifest_sha256=${_public_headers_manifest_hash}\n"
    "exported_symbol_manifest=EXPORTED_SYMBOLS.txt\n"
    "exported_symbol_manifest_sha256=${_exported_symbols_hash}\n"
    "hosted_artifact_disposition=HOSTED_ARTIFACT_DISPOSITION.txt\n"
    "hosted_artifact_disposition_sha256=${_disposition_hash}\n"
    "signing_status=UNSIGNED\n"
    "release_authorization=BLOCKED\n"
    "external_release_authorized=NO\n"
    "generated_utc=${ANO_CRYPTO_PROVENANCE_CREATED_UTC}\n"
    "result=PASS\n"
    "limit=Engineering provenance and hash binding only; no signature, hosted review completion, or official validation claim.\n")

set(_manifest "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/RELEASE_ARTIFACTS.sha256")
file(GLOB _bundle_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}"
    "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/*")
list(SORT _bundle_files)
file(WRITE "${_manifest}" "")
foreach(_relative IN LISTS _bundle_files)
    if(_relative STREQUAL "RELEASE_ARTIFACTS.sha256")
        continue()
    endif()
    set(_full "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}/${_relative}")
    if(IS_SYMLINK "${_full}")
        message(FATAL_ERROR "Provenance bundle rejects symlinks: ${_relative}")
    endif()
    file(SHA256 "${_full}" _hash)
    file(APPEND "${_manifest}" "${_hash}  ${_relative}\n")
endforeach()

if(NOT DEFINED ANO_CRYPTO_VERIFY_PROVENANCE_BUNDLE)
    set(ANO_CRYPTO_VERIFY_PROVENANCE_BUNDLE ON)
endif()
if(ANO_CRYPTO_VERIFY_PROVENANCE_BUNDLE)
    if(DEFINED ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT AND
       NOT ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT STREQUAL "")
        set(_check_output "${ANO_CRYPTO_PROVENANCE_CHECK_OUTPUT}")
    else()
        get_filename_component(_bundle_parent
            "${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}" DIRECTORY)
        set(_check_output "${_bundle_parent}/RELEASE_PROVENANCE_CHECK.txt")
    endif()
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_PROVENANCE_BUNDLE_DIR=${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}
            -DANO_CRYPTO_PROVENANCE_CHECK_OUTPUT=${_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_provenance_bundle.cmake"
        RESULT_VARIABLE _check_result
        OUTPUT_VARIABLE _check_stdout
        ERROR_VARIABLE _check_stderr
        TIMEOUT 300)
    if(NOT _check_result EQUAL 0)
        message(FATAL_ERROR
            "Release provenance verification failed:\n${_check_stdout}${_check_stderr}")
    endif()
endif()

message(STATUS
    "Created unsigned release provenance bundle: ${ANO_CRYPTO_PROVENANCE_OUTPUT_DIR}")
