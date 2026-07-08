# AnoCrypto-C customer PoC package v0 staging script.
# This creates a local engineering package draft from a configured build tree.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_POC_PACKAGE_DIR)
    set(ANO_CRYPTO_POC_PACKAGE_DIR "${ANO_CRYPTO_BUILD_DIR}/poc-package/AnoCrypto-C-PoC-v0")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_POC_PACKAGE_DIR "${ANO_CRYPTO_POC_PACKAGE_DIR}" ABSOLUTE)

if(NOT EXISTS "${ANO_CRYPTO_BUILD_DIR}")
    message(FATAL_ERROR "Build directory does not exist: ${ANO_CRYPTO_BUILD_DIR}")
endif()

set(_sdk_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/sdk")
set(_docs_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/docs")
set(_examples_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/examples")
set(_evidence_dir "${ANO_CRYPTO_POC_PACKAGE_DIR}/evidence-template")

file(REMOVE_RECURSE "${ANO_CRYPTO_POC_PACKAGE_DIR}")
file(MAKE_DIRECTORY "${_sdk_dir}" "${_docs_dir}" "${_examples_dir}" "${_evidence_dir}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${ANO_CRYPTO_BUILD_DIR}" --prefix "${_sdk_dir}"
    RESULT_VARIABLE _install_result
    OUTPUT_VARIABLE _install_stdout
    ERROR_VARIABLE _install_stderr)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log"
    "# Install AnoCrypto-C SDK into PoC package\n"
    "# Result: ${_install_result}\n\n"
    "${_install_stdout}${_install_stderr}")
if(NOT _install_result EQUAL 0)
    message(FATAL_ERROR "SDK install into PoC package failed; see ${ANO_CRYPTO_POC_PACKAGE_DIR}/INSTALL_PACKAGE.log")
endif()

set(_doc_files
    README.md
    LICENSE
    docs/business/POC_CUSTOMER_PACKAGE_V0.md
    docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md
    docs/integration/SDK_INTEGRATION_GUIDE.md
    docs/kcmvp/KCMVP_ALGORITHM_SCOPE.md
    docs/kcmvp/SERVICE_SCOPE_MATRIX.md
    docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md
    docs/kcmvp/RELEASE_EVIDENCE_WORKFLOW.md
    docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md
    docs/design/API_ABI_REVIEW_CHECKLIST.md
    docs/design/BLOCK_CIPHER_MODE_SELECTION.md
    docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md
    docs/ci/CI_MATRIX.md
    docs/roadmap/DEVELOPMENT_WORKFLOW.md
    docs/roadmap/C99_PRIORITY_TODO.md)

foreach(_doc IN LISTS _doc_files)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_doc}")
        set(_dest_rel "${_doc}")
        if(_dest_rel MATCHES "^docs/(.*)$")
            set(_dest_rel "${CMAKE_MATCH_1}")
        endif()
        get_filename_component(_doc_dir "${_dest_rel}" DIRECTORY)
        if(_doc_dir STREQUAL "" OR _doc_dir STREQUAL ".")
            set(_dest_dir "${_docs_dir}")
        else()
            set(_dest_dir "${_docs_dir}/${_doc_dir}")
        endif()
        file(MAKE_DIRECTORY "${_dest_dir}")
        file(COPY "${ANO_CRYPTO_SOURCE_DIR}/${_doc}" DESTINATION "${_dest_dir}")
    endif()
endforeach()

file(COPY "${ANO_CRYPTO_SOURCE_DIR}/examples/core_poc" DESTINATION "${_examples_dir}")
file(COPY "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/README.md" DESTINATION "${_evidence_dir}")

string(TIMESTAMP _created_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/README.md"
"# AnoCrypto-C PoC Package v0\n\n"
"Generated UTC: ${_created_utc}\n\n"
"This directory is a customer PoC package draft for AnoCrypto-C. It is intended to check C ABI integration, installed CMake package lookup, module lifecycle, self-test startup behavior, and a limited SHA-256/HMAC-SHA-256/HMAC_DRBG-SHA-256 smoke flow.\n\n"
"## Contents\n\n"
"- `sdk/`: installed headers, library, and `AnoCryptoC` CMake package files.\n"
"- `examples/core_poc/`: standalone public-API integration example.\n"
"- `docs/`: selected design, integration, evidence, and roadmap notes.\n"
"- `evidence-template/`: release evidence template notes for customer review workflows.\n\n"
"## Build the PoC example\n\n"
"```sh\n"
"cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=$PWD/sdk\n"
"cmake --build build-core-poc\n"
"./build-core-poc/anocrypto_c_core_poc\n"
"```\n\n"
"## Boundary\n\n"
"This package is not official validation evidence. Review local paths, compiler identifiers, binary provenance, source provenance, and logs before sharing externally. Do not add secrets, entropy captures, generated secret bytes, private keys, tokens, or customer deployment details to this package.\n")

file(GLOB_RECURSE _package_files
    LIST_DIRECTORIES false
    RELATIVE "${ANO_CRYPTO_POC_PACKAGE_DIR}"
    "${ANO_CRYPTO_POC_PACKAGE_DIR}/*")
list(SORT _package_files)

file(WRITE "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md"
"# AnoCrypto-C PoC Package v0 Manifest\n\n"
"Generated UTC: ${_created_utc}\n\n"
"Status: engineering package draft.\n\n"
"## Files\n\n")
foreach(_rel IN LISTS _package_files)
    if(NOT _rel STREQUAL "PACKAGE_MANIFEST.md")
        file(SHA256 "${ANO_CRYPTO_POC_PACKAGE_DIR}/${_rel}" _hash)
        file(APPEND "${ANO_CRYPTO_POC_PACKAGE_DIR}/PACKAGE_MANIFEST.md" "- `${_rel}` — SHA-256 `${_hash}`\n")
    endif()
endforeach()

message(STATUS "AnoCrypto-C PoC package v0 written to ${ANO_CRYPTO_POC_PACKAGE_DIR}")
