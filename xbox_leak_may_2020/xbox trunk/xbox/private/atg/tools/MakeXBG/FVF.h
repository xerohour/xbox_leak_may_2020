//-----------------------------------------------------------------------------
// File: FVF.h
//
// Desc: Code for dealing with flexible vertex formats.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef FVF_H
#define FVF_H




//-----------------------------------------------------------------------------
// Name: struct VERTEXFORMAT
// Desc: Holds informations about vertex components
//-----------------------------------------------------------------------------
struct VERTEXFORMAT
{
    BOOL  bPosition;
    DWORD dwNumBlendWeights;
    BOOL  bNormal;
    BOOL  bDiffuse;
    BOOL  bSpecular;
    DWORD dwNumTexCoords[4];

    DWORD dwPositionOffset;
    DWORD dwBlendWeightsOffset;
    DWORD dwNormalOffset;
    DWORD dwDiffuseOffset;
    DWORD dwSpecularOffset;
    DWORD dwTexCoordsOffset[4];

    DWORD dwVertexSize;
};




//-----------------------------------------------------------------------------
// Name: CrackFVF()
// Desc: Dissects an FVF code into vertex component descriptions stored in
//       the VERTEXFORMAT structure.
//-----------------------------------------------------------------------------
VOID  CrackFVF( DWORD dwFVF, VERTEXFORMAT& fvf );




//-----------------------------------------------------------------------------
// Name: EncodeFVF()
// Desc: Builds an FVF code from the vertex component descriptions stores in
//       the VERTEXFORMAT structure.
//-----------------------------------------------------------------------------
DWORD EncodeFVF( VERTEXFORMAT& fvf );




//-----------------------------------------------------------------------------
// Name: CopyVertexBufferWithNewFVF()
// Desc: Makes a copy of a vertex buffer, using a new FVF code.
//-----------------------------------------------------------------------------
HRESULT CopyVertexBufferWithNewFVF( LPDIRECT3DVERTEXBUFFER8 pOldVB, DWORD dwNumVertices,
                                    DWORD dwOldFVF, DWORD dwNewFVF,
                                    LPDIRECT3DVERTEXBUFFER8* ppNewVB );




#endif
