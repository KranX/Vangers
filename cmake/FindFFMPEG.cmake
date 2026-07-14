# Locate the FFmpeg libraries
# This module defines
# {AVUTIL,AVCODEC,AVFORMAT,SWSCALE}_INCLUDE_DIR
# {AVUTIL,AVCODEC,AVFORMAT,SWSCALE}_LIBRARY
# FFMPEG_INLUDE_DIRS
# FFMPEG_LIBRARIES
#

FIND_PATH(AVUTIL_INCLUDE_DIR
	NAMES
		avutil.h
	PATHS
		/usr/local/include
		/usr/pkg/include/ffmpeg3/libavutil
		/usr/include
		/usr/include/x86_64-linux-gnu
		/usr/include/aarch64-linux-gnu
		/usr/include/aarch64-linux-gnu/libavutil
		/local/include
		/mingw/include
		/opt/local/include
		/opt/include
		/sw/include
		/usr/include/libavutil
		/usr/include/ffmpeg
		/usr/include/ffmpeg/libavutil
	PATH_SUFFIXES
		libavutil
		ffmpeg
)

FIND_PATH(AVCODEC_INCLUDE_DIR
	NAMES
		avcodec.h
	PATHS
		/usr/local/include
		/usr/pkg/include/ffmpeg3/libavcodec
		/usr/include
		/usr/include/x86_64-linux-gnu
		/usr/include/aarch64-linux-gnu
		/usr/include/aarch64-linux-gnu/libavcodec
		/opt/local/include
		/local/include
		/mingw/include
		/opt/include
		/sw/include
		/usr/include/libavcodec
		/usr/include/ffmpeg
		/usr/include/ffmpeg/libavcodec
	PATH_SUFFIXES
		libavcodec
		ffmpeg
)

FIND_PATH(AVFORMAT_INCLUDE_DIR
	NAMES
		avformat.h
	PATHS
		/usr/local/include
		/usr/pkg/include/ffmpeg3/libavformat
		/usr/include
		/usr/include/x86_64-linux-gnu
		/usr/include/aarch64-linux-gnu
		/usr/include/aarch64-linux-gnu/libavformat
		/opt/local/include
		/local/include
		/mingw/include
		/opt/include
		/sw/include
		/usr/include/libavformat
		/usr/include/ffmpeg
		/usr/include/ffmpeg/libavformat
	PATH_SUFFIXES
		libavformat
		ffmpeg
)

FIND_PATH(SWSCALE_INCLUDE_DIR
	NAMES
		swscale.h
	PATHS
		/usr/local/include
		/usr/pkg/include/ffmpeg3/libswscale
		/usr/include
		/usr/include/x86_64-linux-gnu
		/usr/include/aarch64-linux-gnu
		/opt/local/include
		/local/include
		/mingw/include
		/opt/include
		/sw/include
		/usr/include/libswscale
		/usr/include/ffmpeg
		/usr/include/ffmpeg/libswscale
	PATH_SUFFIXES
		libswscale
		ffmpeg
)

FIND_LIBRARY(AVUTIL_LIBRARY
	NAMES
		avutil
		avutil-55
		avutil-56
		avutil-57
		avutil-58
		avutil-59
		avutil-60
		avutil-61
	PATHS
		/usr/local/lib
		/usr/pkg/lib/ffmpeg3
		/usr/lib
		/usr/lib/x86_64-linux-gnu
		/usr/lib/aarch64-linux-gnu
		/usr/lib/ffmpeg
		/opt/local/lib
		/sw/lib
		/local/lib
		/mingw/lib
		/local/bin
		/mingw/bin
		/bin
)

FIND_LIBRARY(AVCODEC_LIBRARY
	NAMES
		avcodec
		avcodec-57
		avcodec-58
		avcodec-59
		avcodec-60
		avcodec-61
		avcodec-62
		avcodec-63
	PATHS
		/usr/local/lib
		/usr/pkg/lib/ffmpeg3
		/usr/lib
		/usr/lib/x86_64-linux-gnu
		/usr/lib/aarch64-linux-gnu
		/opt/local/lib
		/sw/lib
		/local/lib
		/mingw/lib
		/local/bin
		/mingw/bin
		/bin
)

FIND_LIBRARY(AVFORMAT_LIBRARY
	NAMES
		avformat
		avformat-57
		avformat-58
		avformat-59
		avformat-60
		avformat-61
		avformat-62
		avformat-63
	PATHS
		/usr/local/lib
		/usr/pkg/lib/ffmpeg3
		/usr/lib
		/usr/lib/x86_64-linux-gnu
		/usr/lib/aarch64-linux-gnu
		/usr/lib/ffmpeg
		/opt/local/lib
		/sw/lib
		/local/lib
		/local/bin
		/mingw/bin
		/mingw/lib
		/bin
)

FIND_LIBRARY(SWSCALE_LIBRARY
	NAMES
		swscale
		swscale-4
		swscale-5
		swscale-6
		swscale-7
		swscale-8
		swscale-9
		swscale-10
	PATHS
		/usr/local/lib
		/usr/pkg/lib/ffmpeg3
		/usr/lib
		/usr/lib/x86_64-linux-gnu
		/usr/lib/aarch64-linux-gnu
		/usr/lib/ffmpeg
		/opt/local/lib
		/sw/lib
		/local/lib
		/local/bin
		/mingw/bin
		/mingw/lib
		/bin
)

get_filename_component(FFMPEG_PARENT_DIR ${AVCODEC_INCLUDE_DIR} DIRECTORY)

SET(FFMPEG_INCLUDE_DIRS
	#${AVUTIL_INCLUDE_DIR}
	${AVCODEC_INCLUDE_DIR}
	${AVFORMAT_INCLUDE_DIR}
	${SWSCALE_INCLUDE_DIR}
	${FFMPEG_PARENT_DIR}
)

SET(FFMPEG_LIBRARIES)

IF(AVUTIL_LIBRARY)
	SET(FFMPEG_LIBRARIES
		${FFMPEG_LIBRARIES}
		${AVUTIL_LIBRARY}
	)
ENDIF(AVUTIL_LIBRARY)

IF(AVCODEC_LIBRARY)
	SET(FFMPEG_LIBRARIES
		${FFMPEG_LIBRARIES}
		${AVCODEC_LIBRARY}
	)
ENDIF(AVCODEC_LIBRARY)

IF(AVFORMAT_LIBRARY)
	SET(FFMPEG_LIBRARIES
		${FFMPEG_LIBRARIES}
		${AVFORMAT_LIBRARY}
	)
ENDIF(AVFORMAT_LIBRARY)

IF(SWSCALE_LIBRARY)
	SET(FFMPEG_LIBRARIES
		${FFMPEG_LIBRARIES}
		${SWSCALE_LIBRARY}
	)
ENDIF(SWSCALE_LIBRARY)

IF(FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES AND SWSCALE_INCLUDE_DIR AND SWSCALE_LIBRARY)
	SET(FFMPEG_FOUND TRUE)
ENDIF(FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES AND SWSCALE_INCLUDE_DIR AND SWSCALE_LIBRARY)

IF(FFMPEG_FOUND)
	IF(NOT FFMPEG_FIND_QUIETLY)
		MESSAGE(STATUS "Found FFmpeg: ${FFMPEG_LIBRARIES}")
	ENDIF(NOT FFMPEG_FIND_QUIETLY)
ELSE(FFMPEG_FOUND)
	IF(FFMPEG_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find FFmpeg")
	ENDIF(FFMPEG_FIND_REQUIRED)
ENDIF(FFMPEG_FOUND)
