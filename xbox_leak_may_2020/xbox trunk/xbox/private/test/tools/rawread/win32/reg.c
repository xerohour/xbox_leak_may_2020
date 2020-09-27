#include <windows.h>
#include "..\defs.h"
#include "..\image.h"
#include "reg.h"
#include "winmisc.h"


BOOL GetReg(HKEY hKeyParent, LPSTR lpszKey, LPSTR lpszValue, DWORD dwValType, LPVOID lpvData, DWORD dwDataSize)
{
    HKEY                    hKey = hKeyParent;

    if(lpszValue)
    {
        if(lpszKey)
            if(RegOpenKey(hKeyParent, lpszKey, &hKey))
                return FALSE;

        if(RegQueryValueEx(hKey, lpszValue, NULL, &dwValType, (LPBYTE)lpvData, &dwDataSize))
            return FALSE;

        RegCloseKey(hKey);
    }
    else
    {
        if(RegQueryValue(hKeyParent, lpszKey, (LPBYTE)lpvData, (PLONG)&dwDataSize))
            return FALSE;
    }
        
    return TRUE;
}


BOOL PutReg(HKEY hKeyParent, LPSTR lpszKey, LPSTR lpszValue, DWORD dwValType, LPVOID lpvData, DWORD dwDataSize)
{
    HKEY                    hKey = hKeyParent;

    if(lpszValue)
    {
        if(lpszKey)
            if(RegCreateKey(hKeyParent, lpszKey, &hKey))
                return FALSE;

        if(RegSetValueEx(hKey, lpszValue, 0, dwValType, (LPBYTE)lpvData, dwDataSize))
            return FALSE;

        RegCloseKey(hKey);
    }
    else
    {
        if(RegSetValue(hKeyParent, lpszKey, REG_SZ, (LPBYTE)lpvData, dwDataSize))
            return FALSE;
    }
        
    return TRUE;
}


void GetRegWindowPos(HWND hWnd)
{
    RECT                    rc;

    if(GetReg(REG_KEY_PARENT, REG_KEY, REG_VAL_WINDOWPOS, REG_TYPE_WINDOWPOS, &rc, sizeof(rc)))
    {
        SetWindowPos(hWnd, NULL, rc.left, rc.top, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
    }
    else
    {
        CenterWindow(hWnd, NULL);
    }
}


void PutRegWindowPos(HWND hWnd)
{
    WINDOWPLACEMENT         wp;

    wp.length = sizeof(wp);
    GetWindowPlacement(hWnd, &wp);
    PutReg(REG_KEY_PARENT, REG_KEY, REG_VAL_WINDOWPOS, REG_TYPE_WINDOWPOS, &(wp.rcNormalPosition), sizeof(wp.rcNormalPosition));
}


void GetRegOptions(LPIMAGESTRUCT lpImage)
{
    GetReg(REG_KEY_PARENT, REG_KEY, REG_VAL_DRIVE, REG_TYPE_DRIVE, &(lpImage->bDrive), sizeof(lpImage->bDrive));
    GetReg(REG_KEY_PARENT, REG_KEY, REG_VAL_FILE, REG_TYPE_FILE, lpImage->szFile, sizeof(lpImage->szFile));
    GetReg(REG_KEY_PARENT, REG_KEY, REG_VAL_OPTIONS, REG_TYPE_OPTIONS, &(lpImage->wFlags), sizeof(lpImage->wFlags));

#ifdef RAWREAD

    GetReg(REG_KEY_PARENT, REG_KEY, REG_VAL_MAXFRAGSIZE, REG_TYPE_MAXFRAGSIZE, &(lpImage->dwMaxFragSize), sizeof(lpImage->dwMaxFragSize));

#endif // RAWREAD

}


void SetRegOptions(LPIMAGESTRUCT lpImage)
{
    PutReg(REG_KEY_PARENT, REG_KEY, REG_VAL_DRIVE, REG_TYPE_DRIVE, &(lpImage->bDrive), sizeof(lpImage->bDrive));
    PutReg(REG_KEY_PARENT, REG_KEY, REG_VAL_FILE, REG_TYPE_FILE, lpImage->szFile, sizeof(lpImage->szFile));
    PutReg(REG_KEY_PARENT, REG_KEY, REG_VAL_OPTIONS, REG_TYPE_OPTIONS, &(lpImage->wFlags), sizeof(lpImage->wFlags));

#ifdef RAWREAD

    PutReg(REG_KEY_PARENT, REG_KEY, REG_VAL_MAXFRAGSIZE, REG_TYPE_MAXFRAGSIZE, &(lpImage->dwMaxFragSize), sizeof(lpImage->dwMaxFragSize));

#endif // RAWREAD

}