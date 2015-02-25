include(RunCTest)

set(CASE_CTEST_TEST_ARGS "")

function(run_ctest_test CASE_NAME)
  set(CASE_CTEST_TEST_ARGS "${ARGN}")
  run_ctest(${CASE_NAME})
endfunction()

run_ctest_test(TestQuiet QUIET)

set(CMAKELISTS_EXTRA_CODE "
add_test(NAME CMakeVersionFail COMMAND \"${CMAKE_COMMAND}\" --version)
set_tests_properties(CMakeVersionFail PROPERTIES WILL_FAIL TRUE)
")
run_ctest_test(TestFail INCLUDE Fail)
set(CMAKELISTS_EXTRA_CODE "")
