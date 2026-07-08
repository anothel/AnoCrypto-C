# anocrypto-c release evidence collection draft.
# Usage:
#   cmake -DANO_CRYPTO_SOURCE_DIR=. \
#         -DANO_CRYPTO_BUILD_DIR=build \
#         -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-14 \
#         -DANO_CRYPTO_RUN_CTEST=ON \
#         -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
#         -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
#         -DANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON \
#         -P tools/collect_release_evidence.cmake
#
# This script creates a local evidence draft for engineering review. It does
# not create official validation evidence and should be reviewed before sharing.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()

if(NOT DEFINED ANO_CRYPTO_EVIDENCE_DIR)
    set(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_BUILD_DIR}/evidence/p1-14")
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_CTEST)
    set(ANO_CRYPTO_RUN_CTEST OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_CREATE_SOURCE_ARCHIVE)
    set(ANO_CRYPTO_CREATE_SOURCE_ARCHIVE ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_INSTALL_SMOKE)
    set(ANO_CRYPTO_RUN_INSTALL_SMOKE OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK)
    set(ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK ON)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE)
    set(ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE OFF)
endif()

if(NOT DEFINED ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK)
    set(ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK ON)
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_EVIDENCE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}" ABSOLUTE)

if(NOT DEFINED ANO_CRYPTO_INSTALL_PREFIX)
    set(ANO_CRYPTO_INSTALL_PREFIX "${ANO_CRYPTO_EVIDENCE_DIR}/install-prefix")
endif()
if(NOT DEFINED ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR)
    set(ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR "${ANO_CRYPTO_EVIDENCE_DIR}/install-smoke-build")
endif()
if(NOT DEFINED ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR)
    set(ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}/tests/install_smoke")
endif()
if(NOT DEFINED ANO_CRYPTO_POC_SMOKE_DIR)
    set(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_EVIDENCE_DIR}/poc-package-smoke")
endif()

get_filename_component(ANO_CRYPTO_INSTALL_PREFIX "${ANO_CRYPTO_INSTALL_PREFIX}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_POC_SMOKE_DIR "${ANO_CRYPTO_POC_SMOKE_DIR}" ABSOLUTE)

file(MAKE_DIRECTORY "${ANO_CRYPTO_EVIDENCE_DIR}")

function(ano_crypto_run_logged log_file description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE _cmd_result
        OUTPUT_VARIABLE _cmd_stdout
        ERROR_VARIABLE _cmd_stderr)
    file(WRITE "${log_file}" "# ${description}\n")
    file(APPEND "${log_file}" "# Command:")
    foreach(_arg IN LISTS ARGN)
        file(APPEND "${log_file}" " ${_arg}")
    endforeach()
    file(APPEND "${log_file}" "\n# Result: ${_cmd_result}\n\n")
    file(APPEND "${log_file}" "${_cmd_stdout}${_cmd_stderr}")
    if(NOT _cmd_result EQUAL 0)
        message(FATAL_ERROR "${description} failed; see ${log_file}")
    endif()
endfunction()

string(TIMESTAMP ANO_CRYPTO_EVIDENCE_UTC "%Y-%m-%dT%H:%M:%SZ" UTC)

set(_git_commit "unknown")
set(_git_status "git unavailable or not a repository")
find_program(_git_exe git)
if(_git_exe)
    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" rev-parse --verify HEAD
        RESULT_VARIABLE _git_commit_result
        OUTPUT_VARIABLE _git_commit_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_git_commit_result EQUAL 0)
        set(_git_commit "${_git_commit_output}")
    endif()

    execute_process(
        COMMAND "${_git_exe}" -C "${ANO_CRYPTO_SOURCE_DIR}" status --short
        RESULT_VARIABLE _git_status_result
        OUTPUT_VARIABLE _git_status_output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(_git_status_result EQUAL 0)
        if(_git_status_output STREQUAL "")
            set(_git_status "clean")
        else()
            set(_git_status "${_git_status_output}")
        endif()
    endif()
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
"AnoCrypto-C Release Evidence Draft\n"
"Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
"Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
"Build directory: ${ANO_CRYPTO_BUILD_DIR}\n"
"Git commit: ${_git_commit}\n"
"Git status: ${_git_status}\n"
"CMake command: ${CMAKE_COMMAND}\n"
"CMake version: ${CMAKE_VERSION}\n"
"Reproducible build status: P1-14 draft procedure; binary byte identity not claimed\n"
"Evidence status: engineering draft only\n"
"Claims boundary: not official validation evidence\n")

