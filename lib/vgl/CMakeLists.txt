SET(vgl_SRCS
		shader.cpp
		shader.h
		base.h
		buffer.h
		itexture.h
		uniform.h
		uniform_ext.h
		util.cpp
		util.h
		vec.h
		vertex_array.h
		camera.cpp
		camera.h
		texture_ext.cpp
		texture_ext.h
		uniform_ext.cpp pipeline.cpp pipeline.h uniform_attrib.h vec.cpp)

ADD_LIBRARY(vgl STATIC ${vgl_SRCS})

TARGET_LINK_LIBRARIES(vgl
		${SDL2_LIBRARY}
		${OPENGL_LIBRARIES}
		)
