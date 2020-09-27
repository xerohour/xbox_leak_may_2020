#include <windows.h>
#include "checkver.h"

#define lmalloc(_usize)     LocalAlloc(LPTR, _usize)
#define lfree(_ptr)         LocalFree(_ptr)

#define VER_ERROR_MSG       "Your copy of %s is old.\r\n\r\nYour version:  %u.%u.%u.%u\r\nRequired version:  %u.%u.%u.%u"

#ifndef _XBOX // GetFileVersionInfo not supported
BOOL WINAPI GetLibraryVersion(LPCSTR lpszPath, LPCSTR lpszFile, LPDWORD lpdwVersionMS, LPDWORD lpdwVersionLS, BOOL *lpbDebug)
{
    char                    szFullPath[MAX_PATH + 1];
    LPVOID                  lpvVersionInfo;
    DWORD                   dwSize;
    DWORD                   dwReserved;
    VS_FIXEDFILEINFO *      lpFFI;
    UINT                    uSize;
    
    if(!lpszFile)
    {
        return FALSE;
    }
    
    if(lpszPath)
    {
        lstrcpy(szFullPath, lpszPath);
    }
    else
    {
        if(!GetSystemDirectory(szFullPath, sizeof(szFullPath)))
        {
            return FALSE;
        }
    }

    if(szFullPath[lstrlen(szFullPath) - 1] != '\\')
    {
        lstrcat(szFullPath, "\\");
    }

    lstrcat(szFullPath, lpszFile);

    if(!(dwSize = GetFileVersionInfoSize(szFullPath, &dwReserved)))
    {
        return FALSE;
    }

    if(!(lpvVersionInfo = lmalloc(dwSize)))
    {
        return FALSE;
    }

    if(!GetFileVersionInfo(szFullPath, 0, dwSize, lpvVersionInfo))
    {
        lfree(lpvVersionInfo);
        return FALSE;
    }

    if(!VerQueryValue(lpvVersionInfo, "\\", (LPVOID *)&lpFFI, &uSize))
    {
        lfree(lpvVersionInfo);
        return FALSE;
    }

	if (lpbDebug)
	{
		if (lpFFI->dwFileFlags & VS_FF_DEBUG)
			*lpbDebug	= TRUE;
		else
			*lpbDebug	= FALSE;
	}

    if(lpdwVersionMS)
    {
        *lpdwVersionMS = lpFFI->dwFileVersionMS;
    }

    if(lpdwVersionLS)
    {
        *lpdwVersionLS = lpFFI->dwFileVersionLS;
    }

    lfree(lpvVersionInfo);

    return TRUE;
}
#endif // ! XBOX

#ifndef _XBOX // GetFileVersionInfo not supported
BOOL WINAPI CheckLibraryVersion(HWND hWnd, LPCSTR lpszPath, LPCSTR lpszFile, DWORD dwMinVersionMS, DWORD dwMinVersionLS, BOOL fDialog)
{
    DWORD                   dwVersionMS, dwVersionLS;
    char                    szMessage[512];
    BOOL                    fOld = TRUE;

    if(!GetLibraryVersion(lpszPath, lpszFile, &dwVersionMS, &dwVersionLS, NULL))
    {
        return FALSE;
    }

    if(HIWORD(dwVersionMS) > HIWORD(dwMinVersionMS))
    {
        fOld = FALSE;
    }
    else
    {
        if(LOWORD(dwVersionMS) > LOWORD(dwMinVersionMS))
        {
            fOld = FALSE;
        }
        else
        {
            if(HIWORD(dwVersionLS) > HIWORD(dwMinVersionLS))
            {
                fOld = FALSE;
            }
            else
            {
                if(LOWORD(dwVersionLS) >= LOWORD(dwMinVersionLS))
                {
                    fOld = FALSE;
                }
            }
        }
    }
    
    if(fOld)
    {
        if(fDialog)
        {
            wsprintf(szMessage, VER_ERROR_MSG, lpszFile, HIWORD(dwVersionMS), LOWORD(dwVersionMS), HIWORD(dwVersionLS), LOWORD(dwVersionLS), HIWORD(dwMinVersionMS), LOWORD(dwMinVersionMS), HIWORD(dwMinVersionLS), LOWORD(dwMinVersionLS));

			MessageBeep(MB_ICONHAND);
            MessageBox(hWnd, szMessage, NULL, MB_OK | MB_ICONHAND | MB_TASKMODAL);
        }

        return FALSE;
    }

    return TRUE;
}
#endif // ! XBOX