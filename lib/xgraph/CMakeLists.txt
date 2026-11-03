include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
)

SET(xgraph_SRCS
	xfont.cpp
	xgraph.cpp
	xprompt.cpp
	xbmp.cpp
	xside.cpp
)

ADD_LIBRARY(xgraph_text STATIC text_layer.cpp)
TARGET_INCLUDE_DIRECTORIES(xgraph_text PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
TARGET_LINK_LIBRARIES(xgraph_text PUBLIC SDL3::SDL3)

if (ANDROID)
	ADD_LIBRARY(xgraph OBJECT ${xgraph_SRCS})
else()
	ADD_LIBRARY(xgraph STATIC ${xgraph_SRCS})
endif()

TARGET_LINK_LIBRARIES(xgraph PUBLIC SDL3::SDL3 xgraph_text)

