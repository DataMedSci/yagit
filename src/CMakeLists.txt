
# Options
# =======

set(GAMMA_VERSION "THREADED" CACHE STRING "Implementation of gamma index")
set_property(CACHE GAMMA_VERSION PROPERTY STRINGS "SEQUENTIAL" "THREADED" "SIMD")

get_property(GAMMA_VERSION_STRINGS CACHE GAMMA_VERSION PROPERTY STRINGS)
if(NOT GAMMA_VERSION IN_LIST GAMMA_VERSION_STRINGS)
    message(FATAL_ERROR "Wrong value of the parameter GAMMA_VERSION")
endif()

set(SIMD_EXTENSION "DEFAULT" CACHE STRING "SIMD instruction set extension")
set_property(CACHE SIMD_EXTENSION PROPERTY STRINGS
    "DEFAULT"
    "SSE2" "SSE3" "SSSE3" "SSE4.1" "SSE4.2"
    "AVX" "AVX2" "AVX512"
    "NATIVE"
)

get_property(SIMD_EXTENSION_STRINGS CACHE SIMD_EXTENSION PROPERTY STRINGS)
if(NOT SIMD_EXTENSION IN_LIST SIMD_EXTENSION_STRINGS)
    message(FATAL_ERROR "Wrong value of the parameter SIMD_EXTENSION")
endif()

# enabling FMA gives better performance, but slightly different results
# TODO: check if enabling FMA gives more or less accurate results
option(ENABLE_FMA "Enable fused multiply-add (FMA) when building yagit library" OFF)

# Build
# =====

find_package(GDCM REQUIRED)

set(YAGIT_SOURCE_FILES
    ImageData.cpp
    GammaResult.cpp
    DataReader.cpp
    DataWriter.cpp
    Interpolation.cpp
)
set(YAGIT_DEPS
    gdcmCommon gdcmDSED
)

if(GAMMA_VERSION STREQUAL "SEQUENTIAL")
    list(APPEND YAGIT_SOURCE_FILES gamma/Gamma.cpp)
elseif(GAMMA_VERSION STREQUAL "THREADED")
    find_package(Threads REQUIRED)

    list(APPEND YAGIT_DEPS Threads::Threads)
    list(APPEND YAGIT_SOURCE_FILES gamma/GammaThreads.cpp)
elseif(GAMMA_VERSION STREQUAL "SIMD")
    find_package(xsimd REQUIRED)

    list(APPEND YAGIT_DEPS xsimd)
    list(APPEND YAGIT_SOURCE_FILES gamma/GammaSimd.cpp)
endif()

add_library(yagit ${YAGIT_SOURCE_FILES})
target_include_directories(yagit PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(yagit PRIVATE ${YAGIT_DEPS})

if(NOT SIMD_EXTENSION STREQUAL "DEFAULT")
    include(../cmake/Simd.cmake)
    add_simd_flags(yagit ${SIMD_EXTENSION})
endif()

if(ENABLE_FMA)
    if(MSVC)
        message(WARNING "There is no explicit option to enable FMA in MSVC")
    else()
        target_compile_options(yagit PRIVATE -mfma)
    endif()
endif()

if(MSVC)
    # line below is commented, because msvc compiler shows also warnings from external dependencies, but it should not
    # target_compile_options(yagit PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
