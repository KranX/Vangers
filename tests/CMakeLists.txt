ADD_EXECUTABLE(xsocket_test
	xsocket_test.cpp
	../src/xsocket.cpp
)

TARGET_INCLUDE_DIRECTORIES(xsocket_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
)

TARGET_LINK_LIBRARIES(xsocket_test PRIVATE
	SDL3::SDL3
	SDL3_net::SDL3_net
)

ADD_TEST(NAME xsocket_loopback COMMAND xsocket_test)
SET_TESTS_PROPERTIES(xsocket_loopback PROPERTIES TIMEOUT 15)

ADD_EXECUTABLE(legacy_data_compat_test
	legacy_data_compat_test.cpp
)

TARGET_INCLUDE_DIRECTORIES(legacy_data_compat_test PRIVATE
	${PROJECT_SOURCE_DIR}/src/iscreen
	${PROJECT_SOURCE_DIR}/lib/xtool
)

TARGET_COMPILE_DEFINITIONS(legacy_data_compat_test PRIVATE
	TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/data"
)

TARGET_LINK_LIBRARIES(legacy_data_compat_test PRIVATE
	SDL3::SDL3
	ZLIB::ZLIB
)

ADD_TEST(NAME legacy_data_compat COMMAND legacy_data_compat_test)

ADD_EXECUTABLE(sdl_event_buffer_test
	sdl_event_buffer_test.cpp
	../src/iscreen/ikeys.cpp
)

