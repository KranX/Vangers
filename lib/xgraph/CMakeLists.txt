include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/src"
)

SET(xgraph_SRCS
	xfont.cpp
	xgraph.cpp
	xbmp.cpp
	xside.cpp
	$<TARGET_OBJECTS:textcore>
)

if (ANDROID)
	ADD_LIBRARY(xgraph OBJECT ${xgraph_SRCS})
else()
	ADD_LIBRARY(xgraph STATIC ${xgraph_SRCS})
	TARGET_LINK_LIBRARIES(xgraph PUBLIC ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES})
endif()

