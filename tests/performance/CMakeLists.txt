add_executable(gammaPerf gammaPerf.cpp)
target_link_libraries(gammaPerf
    yagit
)

add_executable(interpPerf interpPerf.cpp)
target_link_libraries(interpPerf
    yagit
)

include(../../cmake/Common.cmake)
copy_dll_to_exec(yagit gammaPerf)
