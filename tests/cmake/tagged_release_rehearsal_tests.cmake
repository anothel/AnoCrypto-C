cmake_minimum_required(VERSION 3.16)

foreach(_required ANO_CRYPTO_SOURCE_DIR ANO_CRYPTO_TEST_WORK_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "${_required} is required")
    endif()
endforeach()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_TEST_WORK_DIR "${ANO_CRYPTO_TEST_WORK_DIR}" ABSOLUTE)
if(ANO_CRYPTO_TEST_WORK_DIR STREQUAL ANO_CRYPTO_SOURCE_DIR)
    message(FATAL_ERROR "Tagged rehearsal work directory must differ from the source directory")
endif()

find_program(_git_executable NAMES git)
if(NOT _git_executable)
    message(FATAL_ERROR "Tagged release rehearsal tests require Git")
endif()

file(REMOVE_RECURSE "${ANO_CRYPTO_TEST_WORK_DIR}")
file(MAKE_DIRECTORY "${ANO_CRYPTO_TEST_WORK_DIR}/checks")
set(_fixture_repo "${ANO_CRYPTO_TEST_WORK_DIR}/isolated-repository")

function(_git_capture _repo _result_var _output_var _error_var)
    execute_process(
        COMMAND "${_git_executable}" -C "${_repo}" ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        TIMEOUT 120)
    set(${_result_var} "${_result}" PARENT_SCOPE)
    set(${_output_var} "${_output}" PARENT_SCOPE)
    set(${_error_var} "${_error}" PARENT_SCOPE)
endfunction()

function(_git_require _repo _description)
    _git_capture("${_repo}" _result _output _error ${ARGN})
    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed: ${_output}${_error}")
    endif()
endfunction()

function(_check_expect _expected_success _name _description)
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            "-DANO_CRYPTO_SOURCE_DIR=${_fixture_repo}"
            "-DANO_CRYPTO_BUILD_DIR=${ANO_CRYPTO_TEST_WORK_DIR}/driver-build"
            "-DANO_CRYPTO_ACTUAL_RELEASE_TAG=${_expected_tag}"
            "-DANO_CRYPTO_ACTUAL_RELEASE_COMMIT=${_candidate_commit}"
            -DANO_CRYPTO_REQUIRE_ACTUAL_RELEASE_TAG=ON
            "-DANO_CRYPTO_RELEASE_COMPATIBILITY_CHECK_OUTPUT=${ANO_CRYPTO_TEST_WORK_DIR}/checks/${_name}.txt"
            -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_release_compatibility.cmake"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
        TIMEOUT 300)
    if(_expected_success AND NOT _result EQUAL 0)
        message(FATAL_ERROR "${_description} failed unexpectedly:\n${_output}${_error}")
    elseif(NOT _expected_success AND _result EQUAL 0)
        message(FATAL_ERROR "${_description} unexpectedly succeeded")
    endif()
endfunction()

function(_tag_state _repo _result_var _state_var)
    _git_capture("${_repo}" _result _output _error
        rev-parse --verify --quiet "refs/tags/${_expected_tag}^{commit}")
    if(_result EQUAL 0)
        set(_state "PRESENT:${_output}")
    elseif(_result EQUAL 1)
        set(_state "ABSENT")
    else()
        message(FATAL_ERROR "Expected-tag state query failed: ${_error}")
    endif()
    set(${_result_var} "${_result}" PARENT_SCOPE)
    set(${_state_var} "${_state}" PARENT_SCOPE)
endfunction()

# A real repository is used only as a read-only clone source. An extracted ZIP
# gets a minimal independent fixture repository and is never reported as an
# actual clean-checkout rehearsal.
_git_capture("${ANO_CRYPTO_SOURCE_DIR}" _inside_result _inside_output _inside_error
    rev-parse --is-inside-work-tree)
set(_source_is_repository FALSE)
if(_inside_result EQUAL 0 AND _inside_output STREQUAL "true")
    _git_capture("${ANO_CRYPTO_SOURCE_DIR}" _root_result _source_git_root _root_error
        rev-parse --show-toplevel)
    if(NOT _root_result EQUAL 0)
        message(FATAL_ERROR "Source Git root query failed: ${_root_error}")
    endif()
    get_filename_component(_source_real "${ANO_CRYPTO_SOURCE_DIR}" REALPATH)
    get_filename_component(_root_real "${_source_git_root}" REALPATH)
    if(WIN32)
        string(TOLOWER "${_source_real}" _source_real)
        string(TOLOWER "${_root_real}" _root_real)
    endif()
    if(_source_real STREQUAL _root_real)
        set(_source_is_repository TRUE)
    endif()
