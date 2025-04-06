add_executable(UnitTests
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ringbuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/RingBuffer.cpp
)

target_include_directories(UnitTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(UnitTests PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(UnitTests)
