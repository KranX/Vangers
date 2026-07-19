include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
	"${PROJECT_SOURCE_DIR}/lib/xsound"
)

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
	actint/chtree.cpp
	actint/sscript.cpp
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

if (ANDROID)
	ADD_LIBRARY(vangers STATIC ${vangers_SRCS}
		$<TARGET_OBJECTS:xtool>
		$<TARGET_OBJECTS:xgraph>
		$<TARGET_OBJECTS:xsound>
	)
else()
	ADD_EXECUTABLE(vangers ${vangers_SRCS})

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
	ADD_EXECUTABLE(vangers_iscreen_compiler EXCLUDE_FROM_ALL
		${vangers_SRCS}
		iscreen/script_compiler.cpp
	)
	TARGET_COMPILE_DEFINITIONS(vangers_iscreen_compiler PRIVATE ISCREEN_SCRIPT_COMPILER)
	CONFIGURE_VANGERS_TARGET(vangers_iscreen_compiler)

	ADD_CUSTOM_TARGET(compile_iscreen_scripts
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-iscreen
			iscreen/oftr.scr resource/iscreen/oftr.scb
		COMMAND ${CMAKE_COMMAND} -E chdir "${PROJECT_SOURCE_DIR}/data"
			"$<TARGET_FILE:vangers_iscreen_compiler>" --compile-iscreen
			iscreen/oftr2.scr resource/iscreen/oftr2.scb
		DEPENDS vangers_iscreen_compiler
		COMMENT "Compiling English and Russian interface scripts"
		USES_TERMINAL
		VERBATIM
	)
endif()
