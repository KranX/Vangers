INCLUDE_DIRECTORIES(AFTER
	"${PROJECT_SOURCE_DIR}/lib/xtool"
	"${PROJECT_SOURCE_DIR}/lib/xgraph"
	"${PROJECT_SOURCE_DIR}"
)

SET(surmap_SRCS
	3d_shape.cpp
	dsidernd.cpp
	impass.cpp
	siderend.cpp
	sqexp.cpp
	sqibody.cpp
	sqicore.cpp
	sqimenu.cpp
	surmap.cpp
	tools.cpp
	track.cpp
	world.cpp

	missed.cpp
)

SET_SOURCE_FILES_PROPERTIES(${surmap_SRCS}
		PROPERTIES COMPILE_FLAGS
		"-Wno-parentheses -Wno-unused-variable -Wno-unused-parameter -Wno-register -Wno-dangling-else \
		 -Werror=missing-field-initializers -Werror=uninitialized")

SET(vangers_surmap_SRCS
	"${PROJECT_SOURCE_DIR}/src/3d/3dobject.cpp"
	"${PROJECT_SOURCE_DIR}/src/3d/3d_math.cpp"
	"${PROJECT_SOURCE_DIR}/src/3d/3dgraph.cpp"
	"${PROJECT_SOURCE_DIR}/src/3d/dynamics.cpp"
	"${PROJECT_SOURCE_DIR}/src/3d/optimize.cpp"


	"${PROJECT_SOURCE_DIR}/src/terra/land.cpp"
	"${PROJECT_SOURCE_DIR}/src/terra/splay.cpp"
	"${PROJECT_SOURCE_DIR}/src/terra/vmap.cpp"
	"${PROJECT_SOURCE_DIR}/src/units/moveland.cpp"

	"${PROJECT_SOURCE_DIR}/src/palette.cpp"
	"${PROJECT_SOURCE_DIR}/src/dast/poly3d.cpp"

	"${PROJECT_SOURCE_DIR}/src/rle.cpp"
)

ADD_EXECUTABLE(surmap ${surmap_SRCS} ${vangers_surmap_SRCS})
SET_SOURCE_FILES_PROPERTIES(${vangers_surmap_SRCS}
		PROPERTIES COMPILE_FLAGS
		"-Wno-parentheses -Wno-unused-variable -Wno-unused-parameter -Wno-dangling-else")

TARGET_COMPILE_DEFINITIONS(surmap PUBLIC _SURMAP_ _SURMAP_ROUGH_)

TARGET_LINK_LIBRARIES(surmap
	${SDL2_LIBRARY}
	xtool
	xgraph
)