elseif(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/.git")
    message(FATAL_ERROR "Source contains Git metadata but work-tree verification failed: ${_inside_error}")
endif()

if(_source_is_repository)
    _git_capture("${ANO_CRYPTO_SOURCE_DIR}" _head_result _source_head_before _head_error
        rev-parse --verify "HEAD^{commit}")
    if(NOT _head_result EQUAL 0 OR _source_head_before STREQUAL "")
        message(FATAL_ERROR "Source HEAD query failed: ${_head_error}")
    endif()
    _git_capture("${ANO_CRYPTO_SOURCE_DIR}" _status_result _source_status_before _status_error
        status --porcelain=v1 --untracked-files=all)
    if(NOT _status_result EQUAL 0)
        message(FATAL_ERROR "Source status query failed: ${_status_error}")
    endif()

    execute_process(
        COMMAND "${_git_executable}" clone --no-local --no-hardlinks --no-checkout
            "${ANO_CRYPTO_SOURCE_DIR}" "${_fixture_repo}"
        RESULT_VARIABLE _clone_result
        OUTPUT_VARIABLE _clone_output
        ERROR_VARIABLE _clone_error
        TIMEOUT 300)
    if(NOT _clone_result EQUAL 0)
        message(FATAL_ERROR "Independent rehearsal clone failed: ${_clone_output}${_clone_error}")
    endif()
    _git_require("${_fixture_repo}" "Detached rehearsal checkout"
        checkout --detach "${_source_head_before}")
    set(_source_commit "${_source_head_before}")
    if(_source_status_before STREQUAL "")
        set(_mode "CLEAN_GIT_CLONE")
        set(_source_worktree_state "CLEAN")
        set(_release_rehearsal_status "PASS")
    else()
        set(_mode "COMMITTED_HEAD_FIXTURE")
        set(_source_worktree_state "DIRTY")
        set(_release_rehearsal_status "NOT_RUN_SOURCE_DIRTY")
    endif()
else()
    set(_fixture_files
        .github/workflows/ci.yml
        CMakeLists.txt
        README.md
        src/core/build_identity.c
        tests/cmake/hosted_artifact_manifest_tests.cmake
        tools/create_poc_package.cmake)
    file(GLOB_RECURSE _fixture_tree_files
        LIST_DIRECTORIES false
        RELATIVE "${ANO_CRYPTO_SOURCE_DIR}"
        "${ANO_CRYPTO_SOURCE_DIR}/abi/*"
        "${ANO_CRYPTO_SOURCE_DIR}/docs/*"
        "${ANO_CRYPTO_SOURCE_DIR}/evidence/release-template/*"
        "${ANO_CRYPTO_SOURCE_DIR}/include/*"
        "${ANO_CRYPTO_SOURCE_DIR}/release/*"
        "${ANO_CRYPTO_SOURCE_DIR}/tests/compat/*")
    list(APPEND _fixture_files ${_fixture_tree_files})
    list(REMOVE_DUPLICATES _fixture_files)
    foreach(_relative IN LISTS _fixture_files)
        if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" OR
           IS_DIRECTORY "${ANO_CRYPTO_SOURCE_DIR}/${_relative}" OR
           IS_SYMLINK "${ANO_CRYPTO_SOURCE_DIR}/${_relative}")
            message(FATAL_ERROR "Archive rehearsal fixture input is missing or unsafe: ${_relative}")
        endif()
        get_filename_component(_relative_dir "${_relative}" DIRECTORY)
        file(MAKE_DIRECTORY "${_fixture_repo}/${_relative_dir}")
        configure_file("${ANO_CRYPTO_SOURCE_DIR}/${_relative}"
            "${_fixture_repo}/${_relative}" COPYONLY)
    endforeach()
    _git_require("${_fixture_repo}" "Archive fixture Git initialization" init)
    _git_require("${_fixture_repo}" "Archive fixture staging" add --all)
    _git_require("${_fixture_repo}" "Archive fixture commit"
        -c user.name=AnoCrypto-Rehearsal
        -c user.email=rehearsal.invalid
        commit --no-gpg-sign -m "synthetic archive rehearsal fixture")
    _git_capture("${_fixture_repo}" _head_result _fixture_head _head_error
        rev-parse --verify "HEAD^{commit}")
    if(NOT _head_result EQUAL 0 OR _fixture_head STREQUAL "")
        message(FATAL_ERROR "Archive fixture HEAD query failed: ${_head_error}")
    endif()
    set(_source_commit "NOT_AVAILABLE")
    set(_source_head_before "NOT_AVAILABLE")
    set(_source_status_before "NOT_AVAILABLE")
    set(_source_worktree_state "NOT_A_GIT_WORKTREE")
    set(_mode "SYNTHETIC_ARCHIVE_ONLY")
    set(_release_rehearsal_status "SYNTHETIC_ARCHIVE_ONLY")
