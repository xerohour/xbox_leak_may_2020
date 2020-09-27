//-----------------------------------------------------------------------------
// File: PushBuffer.cpp
//
// Desc: PushBuffer-related functionality of the bundler library
//
// Hist: 2001.09.26 - New for November XDK release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "PushBuffer.h"
#include "Bundler.h"

//-----------------------------------------------------------------------------
// Name: CPushBuffer()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CPushBuffer::CPushBuffer( CBundler* pBundler )
{
    m_pBundler = pBundler;
	m_pBuffer = NULL;	// instruction buffer
	m_BufferSize = 0;	// size of buffer in bytes
}




//-----------------------------------------------------------------------------
// Name: ~CPushBuffer (destructor)
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CPushBuffer::~CPushBuffer()
{
	Clear();
}




//-----------------------------------------------------------------------------
// Name: CompileDrawIndexedVertices
// Desc: Allocates buffer and calls XGCompileDrawIndexedVertices to
// fill with pushbuffer commands.
//-----------------------------------------------------------------------------
HRESULT CPushBuffer::CompileDrawIndexedVertices(
		XD3DPRIMITIVETYPE PrimitiveType,
		UINT VertexCount,
		CONST WORD *pIndexData)
{
	// Get the size of the needed buffer
	DWORD Size = 0;
	HRESULT hr = XGCompileDrawIndexedVertices(NULL, &Size, (D3DPRIMITIVETYPE)PrimitiveType, VertexCount, pIndexData);
	if (FAILED(hr) && hr != XD3DERR_BUFFERTOOSMALL)
		return hr;
	BYTE *pBuffer;
	if (m_pBuffer == NULL)
	{
		// Allocate new buffer
		m_pBuffer = new BYTE [Size];
		if (m_pBuffer == NULL)
			return E_OUTOFMEMORY;
		pBuffer = m_pBuffer;
	}
	else
	{
		// Allocate buffer big enough to hold old and new data
		BYTE *pBufferOld = m_pBuffer;
		m_pBuffer = new BYTE [m_BufferSize + Size];
		if (m_pBuffer == NULL)
		{
			m_pBuffer = pBufferOld;
			return E_OUTOFMEMORY;
		}

		// Copy old data
		memcpy(m_pBuffer, pBufferOld, m_BufferSize);
		pBuffer = m_pBuffer + m_BufferSize;		// set copy point to end of old data
		m_BufferSize += Size;
	}
	// Compile the buffer
	return XGCompileDrawIndexedVertices(pBuffer, &Size, (D3DPRIMITIVETYPE)PrimitiveType, VertexCount, pIndexData);
}


//-----------------------------------------------------------------------------
// Name: Clear
// Desc: Delete the buffers and clears current pushbuffer data
//-----------------------------------------------------------------------------
HRESULT CPushBuffer::Clear()
{
	if (m_pBuffer)
		delete m_pBuffer;
	m_pBuffer = NULL;
	m_BufferSize = 0;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SaveToBundle()
// Desc: Handles saving the appropriate data to the packed resource file
//-----------------------------------------------------------------------------
HRESULT CPushBuffer::SaveToBundle( DWORD * pcbHeader, DWORD * pcbData )
{
    HRESULT hr;

    if (m_pBuffer == NULL || m_BufferSize == 0)
    {
        m_pBundler->ErrorMsg( "Error: No push buffer data specified\n" );
        return E_FAIL;
    }
    
    // Pad data file to proper alignment for the start of the vertex buffer
    hr = m_pBundler->PadToAlignment( XD3DPUSHBUFFER_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Save resource header
    hr = SaveHeader( m_pBundler->m_cbData, pcbHeader );
    if( FAILED( hr ) )
        return hr;

    // Save data
    hr = SaveData( pcbData );
    if( FAILED( hr ) )
        return hr;
     
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveHeader()
// Desc: Saves the appropriate data to the header file
//-----------------------------------------------------------------------------
HRESULT CPushBuffer::SaveHeader( DWORD dwStart, DWORD * pcbHeader )
{
    XD3DPushBuffer PB;

    PB.Common =
		XD3DCOMMON_TYPE_PUSHBUFFER |    // Type
		1;                              // Initial Refcount
    PB.Data   = dwStart; 				// Offset of data in data file
    PB.Lock   = 0;                      // Must be zero
	PB.Size	  = m_BufferSize;
    PB.AllocationSize = m_BufferSize;
	
    // Write the resource header out
    if( FAILED( m_pBundler->WriteHeader( &PB, sizeof( PB ) ) ) )
        return E_FAIL;

    *pcbHeader = sizeof( PB );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveData()
// Desc: Saves the raw PushBuffer data to the XPR file
//-----------------------------------------------------------------------------
HRESULT CPushBuffer::SaveData( DWORD * pcbData )
{
	*pcbData = m_BufferSize;
	return m_pBundler->WriteData( m_pBuffer, m_BufferSize );
}
