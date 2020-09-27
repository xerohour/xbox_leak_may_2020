/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       buffer.cpp
 *  Content:    Implementation of the CBuffer class.
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

/*
 * Vertex buffer
 */

//------------------------------------------------------------------------------
// D3DDevice_CreateVertexBuffer
//
extern "C"
HRESULT WINAPI D3DDevice_CreateVertexBuffer(
    UINT Length,
    DWORD Usage,            // Ignored
    DWORD FVF,              // Ignored
    D3DPOOL Pool,           // Ignored
    D3DVertexBuffer **ppVertexBuffer
    )
{
    COUNT_API(API_D3DDEVICE_CREATEVERTEXBUFFER);

    D3DVertexBuffer *pBuffer;
    void *pData;

    // Allocate the memory for the header.   
    pBuffer = (D3DVertexBuffer *)MemAlloc(sizeof(D3DVertexBuffer));

    if (!pBuffer)
    {
        return E_OUTOFMEMORY;
    }

    // Allocate the memory for the buffer data.
    pData = AllocateContiguousMemory(Length, 0);

    if (!pData)
    {
        MemFree(pBuffer);
        return E_OUTOFMEMORY;
    }

    // Fill in its fields.
    //
    pBuffer->Common = /* initial refcount */ 1 
                      | D3DCOMMON_TYPE_VERTEXBUFFER 
                      | D3DCOMMON_D3DCREATED;

    pBuffer->Data = GetGPUAddress(pData);

    // Return the results.
    *ppVertexBuffer = pBuffer;

    return S_OK;
} 

//----------------------------------------------------------------------------
// Lock a vertex buffer and return a pointer to its data.
//
extern "C"
void WINAPI D3DVertexBuffer_Lock(
    D3DVertexBuffer *pBuffer, 
    UINT OffsetToLock, 
    UINT SizeToLock, 
    BYTE **ppbData, 
    DWORD Flags
    )
{
    COUNT_API(API_D3DVERTEXBUFFER_LOCK);
    CHECK(pBuffer, "D3DVertexBuffer_Lock");

    // Validate the parameters.
    if (DBG_CHECK(TRUE))
    {
        if (OffsetToLock != 0 && SizeToLock == 0)
        {
            DXGRIP("LockBuffer - Unable to lock zero bytes.");
        }
    }

    // Flush the vertex cache unless we're asked not to.
    if (!(Flags & D3DLOCK_NOFLUSH))
    {
        CDevice *pDevice = g_pDevice;

        // Invalidate the vertex cache.
        PPUSH pPush = pDevice->StartPush();

        Push1(pPush, NV097_INVALIDATE_VERTEX_CACHE_FILE, 0);

        pDevice->EndPush(pPush + 2);
    }

    // Make sure that we don't have multiple people partying on the buffer
    // at the same time.  Since the GPU will only ever read from this buffer
    // we only have to wait if we want to write to a part that is already
    // in use.
    //
    if (!(Flags & (D3DLOCK_NOOVERWRITE | D3DLOCK_READONLY)))
    {
        // Wait for the resource to be available.
        BlockOnNonSurfaceResource(pBuffer);
    }

    // Get the data.
    *ppbData = (BYTE *)GetDataFromResource(pBuffer) + OffsetToLock;
}

//----------------------------------------------------------------------------
// Get the description for the vertex buffer.
//
extern "C"
void WINAPI D3DVertexBuffer_GetDesc(
    D3DVertexBuffer *pBuffer, 
    D3DVERTEXBUFFER_DESC *pDesc
    )
{
    COUNT_API(API_D3DVERTEXBUFFER_GETDESC);
    CHECK(pBuffer, "D3DVertexBuffer_GetDesc");

    pDesc->Format = D3DFMT_VERTEXDATA;
    pDesc->Type   = pBuffer->GetType();
}

/*
 * Index buffer
 */

