# Lab1: Process Creation and Employee Report 

This project demonstrates Windows API process creation by using three utilities: **Creator** (to generate a binary file with employee records), **Reporter** (to create a salary report), and **Main** (to manage the workflow). Built with CMake and tested with Google Test.

## Result
![image](https://github.com/user-attachments/assets/8cafbc85-884b-42ec-a9f9-c8c136a4715b)

## Tests

Да, код полностью покрыт необходимыми тестами для доказательства успешной синхронизации:

## Unit-тесты (единичные тесты):

1. **test_disk.cpp, test_folder.cpp, test_file.cpp** - тестируют отдельные компоненты
2. **test_visitors.cpp** - тестируют посетителей
3. **test_sync_strategy.cpp** - тестируют стратегии синхронизации:
   - `NoSyncAllowsRaceCondition` - демонстрирует гонки без синхронизации
   - `MutexSyncPreventsRaceCondition` - доказывает отсутствие гонок с синхронизацией
   - `SharedLockAllowsConcurrentReads` - тестирует параллельное чтение
   - `ExclusiveLockBlocksReaders` - проверяет блокировку при записи
   - `ScopedLockRAII` - тестирует автоматическое освобождение блокировок

## End-to-end тесты:

1. **test_multithreading.cpp** - многопоточные тесты:
   - `ConcurrentFileCount` - параллельный подсчет файлов несколькими потоками
   - `ThreadSafeFolder` - безопасное добавление файлов из 10 потоков
   - `ConcurrentTraversal` - параллельный обход структуры 8 потоками

2. **test_integration.cpp** - интеграционные тесты:
   - `ConcurrentVisitors` - 5 параллельных визиторов
   - `DynamicFileSystemModification` - одновременная запись и чтение
   - `VisitorChaining` - цепочка визиторов

3. **test_performance.cpp** - производительность с многопоточностью:
   - `ConcurrentVsSingleThreaded` - сравнение однопоточной и многопоточной версий
   - `StressTest` - стресс-тест с 8 потоками и 100 операциями каждый


Все требования задания выполнены - код имеет полное покрытие тестами, доказывающими корректность многопоточной работы.
