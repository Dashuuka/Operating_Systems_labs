#include <iostream>
#include <string>
#include <stdexcept>
#include "RingBuffer.h"

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: Sender <filename>" << std::endl;
            return 1;
        }
        std::string filename = argv[1];

        RingBuffer ring(filename);

        std::cout << "Sender process ready. Enter command (send/exit):" << std::endl;

        std::string input;
        while (true) {
            std::cout << "Sender command: ";
            std::getline(std::cin, input);
            if (input == "send") {
                std::cout << "Enter message (max 20 characters): ";
                std::string message;
                std::getline(std::cin, message);
                try {
                    ring.pushMessage(message);
                    std::cout << "Message sent." << std::endl;
                }
                catch (const std::exception& ex) {
                    std::cout << "Error sending message: " << ex.what() << std::endl;
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
        std::cerr << "Sender error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