//------------------------------------------------------------------------------
// D3DDevice_CreateIndexBuffer
//
extern "C"
HRESULT WINAPI D3DDevice_CreateIndexBuffer(
    UINT Length,
    DWORD Usage,            // Ignored
    D3DFORMAT Format,
    D3DPOOL Pool,           // Ignored
    D3DIndexBuffer** ppIndexBuffer) 
{ 
    COUNT_API(API_D3DDEVICE_CREATEINDEXBUFFER);

    D3DIndexBuffer *pBuffer;
    void *pData;

    // Allocate the memory for the header and the buffer.
    //
    pBuffer = (D3DIndexBuffer *)MemAllocNoZero(sizeof(D3DIndexBuffer) + Length);

    if (!pBuffer)
    {
        return E_OUTOFMEMORY;
    }

    memset(pBuffer, 0, sizeof(D3DIndexBuffer));

    // Fill in its fields.
    //
    pBuffer->Common = /* initial refcount */ 1 
                      | D3DCOMMON_TYPE_INDEXBUFFER 
                      | D3DCOMMON_D3DCREATED;

    pBuffer->Data = (DWORD)(pBuffer + 1);

    // Return the results.
    *ppIndexBuffer = pBuffer;

    return S_OK;
}

//----------------------------------------------------------------------------
// Get the description for the index buffer.
//
extern "C"
void WINAPI D3DIndexBuffer_GetDesc(
    D3DIndexBuffer *pBuffer, 
    D3DINDEXBUFFER_DESC *pDesc
    )
{
    COUNT_API(API_D3DINDEXBUFFER_GETDESC);
    CHECK(pBuffer, "D3DIndexBuffer_GetDesc");

    pDesc->Format = D3DFMT_INDEX16;
    pDesc->Type   = D3DRTYPE_INDEXBUFFER;
}

/*
 * Palette
 */

//------------------------------------------------------------------------------
// Creates a palette resource, the 'Size' is the number of entries to put in the 
// palette and must be one of 32, 64, 128 or 256.
//
D3DCONST g_PaletteSize[] = { 256, 128, 64, 32 };

extern "C"
HRESULT WINAPI D3DDevice_CreatePalette(
    D3DPALETTESIZE Size, 
    D3DPalette **ppPalette
    )
{
    D3DPalette *pPalette;
    void *pData;

    COUNT_API(API_D3DDEVICE_CREATEPALETTE);

    if (DBG_CHECK(Size >= D3DPALETTE_MAX))
    {
        DPF_ERR("Size must be D3DPALETTESIZE_32, _64, _128, or _256");
    }

    // Allocate the memory for the header.
    pPalette = (D3DPalette *)MemAlloc(sizeof(D3DPalette));

    if (!pPalette)
    {
        return E_OUTOFMEMORY;
    }

    // Allocate the memory for the buffer data.
    pData = AllocateContiguousMemory(g_PaletteSize[Size], 0);

    if (!pData)
    {
        MemFree(pPalette);
        return E_OUTOFMEMORY;
    }

    // Fill in its fields. 

    pPalette->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_PALETTE 
                                                 | D3DCOMMON_VIDEOMEMORY
                                                 | D3DCOMMON_D3DCREATED
                                                 | D3DPALETTE_COMMON_VIDEOMEMORY;

    pPalette->Common |= Size << D3DPALETTE_COMMON_PALETTESIZE_SHIFT;

    pPalette->Data = GetGPUAddress(pData);

    // Return the results.

    *ppPalette = pPalette;

    return S_OK;
}

//----------------------------------------------------------------------------
// Lock a palette and return a pointer to its data.
//
extern "C"
void WINAPI D3DPalette_Lock(
    D3DPalette *pPalette, 
    D3DCOLOR **ppColors, 
    DWORD Flags
    )
{
    COUNT_API(API_D3DPALETTE_LOCK);
    CHECK(pPalette, "D3DPalette_Lock");

    // Make sure that we don't have multiple people partying on the buffer
    // at the same time.  Since the GPU will only ever read from this buffer
    // we only have to wait if we want to write to it.
    //
    if (!(Flags & (D3DLOCK_NOOVERWRITE | D3DLOCK_READONLY)))
    {
        BlockOnNonSurfaceResource(pPalette);
    }

    // Get the data.
    *ppColors = (D3DCOLOR *)GetDataFromResource(pPalette);
}

//----------------------------------------------------------------------------
// Return the size of the palette.
//
extern "C"
D3DPALETTESIZE WINAPI D3DPalette_GetSize(
    D3DPalette *pPalette
    )
{
    COUNT_API(API_D3DPALETTE_GETSIZE);
    CHECK(pPalette, "D3DPalette_GetSize");

    return (D3DPALETTESIZE) ((pPalette->Common & D3DPALETTE_COMMON_PALETTESIZE_MASK) 
                                    >> D3DPALETTE_COMMON_PALETTESIZE_SHIFT);
}

} // end of namespace
