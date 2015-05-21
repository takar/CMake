add_test(NAME FindOpenGL.Test COMMAND
  ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>
  --build-and-test
  "${CMake_SOURCE_DIR}/Tests/FindOpenGL/Test"
  "${CMake_BINARY_DIR}/Tests/FindOpenGL/Test"
  ${build_generator_args}
  --build-project TestFindOpenGL
  --build-options ${build_options}
    -DCMake_TEST_FindOpenGL_REQUIRED=${CMake_TEST_FindOpenGL_REQUIRED}
  --test-command ${CMAKE_CTEST_COMMAND} -V -C $<CONFIGURATION>
  )
