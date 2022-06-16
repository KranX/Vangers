include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/renderer/src"
)

SET(xgraph_SRCS
	xfont.cpp
	xgraph.cpp
	xbmp.cpp
	xside.cpp
)

if (ANDROID)
	ADD_LIBRARY(xgraph OBJECT ${xgraph_SRCS})
else()
	ADD_LIBRARY(xgraph STATIC ${xgraph_SRCS})
	TARGET_LINK_LIBRARIES(xgraph ${SDL2_LIBRARY} renderer)
endif()