SET(xgraph_SRCS
	xfont.cpp
	xgraph.cpp
)

ADD_LIBRARY(xgraph STATIC ${xgraph_SRCS})

TARGET_LINK_LIBRARIES(xgraph
	${SDL2_LIBRARY}
)