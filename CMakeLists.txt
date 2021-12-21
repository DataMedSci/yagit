cmake_minimum_required(VERSION 3.15)
project(giyagit)

add_subdirectory(gi_core)
#add_subdirectory(gi_tests)
add_subdirectory(gi_additions)
add_subdirectory(gi_wrapper_cpp)
add_subdirectory(gi_gui_qt)

include(InstallRequiredSystemLibraries)

set(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION OFF)

set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME "yAGITQt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")

if(WIN32)
    set(CPACK_GENERATOR NSIS)
    #    set(CPACK_SOURCE_GENERATOR 'NSIS')
    set(CPACK_NSIS_DISPLAY_NAME "yAGITQt")
    set(CPACK_NSIS_PACKAGE_NAME "yAGITQt")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "build/gi_core")
    #    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "yAGITQt")
    #    set(CPACK_NSIS_MODIFY_PATH "ON")
endif()

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 1)
include(CPack)

