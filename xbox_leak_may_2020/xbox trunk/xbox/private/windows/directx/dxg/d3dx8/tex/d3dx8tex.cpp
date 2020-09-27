///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8tex.cpp
//  Content:    D3DX texturing APIs
//
///////////////////////////////////////////////////////////////////////////

#include "pchtex.h"

#ifdef DBG

// This is a hack so that users get a reasonable error message when they
// link with D3D8X and don't link with XGraphics. It could be defined
// in any source file that's included in the XGraphics library.

#define XGRAPHICS_SYMBOL you_must_also_link_with_xgraphicsd_lib
extern "C" {
    extern DWORD XGRAPHICS_SYMBOL;
};

#define DBG_LINKWITHXGRAPHICS ((void) (XGRAPHICS_SYMBOL = 0))

#else

#define DBG_LINKWITHXGRAPHICS ((void) 0)

#endif

BOOL D3DX__Premult = TRUE;

void WINAPI D3DXSetDXT3DXT5(BOOL Set)
{
    if (Set)
    {
        D3DX__Premult = FALSE;
    }
    else
    {
        D3DX__Premult = TRUE;
    }
}

BOOL WINAPI D3DXGetDXT3DXT5(void)
{
    return (D3DX__Premult == FALSE);
}

BOOL IsUvl(D3DFORMAT format)
{
    switch (format) {
    case D3DFMT_LIN_V16U16: 
    case D3DFMT_LIN_V8U8: 
    case D3DFMT_LIN_L6V5U5: 
    case D3DFMT_LIN_X8L8V8U8: 
    case D3DFMT_LIN_Q8W8V8U8: 
    case D3DFMT_V16U16: 
    case D3DFMT_V8U8: 
    case D3DFMT_L6V5U5: 
    case D3DFMT_X8L8V8U8: 
    case D3DFMT_Q8W8V8U8: 
        return TRUE;
    default: 
        return FALSE;
    }
}

static HRESULT
    CheckTextureRequirements(
        LPDIRECT3DDEVICE8     pDevice,
        UINT*                 pWidth,
        UINT*                 pHeight,
        UINT*                 pDepth,
        UINT*                 pMipLevels,
        DWORD                 Usage,
        D3DFORMAT*            pFormat,
        D3DPOOL               Pool,
        UINT                  uType)
{
    HRESULT hr;

    // Validate arguments
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(D3DX_DEFAULT == Usage)
        Usage = 0;

    if(Usage != (Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)))
    {
        DPF(0, "Usage parameter is invalid");
        return D3DERR_INVALIDCALL;
    }
    if((2 == uType) && (Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)))
    {
        DPF(0, "Usage parameter is invalid");
        return D3DERR_INVALIDCALL;
    }
    if((D3DPOOL_DEFAULT != Pool) && (D3DPOOL_MANAGED != Pool) && (D3DPOOL_SYSTEMMEM != Pool))
    {
        DPF(0, "Pool parameter is invalid");
        return D3DERR_INVALIDCALL;
    }

    // Get caps
    LPDIRECT3D8 pD3D;
    D3DCAPS8 Caps;
    D3DDISPLAYMODE Mode;

    pDevice->GetDirect3D(&pD3D);
    pDevice->GetDeviceCaps(&Caps);
    pDevice->GetDisplayMode(&Mode);


    // cpWidth and cpHeight
    UINT cpWidth  = pWidth  ? *pWidth  : D3DX_DEFAULT;
    UINT cpHeight = pHeight ? *pHeight : D3DX_DEFAULT;
    UINT cpDepth  = pDepth  ? *pDepth  : D3DX_DEFAULT;

    if(D3DX_DEFAULT == cpWidth && D3DX_DEFAULT == cpHeight)
        cpWidth = cpHeight = 256;
    else if(D3DX_DEFAULT == cpWidth)
        cpWidth = cpHeight;
    else if(D3DX_DEFAULT == cpHeight)
        cpHeight = cpWidth;

    if(0 == cpWidth)
        cpWidth = 1;
    if(0 == cpHeight)
        cpHeight = 1;

    if(uType == 1)
        cpWidth = cpHeight = ((cpWidth > cpHeight) ? cpWidth : cpHeight);

    if(D3DX_DEFAULT == cpDepth || 0 == cpDepth)
        cpDepth = 1;

    if(uType == 2)
    {
        if(cpDepth > Caps.MaxVolumeExtent)
            cpDepth = Caps.MaxVolumeExtent;

        if(cpWidth > Caps.MaxVolumeExtent)
            cpWidth = Caps.MaxVolumeExtent;

        if(cpHeight > Caps.MaxVolumeExtent)
            cpHeight = Caps.MaxVolumeExtent;
    }
    else
    {
        if(cpWidth > Caps.MaxTextureWidth)
            cpWidth = Caps.MaxTextureWidth;

        if(cpHeight > Caps.MaxTextureHeight)
            cpHeight = Caps.MaxTextureHeight;

    }


    if(uType == 0)
    {
        if(Caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
            Caps.MaxTextureAspectRatio = 1;

        if(Caps.MaxTextureAspectRatio)
        {
            if(cpWidth > cpHeight * Caps.MaxTextureAspectRatio)
                cpWidth = cpHeight * Caps.MaxTextureAspectRatio;

            if(cpHeight > cpWidth * Caps.MaxTextureAspectRatio)
                cpHeight = cpWidth * Caps.MaxTextureAspectRatio;
        }
    }

    CONST DWORD dwPow2[] =
    {
        D3DPTEXTURECAPS_POW2,
        D3DPTEXTURECAPS_CUBEMAP_POW2,
        D3DPTEXTURECAPS_VOLUMEMAP_POW2
    };

    if(Caps.TextureCaps & dwPow2[uType])
    {
        DWORD dw;

        for(dw = cpWidth,  cpWidth  = 1; cpWidth  < dw; cpWidth  <<= 1);
        for(dw = cpHeight, cpHeight = 1; cpHeight < dw; cpHeight <<= 1);
        for(dw = cpDepth,  cpDepth  = 1; cpDepth  < dw; cpDepth  <<= 1);
    }

    if(pFormat)
    {
        switch(*pFormat)
        {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
#if 0
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
#endif
            cpWidth  = (cpWidth  + 3) & ~3;
            cpHeight = (cpHeight + 3) & ~3;
            break;
        }
    }



    // MipLevels
    CONST DWORD dwMipmap[] =
    {
        D3DPTEXTURECAPS_MIPMAP,
        D3DPTEXTURECAPS_MIPCUBEMAP,
        D3DPTEXTURECAPS_MIPVOLUMEMAP
    };

    UINT cMipLevels = pMipLevels ? *pMipLevels : D3DX_DEFAULT;

    if((Caps.TextureCaps & dwMipmap[uType]) && !(cpWidth & (cpWidth - 1)) &&
        !(cpHeight & (cpHeight - 1)) && !(cpDepth & (cpDepth - 1)))
    {
        UINT dw, dwW, dwH, dwD;

        for(dw = cpWidth,  dwW = 1; dw && !(dw & 1); dw >>= 1, dwW++);
        for(dw = cpHeight, dwH = 1; dw && !(dw & 1); dw >>= 1, dwH++);
        for(dw = cpDepth,  dwD = 1; dw && !(dw & 1); dw >>= 1, dwD++);

        dw = (dwW > dwH) ? dwW : dwH;

        if(uType == 2)
            dw = (dwD > dw) ? dwD : dw;

        if(cMipLevels > dw || cMipLevels == 0)
            cMipLevels = dw;
    }
    else
    {
        cMipLevels = 1;
    }

    pD3D->Release();

    if(pWidth)
        *pWidth = cpWidth;
    if(pHeight)
        *pHeight = cpHeight;
    if(pDepth)
        *pDepth = cpDepth;
    if(pMipLevels)
        *pMipLevels = cMipLevels;

    return S_OK;
}







///////////////////////////////////////////////////////////////////////////
// Surface APIs ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

