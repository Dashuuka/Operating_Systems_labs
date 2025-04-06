#include "gtest/gtest.h"
#include "RingBuffer.h"
#include <cstdio> // Для удаления файлов после теста

// Тест: простой push/pop
TEST(RingBufferTest, PushPopSimple) {
    const std::string testFile = "test_ringbuffer_simple.bin";
    size_t capacity = 3;
    {
        RingBuffer rb(testFile, capacity, true);
        EXPECT_TRUE(rb.isEmpty());
        EXPECT_FALSE(rb.isFull());

        rb.pushMessage("Hello");
        EXPECT_FALSE(rb.isEmpty());

        std::string msg = rb.popMessage();
        EXPECT_EQ(msg, "Hello");
        EXPECT_TRUE(rb.isEmpty());
    }
    std::remove(testFile.c_str());
}

// Тест: заполнение буфера (isFull)
TEST(RingBufferTest, FullBuffer) {
    const std::string testFile = "test_ringbuffer_full.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        rb.pushMessage("First");
        rb.pushMessage("Second");
        EXPECT_TRUE(rb.isFull());
        // Попытка добавить сообщение в заполненный буфер должна выбросить исключение.
        EXPECT_THROW(rb.pushMessage("Third"), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Тест: извлечение сообщения из пустого буфера
TEST(RingBufferTest, EmptyBuffer) {
    const std::string testFile = "test_ringbuffer_empty.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        EXPECT_THROW(rb.popMessage(), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Тест: проверка кольцевой очереди (wrap-around behavior)
TEST(RingBufferTest, WrapAroundBehavior) {
    const std::string testFile = "test_ringbuffer_wrap.bin";
    size_t capacity = 3;
    {
        RingBuffer rb(testFile, capacity, true);
        rb.pushMessage("Msg1");
        rb.pushMessage("Msg2");
        rb.pushMessage("Msg3");
        EXPECT_TRUE(rb.isFull());

        EXPECT_EQ(rb.popMessage(), "Msg1");
        rb.pushMessage("Msg4");  // Здесь происходит оборачивание индекса tail

        EXPECT_TRUE(rb.isFull());
        EXPECT_EQ(rb.popMessage(), "Msg2");
        EXPECT_EQ(rb.popMessage(), "Msg3");
        EXPECT_EQ(rb.popMessage(), "Msg4");
        EXPECT_TRUE(rb.isEmpty());
    }
    std::remove(testFile.c_str());
}

// Тест: попытка добавить сообщение, превышающее допустимую длину
TEST(RingBufferTest, MessageLengthExceeded) {
    const std::string testFile = "test_ringbuffer_length.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        // Создаем строку длиной MAX_MESSAGE_LENGTH + 1
        std::string longMsg(MAX_MESSAGE_LENGTH + 1, 'a');
        EXPECT_THROW(rb.pushMessage(longMsg), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Тест: проверка сохранения данных между экземплярами (persistent storage)
TEST(RingBufferTest, PersistenceAcrossInstances) {
    const std::string testFile = "test_ringbuffer_persistence.bin";
    size_t capacity = 3;
    {
        // Создаем и инициализируем буфер, записываем сообщения.
        {
            RingBuffer rb(testFile, capacity, true);
            rb.pushMessage("One");
            rb.pushMessage("Two");
        }
        // Создаем новый экземпляр для открытия существующего файла.
        {
            RingBuffer rb(testFile);
            EXPECT_EQ(rb.popMessage(), "One");
            EXPECT_EQ(rb.popMessage(), "Two");
        }
    }
    std::remove(testFile.c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
