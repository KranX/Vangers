FILE(MAKE_DIRECTORY "${TEST_ROOT}")
FILE(COPY "${DATA_ROOT}/actint" "${DATA_ROOT}/iscreen" DESTINATION "${TEST_ROOT}")
# Parse the actual stock item/matrix section, not private gate animation assets
# loaded by the later location declarations. Geometry profiles share this section.
FILE(READ "${DATA_ROOT}/actint/aci_iscr.inc" stock)
STRING(FIND "${stock}" "iscreen_section {" start)
STRING(SUBSTRING "${stock}" ${start} -1 stock)
STRING(FIND "${stock}" "fncMenu {" end)
STRING(SUBSTRING "${stock}" 0 ${end} stock)
FOREACH(profile 1 2)
 FILE(WRITE "${TEST_ROOT}/descriptions${profile}.scr"
  "#include \"actint/a_consts.h\"\n#include \"actint/aci_ev.inc\"\n#include \"actint/a_str.inc\"\n#include \"actint/a_str${profile}.inc\"\n#include \"actint/actint.inc\"\n${stock}\n}\n")
 FOREACH(locale en ru)
  EXECUTE_PROCESS(COMMAND "${SOURCE_PARSER}" --description-map
   "descriptions${profile}.scr" "${profile}-${locale}.scb" "${locale}"
   WORKING_DIRECTORY "${TEST_ROOT}" RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
  IF(NOT result STREQUAL "0")
   MESSAGE(FATAL_ERROR "Source ${profile}/${locale}: ${result}: ${stdout}\n${stderr}")
  ENDIF()
  EXECUTE_PROCESS(COMMAND "${BINARY_PARSER}" --description-map
   "${profile}-${locale}.scb" unused.scb "${locale}"
   WORKING_DIRECTORY "${TEST_ROOT}" RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr)
  IF(NOT result STREQUAL "0")
   MESSAGE(FATAL_ERROR "SCB ${profile}/${locale}: ${result}: ${stdout}\n${stderr}")
  ENDIF()
 ENDFOREACH()
ENDFOREACH()
