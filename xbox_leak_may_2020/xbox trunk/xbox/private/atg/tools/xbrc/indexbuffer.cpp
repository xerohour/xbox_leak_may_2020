//-----------------------------------------------------------------------------
// File: IndexBuffer.cpp
//
// Desc: Contains the IndexBuffer-specific logic for the bundler tool
//
// Hist: 2001.11.30 - New for December XDK Release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "IndexBuffer.h"
#include "Bundler.h"
#include <stdio.h>




//-----------------------------------------------------------------------------
// Name: CIndexBuffer()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CIndexBuffer::CIndexBuffer( CBundler* pBundler )
{
    m_pBundler = pBundler;
	m_IndexCount = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CIndexBuffer (destructor)
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CIndexBuffer::~CIndexBuffer()
{
}




//-----------------------------------------------------------------------------
// Name: AddIndex()
// Desc: Adds a single index to the index data
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::AddIndex( WORD Index )
{
    if (m_IndexCount >= MAX_INDEXBUFFER_COUNT)
    {
        m_pBundler->ErrorMsg( "Error: Index Buffer Overflow\n" );
        return E_FAIL;
    }
    m_rIndex[m_IndexCount++] = Index;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadIndicesFromFile
// Desc: Loads entire vertex data stream from file
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::LoadIndicesFromFile( CHAR* strFilename )
{
    // Open the file
    HANDLE hFile = CreateFileA( strFilename, 
                                GENERIC_READ, 
                                FILE_SHARE_READ, 
                                NULL,
                                OPEN_EXISTING,
                                FILE_FLAG_SEQUENTIAL_SCAN,
                                NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return E_FAIL;

    // Make sure we can fit the whole thing in memory.
    // TODO: Dynamically allocate memory for index buffers
    DWORD dwSize = GetFileSize( hFile, NULL );
    if( dwSize > sizeof( m_rIndex ) )
    {
        m_pBundler->ErrorMsg( "Error: Index data file too large.\n" );
        return E_FAIL;
    }

    // Read the data
    DWORD dwRead;
    ReadFile( hFile, m_rIndex, dwSize, &dwRead, NULL );
    if( dwRead != dwSize )
        return E_FAIL;

    CloseHandle( hFile );

    m_IndexCount = dwRead / sizeof( WORD );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::SaveToBundle( DWORD * pcbHeader, DWORD * pcbData )
{
    if (m_IndexCount == 0)
    {
        m_pBundler->ErrorMsg( "Error: No indices specified\n" );
        return E_FAIL;
    }

	// Compute size of header + XD3DIndexBuffer + alignment fill + indices
	struct UserData {
		DWORD dwType;				// USER_DATA_FLAG | XD3DCOMMON_TYPE_INDEXBUFFER
		DWORD dwSize;				// size of indexbuffer + alignment fill + indices
	} IBUserData;
	DWORD cbHeaderNew = m_pBundler->m_cbHeader + sizeof(UserData) + sizeof(XD3DIndexBuffer);
	DWORD dwAlign = XD3DINDEXBUFFER_ALIGNMENT; 
	DWORD cbFill = 0;
	if (cbHeaderNew % dwAlign)
		cbFill = dwAlign - ( cbHeaderNew % dwAlign );
	
	IBUserData.dwType = USER_DATA_FLAG | XD3DCOMMON_TYPE_INDEXBUFFER;
	IBUserData.dwSize = sizeof(XD3DIndexBuffer) + cbFill + sizeof(WORD) * m_IndexCount;	// size not including the UserData

	// Write header
	if (FAILED(m_pBundler->WriteHeader( &IBUserData, sizeof(UserData))))
		return E_FAIL;
	m_pBundler->m_cbHeader += sizeof(UserData);	// TODO: this should be done in WriteHeader

	// Write XD3DIndexBuffer
    XD3DIndexBuffer xd3dIndexBuffer;
    xd3dIndexBuffer.Common =  XD3DCOMMON_TYPE_INDEXBUFFER |  // Type   
                    1;                               // Initial Refcount
	// Offset from start of XD3DIndexBuffer structure to index data. Needs to be patched when loaded.
    xd3dIndexBuffer.Data   = sizeof(XD3DIndexBuffer) + cbFill;
    xd3dIndexBuffer.Lock   = 0;                          // Must be zero
	
    // Write the resource header
    if( FAILED( m_pBundler->WriteHeader( &xd3dIndexBuffer, sizeof( xd3dIndexBuffer ) ) ) )
        return E_FAIL;
	m_pBundler->m_cbHeader += sizeof(xd3dIndexBuffer);	// TODO: this should be done in WriteHeader

	// Write alignment filler
	if (cbFill)
	{
		BYTE Fill = 0xAA;
		for (DWORD iFill = 0; iFill < cbFill; iFill++)
		{
			if (FAILED(m_pBundler->WriteHeader( &Fill, 1 )))
				return E_FAIL;
			m_pBundler->m_cbHeader += 1;	// TODO: this should be done in WriteHeader
		}
	}

	// Write indices
	if (FAILED( m_pBundler->WriteHeader( m_rIndex, sizeof(WORD) * m_IndexCount ) ) )
		return E_FAIL;
	m_pBundler->m_cbHeader += sizeof(WORD) * m_IndexCount;	// TODO: this should be done in WriteHeader

	// Set return counts
    *pcbHeader = sizeof(UserData) + IBUserData.dwSize;
	*pcbData = 0;	// no video data
    return S_OK;
}

