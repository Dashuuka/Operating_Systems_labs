#pragma once
#include <windows.h>
#include "SharedResources.h" 

struct MarkerData {
    SharedResources* resources;
    int markerNumber;
    HANDLE startEvent;
    HANDLE blockedEvent;
    HANDLE responseEvent;
    char* terminateFlag; 
    CRITICAL_SECTION* flagsCS;
};

DWORD WINAPI MarkerThread(LPVOID param);