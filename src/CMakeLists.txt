include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
	"${PROJECT_SOURCE_DIR}/lib/xsound"
)

ADD_LIBRARY(vangers_text STATIC text/utf8.cpp text/layout.cpp)
TARGET_INCLUDE_DIRECTORIES(vangers_text PUBLIC "${PROJECT_SOURCE_DIR}/src")

ADD_LIBRARY(vangers_settings STATIC
	settings/gamepad_mapping.cpp
	settings/input_binding.cpp
	settings/legacy_settings_import.cpp
	settings/settings.cpp
	settings/settings_io.cpp
	settings/text_encoding.cpp
)

TARGET_INCLUDE_DIRECTORIES(vangers_settings PUBLIC
	"${PROJECT_SOURCE_DIR}/src"
)

TARGET_LINK_LIBRARIES(vangers_settings PRIVATE
	SDL3::SDL3
	toml11::toml11
)

ADD_LIBRARY(vangers_localization STATIC localization/catalog.cpp)
TARGET_INCLUDE_DIRECTORIES(vangers_localization PUBLIC "${PROJECT_SOURCE_DIR}/src")
TARGET_LINK_LIBRARIES(vangers_localization PUBLIC vangers_settings PRIVATE toml11::toml11 SDL3::SDL3)

ADD_LIBRARY(vangers_save_text STATIC saves/text.cpp)
TARGET_LINK_LIBRARIES(vangers_save_text PUBLIC vangers_settings)

SET(EMBEDDED_FONT_SOURCES)
FOREACH(face Text Menu Display)
	SET(FONT_PATH "${PROJECT_SOURCE_DIR}/data/resource/actint/fonts/Vangers${face}-Regular.ttf")
	SET_PROPERTY(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FONT_PATH}")
	IF(face STREQUAL "Text")
		SET(FONT_SYMBOL "font_data")
	ELSE()
		STRING(TOLOWER "${face}_font_data" FONT_SYMBOL)
	ENDIF()
	FILE(READ "${FONT_PATH}" FONT_HEX HEX)
	STRING(REGEX REPLACE "(..)" "0x\\1,\n" FONT_BYTES "${FONT_HEX}")
	SET(FONT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FONT_SYMBOL}.cpp")
	FILE(WRITE "${FONT_SOURCE}"
		"#include <cstddef>\nnamespace vangers::actint {\nextern const unsigned char ${FONT_SYMBOL}[] = {\n${FONT_BYTES}};\nextern const std::size_t ${FONT_SYMBOL}_size = sizeof(${FONT_SYMBOL});\n}\n")
	LIST(APPEND EMBEDDED_FONT_SOURCES "${FONT_SOURCE}")
ENDFOREACH()
ADD_LIBRARY(vangers_ttf STATIC actint/ttf_font.cpp ${EMBEDDED_FONT_SOURCES})
TARGET_LINK_LIBRARIES(vangers_ttf PUBLIC xgraph_text PRIVATE vangers_settings SDL3_ttf::SDL3_ttf)
TARGET_INCLUDE_DIRECTORIES(vangers_ttf PUBLIC "${PROJECT_SOURCE_DIR}/src")

ADD_LIBRARY(vangers_iscreen_text STATIC iscreen/text.cpp iscreen/relief_font.cpp)
TARGET_LINK_LIBRARIES(vangers_iscreen_text PUBLIC vangers_text vangers_settings vangers_ttf SDL3::SDL3)

ADD_LIBRARY(vangers_descriptions STATIC localization/description.cpp)
TARGET_LINK_LIBRARIES(vangers_descriptions PUBLIC vangers_ttf vangers_text)

