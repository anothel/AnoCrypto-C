cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED ANO_CRYPTO_SOURCE_DIR)
  get_filename_component(ANO_CRYPTO_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()
if(NOT DEFINED ANO_CRYPTO_SUBMISSION_SKELETON_SMOKE_DIR)
  set(ANO_CRYPTO_SUBMISSION_SKELETON_SMOKE_DIR "${ANO_CRYPTO_SOURCE_DIR}/build/submission-skeleton-smoke")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
  -DANO_CRYPTO_SOURCE_DIR=${ANO_CRYPTO_SOURCE_DIR}
  -DANO_CRYPTO_SUBMISSION_SKELETON_OUTPUT_DIR=${ANO_CRYPTO_SUBMISSION_SKELETON_SMOKE_DIR}
  -P "${ANO_CRYPTO_SOURCE_DIR}/tools/create_submission_skeleton_package.cmake"
  RESULT_VARIABLE _create)
if(NOT _create EQUAL 0)
  message(FATAL_ERROR "submission skeleton creation failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
  -DANO_CRYPTO_SUBMISSION_SKELETON_DIR=${ANO_CRYPTO_SUBMISSION_SKELETON_SMOKE_DIR}
  -P "${ANO_CRYPTO_SOURCE_DIR}/tools/check_submission_skeleton_package.cmake"
  RESULT_VARIABLE _check)
if(NOT _check EQUAL 0)
  message(FATAL_ERROR "submission skeleton check failed")
endif()
