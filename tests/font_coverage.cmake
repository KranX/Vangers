FILE(MAKE_DIRECTORY "${TEST_ROOT}")
EXECUTE_PROCESS(COMMAND "${CHECKER}" "${LOCALES}" qa-missing-package
 RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "2" OR NOT stderr MATCHES "Missing locale package")
 MESSAGE(FATAL_ERROR "Missing package was accepted as covered: ${result}: ${stdout}\n${stderr}")
ENDIF()
# A real package plus a separate UTF-8 resource, with all three role chains.
FILE(WRITE "${TEST_ROOT}/ja.txt" "日本語「カタカナ」が表示されます。が\nÉté Łódź ÄÖÜ ß")
EXECUTE_PROCESS(COMMAND "${CHECKER}" "${LOCALES}" en --text "${TEST_ROOT}/ja.txt"
 RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "0")
 MESSAGE(FATAL_ERROR "Covered text rejected: ${stdout}\n${stderr}")
ENDIF()
STRING(ASCII 244 143 191 191 absent)
FILE(WRITE "${TEST_ROOT}/missing.txt" "${absent}${absent}")
EXECUTE_PROCESS(COMMAND "${CHECKER}" "${LOCALES}" en --text "${TEST_ROOT}/missing.txt"
 RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "1" OR NOT stderr MATCHES "missing.txt role=Text missing U\\+10FFFF")
 MESSAGE(FATAL_ERROR "Missing glyph not diagnosed by resource/role/codepoint: ${result}: ${stdout}\n${stderr}")
ENDIF()
STRING(ASCII 255 invalid)
FILE(WRITE "${TEST_ROOT}/invalid.txt" "${invalid}")
EXECUTE_PROCESS(COMMAND "${CHECKER}" "${LOCALES}" en --text "${TEST_ROOT}/invalid.txt"
 RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "2")
 MESSAGE(FATAL_ERROR "Malformed UTF-8 accepted: ${result}: ${stdout}\n${stderr}")
ENDIF()
EXECUTE_PROCESS(COMMAND "${CHECKER}" "${LOCALES}" en --text "${CMAKE_CURRENT_LIST_DIR}/fixtures/font-embedded-nul.txt"
 RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "2" OR NOT stderr MATCHES "NUL-free")
 MESSAGE(FATAL_ERROR "Embedded NUL was accepted: ${result}: ${stdout}\n${stderr}")
ENDIF()
