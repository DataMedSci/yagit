set(GAMMA_VERSION "THREADED" CACHE STRING "Implementation of gamma index")
set_property(CACHE GAMMA_VERSION PROPERTY STRINGS "SEQUENTIAL" "THREADED")

get_property(GAMMA_VERSION_STRINGS CACHE GAMMA_VERSION PROPERTY STRINGS)
if(NOT GAMMA_VERSION IN_LIST GAMMA_VERSION_STRINGS)
  message(FATAL_ERROR "Wrong value of the parameter GAMMA_VERSION")
endif()

set(YAGIT_SOURCE_FILES
    ImageData.cpp
    GammaResult.cpp
    DataReader.cpp
    DataWriter.cpp
    Interpolation.cpp
)
if(GAMMA_VERSION STREQUAL "SEQUENTIAL")
    list(APPEND YAGIT_SOURCE_FILES gamma/Gamma.cpp)
elseif(GAMMA_VERSION STREQUAL "THREADED")
    list(APPEND YAGIT_SOURCE_FILES gamma/GammaThreads.cpp)
endif()

add_library(yagit ${YAGIT_SOURCE_FILES})
target_include_directories(yagit PUBLIC ../inc)
target_link_libraries(yagit PRIVATE
    gdcmCommon gdcmDSED
)

if(MSVC)
    # line below is commented, because msvc compiler shows also warnings from external dependencies, but it should not
    # target_compile_options(yagit PRIVATE /W4) # /WX
else()
    target_compile_options(yagit PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
