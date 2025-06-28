/**
 * @file app.c
 * @author Rakesh Kumar
 * @brief A project to demonstrate how to retrieve a device info.
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

void print_device_info(const BLUETOOTH_DEVICE_INFO_STRUCT);

int main() {
    // get the first radio
    DWORD sizeof_BLUETOOTH_FIND_RADIO_PARAMS = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
    BLUETOOTH_FIND_RADIO_PARAMS *pbtfrps = HeapAlloc(GetProcessHeap(), 0, sizeof_BLUETOOTH_FIND_RADIO_PARAMS);
    if(pbtfrps == NULL) {
        wprintf(L"ALLOCATION FAILURE. FALLING...\n");
        return -1;
    }
    pbtfrps->dwSize = sizeof_BLUETOOTH_FIND_RADIO_PARAMS;

    HANDLE hRadio = NULL;
    HBLUETOOTH_RADIO_FIND handle_to_next_find = BluetoothFindFirstRadio(pbtfrps, &hRadio);
    if(handle_to_next_find == NULL) {
        DWORD the_last_error = GetLastError();
        if(the_last_error == ERROR_NO_MORE_ITEMS) {
            wprintf(L"No bluetooth devices found!\n");
        } else {
            wprintf(L"Bluetooth radio not found, error description: %d.\n", the_last_error);
            // Remove the necessary allocation(s) before exiting
            HeapFree(GetProcessHeap(), 0, pbtfrps);
        }
        return -1;
    }


    // we got our first radio here (if every steps above this line ran perfectly)
    DWORD sizeof_BLUETOOTH_DEVICE_INFO_STRUCT = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);
    BLUETOOTH_DEVICE_INFO_STRUCT *pbtdis = HeapAlloc(GetProcessHeap(), 0, sizeof_BLUETOOTH_DEVICE_INFO_STRUCT);
    if(pbtdis == NULL) {
        wprintf(L"ALLOCATION FAILURE. FALLING...\n");
        // Remove the necessary allocation(s) before exiting and free the handles
        BluetoothFindRadioClose(handle_to_next_find);
        CloseHandle(hRadio);
        HeapFree(GetProcessHeap(), 0, pbtfrps);
        return -1; // safe to exit now
    }
    ZeroMemory(pbtdis, sizeof_BLUETOOTH_DEVICE_INFO_STRUCT);
    pbtdis->dwSize = sizeof_BLUETOOTH_DEVICE_INFO_STRUCT;

    BLUETOOTH_ADDRESS *pbth_addr_struct = HeapAlloc(GetProcessHeap(), 0, sizeof(BLUETOOTH_ADDRESS));
    if(pbth_addr_struct == NULL) {
        wprintf(L"ALLOCATION FAILURE. FALLING...\n");
        // Remove the necessary allocation(s) before exiting and free the handles
        BluetoothFindRadioClose(handle_to_next_find);
        CloseHandle(hRadio);
        HeapFree(GetProcessHeap(), 0, pbtdis);
        HeapFree(GetProcessHeap(), 0, pbtfrps);
        return -1; // safe to exit now
    }
    pbth_addr_struct->ullLong = BLUETOOTH_NULL_ADDRESS;
    /*
      Write the MAC address of the bluetooth device you want to have info of in reverse order i.e. in the following way:
      pbth_addr_struct->rgBytes[5] = 0xAB;
      pbth_addr_struct->rgBytes[4] = 0xCD;
      pbth_addr_struct->rgBytes[3] = 0xEF;
      pbth_addr_struct->rgBytes[2] = 0xGH;
      pbth_addr_struct->rgBytes[1] = 0xIJ;
      pbth_addr_struct->rgBytes[0] = 0xKL;

      Ofcourse letters beyond F don't make sense in hex. It is just representational.
      The order in which you read the MAC address of the bluetooth device you are trying to fetch info of, you place it in reverse here.
     */
    pbtdis->Address = *pbth_addr_struct;

    DWORD return_value = BluetoothGetDeviceInfo(hRadio, pbtdis);
    switch (return_value) {
        case ERROR_SUCCESS:
            wprintf(L"Found the device info, printing it...\n");
            print_device_info(*pbtdis);
            break;
        case ERROR_REVISION_MISMATCH:
            wprintf(L"The size of the BLUETOOTH_DEVICE_INFO_STRUCT is not compatible.\n");
            break;
        case ERROR_NOT_FOUND:
            wprintf(L"The radio is not known by the system, or the Address member of the BLUETOOTH_DEVICE_INFO_STRUCT structure is all zeros.");
            break;
        case ERROR_INVALID_PARAMETER:
            wprintf(L"The pbtdis parameter is NULL.\n");
            break;
        default:
            wprintf(L"A default switch has been triggered. Debugging recommended.\n");
            break;
    }

    // Free the handles in reverse to the initiation order
    BluetoothFindRadioClose(handle_to_next_find);
    CloseHandle(hRadio);
    // Free the heap allocations
    HeapFree(GetProcessHeap(), 0, pbtfrps);
    HeapFree(GetProcessHeap(), 0, pbth_addr_struct);
    HeapFree(GetProcessHeap(), 0, pbtdis);


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

void print_device_info(const BLUETOOTH_DEVICE_INFO_STRUCT btdi) {
    wprintf(L"Device name: %s\n", btdi.szName);

    wprintf(L"Address: ", btdi.Address);

    wprintf(L"Last Seen: ");
    print_SYSTEMTIME(btdi.stLastSeen);

    wprintf(L"Last Used: ");
    print_SYSTEMTIME(btdi.stLastUsed);

    if(btdi.fAuthenticated == TRUE) {
        wprintf(L"This device is authenticated.\n");
    } else {
        wprintf(L"This device is not authenticated.\n");
    }

    if(btdi.fRemembered == TRUE) {
        wprintf(L"This device was remembered.\n");
    } else {
        wprintf(L"This device was not remembered.\n");
    }

    if(btdi.fConnected == TRUE) {
        wprintf(L"This device is connected right now.\n");
    } else {
        wprintf(L"This device is not connected right now.\n");
    }

    wprintf(L"Class of Device: %ul\n", btdi.ulClassofDevice);

    wprintf(L"Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
            btdi.Address.rgBytes[5], // Most significant byte first (common display)
            btdi.Address.rgBytes[4],
            btdi.Address.rgBytes[3],
            btdi.Address.rgBytes[2],
            btdi.Address.rgBytes[1],
            btdi.Address.rgBytes[0]  // Least significant byte last
    );
}