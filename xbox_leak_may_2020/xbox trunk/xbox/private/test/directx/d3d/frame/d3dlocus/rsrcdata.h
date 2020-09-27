/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    rsrcdata.h

Author:

    Matt Bronder

Description:

    Resource data list.

*******************************************************************************/

#ifndef __RSRCDATA_H__
#define __RSRCDATA_H__

//******************************************************************************
typedef struct _RESOURCEDATA {

    TCHAR                   szModule[128];
    TCHAR                   szName[128];
    LPVOID                  pvData;
    DWORD                   dwSize;
    struct _RESOURCEDATA*   prdNext;

    _RESOURCEDATA(LPCTSTR _szModule, LPCTSTR _szName, LPVOID _pvData, DWORD _dwSize);

} RESOURCEDATA, *PRESOURCEDATA;

extern PRESOURCEDATA g_prdResourceList;

BOOL LoadResourceData(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize);
void UnloadResourceData(LPCTSTR szName);

#ifdef UNDER_XBOX

BOOL LoadResourceFile(LPCTSTR szName, LPVOID* ppvData, LPDWORD pdwSize);
void UnloadResourceFile(LPCTSTR szName);

#endif // UNDER_XBOX

#endif // __RSRCDATA_H__
