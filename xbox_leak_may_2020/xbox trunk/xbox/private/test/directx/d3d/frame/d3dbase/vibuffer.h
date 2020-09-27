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

LPDIRECT3DVERTEXBUFFER8 CreateVertexBuffer(LPDIRECT3DDEVICE8 pDevice, 
                                            LPVOID pvVertices, UINT uSize, 
                                            DWORD dwUsage = 0, DWORD dwFVF = 0, 
                                            D3DPOOL pool = POOL_DEFAULT);
void                    ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pd3dr);
void                    ReleaseVertexBuffers();
void                    RemoveVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pd3dr);

LPDIRECT3DINDEXBUFFER8  CreateIndexBuffer(LPDIRECT3DDEVICE8 pDevice, 
                                            LPVOID pvIndices, 
                                            UINT uSize, DWORD dwUsage = 0, 
                                            D3DFORMAT fmt = D3DFMT_INDEX16,
                                            D3DPOOL pool = POOL_DEFAULT);
void                    ReleaseIndexBuffer(LPDIRECT3DINDEXBUFFER8 pd3di);
void                    ReleaseIndexBuffers();
void                    RemoveIndexBuffer(LPDIRECT3DINDEXBUFFER8 pd3di);

#endif //__VIBUFFER_H__
