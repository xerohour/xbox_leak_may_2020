/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vibuffer.cpp

Author:

    Matt Bronder

Description:

    Direct3D vertex and index buffer routines.

*******************************************************************************/

#include "d3dbase.h"

//******************************************************************************
// Structures
//******************************************************************************

typedef struct _VERTEXBUFFERDESC {
    LPDIRECT3DVERTEXBUFFER8     pd3dr;
    UINT                        uSize;
    DWORD                       dwFVF;
    DWORD                       dwUsage;
    D3DPOOL                     pool;
    struct _VERTEXBUFFERDESC*   pvbdNext;
} VERTEXBUFFERDESC, *PVERTEXBUFFERDESC;

typedef struct _INDEXBUFFERDESC {
    LPDIRECT3DINDEXBUFFER8      pd3di;
    UINT                        uSize;
    DWORD                       dwUsage;
    D3DPOOL                     pool;
    D3DFORMAT                   fmt;
    struct _INDEXBUFFERDESC*   pibdNext;
} INDEXBUFFERDESC, *PINDEXBUFFERDESC;

//******************************************************************************
// Globals
//******************************************************************************

PVERTEXBUFFERDESC               g_pvbdList = NULL;
PINDEXBUFFERDESC                g_pibdList = NULL;

//******************************************************************************
// Vertex buffer functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateVertexBuffer
//
// Description:
//
//     Create a vertex buffer object of the given size, format, and usage and
//     populate it with the given vertices.  The size of the vertex list must
//     be at least as large as the buffer being created.
//
// Arguments:
//
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     LPVOID pvVertices                    - Vertex list to store in the buffer
//
//     UINT uSize                           - Size of the vertex buffer
//
//     DWORD dwUsage                        - Resource usage for the vertex
//                                            buffer
//
//     DWORD dwFVF                          - Format of the vertex buffer
//
//     D3DPOOL pool                         - Memory pool in which to create the
//                                            vertex buffer
//
// Return Value:
//
//     A pointer to the created vertex buffer object on success, NULL on 
//     failure.
//
//******************************************************************************
LPDIRECT3DVERTEXBUFFER8 CreateVertexBuffer(LPDIRECT3DDEVICE8 pDevice, 
                                           LPVOID pvVertices, 
                                           UINT uSize, DWORD dwUsage, 
                                           DWORD dwFVF, D3DPOOL pool) 
{
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    PVERTEXBUFFERDESC       pvbdNode;
    LPBYTE                  pData;
    HRESULT                 hr;

    hr = pDevice->CreateVertexBuffer(uSize, dwUsage, dwFVF, pool, &pd3dr);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVertexBuffer"));
        return NULL;
    }

    if (pvVertices) {

        hr = pd3dr->Lock(0, uSize, &pData, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Lock"));
            pd3dr->Release();
            return NULL;
        }

        memcpy(pData, pvVertices, uSize);

        hr = pd3dr->Unlock();
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::Unlock"));
            pd3dr->Release();
            return NULL;
        }
    }

    // Add the buffer to the vertex buffer list
    pvbdNode = (PVERTEXBUFFERDESC)MemAlloc(sizeof(VERTEXBUFFERDESC));
    if (!pvbdNode) {
        pd3dr->Release();
        return NULL;
    }
    memset(pvbdNode, 0, sizeof(VERTEXBUFFERDESC));
    pvbdNode->pd3dr = pd3dr;
    pvbdNode->uSize = uSize;
    pvbdNode->dwFVF = dwFVF;
    pvbdNode->dwUsage = dwUsage;
    pvbdNode->pool = pool;
    pvbdNode->pvbdNext = g_pvbdList;
    g_pvbdList = pvbdNode;

    return pd3dr;
}

//******************************************************************************
//
// Function:
//
//     ReleaseVertexBuffer
//
// Description:
//
//     Release the given vertex buffer object and remove it from the vertex
//     buffer table.
//
// Arguments:
//
//     LPDIRECT3DVERTEXBUFFER8 pd3dr            - Pointer to the buffer object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pd3dr) {

    PVERTEXBUFFERDESC   pvbd, pvbdDel;
