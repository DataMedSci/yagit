set(EXAMPLES_SRCS
    gamma2DInterp
    gamma25D
    gamma3D
    gammaImage
)

foreach(example ${EXAMPLES_SRCS})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} yagit)
endforeach()
