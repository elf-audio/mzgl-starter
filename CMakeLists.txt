
set(APP_NAME "MyApp")

cmake_minimum_required(VERSION 3.18)
project(${APP_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

set(MZGL_ROOT "../koala/lib/mzgl")

include_directories(src)

set(SOURCES src/main.cpp)

if(WIN32)
    add_executable(${APP_NAME} WIN32 ${SOURCES})
else()
    add_executable(${APP_NAME} ${SOURCES})
endif()

add_subdirectory(${MZGL_ROOT} "mzgl-build" )

add_compile_definitions(GL_SILENCE_DEPRECATION MZGL_GL2 MZGL_MAC_GLFW __MACOSX_CORE__)	

set_target_properties( ${APP_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY    ${PROJECT_SOURCE_DIR}/bin
)

add_dependencies(${APP_NAME} libmzgl)	
target_link_libraries(${APP_NAME} libmzgl)
