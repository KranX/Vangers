SET(renderer_SRCS
	src/renderer/ResourceStorage.h
	src/renderer/ResourceId.h
	src/renderer/common.h
	src/renderer/common.cpp
	src/renderer/vectormath.cpp
	src/renderer/vectormath.h
	src/renderer/compositor/AbstractCompositor.h
	src/renderer/compositor/CompositorException.cpp
	src/renderer/compositor/CompositorException.h
	src/renderer/compositor/gles3/GLES3Compositor.cpp
	src/renderer/compositor/gles3/GLES3Compositor.h
	src/renderer/compositor/gles3/Shader.cpp
	src/renderer/compositor/gles3/Shader.h
	src/renderer/compositor/gles3/QuadVertexArray.cpp
	src/renderer/compositor/gles3/QuadVertexArray.h
	src/renderer/compositor/gles3/GLES3Texture.cpp
	src/renderer/compositor/gles3/GLES3Texture.h
	src/renderer/compositor/sdl_ext/SDL_extensions.cpp
	src/renderer/compositor/sdl_ext/SDL_extensions.h
	src/renderer/visualbackend/AbstractVisualBackend.h
	src/renderer/visualbackend/AbstractVisualBackend.cpp
	src/renderer/visualbackend/VisualBackendContext.cpp
	src/renderer/visualbackend/VisualBackendContext.h
	src/renderer/visualbackend/dummy/DummyVisualBackend.cpp
	src/renderer/visualbackend/dummy/DummyVisualBackend.h
	src/renderer/visualbackend/rust/RustVisualBackend.cpp
	src/renderer/visualbackend/rust/RustVisualBackend.h
	src/renderer/visualbackend/rust/vange_rs.h
	)


ADD_LIBRARY(renderer STATIC ${renderer_SRCS})

TARGET_INCLUDE_DIRECTORIES(
	renderer 
	INTERFACE "${PROJECT_SOURCE_DIR}/lib/glad/include"
)

SET(RENDERER_LIBS
	${RUSTYVANGERS_LIBRARY}
	glad
	)

IF(UNIX)
    SET(RENDERER_LIBS
		${RENDERER_LIBS}
		dl
		pthread
		)
ENDIF(UNIX)

IF(MINGW)
SET(RENDERER_LIBS
	${RENDERER_LIBS}
	userenv
	bcrypt
		d3d12
		d3dcompiler
			ws2_32
			)
ENDIF(MINGW)

IF(APPLE)
SET(RENDERER_LIBS
		${RENDERER_LIBS}
		"-framework Metal"
		"-framework QuartzCore"
		)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES(
	renderer
	${RENDERER_LIBS}
)
