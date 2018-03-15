SET(server_SRCS
	kdsplus.cpp
	win32f.cpp
	xsocket.cpp
	server.cpp
)

ADD_EXECUTABLE(vangers_server ${server_SRCS})

IF(WIN32)
	SET(WIN_LIB 
		wsock32
	)
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(vangers_server
	${SDL2_LIBRARY}
	${SDL2_NET_LIBRARY}
	xtool
)
