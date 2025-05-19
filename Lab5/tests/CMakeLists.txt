add_executable(FileManagerTests
    file_manager_test.cpp
)

target_include_directories(FileManagerTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(FileManagerTests PRIVATE
    Lab5Core
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(FileManagerTests)