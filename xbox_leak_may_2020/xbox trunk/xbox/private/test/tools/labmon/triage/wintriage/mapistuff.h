#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>
#include <mapi.h>	

BOOL SendOffFailure(HWND hWnd, TCHAR *pszToList, TCHAR *pszTitle, TCHAR *pszMessage);
DWORD CountRecips(PTCHAR pszToList);
BOOL GetLibraryPointers(HWND hWnd);
