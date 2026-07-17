include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
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
endif()

TARGET_LINK_LIBRARIES(xgraph PUBLIC SDL3::SDL3)

