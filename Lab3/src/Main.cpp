#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>
#include <memory>
#include "SharedResources.h"
#include "MarkerThread.h"

struct HandleDeleter {
    void operator()(HANDLE handle) const {
        if (handle != nullptr) CloseHandle(handle);
    }
};
using HandlePtr = std::unique_ptr<void, HandleDeleter>;

struct ThreadInfo {
    HandlePtr handle;
    int markerNumber;
    HandlePtr blockedEvent;
    HandlePtr responseEvent;
};

CRITICAL_SECTION flagsCS;
std::vector<char> terminateFlags;

int main() {
    InitializeCriticalSection(&flagsCS);
    try {
        size_t arraySize;
        std::cout << "Enter array size: ";
        std::cin >> arraySize;
        if (!std::cin || arraySize == 0) throw std::runtime_error("Invalid array size");

        size_t markersCount;
        std::cout << "Enter number of marker threads: ";
        std::cin >> markersCount;
        if (!std::cin || markersCount == 0) throw std::runtime_error("Invalid markers count");

        SharedResources resources(arraySize);
        terminateFlags.resize(markersCount, 0);

        HandlePtr startEvent(CreateEvent(nullptr, TRUE, FALSE, nullptr));
        if (!startEvent) throw std::runtime_error("Failed to create start event");

        std::vector<ThreadInfo> activeThreads;
        for (int i = 0; i < markersCount; ++i) {
            const int markerNumber = i + 1;
            HandlePtr blockedEvent(CreateEvent(nullptr, TRUE, FALSE, nullptr));
            HandlePtr responseEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr));
            if (!blockedEvent || !responseEvent) throw std::runtime_error("Failed to create events");

            auto* data = new MarkerData{
                &resources,
                markerNumber,
                startEvent.get(),
                blockedEvent.get(),
                responseEvent.get(),
                &terminateFlags[i],
                &flagsCS
            };

            HandlePtr thread(CreateThread(
                nullptr,
                0,
                MarkerThread,
                static_cast<LPVOID>(data),
                0,
                nullptr 
            ));
            if (!thread) {
                delete data;
                throw std::runtime_error("Failed to create thread");
            }

            activeThreads.push_back({
                std::move(thread), markerNumber,
                std::move(blockedEvent), std::move(responseEvent)
                });
        }

        SetEvent(startEvent.get());

        while (!activeThreads.empty()) {
            std::vector<HANDLE> blockedEvents;
            for (const auto& thread : activeThreads)
                blockedEvents.push_back(thread.blockedEvent.get());

            DWORD count = static_cast<DWORD>(blockedEvents.size());
            if (WaitForMultipleObjects(count, blockedEvents.data(), TRUE, INFINITE) == WAIT_FAILED)
                throw std::runtime_error("WaitForMultipleObjects failed");

            for (const auto& thread : activeThreads)
                ResetEvent(thread.blockedEvent.get());

            resources.PrintArray();

            int markerToTerminate;
            std::cout << "Enter marker number to terminate: ";
            std::cin >> markerToTerminate;
            if (!std::cin) throw std::runtime_error("Invalid input");

            auto it = std::find_if(activeThreads.begin(), activeThreads.end(),
                [&](const ThreadInfo& t) { return t.markerNumber == markerToTerminate; });
            if (it == activeThreads.end()) throw std::runtime_error("Invalid marker number");

            EnterCriticalSection(&flagsCS);
            terminateFlags[it->markerNumber - 1] = 1; 
            LeaveCriticalSection(&flagsCS);

            SetEvent(it->responseEvent.get());
            WaitForSingleObject(it->handle.get(), INFINITE);
            activeThreads.erase(it);

            resources.PrintArray();

            for (const auto& thread : activeThreads)
                SetEvent(thread.responseEvent.get());
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        DeleteCriticalSection(&flagsCS);
        return 1;
    }

    DeleteCriticalSection(&flagsCS);
    return 0;
}