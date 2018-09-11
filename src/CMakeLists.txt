cmake_minimum_required(VERSION 2.8)

include_directories(".")
include_directories("../include/")

# TODO:
# Animation, Layer, Logger, Theme, GridLayout,

file(GLOB_RECURSE FILES_SOURCES *.cpp)
file(GLOB_RECURSE FILES_INCLUDE *.hpp ../*.hpp)

add_library(${PROJECT_NAME} SHARED ${FILES_INCLUDE} ${FILES_SOURCES})
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)
