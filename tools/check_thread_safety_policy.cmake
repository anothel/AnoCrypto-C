# AnoCrypto-C thread-safety policy and support-matrix check.
# This is an engineering guardrail, not formal validation evidence.

cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
    get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_BUILD_DIR)
    set(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_SOURCE_DIR}/build")
endif()
if(NOT DEFINED ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT)
    set(ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT "${ANO_CRYPTO_BUILD_DIR}/policy-check/THREAD_SAFETY_CHECK.txt")
endif()

get_filename_component(ANO_CRYPTO_SOURCE_DIR "${ANO_CRYPTO_SOURCE_DIR}" ABSOLUTE)
get_filename_component(ANO_CRYPTO_BUILD_DIR "${ANO_CRYPTO_BUILD_DIR}" ABSOLUTE)
get_filename_component(_out_dir "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

set(_violations "")
set(_warnings "")
set(_notes "")

function(ano_crypto_require_file rel_path description)
    if(NOT EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        list(APPEND _violations "missing ${description}: ${rel_path}")
        set(_violations "${_violations}" PARENT_SCOPE)
    endif()
endfunction()

function(ano_crypto_require_text rel_path needle description)
    if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}")
        file(READ "${ANO_CRYPTO_SOURCE_DIR}/${rel_path}" _content)
        string(FIND "${_content}" "${needle}" _pos)
        if(_pos EQUAL -1)
            list(APPEND _violations "missing ${description} in ${rel_path}")
            set(_violations "${_violations}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

ano_crypto_require_file("docs/design/C99_THREAD_SAFETY_POLICY.md" "thread-safety policy document")
ano_crypto_require_file("docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md" "thread-safety support matrix")
ano_crypto_require_file("docs/ci/THREAD_SAFETY_CI_CHECKLIST.md" "thread-safety CI checklist")
ano_crypto_require_file("src/internal/ac_thread_internal.h" "internal lock header")
ano_crypto_require_file("src/core/thread_lock.c" "internal lock implementation")
ano_crypto_require_file("tests/unit/thread_lock_tests.c" "internal lock smoke test")
ano_crypto_require_file("tests/unit/csp_thread_safety_tests.c" "CSP/HMAC concurrent smoke test")

ano_crypto_require_text("CMakeLists.txt" "ANO_CRYPTO_ENABLE_THREAD_SAFETY" "thread-safety build option")
ano_crypto_require_text("CMakeLists.txt" "ano_crypto_thread_safety_policy_check" "thread-safety policy check target")
ano_crypto_require_text("CMakeLists.txt" "ANO_CRYPTO_ENABLE_THREAD_SAFETY AND CMAKE_USE_PTHREADS_INIT" "guard for POSIX concurrent smoke test")
ano_crypto_require_text("docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md" "caller-provided context" "caller-owned context wording")
ano_crypto_require_text("docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md" "snapshot rule" "HMAC key-handle snapshot wording")
ano_crypto_require_text("docs/ci/THREAD_SAFETY_CI_CHECKLIST.md" "ANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF" "explicit OFF-lane documentation")
ano_crypto_require_text("tools/collect_release_evidence.cmake" "THREAD_SAFETY_CHECK.txt" "thread-safety evidence output")
ano_crypto_require_text("tools/create_poc_package.cmake" "THREAD_SAFETY_PACKAGE_NOTES.txt" "thread-safety PoC package note")

if(EXISTS "${ANO_CRYPTO_SOURCE_DIR}/src/core/thread_lock.c")
    file(READ "${ANO_CRYPTO_SOURCE_DIR}/src/core/thread_lock.c" _lock_impl)
    string(FIND "${_lock_impl}" "pthread_mutex_t" _pthread_pos)
    string(FIND "${_lock_impl}" "CRITICAL_SECTION" _win_pos)
    string(FIND "${_lock_impl}" "Unsupported platform fallback" _fallback_pos)
    if(_pthread_pos EQUAL -1)
        list(APPEND _violations "pthread lock branch was not found in src/core/thread_lock.c")
    endif()
    if(_win_pos EQUAL -1)
        list(APPEND _violations "Windows lock branch was not found in src/core/thread_lock.c")
    endif()
    if(_fallback_pos EQUAL -1)
        list(APPEND _violations "unsupported-platform fallback note was not found in src/core/thread_lock.c")
    endif()
endif()

set(_cache_path "${ANO_CRYPTO_BUILD_DIR}/CMakeCache.txt")
if(EXISTS "${_cache_path}")
    file(READ "${_cache_path}" _cache_content)
    set(_thread_safety_enabled OFF)
    if(_cache_content MATCHES "ANO_CRYPTO_ENABLE_THREAD_SAFETY:BOOL=ON")
        set(_thread_safety_enabled ON)
        list(APPEND _notes "configured thread-safety option: ON")
    elseif(_cache_content MATCHES "ANO_CRYPTO_ENABLE_THREAD_SAFETY:BOOL=OFF")
        list(APPEND _warnings "configured thread-safety option is OFF - caller serialization is required for shared state")
    else()
        list(APPEND _warnings "thread-safety option was not found in CMakeCache.txt")
    endif()

    if(_thread_safety_enabled)
        if(_cache_content MATCHES "CMAKE_USE_PTHREADS_INIT:INTERNAL=1" OR
           _cache_content MATCHES "CMAKE_HAVE_LIBC_PTHREAD:INTERNAL=1")
            list(APPEND _notes "CMake reported pthread or libc-pthread support")
        elseif(_cache_content MATCHES "CMAKE_USE_PTHREADS_INIT:INTERNAL=" OR
               _cache_content MATCHES "CMAKE_HAVE_LIBC_PTHREAD:INTERNAL=")
            list(APPEND _warnings "CMake did not report pthread support for this build tree")
        else()
            list(APPEND _warnings "pthread support cache entry was not found - platform branch may be Windows or unreviewed")
        endif()
    endif()
else()
    list(APPEND _warnings "CMakeCache.txt not found - build-configuration check skipped")
endif()

string(TIMESTAMP _checked_utc "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}"
    "AnoCrypto-C thread-safety policy check\n"
    "Checked UTC: ${_checked_utc}\n"
    "Source directory: ${ANO_CRYPTO_SOURCE_DIR}\n"
    "Build directory: ${ANO_CRYPTO_BUILD_DIR}\n\n"
    "Checks:\n"
    "- Thread-safety policy document presence\n"
    "- Public support matrix presence\n"
    "- CI checklist presence\n"
    "- Build option and CMake target presence\n"
    "- Internal lock source/header presence\n"
    "- POSIX and Windows lock branch markers\n"
    "- Concurrent CSP/HMAC smoke test guard\n"
    "- Evidence and PoC package thread-safety records\n\n")

if(_notes)
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Notes:\n")
    foreach(_note IN LISTS _notes)
        file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "- ${_note}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "\n")
else()
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Notes: none\n\n")
endif()

if(_warnings)
    list(LENGTH _warnings _warning_count)
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Warnings / tracked TODOs:\n")
    foreach(_warning IN LISTS _warnings)
        file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "- ${_warning}\n")
    endforeach()
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "\n")
    message(WARNING "AnoCrypto-C thread-safety policy check has ${_warning_count} warning/TODO item(s); see ${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}")
else()
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Warnings / tracked TODOs: none\n\n")
endif()

if(_violations)
    file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Result: FAIL\n\nViolations:\n")
    foreach(_violation IN LISTS _violations)
        file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "- ${_violation}\n")
    endforeach()
    message(FATAL_ERROR "AnoCrypto-C thread-safety policy check failed; see ${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}")
endif()

file(APPEND "${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}" "Result: PASS\n")
message(STATUS "AnoCrypto-C thread-safety policy check passed: ${ANO_CRYPTO_THREAD_SAFETY_CHECK_OUTPUT}")
