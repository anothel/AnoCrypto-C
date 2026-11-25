cmake_minimum_required(VERSION 3.19)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "The minimal consumer smoke runner requires a Linux host.")
endif()

function(validate_path variable)
    # Keep paths literal in CMake lists, Makefiles, and the ELF loader's path list.
    if(NOT "${${variable}}" MATCHES "^/[a-zA-Z0-9_./ +@=-]+$"
       OR "${${variable}}" MATCHES "//|/\\.\\.?(/|$)")
        message(FATAL_ERROR "${variable} must be a safe absolute path.")
    endif()
    string(REGEX REPLACE "/$" "" path "${${variable}}")
    set("${variable}" "${path}" PARENT_SCOPE)
endfunction()

function(is_within path directory output)
    string(FIND "${path}/" "${directory}/" position)
    if(position EQUAL 0)
        set("${output}" TRUE PARENT_SCOPE)
    else()
        set("${output}" FALSE PARENT_SCOPE)
    endif()
endfunction()

function(run_checked label timeout)
    set(command_line "")
    foreach(argument IN LISTS ARGN)
        string(REPLACE "'" "'\\''" quoted_argument "${argument}")
        string(APPEND command_line " '${quoted_argument}'")
    endforeach()
    message(STATUS "${label} command:${command_line}")
    execute_process(COMMAND ${ARGN}
        WORKING_DIRECTORY "${working_directory}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_QUIET
        TIMEOUT "${timeout}")
    # Child output can contain toolchain or environment data. Report status only.
    if(result MATCHES "^-?[0-9]+$")
        message(STATUS "${label} exit ${result}")
    elseif(result MATCHES "timeout")
        message(FATAL_ERROR "${label} timed out after ${timeout} seconds.")
    else()
        message(FATAL_ERROR "${label} did not return an exit code.")
    endif()
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "${label} failed.")
    endif()
    set(command_output "${output}" PARENT_SCOPE)
endfunction()

foreach(variable IN ITEMS SDK_PREFIX SDK_CONFIG_DIR SCRATCH_DIR)
    validate_path("${variable}")
endforeach()

foreach(variable IN ITEMS SDK_PREFIX SDK_CONFIG_DIR)
    if(NOT IS_DIRECTORY "${${variable}}")
        message(FATAL_ERROR "${variable} must be an existing directory.")
    endif()
    get_filename_component(physical_path "${${variable}}" REALPATH)
    if(NOT physical_path STREQUAL "${${variable}}")
        message(FATAL_ERROR "${variable} must use a physical path without symlinks.")
    endif()
endforeach()

is_within("${SDK_CONFIG_DIR}" "${SDK_PREFIX}" config_in_prefix)
set(expected_config "${SDK_CONFIG_DIR}/AnoCryptoCConfig.cmake")
if(NOT config_in_prefix OR NOT EXISTS "${expected_config}"
   OR IS_DIRECTORY "${expected_config}" OR IS_SYMLINK "${expected_config}")
    message(FATAL_ERROR "SDK_CONFIG_DIR must contain the installed AnoCryptoCConfig.cmake inside SDK_PREFIX.")
endif()

get_filename_component(source_root "${CMAKE_CURRENT_LIST_DIR}/.." REALPATH)
get_filename_component(scratch_parent "${SCRATCH_DIR}" DIRECTORY)
get_filename_component(physical_parent "${scratch_parent}" REALPATH)
if(EXISTS "${SCRATCH_DIR}" OR IS_SYMLINK "${SCRATCH_DIR}")
    message(FATAL_ERROR "SCRATCH_DIR already exists.")
endif()
if(NOT IS_DIRECTORY "${scratch_parent}" OR NOT physical_parent STREQUAL scratch_parent)
    message(FATAL_ERROR "SCRATCH_DIR requires an existing parent without symlinks.")
endif()
is_within("${SCRATCH_DIR}" "${source_root}" scratch_in_source)
is_within("${SCRATCH_DIR}" "${SDK_PREFIX}" scratch_in_prefix)
if(scratch_in_source OR scratch_in_prefix)
    message(FATAL_ERROR "SCRATCH_DIR must be outside the source tree and SDK_PREFIX.")
endif()

unset(mkdir_program CACHE)
unset(ldd_program CACHE)
find_program(mkdir_program NAMES mkdir PATHS /usr/bin /bin NO_DEFAULT_PATH REQUIRED)
find_program(ldd_program NAMES ldd PATHS /usr/bin /bin NO_DEFAULT_PATH REQUIRED)
set(working_directory "${scratch_parent}")
# An exclusive mkdir fails if another invocation claims this path first.
run_checked("create scratch" 10 "${mkdir_program}" -m 700 -- "${SCRATCH_DIR}")

set(consumer_source "${SCRATCH_DIR}/source")
file(MAKE_DIRECTORY "${consumer_source}")
foreach(filename IN ITEMS CMakeLists.txt main.c)
    configure_file("${source_root}/examples/installed_consumer_minimal/${filename}"
        "${consumer_source}/${filename}" COPYONLY)
endforeach()

