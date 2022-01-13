
# for opengl or something
set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)


set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required(VERSION 3.4.1)
set(APP_NAME "MyApp")
project(${APP_NAME})


include_directories(src)
set(${APP_NAME}_SOURCES src/main.cpp)
add_subdirectory("../koala/lib/mzgl" "mzgl-build" )
add_compile_definitions(GL_SILENCE_DEPRECATION MZGL_GL2 MZGL_MAC_GLFW __MACOSX_CORE__)	
add_executable(${APP_NAME} ${${APP_NAME}_SOURCES})


set_target_properties( ${APP_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY    ${PROJECT_SOURCE_DIR}/bin
)

add_dependencies(${APP_NAME} libmzgl)


set(LIBS
	libmzgl
	zipper
	pugixml
	RtMidi
	glfw
	boostfs
	yoga
	portaudio_static
	z
)

target_link_libraries(${APP_NAME} "-framework OpenGL -framework Accelerate -framework AppKit -framework CoreMidi -framework AVFoundation -framework CoreMedia")
	
target_link_libraries (${APP_NAME} ${LIBS})
