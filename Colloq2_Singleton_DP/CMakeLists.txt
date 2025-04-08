cmake_minimum_required(VERSION 3.10)
project(FactorialProject)

set(CMAKE_CXX_STANDARD 17)

# Добавляем зависимости для C++ REST SDK и Google Test
find_package(cpprestsdk REQUIRED)
find_package(GTest REQUIRED)

# Указываем файлы исходного кода
add_executable(main main.cpp)

# Линкуем библиотеки
target_link_libraries(main cpprest GTest::GTest GTest::Main)

# Делаем тесты
add_executable(test test.cpp)
target_link_libraries(test cpprest GTest::GTest GTest::Main)
