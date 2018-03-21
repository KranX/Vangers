include_directories(
	"${PROJECT_SOURCE_DIR}/lib/xtool"
)

SET(xgraph_SRCS
	xfont.cpp
	xgraph.cpp
		gl/Camera.cpp
		gl/Camera.h
		gl/SmartShader.cpp
		gl/SmartShader.h
		gl/Texture.cpp
		gl/Texture.h
		gl/common.h
		gl/common.cpp
		gl/Shader.h
		gl/Shader.cpp
		gl/PlainTextureShader.cpp
		gl/PlainTextureShader.h
)

ADD_LIBRARY(xgraph STATIC ${xgraph_SRCS})

TARGET_LINK_LIBRARIES(xgraph
	${SDL2_LIBRARY}
		${OPENGL_LIBRARIES}
)
