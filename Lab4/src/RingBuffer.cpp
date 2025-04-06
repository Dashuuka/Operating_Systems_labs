#include "RingBuffer.h"
#include <stdexcept>
#include <cstring>

static const std::streampos HEADER_POSITION = 0;

// Calculate the file position for the message slot at index.
std::streampos RingBuffer::messagePosition(size_t index) {
    return sizeof(RingBufferHeader) + index * MAX_MESSAGE_LENGTH;
}

RingBuffer::RingBuffer(const std::string& filename, size_t capacity, bool initialize)
    : filename_(filename), capacity_(capacity)
{
    openFile(initialize);
    if (initialize) {
        RingBufferHeader header;
        header.capacity = capacity_;
        header.head = 0;
        header.tail = 0;
        header.count = 0;
        writeHeader(header);
        // Initialize all message slots with empty data.
        char empty[MAX_MESSAGE_LENGTH] = { 0 };
        for (size_t i = 0; i < capacity_; ++i) {
            file_.seekp(messagePosition(i));
            file_.write(empty, MAX_MESSAGE_LENGTH);
        }
        file_.flush();
    }
}

RingBuffer::RingBuffer(const std::string& filename)
    : filename_(filename)
{
    file_.open(filename_, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename_);
    }
    RingBufferHeader header;
    readHeader(header);
    capacity_ = header.capacity;
}

RingBuffer::~RingBuffer() {
    file_.close();
}

void RingBuffer::openFile(bool initialize) {
    if (initialize) {
        file_.open(filename_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else {
        file_.open(filename_, std::ios::in | std::ios::out | std::ios::binary);
    }
    if (!file_.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename_);
    }
}

void RingBuffer::readHeader(RingBufferHeader& header) {
    file_.seekg(HEADER_POSITION);
    file_.read(reinterpret_cast<char*>(&header), sizeof(RingBufferHeader));
    if (!file_) {
        throw std::runtime_error("Failed to read header from file: " + filename_);
    }
}

void RingBuffer::writeHeader(const RingBufferHeader& header) {
    file_.seekp(HEADER_POSITION);
    file_.write(reinterpret_cast<const char*>(&header), sizeof(RingBufferHeader));
    if (!file_) {
        throw std::runtime_error("Failed to write header to file: " + filename_);
    }
    file_.flush();
}

void RingBuffer::writeMessage(size_t index, const char* message) {
    file_.seekp(messagePosition(index));
    file_.write(message, MAX_MESSAGE_LENGTH);
    if (!file_) {
        throw std::runtime_error("Failed to write message to file: " + filename_);
    }
    file_.flush();
}

void RingBuffer::readMessage(size_t index, char* buffer) {
    file_.seekg(messagePosition(index));
    file_.read(buffer, MAX_MESSAGE_LENGTH);
    if (!file_) {
        throw std::runtime_error("Failed to read message from file: " + filename_);
    }
}

void RingBuffer::pushMessage(const std::string& message) {
    if (message.length() > MAX_MESSAGE_LENGTH)
        throw std::runtime_error("Message exceeds maximum allowed length");

    RingBufferHeader header;
    readHeader(header);

    if (header.count == header.capacity) {
        throw std::runtime_error("Ring buffer is full");
    }

    char buffer[MAX_MESSAGE_LENGTH] = { 0 };
    std::memcpy(buffer, message.c_str(), message.length());
    writeMessage(header.tail, buffer);

    header.tail = (header.tail + 1) % header.capacity;
    header.count++;
    writeHeader(header);
}

std::string RingBuffer::popMessage() {
    RingBufferHeader header;
    readHeader(header);

    if (header.count == 0) {
        throw std::runtime_error("Ring buffer is empty");
    }

    char buffer[MAX_MESSAGE_LENGTH] = { 0 };
    readMessage(header.head, buffer);
    std::string message(buffer);

    char empty[MAX_MESSAGE_LENGTH] = { 0 };
    writeMessage(header.head, empty);

    header.head = (header.head + 1) % header.capacity;
    header.count--;
    writeHeader(header);

    return message;
}

bool RingBuffer::isEmpty() {
    RingBufferHeader header;
    readHeader(header);
    return (header.count == 0);
}

bool RingBuffer::isFull() {
    RingBufferHeader header;
    readHeader(header);
    return (header.count == header.capacity);
}
