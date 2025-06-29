/**
 * @file app.c
 * @author Rakesh Kumar
 * @brief A terminal app to check whether a bluetooth radio or radios is connectable.
 * @version 0.1.0
 * @date 2025-06-29
 * 
 * 
 */
#include <stdio.h>
#include <initguid.h>
#include <intsafe.h>
#include <strsafe.h>
#include <WinSock2.h>
#include <bthsdpdef.h>
#include <bluetoothapis.h>

#define switch_(expression, case_all_on_underscore) { \
    DWORD _ = ##expression; \
    switch(_) { \
        ##case_all_on_underscore \
    } \
}

int main() {
    // get handle to radios and perform checks on each one of them
    // starting with the first radio:
    DWORD sizeof_BLUETOOTH_FIND_RADIO_PARAMS = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
    BLUETOOTH_FIND_RADIO_PARAMS btfrp;
    btfrp.dwSize = sizeof_BLUETOOTH_FIND_RADIO_PARAMS;
    HANDLE hRadio = NULL;

    HBLUETOOTH_RADIO_FIND handle_for_next = BluetoothFindFirstRadio(&btfrp, &hRadio);
    if(handle_for_next == NULL) {
        switch_(GetLastError(), 
            case ERROR_INVALID_PARAMETER: // major fault
                wprintf(L"pbtfrp parameter is NULL.\n");
                return -1; // safe to exit
            case ERROR_REVISION_MISMATCH: // major fault
                wprintf(L"The pbtfrp structure is not the right length.\n");
                return -1; // safe to exit
            case ERROR_OUTOFMEMORY: // major fault
                wprintf(L"Out of memory.\n");
                return -1; // safe to exit
            default:
                wprintf(L"Exiting, unknown error occurred, error code: %d.\n", _);
        )
    } // else simply proceed..

    // we got our first radio, check on connectivity
    wprintf(L"The first radio is ");
    if(BluetoothIsConnectable(hRadio) == FALSE) {
        wprintf(L"not connectable.\n");
    } else {
        wprintf(L"connectable.\n");
    }


    // now let us move on to finding other radios and check their connectivity
    while((BluetoothFindNextRadio(handle_for_next, &hRadio)) == TRUE) {
        wprintf(L"The first radio is ");
        if(BluetoothIsConnectable(hRadio) == FALSE) {
            wprintf(L"not connectable.\n");
        } else {
            wprintf(L"connectable.\n");
        }
    }
    if(handle_for_next == NULL) {
        switch_(GetLastError(), 
            case ERROR_INVALID_PARAMETER: // major fault
                wprintf(L"pbtfrp parameter is NULL.\n");
                return -1; // safe to exit
            case ERROR_REVISION_MISMATCH: // major fault
                wprintf(L"The pbtfrp structure is not the right length.\n");
                return -1; // safe to exit
            case ERROR_OUTOFMEMORY: // major fault
                wprintf(L"Out of memory.\n");
                return -1; // safe to exit
            default:
                wprintf(L"Exiting, unknown error occurred, error code: %d.\n", _);
        )
    } // else simply proceed..

    wprintf(L"No more bluetooth radios found.\n");
    

    return 0;
}