# CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# FIND_PACKAGE(ZLIB REQUIRED)

ADD_EXECUTABLE(zip_convert 
	zip_convert.cpp 
	xzip/inzip.cpp 
	xzip/outzip.cpp 
	xzip/treezip.cpp
)

#ADD_DEFINITIONS(-e SDL_main )

TARGET_LINK_LIBRARIES(zip_convert
	xtool
	xgraph
	${SDL2_LIBRARY}
	${ZLIB_LIBRARIES}
	${MINGW_SDL2_LDF}
)


