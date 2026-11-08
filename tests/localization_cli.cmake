FOREACH(mode missing invalid uppercase)
 SET(arguments --locale)
 IF(mode STREQUAL "missing")
  SET(expected "--locale requires a locale ID")
 ELSE()
  SET(expected "Invalid locale ID")
  IF(mode STREQUAL "invalid")
   LIST(APPEND arguments ../ru)
  ELSE()
   LIST(APPEND arguments RU)
  ENDIF()
 ENDIF()
 # Invalid command lines intentionally terminate before initializing game assets.
 EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E env
  SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
  "ASAN_OPTIONS=$ENV{ASAN_OPTIONS}:detect_leaks=0"
  "${GAME}" ${arguments}
  RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 10)
 STRING(FIND "${stdout}\n${stderr}" "${expected}" diagnostic)
 IF(NOT result STREQUAL "1" OR diagnostic LESS 0)
  MESSAGE(FATAL_ERROR "Locale CLI ${mode}: ${result}: ${stdout}\n${stderr}")
 ENDIF()
ENDFOREACH()
