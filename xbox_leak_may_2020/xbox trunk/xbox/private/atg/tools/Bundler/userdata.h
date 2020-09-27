//-----------------------------------------------------------------------------
// File: UserData.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the userdata-related functionality of the bundler tool
//
// Hist: 02.06.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef USERDATA_H
#define USERDATA_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>


class CBundler;


// Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h

#define USERDATA_ALIGNMENT     4




//-----------------------------------------------------------------------------
// Name: class CUserData
// Desc: Handles all vb-specific processing
//-----------------------------------------------------------------------------
class CUserData
{
    CBundler* m_pBundler;

    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader );
    HRESULT SaveUserDataData( DWORD* pcbData );

public:
    HRESULT SaveToBundle( DWORD* pcbHeader, DWORD* pcbData );

    CHAR  m_strSource[MAX_PATH];
    VOID* m_pData;
    DWORD m_dwDataSize;

    CUserData( CBundler* pBundler );
    ~CUserData();
};


#endif // USERDATA_H