# Project includes run before find_package. Defer the identity check until
# the standalone project's find_package and target setup have finished.
set(identity_hook "${SCRATCH_DIR}/check-sdk.cmake")
file(WRITE "${identity_hook}" [=[
function(record_sdk_identity)
    if(NOT AnoCryptoC_CONFIG OR NOT TARGET AnoCryptoC::AnoCryptoC)
        message(FATAL_ERROR "The installed AnoCryptoC package was not selected.")
    endif()
    get_target_property(library_type AnoCryptoC::AnoCryptoC TYPE)
    if(NOT library_type STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "The minimal consumer requires a shared AnoCryptoC SDK.")
    endif()
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/sdk-config.txt" CONTENT "${AnoCryptoC_CONFIG}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/sdk-library.txt" CONTENT "$<TARGET_FILE:AnoCryptoC::AnoCryptoC>")
endfunction()
cmake_language(DEFER CALL record_sdk_identity)
]=])

# The loader inspection and executable use the same environment. The installed
# target must work through the consumer's normal build-tree runtime search path.
set(command_environment "${CMAKE_COMMAND}" -E env
    --unset=LD_LIBRARY_PATH --unset=LD_PRELOAD --unset=LD_AUDIT
    --unset=LD_DEBUG --unset=LD_DEBUG_OUTPUT --unset=LD_PROFILE --unset=LD_PROFILE_OUTPUT
    --unset=LD_TRACE_LOADED_OBJECTS LC_ALL=C)

foreach(run_number RANGE 1 2)
    set(run "run ${run_number}")
    set(build "${SCRATCH_DIR}/build-${run_number}")
    file(MAKE_DIRECTORY "${build}")
    set(working_directory "${build}")
    run_checked("${run} configure" 120 ${command_environment} "${CMAKE_COMMAND}"
        -S "${consumer_source}" -B "${build}" -G "Unix Makefiles"
        "-DCMAKE_BUILD_TYPE:STRING=Release"
        "-DAnoCryptoC_DIR:PATH=${SDK_CONFIG_DIR}"
        "-DCMAKE_PREFIX_PATH:PATH=${SDK_PREFIX}"
        "-DCMAKE_PROJECT_AnoCryptoCMinimalConsumer_INCLUDE:FILEPATH=${identity_hook}"
        -DCMAKE_FIND_USE_PACKAGE_REGISTRY:BOOL=OFF
        -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY:BOOL=OFF)

    file(READ "${build}/sdk-config.txt" selected_config)
    file(READ "${build}/sdk-library.txt" selected_library)
    if(NOT selected_config STREQUAL expected_config)
        message(FATAL_ERROR "${run} selected an unexpected AnoCryptoC config.")
    endif()
    validate_path(selected_library)
    get_filename_component(library_real "${selected_library}" REALPATH)
    is_within("${library_real}" "${SDK_PREFIX}" library_in_prefix)
    if(NOT library_in_prefix OR NOT EXISTS "${library_real}" OR IS_DIRECTORY "${library_real}")
        message(FATAL_ERROR "${run} selected a shared library outside the SDK or missing from it.")
    endif()
    message(STATUS "${run} package config '${selected_config}'")

    run_checked("${run} build" 120 ${command_environment} "${CMAKE_COMMAND}" --build "${build}")
    set(executable "${build}/anocrypto_c_minimal_consumer")
    run_checked("${run} ldd" 30 ${command_environment} "${ldd_program}" "${executable}")

    # Match one resolved SONAME row. Capture the entire path, including spaces.
    string(REPLACE "\n" ";" loader_lines "${command_output}")
    set(library_count 0)
    foreach(line IN LISTS loader_lines)
        string(STRIP "${line}" line)
        if(line MATCHES "=> not found")
            message(FATAL_ERROR "${run} has an unresolved shared dependency.")
        endif()
        if(line MATCHES "^(/.*/|/)?libanocrypto_c\\.so(\\.[0-9]+)*([ \t]|$)")
            math(EXPR library_count "${library_count} + 1")
            if(NOT line MATCHES "^libanocrypto_c\\.so(\\.[0-9]+)*[ \t]+=>[ \t]+(/.+) \\(0x[0-9a-fA-F]+\\)$")
                message(FATAL_ERROR "${run} did not resolve the AnoCryptoC SONAME to an absolute path.")
            endif()
            set(loaded_library "${CMAKE_MATCH_2}")
            validate_path(loaded_library)
            get_filename_component(loaded_real "${loaded_library}" REALPATH)
            if(NOT loaded_real STREQUAL library_real)
                message(FATAL_ERROR "${run} resolved a different library than the installed SDK target.")
            endif()
        endif()
    endforeach()
    if(NOT library_count EQUAL 1)
        message(FATAL_ERROR "${run} requires exactly one resolved AnoCryptoC shared library.")
    endif()
    message(STATUS "${run} loaded library '${loaded_real}'")
    run_checked("${run} consumer" 30 ${command_environment} "${executable}")
endforeach()

message(STATUS "Minimal installed consumer engineering smoke passed twice.")
