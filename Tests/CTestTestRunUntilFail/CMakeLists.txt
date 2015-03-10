cmake_minimum_required(VERSION 2.8.12)
project(CTestTestSerialOrder)
enable_testing()
set(TEST_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/test_output.txt")
add_test(NAME initialization
  COMMAND ${CMAKE_COMMAND}
  "-DTEST_OUTPUT_FILE=${TEST_OUTPUT_FILE}"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/init.cmake")
add_test(NAME test1
  COMMAND ${CMAKE_COMMAND}
  "-DTEST_OUTPUT_FILE=${TEST_OUTPUT_FILE}"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/test1.cmake")
set_tests_properties(test1 PROPERTIES DEPENDS "initialization")