file(GLOB_RECURSE _source_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
    "${ANO_CRYPTO_SOURCE_DIR}/*")
list(SORT _source_files)

set(_included_source_files "")
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "")
foreach(_rel IN LISTS _source_files)
    set(_skip OFF)
    if(_rel MATCHES "(^|/)(\\.git|build|build-[^/]*|cmake-build-[^/]*|\\.cache)(/|$)")
        set(_skip ON)
    endif()
    if(_rel MATCHES "^evidence/(generated|out|p1-[0-9][^/]*|release-[^/]*)/")
        set(_skip ON)
    endif()
    if(_rel MATCHES "\\.(zip|patch|tar|tgz|gz)$")
        set(_skip ON)
    endif()

    if(NOT _skip)
        list(APPEND _included_source_files "${_rel}")
        file(SHA256 "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_FILES.sha256" "${_hash}  ${_rel}\n")
    endif()
endforeach()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt" "")
foreach(_rel IN LISTS _included_source_files)
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE_CONTENTS.txt" "${_rel}\n")
endforeach()

if(ANO_CRYPTO_CREATE_SOURCE_ARCHIVE)
    set(_archive_root "${ANO_CRYPTO_EVIDENCE_DIR}/source-archive-work")
    set(_archive_tree "${_archive_root}/anocrypto-c")
    set(_archive_path "${ANO_CRYPTO_EVIDENCE_DIR}/anocrypto-c-source.tar.gz")
    file(REMOVE_RECURSE "${_archive_root}")
    file(MAKE_DIRECTORY "${_archive_tree}")

    foreach(_rel IN LISTS _included_source_files)
        get_filename_component(_rel_dir "${_rel}" DIRECTORY)
        if(_rel_dir STREQUAL "" OR _rel_dir STREQUAL ".")
            set(_dest_dir "${_archive_tree}")
        else()
            set(_dest_dir "${_archive_tree}/${_rel_dir}")
        endif()
        file(MAKE_DIRECTORY "${_dest_dir}")
        file(COPY "${ANO_CRYPTO_SOURCE_DIR}/${_rel}" DESTINATION "${_dest_dir}")
    endforeach()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar cfz "${_archive_path}" "anocrypto-c"
        WORKING_DIRECTORY "${_archive_root}"
        RESULT_VARIABLE _archive_result
        OUTPUT_VARIABLE _archive_stdout
        ERROR_VARIABLE _archive_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.log"
        "# Create source archive\n# Result: ${_archive_result}\n\n${_archive_stdout}${_archive_stderr}")
    if(NOT _archive_result EQUAL 0)
        message(FATAL_ERROR "source archive creation failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.log")
    endif()

    file(SHA256 "${_archive_path}" _archive_hash)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256"
        "${_archive_hash}  anocrypto-c-source.tar.gz\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt"
        "Source archive: anocrypto-c-source.tar.gz\n"
        "Source archive SHA-256: ${_archive_hash}\n")
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SOURCE_ARCHIVE.sha256"
        "Source archive creation was not requested. Set ANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/BINARY_FILES.sha256" "")
