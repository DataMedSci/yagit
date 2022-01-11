cmake_minimum_required(VERSION 3.15)
project(giyagit)

add_subdirectory(gi_core)
add_subdirectory(gi_tests)
add_subdirectory(gi_additions)
add_subdirectory(gi_wrapper_cpp)
add_subdirectory(gi_gui_qt)

include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME "yAGIT")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")

if(WIN32)
    set(CPACK_GENERATOR NSIS)
    set(CPACK_NSIS_DISPLAY_NAME "yAGIT")
    set(CPACK_NSIS_PACKAGE_NAME "yAGIT")
endif()

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")

include(CPack)
