/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       surface.hpp
 *  Content:    Class header the stand-alone surface class. This class
 *              is intended to be returned by the CreateRenderTarget
 *              creation method. It is also used by the CreateZStencil
 *              device method.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//------------------------------------------------------------------------------
// Creates a surface that wraps part of a texture.
//
HRESULT CreateSurfaceOfTexture(
    DWORD Format,
    DWORD Size,
    D3DBaseTexture *pParent,
    void *pvData,
    D3DSurface **ppSurface
    );

//------------------------------------------------------------------------------
// Initializes a pre-allocated surface.  This will never be a PO2 surface
// and the pitch will always match the width.  This is only used for
// frame- and back-buffers.
//
void InitializeSurface(
    D3DSurface *pSurface,
    DWORD Format,
    DWORD Size,
    void *pvData);

//------------------------------------------------------------------------------
// Creates a surface that has no owner.
//
HRESULT CreateStandAloneSurface(
    DWORD Width,
    DWORD Height, 
    D3DFORMAT D3DFormat,
    D3DSurface **ppSurface
    );

//------------------------------------------------------------------------------
// Creates a volume that wraps part of a texture.
//
HRESULT CreateVolumeOfTexture(
    DWORD Format,
    D3DBaseTexture *pParent,
    void *pvData,
    D3DVolume **ppVolume
    );

//------------------------------------------------------------------------------
// Creates a surface in which the header and data are contiguous.
//
HRESULT CreateSurfaceWithContiguousHeader(
    DWORD Width,
    DWORD Height,     
    D3DFORMAT D3DFormat,
    D3DSurface **ppSurface
    );

} // end namespace
