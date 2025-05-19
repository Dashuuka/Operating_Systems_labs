cmake_minimum_required(VERSION 3.8)
project(VisitorFileSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(filesystem_lib
    src/Disk.cpp
    src/Folder.cpp
    src/File.cpp
    src/ConcreteVisitors.cpp
)

if(UNIX)
    target_link_libraries(filesystem_lib pthread)
endif()

add_executable(filesystem_app app/main.cpp)
target_link_libraries(filesystem_app filesystem_lib)

# Äîáàâëÿåì ïîääèðåêòîðèþ ñ òåñòàìè
add_subdirectory(tests)
