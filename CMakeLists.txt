SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

# root
PROJECT(vangers)

# modules
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# options
OPTION(BINARY_SCRIPT "Use binary script for interface" ON)
OPTION(DIAGEN_TEST "Dialog generator and test subsystem" OFF)

# hide optons
OPTION(ACTINT "Actint user interface in road game" ON) 

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")
IF(MINGW)
		SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
ENDIF()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# depends
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_net REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)

FIND_PACKAGE(Clunk REQUIRED)

FIND_PACKAGE(FFMPEG REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
set(WINDOWS_RES "../vangers.res")

IF(BINARY_SCRIPT)
	ADD_DEFINITIONS(-D_BINARY_SCRIPT_)
ENDIF(BINARY_SCRIPT)

IF(ACTINT)
	ADD_DEFINITIONS(-DACTINT)
ENDIF(ACTINT)

IF(DIAGEN_TEST)
	ADD_DEFINITIONS(-DDIAGEN_TEST)
ENDIF(DIAGEN_TEST)

# globals
INCLUDE_DIRECTORIES(SYSTEM
	${ZLIB_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2_NET_INCLUDE_DIRS}
	${FFMPEG_INCLUDE_DIRS}
	${CLUNK_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${STEAM_INCLUDE_DIR}
)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-source-encoding -O2 -msse2 -ftree-vectorize -ffast-math")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -Werror=delete-non-virtual-dtor")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=parentheses")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=class-memaccess")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=writable-strings")
endif()

if(SUPPRESS_DEPRECATED_COPY_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-copy")
endif()

if(ENABLE_ADDITIONAL_CHECKS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=tautological-compare -Werror=misleading-indentation")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=implicit-fallthrough -Werror=int-in-bool-context")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=sequence-point -Werror=unused-value")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=unused-but-set-variable")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=unsequenced -Werror=unused-comparison")
    endif()
endif()

# For modern gcc
add_definitions(-D_USE_MATH_DEFINES)

# subdirs
ADD_SUBDIRECTORY("src")
ADD_SUBDIRECTORY("lib/xgraph")
ADD_SUBDIRECTORY("lib/xsound")
ADD_SUBDIRECTORY("lib/xtool")
ADD_SUBDIRECTORY("lib/utils")
ADD_SUBDIRECTORY("server")
