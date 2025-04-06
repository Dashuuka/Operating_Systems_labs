#include "gtest/gtest.h"
#include "RingBuffer.h"
#include <cstdio>

// Test: Basic push/pop
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

// Test: Full buffer
TEST(RingBufferTest, FullBuffer) {
    const std::string testFile = "test_ringbuffer_full.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        rb.pushMessage("First");
        rb.pushMessage("Second");
        EXPECT_TRUE(rb.isFull());
        EXPECT_THROW(rb.pushMessage("Third"), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Test: Empty buffer
TEST(RingBufferTest, EmptyBuffer) {
    const std::string testFile = "test_ringbuffer_empty.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        EXPECT_THROW(rb.popMessage(), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Test: Wrap-around behavior
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
        rb.pushMessage("Msg4");
        EXPECT_TRUE(rb.isFull());
        EXPECT_EQ(rb.popMessage(), "Msg2");
        EXPECT_EQ(rb.popMessage(), "Msg3");
        EXPECT_EQ(rb.popMessage(), "Msg4");
        EXPECT_TRUE(rb.isEmpty());
    }
    std::remove(testFile.c_str());
}

// Test: Message length exceeded
TEST(RingBufferTest, MessageLengthExceeded) {
    const std::string testFile = "test_ringbuffer_length.bin";
    size_t capacity = 2;
    {
        RingBuffer rb(testFile, capacity, true);
        std::string longMsg(MAX_MESSAGE_LENGTH + 1, 'a');
        EXPECT_THROW(rb.pushMessage(longMsg), std::runtime_error);
    }
    std::remove(testFile.c_str());
}

// Test: Persistence across instances
TEST(RingBufferTest, PersistenceAcrossInstances) {
    const std::string testFile = "test_ringbuffer_persistence.bin";
    size_t capacity = 3;
    {
        {
            RingBuffer rb(testFile, capacity, true);
            rb.pushMessage("One");
            rb.pushMessage("Two");
        }
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
