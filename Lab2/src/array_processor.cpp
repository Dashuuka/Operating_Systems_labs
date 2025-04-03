#include "array_processor.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath> 
#include <iomanip> 

std::mutex ArrayProcessor::console_mutex;

ArrayProcessor::ThreadResults ArrayProcessor::results;

void ArrayProcessor::ValidateArray(const std::vector<int>& arr) {
    if (arr.empty()) {
        throw EmptyArrayException();
    }
}

DWORD WINAPI ArrayProcessor::FindMinMaxThread(LPVOID lpParam) {
    auto* arr = reinterpret_cast<std::vector<int>*>(lpParam);
    int min = (*arr)[0], max = (*arr)[0];

    for (int num : *arr) {
        if (num < min) min = num;
        if (num > max) max = num;
        Sleep(kMinMaxSleepMs);
    }

    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "Min: " << min << "\nMax: " << max << std::endl;
    }

    results.min = min;
    results.max = max;
    return 0;
}

DWORD WINAPI ArrayProcessor::CalculateAverageThread(LPVOID lpParam) {
    auto* arr = reinterpret_cast<std::vector<int>*>(lpParam);
    double sum = 0;

    for (int num : *arr) {
        sum += num;
        Sleep(kAverageSleepMs);
    }

    double average = sum / arr->size();
    int roundedAverage = static_cast<int>(std::round(average));

    {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "Average: " << std::fixed << std::setprecision(2) << average
            << " (rounded to " << roundedAverage << ")" << std::endl;
    }

    results.average = roundedAverage;
    return 0;
}

void ArrayProcessor::ProcessArray(std::vector<int>& arr) {
    ValidateArray(arr);

    HANDLE hMinMax = CreateThread(nullptr, 0, FindMinMaxThread, &arr, 0, nullptr);
    HANDLE hAverage = CreateThread(nullptr, 0, CalculateAverageThread, &arr, 0, nullptr);

    if (hMinMax == nullptr || hAverage == nullptr) {
        throw std::runtime_error("Failed to create threads");
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    for (int& num : arr) {
        if (num == results.min || num == results.max) {
            num = results.average; 
        }
    }
}
