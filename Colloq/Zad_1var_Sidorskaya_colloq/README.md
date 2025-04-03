### **Структура проекта с заголовочным файлом**

```
FactorialProject/
│
├── CMakeLists.txt           # Файл для сборки проекта
├── main.cpp                 # Основной код программы
├── test.cpp                 # Файл с юнит-тестами
├── factorial.h              # Заголовочный файл для функций (например, для вычисления факториала)
├── reverse_list.h           # Заголовочный файл для работы с реверсированием связного списка
└── README.md                # Описание проекта в формате Markdown
```

---

### **Файл: `CMakeLists.txt`**

```cmake
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
```

---

### **Файл: `main.cpp`**

```cpp
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "factorial.h"
#include "reverse_list.h"

# FactorialProject

Этот проект включает несколько задач на вычисление факториалов, удаление дубликатов из массива и реверсирование связного списка. Все эти задачи реализованы в виде консольного приложения с возможностью доступа через REST API.

## Структура проекта

Проект состоит из следующих файлов:
- `main.cpp`: Основная программа, решающая задачи, а также реализующая REST API.
- `test.cpp`: Файл с юнит-тестами для проверки работы функций.
- `factorial.h`: Заголовочный файл для вычисления факториалов.
- `reverse_list.h`: Заголовочный файл для работы с реверсированием связного списка.
- `CMakeLists.txt`: Файл для сборки проекта с использованием CMake.

## Зависимости

- **C++ REST SDK (cpprestsdk)**: Для работы с HTTP и создания REST API.
- **Google Test (gtest)**: Для юнит-тестирования функций.

## Сборка проекта

Для сборки проекта необходимо выполнить следующие шаги:

1. Установите зависимости:
   - **C++ REST SDK**: Для Linux можно установить через `sudo apt-get install libcpprest-dev`.
   - **Google Test**: Для Linux можно установить через `sudo apt-get install libgtest-dev`.

2. Склонируйте репозиторий и создайте папку для сборки:
   ```bash
   git clone <url-репозитория>
   cd FactorialProject
   mkdir build
   cd build
   cmake ..
   ```

3. Соберите проект:
   ```bash
   make
   ```

## Запуск проекта

### Запуск консольного приложения

1. В директории с собранным проектом выполните:
   ```bash
   ./main
   ```
2. Программа запросит ввод числа и выведет на экран факториалы от 1 до n.

### Запуск REST сервиса

Для работы с REST сервисом:
1. Сервер запускается на порту 8080.
2. Откройте браузер или используйте `curl` для тестирования API:
   ```bash
   curl "http://localhost:8080/factorial?n=5"
   ```

### Запуск юнит-тестов

Для выполнения юнит-тестов:
1. В директории с собранным проектом выполните:
   ```bash
   ./test
   ```

## Тесты

В проекте реализованы юнит-тесты для проверки:
- Функции вычисления факториалов.
- Функции удаления дубликатов из массива.
- Функции реверсирования связного списка.


