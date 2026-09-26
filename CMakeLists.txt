SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum macOS deployment version")
CMAKE_MINIMUM_REQUIRED(VERSION 4.0.0)

# root
PROJECT(vangers)

INCLUDE(CTest)

# modules
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# options
OPTION(BINARY_SCRIPT "Use binary script for interface" ON)
OPTION(DIAGEN_TEST "Dialog generator and test subsystem" OFF)

# hide optons
OPTION(ACTINT "Actint user interface in road game" ON) 

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# depends
FIND_PACKAGE(SDL3 3.2 CONFIG REQUIRED COMPONENTS SDL3)
FIND_PACKAGE(SDL3_net 3.2 CONFIG REQUIRED)
FIND_PACKAGE(toml11 4.4...<4.5 CONFIG REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
FIND_PACKAGE(VorbisFile REQUIRED)

FIND_PACKAGE(Clunk REQUIRED)

FIND_PACKAGE(FFMPEG REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
set(WINDOWS_RES "${PROJECT_SOURCE_DIR}/vangers.res")

IF(ACTINT)
	ADD_DEFINITIONS(-DACTINT)
ENDIF(ACTINT)

IF(DIAGEN_TEST)
	ADD_DEFINITIONS(-DDIAGEN_TEST)
ENDIF(DIAGEN_TEST)

# globals
INCLUDE_DIRECTORIES(SYSTEM
	${ZLIB_INCLUDE_DIR}
	${FFMPEG_INCLUDE_DIRS}
	${CLUNK_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${VORBIS_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${STEAM_INCLUDE_DIR}
)

INCLUDE(CheckCXXCompilerFlag)

# For modern MSVC
add_definitions(-D_USE_MATH_DEFINES)

IF(MSVC)
	# The sources contain UTF-8 (Russian) string literals. GCC/clang use UTF-8 as
	# the execution charset by default, so tell MSVC to do the same.
	add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>")
	# Report the real __cplusplus value instead of the legacy 199711L.
	add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>")
	# A few translation units (actint, univang) are large enough to exceed the
	# default object file section limit otherwise.
	add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/bigobj>")
	add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:/W3>")
	# Keep the POSIX CRT names (strdup, ...) and the classic CRT functions usable
	# without deprecation noise, and stop windows.h from defining min/max macros.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
	add_definitions(-DNOMINMAX)
ELSE()
	SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize -ffast-math")
	CHECK_CXX_COMPILER_FLAG("-msse2" HAVE_SIMDSSE)
	IF(HAVE_SIMDSSE)
	  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse2")
	ENDIF()

	add_compile_options(-Werror=shift-negative-value)
	add_compile_options(-Werror=dangling-else)
	add_compile_options(-Werror=parentheses)
	add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-non-virtual-dtor>)
	add_compile_options(-Werror=write-strings)
	add_compile_options(-fsigned-char)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-Werror=mismatched-new-delete )
	endif()
ENDIF()

IF(WIN32)
	SET(WIN_DBG
		dbghelp
	)
	IF(MSVC)
		# The committed vangers.res was produced by windres as a 32-bit COFF
		# object, which MSVC's x64 linker rejects (LNK1112). The icon is optional,
		# so MSVC builds without an embedded resource for now.
		SET(WINDOWS_RES "")
	ELSE()
		SET(WINDOWS_RES "${PROJECT_SOURCE_DIR}/vangers.res")
	ENDIF()
ENDIF(WIN32)

# Get the current working branch
IF(NOT($ENV{GITHUB_REF_NAME}))
	execute_process(
		COMMAND git rev-parse --abbrev-ref HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_BRANCH
		COMMAND_ECHO STDOUT
		ECHO_OUTPUT_VARIABLE
		ECHO_ERROR_VARIABLE
		OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()

# Get the latest commit hash
IF(NOT($ENV{GITHUB_SHA}))
	execute_process(
		COMMAND git rev-parse HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_COMMIT_HASH
		COMMAND_ECHO STDOUT
		ECHO_OUTPUT_VARIABLE
		ECHO_ERROR_VARIABLE
		OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()

# subdirs
ADD_SUBDIRECTORY("src")
ADD_SUBDIRECTORY("lib/xgraph")
ADD_SUBDIRECTORY("lib/xsound")
ADD_SUBDIRECTORY("lib/xtool")

if(NOT ANDROID)
	ADD_SUBDIRECTORY("lib/utils")
	ADD_SUBDIRECTORY("server")
	ADD_SUBDIRECTORY("surmap")
	IF(BUILD_TESTING)
		ADD_SUBDIRECTORY("tests")
	ENDIF()
endif()
