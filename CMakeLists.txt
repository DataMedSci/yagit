cmake_minimum_required(VERSION 3.16)
project(yagit
    LANGUAGES CXX
)


# Options
# =======

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTING "Build unit tests" OFF)
option(BUILD_PERFORMANCE_TESTING "Build performance tests" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(GNUInstallDirs)


# Build
# =====

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32 AND BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests/unit)
    add_subdirectory(tests/manual)
endif()
if(BUILD_PERFORMANCE_TESTING)
    add_subdirectory(tests/performance)
endif()


# Installation
# ============

set(PACKAGE_NAME ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME "${PACKAGE_NAME}Targets")
set(CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake")
set(VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}")


include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/Config.cmake.in
    ${CONFIG_FILE}
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
    NO_SET_AND_CHECK_MACRO
)

if(NOT DEFINED CMAKE_PROJECT_VERSION OR CMAKE_PROJECT_VERSION STREQUAL "")
    include(cmake/GetProjectVersion.cmake)
    get_project_version()
endif()

write_basic_package_version_file(
    ${VERSION_FILE}
    VERSION ${yagit_VERSION}
    COMPATIBILITY AnyNewerVersion
)


install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PACKAGE_NAME}
        EXPORT ${TARGETS_EXPORT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT ${TARGETS_EXPORT_NAME}
        FILE ${TARGETS_EXPORT_NAME}.cmake
        NAMESPACE ${PACKAGE_NAME}::
        DESTINATION ${CONFIG_INSTALL_DIR}
)
install(FILES ${CONFIG_FILE} ${VERSION_FILE}
        DESTINATION ${CONFIG_INSTALL_DIR}
)
install(DIRECTORY docs/build/html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        OPTIONAL
)
