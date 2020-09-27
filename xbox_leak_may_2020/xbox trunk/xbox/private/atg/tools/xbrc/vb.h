//-----------------------------------------------------------------------------
// File: vb.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the vertexbuffer-related functionality of the bundler tool
//
// Hist: 2001.02.06 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef VB_H
#define VB_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "xd3d.h"

class CBundler;

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
    HRESULT LoadVertexDataFromFile( CHAR* strFilename );
    static DWORD VertexFormatFromString( CONST CHAR *str );
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
// TODO: Have two different format keys to allow data to be input one way and written another
//    DWORD  m_InputFormat[MAX_VB_FORMATS];
//    DWORD  m_OutputFormat[MAX_VB_FORMATS];
    DWORD  m_DataCount;
    DWORD  m_FormatCount;
    BOOL   m_bRawVertices;  // true if data was loaded from file in packed format
};


#endif // VB_H
