SET(xtool_SRCS
	xtcore.cpp
	xrecorder/xrec.cpp
	port.cpp
	xerrhead.cpp
	xbuffer/xbcnvin.cpp
	xbuffer/xbcnvout.cpp
	xbuffer/xbcore.cpp
	xbuffer/xbsearch.cpp
	xmsgbuffer/xmsgbuf.cpp
	xresource/zip_resource.cpp
	xstream/xscnvin.cpp
	xstream/xscnvout.cpp
	xstream/xscore.cpp
	xstream/xsenlv.cpp
	xstream/xsgetlin.cpp
	xstream/xsinput.cpp
	xstream/xsoutput.cpp
	xstream/xsrdwr.cpp
	xstream/xsserv.cpp
	xutil/xclock.cpp
	iniparser/dictionary.c
	iniparser/iniparser.c
)

IF(DXSTREAM_DEBUG)
	ADD_DEFINITIONS(-DXSTREAM_DEBUG)
ENDIF(DXSTREAM_DEBUG)

INCLUDE_DIRECTORIES(BEFORE .)

ADD_LIBRARY(xtool STATIC ${xtool_SRCS})

TARGET_LINK_LIBRARIES(xtool ${WIN_DBG})