if(EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    file(GLOB_RECURSE _binary_files
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_BUILD_DIR}"
        "${ANO_CRYPTO_BUILD_DIR}/libanocrypto_c*"
        "${ANO_CRYPTO_BUILD_DIR}/anocrypto_c*"
        "${ANO_CRYPTO_BUILD_DIR}/ano_crypto_*")
    list(SORT _binary_files)
    foreach(_rel IN LISTS _binary_files)
        if(NOT _rel MATCHES "CMakeFiles/" AND NOT _rel MATCHES "^evidence/")
            file(SHA256 "${ANO_CRYPTO_BUILD_DIR}/${_rel}" _hash)
            file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BINARY_FILES.sha256" "${_hash}  ${_rel}\n")
        endif()
    endforeach()
endif()

if(ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK)
    set(_policy_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/PUBLIC_SURFACE_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_POLICY_CHECK_OUTPUT=${_policy_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_public_surface.cmake"
        RESULT_VARIABLE _policy_result
        OUTPUT_VARIABLE _policy_stdout
        ERROR_VARIABLE _policy_stderr)
    file(APPEND "${_policy_check_output}" "\nCommand output:\n${_policy_stdout}${_policy_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Public surface check result: ${_policy_result}\n")
    if(NOT _policy_result EQUAL 0)
        message(FATAL_ERROR "public surface check failed; see ${_policy_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/PUBLIC_SURFACE_CHECK.txt"
        "Public surface check was not requested. Set ANO_CRYPTO_RUN_PUBLIC_SURFACE_CHECK=ON.\n")
endif()

if(ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK)
    set(_thread_check_output "${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_SAFETY_CHECK.txt")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT=${_thread_check_output}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_thread_safety_policy.cmake"
        RESULT_VARIABLE _thread_check_result
        OUTPUT_VARIABLE _thread_check_stdout
        ERROR_VARIABLE _thread_check_stderr)
    file(APPEND "${_thread_check_output}" "\nCommand output:\n${_thread_check_stdout}${_thread_check_stderr}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Thread-safety policy check result: ${_thread_check_result}\n")
    if(NOT _thread_check_result EQUAL 0)
        message(FATAL_ERROR "thread-safety policy check failed; see ${_thread_check_output}")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/THREAD_SAFETY_CHECK.txt"
        "Thread-safety policy check was not requested. Set ANO_CRYPTO_RUN_THREAD_SAFETY_CHECK=ON.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/REPRODUCIBLE_BUILD_NOTES.txt"
    "AnoCrypto-C reproducible build evidence notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: P1-14 draft procedure; binary byte identity is not claimed.\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n"
    "Source archive content list: SOURCE_ARCHIVE_CONTENTS.txt\n"
    "Source hash list: SOURCE_FILES.sha256\n"
    "Binary hash list: BINARY_FILES.sha256\n"
    "Public surface check: PUBLIC_SURFACE_CHECK.txt\n"
    "Thread-safety policy check: THREAD_SAFETY_CHECK.txt\n"
    "TODO: normalize archive timestamps if byte-for-byte source archives become a release requirement.\n"
    "TODO: pin generator, compiler, locale, timezone, debug-info, and path-prefix-map policies for release-candidate builds.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SCOPE_REVIEW_NOTES.txt"
    "AnoCrypto-C P1-14 evidence and scope-review notes\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: engineering planning artifact; no new cryptographic algorithm implementation is added by P1-14.\n"
    "Development workflow: docs/roadmap/DEVELOPMENT_WORKFLOW.md\n"
    "Service scope matrix: docs/kcmvp/SERVICE_SCOPE_MATRIX.md\n"
    "Differential validation strategy: docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md\n"
    "Block cipher/mode selection gate: docs/design/BLOCK_CIPHER_MODE_SELECTION.md\n"
    "Side-channel review checklist: docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md\n"
    "Customer PoC intake questionnaire: docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md\n"
    "Current implemented algorithm slice remains SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256.\n"
    "Block cipher/mode implementation remains deferred until customer and validation-target candidate inputs are reviewed.\n"
    "Differential tests remain deferred until a reviewed public/reference source or tool is selected.\n"
    "SBOM/signing plan: docs/kcmvp/SBOM_SIGNING_PLAN.md\n"
    "Evidence manifest policy: docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md\n"
    "PoC package review manifest: docs/business/POC_PACKAGE_REVIEW_MANIFEST.md\n"
    "Thread-safety support matrix: docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md\n"
    "Thread-safety CI checklist: docs/ci/THREAD_SAFETY_CI_CHECKLIST.md\n"
    "Thread-safety evidence check: THREAD_SAFETY_CHECK.txt\n"
    "Do not include secrets, entropy bytes, generated secret bytes, private keys, tokens, customer data, or internal deployment details in scope evidence.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Scope review notes: SCOPE_REVIEW_NOTES.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SBOM_STATUS.txt"
    "AnoCrypto-C SBOM status\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: placeholder only; no complete SBOM is generated by P1-14.\n"
    "Hash manifests available: SOURCE_FILES.sha256, SOURCE_ARCHIVE.sha256, BINARY_FILES.sha256, EVIDENCE_MANIFEST.sha256.\n"
    "Candidate SBOM formats: SPDX JSON or CycloneDX JSON after tool and schema review.\n"
    "SBOM plan: docs/kcmvp/SBOM_SIGNING_PLAN.md\n"
    "Do not add secrets, private keys, tokens, entropy captures, generated secret bytes, customer data, or internal deployment details to SBOM inputs.\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "SBOM status: SBOM_STATUS.txt\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/SIGNING_STATUS.txt"
    "AnoCrypto-C signing status\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: signing not performed by P1-14 evidence collection.\n"
    "Private signing keys and signing tokens must not be stored in this repository, evidence directory, or PoC package.\n"
    "Candidate signed artifacts: source archive, source hash file, binary hash file, PoC package archive, SBOM, and evidence manifest after policy review.\n"
    "Preferred future approach: detached signatures with separately reviewed verification instructions.\n"
    "Signing plan: docs/kcmvp/SBOM_SIGNING_PLAN.md\n")
