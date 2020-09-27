//-----------------------------------------------------------------------------
// File: IndexBuffer.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the index buffer-related functionality of the bundler tool
//
// Hist: 11.30.01 - New for December
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>


class CBundler;


// Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h

#define D3DINDEXBUFFER_ALIGNMENT     4

#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
#define D3DCOMMON_VIDEOMEMORY        0x00800000


// IndexBuffer Format
typedef struct 
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
} D3DIndexBuffer;


#define MAX_INDICES 65536




//-----------------------------------------------------------------------------
// Name: class CVertexBuffer
// Desc: Handles all vb-specific processing
//-----------------------------------------------------------------------------
class CIndexBuffer
{
public:
    CIndexBuffer( CBundler * pBundler );
    ~CIndexBuffer();

    HRESULT SaveToBundle( DWORD *pcbHeader, DWORD *pcbData );
    HRESULT AddIndex( WORD wVal );
    HRESULT LoadIndicesFromFile( CHAR* strFilename );

    DWORD  m_dwIndices;

private:
    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD * pcbHeader );
    HRESULT SaveIndexBufferData( DWORD * pcbData );

    CBundler * m_pBundler;
    WORD   m_Indices[MAX_INDICES];
};


#endif // IndexBuffer_H