#if defined(DEBUG) || defined(_DEBUG)
    UINT                uRef;
#endif

    if (!pd3dr) {
        return;
    }

    if (g_pvbdList) {

        if (g_pvbdList->pd3dr == pd3dr) {
            pvbd = g_pvbdList->pvbdNext;
            MemFree(g_pvbdList);
            g_pvbdList = pvbd;
        }

        else {

            for (pvbd = g_pvbdList; 
                 pvbd->pvbdNext && pvbd->pvbdNext->pd3dr != pd3dr; 
                 pvbd = pvbd->pvbdNext
            );

            if (pvbd->pvbdNext) {
                pvbdDel = pvbd->pvbdNext;
                pvbd->pvbdNext = pvbdDel->pvbdNext;
                MemFree(pvbdDel);
            }
        }
    }

#if defined(DEBUG) || defined(_DEBUG)
    uRef = pd3dr->Release();
    if (uRef != 0) {
        DebugString(TEXT("WARNING: Vertex buffer has a ref count of %d on release"), uRef);
    }
#else
    pd3dr->Release();
#endif
}

//******************************************************************************
//
// Function:
//
//     ReleaseVertexBuffers
//
// Description:
//
//     Release all vertex buffers.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseVertexBuffers() {

    while (g_pvbdList) {
        ReleaseVertexBuffer(g_pvbdList->pd3dr);
    }
}

//******************************************************************************
//
// Function:
//
//     RemoveVertexBuffer
//
// Description:
//
//     Remove the given vertex buffer from the vertex buffer table (if present)
//
// Arguments:
//
//     LPDIRECT3DVERTEXBUFFER8 pd3dr           - Pointer to the buffer object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RemoveVertexBuffer(LPDIRECT3DVERTEXBUFFER8 pd3dr) {

    PVERTEXBUFFERDESC   pvbd, pvbdDel;

    if (!pd3dr) {
        return;
    }

    if (g_pvbdList) {

        if (g_pvbdList->pd3dr == pd3dr) {
            pvbd = g_pvbdList->pvbdNext;
            MemFree(g_pvbdList);
            g_pvbdList = pvbd;
        }

        else {

            for (pvbd = g_pvbdList; 
                 pvbd->pvbdNext && pvbd->pvbdNext->pd3dr != pd3dr; 
                 pvbd = pvbd->pvbdNext
            );

            if (pvbd->pvbdNext) {
                pvbdDel = pvbd->pvbdNext;
                pvbd->pvbdNext = pvbdDel->pvbdNext;
                MemFree(pvbdDel);
            }
        }
    }
}

//******************************************************************************
// Index buffer functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateIndexBuffer
//
// Description:
//
//     Create an index buffer object of the given size, format, and usage and
//     populate it with the given indices.  The size of the index list must
//     be at least as large as the buffer being created.
//
// Arguments:
//
//     LPDIRECT3DDEVICE8 pDevice            - Pointer to the device object
//
//     LPVOID pvIndices                     - Index list to store in the buffer
//
//     UINT uSize                           - Size of the index buffer
//
//     DWORD dwUsage                        - Resource usage for the index
//                                            buffer
//
//     D3DFORMAT fmt                        - Format of the index buffer
//
//     D3DPOOL pool                         - Memory pool in which to create the
//                                            index buffer
//
// Return Value:
//
//     A pointer to the created index buffer object on success, NULL on 
//     failure.
//
//******************************************************************************
LPDIRECT3DINDEXBUFFER8 CreateIndexBuffer(LPDIRECT3DDEVICE8 pDevice, 
                                         LPVOID pvIndices, 
                                         UINT uSize, DWORD dwUsage, 
                                         D3DFORMAT fmt, D3DPOOL pool)
{
    LPDIRECT3DINDEXBUFFER8  pd3di;
    PINDEXBUFFERDESC        pibdNode;
    LPBYTE                  pData;
    HRESULT                 hr;

    hr = pDevice->CreateIndexBuffer(uSize, dwUsage, fmt, pool, &pd3di);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DDevice8::CreateIndexBuffer"));
        return NULL;
    }

    if (pvIndices) {

        hr = pd3di->Lock(0, uSize, &pData, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Lock"));
            pd3di->Release();
            return NULL;
        }

        memcpy(pData, pvIndices, uSize);

        hr = pd3di->Unlock();
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::Unlock"));
            pd3di->Release();
            return NULL;
        }
    }

    // Add the buffer to the index buffer list
    pibdNode = (PINDEXBUFFERDESC)MemAlloc(sizeof(INDEXBUFFERDESC));
    if (!pibdNode) {
        pd3di->Release();
        return NULL;
    }
    memset(pibdNode, 0, sizeof(INDEXBUFFERDESC));
    pibdNode->pd3di = pd3di;
    pibdNode->uSize = uSize;
    pibdNode->dwUsage = dwUsage;
    pibdNode->pool = pool;
    pibdNode->fmt = fmt;
    pibdNode->pibdNext = g_pibdList;
    g_pibdList = pibdNode;

    return pd3di;
}

