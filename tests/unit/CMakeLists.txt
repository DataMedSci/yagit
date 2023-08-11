find_package(GTest REQUIRED)

set(TESTS_SRCS
    DataReaderTest
    DataWriterTest
    GammaTest
    GammaCommonTest
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

    # add_test(NAME ${test} COMMAND ${test} WORKING_DIRECTORY $<TARGET_FILE_DIR:${test}>)

    include(GoogleTest)
    gtest_discover_tests(${test}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${test}>
        NO_PRETTY_TYPES
        NO_PRETTY_VALUES
    )
endforeach()

include(../../cmake/Common.cmake)
copy_directory(${CMAKE_CURRENT_SOURCE_DIR}/data ImageDataTest data)


copy_dll_to_exec(yagit ImageDataTest)
