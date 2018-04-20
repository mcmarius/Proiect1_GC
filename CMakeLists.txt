set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wpedantic -Wextra")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_library(LIBWMM winmm)

set(SOIL_INCLUDE_DIR "C:\\MinGW\\include\\GL")

include_directories(${SOIL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)
add_executable(Project1 ${SOURCE_FILES})

target_link_libraries (Project1 soil ${SOIL_LIBRARY} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${LIBWMM})