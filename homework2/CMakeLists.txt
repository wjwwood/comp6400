cmake_minimum_required(VERSION 2.6)
project(homework0)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set(CMAKE_BUILD_TYPE Debug)

find_library(GLUT_LIBRARY GLUT)
find_library(OpenGL_LIBRARY OpenGL)

# Shelby Model
include_directories(include)
add_executable(shelby src/main.cc src/shelby.cc)
target_link_libraries(shelby ${OpenGL_LIBRARY} ${GLUT_LIBRARY})

