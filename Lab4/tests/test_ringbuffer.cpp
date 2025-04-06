#include "gtest/gtest.h"
#include "RingBuffer.h"
#include <cstdio> // Для удаления файла после теста

TEST(RingBufferTest, PushPopTest) {
    const std::string testFile = "test_ringbuffer.bin";
    size_t capacity = 3;

    {
        // Создание и инициализация кольцевой очереди.
        RingBuffer rb(testFile, capacity, true);
        EXPECT_TRUE(rb.isEmpty());
        EXPECT_FALSE(rb.isFull());

        // Добавление сообщения.
        rb.pushMessage("Test");
        EXPECT_FALSE(rb.isEmpty());

        // Чтение сообщения и проверка.
        std::string msg = rb.popMessage();
        EXPECT_EQ(msg, "Test");
        EXPECT_TRUE(rb.isEmpty());
    }
    // Удаление тестового файла.
    std::remove(testFile.c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
