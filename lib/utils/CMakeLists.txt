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
	${ZLIB_LIBRARIES}
	xtool
	xgraph
	${SDL2_LIBRARY}
)