file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Signing status: SIGNING_STATUS.txt\n")

if(ANO_CRYPTO_RUN_CTEST)
    find_program(_ctest_exe ctest)
    if(NOT _ctest_exe)
        message(FATAL_ERROR "ctest executable not found")
    endif()
    if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}/CTestTestfile.cmake")
        message(FATAL_ERROR "CTestTestfile.cmake not found in ANO_CRYPTO_BUILD_DIR")
    endif()

    execute_process(
        COMMAND "${_ctest_exe}" --output-on-failure
        WORKING_DIRECTORY "${ANO_CRYPTO_BUILD_DIR}"
        RESULT_VARIABLE _ctest_result
        OUTPUT_VARIABLE _ctest_stdout
        ERROR_VARIABLE _ctest_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" "${_ctest_stdout}${_ctest_stderr}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "CTest result: ${_ctest_result}\n")
    if(NOT _ctest_result EQUAL 0)
        message(FATAL_ERROR "ctest failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/ctest.log" "CTest was not requested. Set ANO_CRYPTO_RUN_CTEST=ON to capture test output.\n")
endif()

if(ANO_CRYPTO_RUN_INSTALL_SMOKE)
    if(NOT EXISTS "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "install smoke source directory is missing CMakeLists.txt: ${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}")
    endif()

    file(REMOVE_RECURSE "${ANO_CRYPTO_INSTALL_PREFIX}" "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")
    file(MAKE_DIRECTORY "${ANO_CRYPTO_INSTALL_PREFIX}" "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_INSTALL.log"
        "Install anocrypto-c package prefix"
        "${CMAKE_COMMAND}" --install "${ANO_CRYPTO_BUILD_DIR}" --prefix "${ANO_CRYPTO_INSTALL_PREFIX}")

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_CONFIGURE.log"
        "Configure install smoke consumer"
        "${CMAKE_COMMAND}" -S "${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}" -B "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" -DCMAKE_PREFIX_PATH=${ANO_CRYPTO_INSTALL_PREFIX})

    ano_crypto_run_logged(
        "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_BUILD.log"
        "Build install smoke consumer"
        "${CMAKE_COMMAND}" --build "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}" --parallel)

    set(_smoke_exe "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke")
    if(NOT EXISTS "${_smoke_exe}")
        set(_smoke_exe "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke.exe")
    endif()
    if(NOT EXISTS "${_smoke_exe}")
        file(GLOB_RECURSE _smoke_candidates
            LIST_DIRECTORIES false
            "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}/anocrypto_c_install_smoke*")
        foreach(_candidate IN LISTS _smoke_candidates)
            if(NOT _candidate MATCHES "CMakeFiles")
                set(_smoke_exe "${_candidate}")
                break()
            endif()
        endforeach()
    endif()
    if(NOT EXISTS "${_smoke_exe}")
        message(FATAL_ERROR "install smoke executable not found under ${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}")
    endif()

    execute_process(
        COMMAND "${_smoke_exe}"
        WORKING_DIRECTORY "${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}"
        RESULT_VARIABLE _smoke_result
        OUTPUT_VARIABLE _smoke_stdout
        ERROR_VARIABLE _smoke_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "# Run install smoke consumer\n# Executable: ${_smoke_exe}\n# Result: ${_smoke_result}\n\n${_smoke_stdout}${_smoke_stderr}")
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_SUMMARY.txt"
        "Install prefix: ${ANO_CRYPTO_INSTALL_PREFIX}\n"
        "Install smoke source: ${ANO_CRYPTO_INSTALL_SMOKE_SOURCE_DIR}\n"
        "Install smoke build: ${ANO_CRYPTO_INSTALL_SMOKE_BUILD_DIR}\n"
        "Install smoke executable: ${_smoke_exe}\n"
        "Install smoke result: ${_smoke_result}\n")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Install smoke result: ${_smoke_result}\n")
    if(NOT _smoke_result EQUAL 0)
        message(FATAL_ERROR "install smoke failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/INSTALL_SMOKE_RUN.log"
        "Install smoke was not requested. Set ANO_CRYPTO_RUN_INSTALL_SMOKE=ON to capture installed-package smoke output.\n")
