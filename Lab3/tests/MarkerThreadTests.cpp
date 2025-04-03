#include "gtest/gtest.h"
#include "SharedResources.h"
#include "MarkerThread.h"
#include <windows.h>
#include <thread>
#include <vector>

// Тесты для класса SharedResources
TEST(SharedResourcesTest, ConstructorInitializesZeros) {
    const size_t size = 5;
    SharedResources resources(size);
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(resources.GetArrayElement(i), 0);
    }
}

TEST(SharedResourcesTest, CheckAndMarkElementSuccess) {
    SharedResources resources(5);
    bool success = resources.CheckAndMarkElement(2, 3);
    EXPECT_TRUE(success);
    EXPECT_EQ(resources.GetArrayElement(2), 3);
}

TEST(SharedResourcesTest, CheckAndMarkElementFailure) {
    SharedResources resources(5);
    resources.CheckAndMarkElement(2, 3);
    bool success = resources.CheckAndMarkElement(2, 4);
    EXPECT_FALSE(success);
    EXPECT_EQ(resources.GetArrayElement(2), 3);
}

TEST(SharedResourcesTest, UnmarkElements) {
    SharedResources resources(5);
    resources.CheckAndMarkElement(0, 2);
    resources.CheckAndMarkElement(1, 2);
    resources.CheckAndMarkElement(2, 3);
    resources.UnmarkElements(2);
    EXPECT_EQ(resources.GetArrayElement(0), 0);
    EXPECT_EQ(resources.GetArrayElement(1), 0);
    EXPECT_EQ(resources.GetArrayElement(2), 3);
}

TEST(SharedResourcesTest, GetArraySize) {
    const size_t size = 10;
    SharedResources resources(size);
    EXPECT_EQ(resources.GetArraySize(), size);
}

TEST(SharedResourcesTest, GetArrayElement) {
    SharedResources resources(5);
    resources.CheckAndMarkElement(3, 5);
    EXPECT_EQ(resources.GetArrayElement(3), 5);
}

// Тесты для потока MarkerThread
TEST(MarkerThreadTest, TerminateSignalClearsMarks) {
    const int markerNumber = 1;
    const size_t arraySize = 5;
    SharedResources resources(arraySize);

    HANDLE startEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    HANDLE blockedEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    HANDLE responseEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    char terminateFlag = 0;
    CRITICAL_SECTION flagsCS;
    InitializeCriticalSection(&flagsCS);

    MarkerData* data = new MarkerData{
        &resources,
        markerNumber,
        startEvent,
        blockedEvent,
        responseEvent,
        &terminateFlag,
        &flagsCS
    };

    DWORD threadId;
    HANDLE hThread = CreateThread(
        nullptr,
        0,
        MarkerThread,
        data,
        0,
        &threadId
    );
    ASSERT_NE(hThread, nullptr);

    SetEvent(startEvent);

    Sleep(50);

    resources.LockArray();
    for (size_t i = 0; i < arraySize; ++i) {
        if (resources.GetArrayElement(i) == 0) {
            resources.CheckAndMarkElement(i, 99);
        }
    }
    resources.UnlockArray();

    WaitForSingleObject(blockedEvent, INFINITE);

    EnterCriticalSection(&flagsCS);
    terminateFlag = 1;
    LeaveCriticalSection(&flagsCS);

    SetEvent(responseEvent);

    WaitForSingleObject(hThread, INFINITE);

    for (size_t i = 0; i < arraySize; ++i) {
        EXPECT_NE(resources.GetArrayElement(i), markerNumber);
    }

    CloseHandle(hThread);
    CloseHandle(startEvent);
    CloseHandle(blockedEvent);
    CloseHandle(responseEvent);
    DeleteCriticalSection(&flagsCS);
}

TEST(MarkerThreadTest, ThreadSignalsBlockedWhenCannotMark) {
    const int markerNumber = 2;
    const size_t arraySize = 1;
    SharedResources resources(arraySize);
    resources.CheckAndMarkElement(0, 99); 

    HANDLE startEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    HANDLE blockedEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    HANDLE responseEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    char terminateFlag = 0;
    CRITICAL_SECTION flagsCS;
    InitializeCriticalSection(&flagsCS);

    MarkerData* data = new MarkerData{
        &resources,
        markerNumber,
        startEvent,
        blockedEvent,
        responseEvent,
        &terminateFlag,
        &flagsCS
    };

    DWORD threadId;
    HANDLE hThread = CreateThread(
        nullptr,
        0,
        MarkerThread,
        data,
        0,
        &threadId
    );
    ASSERT_NE(hThread, nullptr);

    SetEvent(startEvent);

    DWORD waitResult = WaitForSingleObject(blockedEvent, 1000); 
    ASSERT_EQ(waitResult, WAIT_OBJECT_0);

    EnterCriticalSection(&flagsCS);
    terminateFlag = 1;
    LeaveCriticalSection(&flagsCS);
    SetEvent(responseEvent);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(startEvent);
    CloseHandle(blockedEvent);
    CloseHandle(responseEvent);
    DeleteCriticalSection(&flagsCS);
}