HRESULT WINAPI
    D3DXLoadSurfaceFromFileA(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        LPCSTR                pSrcFile,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, FALSE)))
        return hr;

    return D3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette,
        pDestRect, fm.m_pvData, fm.m_cbData, pSrcRect, dwFilter, ColorKey, 
        pSrcInfo);
}

#if 0
HRESULT WINAPI
    D3DXLoadSurfaceFromFileW(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        LPCWSTR               pSrcFile,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, TRUE)))
        return hr;

    return D3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette,
        pDestRect, fm.m_pvData, fm.m_cbData, pSrcRect, dwFilter, ColorKey,
        pSrcInfo);
}
#endif
#if 0
HRESULT WINAPI
    D3DXLoadSurfaceFromResourceA(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        HMODULE               hSrcModule,
        LPCSTR                pSrcResource,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo)
{
    HRESULT hr;
    CD3DXResource res;

    if(FAILED(hr = res.Open(hSrcModule, pSrcResource, FALSE)))
        return hr;

    return D3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette,
        pDestRect, res.m_pvData, res.m_cbData, pSrcRect, dwFilter, ColorKey,
        pSrcInfo);
}


HRESULT WINAPI
    D3DXLoadSurfaceFromResourceW(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        HMODULE               hSrcModule,
        LPCWSTR               pSrcResource,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo)
{
    HRESULT hr;
    CD3DXResource res;

    if(FAILED(hr = res.Open(hSrcModule, pSrcResource, TRUE)))
        return hr;

    return D3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette,
        pDestRect, res.m_pvData, res.m_cbData, pSrcRect, dwFilter, ColorKey,
        pSrcInfo);
}
#endif

