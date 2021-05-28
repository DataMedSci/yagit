cmake_minimum_required(VERSION 3.15)
project(giyagit)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")
include(common)
include(CPM)

add_subdirectory(gi_core)
add_subdirectory(gi_additions)
add_subdirectory(gi_wrapper_cpp)
add_subdirectory(gi_gui)
