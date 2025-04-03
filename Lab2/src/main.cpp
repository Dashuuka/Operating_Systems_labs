#include "array_processor.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main() {
    try {
        std::vector<int> arr;
        int size;

        std::cout << "Enter array size: ";
        if (!(std::cin >> size) || size <= 0) {
            throw ArrayProcessor::InvalidInputException("Invalid array size");
        }

        arr.resize(size);
        for (int i = 0; i < size; ++i) {
            std::cout << "Enter element " << i + 1 << ": ";
            if (!(std::cin >> arr[i])) {
                throw ArrayProcessor::InvalidInputException("Invalid input for array element");
            }
        }

        ArrayProcessor::ProcessArray(arr);

        std::cout << "Result array: ";
        for (const auto& num : arr) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}