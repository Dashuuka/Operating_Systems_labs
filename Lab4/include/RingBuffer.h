#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <string>
#include <fstream>

const size_t MAX_MESSAGE_LENGTH = 20;

struct RingBufferHeader {
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
};

class RingBuffer {
public:
    RingBuffer(const std::string& filename, size_t capacity, bool initialize = false);
    RingBuffer(const std::string& filename);
    ~RingBuffer();

    void pushMessage(const std::string& message);
    std::string popMessage();
    bool isEmpty();
    bool isFull();

private:
    std::string filename_;
    size_t capacity_;
    std::fstream file_;

    void openFile(bool initialize);
    void readHeader(RingBufferHeader& header);
    void writeHeader(const RingBufferHeader& header);
    void writeMessage(size_t index, const char* message);
    void readMessage(size_t index, char* buffer);
    std::streampos messagePosition(size_t index);
};

#endif // RINGBUFFER_H
