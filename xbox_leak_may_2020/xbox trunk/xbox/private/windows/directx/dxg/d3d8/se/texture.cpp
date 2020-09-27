/*==========================================================================;
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       texture.cpp
 *  Content:    Implementation of the CBaseTexture class.
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
 * D3DBaseTexture implementation
 */

//----------------------------------------------------------------------------
// Helper to create an instance of any texture.
//
HRESULT CreateTexture
(
    DWORD Width,
    DWORD Height,
    DWORD Depth,
    DWORD Levels,
    DWORD Usage,
    D3DFORMAT D3DFormat,
    bool isCubeMap,
    bool isVolumeTexture,
    D3DBaseTexture **ppTexture
)
{
    D3DBaseTexture *pTexture;
    void *pData;

    // Assume failure.
    *ppTexture = NULL;

    // Get the format, size and memory size of the texture.
    DWORD MemorySize;
    DWORD Format;
    DWORD Size;

    MemorySize = PixelJar::EncodeFormat(Width,
                                        Height,
                                        Depth,
                                        Levels,
                                        D3DFormat,
                                        0,
                                        isCubeMap,
                                        isVolumeTexture,
                                        &Format,
                                        &Size);

    if (Usage & D3DUSAGE_BORDERSOURCE_TEXTURE)
    {
        Format &= ~D3DFORMAT_BORDERSOURCE_COLOR;
    }

    // Allocate the memory for the header.
    pTexture = (D3DBaseTexture *)MemAlloc(sizeof(D3DBaseTexture));

    if (!pTexture)
    {
        return E_OUTOFMEMORY;
    }

    // Allocate the memory for the texture data.
    pData = AllocateContiguousMemory(MemorySize,
                                     D3DTEXTURE_ALIGNMENT);

    if (!pData)
    {
        MemFree(pTexture);
        return E_OUTOFMEMORY;
    }

    // Fill in its fields.  We just whack them in instead of going through a 
    // helper.  Probably not a good idea as these fields may change
    // in the future.
    //
    pTexture->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_TEXTURE | D3DCOMMON_VIDEOMEMORY | D3DCOMMON_D3DCREATED;
    pTexture->Data    = GetGPUAddress(pData);
    pTexture->Format  = Format;
    pTexture->Size    = Size;
    pTexture->Lock    = 0;

    *ppTexture = pTexture;

    return S_OK;
}

//------------------------------------------------------------------------------
// Get the number of mipmap levels for this texture.  This is encode directly
// into the format.
//
extern "C"
DWORD WINAPI D3DBaseTexture_GetLevelCount
(
    D3DBaseTexture *pTexture
)
{
    COUNT_API(API_D3DBASETEXTURE_GETLEVELCOUNT);
    CHECK(pTexture, "D3DBaseTexture_GetLevelCount");

    return PixelJar::GetMipmapLevelCount(pTexture);
}

/*
 * D3DTexture implementation
 */

//------------------------------------------------------------------------------
// Build a descriptor structure that provides information about a specific
// level of this texture.
//
extern "C"
void WINAPI D3DTexture_GetLevelDesc
(
    D3DTexture *pTexture, 
    UINT Level, 
    D3DSURFACE_DESC *pDesc
)
{
    COUNT_API(API_D3DTEXTURE_GETLEVELDESC);
    CHECK(pTexture, "D3DTexture_GetLevelDesc");

    PixelJar::Get2DSurfaceDesc(pTexture,
                               Level, 
                               pDesc);
}

//------------------------------------------------------------------------------
// Create a surface which wraps a mipmap level of this texture.
//
extern "C"
HRESULT WINAPI D3DTexture_GetSurfaceLevel
(
    D3DTexture *pTexture, 
    UINT Level, 
    D3DSurface **ppSurfaceLevel
)
{
    COUNT_API(API_D3DTEXTURE_GETSURFACELEVEL);
    CHECK(pTexture, "D3DTexture_GetSurfaceLevel");

    if (DBG_CHECK(Level >= PixelJar::GetMipmapLevelCount(pTexture)))
    {
        DXGRIP("D3DTexture_GetSurfaceLevel - invalid mipmap level.");
    }

    BYTE *pbData;
    DWORD RowPitch, SlicePitch;
    DWORD Format, Size;

    // Find the memory for our texture.
    PixelJar::FindSurfaceWithinTexture(pTexture,
                                       D3DCUBEMAP_FACE_POSITIVE_X, 
                                       Level, 
                                       &pbData,
                                       &RowPitch,
                                       &SlicePitch,
                                       &Format,
                                       &Size);

    // Allocate the surface.
    return CreateSurfaceOfTexture(Format,
                                  Size,
                                  pTexture,
                                  pbData,
                                  ppSurfaceLevel);
}

//------------------------------------------------------------------------------
// Lock the memory for this texture and allow it to be read/written.
//
extern "C"
void WINAPI D3DTexture_LockRect
(
    D3DTexture *pTexture, 
    UINT Level, 
    D3DLOCKED_RECT *pLockedRect, 
    CONST RECT *pRect, 
    DWORD Flags
)
{
    COUNT_API(API_D3DTEXTURE_LOCKRECT);
    CHECK(pTexture, "D3DTexture_LockRect");

    // This code is shared between normal textures and cube maps.
    PixelJar::Lock2DSurface(pTexture, 
                            D3DCUBEMAP_FACE_POSITIVE_X, 
                            Level, 
                            pLockedRect, 
                            pRect,  
                            Flags);

}

