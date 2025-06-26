/**
 * @file main.c
 * @author Rakesh Kumar
 * @brief A program to find out number of bluetooth radios available on system.
 * @note Please maintain the order in which headers are imported because a preprocessor simply pastes the entire code 
 * and forgets what the hell their contexts and relationships are!
 * @version 0.1.0
 * @date 2025-06-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stdio.h>
#include <intsafe.h>
#include <initguid.h>
#include <bthsdpdef.h>

// // To work with bluetooth sockets
#include <WinSock2.h>
#include <ws2bth.h>

#include <BluetoothAPIs.h>

int main() {

    int numberOfRadiosFound = 0; // number of bluetooth radios attached to the system for example the radio that comes with the laptop plus any other bluetooth radio being used via usb etc.

    // pointer to a BLUETOOTH_FIND_RADIO_PARAMS structure
    BLUETOOTH_FIND_RADIO_PARAMS *pbtfrp = HeapAlloc(GetProcessHeap(), 0, sizeof(BLUETOOTH_FIND_RADIO_PARAMS));
    if(pbtfrp == NULL) {
        wprintf(L"-FATIAL- Heap alloc failed for BLUETOOTH_FIND_RADIO_PARAMS structure pointer.");
        return -1; // we have a safe option to exit here.
    } // else continue in source code
    pbtfrp->dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

    HANDLE phRadio = NULL;
    // Get first bluetooth radio
    HBLUETOOTH_RADIO_FIND nextsHandle = BluetoothFindFirstRadio(pbtfrp, &phRadio);
    if(nextsHandle == NULL) {
        DWORD lastError = GetLastError();
        wprintf(L"-Fatal- Operation failure: Couldn't find the first radio thus no next radio handle was obtained: %d\n", lastError);
        return -1; // we have a safe option to exit here.
    }
    ++numberOfRadiosFound;

    // Find next bluetooth radio
    BOOL isNextRadioNotAvailable = FALSE;
    while((isNextRadioNotAvailable = BluetoothFindNextRadio(pbtfrp, &phRadio)))
        ++numberOfRadiosFound;

    // print number of radios that were found..
    wprintf(L"Number of radios that were found = %d\n", numberOfRadiosFound);

    // close all handles
    // Close the enumeration handle for finding bluetooth radios
    if(!BluetoothFindRadioClose(nextsHandle)) {
        DWORD lastError = GetLastError();
        wprintf(L"-Fatal- Failure to close bluetooth radio handle: %d\n", lastError);
    }
    // close radio handle
    if(CloseHandle(phRadio) == 0) {
        DWORD lastError = GetLastError();
        wprintf(L"-Fatal- Failure to close radio handle: %d\n", lastError);
    }

    return 0;
}