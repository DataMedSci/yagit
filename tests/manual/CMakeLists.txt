
add_executable(simulatedWendling simulatedWendling.cpp)
target_link_libraries(simulatedWendling
    yagit
)

include(../../cmake/Common.cmake)
copy_dll_to_exec(yagit simulatedWendling)
