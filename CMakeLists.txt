#project name
project(Terrain)

cmake_minimum_required(VERSION 2.8)

# Compiler flags
add_definitions(-O2 -Wall)

aux_source_directory(. SRC_LIST)

find_library(GL GL)
find_library(GLU GLU)
find_library(GLUT glut)

#
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${GL} ${GLU} ${GLUT})
