#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <string>
#include <fstream>

// Maximum message length: 20 characters.
const size_t MAX_MESSAGE_LENGTH = 20;

// Structure for ring buffer header stored at the beginning of the file.
struct RingBufferHeader {
    size_t capacity;  // Total number of message slots.
    size_t head;      // Index for next message to read.
    size_t tail;      // Index for next message to write.
    size_t count;     // Current number of messages in the buffer.
};

// Class implementing a file-based ring buffer (FIFO).
class RingBuffer {
public:
    // Constructor for creating/initializing a new ring buffer file.
    // If initialize == true, the file is created/truncated and the header is written.
    RingBuffer(const std::string& filename, size_t capacity, bool initialize = false);

    // Overloaded constructor for opening an existing ring buffer file.
    RingBuffer(const std::string& filename);

    ~RingBuffer();

    // Push a message into the buffer. Throws if the message length exceeds MAX_MESSAGE_LENGTH or the buffer is full.
    void pushMessage(const std::string& message);

    // Pop a message from the buffer. Throws if the buffer is empty.
    std::string popMessage();

    // Check if the buffer is empty.
    bool isEmpty();

    // Check if the buffer is full.
    bool isFull();

private:
    std::string filename_;
    size_t capacity_;
    std::fstream file_;

    // Open file with given mode.
    void openFile(bool initialize);

    // Read/write header.
    void readHeader(RingBufferHeader& header);
    void writeHeader(const RingBufferHeader& header);

    // Write/read a message at a given index.
    void writeMessage(size_t index, const char* message);
    void readMessage(size_t index, char* buffer);

    // Calculate file position for a message slot.
    std::streampos messagePosition(size_t index);
};

#endif // RINGBUFFER_H
