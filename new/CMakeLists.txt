cmake_minimum_required(VERSION 3.23.0)
project(giyagit)


# add_subdirectory?
# TODO: improve?

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 20) # for now, only for default operator==

find_package(GDCM REQUIRED)

add_library(yagit
    src/DataReader.cpp # CMAKE_SOURCE_DIR?
    # src/ImageData.cpp
    src/GammaResult.cpp
    src/DoseData.cpp
    src/gamma/Gamma.cpp
)
target_include_directories(yagit PUBLIC inc) # PUBLIC?
target_link_libraries(yagit
    gdcm::gdcm
)

# temporary tests
add_executable(test src/test.cpp)
target_link_libraries(test
    yagit
)
add_executable(test2 src/test2.cpp)
target_link_libraries(test2
    yagit
)
add_executable(test3 src/test3.cpp)
target_link_libraries(test3
    yagit
)
# 

if(MSVC)
    # target_compile_options(yagit PRIVATE /W4) # /WX
    # target_compile_options(main PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic) # -Werror
    # target_compile_options(test PRIVATE -Wall -Wextra -Wpedantic) # -Werror
    # target_compile_options(test2 PRIVATE -Wall -Wextra -Wpedantic) # -Werror
endif()
