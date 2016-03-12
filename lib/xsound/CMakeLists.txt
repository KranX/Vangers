SET(xsound_SRCS
	xsound.cpp
	avi.cpp
	ogg_stream.cpp
)


ADD_LIBRARY(xsound STATIC ${xsound_SRCS})

TARGET_LINK_LIBRARIES(xsound
	${OGG_LIBRARY}
	${VORBIS_LIBRARY}
	${VORBISFILE_LIBRARY}
	${CLUNK_LIBRARY}
	${FFMPEG_LIBRARIES}
)
