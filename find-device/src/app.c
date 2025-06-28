/**
 * @file app.c
 * @author Rakesh Kumar
 * @brief A project to find all bluetooth devices available.
 * @version 0.1.0
 * @date 2025-06-28
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

void print_bluetooth_device_info(const BLUETOOTH_DEVICE_INFO_STRUCT);

int main() {

    // get the handle to the first radio
    DWORD struct_BFRP_size = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
    BLUETOOTH_FIND_RADIO_PARAMS *pbtbfrp = HeapAlloc(GetProcessHeap(), 0, struct_BFRP_size);
    if(pbtbfrp == NULL) {
        wprintf(L"Heap alloc failed.\n");
        return -1; // safe to exit
    } // else continue
    pbtbfrp->dwSize = struct_BFRP_size;
    HANDLE hRadio = NULL;
    HBLUETOOTH_RADIO_FIND nextsHandle = BluetoothFindFirstRadio(pbtbfrp, &hRadio);
    if(nextsHandle == NULL) {
        wprintf(L"ERROR, first radio couldn't be found, code: %d\n", GetLastError());
        HeapFree(GetProcessHeap(), 0, pbtbfrp);
        return -1; // safe to exit
    } // else continue, we got the handle to the radio: hRadio


    // Enable discovery if not already
    if(BluetoothIsDiscoverable(hRadio) == FALSE) {
        // Enable discovery
        BOOL bt_discovery_enabled = BluetoothEnableDiscovery(hRadio, TRUE);
        switch (bt_discovery_enabled) {
        case TRUE:
            wprintf(L"Discovery enabled.\n");
            break;
        default:
            wprintf(L"Discovery couldn't be enabled.\n");
            break;
        }
    }



    // define search criteria
    DWORD struct_BDSP_size = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
    BLUETOOTH_DEVICE_SEARCH_PARAMS *pbtdsp = HeapAlloc(GetProcessHeap(), 0, struct_BDSP_size);
    if(pbtdsp == NULL) {
        wprintf(L"-FATAL- ERROR: %d\n", GetLastError());
        HeapFree(GetProcessHeap(), 0, pbtbfrp);
        return -1; // safe to exit, no memory occupied
    } // else simply continue
    pbtdsp->dwSize = struct_BDSP_size;
    pbtdsp->fReturnAuthenticated = TRUE;
    pbtdsp->fReturnRemembered = TRUE;
    pbtdsp->fReturnUnknown = TRUE;
    pbtdsp->fReturnConnected = TRUE;
    pbtdsp->fIssueInquiry = TRUE;
    pbtdsp->cTimeoutMultiplier = 48;
    pbtdsp->hRadio = hRadio;

    // device info structure
    DWORD struct_BDIS_size = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);
    BLUETOOTH_DEVICE_INFO_STRUCT *bdis = HeapAlloc(GetProcessHeap(), 0, struct_BDIS_size);
    if(bdis == NULL) {
        wprintf(L"Heap alloc failed.\n");
        HeapFree(GetProcessHeap(), 0, pbtbfrp);
        HeapFree(GetProcessHeap(), 0, pbtdsp);
        return -1;
    }
    ZeroMemory(bdis, struct_BDIS_size);
    bdis->dwSize = struct_BDIS_size;
    bdis->ulClassofDevice = 0;

    // find first device
    HBLUETOOTH_DEVICE_FIND hbtdf = BluetoothFindFirstDevice(pbtdsp, bdis);
    if(hbtdf == NULL) {
        DWORD the_last_error = GetLastError();
        if(the_last_error == ERROR_NO_MORE_ITEMS) {
            wprintf(L"No bluetooth device found.");
        } else {
            wprintf(L"Couldn't find device, ERROR: %d\n", the_last_error);
        }
        HeapFree(GetProcessHeap(), 0, pbtbfrp);
        HeapFree(GetProcessHeap(), 0, pbtdsp);
        HeapFree(GetProcessHeap(), 0, bdis);
        return -1;
    }


    // print device info
    print_bluetooth_device_info(*bdis);
    // try to find other devices
    while(BluetoothFindNextDevice(hbtdf, bdis) == TRUE) {
        // print each device info if available
        wprintf(L"-----------------------------------------------------------------\n");
        print_bluetooth_device_info(*bdis);
    }

    // free heap occupied memories
    HeapFree(GetProcessHeap(), 0, pbtbfrp);
    HeapFree(GetProcessHeap(), 0, pbtdsp);
    HeapFree(GetProcessHeap(), 0, bdis);
    // Close all handles
    BluetoothFindDeviceClose(hbtdf);
    BluetoothFindRadioClose(nextsHandle);
    CloseHandle(hRadio);
    return 0;
}

void print_SYSTEMTIME(SYSTEMTIME systemTime) {
    wprintf(L"Year-%hu, Month-%hu, Day of Week-%hu, Day Date-%hu, %hu:%hu:%hu\n",
        systemTime.wYear,
        systemTime.wMonth,
        systemTime.wDayOfWeek,
        systemTime.wDay,
        systemTime.wHour,
        systemTime.wMinute,
        systemTime.wSecond
    );
}

void print_bluetooth_device_info(const BLUETOOTH_DEVICE_INFO_STRUCT bdis) {
    wprintf(L"Device name: %s\n", bdis.szName);

    wprintf(L"Last Seen: ");
    print_SYSTEMTIME(bdis.stLastSeen);

    wprintf(L"Last Used: ");
    print_SYSTEMTIME(bdis.stLastUsed);

    if(bdis.fAuthenticated == TRUE) {
        wprintf(L"This device is authenticated.\n");
    } else {
        wprintf(L"This device is not authenticated.\n");
    }

    if(bdis.fRemembered == TRUE) {
        wprintf(L"This device was remembered.\n");
    } else {
        wprintf(L"This device was not remembered.\n");
    }

    if(bdis.fConnected == TRUE) {
        wprintf(L"This device is connected right now.\n");
    } else {
        wprintf(L"This device is not connected right now.\n");
    }

    wprintf(L"Class of Device: %ul\n", bdis.ulClassofDevice);

    wprintf(L"Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
            bdis.Address.rgBytes[5], // Most significant byte first (common display)
            bdis.Address.rgBytes[4],
            bdis.Address.rgBytes[3],
            bdis.Address.rgBytes[2],
            bdis.Address.rgBytes[1],
            bdis.Address.rgBytes[0]  // Least significant byte last
    );
}