//******************************************************************************
//
// Function:
//
//     ReleaseIndexBuffer
//
// Description:
//
//     Release the given index buffer object and remove it from the index
//     buffer table.
//
// Arguments:
//
//     LPDIRECT3DINDEXBUFFER8 pd3di          - Pointer to the buffer object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseIndexBuffer(LPDIRECT3DINDEXBUFFER8 pd3di) {

    PINDEXBUFFERDESC    pibd, pibdDel;
#if defined(DEBUG) || defined(_DEBUG)
    UINT                uRef;
#endif

    if (!pd3di) {
        return;
    }

    if (g_pibdList) {

        if (g_pibdList->pd3di == pd3di) {
            pibd = g_pibdList->pibdNext;
            MemFree(g_pibdList);
            g_pibdList = pibd;
        }

        else {

            for (pibd = g_pibdList; 
                 pibd->pibdNext && pibd->pibdNext->pd3di != pd3di; 
                 pibd = pibd->pibdNext
            );

            if (pibd->pibdNext) {
                pibdDel = pibd->pibdNext;
                pibd->pibdNext = pibdDel->pibdNext;
                MemFree(pibdDel);
            }
        }
    }

#if defined(DEBUG) || defined(_DEBUG)
    uRef = pd3di->Release();
    if (uRef != 0) {
        DebugString(TEXT("WARNING: Index buffer has a ref count of %d on release"), uRef);
    }
#else
    pd3di->Release();
#endif
}

//******************************************************************************
//
// Function:
//
//     ReleaseIndexBuffers
//
// Description:
//
//     Release all index buffers.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseIndexBuffers() {

    while (g_pibdList) {
        ReleaseIndexBuffer(g_pibdList->pd3di);
    }
}

//******************************************************************************
//
// Function:
//
//     RemoveVertexBuffer
//
// Description:
//
//     Remove the given vertex buffer from the vertex buffer table (if present)
//
// Arguments:
//
//     LPDIRECT3DINDEXBUFFER8 pd3di         - Pointer to the buffer object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RemoveIndexBuffer(LPDIRECT3DINDEXBUFFER8 pd3di) {

    PINDEXBUFFERDESC    pibd, pibdDel;

    if (!pd3di) {
        return;
    }

    if (g_pibdList) {

        if (g_pibdList->pd3di == pd3di) {
            pibd = g_pibdList->pibdNext;
            MemFree(g_pibdList);
            g_pibdList = pibd;
        }

        else {

            for (pibd = g_pibdList; 
                 pibd->pibdNext && pibd->pibdNext->pd3di != pd3di; 
                 pibd = pibd->pibdNext
            );

            if (pibd->pibdNext) {
                pibdDel = pibd->pibdNext;
                pibd->pibdNext = pibdDel->pibdNext;
                MemFree(pibdDel);
            }
        }
    }
}
