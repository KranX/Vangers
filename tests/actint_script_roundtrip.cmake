FILE(MAKE_DIRECTORY "${TEST_ROOT}")

FUNCTION(run_parser executable input output encoding expected_option)
	FILE(REMOVE "${TEST_ROOT}/logfile.txt")
	SET(command "${executable}" --parse "${input}" "${output}" "${encoding}")
	IF(NOT expected_option STREQUAL "")
		# Abort intentionally exits with live models. Positive cases keep leak checking on.
		LIST(PREPEND command "${CMAKE_COMMAND}" -E env "ASAN_OPTIONS=$ENV{ASAN_OPTIONS}:detect_leaks=0")
	ENDIF()
	EXECUTE_PROCESS(
		COMMAND ${command}
		WORKING_DIRECTORY "${TEST_ROOT}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE stdout
		ERROR_VARIABLE stderr
		TIMEOUT 10
	)
	IF(expected_option STREQUAL "")
		IF(NOT result STREQUAL "0")
			MESSAGE(FATAL_ERROR "Parser failed (${result}): ${stdout}\n${stderr}")
		ENDIF()
	ELSE()
		IF(NOT result STREQUAL "1")
			MESSAGE(FATAL_ERROR "Invalid input did not use ErrH.Abort (${result}): ${stdout}\n${stderr}")
		ENDIF()
		FILE(READ "${TEST_ROOT}/logfile.txt" log)
		FOREACH(text "ERROR_DIALOG:" "Invalid actint display text" "${input}: option ${expected_option}")
			STRING(FIND "${stderr}" "${text}" position)
			IF(position LESS 0)
				MESSAGE(FATAL_ERROR "Error dialog lacks '${text}': ${stderr}")
			ENDIF()
		ENDFOREACH()
		STRING(FIND "${log}" "${input}: option ${expected_option}" position)
		IF(position LESS 0)
			MESSAGE(FATAL_ERROR "Log lacks resource/option context: ${log}")
		ENDIF()
	ENDIF()
ENDFUNCTION()

SET(unicode_text "Тест 日本 😀")
STRING(ASCII 240 166 legacy_text) # CP866 Ёж, intentionally not UTF-8.
STRING(ASCII 255 invalid_byte)

FOREACH(encoding unicode legacy)
	SET(text "${${encoding}_text}")
	SET(body "world_map {\nworld_data 0 0 0 {\nname \"${text}\"\n}\n}\ninvItem \"${text}\" {\nprm_template \"${text}\"\nprompt_text \"${text}\"\n}\nfncMenu {\nfncMenuItem {\nstring \"${text}\"\n}\n}\n")
	FILE(WRITE "${TEST_ROOT}/${encoding}.inc" "${body}")
	IF(encoding STREQUAL "unicode")
		SET(flag "utf8_text\n")
	ELSE()
		SET(flag "")
	ENDIF()
	FILE(WRITE "${TEST_ROOT}/${encoding}.scr" "${flag}#include \"${encoding}.inc\"\n")
	run_parser("${SOURCE_PARSER}" "${encoding}.scr" "${encoding}.scb" "${encoding}" "")
	run_parser("${BINARY_PARSER}" "${encoding}.scb" unused.scb "${encoding}" "")
	STRING(REGEX REPLACE "\n$" "" no_newline "${body}")
	FILE(WRITE "${TEST_ROOT}/${encoding}-no-newline.scr" "${flag}${no_newline}")
	run_parser("${SOURCE_PARSER}" "${encoding}-no-newline.scr" "${encoding}-no-newline.scb" "${encoding}" "")
	run_parser("${BINARY_PARSER}" "${encoding}-no-newline.scb" unused.scb "${encoding}" "")
ENDFOREACH()

# The flag deliberately applies to included files as well; reject a mixed legacy include.
FILE(WRITE "${TEST_ROOT}/mixed.scr" "utf8_text\n#include \"legacy.inc\"\n")
run_parser("${SOURCE_PARSER}" mixed.scr mixed.scb unicode name)

# Exercise every source-parser decoding path, not just the allocating read_text helper.
SET(name "world_map {\nworld_data 0 0 0 {\nname \"${invalid_byte}\"\n}\n}\n")
SET(invItem "invItem \"${invalid_byte}\" {\n}\n")
SET(prm_template "invItem good {\nprm_template \"${invalid_byte}\"\n}\n")
SET(string "fncMenu {\nfncMenuItem {\nstring \"${invalid_byte}\"\n}\n}\n")
FOREACH(option name invItem prm_template string)
	FILE(WRITE "${TEST_ROOT}/bad-${option}.scr" "utf8_text\n${${option}}")
	run_parser("${SOURCE_PARSER}" "bad-${option}.scr" "bad-${option}.scb" unicode "${option}")
ENDFOREACH()

EXECUTE_PROCESS(
	COMMAND "${BINARY_PARSER}" --corrupt unicode.scb bad-binary.scb
	WORKING_DIRECTORY "${TEST_ROOT}"
	RESULT_VARIABLE result
)
IF(NOT result STREQUAL "0")
	MESSAGE(FATAL_ERROR "Cannot corrupt real SCB output (${result})")
ENDIF()
run_parser("${BINARY_PARSER}" bad-binary.scb unused.scb unicode name)

FOREACH(api screen prompt)
	EXECUTE_PROCESS(
		COMMAND "${CMAKE_COMMAND}" -E env "ASAN_OPTIONS=$ENV{ASAN_OPTIONS}:detect_leaks=0"
			"${BINARY_PARSER}" --invalid-api "${api}"
		WORKING_DIRECTORY "${TEST_ROOT}"
		RESULT_VARIABLE result
		ERROR_VARIABLE stderr
		OUTPUT_QUIET
		TIMEOUT 10
	)
	IF(api STREQUAL "screen")
		SET(context "aciScreenText::alloc_utf8")
	ELSE()
		SET(context "aciPromptData::add_utf8_str")
	ENDIF()
	STRING(FIND "${stderr}" "ERROR_DIALOG:" dialog)
	STRING(FIND "${stderr}" "${context}" position)
	IF(NOT result STREQUAL "1" OR position LESS 0 OR dialog LESS 0)
		MESSAGE(FATAL_ERROR "Invalid ${api} API input lost its diagnostic (${result}): ${stderr}")
	ENDIF()
ENDFOREACH()
