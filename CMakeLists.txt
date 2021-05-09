cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
project(LTL_to_NGA LANGUAGES CXX)

set(LTL_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(apps)
