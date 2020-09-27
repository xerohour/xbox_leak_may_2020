/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rsrcdata.cpp

Author:

    Matt Bronder

Description:

    Resource loading and unloading.

*******************************************************************************/

#include "d3dbase.h"

//******************************************************************************
BOOL LoadResourceFile(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize) {

#ifndef UNDER_XBOX

    HGLOBAL     hg;
    HRSRC       hrsrc;
    LPBYTE      pData;

    if (hrsrc = FindResource(GetInstance(), szName, RT_RCDATA)) {
        if (ppvData) {
            if (hg = LoadResource(GetInstance(), hrsrc)) {
                *ppvData = LockResource(hg);
            }
            else {
                *ppvData = NULL;
            }
            if (!*ppvData) {
                return FALSE;
            }
        }
        if (pdwSize) {
            *pdwSize = SizeofResource(GetInstance(), hrsrc);
        }

        return TRUE;
    }
    else {
        return FALSE;
    }

#else

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

#endif // UNDER_XBOX
}

//******************************************************************************
void UnloadResourceFile(LPCTSTR szName) {

#ifdef UNDER_XBOX

    PVOID pvFile;
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

#endif // UNDER_XBOX
}
