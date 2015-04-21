if(NOT CPackComponentsDEB_SOURCE_DIR)
  message(FATAL_ERROR "CPackComponentsDEB_SOURCE_DIR not set")
endif()

include(${CPackComponentsDEB_SOURCE_DIR}/RunCPackVerifyResult.cmake)

# TODO: currently debian doens't produce lower cased names
set(expected_file_mask "${CPackComponentsDEB_BINARY_DIR}/MyLib-*.deb")
set(expected_count 3)


set(actual_output)
run_cpack(actual_output
          CPack_output
          CPack_error
          EXPECTED_FILE_MASK "${expected_file_mask}"
          CONFIG_ARGS "${config_args}"
          CONFIG_VERBOSE "${config_verbose}")

message(STATUS "expected_count='${expected_count}'")
message(STATUS "expected_file_mask='${expected_file_mask}'")
message(STATUS "actual_output_files='${actual_output}'")

if(NOT actual_output)
  message(FATAL_ERROR "error: expected_files do not exist: CPackComponentsDEB test fails. (CPack_output=${CPack_output}, CPack_error=${CPack_error}")
endif()

list(LENGTH actual_output actual_count)
message(STATUS "actual_count='${actual_count}'")
if(NOT actual_count EQUAL expected_count)
  message(FATAL_ERROR "error: expected_count=${expected_count} does not match actual_count=${actual_count}: CPackComponents test fails. (CPack_output=${CPack_output}, CPack_error=${CPack_error})")
endif()


# lintian checks
set(lintian_output_errors_all "")
foreach(_f IN LISTS actual_output)
  set(STRINGS_TO_AVOID "E:([^\r\n]*)control-file-has-bad-permissions")
  lintian_check_specific_errors(lintian_output_errors
                                FILENAME "${_f}"
                                ERROR_REGEX_STRINGS "${STRINGS_TO_AVOID}")

  set(lintian_output_errors_all "${lintian_output_errors_all}${lintian_output_errors}")
endforeach()
if(NOT "${lintian_output_errors_all}" STREQUAL "")
  message(FATAL_ERROR "Lintian checks failed:\n${lintian_output_errors_all}")
endif()


# dpkg-deb checks
set(dpkgdeb_output_errors_all "")
foreach(_f IN LISTS actual_output)
  run_dpkgdeb(dpkg_output
              FILENAME "${_f}"
              )

  dpkgdeb_return_specific_metaentry(dpkgentry
                                    DPKGDEB_OUTPUT "${dpkg_output}"
                                    METAENTRY "Maintainer:")

  if(NOT "${dpkgentry}" STREQUAL "None")
    set(dpkgdeb_output_errors_all "${dpkgdeb_output_errors_all}"
        "dpkg-deb: ${_f}: Incorrect value for Maintainer: ${dpkgentry} != None\n")
  endif()
endforeach()


if(NOT "${dpkgdeb_output_errors_all}" STREQUAL "")
  message(FATAL_ERROR "dpkg-deb checks failed:\n${dpkgdeb_output_errors_all}")
endif()
