#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include "RingBuffer.h"

#ifdef _WIN32
#define SENDER_COMMAND "start Sender "
#else
#define SENDER_COMMAND "./Sender "
#endif

int main() {
    try {
        std::string filename;
        size_t capacity;
        int numSenders;

        std::cout << "Enter binary file name: ";
        std::getline(std::cin, filename);

        std::cout << "Enter number of records (capacity): ";
        std::cin >> capacity;
        std::cin.ignore();

        RingBuffer ring(filename, capacity, true);

        std::cout << "Enter number of Sender processes to launch: ";
        std::cin >> numSenders;
        std::cin.ignore();

        for (int i = 0; i < numSenders; ++i) {
            std::string command = SENDER_COMMAND + filename;
            std::cout << "Launching Sender process " << i + 1 << std::endl;
            int ret = system(command.c_str());
            if (ret != 0) {
                std::cerr << "Failed to launch Sender process " << i + 1 << std::endl;
            }
        }

        std::cout << "Press Enter when all Sender processes are ready...";
        std::cin.get();

        std::string input;
        while (true) {
            std::cout << "Enter command (read/exit): ";
            std::getline(std::cin, input);
            if (input == "read") {
                try {
                    std::string message = ring.popMessage();
                    std::cout << "Received message: " << message << std::endl;
                }
                catch (const std::exception& ex) {
                    std::cout << "No messages available: " << ex.what() << std::endl;
                }
            }
            else if (input == "exit") {
                break;
            }
            else {
                std::cout << "Unknown command." << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Receiver error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
