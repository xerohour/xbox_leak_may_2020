//-----------------------------------------------------------------------------
// File: pushbuffer.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the pushbuffer-related functionality of the bundler library
//
// Hist: 2001.09.26 - New for November XDK release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef PUSHBUFFER_H
#define PUSHBUFFER_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "xd3d.h"

class CBundler;

//-----------------------------------------------------------------------------
// Name: class CPushBuffer
// Desc: Handles all pushbuffer-specific processing
//-----------------------------------------------------------------------------
class CPushBuffer
{
public:
    CPushBuffer( CBundler * pBundler );
    ~CPushBuffer();

    HRESULT SaveToBundle( DWORD *pcbHeader, DWORD *pcbData );
	HRESULT CompileDrawIndexedVertices(
		XD3DPRIMITIVETYPE PrimitiveType,
		UINT VertexCount,
		CONST WORD *pIndexData);	// multiple calls append data to same pushbuffer
	HRESULT Clear();	// clears current pushbuffer data
	
private:
    HRESULT SaveHeader( DWORD dwStart, DWORD * pcbHeader );
    HRESULT SaveData( DWORD * pcbData );
    CBundler * m_pBundler;
	BYTE *m_pBuffer;	// instruction buffer
	DWORD m_BufferSize;	// size of buffer in bytes
};


#endif // PUSHBUFFER_H
