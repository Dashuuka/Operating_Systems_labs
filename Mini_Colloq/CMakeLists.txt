cmake_minimum_required(VERSION 3.8)
project(VisitorFileSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# FetchContent для Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Добавляем директорию с заголовочными файлами
include_directories(${PROJECT_SOURCE_DIR}/include)

# Создаем библиотеку с основными классами
add_library(filesystem_lib
    src/Disk.cpp
    src/Folder.cpp
    src/File.cpp
    src/ConcreteVisitors.cpp
)

# Связываем с pthread если это Unix
if(UNIX)
    target_link_libraries(filesystem_lib pthread)
endif()

# Добавляем исполняемый файл
add_executable(filesystem_app app/main.cpp)
target_link_libraries(filesystem_app filesystem_lib)

# Добавляем поддиректорию с тестами
add_subdirectory(tests)