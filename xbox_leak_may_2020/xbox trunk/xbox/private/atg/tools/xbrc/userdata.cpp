//-----------------------------------------------------------------------------
// File: UserData.cpp
//
// Desc: Contains the userdata-specific logic for the bundler tool
//
// Hist: 2001.02.06 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UserData.h"
#include "Bundler.h"
#include <stdio.h>




//-----------------------------------------------------------------------------
// Name: CUserData()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CUserData::CUserData( CBundler* pBundler )
{
    m_pBundler     = pBundler;
    m_strSource[0] = '\0';
    m_pData        = NULL;
    m_dwDataSize   = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CUserData()
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CUserData::~CUserData()
{
    if( m_pData )
        delete[] m_pData;
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CUserData::SaveToBundle( DWORD* pcbHeader, DWORD* pcbData )
{
    HRESULT hr;

    if( 0 == m_strSource[0] )
    {
        m_pBundler->ErrorMsg( "Error: No source specified\n" );
        return E_FAIL;
    }

    // Add the path (if necessary) to the source filename
    CHAR strUserDataFilePath[MAX_PATH];
    if( strchr( m_strSource, ':' ) )
        strcpy( strUserDataFilePath, m_strSource );
    else
    {
        strcpy( strUserDataFilePath, m_pBundler->m_strPath );
        strcat( strUserDataFilePath, m_strSource );
    }

    HANDLE hFile = CreateFileA( strUserDataFilePath, GENERIC_READ, FILE_SHARE_READ,
                                NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
    {
        m_pBundler->ErrorMsg( "Error: Could not find userdata source: %s\n", m_strSource );
        return E_FAIL;
    }

    // Allocate memory
    m_dwDataSize = GetFileSize( hFile, NULL );
    m_pData      = new BYTE[8+m_dwDataSize];

    ((DWORD*)m_pData)[0] = 0x80000000;
    ((DWORD*)m_pData)[1] = m_dwDataSize;

    // Read the file
    DWORD dwRead;
    ReadFile( hFile, (BYTE*)m_pData+8, m_dwDataSize, &dwRead, NULL );
    CloseHandle( hFile );

    // Pad data file to proper alignment for the start of the userdata
    hr = m_pBundler->PadToAlignment( USERDATA_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Save resource header
    hr = SaveHeaderInfo( m_pBundler->m_cbData, pcbHeader );
    if( FAILED( hr ) )
        return hr;

    // Save userdata data
    hr = SaveUserDataData( pcbData );
    if( FAILED( hr ) )
        return hr;
     
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CUserData::SaveHeaderInfo( DWORD dwStart, DWORD * pcbHeader )
{
/*
    DWORD dwUserDataID = 0x8000000;

    // Write the userdata tag to the resource header
    if( FAILED( m_pBundler->WriteHeader( &dwUserDataID, sizeof(DWORD) ) ) )
        return E_FAIL;

    // Write the userdata size to the resource header
    if( FAILED( m_pBundler->WriteHeader( &m_dwDataSize, sizeof(DWORD) ) ) )
        return E_FAIL;

    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( m_pData, m_dwDataSize ) ) )
        return E_FAIL;
*/
    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( m_pData, m_dwDataSize+8 ) ) )
        return E_FAIL;

    (*pcbHeader) = m_dwDataSize+8;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveUserDataData()
// Desc: Saves the raw UserData data to the XPR file
//-----------------------------------------------------------------------------
HRESULT CUserData::SaveUserDataData( DWORD* pcbData )
{
    // There shouldn't be any video memory data to write
    (*pcbData) = 0;

    return S_OK;
}
