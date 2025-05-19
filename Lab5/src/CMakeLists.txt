add_library(Lab5Core
    file_manager.cpp
)

target_include_directories(Lab5Core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(Lab5Core PUBLIC _CRT_SECURE_NO_WARNINGS)

add_executable(server
    server.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE
    Lab5Core
)

add_executable(client
    client.cpp
)

target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(client PRIVATE
    Lab5Core
)