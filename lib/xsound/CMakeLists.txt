include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
)

if (CLUNK_SUBMODULE)
	include_directories("${PROJECT_SOURCE_DIR}")
	include_directories("${PROJECT_SOURCE_DIR}/clunk")
endif ()

SET(xsound_SRCS
	xsound.cpp
	avi.cpp
	ogg_stream.cpp
)

ADD_LIBRARY(xsound STATIC ${xsound_SRCS})

TARGET_LINK_LIBRARIES(xsound
	${FFMPEG_LIBRARIES}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
)

if (CLUNK_SUBMODULE)
	target_link_libraries(xsound clunk)
else ()
	target_link_libraries(xsound ${CLUNK_LIBRARY})
endif ()