endif()

include("${_fixture_repo}/release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake")
set(_expected_tag "${ANO_CRYPTO_RELEASE_TAG}")
set(_release_version "${ANO_CRYPTO_RELEASE_VERSION}")

if(_source_is_repository)
    _tag_state("${ANO_CRYPTO_SOURCE_DIR}" _source_tag_result_before _source_tag_before)
    if(NOT _source_tag_before STREQUAL "ABSENT")
        message(FATAL_ERROR
            "P1-33 rehearsal requires the expected tag to be absent from the source repository")
    endif()
else()
    set(_source_tag_before "NOT_APPLICABLE")
endif()

_git_capture("${_fixture_repo}" _candidate_result _candidate_commit _candidate_error
    rev-parse --verify "HEAD^{commit}")
if(NOT _candidate_result EQUAL 0 OR _candidate_commit STREQUAL "")
    message(FATAL_ERROR "Rehearsal candidate query failed: ${_candidate_error}")
endif()
_git_require("${_fixture_repo}" "Synthetic local tag creation"
    update-ref "refs/tags/${_expected_tag}" "${_candidate_commit}")
_git_capture("${_fixture_repo}" _tag_type_result _tag_type _tag_type_error
    cat-file -t "refs/tags/${_expected_tag}")
if(NOT _tag_type_result EQUAL 0 OR NOT _tag_type STREQUAL "commit")
    message(FATAL_ERROR "Synthetic rehearsal tag is not an unsigned lightweight tag: ${_tag_type_error}")
endif()

_check_expect(TRUE positive "Synthetic local tag/commit/HEAD identity")

file(WRITE "${_fixture_repo}/rehearsal-dirty.tmp" "dirty fixture\n")
_check_expect(FALSE dirty "Dirty worktree rejection")
file(REMOVE "${_fixture_repo}/rehearsal-dirty.tmp")

_git_require("${_fixture_repo}" "Synthetic moved-HEAD commit"
    -c user.name=AnoCrypto-Rehearsal
    -c user.email=rehearsal.invalid
    commit --allow-empty --no-gpg-sign -m "synthetic moved HEAD fixture")
_git_capture("${_fixture_repo}" _moved_result _moved_commit _moved_error
    rev-parse --verify "HEAD^{commit}")
if(NOT _moved_result EQUAL 0 OR _moved_commit STREQUAL "" OR
   _moved_commit STREQUAL _candidate_commit)
    message(FATAL_ERROR "Synthetic moved HEAD was not created: ${_moved_error}")
endif()
_check_expect(FALSE moved-head "Moved HEAD rejection")
_git_require("${_fixture_repo}" "Restore rehearsal HEAD"
    reset --hard "${_candidate_commit}")

_git_require("${_fixture_repo}" "Synthetic moved-tag update"
    update-ref "refs/tags/${_expected_tag}" "${_moved_commit}")
_check_expect(FALSE moved-tag "Moved tag rejection")
_git_require("${_fixture_repo}" "Restore synthetic local tag"
    update-ref "refs/tags/${_expected_tag}" "${_candidate_commit}")
_check_expect(TRUE restored "Restored tag/commit/HEAD identity")

