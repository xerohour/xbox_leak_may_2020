/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rsrcdata.cpp

Author:

    Matt Bronder

Description:

    Resource data list.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// Globals
//******************************************************************************

PRESOURCEDATA g_prdResourceList = NULL;

//******************************************************************************
_RESOURCEDATA::_RESOURCEDATA(LPCTSTR _szModule, LPCTSTR _szName, LPVOID _pvData, DWORD _dwSize) {

    _tcscpy(szModule, _szModule);
    _tcscpy(szName, _szName);
    pvData = _pvData;
    dwSize = _dwSize;
    prdNext = g_prdResourceList;
    g_prdResourceList = this;
}

//******************************************************************************
BOOL LoadResourceData(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize) {

    PRESOURCEDATA prd;
#ifdef UNDER_XBOX
    PVOID pvData;
#ifdef UNICODE
    char szBuffer[MAX_PATH];
#endif
    TCHAR szFullName[MAX_PATH];
    LPSTR szResource;
#endif
    TCHAR szModule[128];
    BOOL bRet = FALSE;

    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        return FALSE;
    }

    GetModuleName(GetTestInstance(), szModule, 128);

    for (prd = g_prdResourceList; prd; prd = prd->prdNext) {
#ifndef UNDER_XBOX
        if (!_tcscmp(szName, prd->szName)) {
#else
        if ((*prd->szModule == TEXT('\0') || !_tcscmp(szModule, prd->szModule)) && !_tcscmp(szName, prd->szName)) {
#endif // UNDER_XBOX
            if (ppvData) {
                *ppvData = prd->pvData;
            }
            if (pdwSize) {
                *pdwSize = prd->dwSize;
            }
            return TRUE;
        }
    }

#ifdef UNDER_XBOX

    wsprintf(szFullName, TEXT("%s_%s"), szModule, szName);
#ifndef UNICODE
    szResource = szFullName;
#else
    wcstombs(szBuffer, szFullName, MAX_PATH);
    szResource = szBuffer;
#endif

    pvData = XLoadSection(szResource);
    if (pvData) {
        if (ppvData) {
            *ppvData = pvData;
        }
        if (pdwSize) {
            HANDLE hSection = XGetSectionHandle(szResource);
            *pdwSize = XGetSectionSize(hSection);
        }
        bRet = TRUE;
    }

#endif // UNDER_XBOX

    return bRet;
}

//******************************************************************************
void UnloadResourceData(LPCTSTR szName) {

#ifdef UNDER_XBOX

#ifdef UNICODE
    char szBuffer[MAX_PATH];
#endif
    TCHAR szFullName[MAX_PATH];
    LPSTR szResource;
    TCHAR szModule[128];

    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        return;
    }

    GetModuleName(GetTestInstance(), szModule, 128);

    wsprintf(szFullName, TEXT("%s_%s"), szModule, szName);
#ifndef UNICODE
    szResource = szFullName;
#else
    wcstombs(szBuffer, szFullName, MAX_PATH);
    szResource = szBuffer;
#endif

    XFreeSection(szResource);

#endif // UNDER_XBOX
}

#ifdef UNDER_XBOX

//******************************************************************************
BOOL LoadResourceFile(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize) {

    PVOID pvFile;
    BOOL bRet = FALSE;
#ifdef UNICODE
    char szBuffer[MAX_PATH];
    LPSTR szResource;
#endif

    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        return FALSE;
    }

#ifndef UNICODE
    szResource = szName;
#else
    wcstombs(szBuffer, szName, MAX_PATH);
    szResource = szBuffer;
#endif

    pvFile = XLoadSection(szResource);
    if (pvFile) {
        if (ppvData) {
            *ppvData = pvFile;
        }
        if (pdwSize) {
            HANDLE hSection = XGetSectionHandle(szResource);
            *pdwSize = XGetSectionSize(hSection);
        }
        bRet = TRUE;
    }

    return bRet;
}

//******************************************************************************
void UnloadResourceFile(LPCTSTR szName) {

#ifdef UNICODE
    char szBuffer[MAX_PATH];
    LPSTR szResource;
#endif

    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        return;
    }

#ifndef UNICODE
    szResource = szName;
#else
    wcstombs(szBuffer, szName, MAX_PATH);
    szResource = szBuffer;
#endif

    XFreeSection(szResource);
}

#endif // UNDER_XBOX
