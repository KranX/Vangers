STRING(RANDOM LENGTH 12 ALPHABET 0123456789abcdef nonce)
SET(stage "${TEST_ROOT}/installed-${nonce}")
FILE(MAKE_DIRECTORY "${stage}/bin" "${stage}/old-data")
FILE(COPY "${PROBE}" DESTINATION "${stage}/bin")
FILE(COPY "${LOCALES}" DESTINATION "${stage}/bin")
GET_FILENAME_COMPONENT(exe "${PROBE}" NAME)
IF(WIN32)
 GET_FILENAME_COMPONENT(directory "${PROBE}" DIRECTORY)
 FILE(GLOB dlls "${directory}/*.dll")
 IF(dlls)
  FILE(COPY ${dlls} DESTINATION "${stage}/bin")
 ENDIF()
ENDIF()
# No source-tree/environment rescue: the relocated executable uses its own payload.
FOREACH(locale en ru)
 SET(expected "NEW GAME")
 IF(locale STREQUAL "ru")
  SET(expected "НОВАЯ ИГРА")
 ENDIF()
 EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E env --unset=VANGERS_LOCALES
  "${stage}/bin/${exe}" --installed "${locale}" "${expected}"
  WORKING_DIRECTORY "${stage}/old-data" RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
 IF(NOT result STREQUAL "0")
  MESSAGE(FATAL_ERROR "Installed ${locale}: ${result}: ${stdout}\n${stderr}")
 ENDIF()
ENDFOREACH()
# A local package wins over the installed copy; a broken explicit root is not hidden.
FILE(COPY "${LOCALES}" DESTINATION "${stage}/old-data")
FILE(READ "${stage}/old-data/locales/en/strings.toml" strings)
STRING(REPLACE "NEW GAME" "LOCAL OVERRIDE" strings "${strings}")
FILE(WRITE "${stage}/old-data/locales/en/strings.toml" "${strings}")
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E env --unset=VANGERS_LOCALES
 "${stage}/bin/${exe}" --installed en "LOCAL OVERRIDE"
 WORKING_DIRECTORY "${stage}/old-data" RESULT_VARIABLE result)
IF(NOT result STREQUAL "0")
 MESSAGE(FATAL_ERROR "The local override was ignored")
ENDIF()
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E env "VANGERS_LOCALES=${stage}/missing"
 "${stage}/bin/${exe}" --installed en "NEW GAME"
 WORKING_DIRECTORY "${stage}/old-data" RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET)
IF(NOT result STREQUAL "1")
 MESSAGE(FATAL_ERROR "A broken explicit locale root was silently hidden")
ENDIF()
