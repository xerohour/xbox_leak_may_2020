/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       surface.cpp
 *  Content:    Implementation of the CSurface class.
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
 * D3DSurface
 */

//------------------------------------------------------------------------------
// Creates a surface that wraps part of a texture.
//
HRESULT CreateSurfaceOfTexture(
    DWORD Format,
    DWORD Size,
    D3DBaseTexture *pParent,
    void *pvData,
    D3DSurface **ppSurface
    )
{
    D3DSurface *pSurface;

    // Allocate the memory for the header.
    pSurface = (D3DSurface *)MemAlloc(sizeof(D3DSurface));

    if (!pSurface)
    {
        return E_OUTOFMEMORY;
    }

    // Fill in its fields.
    pSurface->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_SURFACE | (pParent->Common & D3DCOMMON_VIDEOMEMORY) | D3DCOMMON_D3DCREATED;
    pSurface->Data    = GetGPUAddress(pvData);
    pSurface->Format  = Format;
    pSurface->Size    = Size;
    pSurface->Parent  = pParent;
    pSurface->Lock    = 0;

    pParent->AddRef();

    *ppSurface = pSurface;

    return S_OK;
}

//------------------------------------------------------------------------------
// Initializes a pre-allocated surface.  These will not be a PO2 surface.
//
void InitializeSurface(
    D3DSurface *pSurface,
    DWORD Format,
    DWORD Size,
    void *pvData)
{
    // Just fill it in.
    pSurface->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_SURFACE | D3DCOMMON_VIDEOMEMORY | D3DCOMMON_D3DCREATED;
    pSurface->Data    = GetGPUAddress(pvData);
    pSurface->Format  = Format;
    pSurface->Size    = Size;
    pSurface->Parent  = NULL;
    pSurface->Lock    = 0;
}

//------------------------------------------------------------------------------
// Creates a surface that has no owner.
//
HRESULT CreateStandAloneSurface(
    DWORD Width,
    DWORD Height,     
    D3DFORMAT D3DFormat,
    D3DSurface **ppSurface
    )
{
    D3DSurface *pSurface;

    DWORD Format;
    DWORD Size;
    DWORD MemorySize;

    void *pData;

    MemorySize = PixelJar::EncodeFormat(Width,
                                        Height,
                                        1,
                                        1,
                                        D3DFormat,
                                        0,
                                        false, 
                                        false,
                                        &Format,
                                        &Size);

    // Allocate the memory for the header.
    pSurface = (D3DSurface *)MemAlloc(sizeof(D3DSurface));
    if (!pSurface)
    {
        return E_OUTOFMEMORY;
    }

    // Allocate the data.
    pData = AllocateContiguousMemory(MemorySize, D3DSURFACE_ALIGNMENT);
    if (!pData)
    {
        MemFree(pSurface);
        return E_OUTOFMEMORY;
    }
    
    // Fill in its fields.  We just whack them in instead of going through a 
    // helper.  Probably not a good idea as these fields may change
    // in the future.
    //
    pSurface->Common  = /* initial refcount */ 1 | D3DSURFACE_OWNSMEMORY | D3DCOMMON_TYPE_SURFACE | D3DCOMMON_VIDEOMEMORY | D3DCOMMON_D3DCREATED;
    pSurface->Data    = GetGPUAddress(pData);
    pSurface->Format  = Format;
    pSurface->Size    = Size;
    pSurface->Parent  = NULL;
    pSurface->Lock    = 0;

    *ppSurface = pSurface;

    return S_OK;
}

