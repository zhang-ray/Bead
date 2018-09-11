
set (CMAKE_CXX_STANDARD 14)

include_directories(../../include/)
include_directories(../../src/)


file(GLOB_RECURSE FILES_SOURCES *.cpp)
file(GLOB_RECURSE FILES_INCLUDE *.hpp)

add_executable(Calculator ${FILES_INCLUDE} ${FILES_SOURCES})
target_link_libraries(Calculator Bead)
