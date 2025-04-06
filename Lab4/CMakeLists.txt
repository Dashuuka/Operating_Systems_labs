cmake_minimum_required(VERSION 3.8)
project(Lab4Sync)

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

add_executable(Receiver src/Receiver.cpp src/RingBuffer.cpp)
add_executable(Sender src/Sender.cpp src/RingBuffer.cpp)

if(UNIX)
    target_link_libraries(Receiver pthread)
    target_link_libraries(Sender pthread)
endif()

add_subdirectory(tests)
