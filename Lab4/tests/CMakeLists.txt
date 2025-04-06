# Добавление тестового исполняемого файла для кольцевой очереди.
add_executable(test_ringbuffer test_ringbuffer.cpp ../src/RingBuffer.cpp)
target_link_libraries(test_ringbuffer gtest_main)
add_test(NAME RingBufferTest COMMAND test_ringbuffer)
