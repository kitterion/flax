cmake_minimum_required(VERSION 3.1)
project(sol2)

add_library(sol sol.hpp empty.cpp)
target_include_directories(sol PUBLIC .)

