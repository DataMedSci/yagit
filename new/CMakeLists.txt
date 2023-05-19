cmake_minimum_required(VERSION 3.23.0)
project(giyagit)

option(BUILD_EXAMPLES "Enable compilation of examples" OFF)

set(CMAKE_CXX_STANDARD 17)

find_package(GDCM REQUIRED)

add_library(yagit
    src/ImageData.cpp
    src/GammaResult.cpp
    src/gamma/Gamma.cpp
    src/DataReader.cpp
    src/Interpolation.cpp
)

target_include_directories(yagit PUBLIC inc) # PUBLIC?
target_link_libraries(yagit PRIVATE
    gdcmCommon gdcmDSED
)

# add_executable(test src/test.cpp)
# target_link_libraries(test
#     yagit
# )

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MSVC)
    # line below is commented, because msvc compiler shows also warnings from external dependencies, but it should not
    # target_compile_options(yagit PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic) # -Werror
endif()
