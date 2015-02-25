include(RunCTest)

set(CASE_CTEST_BUILD_ARGS "")

function(run_ctest_build CASE_NAME)
  set(CASE_CTEST_BUILD_ARGS "${ARGN}")
  run_ctest(${CASE_NAME})
endfunction()

run_ctest_build(BuildQuiet QUIET)

set(CMAKELISTS_EXTRA_CODE "
enable_language(CXX)
add_executable(compile_error
 ${RunCMake_SOURCE_DIR}/../../CTestTestFailure/badCode.cxx)")
run_ctest_build(BuildCompileError TARGET compile_error)
set(CMAKELISTS_EXTRA_CODE "")
