# - Find SDL2_net library and headers
# 
# Find module for SDL_net 2.0 (http://www.libsdl.org/projects/SDL_net/).
# It defines the following variables:
#  SDL2_NET_INCLUDE_DIRS - The location of the headers, e.g., SDL_net.h.
#  SDL2_NET_LIBRARIES - The libraries to link against to use SDL2_net.
#  SDL2_NET_FOUND - If false, do not try to use SDL2_net.
#  SDL2_NET_VERSION_STRING
#    Human-readable string containing the version of SDL2_net.
#
# Also defined, but not for general use are:
#   SDL2_NET_INCLUDE_DIR - The directory that contains SDL_net.h.
#   SDL2_NET_LIBRARY - The location of the SDL2_net library.
#

#=============================================================================
# Copyright 2013 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_package(PkgConfig QUIET)
pkg_check_modules(PC_SDL2_NET QUIET SDL2_net)

find_path(SDL2_NET_INCLUDE_DIR
  NAMES SDL_net.h
  HINTS
    ${PC_SDL2_NET_INCLUDEDIR}
    ${PC_SDL2_NET_INCLUDE_DIRS}
    /Library/Frameworks
  PATH_SUFFIXES SDL2
)

MESSAGE("SDL2_NET_INCLUDE_DIR is ${SDL2_NET_INCLUDE_DIR}")

find_library(SDL2_NET_LIBRARY
  NAMES SDL2_net
  HINTS
    ${PC_SDL2_NET_LIBDIR}
    ${PC_SDL2_NET_LIBRARY_DIRS}
    /Library/Frameworks
  PATH_SUFFIXES x64 x86
)

if(SDL2_NET_INCLUDE_DIR AND EXISTS "${SDL2_NET_INCLUDE_DIR}/SDL_net.h")
  file(STRINGS "${SDL2_NET_INCLUDE_DIR}/SDL_net.h" SDL2_NET_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_NET_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_NET_INCLUDE_DIR}/SDL_net.h" SDL2_NET_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_NET_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_NET_INCLUDE_DIR}/SDL_net.h" SDL2_NET_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_NET_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL_NET_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_NET_VERSION_MAJOR "${SDL2_NET_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_NET_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_NET_VERSION_MINOR "${SDL2_NET_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL_NET_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_NET_VERSION_PATCH "${SDL2_NET_VERSION_PATCH_LINE}")
  set(SDL2_NET_VERSION_STRING ${SDL2_NET_VERSION_MAJOR}.${SDL2_NET_VERSION_MINOR}.${SDL2_NET_VERSION_PATCH})
  unset(SDL2_NET_VERSION_MAJOR_LINE)
  unset(SDL2_NET_VERSION_MINOR_LINE)
  unset(SDL2_NET_VERSION_PATCH_LINE)
  unset(SDL2_NET_VERSION_MAJOR)
  unset(SDL2_NET_VERSION_MINOR)
  unset(SDL2_NET_VERSION_PATCH)
endif()

set(SDL2_NET_INCLUDE_DIRS ${SDL2_NET_INCLUDE_DIR})
set(SDL2_NET_LIBRARIES ${SDL2_NET_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2_net
                                  REQUIRED_VARS SDL2_NET_INCLUDE_DIRS SDL2_NET_LIBRARIES
                                  VERSION_VAR SDL2_NET_VERSION_STRING)

mark_as_advanced(SDL2_NET_INCLUDE_DIR SDL2_NET_LIBRARY)
