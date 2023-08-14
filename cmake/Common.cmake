function(copy_target_output source_target dest_target)
    add_custom_command(
        TARGET ${dest_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${source_target}>
            $<TARGET_FILE_DIR:${dest_target}>
    )
endfunction()

function(copy_directory source_directory dest_target dest_dirname)
    add_custom_command(
        TARGET ${dest_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${source_directory}
            $<TARGET_FILE_DIR:${dest_target}>/${dest_dirname}
    )
endfunction()

function(copy_dll_to_exec dll_target exec_target)
    if(WIN32 AND BUILD_SHARED_LIBS)
        copy_target_output(${dll_target} ${exec_target})
    endif()
endfunction()
