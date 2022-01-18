SET(glad_SRCS
	src/glad.c
	include/glad/glad.h
)


ADD_LIBRARY(glad STATIC ${glad_SRCS})

include_directories(
	"${PROJECT_SOURCE_DIR}/lib/glad/include"
)

target_include_directories( 
	glad 
	INTERFACE "${PROJECT_SOURCE_DIR}/lib/glad/include"
)

