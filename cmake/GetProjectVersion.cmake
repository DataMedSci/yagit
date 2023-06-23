macro(get_project_version)
    find_package(Git)

    if(GIT_EXECUTABLE)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
            RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT GIT_DESCRIBE_ERROR_CODE)
            string(REGEX REPLACE "^v" "" GIT_DESCRIBE_VERSION ${GIT_DESCRIBE_VERSION})
            set(${PROJECT_NAME}_VERSION ${GIT_DESCRIBE_VERSION})
        endif()
    endif()

    if(NOT DEFINED ${PROJECT_NAME}_VERSION)
        set(${PROJECT_NAME}_VERSION "0.0.0-unknown")
        message(WARNING "Failed to determine yagit version from Git tags. Using default version \"${${PROJECT_NAME}_VERSION}\".")
    endif()
endmacro()
