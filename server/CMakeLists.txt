INCLUDE_DIRECTORIES(AFTER
    ${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/src)

SET(server_SRCS
    kdsplus.cpp
    server.cpp
    ../multiplayer/xsocket.cpp
    #win32f.cpp -- candidate to remove
    )

ADD_EXECUTABLE(vangers_server ${server_SRCS})

IF (WIN32)
    SET(WIN_LIB
        wsock32
        )
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(vangers_server
    ${SDL2_LIBRARY}
    ${SDL2_NET_LIBRARY}
    xtool
    xgraph
    )
