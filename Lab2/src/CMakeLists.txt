add_library(array_processor STATIC array_processor.cpp)
target_include_directories(array_processor PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(ProcessCreationLab main.cpp)
target_link_libraries(ProcessCreationLab array_processor)