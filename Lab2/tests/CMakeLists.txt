add_executable(UnitTests test_array_processor.cpp)
target_include_directories(UnitTests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(UnitTests PRIVATE array_processor gtest_main)

add_test(NAME ArrayProcessorTests COMMAND UnitTests)