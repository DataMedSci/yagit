add_executable(perfTest perfTest.cpp)
target_link_libraries(perfTest
    yagit
)

include(../../cmake/Common.cmake)
copy_dll_to_exec(yagit perfTest)