SET(vangers_SRCS
	3d/3d_math.cpp
	3d/3dgraph.cpp
	3d/3dobject.cpp
	3d/dynamics.cpp
	3d/optimize.cpp
	actint/aci_scr.cpp
	actint/actint.cpp
	actint/actintml.cpp
	actint/ascr_fnc.cpp
	actint/ascript.cpp
	actint/prompt_text.cpp
	actint/chtree.cpp
	actint/sscript.cpp
	actint/text.cpp
	backg.cpp
	dast/poly3d.cpp
	iscreen/controls.cpp
	iscreen/hfont.cpp
	iscreen/i_chat.cpp
	iscreen/iextern.cpp
	iscreen/ikeys.cpp
	iscreen/imap_fnc.cpp
	iscreen/iml.cpp
	iscreen/iscr_fnc.cpp
	iscreen/iscreen.cpp
	iscreen/iscript.cpp
	iscreen/ivmap.cpp
	iscreen/script.cpp
	iscreen/settings_adapter.cpp
	iscreen/smooth.cpp
	network.cpp
	palette.cpp
	particle/bmp2scr.cpp
	particle/deform.cpp
	particle/df.cpp
	particle/light.cpp
	particle/particle.cpp
	particle/partmap.cpp
	particle/pmaptype.cpp
	rle.cpp
	road.cpp
	sound/music.cpp
	sound/sound.cpp
	#sqint/sqibody.cpp #FOR surmap
	#sqint/sqicore.cpp #FOR surmap
	terra/compress.cpp
	terra/huff1.cpp
	terra/land.cpp
	terra/perpslop.cpp
	terra/siderend.cpp #ASM
	terra/slopskip.cpp
	terra/splay.cpp
	terra/vmap.cpp #ASM
	units/effect.cpp
	units/hobj.cpp
	units/items.cpp
	units/mechos.cpp
	units/moveland.cpp
	units/sensor.cpp
	units/track.cpp
	uvs/diagen.cpp
	uvs/screen.cpp
	uvs/univang.cpp
	xgamepad.cpp
	xsocket.cpp
	zmod_client.cpp
	${WINDOWS_RES}
        actint/layout.h)

# Compile resource readers separately so tools/tests can select their source mode.
# Reuse the remaining production objects in both tools and integration tests.
SET(vangers_SCRIPT_SRCS
	actint/ascr_fnc.cpp
	actint/ascript.cpp
	actint/sscript.cpp
	iscreen/iscr_fnc.cpp
	iscreen/iscript.cpp
	iscreen/script.cpp
	road.cpp
	uvs/diagen.cpp
)

if (ANDROID)
	ADD_LIBRARY(vangers STATIC ${vangers_SRCS}
		$<TARGET_OBJECTS:xtool>
		$<TARGET_OBJECTS:xgraph>
		$<TARGET_OBJECTS:xsound>
	)
else()
	SET(vangers_COMMON_SRCS ${vangers_SRCS})
	LIST(REMOVE_ITEM vangers_COMMON_SRCS ${vangers_SCRIPT_SRCS} ${WINDOWS_RES})
	ADD_LIBRARY(vangers_common OBJECT ${vangers_COMMON_SRCS})
	# Link the SDL entry point directly: MinGW does not pull WinMain from xtool's
	# static archive. Integration tests keep their own main and share only runtime code.
	ADD_EXECUTABLE(vangers
		${vangers_SCRIPT_SRCS}
		../lib/xtool/xtmain.cpp
		${WINDOWS_RES}
		$<TARGET_OBJECTS:vangers_common>
	)

	IF(WIN32)
		SET(WIN_LIB
			wsock32
			#-mwindows
			#-Wl,-subsystem,windows
		)
	ENDIF(WIN32)

endif(ANDROID)

IF(BINARY_SCRIPT)
	TARGET_COMPILE_DEFINITIONS(vangers PRIVATE _BINARY_SCRIPT_)
ENDIF()

function(CONFIGURE_VANGERS_TARGET target_name)
	TARGET_LINK_LIBRARIES(${target_name} PRIVATE
		vangers_localization
		vangers_descriptions
		vangers_save_text
		vangers_iscreen_text
		vangers_ttf
		vangers_text
		vangers_settings
		SDL3::SDL3
		SDL3_net::SDL3_net
		Clunk::Clunk
		${OGG_LIBRARY}
		${VORBIS_LIBRARY}
		${VORBISFILE_LIBRARY}
		${FFMPEG_LIBRARIES}
		${ZLIB_LIBRARIES}
		${OPENGL_LIBRARIES}
		${WIN_LIB}
		${STEAM_LIBS}
	)

	if (NOT ANDROID)
		TARGET_LINK_LIBRARIES(${target_name} PRIVATE
			xtool
			xgraph
			xsound
		)
	endif()
endfunction()

CONFIGURE_VANGERS_TARGET(vangers)

if (NOT ANDROID)
	CONFIGURE_VANGERS_TARGET(vangers_common)
	# Run even when no C++ source changed: catalogs are editable runtime files.
	ADD_CUSTOM_TARGET(vangers_locale_files
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${PROJECT_SOURCE_DIR}/data/locales" "$<TARGET_FILE_DIR:vangers>/locales"
		VERBATIM)
	ADD_DEPENDENCIES(vangers vangers_locale_files)
endif()

