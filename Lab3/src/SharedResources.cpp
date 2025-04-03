#include "SharedResources.h"
#include <iostream>

SharedResources::SharedResources(size_t arraySize) : array_(arraySize, 0) {
    InitializeCriticalSection(&arrayCriticalSection_);
}

SharedResources::~SharedResources() {
    DeleteCriticalSection(&arrayCriticalSection_);
}

bool SharedResources::CheckAndMarkElement(size_t index, int markerNumber) {
    EnterCriticalSection(&arrayCriticalSection_);
    bool success = false;
    if (index < array_.size() && array_[index] == 0) {
        array_[index] = markerNumber;
        success = true;
    }
    LeaveCriticalSection(&arrayCriticalSection_);
    return success;
}

void SharedResources::UnmarkElements(int markerNumber) {
    EnterCriticalSection(&arrayCriticalSection_);
    for (auto& element : array_) {
        if (element == markerNumber) {
            element = 0;
        }
    }
    LeaveCriticalSection(&arrayCriticalSection_);
}

void SharedResources::PrintArray() const {
    EnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&arrayCriticalSection_));
    for (size_t i = 0; i < array_.size(); ++i) {
        std::cout << array_[i] << " ";
    }
    std::cout << std::endl;
    LeaveCriticalSection(const_cast<LPCRITICAL_SECTION>(&arrayCriticalSection_));
}

size_t SharedResources::GetArraySize() const {
    return array_.size();
}

int SharedResources::GetArrayElement(size_t index) const {
    EnterCriticalSection(const_cast<LPCRITICAL_SECTION>(&arrayCriticalSection_));
    int value = array_.at(index);
    LeaveCriticalSection(const_cast<LPCRITICAL_SECTION>(&arrayCriticalSection_));
    return value;
}