endif()

if(ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake")
        message(FATAL_ERROR "PoC package smoke script is missing: ${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake")
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
            -DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_BUILD_DIR}
            -DANO_CRYPTO_POC_SMOKE_DIR=${ANO_CRYPTO_POC_SMOKE_DIR}
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/run_poc_package_smoke.cmake"
        RESULT_VARIABLE _poc_smoke_result
        OUTPUT_VARIABLE _poc_smoke_stdout
        ERROR_VARIABLE _poc_smoke_stderr)
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log"
        "# Run customer PoC package smoke\n"
        "# Result: ${_poc_smoke_result}\n\n"
        "${_poc_smoke_stdout}${_poc_smoke_stderr}")
    file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "PoC package smoke result: ${_poc_smoke_result}\n")
    if(NOT _poc_smoke_result EQUAL 0)
        message(FATAL_ERROR "PoC package smoke failed; see ${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log")
    endif()
else()
    file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/POC_PACKAGE_SMOKE.log"
        "PoC package smoke was not requested. Set ANO_CRYPTO_RUN_POC_PACKAGE_SMOKE=ON to capture customer PoC package smoke output.\n")
endif()

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/README.md"
"# anocrypto-c Evidence Draft\n\n"
"Generated: ${ANO_CRYPTO_EVIDENCE_UTC}\n\n"
"This directory is an engineering evidence draft for the current local build.\n"
"It is not official validation evidence. Review path disclosure, source provenance,\n"
"test-vector provenance, compiler identity, binary provenance, and smoke logs before sharing.\n\n"
"## Files\n\n"
"- `BUILD_METADATA.txt`: local build and source metadata.\n"
"- `SOURCE_FILES.sha256`: SHA-256 hashes of repository source files included by the script.\n"
"- `SOURCE_ARCHIVE_CONTENTS.txt`: file list used for the local source archive.\n"
"- `SOURCE_ARCHIVE.sha256`: SHA-256 hash of `anocrypto-c-source.tar.gz` when archive creation is enabled.\n"
"- `BINARY_FILES.sha256`: SHA-256 hashes of build outputs matched by the script.\n"
"- `PUBLIC_SURFACE_CHECK.txt`: repository naming, claims-boundary, header, and package identity check.\n"
"- `THREAD_SAFETY_CHECK.txt`: support-matrix, CI-checklist, lock-branch, and build-option check.\n"
"- `REPRODUCIBLE_BUILD_NOTES.txt`: current reproducibility procedure notes and unresolved gaps.\n"
"- `ctest.log`: captured CTest output when requested.\n"
"- `INSTALL_SMOKE_*.log`: installed-package configure/build/run logs when requested.\n"
"- `INSTALL_SMOKE_SUMMARY.txt`: install smoke paths and result when requested.\n"
"- `POC_PACKAGE_SMOKE.log`: customer PoC package creation/configure/build/run summary when requested.\n"
"- `poc-package-smoke/`: detailed PoC package smoke logs and staged package when requested.\n"
"- `SBOM_STATUS.txt`: placeholder status for future SBOM generation.\n"
"- `SIGNING_STATUS.txt`: placeholder status for future detached-signature workflow.\n"
"- `EVIDENCE_MANIFEST_README.txt`: notes for the generated evidence manifest.\n"
"- `EVIDENCE_MANIFEST.sha256`: SHA-256 manifest of generated evidence files.\n"
"- Platform entropy provider status appears in `ctest.log` when the provider test output is shown by CTest.\n\n"
"## Review before release-candidate use\n\n"
"- Confirm source archive contents match the intended release scope.\n"
"- Confirm install smoke and PoC package smoke paths do not disclose internal deployment details before sharing.\n"
"- Confirm PoC package examples and manifests do not contain secrets, entropy bytes, generated secret bytes, private keys, tokens, or customer data.\n"
"- Confirm SBOM/signing placeholder status is accurate for this package.\n"
"- Confirm reproducibility gaps are marked as TODO instead of stronger claims.\n"
"- Confirm no secrets, entropy bytes, generated secret bytes, private keys, tokens, or internal deployment details are present.\n"
"- Confirm platform entropy evidence records provider/status only and does not contain entropy bytes.\n"
"- Confirm thread-safety evidence records only status, platform branch, and support-matrix notes.\n")

