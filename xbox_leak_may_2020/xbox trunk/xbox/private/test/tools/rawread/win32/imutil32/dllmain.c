#include <windows.h>
#include "..\debug.h"


BOOL WINAPI imutil_ThunkConnect32(LPSTR lpszDll16, LPSTR lpszDll32, DWORD hInst, DWORD dwReason);


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, DWORD dwReserved)
{
    static DWORD                dwRefCount = 0;

    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
            DPF("DLL_PROCESS_ATTACH: 0x%8.8lX, 0x%8.8lX, 0x%8.8lX", hInst, dwReason, dwReserved);
            dwRefCount++;
            break;

        case DLL_THREAD_ATTACH:
            DPF("DLL_THREAD_ATTACH: 0x%8.8lX, 0x%8.8lX, 0x%8.8lX", hInst, dwReason, dwReserved);
            break;

        case DLL_PROCESS_DETACH:
            DPF("DLL_PROCESS_DETACH: 0x%8.8lX, 0x%8.8lX, 0x%8.8lX", hInst, dwReason, dwReserved);
            dwRefCount--;

            if(!dwRefCount)
            {
                DPF("IMUTIL32 going away!");
            }

            break;

        case DLL_THREAD_DETACH:
             DPF("DLL_THREAD_DETACH: 0x%8.8lX, 0x%8.8lX, 0x%8.8lX", hInst, dwReason, dwReserved);
             break;
    }
   
    return imutil_ThunkConnect32("IMUTIL16.DLL", "IMUTIL32.DLL", (DWORD)hInst, dwReason);
}