HRESULT WINAPI
    D3DXLoadSurfaceFromFileInMemory(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        LPCVOID               pvSrcData,
        UINT                  cbSrcData,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo)
{
    HRESULT hr;
    CD3DXImage Image;
    RECT SrcRect;

    // Validate args
    if(!pDestSurface)
    {
        DPF(0, "pDestSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pvSrcData || !cbSrcData)
    {
        DPF(0, "Source surface data (pvSrcData or cbSrcData) is invalid");
        return D3DERR_INVALIDCALL;
    }

    // Pick file loader
    if(FAILED(hr = Image.Load(pvSrcData, cbSrcData, pSrcInfo)))
        return hr;

    if(pSrcRect)
    {
        SrcRect = *pSrcRect;

        if(SrcRect.left < 0 || SrcRect.right  > Image.m_Rect.right  || SrcRect.left > SrcRect.right ||
           SrcRect.top  < 0 || SrcRect.bottom > Image.m_Rect.bottom || SrcRect.top  > SrcRect.bottom)
        {
            DPF(0, "Invalid source rect");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        SrcRect = Image.m_Rect;
    }

    // Load image data into surface
    if(FAILED(hr = D3DXLoadSurfaceFromMemory(pDestSurface, pDestPalette,
        pDestRect, Image.m_pvData, Image.m_Format, Image.m_cbPitch, Image.m_pPalette,
        &SrcRect, dwFilter, ColorKey)))
    {
        return hr;
    }

    return S_OK;
}


HRESULT WINAPI
    D3DXLoadSurfaceFromSurface(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        LPDIRECT3DSURFACE8    pSrcSurface,
        CONST PALETTEENTRY*   pSrcPalette,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey)
{
    D3DSURFACE_DESC SrcDesc;
    D3DSURFACE_DESC DstDesc;
    D3DLOCKED_RECT SrcLock;
    RECT SrcRect;
    PBYTE pSrcBits = NULL;
    HRESULT hr;


    // Validate args
    if(!pDestSurface)
    {
        DPF(0, "pDestSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcSurface)
    {
        DPF(0, "pSrcSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    pSrcSurface->GetDesc(&SrcDesc);

    if(pSrcRect)
    {
        SrcRect = *pSrcRect;

        if(SrcRect.left < 0 || (DWORD) SrcRect.right  > SrcDesc.Width  || SrcRect.left > SrcRect.right ||
           SrcRect.top  < 0 || (DWORD) SrcRect.bottom > SrcDesc.Height || SrcRect.top  > SrcRect.bottom)
        {
            DPF(0, "Invalid source rect");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        SrcRect.left   = 0;
        SrcRect.top    = 0;
        SrcRect.right  = SrcDesc.Width;
        SrcRect.bottom = SrcDesc.Height;
    }


    // Get source surface data
    if(FAILED(hr = pSrcSurface->LockRect(&SrcLock, NULL, D3DLOCK_READONLY | D3DLOCK_TILED)))
    {
        DPF(0, "Could not lock source surface");
        return hr;
    }

    DBG_LINKWITHXGRAPHICS;

    if (XGIsSwizzledFormat(SrcDesc.Format))
    {
        DWORD dx, dy;

        pSrcBits = (PBYTE)LocalAlloc(0, SrcDesc.Size);

        if (pSrcBits == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LDone;
        }

        POINT point = { SrcRect.left, SrcRect.top };

        XGUnswizzleRect(
                SrcLock.pBits,
                SrcDesc.Width,
                SrcDesc.Height,
                &SrcRect,
                pSrcBits,
                SrcLock.Pitch,
                &point,
                XGBytesPerPixelFromFormat(SrcDesc.Format));
    }
    else 
    {
        pSrcBits = (PBYTE)SrcLock.pBits;
    }

    if(FAILED(hr = D3DXLoadSurfaceFromMemory(pDestSurface, pDestPalette,
        pDestRect, pSrcBits, SrcDesc.Format, SrcLock.Pitch, pSrcPalette,
        &SrcRect, dwFilter, ColorKey)))
    {
        goto LDone;
    }

    hr = S_OK;
    goto LDone;

LDone:

    if (pSrcBits != SrcLock.pBits && pSrcBits != NULL)
    {
        LocalFree(pSrcBits);
    }

    pSrcSurface->UnlockRect();
    return hr;
}


HRESULT WINAPI
    D3DXLoadSurfaceFromMemory(
        LPDIRECT3DSURFACE8    pDestSurface,
        CONST PALETTEENTRY*   pDestPalette,
        CONST RECT*           pDestRect,
        LPCVOID               pSrcMemory,
        D3DFORMAT             SrcFormat,
        UINT                  cbSrcPitch,
        CONST PALETTEENTRY*   pSrcPalette,
        CONST RECT*           pSrcRect,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey)
{
    D3DSURFACE_DESC DestDesc;
    D3DLOCKED_RECT DestLock;
    RECT DestRect;
    HRESULT hr;
    CD3DXBlt Blt;
    PBYTE pDestBits = NULL;
    DWORD dx, dy;
    BOOL bSwizzled;

    SURFACE_PORT_WARNING();

    // Validate args
    if(!pDestSurface)
    {
        DPF(0, "pDestSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcMemory)
    {
        DPF(0, "pSrcMemory pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcRect)
    {
        DPF(0, "pSrcRect pointer is invalid");
        return D3DERR_INVALIDCALL;
    }




    if(D3DX_DEFAULT == dwFilter)
    {
        if(IsUvl(SrcFormat))
            dwFilter = D3DX_FILTER_POINT | D3DX_FILTER_DITHER;
        else
            dwFilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;
    }


    pDestSurface->GetDesc(&DestDesc);

    if(pDestRect)
    {
        DestRect = *pDestRect;

        if(DestRect.left < 0 || (DWORD) DestRect.right  > DestDesc.Width  || DestRect.left > DestRect.right ||
           DestRect.top  < 0 || (DWORD) DestRect.bottom > DestDesc.Height || DestRect.top  > DestRect.bottom)
        {
            DPF(0, "Invalid destination rect");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        DestRect.left   = 0;
        DestRect.top    = 0;
        DestRect.right  = DestDesc.Width;
        DestRect.bottom = DestDesc.Height;
    }


    // Get dest surface data
    if(FAILED(hr = pDestSurface->LockRect(&DestLock, NULL, D3DLOCK_TILED)))
    {
        DPF(0, "Could not lock destination surface");
        return hr;
    }

    DBG_LINKWITHXGRAPHICS;

    bSwizzled = XGIsSwizzledFormat(DestDesc.Format);

    if (bSwizzled)
    {
        pDestBits = (PBYTE)LocalAlloc(0, DestDesc.Size);

        if (pDestBits == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LDone;
        }
    }
    else 
    {
        pDestBits = (PBYTE)DestLock.pBits;
    }

    // Blt using specified filter
    D3DX_BLT DestBlt, SrcBlt;

    DestBlt.pData            = pDestBits;
    DestBlt.RowPitch         = DestLock.Pitch;
    DestBlt.SlicePitch       = 0;
    DestBlt.Format           = DestDesc.Format;
    DestBlt.ColorKey         = 0;
    DestBlt.pPalette         = pDestPalette;

    DestBlt.Region.Left      = 0;
    DestBlt.Region.Right     = (UINT) DestDesc.Width;
    DestBlt.Region.Top       = 0;
    DestBlt.Region.Bottom    = (UINT) DestDesc.Height;
    DestBlt.Region.Front     = 0;
    DestBlt.Region.Back      = 1;

    DestBlt.SubRegion.Left   = (UINT) DestRect.left;
    DestBlt.SubRegion.Top    = (UINT) DestRect.top;
    DestBlt.SubRegion.Right  = (UINT) DestRect.right;
    DestBlt.SubRegion.Bottom = (UINT) DestRect.bottom;
    DestBlt.SubRegion.Front  = 0;
    DestBlt.SubRegion.Back   = 1;


    SrcBlt.pData             = (LPVOID) pSrcMemory;
    SrcBlt.RowPitch          = cbSrcPitch;
    SrcBlt.SlicePitch        = 0;
    SrcBlt.Format            = SrcFormat;
    SrcBlt.ColorKey          = ColorKey;
    SrcBlt.pPalette          = pSrcPalette;

    SrcBlt.SubRegion.Left    = (UINT) pSrcRect->left;
    SrcBlt.SubRegion.Top     = (UINT) pSrcRect->top;
    SrcBlt.SubRegion.Right   = (UINT) pSrcRect->right;
    SrcBlt.SubRegion.Bottom  = (UINT) pSrcRect->bottom;
    SrcBlt.SubRegion.Front   = 0;
    SrcBlt.SubRegion.Back    = 1;


    if(FAILED(hr = Blt.Blt(&DestBlt, &SrcBlt, dwFilter)))
        goto LDone;

    if (bSwizzled)
    {
        POINT point = { DestRect.left, DestRect.top };

        DBG_LINKWITHXGRAPHICS;

        XGSwizzleRect(
                pDestBits,
                DestLock.Pitch,
                &DestRect,
                DestLock.pBits,
                DestDesc.Width,
                DestDesc.Height,
                &point,
                XGBytesPerPixelFromFormat(DestDesc.Format));
    }

    hr = S_OK;
    goto LDone;

LDone:

    if (bSwizzled && pDestBits != NULL)
    {
        LocalFree(pDestBits);
    }

    pDestSurface->UnlockRect();
    return hr;
}



///////////////////////////////////////////////////////////////////////////
// Volume APIs ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


HRESULT WINAPI
    D3DXLoadVolumeFromVolume(
        LPDIRECT3DVOLUME8     pDestVolume,
        CONST PALETTEENTRY*   pDestPalette,
        CONST D3DBOX*         pDestBox,
        LPDIRECT3DVOLUME8     pSrcVolume,
        CONST PALETTEENTRY*   pSrcPalette,
        CONST D3DBOX*         pSrcBox,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey)
{
    D3DVOLUME_DESC SrcDesc;
    D3DLOCKED_BOX  SrcLock;
    D3DBOX SrcBox;
    HRESULT hr;
    PBYTE pSrcBits = NULL;


    // Validate args
    if(!pDestVolume)
    {
        DPF(0, "pDestVolume pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcVolume)
    {
        DPF(0, "pSrcVolume pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    pSrcVolume->GetDesc(&SrcDesc);

    if(pSrcBox)
    {
        SrcBox = *pSrcBox;

        if((DWORD) SrcBox.Right  > SrcDesc.Width  || SrcBox.Left  > SrcBox.Right ||
           (DWORD) SrcBox.Bottom > SrcDesc.Height || SrcBox.Top   > SrcBox.Bottom ||
           (DWORD) SrcBox.Back   > SrcDesc.Depth  || SrcBox.Front > SrcBox.Back)
        {
            DPF(0, "Invalid source box");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        SrcBox.Left   = 0;
        SrcBox.Top    = 0;
        SrcBox.Front  = 0;
        SrcBox.Right  = SrcDesc.Width;
        SrcBox.Bottom = SrcDesc.Height;
        SrcBox.Back   = SrcDesc.Depth;
    }


    if(FAILED(hr = pSrcVolume->LockBox(&SrcLock, NULL, D3DLOCK_READONLY | D3DLOCK_TILED)))
    {
        DPF(0, "Could not lock source volume");
        return hr;
    }

    DBG_LINKWITHXGRAPHICS;

    if (XGIsSwizzledFormat(SrcDesc.Format))
    {
        pSrcBits = (PBYTE)LocalAlloc(0, SrcDesc.Size);

        if (pSrcBits == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LDone;
        }

        XGPOINT3D point = { SrcBox.Left, SrcBox.Top, SrcBox.Front };

        XGUnswizzleBox(
                SrcLock.pBits,
                SrcDesc.Width,
                SrcDesc.Height,
                SrcDesc.Depth,
                &SrcBox,
                pSrcBits,
                SrcLock.RowPitch,
                SrcLock.SlicePitch,
                &point,
                XGBytesPerPixelFromFormat(SrcDesc.Format));
    }
    else 
    {
        pSrcBits = (PBYTE)SrcLock.pBits;
    }

    if(FAILED(hr = D3DXLoadVolumeFromMemory(pDestVolume, pDestPalette,
        pDestBox, pSrcBits, SrcDesc.Format, SrcLock.RowPitch, SrcLock.SlicePitch,
        pSrcPalette, &SrcBox, dwFilter, ColorKey)))
    {
        goto LDone;
    }

    hr = S_OK;
    goto LDone;

LDone:

    if (pSrcBits != SrcLock.pBits && pSrcBits != NULL)
    {
        LocalFree(pSrcBits);
    }

    pSrcVolume->UnlockBox();
    return hr;
}


HRESULT WINAPI
    D3DXLoadVolumeFromMemory(
        LPDIRECT3DVOLUME8     pDestVolume,
        CONST PALETTEENTRY*   pDestPalette,
        CONST D3DBOX*         pDestBox,
        LPCVOID               pSrcMemory,
        D3DFORMAT             SrcFormat,
        UINT                  cbSrcRowPitch,
        UINT                  cbSrcSlicePitch,
        CONST PALETTEENTRY*   pSrcPalette,
        CONST D3DBOX*         pSrcBox,
        DWORD                 dwFilter,
        D3DCOLOR              ColorKey)
{
    D3DVOLUME_DESC DestDesc;
    D3DLOCKED_BOX DestLock;
    D3DBOX DestBox;
    HRESULT hr;
    CD3DXBlt Blt;
    PBYTE pDestBits = NULL;
    BOOL bSwizzled;

    SURFACE_PORT_WARNING();

    // Validate args
    if(!pDestVolume)
    {
        DPF(0, "pDestVolume pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcMemory)
    {
        DPF(0, "pSrcMemory pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pSrcBox)
    {
        DPF(0, "pSrcBox pointer is invalid");
        return D3DERR_INVALIDCALL;
    }


    if(D3DX_DEFAULT == dwFilter)
    {
        if(IsUvl(SrcFormat))
            dwFilter = D3DX_FILTER_POINT | D3DX_FILTER_DITHER;
        else
            dwFilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;
    }


    pDestVolume->GetDesc(&DestDesc);

    if(pDestBox)
    {
        DestBox = *pDestBox;

        if((DWORD) DestBox.Right  > DestDesc.Width  || DestBox.Left  > DestBox.Right ||
           (DWORD) DestBox.Bottom > DestDesc.Height || DestBox.Top   > DestBox.Bottom ||
           (DWORD) DestBox.Back   > DestDesc.Depth  || DestBox.Front > DestBox.Back)
        {
            DPF(0, "Invalid destination box");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        DestBox.Left   = 0;
        DestBox.Top    = 0;
        DestBox.Front  = 0;
        DestBox.Right  = DestDesc.Width;
        DestBox.Bottom = DestDesc.Height;
        DestBox.Back   = DestDesc.Depth;
    }


    // Get dest Volume data
    if(FAILED(hr = pDestVolume->LockBox(&DestLock, NULL, D3DLOCK_TILED)))
    {
        DPF(0, "Could not lock destination volume");
        return hr;
    }

    DBG_LINKWITHXGRAPHICS;

    bSwizzled = XGIsSwizzledFormat(DestDesc.Format);

    if (bSwizzled)
    {
        pDestBits = (PBYTE)LocalAlloc(0, DestDesc.Size);

        if (pDestBits == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto LDone;
        }
    }
    else 
    {
        pDestBits = (PBYTE)DestLock.pBits;
    }

    // Blt using specified filter
    D3DX_BLT DestBlt, SrcBlt;

    DestBlt.pData       = pDestBits;
    DestBlt.RowPitch    = DestLock.RowPitch;
    DestBlt.SlicePitch  = DestLock.SlicePitch;
    DestBlt.SubRegion   = DestBox;
    DestBlt.Format      = DestDesc.Format;
    DestBlt.ColorKey    = 0;
    DestBlt.pPalette    = pDestPalette;

    DestBlt.Region.Left      = 0;
    DestBlt.Region.Right     = (UINT) DestDesc.Width;
    DestBlt.Region.Top       = 0;
    DestBlt.Region.Bottom    = (UINT) DestDesc.Height;
    DestBlt.Region.Front     = 0;
    DestBlt.Region.Back      = (UINT) DestDesc.Depth;

    SrcBlt.pData        = (LPVOID) pSrcMemory;
    SrcBlt.RowPitch     = cbSrcRowPitch;
    SrcBlt.SlicePitch   = cbSrcSlicePitch;
    SrcBlt.SubRegion    = *pSrcBox;
    SrcBlt.Format       = SrcFormat;
    SrcBlt.ColorKey     = ColorKey;
    SrcBlt.pPalette     = pSrcPalette;


    if(FAILED(hr = Blt.Blt(&DestBlt, &SrcBlt, dwFilter)))
        goto LDone;

    if (bSwizzled)
    {
        XGPOINT3D point = { DestBox.Left, DestBox.Top, DestBox.Front };

        DBG_LINKWITHXGRAPHICS;

        XGSwizzleBox(
                pDestBits,
                DestLock.RowPitch,
                DestLock.SlicePitch,
                &DestBox,
                DestLock.pBits,
                DestDesc.Width,
                DestDesc.Height,
                DestDesc.Depth,
                &point,
                XGBytesPerPixelFromFormat(DestDesc.Format));
    }

    hr = S_OK;
    goto LDone;

LDone:

    if (bSwizzled && pDestBits != NULL)
    {
        LocalFree(pDestBits);
    }

    pDestVolume->UnlockBox();
    return hr;
}






///////////////////////////////////////////////////////////////////////////
// Texture APIs ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

HRESULT WINAPI
    D3DXCheckTextureRequirements(
        LPDIRECT3DDEVICE8     pDevice,
        UINT*                 pWidth,
        UINT*                 pHeight,
        UINT*                 pNumMipLevels,
        DWORD                 Usage,
        D3DFORMAT*            pFormat,
        D3DPOOL               Pool)
{
    return CheckTextureRequirements(pDevice, pWidth, pHeight, NULL,
        pNumMipLevels, Usage, pFormat, Pool, 0);
}


HRESULT WINAPI
    D3DXCreateTexture(
        LPDIRECT3DDEVICE8     pDevice,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;

    // Validate arguments
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppTexture)
    {
        DPF(0, "ppTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(D3DX_DEFAULT == Usage)
        Usage = 0;

    // Check texture requirements
    if(FAILED(hr = D3DXCheckTextureRequirements(pDevice, &cpWidth, &cpHeight,
        &cMipLevels, Usage, &Format, Pool)))
    {
        return hr;
    }

    // Create texture
    if(FAILED(hr = pDevice->CreateTexture(cpWidth, cpHeight, cMipLevels,
        Usage, Format, Pool, ppTexture)))
    {
        DPF(0, "CreateTexture failed");
        return hr;
    }

    return S_OK;
}


HRESULT WINAPI
    D3DXCreateTextureFromFileA(
        LPDIRECT3DDEVICE8     pDevice,
        LPCSTR                pSrcFile,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    return D3DXCreateTextureFromFileExA(pDevice, pSrcFile, D3DX_DEFAULT,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
}

#if 0
HRESULT WINAPI
    D3DXCreateTextureFromFileW(
        LPDIRECT3DDEVICE8     pDevice,
        LPCWSTR               pSrcFile,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    return D3DXCreateTextureFromFileExW(pDevice, pSrcFile, D3DX_DEFAULT,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
}
#endif
#if 0
HRESULT WINAPI
    D3DXCreateTextureFromResourceA(
        LPDIRECT3DDEVICE8     pDevice,
        HMODULE               hSrcModule,
        LPCSTR                pSrcResource,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    return D3DXCreateTextureFromResourceExA(pDevice, hSrcModule, pSrcResource,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
}


HRESULT WINAPI
    D3DXCreateTextureFromResourceW(
        LPDIRECT3DDEVICE8     pDevice,
        HMODULE               hSrcModule,
        LPCWSTR               pSrcResource,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    return D3DXCreateTextureFromResourceExW(pDevice, hSrcModule, pSrcResource,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
}
#endif

HRESULT WINAPI
    D3DXCreateTextureFromFileExA(
        LPDIRECT3DDEVICE8     pDevice,
        LPCSTR                pSrcFile,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        DWORD                 dwFilter,
        DWORD                 dwMipFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo,
        PALETTEENTRY*         pPalette,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, FALSE)))
        return hr;

    return D3DXCreateTextureFromFileInMemoryEx(pDevice, fm.m_pvData,
        fm.m_cbData, cpWidth, cpHeight, cMipLevels, Usage, Format,
        Pool, dwFilter, dwMipFilter, ColorKey, pSrcInfo, pPalette, 
        ppTexture);
}

#if 0
HRESULT WINAPI
    D3DXCreateTextureFromFileExW(
        LPDIRECT3DDEVICE8     pDevice,
        LPCWSTR               pSrcFile,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        DWORD                 dwFilter,
        DWORD                 dwMipFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo,
        PALETTEENTRY*         pPalette,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, TRUE)))
        return hr;

    return D3DXCreateTextureFromFileInMemoryEx(pDevice, fm.m_pvData,
        fm.m_cbData, cpWidth, cpHeight, cMipLevels, Usage, Format,
        Pool, dwFilter, dwMipFilter, ColorKey, pSrcInfo, pPalette, 
        ppTexture);
}
#endif
#if 0
HRESULT WINAPI
    D3DXCreateTextureFromResourceExA(
        LPDIRECT3DDEVICE8     pDevice,
        HMODULE               hSrcModule,
        LPCSTR                pSrcResource,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        DWORD                 dwFilter,
        DWORD                 dwMipFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo,
        PALETTEENTRY*         pPalette,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;
    CD3DXResource res;

    if(FAILED(hr = res.Open(hSrcModule, pSrcResource, FALSE)))
        return hr;

    return D3DXCreateTextureFromFileInMemoryEx(pDevice, res.m_pvData,
        res.m_cbData, cpWidth, cpHeight, cMipLevels, Usage, Format,
        Pool, dwFilter, dwMipFilter, ColorKey, pSrcInfo, pPalette, 
        ppTexture);
}


HRESULT WINAPI
    D3DXCreateTextureFromResourceExW(
        LPDIRECT3DDEVICE8     pDevice,
        HMODULE               hSrcModule,
        LPCWSTR               pSrcResource,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        DWORD                 dwFilter,
        DWORD                 dwMipFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo,
        PALETTEENTRY*         pPalette,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;
    CD3DXResource res;

    if(FAILED(hr = res.Open(hSrcModule, pSrcResource, TRUE)))
        return hr;

    return D3DXCreateTextureFromFileInMemoryEx(pDevice, res.m_pvData,
        res.m_cbData, cpWidth, cpHeight, cMipLevels, Usage, Format,
        Pool, dwFilter, dwMipFilter, ColorKey, pSrcInfo, pPalette, 
        ppTexture);
}
#endif

HRESULT WINAPI
    D3DXCreateTextureFromFileInMemory(
        LPDIRECT3DDEVICE8     pDevice,
        LPCVOID               pvSrcData,
        UINT                  cbSrcData,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    return D3DXCreateTextureFromFileInMemoryEx(pDevice, pvSrcData, cbSrcData,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTexture);
}


HRESULT WINAPI
    D3DXCreateTextureFromFileInMemoryEx(
        LPDIRECT3DDEVICE8     pDevice,
        LPCVOID               pvSrcData,
        UINT                  cbSrcData,
        UINT                  cpWidth,
        UINT                  cpHeight,
        UINT                  cMipLevels,
        DWORD                 Usage,
        D3DFORMAT             Format,
        D3DPOOL               Pool,
        DWORD                 dwFilter,
        DWORD                 dwMipFilter,
        D3DCOLOR              ColorKey,
        D3DXIMAGE_INFO*       pSrcInfo,
        PALETTEENTRY*         pPalette,
        LPDIRECT3DTEXTURE8*   ppTexture)
{
    HRESULT hr;
    UINT iLevel;
    CD3DXImage Image;
    CD3DXImage *pImage;
    UINT cImageLevels;
    PALETTEENTRY Palette[256];
    BOOL bUpdate;
    BOOL bCascade;

    LPDIRECT3DSURFACE8 pSrc        = NULL;
    LPDIRECT3DSURFACE8 pDest       = NULL;
    LPDIRECT3DTEXTURE8 pTexture    = NULL;
    LPDIRECT3DTEXTURE8 pTextureVid = NULL;
    LPDIRECT3DTEXTURE8 pTextureSys = NULL;



    // Validate args
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pvSrcData)
    {
        DPF(0, "pvSrcData pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!cbSrcData)
    {
        DPF(0, "cbSrcData value is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppTexture)
    {
        DPF(0, "ppTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }


    // Load file, and figure out how many mip-levels were loaded
    if(FAILED(hr = Image.Load(pvSrcData, cbSrcData, pSrcInfo)))
        return hr;

    cImageLevels = 1;
    pImage = Image.m_pMip;

    while(pImage)
    {
        cImageLevels++;
        pImage = pImage->m_pMip;
    }


    // Default values
    if(0 == cpWidth || D3DX_DEFAULT == cpWidth)
        cpWidth = Image.m_Rect.right;

    if(0 == cpHeight || D3DX_DEFAULT == cpHeight)
        cpHeight = Image.m_Rect.bottom;


    if(D3DFMT_UNKNOWN == Format)
    {
        if(ColorKey)
        {
            // If we are using colorkey, try to pick a format with alpha.
            switch(Image.m_Format)
            {
            case D3DFMT_R8G8B8:
            case D3DFMT_X8R8G8B8:
            case D3DFMT_R5G6B5:
                Format = D3DFMT_A8R8G8B8;
                break;

            case D3DFMT_X1R5G5B5:
#if 0
            case D3DFMT_R3G3B2:
            case D3DFMT_X4R4G4B4:
#endif
                Format = D3DFMT_A1R5G5B5;
                break;

            case D3DFMT_L8:
                Format = D3DFMT_A8L8;
                break;

            default:
                Format = Image.m_Format;
                break;
            }
        }
        else
        {
            // Avoid strange things, like 24 bit formats
            Format = (D3DFMT_R8G8B8 == Image.m_Format) ? D3DFMT_X8R8G8B8 : Image.m_Format;
        }
    }

    if(D3DX_DEFAULT == dwFilter)
    {
        if(IsUvl(Image.m_Format))
            dwFilter = D3DX_FILTER_POINT | D3DX_FILTER_DITHER;
        else
            dwFilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;
    }

    if(D3DX_DEFAULT == dwMipFilter)
    {
        if(IsUvl(Image.m_Format))
            dwMipFilter = D3DX_FILTER_POINT;
        else
            dwMipFilter = D3DX_FILTER_BOX;
    }

    bCascade = 
        ((dwFilter    & 0xff) == D3DX_FILTER_NONE)  ||
        ((dwMipFilter & 0xff) == D3DX_FILTER_POINT) || 
        ((dwMipFilter & 0xff) == D3DX_FILTER_BOX);


    // Compute palette
    if(Image.m_pPalette)
    {
        // Convert colorkey from ARGB to ABGR
        ColorKey = (ColorKey & 0xff00ff00) | ((ColorKey >> 16) & 0x000000ff) | ((ColorKey << 16) & 0x00ff0000);

        // Apply colorkey to palette.  This needs to be done here, since it might
        // influence how we choose image formats later.
        for(UINT i = 0; i < 256; i++)
        {
            if(*((DWORD*) &Image.m_pPalette[i]) == (DWORD) ColorKey)
                *((DWORD*) &Palette[i]) = 0;
            else
                *((DWORD*) &Palette[i]) = *((DWORD*) &Image.m_pPalette[i]);
        }

        ColorKey = 0;
    }
    else
    {
        // Image was not paletteized.. using opaque-white default palette
        memset(Palette, 0xff, 256 * sizeof(PALETTEENTRY));
    }

    if(pPalette)
    {
        // Copy palette into user's buffer
        memcpy(pPalette, Palette, 256 * sizeof(PALETTEENTRY));
    }
    else
    {
        // Since user did not provice us any memory to return a palette, we
        // need to depalattize the image.. but to what format?  Will try for
        // A8R8G8B8 if image contains alpha.  X8R8G8B8 if not.
#if 0
        if(D3DFMT_A8P8 == Format)
        {
            Format = D3DFMT_A8R8G8B8;
        }
        else 
#endif            
        if(D3DFMT_P8 == Format)
        {
            Format = D3DFMT_X8R8G8B8;

            for(UINT i = 0; i < 256; i++)
            {
                if(0xff != Image.m_pPalette[i].peFlags)
                {
                    Format = D3DFMT_A8R8G8B8;
                    break;
                }
            }
        }
    }



    // Create texture
    if(FAILED(hr = D3DXCheckTextureRequirements(pDevice, &cpWidth, &cpHeight,
        &cMipLevels, Usage, &Format, Pool)))
    {
        goto LDone;
    }

    if(FAILED(hr = pDevice->CreateTexture(cpWidth, cpHeight, cMipLevels,
        Usage, Format, Pool, &pTextureVid)))
    {
        DPF(0, "CreateTexture failed");
        goto LDone;
    }

    pTexture = pTextureVid;

#if 0
    // Create dummy system texture, if needed.
    bUpdate = (D3DPOOL_DEFAULT == Pool);

    if(bUpdate)
    {
        if(FAILED(hr = pDevice->CreateTexture(cpWidth, cpHeight, cMipLevels,
            0, Format, D3DPOOL_SYSTEMMEM, &pTextureSys)))
        {
            DPF(0, "CreateTexture failed");
            goto LDone;
        }

        pTexture = pTextureSys;
    }
#else
    bUpdate = FALSE;
#endif

    // Load image data
    iLevel = 0;
    pImage = &Image;

    while((iLevel < cImageLevels) && (iLevel < cMipLevels))
    {
        if(FAILED(hr = pTexture->GetSurfaceLevel(iLevel, &pDest)))
            goto LDone;

        hr = D3DXLoadSurfaceFromMemory(pDest, pPalette, NULL,
            pImage->m_pvData, pImage->m_Format, pImage->m_cbPitch, Palette,
            &pImage->m_Rect, dwFilter, ColorKey);

        RELEASE(pDest);

        if(FAILED(hr))
            goto LDone;

        iLevel++;

        if(iLevel < cImageLevels)
            pImage = pImage->m_pMip;
    }


    // Filter mipmaps
    if(!bCascade)
    {
        while(iLevel < cMipLevels)
        {
            if(FAILED(hr = pTexture->GetSurfaceLevel(iLevel, &pDest)))
                goto LDone;

            hr = D3DXLoadSurfaceFromMemory(pDest, pPalette, NULL,
                pImage->m_pvData, pImage->m_Format, pImage->m_cbPitch, Palette,
                &pImage->m_Rect, dwMipFilter, ColorKey);

            RELEASE(pDest);

            if(FAILED(hr))
                goto LDone;

            iLevel++;
        }
    }
    else 
    {
        if(cImageLevels < cMipLevels)
        {
            if(FAILED(hr = D3DXFilterTexture(pTexture, Palette, cImageLevels - 1, dwMipFilter)))
                goto LDone;
        }
    }


    // Update or Copy result to texture, if needed
#if 0
    if(bUpdate)
    {
        if(FAILED(hr = pDevice->UpdateTexture(pTextureSys, pTextureVid)))
            goto LDone;
    }
#endif

    *ppTexture = pTextureVid;
    pTextureVid = NULL;

    hr = S_OK;
    goto LDone;

LDone:
    RELEASE(pDest);
    RELEASE(pSrc);
    RELEASE(pTextureVid);
    RELEASE(pTextureSys);
    return hr;
}


HRESULT WINAPI
    D3DXFilterTexture(
        LPDIRECT3DTEXTURE8    pTexture,
        CONST PALETTEENTRY*   pPalette,
        UINT                  uSrcLevel,
        DWORD                 dwFilter)
{
    HRESULT hr;

    // Validate arguments
    if(!pTexture)
    {
        DPF(0, "pTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(D3DX_DEFAULT == dwFilter)
    {
        D3DSURFACE_DESC desc;
        pTexture->GetLevelDesc(0, &desc);

        if(IsUvl(desc.Format))
            dwFilter = D3DX_FILTER_POINT;
        else
            dwFilter = D3DX_FILTER_BOX;
    }

    UINT uMipLevel;
    UINT cMipLevels = pTexture->GetLevelCount();

    if(D3DX_DEFAULT == uSrcLevel)
        uSrcLevel = 0;

    if(uSrcLevel >= cMipLevels)
    {
        DPF(0, "uSrcLevel larger than num Mip level in the texture");
        return D3DERR_INVALIDCALL;
    }


    // For each level, filter it down
    LPDIRECT3DSURFACE8 pSrc  = NULL;
    LPDIRECT3DSURFACE8 pDest = NULL;

    BOOL bCascade = ((dwFilter & 0xff) == D3DX_FILTER_POINT) ||
                    ((dwFilter & 0xff) == D3DX_FILTER_BOX);

    if(FAILED(hr = pTexture->GetSurfaceLevel(uSrcLevel, &pSrc)))
        goto LDone;

    for(uMipLevel = uSrcLevel + 1; uMipLevel < cMipLevels; uMipLevel++)
    {
        if(FAILED(hr = pTexture->GetSurfaceLevel(uMipLevel, &pDest)))
            goto LDone;

        if(FAILED(hr = D3DXLoadSurfaceFromSurface(pDest,
            pPalette, NULL, pSrc, pPalette, NULL, dwFilter, 0)))
        {
            goto LDone;
        }


        if(bCascade)
        {
            RELEASE(pSrc);

            pSrc  = pDest;
            pDest = NULL;
        }
        else
        {
            RELEASE(pDest);
        }
    }

    hr = S_OK;
    goto LDone;

LDone:
    RELEASE(pSrc);
    RELEASE(pDest);

    return hr;
}



///////////////////////////////////////////////////////////////////////////
// CubeTexture APIs ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

HRESULT WINAPI
    D3DXCheckCubeTextureRequirements(
        LPDIRECT3DDEVICE8       pDevice,
        UINT*                   pSize,
        UINT*                   pNumMipLevels,
        DWORD                   Usage,
        D3DFORMAT*              pFormat,
        D3DPOOL                 Pool)
{
    return CheckTextureRequirements(pDevice, pSize, pSize, NULL,
        pNumMipLevels, Usage, pFormat, Pool, 1);
}


HRESULT WINAPI
    D3DXCreateCubeTexture(
        LPDIRECT3DDEVICE8       pDevice,
        UINT                    cpSize,
        UINT                    cMipLevels,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    HRESULT hr;

    // Validate arguments
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppCubeTexture)
    {
        DPF(0, "ppCubeTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(D3DX_DEFAULT == Usage)
        Usage = 0;

    // Check cube texture requirements
    if(FAILED(hr = D3DXCheckCubeTextureRequirements(pDevice, &cpSize,
        &cMipLevels, Usage, &Format, Pool)))
    {
        return hr;
    }

    // Create cube texture
    if(FAILED(hr = pDevice->CreateCubeTexture(cpSize, cMipLevels,
        Usage, Format, Pool, ppCubeTexture)))
    {
        DPF(0, "CreateCubeTexture failed");
        return hr;
    }

    return S_OK;
}


HRESULT WINAPI
    D3DXCreateCubeTextureFromFileA(
        LPDIRECT3DDEVICE8       pDevice,
        LPCSTR                  pSrcFile,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    return D3DXCreateCubeTextureFromFileExA(pDevice, pSrcFile, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, NULL, NULL, ppCubeTexture);
}

#if 0
HRESULT WINAPI
    D3DXCreateCubeTextureFromFileW(
        LPDIRECT3DDEVICE8       pDevice,
        LPCWSTR                 pSrcFile,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    return D3DXCreateCubeTextureFromFileExW(pDevice, pSrcFile, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, 0, NULL, NULL, ppCubeTexture);
}
#endif

HRESULT WINAPI
    D3DXCreateCubeTextureFromFileExA(
        LPDIRECT3DDEVICE8       pDevice,
        LPCSTR                  pSrcFile,
        UINT                    cpSize,
        UINT                    cMipLevels,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        DWORD                   dwFilter,
        DWORD                   dwMipFilter,
        D3DCOLOR                ColorKey,
        D3DXIMAGE_INFO*         pSrcInfo,
        PALETTEENTRY*           pPalette,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, FALSE)))
        return hr;

    return D3DXCreateCubeTextureFromFileInMemoryEx(pDevice, fm.m_pvData,
        fm.m_cbData, cpSize, cMipLevels, Usage, Format, Pool, dwFilter,
        dwMipFilter, ColorKey, pSrcInfo, pPalette, ppCubeTexture);
}

#if 0
HRESULT WINAPI
    D3DXCreateCubeTextureFromFileExW(
        LPDIRECT3DDEVICE8       pDevice,
        LPCWSTR                 pSrcFile,
        UINT                    cpSize,
        UINT                    cMipLevels,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        DWORD                   dwFilter,
        DWORD                   dwMipFilter,
        D3DCOLOR                ColorKey,
        D3DXIMAGE_INFO*         pSrcInfo,
        PALETTEENTRY*           pPalette,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    HRESULT hr;
    CD3DXFile fm;

    if(FAILED(hr = fm.Open(pSrcFile, TRUE)))
        return hr;

    return D3DXCreateCubeTextureFromFileInMemoryEx(pDevice, fm.m_pvData,
        fm.m_cbData, cpSize, cMipLevels, Usage, Format,
        Pool, dwFilter, dwMipFilter, ColorKey, pSrcInfo, pPalette, 
        ppCubeTexture);
}
#endif

HRESULT WINAPI
    D3DXCreateCubeTextureFromFileInMemory(
        LPDIRECT3DDEVICE8       pDevice,
        LPCVOID                 pvSrcData,
        UINT                    cbSrcData,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    return D3DXCreateCubeTextureFromFileInMemoryEx(pDevice, pvSrcData, cbSrcData,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppCubeTexture);
}


HRESULT WINAPI
    D3DXCreateCubeTextureFromFileInMemoryEx(
        LPDIRECT3DDEVICE8       pDevice,
        LPCVOID                 pvSrcData,
        UINT                    cbSrcData,
        UINT                    cpSize,
        UINT                    cMipLevels,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        DWORD                   dwFilter,
        DWORD                   dwMipFilter,
        D3DCOLOR                ColorKey,
        D3DXIMAGE_INFO*         pSrcInfo,
        PALETTEENTRY*           pPalette,
        LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    HRESULT hr;
    UINT iLevel;
    UINT iFace;
    CD3DXImage Image;
    CD3DXImage *pImage;
    CD3DXImage *pImageFace;
    UINT cImageFaces;
    UINT cImageLevels;
    PALETTEENTRY Palette[256];
    BOOL bUpdate;
    BOOL bCascade;

    LPDIRECT3DSURFACE8 pSrc  = NULL;
    LPDIRECT3DSURFACE8 pDest = NULL;
    LPDIRECT3DCUBETEXTURE8 pCubeTexture    = NULL;
    LPDIRECT3DCUBETEXTURE8 pCubeTextureVid = NULL;
    LPDIRECT3DCUBETEXTURE8 pCubeTextureSys = NULL;



    // Validate args
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!pvSrcData)
    {
        DPF(0, "pvSrcData pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!cbSrcData)
    {
        DPF(0, "cbSrcData value is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppCubeTexture)
    {
        DPF(0, "ppCubeTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }


    // Load file, and figure out how many faces and mip-levels were loaded
    if(FAILED(hr = Image.Load(pvSrcData, cbSrcData, pSrcInfo)))
        return hr;

    cImageFaces = 1;
    pImage = Image.m_pFace;

    while(pImage)
    {
        cImageFaces++;
        pImage = pImage->m_pFace;
    }

    if(cImageFaces != 6)
    {
        DPF(0, "File does not contain a cube texture");
        return E_FAIL;
    }

    cImageLevels = 1;
    pImage = Image.m_pMip;

    while(pImage)
    {
        cImageLevels++;
        pImage = pImage->m_pMip;
    }


    // Default values
    if(0 == cpSize || D3DX_DEFAULT == cpSize)
        cpSize = Image.m_Rect.right > Image.m_Rect.bottom ? Image.m_Rect.right : Image.m_Rect.bottom;

    if(D3DX_DEFAULT == dwFilter)
    {
        if(IsUvl(Image.m_Format))
            dwFilter = D3DX_FILTER_POINT | D3DX_FILTER_DITHER;
        else
            dwFilter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;
    }

    if(D3DX_DEFAULT == dwMipFilter)
    {
        if(IsUvl(Image.m_Format))
            dwMipFilter = D3DX_FILTER_POINT;
        else
            dwMipFilter = D3DX_FILTER_BOX;
    }

    dwFilter |= D3DX_FILTER_MIRROR;
    dwMipFilter |= D3DX_FILTER_MIRROR;

    bCascade = 
        ((dwFilter    & 0xff) == D3DX_FILTER_NONE)  ||
        ((dwMipFilter & 0xff) == D3DX_FILTER_POINT) || 
        ((dwMipFilter & 0xff) == D3DX_FILTER_BOX);



    if(D3DFMT_UNKNOWN == Format)
    {
        if(ColorKey)
        {
            // If we are using colorkey, try to pick a format with alpha.
            switch(Image.m_Format)
            {
#if 0
            case D3DFMT_R8G8B8:
#endif
            case D3DFMT_X8R8G8B8:
            case D3DFMT_R5G6B5:
                Format = D3DFMT_A8R8G8B8;
                break;

            case D3DFMT_X1R5G5B5:
#if 0
            case D3DFMT_R3G3B2:
            case D3DFMT_X4R4G4B4:
#endif
                Format = D3DFMT_A1R5G5B5;
                break;

            case D3DFMT_L8:
                Format = D3DFMT_A8L8;
                break;

            default:
                Format = Image.m_Format;
                break;
            }
        }
        else
        {
            // Avoid strange things, like 24 bit formats
#if 0
            Format = (D3DFMT_R8G8B8 == Image.m_Format) ? D3DFMT_X8R8G8B8 : Image.m_Format;
#else
            Format = Image.m_Format;
#endif
        }
    }


    // Compute palette
    if(Image.m_pPalette)
    {
        // Convert colorkey from ARGB to ABGR
        ColorKey = (ColorKey & 0xff00ff00) | ((ColorKey >> 16) & 0x000000ff) | ((ColorKey << 16) & 0x00ff0000);

        // Apply colorkey to palette.  This needs to be done here, since it might
        // influence how we choose image formats later.
        for(UINT i = 0; i < 256; i++)
        {
            if(*((DWORD*) &Image.m_pPalette[i]) == (DWORD) ColorKey)
                *((DWORD*) &Palette[i]) = 0;
            else
                *((DWORD*) &Palette[i]) = *((DWORD*) &Image.m_pPalette[i]);
        }

        ColorKey = 0;
    }
    else
    {
        // Image was not paletteized.. using opaque-white default palette
        memset(Palette, 0xff, 256 * sizeof(PALETTEENTRY));
    }

    if(pPalette)
    {
        // Copy palette into user's buffer
        memcpy(pPalette, Palette, 256 * sizeof(PALETTEENTRY));
    }
    else
    {
        // Since user did not provice us any memory to return a palette, we
        // need to depalattize the image.. but to what format?  Will try for
        // A8R8G8B8 if image contains alpha.  X8R8G8B8 if not.
#if 0
        if(D3DFMT_A8P8 == Format)
        {
            Format = D3DFMT_A8R8G8B8;
        }
        else
#endif            
        if(D3DFMT_P8 == Format)
        {
            Format = D3DFMT_X8R8G8B8;

            for(UINT i = 0; i < 256; i++)
            {
                if(0xff != Image.m_pPalette[i].peFlags)
                {
                    Format = D3DFMT_A8R8G8B8;
                    break;
                }
            }
        }
    }



    // Create texture
    if(FAILED(hr = D3DXCheckCubeTextureRequirements(pDevice, &cpSize,
        &cMipLevels, Usage, &Format, Pool)))
    {
        goto LDone;
    }

    if(FAILED(hr = pDevice->CreateCubeTexture(cpSize, cMipLevels,
        Usage, Format, Pool, &pCubeTextureVid)))
    {
        DPF(0, "CreateCubeTexture failed");
        goto LDone;
    }

    pCubeTexture = pCubeTextureVid;

#if 0
    // Create dummy system texture, if needed.
    bUpdate = (D3DPOOL_DEFAULT == Pool);

    if(bUpdate)
    {
        if(FAILED(hr = pDevice->CreateCubeTexture(cpSize, cMipLevels,
            0, Format, D3DPOOL_SYSTEMMEM, &pCubeTextureSys)))
        {
            DPF(0, "CreateCubeTexture failed");
            goto LDone;
        }

        pCubeTexture = pCubeTextureSys;
    }
#else
    bUpdate = FALSE;
#endif

    // Load image data
    pImageFace = &Image;

    for(iFace = 0; iFace < 6; iFace++)
    {
        iLevel = 0;
        pImage = pImageFace;

        while((iLevel < cImageLevels) && (iLevel < cMipLevels))
        {
            if(FAILED(hr = pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES) iFace, iLevel, &pDest)))
                goto LDone;

            if(FAILED(hr = D3DXLoadSurfaceFromMemory(pDest, pPalette, NULL,
                pImage->m_pvData, pImage->m_Format, pImage->m_cbPitch, Palette,
                &pImage->m_Rect, dwFilter, ColorKey)))
            {
                goto LDone;
            }

            RELEASE(pDest);

            iLevel++;

            if(iLevel < cImageLevels)
                pImage = pImage->m_pMip;
        }

        // Filter non-cascaded mipmaps
        if(!bCascade)
        {
            while(iLevel < cMipLevels)
            {
                if(FAILED(hr = pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES) iFace, iLevel, &pDest)))
                    goto LDone;

                if(FAILED(hr = D3DXLoadSurfaceFromMemory(pDest, pPalette, NULL,
                    pImage->m_pvData, pImage->m_Format, pImage->m_cbPitch, Palette,
                    &pImage->m_Rect, dwMipFilter, ColorKey)))
                {
                    goto LDone;
                }

                RELEASE(pDest);
                iLevel++;
            }
        }

        pImageFace = pImageFace->m_pFace;
    }


    // Filter cascaded mipmaps
    if(bCascade && (cImageLevels < cMipLevels))
    {
        if(FAILED(hr = D3DXFilterCubeTexture(pCubeTexture, Palette, cImageLevels - 1, dwMipFilter)))
            goto LDone;
    }


    // Update or Copy result to texture, if needed
#if 0
    if(bUpdate)
    {
        if(FAILED(hr = pDevice->UpdateTexture(pCubeTextureSys, pCubeTextureVid)))
            goto LDone;
    }
#endif

    *ppCubeTexture = pCubeTextureVid;
    pCubeTextureVid = NULL;

    hr = S_OK;
    goto LDone;

LDone:
    RELEASE(pDest);
    RELEASE(pSrc);
    RELEASE(pCubeTextureVid);
    RELEASE(pCubeTextureSys);
    return hr;
}


HRESULT WINAPI
    D3DXFilterCubeTexture(
        LPDIRECT3DCUBETEXTURE8  pCubeTexture,
        CONST PALETTEENTRY*     pPalette,
        UINT                    uSrcLevel,
        DWORD                   dwFilter)
{
    HRESULT hr;

    // Validate arguments
    if(!pCubeTexture)
    {
        DPF(0, "pCubeTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(D3DX_DEFAULT == dwFilter)
    {
        D3DSURFACE_DESC desc;
        pCubeTexture->GetLevelDesc(0, &desc);

        if(IsUvl(desc.Format))
            dwFilter = D3DX_FILTER_POINT;
        else
            dwFilter = D3DX_FILTER_BOX;
    }

    dwFilter |= D3DX_FILTER_MIRROR;

    UINT iMipLevel;
    UINT cMipLevels = pCubeTexture->GetLevelCount();

    if(D3DX_DEFAULT == uSrcLevel)
        uSrcLevel = 0;

    if(uSrcLevel >= cMipLevels)
    {
        DPF(0, "uSrcLevel larger than num Mip level in the texture");
        return D3DERR_INVALIDCALL;
    }



    // For each level, filter it down
    LPDIRECT3DSURFACE8 pSrc  = NULL;
    LPDIRECT3DSURFACE8 pDest = NULL;

    BOOL bCascade = ((dwFilter & 0xff) == D3DX_FILTER_POINT) ||
                    ((dwFilter & 0xff) == D3DX_FILTER_BOX);


    for(UINT iFace = 0; iFace < 6; iFace++)
    {
        if(FAILED(hr = pCubeTexture->GetCubeMapSurface(
            (D3DCUBEMAP_FACES) iFace, uSrcLevel, &pSrc)))
        {
            goto LDone;
        }

        for(iMipLevel = uSrcLevel + 1; iMipLevel < cMipLevels; iMipLevel++)
        {
            if(FAILED(hr = pCubeTexture->GetCubeMapSurface(
                (D3DCUBEMAP_FACES) iFace, iMipLevel, &pDest)))
            {
                goto LDone;
            }

            if(FAILED(hr = D3DXLoadSurfaceFromSurface(pDest,
                pPalette, NULL, pSrc, pPalette, NULL, dwFilter, 0)))
            {
                goto LDone;
            }


            if(bCascade)
            {
                RELEASE(pSrc);

                pSrc  = pDest;
                pDest = NULL;
            }
            else
            {
                RELEASE(pDest);
            }
        }

        RELEASE(pSrc);
    }

    hr = S_OK;
    goto LDone;

LDone:
    RELEASE(pSrc);
    RELEASE(pDest);

    return hr;
}


///////////////////////////////////////////////////////////////////////////
// VolumeTexture APIs /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

HRESULT WINAPI
    D3DXCheckVolumeTextureRequirements(
        LPDIRECT3DDEVICE8     pDevice,
        UINT*                 pWidth,
        UINT*                 pHeight,
        UINT*                 pDepth,
        UINT*                 pNumMipLevels,
        DWORD                 Usage,
        D3DFORMAT*            pFormat,
        D3DPOOL               Pool)
{
    return CheckTextureRequirements(pDevice, pWidth, pHeight, pDepth,
        pNumMipLevels, Usage, pFormat, Pool, 2);
}


HRESULT WINAPI
    D3DXCreateVolumeTexture(
        LPDIRECT3DDEVICE8         pDevice,
        UINT                      cpWidth,
        UINT                      cpHeight,
        UINT                      cpDepth,
        UINT                      cMipLevels,
        DWORD                     Usage,
        D3DFORMAT                 Format,
        D3DPOOL                   Pool,
       LPDIRECT3DVOLUMETEXTURE8*  ppVolumeTexture)
{
    HRESULT hr;

    // Validate arguments
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!ppVolumeTexture)
    {
        DPF(0, "ppVolumeTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(D3DX_DEFAULT == Usage)
        Usage = 0;

    // Check volume texture requirements
    if(FAILED(hr = D3DXCheckVolumeTextureRequirements(pDevice, &cpWidth,
        &cpHeight, &cpDepth, &cMipLevels, Usage, &Format, Pool)))
    {
        return hr;
    }

    // Create volume texture
    if(FAILED(hr = pDevice->CreateVolumeTexture(cpWidth, cpHeight, cpDepth,
        cMipLevels, Usage, Format, Pool, ppVolumeTexture)))
    {
        DPF(0, "CreateVolumeTexture failed");
        return hr;
    }

    return S_OK;
}


HRESULT WINAPI
    D3DXFilterVolumeTexture(
        LPDIRECT3DVOLUMETEXTURE8  pVolumeTexture,
        CONST PALETTEENTRY*       pPalette,
        UINT                      uSrcLevel,
        DWORD                     dwFilter)
{
    HRESULT hr;

    // Validate arguments
    if(!pVolumeTexture)
    {
        DPF(0, "pVolumeTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(D3DX_DEFAULT == dwFilter)
    {
        D3DVOLUME_DESC desc;
        pVolumeTexture->GetLevelDesc(0, &desc);

        if(IsUvl(desc.Format))
            dwFilter = D3DX_FILTER_POINT;
        else
            dwFilter = D3DX_FILTER_BOX;
    }


    UINT iMipLevel;
    UINT cMipLevels = pVolumeTexture->GetLevelCount();

    if(D3DX_DEFAULT == uSrcLevel)
        uSrcLevel = 0;

    if(uSrcLevel >= cMipLevels)
    {
        DPF(0, "uSrcLevel larger than num Mip level in the volume");
        return D3DERR_INVALIDCALL;
    }


    // For each level, filter it down
    LPDIRECT3DVOLUME8 pSrc  = NULL;
    LPDIRECT3DVOLUME8 pDest = NULL;

    BOOL bCascade = ((dwFilter & 0xff) == D3DX_FILTER_POINT) ||
                    ((dwFilter & 0xff) == D3DX_FILTER_BOX);


    if(FAILED(hr = pVolumeTexture->GetVolumeLevel(uSrcLevel, &pSrc)))
        goto LDone;

    for(iMipLevel = uSrcLevel + 1; iMipLevel < cMipLevels; iMipLevel++)
    {
        if(FAILED(hr = pVolumeTexture->GetVolumeLevel(iMipLevel, &pDest)))
            goto LDone;

        if(FAILED(hr = D3DXLoadVolumeFromVolume(pDest,
            pPalette, NULL, pSrc, pPalette, NULL, dwFilter, 0)))
        {
            goto LDone;
        }


        if(bCascade)
        {
            RELEASE(pSrc);

            pSrc  = pDest;
            pDest = NULL;
        }
        else
        {
            RELEASE(pDest);
        }
    }

    RELEASE(pSrc);

    hr = S_OK;
    goto LDone;

LDone:
    RELEASE(pSrc);
    RELEASE(pDest);

    return hr;
}


