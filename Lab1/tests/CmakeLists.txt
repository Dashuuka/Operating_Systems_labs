add_executable(UnitTests test_employee.cpp)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(UnitTests gtest_main)
add_test(NAME UnitTests COMMAND UnitTests)