#pragma once
#include <vector>
#include <windows.h>

class SharedResources {
public:
    explicit SharedResources(size_t arraySize);
    ~SharedResources();

    bool CheckAndMarkElement(size_t index, int markerNumber);
    void UnmarkElements(int markerNumber);
    void PrintArray() const;
    size_t GetArraySize() const;
    int GetArrayElement(size_t index) const;

    void LockArray() { EnterCriticalSection(&arrayCriticalSection_); }
    void UnlockArray() { LeaveCriticalSection(&arrayCriticalSection_); }

private:
    std::vector<int> array_;
    CRITICAL_SECTION arrayCriticalSection_; 
};