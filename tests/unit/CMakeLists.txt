find_package(GTest REQUIRED)

set(TESTS_SRCS
    DataReaderTest
    DataWriterTest
    GammaTest
    GammaResultTest
    ImageDataTest
    InterpolationTest
)

foreach(test ${TESTS_SRCS})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test}
        yagit
        GTest::GTest GTest::Main
    )
endforeach()

include(../../cmake/Common.cmake)
copy_directory(${CMAKE_CURRENT_SOURCE_DIR}/data ImageDataTest data)


copy_dll_to_exec(yagit ImageDataTest)
