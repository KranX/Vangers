# root
PROJECT(vangers)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# modules
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# options
OPTION(RUSSIAN_VERSION "Build russian version" ON)
OPTION(BINARY_SCRIPT "Use binary script for interface" ON)
OPTION(WITH_OPENGL "Use experimental OpenGL renderer" OFF)
OPTION(DIAGEN_TEST "Dialog generator and test subsystem" OFF)

# hide optons
OPTION(ACTINT "Actint user interface in road game" ON) 

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

# depends
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_net REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
if (NOT APPLE)
	FIND_PACKAGE(VorbisFile REQUIRED)
endif (NOT APPLE)

FIND_PACKAGE(Clunk REQUIRED)
if (MINGW)
	ADD_DEFINITIONS(-I/local/include/)
	ADD_DEFINITIONS(-L/local/lib/)
	#set(SDL_net_INCLUDE_DIR "/local/include/SDL")
	ADD_DEFINITIONS(-I/local/include/SDL2)
	#set(SDLNET_LIBRARY "/local/bin/SDL_net.dll")
	
	set(FFMPEG_INCLUDE_DIRS "/local/include/libavcodec /local/include/libavformat")
	ADD_DEFINITIONS(-I/local/include/libavcodec -I/local/include/libavformat)
	set(FFMPEG_LIBRARIES "/local/bin/avutil-52.dll" "/local/bin/avformat-54.dll" "/local/bin/avcodec-54.dll")
	set(ZLIB_LIBRARIES "/local/lib/zlib1.dll")
	
	set(CLUNK_INCLUDE_DIR "/local/include/clunk")
	ADD_DEFINITIONS(-I/local/include/clunk)
	set(CLUNK_LIBRARY "/local/lib/libclunk.dll")
	set(WINDOWS_RES "../vangers.res")
	ADD_DEFINITIONS(-Wl,-subsystem,windows)
else (MINGW)
	FIND_PACKAGE(FFMPEG REQUIRED)
	FIND_PACKAGE(ZLIB REQUIRED)
	set(WINDOWS_RES "")
endif (MINGW)



IF(RUSSIAN_VERSION)
	ADD_DEFINITIONS(-DRUSSIAN_VERSION)
ENDIF(RUSSIAN_VERSION)
IF(BINARY_SCRIPT)
	ADD_DEFINITIONS(-D_BINARY_SCRIPT_)
ENDIF(BINARY_SCRIPT)

IF(WITH_OPENGL)
	FIND_PACKAGE(OpenGL REQUIRED)
	ADD_DEFINITIONS(-DWITH_OPENGL)
ENDIF(WITH_OPENGL)

IF(ACTINT)
	ADD_DEFINITIONS(-DACTINT)
ENDIF(ACTINT)

IF(DIAGEN_TEST)
	ADD_DEFINITIONS(-DDIAGEN_TEST)
ENDIF(DIAGEN_TEST)

# globals
INCLUDE_DIRECTORIES(SYSTEM
	${SDL2_INCLUDE_DIR}
	${SDL2_NET_INCLUDE_DIR}
	${FFMPEG_INCLUDE_DIRS}
	${CLUNK_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	/usr/local/include
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

IF (NOT MINGW)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorize -ffast-math")
ENDIF (NOT MINGW)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")



# subdirs
ADD_SUBDIRECTORY("src")
ADD_SUBDIRECTORY("lib/xgraph")
ADD_SUBDIRECTORY("lib/xsound")
ADD_SUBDIRECTORY("lib/xtool")
ADD_SUBDIRECTORY("lib/utils")