_git_capture("${_fixture_repo}" _final_status_result _final_status _final_status_error
    status --porcelain=v1 --untracked-files=all)
if(NOT _final_status_result EQUAL 0 OR NOT _final_status STREQUAL "")
    message(FATAL_ERROR "Rehearsal fixture did not finish clean: ${_final_status}${_final_status_error}")
endif()
_git_capture("${_fixture_repo}" _final_head_result _final_head _final_head_error
    rev-parse --verify "HEAD^{commit}")
_git_capture("${_fixture_repo}" _final_tag_result _final_tag _final_tag_error
    rev-parse --verify "refs/tags/${_expected_tag}^{commit}")
if(NOT _final_head_result EQUAL 0 OR NOT _final_tag_result EQUAL 0 OR
   NOT _final_head STREQUAL _candidate_commit OR
   NOT _final_tag STREQUAL _candidate_commit)
    message(FATAL_ERROR
        "Final rehearsal identity is inconsistent: ${_final_head_error}${_final_tag_error}")
endif()

if(_source_is_repository)
    _git_capture("${ANO_CRYPTO_SOURCE_DIR}" _source_head_result_after _source_head_after _source_head_error_after
        rev-parse --verify "HEAD^{commit}")
    _git_capture("${ANO_CRYPTO_SOURCE_DIR}" _source_status_result_after _source_status_after _source_status_error_after
        status --porcelain=v1 --untracked-files=all)
    _tag_state("${ANO_CRYPTO_SOURCE_DIR}" _source_tag_result_after _source_tag_after)
    if(NOT _source_head_result_after EQUAL 0 OR
       NOT _source_status_result_after EQUAL 0 OR
       NOT _source_head_after STREQUAL _source_head_before OR
       NOT _source_status_after STREQUAL _source_status_before OR
       NOT _source_tag_result_after EQUAL _source_tag_result_before OR
       NOT _source_tag_after STREQUAL _source_tag_before OR
       NOT _source_tag_after STREQUAL "ABSENT")
        message(FATAL_ERROR
            "Source repository changed during rehearsal: ${_source_head_error_after}${_source_status_error_after}")
    endif()
    set(_source_repository_unchanged "YES")
else()
    set(_source_tag_after "NOT_APPLICABLE")
    set(_source_repository_unchanged "NOT_APPLICABLE")
endif()

file(REMOVE_RECURSE "${_fixture_repo}")
if(EXISTS "${_fixture_repo}")
    message(FATAL_ERROR "Isolated rehearsal repository cleanup failed")
endif()

file(WRITE "${ANO_CRYPTO_TEST_WORK_DIR}/TAGGED_RELEASE_REHEARSAL.txt"
    "ANOCRYPTO_TAGGED_RELEASE_REHEARSAL_V1\n"
    "release_version=${_release_version}\n"
    "expected_tag=${_expected_tag}\n"
    "mode=${_mode}\n"
    "source_commit=${_source_commit}\n"
    "rehearsal_commit=${_candidate_commit}\n"
    "source_worktree_state=${_source_worktree_state}\n"
    "source_expected_tag_before=${_source_tag_before}\n"
    "source_expected_tag_after=${_source_tag_after}\n"
    "source_repository_unchanged=${_source_repository_unchanged}\n"
    "synthetic_tag_scope=ISOLATED_REPOSITORY_ONLY\n"
    "tag_commit_head_identity=PASS\n"
    "dirty_worktree_rejection=PASS\n"
    "moved_tag_rejection=PASS\n"
    "moved_head_rejection=PASS\n"
    "user_repository_tag_created=NO\n"
    "tag_signing=NOT_RUN\n"
    "actual_signature=NOT_CREATED\n"
    "timestamp_response=NOT_CREATED\n"
    "hosted_runner=NOT_RUN\n"
    "hosted_artifact_review=UNREVIEWED\n"
    "github_attestation=NOT_RUN_SECONDARY_ONLY\n"
    "repository_signing_status=UNSIGNED\n"
    "signing_handoff=BLOCKED\n"
    "external_release_authorized=NO\n"
    "release_rehearsal_status=${_release_rehearsal_status}\n"
    "result=PASS\n")

message(STATUS
    "Tagged release rehearsal regressions passed (${_mode}, ${_candidate_commit})")
