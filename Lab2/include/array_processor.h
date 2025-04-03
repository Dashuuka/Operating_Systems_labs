#pragma once
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <string>
#include <mutex>
#include <cmath> 
#include <iomanip> 

class ArrayProcessor {
public:
    static constexpr DWORD kMinMaxSleepMs = 7;
    static constexpr DWORD kAverageSleepMs = 12;

    class EmptyArrayException : public std::runtime_error {
    public:
        EmptyArrayException() : runtime_error("Operation on empty array") {}
    };

    class InvalidInputException : public std::runtime_error {
    public:
        InvalidInputException(const std::string& message) : runtime_error(message) {}
    };

    static DWORD WINAPI FindMinMaxThread(LPVOID lpParam);
    static DWORD WINAPI CalculateAverageThread(LPVOID lpParam);

    static void ProcessArray(std::vector<int>& arr);

private:
    static void ValidateArray(const std::vector<int>& arr);

    static std::mutex console_mutex;

    struct ThreadResults {
        int min = 0;
        int max = 0;
        int average = 0;
    };

    static ThreadResults results;
};
