cmake_minimum_required(VERSION 3.1)
project(flax)

if(CMAKE_BUILD_TYPE STREQUAL "")
  message("No build type specified. Defaulting to \"Debug\".")
  set(CMAKE_BUILD_TYPE "Debug")
endif(CMAKE_BUILD_TYPE STREQUAL "")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(external/googletest)
add_subdirectory(external/lua)
add_subdirectory(external/sol2)

add_subdirectory(src)
