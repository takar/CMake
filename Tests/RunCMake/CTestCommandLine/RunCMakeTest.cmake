include(RunCMake)

run_cmake_command(repeat-until-fail-bad1 ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail)
run_cmake_command(repeat-until-fail-bad2 ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail foo)
run_cmake_command(repeat-until-fail-good ${CMAKE_CTEST_COMMAND}
  --repeat-until-fail 2)
