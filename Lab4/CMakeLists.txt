cmake_minimum_required(VERSION 3.8)
project(Lab4Sync)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключение GoogleTest через FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Включение директории с заголовочными файлами
include_directories(${PROJECT_SOURCE_DIR}/include)

# Добавление исполняемых файлов для Receiver и Sender
add_executable(Receiver src/Receiver.cpp src/RingBuffer.cpp)
add_executable(Sender src/Sender.cpp src/RingBuffer.cpp)

# Для Unix-подобных систем подключаем pthread при необходимости
if(UNIX)
    target_link_libraries(Receiver pthread)
    target_link_libraries(Sender pthread)
endif()

# Подключение тестов
add_subdirectory(tests)
