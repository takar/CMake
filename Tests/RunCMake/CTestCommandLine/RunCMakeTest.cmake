include(RunCMake)

run_cmake_command(repeat-until-fail-bad1 ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail)
run_cmake_command(repeat-until-fail-bad2 ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail foo)
run_cmake_command(repeat-until-fail-good ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail 2)
# Use a single build tree for a few tests without cleaning.
set(RunCMake_TEST_BINARY_DIR ${RunCMake_BINARY_DIR}/Build-build)
set(RunCMake_TEST_NO_CLEAN 1)
file(REMOVE_RECURSE "${RunCMake_TEST_BINARY_DIR}")
file(MAKE_DIRECTORY "${RunCMake_TEST_BINARY_DIR}")

set(RunCMake_TEST_OPTIONS -DCMAKE_VERBOSE_MAKEFILE=1)
run_cmake(Build)
unset(RunCMake_TEST_OPTIONS)
run_cmake_command(repeat-until-fail-tests ${CMAKE_CTEST_COMMAND}
  -C ${RunCMake_CTestTest_CONFIG}
  --repeat-until-fail 3 )