/*
 * D3DCubeTexture implementation
 */

//------------------------------------------------------------------------------
// Build a descriptor structure that provides information about a specific
// level of this texture.
//
extern "C"
void WINAPI D3DCubeTexture_GetLevelDesc
(
    D3DCubeTexture *pTexture, 
    UINT Level, 
    D3DSURFACE_DESC *pDesc
)
{
    COUNT_API(API_D3DCUBETEXTURE_GETLEVELDESC);
    CHECK(pTexture, "D3DCubeTexture_GetLevelDesc");

    PixelJar::Get2DSurfaceDesc(pTexture, 
                               Level, 
                               pDesc);
}

//------------------------------------------------------------------------------
// Create a surface which wraps a mipmap level of a face of this texture.
//
extern "C"
HRESULT WINAPI D3DCubeTexture_GetCubeMapSurface
(
    D3DCubeTexture *pTexture, 
    D3DCUBEMAP_FACES FaceType, 
    UINT Level, 
    D3DSurface **ppCubeMapSurface
)
{
    COUNT_API(API_D3DCUBETEXTURE_GETCUBEMAPSURFACE);
    CHECK(pTexture, "D3DCubeTexture_GetCubeMapSurface");

    if (DBG_CHECK(Level >= PixelJar::GetMipmapLevelCount(pTexture)))
    {
        DXGRIP("D3DTexture_GetSurfaceLevel - invalid mipmap level.");
    }

    BYTE *pbData;
    DWORD RowPitch, SlicePitch;
    DWORD Format, Size;

    // Find the memory for our texture.
    PixelJar::FindSurfaceWithinTexture(pTexture,
                                       FaceType, 
                                       Level, 
                                       &pbData,
                                       &RowPitch,
                                       &SlicePitch,
                                       &Format,
                                       &Size);

    // Allocate the surface.
    return CreateSurfaceOfTexture(Format,
                                  Size,
                                  pTexture,
                                  pbData,
                                  ppCubeMapSurface);
}

//------------------------------------------------------------------------------
// Lock the memory for this texture and allow it to be read/written.
//
extern "C"
void WINAPI D3DCubeTexture_LockRect
(
    D3DCubeTexture *pTexture, 
    D3DCUBEMAP_FACES FaceType, 
    UINT Level, 
    D3DLOCKED_RECT *pLockedRect, 
    CONST RECT *pRect, 
    DWORD Flags
)
{
    COUNT_API(API_D3DCUBETEXTURE_LOCKRECT);
    CHECK(pTexture, "D3DCubeTexture_LockRect");

    // This code is shared between normal textures and cube maps.
    PixelJar::Lock2DSurface(pTexture, 
                            FaceType, 
                            Level, 
                            pLockedRect, 
                            pRect, 
                            Flags);
}

/*
 * D3DVolumeTexture implementation
 */

//------------------------------------------------------------------------------
// Build a descriptor structure that provides information about a specific
// level of this texture.
//
extern "C"
void WINAPI D3DVolumeTexture_GetLevelDesc
(
    D3DVolumeTexture *pTexture, 
    UINT Level, 
    D3DVOLUME_DESC *pDesc
)
{
    COUNT_API(API_D3DVOLUMETEXURE_GETLEVELDESC);
    CHECK(pTexture, "D3DVolumeTexture_GetLevelDesc");

    PixelJar::Get3DSurfaceDesc(pTexture,
                               Level,
                               pDesc);
}

//------------------------------------------------------------------------------
// Create a volume which wraps a mipmap level of this texture.
//
extern "C"
HRESULT WINAPI D3DVolumeTexture_GetVolumeLevel
(
    D3DVolumeTexture *pTexture, 
    UINT Level, 
    D3DVolume **ppVolumeLevel
)
{
    COUNT_API(API_D3DVOLUMETEXURE_GETVOLUMELEVEL);
    CHECK(pTexture, "D3DVolumeTexture_GetVolumeLevel");

    if (DBG_CHECK(Level >= PixelJar::GetMipmapLevelCount(pTexture)))
    {
        DXGRIP("D3DVolumeTexture_GetVolumeLevel - invalid mipmap level.");
    }

    BYTE *pbData;
    DWORD RowPitch, SlicePitch;
    DWORD Format, Size;

    // Find the memory for our texture.
    PixelJar::FindSurfaceWithinTexture(pTexture,
                                       D3DCUBEMAP_FACE_POSITIVE_X, 
                                       Level, 
                                       &pbData,
                                       &RowPitch,
                                       &SlicePitch,
                                       &Format,
                                       &Size);

    // Allocate the surface.
    return CreateVolumeOfTexture(Format,
                                 pTexture,
                                 pbData,
                                 ppVolumeLevel);
}

//------------------------------------------------------------------------------
// Lock a portion of a 3D texture.
//
extern "C"
void WINAPI D3DVolumeTexture_LockBox
(
    D3DVolumeTexture *pTexture, 
    UINT Level, 
    D3DLOCKED_BOX *pLockedVolume, 
    CONST D3DBOX *pBox, 
    DWORD Flags
)
{
    COUNT_API(API_D3DVOLUMETEXURE_LOCKBOX);
    CHECK(pTexture, "D3DVolumeTexture_LockBox");

    PixelJar::Lock3DSurface(pTexture,
                            Level, 
                            pLockedVolume,
                            pBox,
                            Flags);
}

} // end of namespace
