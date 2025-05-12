# Включаем покрытие кода
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
endif()

# Добавляем все тестовые файлы
add_executable(UnitTests
    ${CMAKE_CURRENT_SOURCE_DIR}/test_disk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_folder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_visitors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_sync_strategy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_multithreading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_integration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_performance.cpp
)

target_link_libraries(UnitTests PRIVATE
    filesystem_lib
    gtest_main
    gmock_main
)

include(GoogleTest)
gtest_discover_tests(UnitTests)

# Добавляем цель для генерации отчета о покрытии
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --html --html-details -o ${CMAKE_BINARY_DIR}/coverage/index.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
    )
endif()