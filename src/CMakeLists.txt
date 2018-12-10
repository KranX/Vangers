include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
	"${PROJECT_SOURCE_DIR}/lib/xsound"
	"${PROJECT_SOURCE_DIR}/lib/vgl"
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
	xjoystick.cpp
	xsocket.cpp
	zmod_client.cpp
	cli_options.cpp
	cli_options.h
	util/ring_buffer.h
	util/Timer.cpp
	util/Timer.h
	util/TimerStorage.cpp
	util/TimerStorage.h
	util/PerfWidget.cpp
	util/PerfWidget.h
	terra/vmaprenderer.cpp
	terra/vmaprenderer.h
	${WINDOWS_RES}
)

SET(VANGERS_ICON vangers.icns)
SET(VANGERS_ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../osx/Resource/${VANGERS_ICON})

ADD_EXECUTABLE(vangers MACOSX_BUNDLE ${vangers_SRCS} ${VANGERS_ICON_PATH})

IF(WIN32)
	SET(WIN_LIB 
		wsock32
		#-mwindows
		#-Wl,-subsystem,windows
	)
ENDIF(WIN32)

IF(APPLE)
	SET(MACOSX_BUNDLE_ICON_FILE ${VANGERS_ICON})
	SET_SOURCE_FILES_PROPERTIES(${VANGERS_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	ADD_CUSTOM_COMMAND(TARGET vangers POST_BUILD
			COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../osx/third_party_libs_tool/third_party_libs_tool "$<TARGET_FILE_DIR:vangers>/../..")
	ADD_CUSTOM_COMMAND(TARGET vangers POST_BUILD
			COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../osx/wrap_with_shell.sh "$<TARGET_FILE_DIR:vangers>/vangers" "${CMAKE_CURRENT_SOURCE_DIR}/../osx/MacOS/vangers")

ENDIF(APPLE)

TARGET_LINK_LIBRARIES(vangers
	${SDL2_LIBRARY}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
	${CLUNK_LIBRARY}
	${SDL2_NET_LIBRARY}
		${SDL2_IMAGE_LIBRARY}
	${FFMPEG_LIBRARIES}
	${ZLIB_LIBRARIES}
	${OPENGL_LIBRARIES}
		${GLEW_LIBRARIES}
		${GLM_LIBRARIES}
	${WIN_LIB}
	xtool
	xgraph
	xsound
	vgl
)