TARGET_INCLUDE_DIRECTORIES(sdl_event_buffer_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/iscreen
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(sdl_event_buffer_test PRIVATE SDL3::SDL3)

ADD_TEST(NAME sdl_event_buffer COMMAND sdl_event_buffer_test)

ADD_EXECUTABLE(renderer_aspect_test
	renderer_aspect_test.cpp
)

TARGET_INCLUDE_DIRECTORIES(renderer_aspect_test PRIVATE
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(renderer_aspect_test PRIVATE SDL3::SDL3)

ADD_TEST(NAME renderer_aspect COMMAND renderer_aspect_test)
SET_TESTS_PROPERTIES(renderer_aspect PROPERTIES TIMEOUT 15)

ADD_EXECUTABLE(network_control_flags_test
	network_control_flags_test.cpp
)

TARGET_INCLUDE_DIRECTORIES(network_control_flags_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(network_control_flags_test PRIVATE SDL3::SDL3)

ADD_TEST(NAME network_control_flags COMMAND network_control_flags_test)

ADD_EXECUTABLE(traction_control_cadence_test
	traction_control_cadence_test.cpp
)

TARGET_INCLUDE_DIRECTORIES(traction_control_cadence_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
)

ADD_TEST(NAME traction_control_cadence COMMAND traction_control_cadence_test)

ADD_EXECUTABLE(settings_test
	settings_test.cpp
)

FIND_PACKAGE(Threads REQUIRED)

TARGET_LINK_LIBRARIES(settings_test PRIVATE
	vangers_settings
	Threads::Threads
)

ADD_TEST(NAME settings COMMAND settings_test)

ADD_EXECUTABLE(text_encoding_test
	text_encoding_test.cpp
)

TARGET_LINK_LIBRARIES(text_encoding_test PRIVATE
	vangers_settings
)

ADD_TEST(NAME text_encoding COMMAND text_encoding_test)

ADD_EXECUTABLE(actint_text_test
	actint_text_test.cpp
	../src/actint/text.cpp
)

TARGET_LINK_LIBRARIES(actint_text_test PRIVATE vangers_settings vangers_text)

ADD_TEST(NAME actint_text COMMAND actint_text_test)

ADD_EXECUTABLE(actint_mouse_prompt_test
	actint_mouse_prompt_test.cpp
	../lib/xgraph/xprompt.cpp
)

TARGET_INCLUDE_DIRECTORIES(actint_mouse_prompt_test PRIVATE
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(actint_mouse_prompt_test PRIVATE vangers_settings SDL3::SDL3 xgraph_text)

ADD_TEST(NAME actint_mouse_prompt COMMAND actint_mouse_prompt_test)

ADD_TEST(NAME actint_script_roundtrip
	COMMAND ${CMAKE_COMMAND}
		"-DSOURCE_PARSER=$<TARGET_FILE:actint_script_source_test>"
		"-DBINARY_PARSER=$<TARGET_FILE:actint_script_binary_test>"
		"-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/actint-script-roundtrip"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/actint_script_roundtrip.cmake"
)
SET_TESTS_PROPERTIES(actint_script_roundtrip PROPERTIES TIMEOUT 30)

ADD_TEST(NAME actint_runtime_regressions COMMAND actint_script_binary_test --runtime)
SET_TESTS_PROPERTIES(actint_runtime_regressions PROPERTIES TIMEOUT 15)

FOREACH(renderer bitmap ttf)
	FOREACH(widget menu counter)
		ADD_TEST(NAME actint_${widget}_${renderer}
			COMMAND actint_script_binary_test --render-${widget} ${renderer})
		SET_TESTS_PROPERTIES(actint_${widget}_${renderer} PROPERTIES TIMEOUT 15)
	ENDFOREACH()
ENDFOREACH()

ADD_EXECUTABLE(legacy_settings_migration_test
	legacy_settings_migration_test.cpp
)

TARGET_LINK_LIBRARIES(legacy_settings_migration_test PRIVATE
	vangers_settings
	SDL3::SDL3
)

ADD_TEST(NAME legacy_settings_migration COMMAND legacy_settings_migration_test)

ADD_EXECUTABLE(rle_test
	rle_test.cpp
	../src/rle.cpp
)

TARGET_INCLUDE_DIRECTORIES(rle_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(rle_test PRIVATE SDL3::SDL3)

ADD_TEST(NAME rle_roundtrip COMMAND rle_test)

ADD_EXECUTABLE(actint_ttf_test actint_ttf_test.cpp)
TARGET_LINK_LIBRARIES(actint_ttf_test PRIVATE vangers_ttf SDL3_ttf::SDL3_ttf)
TARGET_COMPILE_DEFINITIONS(actint_ttf_test PRIVATE
	TEST_FONT="${PROJECT_SOURCE_DIR}/data/resource/actint/fonts/VangersText-Regular.ttf"
)
ADD_TEST(NAME actint_ttf COMMAND actint_ttf_test)

ADD_EXECUTABLE(iscreen_text_test iscreen_text_test.cpp)
TARGET_LINK_LIBRARIES(iscreen_text_test PRIVATE vangers_iscreen_text)
ADD_TEST(NAME iscreen_text COMMAND iscreen_text_test)

ADD_TEST(NAME iscreen_script_roundtrip COMMAND ${CMAKE_COMMAND}
	"-DSOURCE_PARSER=$<TARGET_FILE:iscreen_source_test>"
	"-DBINARY_PARSER=$<TARGET_FILE:iscreen_binary_test>"
	"-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/iscreen-script-roundtrip"
	-P "${CMAKE_CURRENT_SOURCE_DIR}/iscreen_script_roundtrip.cmake")
SET_TESTS_PROPERTIES(iscreen_script_roundtrip PROPERTIES TIMEOUT 30)
FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-input")
ADD_TEST(NAME iscreen_input COMMAND iscreen_binary_test --input)
SET_TESTS_PROPERTIES(iscreen_input PROPERTIES TIMEOUT 15 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-input")
FOREACH(renderer bitmap ttf)
	FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-relief-${renderer}")
	ADD_TEST(NAME iscreen_relief_${renderer} COMMAND iscreen_binary_test --relief ${renderer})
	SET_TESTS_PROPERTIES(iscreen_relief_${renderer} PROPERTIES TIMEOUT 15
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-relief-${renderer}")
ENDFOREACH()

FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-relief-procedural")
ADD_TEST(NAME iscreen_relief_procedural COMMAND iscreen_binary_test --relief procedural)
SET_TESTS_PROPERTIES(iscreen_relief_procedural PROPERTIES TIMEOUT 15
 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-relief-procedural")

FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-shadow-damage")
ADD_TEST(NAME iscreen_shadow_damage COMMAND iscreen_binary_test --shadow)
SET_TESTS_PROPERTIES(iscreen_shadow_damage PROPERTIES TIMEOUT 15
 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-shadow-damage")

ADD_EXECUTABLE(relief_cache_test relief_cache_test.cpp)
TARGET_LINK_LIBRARIES(relief_cache_test PRIVATE vangers_iscreen_text)
ADD_TEST(NAME iscreen_relief_cache COMMAND relief_cache_test)

ADD_EXECUTABLE(glyph_surface_test glyph_surface_test.cpp)
TARGET_INCLUDE_DIRECTORIES(glyph_surface_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
ADD_TEST(NAME glyph_surface COMMAND glyph_surface_test)

ADD_TEST(NAME acs_script_roundtrip COMMAND ${CMAKE_COMMAND}
 "-DSOURCE_PARSER=$<TARGET_FILE:acs_source_test>"
 "-DBINARY_PARSER=$<TARGET_FILE:acs_binary_test>"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/acs-script-roundtrip"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/acs_script_roundtrip.cmake")
FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-runtime")
ADD_TEST(NAME acs_runtime COMMAND acs_binary_test --runtime)
SET_TESTS_PROPERTIES(acs_runtime PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-runtime")
ADD_EXECUTABLE(save_text_test save_text_test.cpp)
TARGET_LINK_LIBRARIES(save_text_test PRIVATE vangers_save_text)
FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/save-text")
ADD_TEST(NAME save_text COMMAND save_text_test)
SET_TESTS_PROPERTIES(save_text PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/save-text")

FOREACH(renderer bitmap ttf)
 FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-${renderer}")
 ADD_TEST(NAME acs_render_${renderer} COMMAND acs_binary_test --render ${renderer})
 SET_TESTS_PROPERTIES(acs_render_${renderer} PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-${renderer}")
ENDFOREACH()
FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-save")
ADD_TEST(NAME acs_save_format COMMAND acs_binary_test --save)
SET_TESTS_PROPERTIES(acs_save_format PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/acs-save")

FOREACH(encoding legacy utf8 ids)
 FOREACH(language ru en)
  FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/diagen-${encoding}-${language}")
  ADD_TEST(NAME diagen_${encoding}_${language} COMMAND diagen_runtime_test ${encoding} ${language})
  SET_TESTS_PROPERTIES(diagen_${encoding}_${language} PROPERTIES TIMEOUT 30
   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/diagen-${encoding}-${language}")
 ENDFOREACH()
ENDFOREACH()
ADD_TEST(NAME diagen_resource_roundtrip COMMAND ${CMAKE_COMMAND}
 "-DSOURCE_PARSER=$<TARGET_FILE:diagen_source_test>"
 "-DBINARY_PARSER=$<TARGET_FILE:diagen_runtime_test>"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/diagen-resources"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/diagen_resources.cmake")
SET_TESTS_PROPERTIES(diagen_resource_roundtrip PROPERTIES TIMEOUT 90)
ADD_TEST(NAME diagen_shipped_roundtrip COMMAND ${CMAKE_COMMAND}
 "-DSOURCE_PARSER=$<TARGET_FILE:diagen_source_test>"
 "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}/data/data"
 "-DARCHIVE_ROOT=${PROJECT_SOURCE_DIR}/data"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/diagen-shipped-source"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/diagen_shipped_roundtrip.cmake")
SET_TESTS_PROPERTIES(diagen_shipped_roundtrip PROPERTIES TIMEOUT 90)
SET(diagen_legacy_dir "${CMAKE_CURRENT_BINARY_DIR}/diagen-legacy")
FILE(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/diagen-legacy.zip" DESTINATION "${diagen_legacy_dir}")
FILE(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/diagen-en-progress.zip" DESTINATION "${diagen_legacy_dir}")
FOREACH(language ru en)
 SET(dir "${CMAKE_CURRENT_BINARY_DIR}/diagen-shipped-${language}")
 FILE(MAKE_DIRECTORY "${dir}")
 CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/data/diagen.text" "${dir}/diagen.text" COPYONLY)
 CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/data/diagen_eng.text" "${dir}/diagen_eng.text" COPYONLY)
 ADD_TEST(NAME diagen_shipped_${language} COMMAND diagen_runtime_test shipped ${language} "${diagen_legacy_dir}")
 SET_TESTS_PROPERTIES(diagen_shipped_${language} PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${dir}")
ENDFOREACH()

FOREACH(mode canonical legacy-indices)
 SET(dir "${CMAKE_CURRENT_BINARY_DIR}/diagen-${mode}")
 FILE(MAKE_DIRECTORY "${dir}")
 CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/data/diagen.text" "${dir}/diagen.text" COPYONLY)
 CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/data/diagen_eng.text" "${dir}/diagen_eng.text" COPYONLY)
 ADD_TEST(NAME diagen_${mode} COMMAND diagen_runtime_test ${mode} "${diagen_legacy_dir}")
 SET_TESTS_PROPERTIES(diagen_${mode} PROPERTIES TIMEOUT 90 WORKING_DIRECTORY "${dir}")
ENDFOREACH()

FOREACH(mode empty empty-legacy empty-all empty-all-legacy)
 FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/diagen-${mode}")
 ADD_TEST(NAME diagen_${mode} COMMAND diagen_runtime_test ${mode})
 SET_TESTS_PROPERTIES(diagen_${mode} PROPERTIES TIMEOUT 30
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/diagen-${mode}")
ENDFOREACH()

ADD_EXECUTABLE(text_layout_test text_layout_test.cpp)
TARGET_LINK_LIBRARIES(text_layout_test PRIVATE vangers_text vangers_settings)
ADD_TEST(NAME text_layout COMMAND text_layout_test)
FOREACH(renderer bitmap ttf)
 ADD_TEST(NAME actint_flow_${renderer} COMMAND actint_script_binary_test --flow ${renderer})
 SET_TESTS_PROPERTIES(actint_flow_${renderer} PROPERTIES TIMEOUT 30)
ENDFOREACH()

FOREACH(renderer bitmap ttf)
 FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-flow-${renderer}")
 ADD_TEST(NAME iscreen_flow_${renderer} COMMAND iscreen_binary_test --flow ${renderer})
 SET_TESTS_PROPERTIES(iscreen_flow_${renderer} PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/iscreen-flow-${renderer}")
ENDFOREACH()

GET_PROPERTY(locale_tests DIRECTORY PROPERTY TESTS)
FOREACH(test IN LISTS locale_tests)
 SET_PROPERTY(TEST "${test}" APPEND PROPERTY ENVIRONMENT "VANGERS_LOCALES=${PROJECT_SOURCE_DIR}/data/locales")
ENDFOREACH()

TARGET_LINK_LIBRARIES(actint_text_test PRIVATE vangers_localization)
TARGET_LINK_LIBRARIES(actint_mouse_prompt_test PRIVATE vangers_localization)

ADD_EXECUTABLE(localization_test localization_test.cpp)
TARGET_LINK_LIBRARIES(localization_test PRIVATE vangers_localization toml11::toml11 ZLIB::ZLIB)
ADD_TEST(NAME localization COMMAND localization_test "${PROJECT_SOURCE_DIR}/data/locales")
SET_TESTS_PROPERTIES(localization PROPERTIES ENVIRONMENT "VANGERS_LOCALES=${PROJECT_SOURCE_DIR}/data/locales")
ADD_TEST(NAME localization_cli COMMAND ${CMAKE_COMMAND}
 "-DGAME=$<TARGET_FILE:vangers>" -P "${CMAKE_CURRENT_SOURCE_DIR}/localization_cli.cmake")
SET_TESTS_PROPERTIES(localization_cli PROPERTIES TIMEOUT 45)
ADD_TEST(NAME diagen_locale_source COMMAND ${CMAKE_COMMAND}
 "-DSOURCE_PARSER=$<TARGET_FILE:diagen_source_test>"
 "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}/data/data"
 "-DENGLISH_ARCHIVE=${PROJECT_SOURCE_DIR}/data/diagen_eng.text"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/diagen-third-source"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/diagen_locale_source.cmake")
SET_TESTS_PROPERTIES(diagen_locale_source PROPERTIES TIMEOUT 45
 ENVIRONMENT "VANGERS_LOCALES=${PROJECT_SOURCE_DIR}/data/locales")
ADD_TEST(NAME localization_installed COMMAND ${CMAKE_COMMAND}
 "-DPROBE=$<TARGET_FILE:localization_test>" "-DLOCALES=${PROJECT_SOURCE_DIR}/data/locales"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/localization-install"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/localization_installed.cmake")
SET_TESTS_PROPERTIES(localization_installed PROPERTIES TIMEOUT 30)
IF(UNIX)
 FIND_PROGRAM(DEPLOY_BASH bash)
 IF(DEPLOY_BASH)
  ADD_TEST(NAME localization_deploy_msys2 COMMAND ${CMAKE_COMMAND}
   "-DHELPER=msys2" "-DSHELL=${DEPLOY_BASH}" "-DPROBE=$<TARGET_FILE:localization_test>"
   "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}" "-DLOCALES=${PROJECT_SOURCE_DIR}/data/locales"
   "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/locale deploy" -P "${CMAKE_CURRENT_SOURCE_DIR}/localization_deploy.cmake")
  SET_TESTS_PROPERTIES(localization_deploy_msys2 PROPERTIES TIMEOUT 30)
 ENDIF()
ENDIF()
IF(WIN32)
 FIND_PROGRAM(DEPLOY_POWERSHELL NAMES pwsh powershell REQUIRED)
 ADD_TEST(NAME localization_deploy_msvc COMMAND ${CMAKE_COMMAND}
  "-DHELPER=msvc" "-DSHELL=${DEPLOY_POWERSHELL}" "-DPROBE=$<TARGET_FILE:localization_test>"
  "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}" "-DLOCALES=${PROJECT_SOURCE_DIR}/data/locales"
  "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/locale deploy" -P "${CMAKE_CURRENT_SOURCE_DIR}/localization_deploy.cmake")
 SET_TESTS_PROPERTIES(localization_deploy_msvc PROPERTIES TIMEOUT 60)
ENDIF()
ADD_TEST(NAME actint_comments COMMAND ${CMAKE_COMMAND}
 "-DSOURCE_PARSER=$<TARGET_FILE:actint_script_source_test>"
 "-DBINARY_PARSER=$<TARGET_FILE:actint_script_binary_test>"
 "-DITEMS=${PROJECT_SOURCE_DIR}/data/actint/items.inc"
 "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/actint-comments"
 -P "${CMAKE_CURRENT_SOURCE_DIR}/actint_comments.cmake")
SET_TESTS_PROPERTIES(actint_comments PROPERTIES TIMEOUT 30
 ENVIRONMENT "VANGERS_LOCALES=${PROJECT_SOURCE_DIR}/data/locales")