file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST_README.txt"
    "AnoCrypto-C evidence manifest\n"
    "Generated UTC: ${ANO_CRYPTO_EVIDENCE_UTC}\n"
    "Status: engineering hash manifest for local evidence review.\n"
    "Manifest file: EVIDENCE_MANIFEST.sha256\n"
    "Policy document: docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md\n"
    "The manifest excludes itself, the temporary source-archive-work tree, and transient smoke build directories. Review local paths and logs before sharing.\n")

file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/BUILD_METADATA.txt" "Evidence manifest: EVIDENCE_MANIFEST.sha256\n")

file(GLOB_RECURSE _evidence_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_EVIDENCE_DIR}"
    "${ANO_CRYPTO_EVIDENCE_DIR}/*")
list(SORT _evidence_files)
file(WRITE "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "")
foreach(_rel IN LISTS _evidence_files)
    set(_manifest_skip OFF)
    if(_rel STREQUAL "EVIDENCE_MANIFEST.sha256")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^source-archive-work/")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^install-smoke-build/")
        set(_manifest_skip ON)
    endif()
    if(_rel MATCHES "^poc-package-smoke/core-poc-build/")
        set(_manifest_skip ON)
    endif()
    if(NOT _manifest_skip)
        file(SHA256 "${ANO_CRYPTO_EVIDENCE_DIR}/${_rel}" _evidence_hash)
        file(APPEND "${ANO_CRYPTO_EVIDENCE_DIR}/EVIDENCE_MANIFEST.sha256" "${_evidence_hash}  ${_rel}\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C evidence draft written to ${ANO_CRYPTO_EVIDENCE_DIR}")
