cmake_minimum_required(VERSION 3.23.0)
project(giyagit)

option(BUILD_EXAMPLES "Enable compilation of examples" OFF)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20) # for now, only for default operator==

# set(CMAKE_BUILD_TYPE Debug)

find_package(GDCM REQUIRED)

add_library(yagit
    src/ImageData.cpp
    src/ImageData.cpp # CMAKE_SOURCE_DIR?
    src/gamma/Gamma.cpp
    src/GammaResult.cpp
    src/DataReader.cpp
    src/Interpolation.cpp
)
target_include_directories(yagit PUBLIC inc) # PUBLIC?
target_link_libraries(yagit
    gdcm::gdcm
)

# add_executable(test src/test.cpp)
# target_link_libraries(test
#     yagit
# )

if(BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

if(MSVC)
    # target_compile_options(yagit PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic) # -Werror
endif()
