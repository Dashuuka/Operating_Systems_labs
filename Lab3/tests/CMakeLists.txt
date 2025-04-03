# Тестовый исполняемый файл
add_executable(UnitTests
    ${CMAKE_CURRENT_SOURCE_DIR}/MarkerThreadTests.cpp
)

# Настройки для тестов
target_include_directories(UnitTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(UnitTests PRIVATE
    Lab3Core
    gtest_main
)

# Регистрация тестов
include(GoogleTest)
gtest_discover_tests(UnitTests)