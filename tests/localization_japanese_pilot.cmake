STRING(RANDOM LENGTH 12 ALPHABET 0123456789abcdef nonce)
SET(stage "${TEST_ROOT}/pilot-${nonce}")
SET(source "${stage}/source")
FILE(MAKE_DIRECTORY "${source}/data")
FILE(GLOB common "${DATA_ROOT}/data/*.dil" "${DATA_ROOT}/data/room.lst")
FILE(COPY ${common} "${DATA_ROOT}/data/en" "${DATA_ROOT}/data/ja" DESTINATION "${source}/data")
EXECUTE_PROCESS(COMMAND "${SOURCE_PARSER}" pack ja
 WORKING_DIRECTORY "${source}" RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "0")
 MESSAGE(FATAL_ERROR "Japanese archive generation failed: ${stdout}\n${stderr}")
ENDIF()
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E compare_files
 "${source}/diagen_new.text" "${DATA_ROOT}/locales/ja/diagen.text" RESULT_VARIABLE result)
IF(NOT result STREQUAL "0")
 MESSAGE(FATAL_ERROR "Packaged Japanese dialogue is stale")
ENDIF()

# Match an installed game: the executable and locale payload are separate
# from the original game data, with no explicit locale-root override.
FILE(MAKE_DIRECTORY "${stage}/bin" "${stage}/game data")
FILE(COPY "${RUNTIME_PARSER}" DESTINATION "${stage}/bin")
FILE(COPY "${DATA_ROOT}/locales/en" "${DATA_ROOT}/locales/ja" DESTINATION "${stage}/bin/locales")
FILE(COPY "${DATA_ROOT}/diagen_eng.text" DESTINATION "${stage}/game data")
GET_FILENAME_COMPONENT(exe "${RUNTIME_PARSER}" NAME)
IF(WIN32)
 GET_FILENAME_COMPONENT(runtime_dir "${RUNTIME_PARSER}" DIRECTORY)
 FILE(GLOB dlls "${runtime_dir}/*.dll")
 IF(dlls)
  FILE(COPY ${dlls} DESTINATION "${stage}/bin")
 ENDIF()
ENDIF()
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E env --unset=VANGERS_LOCALES
 "${stage}/bin/${exe}" japanese-pilot
 WORKING_DIRECTORY "${stage}/game data" RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
IF(NOT result STREQUAL "0")
 MESSAGE(FATAL_ERROR "Japanese runtime regression: ${stdout}\n${stderr}")
ENDIF()
FILE(REMOVE_RECURSE "${stage}")