if (NOT ANDROID)
	ADD_EXECUTABLE(vangers_iscreen_compiler EXCLUDE_FROM_ALL
		${vangers_SCRIPT_SRCS}
		../lib/xtool/xtmain.cpp
		${WINDOWS_RES}
		$<TARGET_OBJECTS:vangers_common>
		iscreen/script_compiler.cpp
	)
	TARGET_COMPILE_DEFINITIONS(vangers_iscreen_compiler PRIVATE ISCREEN_SCRIPT_COMPILER)
	CONFIGURE_VANGERS_TARGET(vangers_iscreen_compiler)

	IF(BUILD_TESTING)
		FOREACH(script_mode source binary)
			ADD_EXECUTABLE(actint_script_${script_mode}_test
				${vangers_SCRIPT_SRCS}
				$<TARGET_OBJECTS:vangers_common>
				../tests/actint_script_test.cpp
				../tests/actint_render_test.cpp
				../lib/xtool/xerrhead.cpp
			)
			TARGET_COMPILE_DEFINITIONS(actint_script_${script_mode}_test PRIVATE
				SDL_ShowSimpleMessageBox=VangersTest_ShowSimpleMessageBox
			)
			IF(script_mode STREQUAL "binary")
				TARGET_COMPILE_DEFINITIONS(actint_script_${script_mode}_test PRIVATE _BINARY_SCRIPT_)
			ENDIF()
			CONFIGURE_VANGERS_TARGET(actint_script_${script_mode}_test)
			ADD_EXECUTABLE(iscreen_${script_mode}_test
				${vangers_SCRIPT_SRCS}
				$<TARGET_OBJECTS:vangers_common>
				../tests/iscreen_runtime_test.cpp
				../lib/xtool/xerrhead.cpp
			)
			TARGET_COMPILE_DEFINITIONS(iscreen_${script_mode}_test PRIVATE SDL_ShowSimpleMessageBox=VangersTest_ShowSimpleMessageBox)
			IF(script_mode STREQUAL "binary")
				TARGET_COMPILE_DEFINITIONS(iscreen_${script_mode}_test PRIVATE _BINARY_SCRIPT_)
			ENDIF()
			CONFIGURE_VANGERS_TARGET(iscreen_${script_mode}_test)
			ADD_EXECUTABLE(acs_${script_mode}_test
				${vangers_SCRIPT_SRCS}
				$<TARGET_OBJECTS:vangers_common>
				../tests/acs_runtime_test.cpp
				../lib/xtool/xerrhead.cpp)
			TARGET_COMPILE_DEFINITIONS(acs_${script_mode}_test PRIVATE SDL_ShowSimpleMessageBox=VangersTest_ShowSimpleMessageBox)
			IF(script_mode STREQUAL "binary")
				TARGET_COMPILE_DEFINITIONS(acs_${script_mode}_test PRIVATE _BINARY_SCRIPT_)
			ENDIF()
			CONFIGURE_VANGERS_TARGET(acs_${script_mode}_test)
		ENDFOREACH()
		FOREACH(mode runtime source)
			ADD_EXECUTABLE(diagen_${mode}_test
				${vangers_SCRIPT_SRCS}
				$<TARGET_OBJECTS:vangers_common>
				../tests/diagen_runtime_test.cpp
				../lib/xtool/xerrhead.cpp)
			TARGET_COMPILE_DEFINITIONS(diagen_${mode}_test PRIVATE _BINARY_SCRIPT_
				SDL_ShowSimpleMessageBox=VangersTest_ShowSimpleMessageBox)
			IF(mode STREQUAL "source")
				TARGET_COMPILE_DEFINITIONS(diagen_${mode}_test PRIVATE DIAGEN_TEST)
			ENDIF()
			CONFIGURE_VANGERS_TARGET(diagen_${mode}_test)
		ENDFOREACH()
	ENDIF()

	ADD_CUSTOM_TARGET(compile_iscreen_scripts
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-iscreen
			iscreen/oftr.scr resource/iscreen/oftr.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-iscreen
			iscreen/oftr2.scr resource/iscreen/oftr2.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-actint
			actint/aci_low.scr resource/actint/aci_low.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-actint
			actint/aci_low2.scr resource/actint/aci_low2.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-actint
			actint/aci_hi.scr resource/actint/aci_hi.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-actint
			actint/aci_hi2.scr resource/actint/aci_hi2.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-acs
			actint/acs_low.scr resource/actint/acs_low.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-acs
			actint/acs_low2.scr resource/actint/acs_low2.scb
		DEPENDS vangers_iscreen_compiler
		COMMENT "Compiling English and Russian interface scripts"
		USES_TERMINAL
		VERBATIM
	)
	# Keep the historical target name working for existing packaging scripts.
	ADD_CUSTOM_TARGET(compile_interface_scripts DEPENDS compile_iscreen_scripts)
endif()
