add_library(Lab3Core STATIC
    SharedResources.cpp
    MarkerThread.cpp
)

target_include_directories(Lab3Core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(Lab3Main Main.cpp)
target_link_libraries(Lab3Main Lab3Core)