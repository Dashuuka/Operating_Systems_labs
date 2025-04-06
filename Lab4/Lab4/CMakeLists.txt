# CMakeList.txt: проект CMake для Lab4; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Lab4 "Lab4.cpp" "Lab4.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Lab4 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
