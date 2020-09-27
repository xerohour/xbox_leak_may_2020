
//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1996 - 1996
//
//  File:       crtem.h
//
//  Contents:   'C' Run Time Emulation Definitions
//
//  History:	03-Jun-96   philh   created
//--------------------------------------------------------------------------

#ifndef __CRTEM_H__
#define __CRTEM_H__


///////////////////////////////////////////////////////////////////////
//
// Definitions that help reduce our dependence on the C runtimes
//
#ifndef UNDER_CE
#define wcslen(sz)      lstrlenW(sz)            // yes it IS implemented by Win95

#define strlen(sz)      lstrlenA(sz)
#define strcpy(s1,s2)   lstrcpyA(s1,s2)
#define strcmp(s1,s2)   lstrcmpA(s1,s2)
#define _stricmp(s1,s2)  lstrcmpiA(s1,s2)
#define strcat(s1,s2)   lstrcatA(s1,s2)
#endif

///////////////////////////////////////////////////////////////////////
//
// C runtime excluders that we only use in non-debug builds
//

////////////////////////////////////////////
//
// enable intrinsics that we can (NOT on WinCE)
/////////////////////////////////

////////////////////////////////////////////
//
// memory management
//
#define malloc(cb)          ((void*)LocalAlloc(LPTR, cb))
#define free(pv)            (LocalFree((HLOCAL)pv))
#define realloc(pv, cb)     ((void*)LocalReAlloc((HLOCAL)pv, cb, LMEM_MOVEABLE))

#ifndef _XBOX

#ifdef CE_BUILD

#ifdef UNDER_CE
#define MODULE_NAME TEXT("WINCRYPT")
#define assert(x) DBGCHK(MODULE_NAME,(x))
// assume called with bAlertable == FALSE
#define WaitForSingleObjectEx(x, y, bAlertable) WaitForSingleObject(x, y)
#define WaitForMultipleObjectsEx(cWait,rghWait,f,t,bAlertable) WaitForMultipleObjects(cWait,rghWait,f,t)
// grossly simplified implementation of CompareStringA - only correctly returns 2 for equality
#define CompareStringA(lcid, flags, cs1, len1, cs2, len2) ((len1 == len2 && !_strnicmp(cs1,cs2,len1)) ? 2 : 1)
LONG RegEnumKeyA(  HKEY hKey,     // handle to key to query
  DWORD dwIndex, // index of subkey to query
  LPSTR lpName, // address of buffer for subkey name
  DWORD cbName   // size of subkey buffer
  );
LONG
APIENTRY
CERegCreateKeyExA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey,
    IN DWORD Reserved,
    IN LPSTR lpClass,
    IN DWORD dwOptions,
    IN REGSAM samDesired,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    OUT PHKEY phkResult,
    OUT LPDWORD lpdwDisposition
    );
LONG
APIENTRY
CERegOpenKeyExA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey,
    IN DWORD ulOptions,
    IN REGSAM samDesired,
    OUT PHKEY phkResult
    );
LONG
APIENTRY
CERegDeleteKeyA (
    IN HKEY hKey,
    IN LPCSTR lpSubKey
    );
LONG
APIENTRY
CERegSetValueExA (
    IN HKEY hKey,
    IN LPCSTR lpValueName,
    IN DWORD Reserved,
    IN DWORD dwType,
    IN CONST BYTE* lpData,
    IN DWORD cbData
    );
#define RegCreateKeyExA CERegCreateKeyExA
#define RegOpenKeyExA CERegOpenKeyExA
#define RegDeleteKeyA CERegDeleteKeyA
#define RegSetValueExA CERegSetValueExA
#endif

#endif // CE_BUILD

#endif

#endif
