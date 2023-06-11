cmake_minimum_required(VERSION 3.16)
project(giyagit
    LANGUAGES CXX
)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTING "Build unit tests" OFF)
option(BUILD_PERFORMANCE_TESTING "Build performance tests" OFF)

set(CMAKE_CXX_STANDARD 17)

find_package(GDCM REQUIRED)

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_PERFORMANCE_TESTING)
    add_subdirectory(tests/performance)
endif()
