include(RunCMake)

function(run_cpack_symlink_test TEST_NAME)
  set(RunCMake_TEST_NO_CLEAN TRUE)
  set(RunCMake_TEST_BINARY_DIR "${RunCMake_BINARY_DIR}/${TEST_NAME}-build")
  set(RunCMake_TEST_SOURCE_DIR "${RunCMake_BINARY_DIR}/${TEST_NAME}-source")
  file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")
  file(REMOVE_RECURSE "${RunCMake_TEST_SOURCE_DIR}")
  file(MAKE_DIRECTORY "${RunCMake_TEST_SOURCE_DIR}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar xvf
      "${RunCMake_SOURCE_DIR}/testcpacksym.tar"
    WORKING_DIRECTORY "${RunCMake_TEST_SOURCE_DIR}"
    )
  run_cmake(${TEST_NAME})
  run_cmake_command(${TEST_NAME} ${CMAKE_CPACK_COMMAND}
   --config CPackSourceConfig.cmake)
endfunction()

run_cpack_symlink_test(CPack_TestSymlinks)
