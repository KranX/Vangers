ADD_EXECUTABLE(xsocket_test
	xsocket_test.cpp
	../src/xsocket.cpp
)

TARGET_INCLUDE_DIRECTORIES(xsocket_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
)

TARGET_LINK_LIBRARIES(xsocket_test PRIVATE
	SDL3::SDL3
	SDL3_net::SDL3_net
)

ADD_TEST(NAME xsocket_loopback COMMAND xsocket_test)
SET_TESTS_PROPERTIES(xsocket_loopback PROPERTIES TIMEOUT 15)

ADD_EXECUTABLE(legacy_data_compat_test
	legacy_data_compat_test.cpp
)

TARGET_INCLUDE_DIRECTORIES(legacy_data_compat_test PRIVATE
	${PROJECT_SOURCE_DIR}/src/iscreen
	${PROJECT_SOURCE_DIR}/lib/xtool
)

TARGET_COMPILE_DEFINITIONS(legacy_data_compat_test PRIVATE
	TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/data"
)

TARGET_LINK_LIBRARIES(legacy_data_compat_test PRIVATE
	SDL3::SDL3
	ZLIB::ZLIB
)

ADD_TEST(NAME legacy_data_compat COMMAND legacy_data_compat_test)

ADD_EXECUTABLE(sdl_event_buffer_test
	sdl_event_buffer_test.cpp
	../src/iscreen/ikeys.cpp
)

TARGET_INCLUDE_DIRECTORIES(sdl_event_buffer_test PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/iscreen
	${PROJECT_SOURCE_DIR}/lib/xtool
	${PROJECT_SOURCE_DIR}/lib/xgraph
)

TARGET_LINK_LIBRARIES(sdl_event_buffer_test PRIVATE SDL3::SDL3)

ADD_TEST(NAME sdl_event_buffer COMMAND sdl_event_buffer_test)
