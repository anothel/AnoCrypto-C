# Keep one existing CTest registration, including its release-evidence index.
execute_process(COMMAND "${MODULE_STATE_TEST}" RESULT_VARIABLE status TIMEOUT 30)
if(NOT status STREQUAL "0")
    message(FATAL_ERROR "Library-backed module state tests failed: ${status}")
endif()
foreach(scenario RANGE 0 4)
    execute_process(COMMAND "${MODULE_RACE_TEST}" "${scenario}"
        RESULT_VARIABLE status TIMEOUT 30)
    if(NOT status STREQUAL "0")
        message(FATAL_ERROR "Module lifecycle race scenario ${scenario} failed: ${status}")
    endif()
endforeach()
