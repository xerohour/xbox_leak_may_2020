/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vibuffer.h

Author:

    Matt Bronder

Description:

    Direct3D vertex and index buffer routines.

*******************************************************************************/

#ifndef __VIBUFFER_H__
#define __VIBUFFER_H__

//******************************************************************************
// Function prototypes
//******************************************************************************

CVertexBuffer8*         CreateVertexBuffer(CDevice8* pDevice, LPVOID pvVertices, 
                                            UINT uSize, 
                                            DWORD dwUsage = 0, DWORD dwFVF = 0, 
                                            D3DPOOL pool = POOL_DEFAULT);
void                    ReleaseVertexBuffer(CVertexBuffer8* pd3dr);
void                    ReleaseVertexBuffers();
void                    RemoveVertexBuffer(CVertexBuffer8* pd3dr);

CIndexBuffer8*          CreateIndexBuffer(CDevice8* pDevice, LPVOID pvIndices, 
                                            UINT uSize, DWORD dwUsage = 0, 
                                            D3DFORMAT fmt = D3DFMT_INDEX16,
                                            D3DPOOL pool = POOL_DEFAULT);
void                    ReleaseIndexBuffer(CIndexBuffer8* pd3di);
void                    ReleaseIndexBuffers();
void                    RemoveIndexBuffer(CIndexBuffer8* pd3di);

#endif //__VIBUFFER_H__
