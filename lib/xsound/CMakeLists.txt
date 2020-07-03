include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
)

SET(xsound_SRCS
	xsound.cpp
	avi.cpp
	ogg_stream.cpp
)

ADD_LIBRARY(xsound STATIC ${xsound_SRCS})

TARGET_LINK_LIBRARIES(xsound
	${CLUNK_LIBRARY}
	${FFMPEG_LIBRARIES}
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
)
