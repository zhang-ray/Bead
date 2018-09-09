
set (CMAKE_CXX_STANDARD 11)

include_directories(../include/)
include_directories(../src/)

add_executable(Simulator Simulator.cpp)
target_link_libraries(Simulator Bead SDL2)
