//-----------------------------------------------------------------------------
// File: IndexBuffer.cpp
//
// Desc: Contains the index buffer-specific logic for the bundler tool
//
// Hist: 11.30.01 - New for December
//
// Copyright (c) Microsoft Corporation. All rights reserved.
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
    m_pBundler  = pBundler;
    m_dwIndices = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CIndexBuffer (destructor)
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CIndexBuffer::~CIndexBuffer()
{
}




//-----------------------------------------------------------------------------
// Name: AddVertexData()
// Desc: Adds a single value to the vertex data stream
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::AddIndex( WORD wVal )
{
    if( m_dwIndices >= MAX_INDICES )
    {
        m_pBundler->ErrorMsg( "Error: Index Data Buffer Overflow\n" );
        return E_FAIL;
    }
    m_Indices[ m_dwIndices++ ] = wVal;

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
    if( dwSize > sizeof( m_Indices ) )
    {
        m_pBundler->ErrorMsg( "Error: Index data file too large.\n" );
        return E_FAIL;
    }

    // Read the data
    DWORD dwRead;
    ReadFile( hFile, m_Indices, dwSize, &dwRead, NULL );
    if( dwRead != dwSize )
        return E_FAIL;

    CloseHandle( hFile );

    m_dwIndices = dwRead / sizeof( WORD );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::SaveToBundle( DWORD * pcbHeader, DWORD * pcbData )
{
    HRESULT hr;

    if(m_dwIndices == 0)
    {
        m_pBundler->ErrorMsg( "Error: No indices specified\n" );
        return E_FAIL;
    }
    
    // Pad data file to proper alignment for the start of the vertex buffer
    hr = m_pBundler->PadToAlignment( D3DINDEXBUFFER_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Save resource header
    hr = SaveHeaderInfo( m_pBundler->m_cbData, pcbHeader );
    if( FAILED( hr ) )
        return hr;

    // Save vertexbuffer data
    hr = SaveIndexBufferData( pcbData );
    if( FAILED( hr ) )
        return hr;
     
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeaderInfo()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::SaveHeaderInfo( DWORD dwStart, DWORD * pcbHeader )
{
    D3DIndexBuffer d3dvb;

    d3dvb.Common =  D3DCOMMON_TYPE_INDEXBUFFER |    // Type
                    1;                              // Initial Refcount

    d3dvb.Data   = dwStart;                    // Offset of data in data file
    d3dvb.Lock   = 0;                          // Must be zero


    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( &d3dvb, sizeof( d3dvb ) ) ) )
        return E_FAIL;

    *pcbHeader = sizeof( d3dvb );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveIndexBufferData()
// Desc: Saves the raw VertexBuffer data to the XPR file
//-----------------------------------------------------------------------------
HRESULT CIndexBuffer::SaveIndexBufferData( DWORD * pcbData )
{
    *pcbData = m_dwIndices * sizeof( WORD );
    m_pBundler->WriteData( m_Indices, *pcbData );
    return S_OK;
}





