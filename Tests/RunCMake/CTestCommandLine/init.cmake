# This is run by test initialization in Build.cmake with cmake -P
# It creates TEST_OUTPUT_FILE with a 0 in it
file(WRITE "${TEST_OUTPUT_FILE}" "0")
