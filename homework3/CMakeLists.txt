cmake_minimum_required(VERSION 2.6)
project(homework2)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set(CMAKE_BUILD_TYPE Debug)

# find opengl and glut
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OPENGL not found!")
endif(NOT OPENGL_FOUND)

find_package(GLUT REQUIRED)


include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

# Homework 3
add_executable(homework3 src/main.cc)
target_link_libraries(homework3 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
