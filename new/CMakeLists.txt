cmake_minimum_required(VERSION 3.23.0)
project(giyagit)

option(BUILD_EXAMPLES "Enable compilation of examples" ON)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20) # for now, only for default operator==
# 
# set(CMAKE_BUILD_TYPE Debug)
# 
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

# temporary tests
add_executable(test src/manual_tests/test.cpp)
target_link_libraries(test
    yagit
)
add_executable(test_interp src/manual_tests/test_interp.cpp)
target_link_libraries(test_interp
    yagit
)
add_executable(test_interp_2d_3d src/manual_tests/test_interp_2d_3d.cpp)
target_link_libraries(test_interp_2d_3d
    yagit
)
# 

if(BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

if(MSVC)
    # target_compile_options(yagit PRIVATE /W4) # /WX
    # target_compile_options(main PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic) # -Werror
    # target_compile_options(test PRIVATE -Wall -Wextra -Wpedantic) # -Werror
    # target_compile_options(test2 PRIVATE -Wall -Wextra -Wpedantic) # -Werror
endif()
