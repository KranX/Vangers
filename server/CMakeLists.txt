SET(server_SRCS
	kdsplus.cpp
	xsocket.cpp
	server.cpp
#win32f.cpp -- candidat to remove
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
