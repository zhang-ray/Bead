cmake_minimum_required(VERSION 2.8)
project(Bead)


set (CMAKE_CXX_STANDARD 14) # or C++14?


# treat all warnings as errors
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)




add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
