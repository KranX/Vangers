# Locate SDL_ffmpeg library
# This module defines
# SDLFFMPEG_LIBRARY, the name of the library to link against
# SDLFFMPEG_FOUND, if false, do not try to link to SDL
# SDLFFMPEG_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Based on FindSDL_ttf.cmake from cmake distribution, original
# copyright note:
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake 
# module, but with modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc).

FIND_PATH(SDLFFMPEG_INCLUDE_DIR SDL_ffmpeg.h
  HINTS
  $ENV{SDLFFMPEGDIR}
  $ENV{SDLDIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL
  /usr/include/SDL
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL # Fink
  /sw/include
  /opt/local/include/SDL # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL # Blastwave
  /opt/csw/include 
  /opt/include/SDL
  /opt/include
)

FIND_LIBRARY(SDLFFMPEG_LIBRARY 
  NAMES SDL_ffmpeg
  HINTS
  $ENV{SDLFFMPEGDIR}
  $ENV{SDLDIR}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
    PATH_SUFFIXES lib64 lib
)

SET(SDLFFMPEG_FOUND "NO")
IF(SDLFFMPEG_LIBRARY AND SDLFFMPEG_INCLUDE_DIR)
  SET(SDLFFMPEG_FOUND "YES")
ENDIF(SDLFFMPEG_LIBRARY AND SDLFFMPEG_INCLUDE_DIR)

