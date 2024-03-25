set(EXAMPLES_SRCS
    gammaSimple
    gamma3D
    gammaWithInterp
)

foreach(example ${EXAMPLES_SRCS})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} yagit)
endforeach()

include(../cmake/Common.cmake)
copy_dll_to_exec(yagit gammaImage)
