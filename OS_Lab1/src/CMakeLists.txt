add_executable(Main Main.cpp)
add_executable(Creator Creator.cpp)
add_executable(Reporter Reporter.cpp)

target_include_directories(Main PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Creator PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Reporter PRIVATE ${CMAKE_SOURCE_DIR}/include)
