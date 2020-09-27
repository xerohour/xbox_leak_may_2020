//-----------------------------------------------------------------------------
// File: IndexBuffer.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the indexbuffer-related functionality of the bundler tool
//
// Hist: 2001.11.30 - New for December XDK release
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "xd3d.h"

class CBundler;

#define MAX_INDEXBUFFER_COUNT 65536

//-----------------------------------------------------------------------------
// Name: class CIndexBuffer
// Desc: Handles IndexBuffer-specific processing
//-----------------------------------------------------------------------------
class CIndexBuffer
{
public:
    CIndexBuffer( CBundler * pBundler );
    ~CIndexBuffer();

    HRESULT SaveToBundle( DWORD *pcbHeader, DWORD *pcbData ); // indices are saved after the header
    HRESULT AddIndex( WORD Index );
    HRESULT LoadIndicesFromFile( CHAR* strFilename );
	
    DWORD m_IndexCount;
	WORD m_rIndex[MAX_INDEXBUFFER_COUNT];

private:
    CBundler * m_pBundler;
};


#endif // INDEXBUFFER_H
