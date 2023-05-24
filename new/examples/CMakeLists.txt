set(EXAMPLES_SRCS
    gamma2D
    gamma25D
    gamma3DInterp
    gammaImage
)

foreach(example ${EXAMPLES_SRCS})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} yagit)
endforeach()
