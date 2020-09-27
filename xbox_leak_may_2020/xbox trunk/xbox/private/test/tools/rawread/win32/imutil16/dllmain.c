#include <windows.h>
#include "..\debug.h"


BOOL WINAPI imutil_ThunkConnect16(LPSTR lpszDll16, LPSTR lpszDll32, WORD hInst, DWORD dwReason);


BOOL WINAPI DllEntryPoint(DWORD dwReason, HINSTANCE hInst, WORD wDS, WORD wHeapSize, DWORD dwReserved1, WORD wReserved2)
{
    static DWORD                dwRefCount = 0;

    if(dwReason)
    {
        DPF("DLL_PROCESS_ATTACH: 0x%8.8lX, 0x%4.4X, 0x%4.4X, 0x%4.4X, 0x%8.8lX, 0x%4.4X", hInst, dwReason, wDS, wHeapSize, dwReserved1, wReserved2);
        dwRefCount++;
    }
    else
    {
        DPF("DLL_PROCESS_DETACH: 0x%8.8lX, 0x%4.4X, 0x%4.4X, 0x%4.4X, 0x%8.8lX, 0x%4.4X", hInst, dwReason, wDS, wHeapSize, dwReserved1, wReserved2);
        dwRefCount--;

        if(!dwRefCount)
        {
            DPF("IMUTIL16 going away!");
        }
    }

    return imutil_ThunkConnect16("IMUTIL16.DLL", "IMUTIL32.DLL", hInst, dwReason);
}