//------------------------------------------------------------------------------
// Creates a surface in which the header and data are contiguous.
//
HRESULT CreateSurfaceWithContiguousHeader(
    DWORD Width,
    DWORD Height,     
    D3DFORMAT D3DFormat,
    D3DSurface **ppSurface
    )
{
    D3DSurface *pSurface;

    DWORD Format;
    DWORD Size;
    DWORD MemorySize;
    DWORD HeaderSize;

    void *pData;

    MemorySize = PixelJar::EncodeFormat(Width,
                                        Height,
                                        1,
                                        1,
                                        D3DFormat,
                                        0,
                                        false, 
                                        false,
                                        &Format,
                                        &Size);

    HeaderSize = (sizeof(D3DSurface) + D3DSURFACE_ALIGNMENT - 1) & 
            ~(D3DSURFACE_ALIGNMENT - 1);

    pSurface = (D3DSurface *)MmAllocateContiguousMemoryEx(
            HeaderSize + MemorySize,
            0, 
            AGP_APERTURE_BYTES - 1,
            D3DSURFACE_ALIGNMENT, 
            PAGE_READWRITE | PAGE_WRITECOMBINE);

    if (!pSurface)
    {
        return E_OUTOFMEMORY;
    }

    pData = (PVOID)((PBYTE)pSurface + HeaderSize);

    // Fill in its fields.  We just whack them in instead of going through a 
    // helper.  Probably not a good idea as these fields may change
    // in the future.
    //
    pSurface->Common  = /* initial refcount */ 1 | D3DSURFACE_OWNSMEMORY | D3DCOMMON_TYPE_SURFACE | D3DCOMMON_VIDEOMEMORY | D3DCOMMON_D3DCREATED;
    pSurface->Data    = GetGPUAddress(pData);
    pSurface->Format  = Format;
    pSurface->Size    = Size;
    pSurface->Parent  = NULL;
    pSurface->Lock    = 0;

    *ppSurface = pSurface;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DSurface_GetContainer
//
extern "C"
HRESULT WINAPI D3DSurface_GetContainer(
    D3DSurface      *pSurface,
    D3DBaseTexture **ppTexture) 
{ 
    COUNT_API(API_D3DSURFACE_GETCONTAINER);
    CHECK(pSurface, "D3DSurface_GetContainer");

    *ppTexture = pSurface->Parent;

    if (*ppTexture)
    {
        (*ppTexture)->AddRef();
    }

    return *ppTexture ? S_OK : E_FAIL;
}

//------------------------------------------------------------------------------
// D3DSurface_GetDesc
//
extern "C"
void WINAPI D3DSurface_GetDesc(
    D3DSurface *pSurface,
    D3DSURFACE_DESC   *pDesc) 
{ 
    COUNT_API(API_D3DSURFACE_GETDESC);
    CHECK(pSurface, "D3DSurface_GetDesc");

    PixelJar::Get2DSurfaceDesc(pSurface,
                               0,
                               pDesc);
}

//------------------------------------------------------------------------------
// D3DSurface_LockRect
//
extern "C"
void WINAPI D3DSurface_LockRect(
    D3DSurface *pSurface,
    D3DLOCKED_RECT    *pLockedRect,
    CONST RECT        *pRect,
    DWORD Flags) 
{ 
    COUNT_API(API_D3DSURFACE_LOCKRECT);
    CHECK(pSurface, "D3DSurface_LockRect");

    // This code is shared between normal textures and cube maps.
    PixelJar::Lock2DSurface(pSurface,
                            D3DCUBEMAP_FACE_POSITIVE_X, 
                            0, 
                            pLockedRect, 
                            pRect, 
                            Flags);

}

/*
 * D3DVolume
 */

//------------------------------------------------------------------------------
// Creates a volume that wraps part of a texture.
//
HRESULT CreateVolumeOfTexture(
    DWORD Format,
    D3DBaseTexture *pParent,
    void *pvData,
    D3DVolume **ppVolume
    )
{
    D3DVolume *pVolume;

    // Allocate the memory for the header.
    pVolume = (D3DVolume *)MemAlloc(sizeof(D3DVolume));

    if (!pVolume)
    {
        return E_OUTOFMEMORY;
    }

    // Fill in its fields.
    pVolume->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_SURFACE | (pParent->Common & D3DCOMMON_VIDEOMEMORY) | D3DCOMMON_D3DCREATED;
    pVolume->Data   = GetGPUAddress(pvData);
    pVolume->Format = Format;
    pVolume->Size   = 0;
    pVolume->Parent = pParent;
    pVolume->Lock   = 0;

    pParent->AddRef();

    *ppVolume = pVolume;

    return S_OK;
}

//------------------------------------------------------------------------------
// D3DVolume_GetDesc
//
extern "C"
void WINAPI D3DVolume_GetDesc(
    D3DVolume *pVolume, 
    D3DVOLUME_DESC *pDesc
    ) 
{ 
    COUNT_API(API_D3DVOLUME_GETDESC);
    CHECK(pVolume, "D3DVolume_GetDesc");

    PixelJar::Get3DSurfaceDesc(pVolume,
                               0,
                               pDesc);
}

//------------------------------------------------------------------------------
// D3DVolume_LockBox
//
extern "C"
void WINAPI D3DVolume_LockBox(
    D3DVolume *pVolume,
    D3DLOCKED_BOX *pLockedVolume,
    CONST D3DBOX *pBox,
    DWORD Flags
    ) 
{
    COUNT_API(API_D3DVOLUME_LOCKBOX);
    CHECK(pVolume, "D3DVolume_LockBox");

    // This code is shared between normal textures and cube maps.
    PixelJar::Lock3DSurface(pVolume, 
                            0, 
                            pLockedVolume, 
                            pBox, 
                            Flags);

}

//------------------------------------------------------------------------------
// D3DVolume_GetContainer
//
extern "C"
HRESULT WINAPI D3DVolume_GetContainer(
    D3DVolume *pVolume, 
    D3DBaseTexture **ppTexture
    )
{
    COUNT_API(API_D3DVOLUME_GETCONTAINER);
    CHECK(pVolume, "D3DVolume_GetContainer");

    *ppTexture = pVolume->Parent;

    if (*ppTexture)
    {
        (*ppTexture)->AddRef();
    }

    return *ppTexture ? S_OK : E_FAIL;
}

} // end of namespace

