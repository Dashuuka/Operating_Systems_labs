#include "MarkerThread.h"
#include "SharedResources.h"
#include <windows.h>
#include <iostream>

DWORD WINAPI MarkerThread(LPVOID param) {
    MarkerData* data = static_cast<MarkerData*>(param);
    SharedResources* resources = data->resources;
    const int markerNumber = data->markerNumber;
    
    WaitForSingleObject(data->startEvent, INFINITE);
    
    srand(markerNumber);
    bool terminate = false;
    
    while (!terminate) {
        const int randomValue = rand();
        const size_t index = static_cast<size_t>(randomValue) % resources->GetArraySize();

        if (resources->CheckAndMarkElement(index, markerNumber)) {
            Sleep(5);
            if (resources->GetArrayElement(index) != markerNumber) continue;
            Sleep(5);
        } else {
            std::cout << "Marker " << markerNumber 
                     << " cannot mark index " << index 
                     << std::endl;
            
            SetEvent(data->blockedEvent);
            
            WaitForSingleObject(data->responseEvent, INFINITE);
            
            EnterCriticalSection(data->flagsCS);
            terminate = *data->terminateFlag;
            LeaveCriticalSection(data->flagsCS);
            
            ResetEvent(data->blockedEvent);
        }
    }
    
    resources->UnmarkElements(markerNumber);
    delete data;
    return 0;
}