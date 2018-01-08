FIND_PATH(CLUNK_INCLUDE_DIR
  NAMES
    clunk.h
  PATHS
	/usr/include
    /usr/include/x86_64-linux-gnu
    /usr/local/include
    /local/include
    /mingw/include
    /opt/local/include
    /opt/include
    /sw/include
  PATH_SUFFIXES
    clunk
)

FIND_LIBRARY(CLUNK_LIBRARY
  NAMES
    clunk
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
    /local/lib
    /mingw/lib
	/local/bin
	/mingw/bin
	/usr/local/lib
)
IF(CLUNK_INCLUDE_DIR AND CLUNK_LIBRARY)
   SET(CLUNK_FOUND TRUE)
ENDIF(CLUNK_INCLUDE_DIR AND CLUNK_LIBRARY)

IF(CLUNK_FOUND)
  IF(NOT CLUNK_FIND_QUIETLY)
    MESSAGE(STATUS "Found clunk: ${CLUNK_LIBRARY} ${CLUNK_INCLUDE_DIR}")
  ENDIF(NOT CLUNK_FIND_QUIETLY)
ELSE(CLUNK_FOUND)
  IF(CLUNK_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find clunk")
  ENDIF(CLUNK_FIND_REQUIRED)
ENDIF(CLUNK_FOUND)