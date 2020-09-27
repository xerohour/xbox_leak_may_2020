//-----------------------------------------------------------------------------
// File: vb.cpp
//
// Desc: Header file containing class, struct, and constant definitions
//       for the vertexbuffer-related functionality of the bundler tool
//
// Hist: 02.06.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef VB_H
#define VB_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>


class CBundler;


// Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h

#define D3DVERTEXBUFFER_ALIGNMENT     4

#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
#define D3DCOMMON_VIDEOMEMORY        0x00800000

// Vertex attribute formats
#define XD3DVSDT_FLOAT1      0x00    // 1D float expanded to (value, 0., 0., 1.)
#define XD3DVSDT_FLOAT2      0x01    // 2D float expanded to (value, value, 0., 1.)
#define XD3DVSDT_FLOAT3      0x02    // 3D float expanded to (value, value, value, 1.)
#define XD3DVSDT_FLOAT4      0x03    // 4D float
#define XD3DVSDT_D3DCOLOR    0x04    // 4D packed unsigned bytes mapped to 0. to 1. range
#define XD3DVSDT_SHORT2      0x06    // 2D signed short expanded to (value, value, 0., 1.)
#define XD3DVSDT_SHORT4      0x07    // 4D signed short
#define XD3DVSDT_NORMSHORT1  0x08    // 1D signed, normalized short expanded to (value, 0, 0., 1.)
#define XD3DVSDT_NORMSHORT2  0x09    // 2D signed, normalized short expanded to (value, value, 0., 1.)
#define XD3DVSDT_NORMSHORT3  0x0A    // 3D signed, normalized short expanded to (value, value, value, 1.)  
#define XD3DVSDT_NORMSHORT4  0x0B    // 4D signed, normalized short expanded to (value, value, value, value)  
#define XD3DVSDT_NORMPACKED3 0x0C    // 3 signed, normalized components packed in 32-bits.  (11,11,10).  
#define XD3DVSDT_SHORT1      0x0D    // 1D signed short expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_SHORT3      0x0E    // 3D signed short expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE1      0x0F    // 1D packed byte expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_PBYTE2      0x10    // 2D packed byte expanded to (value, value, 0., 1.)
#define XD3DVSDT_PBYTE3      0x11    // 3D packed byte expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE4      0x12    // 4D packed byte expanded to (value, value, value, value) 
#define XD3DVSDT_FLOAT2H     0x13    // 2D homogeneous float expanded to (value, value,0., value.)


// VertexBuffer Format
typedef struct 
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
} D3DVertexBuffer;


#define MAX_VB_DATA 655360
#define MAX_VB_FORMATS 32




//-----------------------------------------------------------------------------
// Name: class CVertexBuffer
// Desc: Handles all vb-specific processing
//-----------------------------------------------------------------------------
class CVertexBuffer
{
public:
    CVertexBuffer( CBundler * pBundler );
    ~CVertexBuffer();

    HRESULT SaveToBundle( DWORD *pcbHeader, DWORD *pcbData );
    HRESULT AddVertexData( DOUBLE val);
    HRESULT AddVertexFormat( DWORD format);
    DWORD   m_cBytesPerVertex;
    DWORD   m_cInputValuesPerVertex;
    DWORD   m_cVertices;
    DWORD   m_cTotalBytesinVB;

private:
    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD * pcbHeader );
    HRESULT SaveVertexBufferData( DWORD * pcbData );

    CBundler * m_pBundler;
    DOUBLE m_Data[MAX_VB_DATA];
    DWORD  m_Format[MAX_VB_FORMATS];
    DWORD  m_DataCount;
    DWORD  m_FormatCount;
};


#endif // VB_H