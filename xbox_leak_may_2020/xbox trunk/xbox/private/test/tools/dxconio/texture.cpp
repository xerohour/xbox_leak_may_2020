/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    texture.cpp

Description:

    Direct3D texture routines.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include <limits.h>
#include "d3denum.h"
#include "d3dinit.h"
#include "util.h"
#include "texture.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

namespace DXCONIO {

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

//******************************************************************************
// Structures
//******************************************************************************

typedef struct _TEXTUREDESC {
    LPDIRECT3DBASETEXTURE8  pd3dt;
    TCHAR                   szName[MAX_PATH];
    WORD                    wResourceID;
    UINT                    uWidth;
    UINT                    uHeight;
    UINT                    uDepth;
    D3DFORMAT               fmt;
    TEXTURETYPE             tt;
    D3DPOOL                 pool;
    float                   fAlpha;
    D3DCOLOR                cColorKey;
    struct _TEXTUREDESC*    ptexdNext;
} TEXTUREDESC, *PTEXTUREDESC;

//******************************************************************************
typedef struct _ARGBPIXELDESC {
    DWORD   dwAMask;
    DWORD   dwRMask;
    DWORD   dwGMask;
    DWORD   dwBMask;
    DWORD   dwAShift;
    DWORD   dwRShift;
    DWORD   dwGShift;
    DWORD   dwBShift;
    UINT    uStride;
} ARGBPIXELDESC, *PARGBPIXELDESC;

//******************************************************************************
// Local function prototypes
//******************************************************************************

static PTEXTUREDESC             GetTextureDesc(LPDIRECT3DBASETEXTURE8 pd3dt);
static void                     GetARGBPixelDesc(D3DFORMAT fmt, PARGBPIXELDESC ppixd);

/*
static BOOL                     InitPalette(LPPALETTEENTRY ppe, UINT uNumEntries);

static BOOL                     ReduceResolution(LPDIRECTDRAWSURFACE7 pddsSrc);
*/

//******************************************************************************
// Globals
//******************************************************************************

PTEXTUREDESC                    g_ptexdList = NULL;

//******************************************************************************
// Texture functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateTexture
//
// Description:
//
//     Create a IDirectDrawSurface7 texture object.
//
// Arguments:
//
//     DWORD dwWidth                        - Width of the surface
//
//     DWORD dwHeight                       - Height of the surface
//
//     PIXELFORMAT pxf                      - Optional pixel format of the 
//                                            texture to create
//
//     MEMORYPOOL mp                        - Location in which to create the
//                                            texture
//
//     TEXTURETYPE tt                       - Type of texture to create (i.e.
//                                            mipmap, cubemap, bumpmap, etc.)
//
// Return Value:
//
//     A pointer to the created IDirectDrawSurface7 object on 
//     success, NULL on failure.
//
//******************************************************************************
LPDIRECT3DBASETEXTURE8 CreateTexture(UINT uWidth, UINT uHeight, D3DFORMAT fmt, 
                                    TEXTURETYPE tt, D3DPOOL pool)
{
    return CreateTexture(uWidth, uHeight, 0, fmt, tt, pool);
}

//******************************************************************************
LPDIRECT3DBASETEXTURE8 CreateTexture(UINT uWidth, UINT uHeight, UINT uDepth,
                                    D3DFORMAT fmt, TEXTURETYPE tt, D3DPOOL pool)
{
    LPDIRECT3DTEXTURE8  pd3dt;
    D3DFORMAT           fmtt;
    PTEXTUREDESC        ptexdNode;
    DWORD               dwUsage = 0;
    D3DRESOURCETYPE     rt;
    HRESULT             hr;

    // Verify the current device supports the given format
    if (tt & TTYPE_DEPTH) {
        dwUsage |= D3DUSAGE_DEPTHSTENCIL;
    }
    else if (tt & TTYPE_TARGET) {
        dwUsage |= D3DUSAGE_RENDERTARGET;
    }

    if (tt & TTYPE_VOLUME) {
        rt = D3DRTYPE_VOLUMETEXTURE;
    }
    else if (tt & TTYPE_CUBEMAP) {
        rt = D3DRTYPE_CUBETEXTURE;
    }
    else {
        rt = D3DRTYPE_TEXTURE;
    }

    hr = g_pd3d->CheckDeviceFormat(g_disdCurrent.padpd->uAdapter, 
                                    g_disdCurrent.pdevd->d3ddt, 
                                    g_disdCurrent.pdm->d3ddm.Format, 
                                    dwUsage, rt, fmt);
//##HACK to get around checkdeviceformats failing on valid formats
/*
    if (FAILED(hr)) {
        if (hr != D3DERR_NOTAVAILABLE) {
            ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
        }
        return NULL;
    }
*/
    // Create a texture of the specified type
    if (tt & TTYPE_VOLUME) {

        D3DVOLUME_DESC d3dvd;

        hr = g_pd3dDevice->CreateVolumeTexture(uWidth, uHeight, uDepth, 
                                    tt & TTYPE_MIPMAP ? 0 : 1,
                                    dwUsage, fmt, pool, 
                                    (LPDIRECT3DVOLUMETEXTURE8*)&pd3dt);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVolumeTexture"));
            return NULL;
        }

#ifndef DEMO_HACK
        ((LPDIRECT3DVOLUMETEXTURE8)pd3dt)->GetLevelDesc(0, &d3dvd);
        fmtt = d3dvd.Format;
#else
        fmtt = D3DFMT_A8R8G8B8;
#endif
    }
    else if (tt & TTYPE_CUBEMAP) {

        D3DSURFACE_DESC d3dsd;

        hr = g_pd3dDevice->CreateCubeTexture(uWidth > uHeight ? uWidth : uHeight,
                                    tt & TTYPE_MIPMAP ? 0 : 1,
                                    dwUsage, fmt, pool, 
                                    (LPDIRECT3DCUBETEXTURE8*)&pd3dt);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"));
            return NULL;
        }

#ifndef DEMO_HACK
        ((LPDIRECT3DCUBETEXTURE8)pd3dt)->GetLevelDesc(0, &d3dsd);
        fmtt = d3dsd.Format;
#else
        fmtt = D3DFMT_A8R8G8B8;
#endif
    }
    else {

        D3DSURFACE_DESC d3dsd;

        hr = g_pd3dDevice->CreateTexture(uWidth, uHeight,
                                    tt & TTYPE_MIPMAP ? 0 : 1, 
                                    dwUsage, fmt, pool, 
                                    (LPDIRECT3DTEXTURE8*)&pd3dt);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"));
            return NULL;
        }

#ifndef DEMO_HACK
        ((LPDIRECT3DTEXTURE8)pd3dt)->GetLevelDesc(0, &d3dsd);
        fmtt = d3dsd.Format;
#else
        fmtt = D3DFMT_A8R8G8B8;
#endif
    }

    // Add the texture to the texture list
    ptexdNode = (PTEXTUREDESC)MemAlloc(sizeof(TEXTUREDESC));
    if (!ptexdNode) {
        pd3dt->Release();
        return NULL;
    }
    memset(ptexdNode, 0, sizeof(TEXTUREDESC));
    ptexdNode->pd3dt = pd3dt;
    ptexdNode->uWidth = uWidth;
    ptexdNode->uHeight = uHeight;
    ptexdNode->uDepth = uDepth;
    ptexdNode->fmt = fmtt;
    ptexdNode->pool = pool;
    ptexdNode->tt = tt;
    ptexdNode->ptexdNext = g_ptexdList;
    g_ptexdList = ptexdNode;

    return pd3dt;
}

//******************************************************************************
//
// Function:
//
//     CreateTexture
//
// Description:
//
//     Create a texture object and populate it using the image contained in
//     the given resource or file name.
//
// Arguments:
//
//     LPCTSTR szName                       - File or resource name of the 
//                                            bitmap to create the texture from
//
//     PIXELFORMAT pxf                      - Optional pixel format of the 
//                                            texture to create
//
//     MEMORYPOOL mp                        - Location in which to create the
//                                            texture
//
//     TEXTURETYPE tt                       - Type of texture to create (i.e.
//                                            mipmap, cubemap, bumpmap, etc.)
//
//
//     float fAlpha                         - Optional transparency value for
//                                            the texture
//
//     D3DCOLOR cColorKey                   - Optional color key for the
//                                            texture
//
// Return Value:
//
//     A pointer to the created IDirectDrawSurface7 object on 
//     success, NULL on failure.
//
//******************************************************************************
LPDIRECT3DBASETEXTURE8 CreateTexture(LPCTSTR szName, D3DFORMAT fmt, TEXTURETYPE tt,
                                 PALETTEENTRY* pcPalette, float fAlpha,
                                 D3DCOLOR cColorKey, UINT uWidth, UINT uHeight)
{
    LPDIRECT3DBASETEXTURE8  pd3dt = NULL;
    D3DRESOURCETYPE         rt;
    D3DFORMAT               fmtt;
    DWORD                   dwUsage = 0;
    PTEXTUREDESC            ptexdNode;
    LPTSTR                  szImage;
    TCHAR                   szResource[32] = {TEXT("resource ")};
    WORD                    wResourceID;
    HRESULT                 hr;

    if (!szName) {
        // Fail if a NULL name pointer or a resource identifer of zero is given
        return NULL;
    }

    // Verify the current device supports the given format
    // ##REVIEW: Should this be replaced with D3DXCheckMipMapRequirements and
    // D3DXCheckCubeMapRequirements?
    if (tt & TTYPE_DEPTH) {
        dwUsage |= D3DUSAGE_DEPTHSTENCIL;
    }
    else if (tt & TTYPE_TARGET) {
        dwUsage |= D3DUSAGE_RENDERTARGET;
    }

    if (tt & TTYPE_VOLUME) {
        rt = D3DRTYPE_VOLUMETEXTURE;
    }
    else if (tt & TTYPE_CUBEMAP) {
        rt = D3DRTYPE_CUBETEXTURE;
    }
    else {
        rt = D3DRTYPE_TEXTURE;
    }

    hr = g_pd3d->CheckDeviceFormat(g_disdCurrent.padpd->uAdapter, 
                                    g_disdCurrent.pdevd->d3ddt, 
                                    g_disdCurrent.pdm->d3ddm.Format, 
                                    dwUsage, rt, fmt);
//##HACK to get around checkdeviceformats failing on valid formats
/*
    if (FAILED(hr)) {
        if (hr != D3DERR_NOTAVAILABLE) {
            ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
        }
        return NULL;
    }
*/
    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        wResourceID = (WORD)szName;
        wsprintf(szResource + 9, TEXT("%d"), wResourceID);
        szImage = szResource;
    }
    else {
        wResourceID = 0;
        szImage = (LPTSTR)szName;
    }

// ##REVIEW: Don't apply a box filter (use point filter instead) for the mipfilter
// if any fAlpha or color key is given because the mipmap will have to be re-filtered
// later on after translucency information is set...the box filter will be applied at
// that time

    if (tt & TTYPE_VOLUME) {

        return NULL;
    }

    else if (tt & TTYPE_CUBEMAP) {

        LPDIRECT3DCUBETEXTURE8  pd3dcube;
        LPDIRECT3DSURFACE8      pd3ds;
        D3DSURFACE_DESC         d3dsd;
        TCHAR                   szFace[6][MAX_PATH];
        TCHAR                   szTemp[MAX_PATH];
        TCHAR*                  sz;
        UINT                    i;

        // Create the cubemap
        hr = D3DXCreateCubeTexture(g_pd3dDevice, uWidth > uHeight ? uWidth : uHeight,
                            tt & TTYPE_MIPMAP ? 0 : 1, 0, fmt, D3DPOOL_DEFAULT,
                            &pd3dcube);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("D3DXCreateCubeMap"));
            return NULL;
        }

        pd3dcube->GetLevelDesc(0, &d3dsd);
        fmtt = d3dsd.Format;

        // Initialize the image names for the faces
        _tcscpy(szTemp, szName);
        sz = szTemp + _tcslen(szTemp);
        if (*(sz-1) == TEXT('0')) {
            sz--;
        }
        else if (*(sz-5) == TEXT('0')) {
            sz -= 5;
        }
        if (*sz) {
            for (i = 0, sz--; i < 6; i++) {
// ##REVIEW: Will the following work for UNICODE?:
                *sz = TEXT('0') + i;
                _tcscpy(szFace[i], szTemp);
            }
        }
        else {
            for (i = 0; i < 6; i++) {
                _tcscpy(szFace[i], szName);
            }
        }

        // Copy the image into the faces
        for (i = 0; i < 6; i++) {

            hr = pd3dcube->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pd3ds);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetCubeMapSurface"));
                pd3dcube->Release();
                return NULL;
            }

            CHAR szAnsi[1024];
            WideCharToMultiByte(CP_ACP, 0, szFace[i], -1, szAnsi, 1024, NULL, NULL);

            hr = D3DXLoadSurfaceFromFile(pd3ds, pcPalette, NULL,
                        szAnsi, NULL, D3DX_FILTER_LINEAR, 0, NULL);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("D3DXLoadSurfaceFromFile/Resource"));
                pd3ds->Release();
                pd3dcube->Release();
                return NULL;
            }

            pd3ds->Release();
        }

        pd3dt = (LPDIRECT3DCUBETEXTURE8)pd3dcube;
    }

    else if (tt & TTYPE_BUMPMAP) {

        // The given image is a height map.  Use it to generate the bump map
        LPDIRECT3DTEXTURE8  pd3dtHeight;
        D3DSURFACE_DESC     d3dsd;

        // Load the height map
        pd3dtHeight = (LPDIRECT3DTEXTURE8)CreateTexture(szName, D3DFMT_A8R8G8B8, 
                                            TTYPE_TEXTURE, NULL, 1.0f, 
                                            COLORKEY_NONE, uWidth, uHeight);
        if (!pd3dtHeight) {
            return NULL;
        }
        pd3dtHeight->GetLevelDesc(0, &d3dsd);

        // Create the bump map
        pd3dt = CreateTexture(d3dsd.Width, d3dsd.Height, fmt, tt, D3DPOOL_DEFAULT);
        if (!pd3dt) {
            ReleaseTexture(pd3dtHeight);
            return NULL;
        }
        
        hr = CopyTexture(pd3dt, pcPalette, pd3dtHeight, NULL, D3DX_FILTER_LINEAR);

        ReleaseTexture(pd3dtHeight);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        ((LPDIRECT3DTEXTURE8)pd3dt)->GetLevelDesc(0, &d3dsd);
        fmtt = d3dsd.Format;

        return pd3dt;
    }

    else {

        D3DSURFACE_DESC d3dsd;

        CHAR szAnsi[1024];
        WideCharToMultiByte(CP_ACP, 0, szName, -1, szAnsi, 1024, NULL, NULL);

        // Attempt to load the image as a file
        hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, 
                            szAnsi, uWidth, uHeight, tt & TTYPE_MIPMAP ? 0 : 1,
                            0, fmt, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_POINT,
                            0, NULL, pcPalette, (LPDIRECT3DTEXTURE8*)&pd3dt);
        if (FAILED(hr)) {
            TCHAR szError[512];
            D3DXGetErrorString(hr, szError, 512);
            DebugString(TEXT("D3DXCreateMipMapFromFile/ResourceEx failed on %s with %s [0x%X]"), szImage, szError, hr);
            return NULL;
        }

        ((LPDIRECT3DTEXTURE8)pd3dt)->GetLevelDesc(0, &d3dsd);
        fmtt = d3dsd.Format;
    }

    // Add the texture to the texture list
    ptexdNode = (PTEXTUREDESC)MemAlloc(sizeof(TEXTUREDESC));
    if (!ptexdNode) {
        pd3dt->Release();
        return NULL;
    }
    memset(ptexdNode, 0, sizeof(TEXTUREDESC));
    ptexdNode->pd3dt = pd3dt;
    if (wResourceID) {
        _tcscpy(ptexdNode->szName, szResource);
        ptexdNode->wResourceID = wResourceID;
    }
    else {
        _tcscpy(ptexdNode->szName, szName);
        ptexdNode->wResourceID = 0;
    }
    ptexdNode->uWidth = uWidth;
    ptexdNode->uHeight = uHeight;
    ptexdNode->uDepth = 0;
    ptexdNode->fmt = fmtt;
    ptexdNode->pool = D3DPOOL_DEFAULT;
    ptexdNode->tt = tt;
    ptexdNode->fAlpha = fAlpha;
    ptexdNode->cColorKey = cColorKey;
    ptexdNode->ptexdNext = g_ptexdList;
    g_ptexdList = ptexdNode;

    // Set the translucency.  This will also perform a filter of the highest
    // resolution level(s) to all lower resolution levels (if present)
    if (!SetTranslucency(pd3dt, pcPalette, fAlpha, cColorKey)) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    return pd3dt;
}

//******************************************************************************
void ReleaseTexture(LPDIRECT3DBASETEXTURE8 pd3dt) {

    PTEXTUREDESC ptexd, ptexdDel;
#if defined(DEBUG) || defined(_DEBUG)
    TCHAR        szName[MAX_PATH];
    UINT         uRef;
#endif

    if (!pd3dt) {
        return;
    }

    if (g_ptexdList) {

        if (g_ptexdList->pd3dt == pd3dt) {
            ptexd = g_ptexdList->ptexdNext;
#if defined(DEBUG) || defined(_DEBUG)
            _tcscpy(szName, g_ptexdList->szName);
#endif
            MemFree(g_ptexdList);
            g_ptexdList = ptexd;
        }

        else {

            for (ptexd = g_ptexdList; 
                 ptexd->ptexdNext && ptexd->ptexdNext->pd3dt != pd3dt; 
                 ptexd = ptexd->ptexdNext
            );

            if (ptexd->ptexdNext) {
                ptexdDel = ptexd->ptexdNext;
                ptexd->ptexdNext = ptexdDel->ptexdNext;
#if defined(DEBUG) || defined(_DEBUG)
                _tcscpy(szName, ptexdDel->szName);
#endif
                MemFree(ptexdDel);
            }
        }
    }

#if defined(DEBUG) || defined(_DEBUG)
    uRef = pd3dt->Release();
    if (uRef != 0) {
        DebugString(TEXT("WARNING: Texture %s has a ref count of %d on release"), szName, uRef);
    }
#else
    pd3dt->Release();
#endif
}

//******************************************************************************
//
// Function:
//
//     ReleaseTextures
//
// Description:
//
//     Release all textures.
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
void ReleaseTextures() {

    while (g_ptexdList) {
        ReleaseTexture(g_ptexdList->pd3dt);
    }
}

//******************************************************************************
void GetARGBPixelDesc(D3DFORMAT fmt, PARGBPIXELDESC ppixd) {

    if (!ppixd) {
        return;
    }

    memset(ppixd, 0, sizeof(ARGBPIXELDESC));

    switch (fmt) {

        case D3DFMT_P8:
        case D3DFMT_A8R8G8B8:
            ppixd->dwAMask     = 0xFF000000;
            ppixd->dwAShift    = 24;

        case D3DFMT_X8R8G8B8:
            ppixd->dwRMask     = 0x00FF0000;
            ppixd->dwGMask     = 0x0000FF00;
            ppixd->dwBMask     = 0x000000FF;
            ppixd->dwRShift    = 16;
            ppixd->dwGShift    = 8;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 4;
            break;

        case D3DFMT_R8G8B8:
            ppixd->dwRMask     = 0xFF0000;
            ppixd->dwGMask     = 0x00FF00;
            ppixd->dwBMask     = 0x0000FF;
            ppixd->dwRShift    = 16;
            ppixd->dwGShift    = 8;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 3;
            break;

        case D3DFMT_A1R5G5B5:
            ppixd->dwAMask     = 0x8000;
            ppixd->dwAShift    = 15;

        case D3DFMT_X1R5G5B5:
            ppixd->dwRMask     = 0x7C00;
            ppixd->dwGMask     = 0x03E0;
            ppixd->dwBMask     = 0x001F;
            ppixd->dwRShift    = 10;
            ppixd->dwGShift    = 5;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_R5G6B5:
            ppixd->dwRMask     = 0xF800;
            ppixd->dwGMask     = 0x07E0;
            ppixd->dwBMask     = 0x001F;
            ppixd->dwRShift    = 11;
            ppixd->dwGShift    = 5;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_A4R4G4B4:
            ppixd->dwAMask     = 0xF000;
            ppixd->dwAShift    = 12;

        case D3DFMT_X4R4G4B4:
            ppixd->dwRMask     = 0x0F00;
            ppixd->dwGMask     = 0x00F0;
            ppixd->dwBMask     = 0x000F;
            ppixd->dwRShift    = 8;
            ppixd->dwGShift    = 4;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;
    }
}

//******************************************************************************
static PTEXTUREDESC GetTextureDesc(LPDIRECT3DBASETEXTURE8 pd3dt) {

    PTEXTUREDESC ptexd;

    for (ptexd = g_ptexdList; 
         ptexd && ptexd->pd3dt != pd3dt; 
         ptexd = ptexd->ptexdNext
    );

    return ptexd;
}

/*
//******************************************************************************
BOOL PopulateTexture(LPDIRECTDRAWSURFACE7 pdds, LPCTSTR szName, float fAlpha, 
                     D3DCOLOR cColorKey)
{
    DDSURFACEDESC2  ddsd;
    LPTSTR          szImage;
    TCHAR           szResource[32] = {TEXT("resource ")};
    WORD            wResourceID;
    HBITMAP         hBitmap = NULL;
    BITMAP          bitmap;
    HDC             hdcSrc = NULL,
                    hdcDst = NULL;
    DWORD           dwPitch;
    UINT            i, j;
    HRESULT         hr;

    if (!szName) {
        // Fail if a NULL name pointer or a resource identifer of zero is given
        return NULL;
    }

    // Get the surface description
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    hr = pdds->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetSurfaceDesc"))) {
        return FALSE;
    }

    // If the high order word of the name is zero, the name is a resource
    // identifier rather than a pointer to the name string
    if (!(BOOL)((DWORD)szName >> 16)) {
        wResourceID = (WORD)szName;
        wsprintf(szResource + 9, TEXT("%d"), wResourceID);
        szImage = szResource;
    }
    else {
        wResourceID = 0;
        szImage = (LPTSTR)szName;
    }

    // Attempt to load the bitmap as a resource
    hBitmap = LoadBitmap(g_hInstance, szName);
    if (!hBitmap && !wResourceID) {
        // Attempt to load the bitmap as a file
        hBitmap = (HBITMAP)LoadImage(g_hInstance, szName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    if (!hBitmap) {
        DebugString(TEXT("LoadBitmap/LoadImage failed on %s"), szImage);
        return NULL;
    }

    // Get the bitmap information
    if (!GetObject(hBitmap, sizeof(BITMAP), &bitmap)) {
        DebugString(TEXT("GetObject failed on %s"), szImage);
        DeleteObject(hBitmap);
        return FALSE;
    }

    // If the texture is palettized, initialize the palette
    if (ddsd.ddpfPixelFormat.dwRGBBitCount < 16) {

        LPDIRECTDRAWPALETTE pddp;
        RGBQUAD             prgbq[256];
        UINT                uNumEntries;
        PALETTEENTRY        ppe[256];

        hr = pdds->GetPalette(&pddp);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetPalette"))) {
            DeleteObject(hBitmap);
            return FALSE;
        }

        // Get the color table from the bitmap
        hdcSrc = CreateCompatibleDC(NULL);
        if (!hdcSrc) {
            DebugString(TEXT("CreateCompatibleDC failed for %s"), szImage);
            pddp->Release();
            DeleteObject(hBitmap);
            return FALSE;
        }

        SelectObject(hdcSrc, hBitmap);

        uNumEntries = GetDIBColorTable(hdcSrc, 0, 
                                        (0x1 << bitmap.bmBitsPixel), prgbq);

        DeleteDC(hdcSrc);

        memset(&ppe, 0, sizeof(PALETTEENTRY) * 256);

        if (uNumEntries) {

            // Convert the RGBQUAD entries to PALETTEENTRY structures
            for (i = 0; i < uNumEntries; i++) {
                ppe[i].peRed = prgbq[i].rgbRed;
                ppe[i].peGreen = prgbq[i].rgbGreen;
                ppe[i].peBlue = prgbq[i].rgbBlue;
            }
        }
        else {

            // Use the existing entries
            hr = pddp->GetEntries(0, 0, (UINT)(0x1 << ddsd.ddpfPixelFormat.dwRGBBitCount), ppe);
            if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetEntries"))) {
                if (!InitPalette(ppe, (0x1 << ddsd.ddpfPixelFormat.dwRGBBitCount))) {
                    pddp->Release();
                    DeleteObject(hBitmap);
                    return FALSE;
                }
            }
        }

        // Initialize alpha in the palette
        if (fAlpha != 1.0f && (g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) {
            for (i = 0; i < (UINT)(0x1 << ddsd.ddpfPixelFormat.dwRGBBitCount); i++) {
                ppe[i].peFlags = (BYTE)(fAlpha * 255.0f);
            }
        }

        // Set the new palette entries
        hr = pddp->SetEntries(0, 0, (UINT)(0x1 << ddsd.ddpfPixelFormat.dwRGBBitCount), ppe);

        // Release a reference on the palette
        pddp->Release();

        if (ResultFailed(hr, TEXT("IDirectDrawPalette::SetEntries"))) {
            DeleteObject(hBitmap);
            return FALSE;
        }
    }

    // Create an offscreen DC for the bitmap
    hdcSrc = CreateCompatibleDC(NULL);
    if (!hdcSrc) {
        DebugString(TEXT("CreateCompatibleDC failed for %s"), szImage);
        DeleteObject(hBitmap);
        return FALSE;
    }

    SelectObject(hdcSrc, hBitmap);

    // Get a DC on the texture surface
    hr = pdds->GetDC(&hdcDst);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetDC"))) {
        DeleteDC(hdcSrc);
        DeleteObject(hBitmap);
        return FALSE;
    }

    // Copy the bitmap into the texture
    if (!BitBlt(hdcDst, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcSrc, 0, 
                0, SRCCOPY)) 
    {
        DebugString(TEXT("BitBlt failed for %s"), szImage);
        DeleteDC(hdcSrc);
        DeleteObject(hBitmap);
        return FALSE;
    }

    // Release the DCs and bitmap
    pdds->ReleaseDC(hdcDst);
    DeleteDC(hdcSrc);
    DeleteObject(hBitmap);

    // Set the translucency information if the surface has alpha bits
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {

        // Lock the surface
        memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        hr = pdds->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR |
                                    DDLOCK_WAIT, NULL);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Lock"))) {
            return FALSE;
        }

        switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {

            case 16: {

                LPWORD pwPixel = (LPWORD)ddsd.lpSurface;
                dwPitch = ddsd.lPitch / (sizeof(WORD) / sizeof(BYTE));

                // Set all alpha bits of the texture to 1
                for (i = 0; i < ddsd.dwHeight; i++) {

                    for (j = 0; j < ddsd.dwWidth; j++) {

                        *(pwPixel + j) |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
                    }

                    pwPixel += dwPitch;
                }
                
                break;
            }

            case 32: {

                LPDWORD pdwPixel = (LPDWORD)ddsd.lpSurface;
                dwPitch = ddsd.lPitch / (sizeof(DWORD) / sizeof(BYTE));

                // Set all alpha bits of the texture to 1
                for (i = 0; i < ddsd.dwHeight; i++) {

                    for (j = 0; j < ddsd.dwWidth; j++) {

                        *(pdwPixel + j) |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
                    }

                    pdwPixel += dwPitch;
                }

                break;
            }
        }

        // Unlock the surface and release it
        hr = pdds->Unlock(NULL);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Unlock"))) {
            return FALSE;
        }

        // Set the translucency
        SetTranslucency(pdds, fAlpha);
    }

    // Add colorkey information if necessary
    if (RGBA_GETALPHA(cColorKey) != 0xFF) {
        SetColorKey(pdds, cColorKey);
    }

    // Reduce the resolution for the lower levels of the mipmap
    // using the bitmap information from the highest level
    if (ddsd.ddsCaps.dwCaps & DDSCAPS_MIPMAP) {
        if (!ReduceResolution(pdds)) {
            return FALSE;
        }
    }

    return TRUE;
}
*/

//******************************************************************************
BOOL SetTranslucency(LPDIRECT3DSURFACE8 pd3ds, PALETTEENTRY* pcPalette, 
                        float fAlpha, D3DCOLOR cColorKey) 
{
    D3DSURFACE_DESC     d3dsd;
    D3DLOCKED_RECT      d3dlr;
    ARGBPIXELDESC       pixd;
    DWORD               dwPitch;
    DWORD               dwColorKey;
    UINT                i, j;
    HRESULT             hr;

    if (!(cColorKey == COLORKEY_NONE || cColorKey == COLORKEY_PIXEL1)) {
        // Get the surface-specific colorkey value (the representation of
        // the pixel in surface memory) from the red, green, and blue
        // components of the given D3DCOLOR
        dwColorKey = ColorToPixel(pd3ds, pcPalette, cColorKey & 0xFFFFFF);
    }

    // Get the format of the mipmap
    pd3ds->GetDesc(&d3dsd);

    // Lock the texture
    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
        return FALSE;
    }

    // If the texture is palettized, set the translucency in the palette entries
    if (d3dsd.Format == D3DFMT_P8) {

        UINT i;

        if (!pcPalette || !(g_disdCurrent.pdevd->d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) {
            return FALSE;
        }

        for (i = 0; i < 256; i++) {
            pcPalette[i].peFlags = (BYTE)(fAlpha * 255.0f);
        }

        if (cColorKey == COLORKEY_PIXEL1) {
            dwColorKey = *(LPBYTE)d3dlr.pBits;
        }
        if (cColorKey != COLORKEY_NONE) {
            pcPalette[dwColorKey].peFlags = 0;
        }

        return TRUE;
    }

    // Get the pixel format of the texture
    GetARGBPixelDesc(d3dsd.Format, &pixd);

    if (cColorKey == COLORKEY_NONE) {
        dwColorKey = COLORKEY_NONE;
    }

    // Set the translucency
    switch (d3dsd.Format) {

        case D3DFMT_A8R8G8B8:

            LPDWORD pdwPixel;
            DWORD   dwAlpha;

            dwAlpha = (DWORD)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask;
            pdwPixel = (LPDWORD)d3dlr.pBits;

            dwPitch = d3dlr.Pitch / pixd.uStride;

            if (cColorKey == COLORKEY_PIXEL1) {
                dwColorKey = *pdwPixel;
            }

            for (i = 0; i < d3dsd.Height; i++) {

                for (j = 0; j < d3dsd.Width; j++) {

                    if ((*(pdwPixel + j) & ~pixd.dwAMask) == dwColorKey) {
                        *(pdwPixel + j) &= ~pixd.dwAMask;
                    }
                    else {
                        *(pdwPixel + j) = dwAlpha | (~pixd.dwAMask & *(pdwPixel + j));
                    }
                }

                pdwPixel += dwPitch;
            }

            break;

        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4: {

            LPWORD pwPixel;
            WORD   wAlpha;

            wAlpha = (WORD) ((UINT)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask);
            pwPixel = (LPWORD)d3dlr.pBits;

            dwPitch = d3dlr.Pitch / pixd.uStride;

            if (cColorKey == COLORKEY_PIXEL1) {
                dwColorKey = *pwPixel;
            }

            for (i = 0; i < d3dsd.Height; i++) {

                for (j = 0; j < d3dsd.Width; j++) {

                    if ((*(pwPixel + j) & ~pixd.dwAMask) == dwColorKey) {
                        *(pwPixel + j) &= ~pixd.dwAMask;
                    }
                    else {
                        *(pwPixel + j) = wAlpha | (~(WORD)pixd.dwAMask & *(pwPixel + j));
                    }
                }

                pwPixel += dwPitch;
            }

            break;
        }
    }

    // Unlock the surface
    hr = pd3ds->UnlockRect();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"));
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SetTranslucency
//
// Description:
//
//     Set the alpha channel information on a texture.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pd3dt           - Texture to make translucent
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetTranslucency(LPDIRECT3DBASETEXTURE8 pd3dt, PALETTEENTRY* pcPalette, 
                        float fAlpha, D3DCOLOR cColorKey) 
{
    PTEXTUREDESC    ptexd;
    HRESULT         hr;

    // Parameter validation
    if (!pd3dt) {
        return FALSE;
    }

    // Fail if the current device does not support translucency
    if (!(g_disdCurrent.pdevd->d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHA)) {
        return FALSE;
    }

    // ##TODO: Get the resource type of the surface and then set the translucency accordingly
    // for now assume mipmap

    ptexd = GetTextureDesc(pd3dt);
    if (!ptexd) {
        return FALSE;
    }

    if (ptexd->tt & TTYPE_VOLUME) {

    }
    else if (ptexd->tt & TTYPE_CUBEMAP) {

        LPDIRECT3DSURFACE8      pd3ds;
        LPDIRECT3DCUBETEXTURE8  pd3dcube = (LPDIRECT3DCUBETEXTURE8)pd3dt;
        UINT                    i;

        for (i = 0; i < 6; i++) {

            hr = pd3dcube->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pd3ds);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetCubeMapSurface"));
                return FALSE;
            }

            if (!SetTranslucency(pd3ds, pcPalette, fAlpha, cColorKey)) {
                pd3ds->Release();
                return FALSE;
            }

            pd3ds->Release();
        }

        if (ptexd->tt & TTYPE_MIPMAP) {

            hr = D3DXFilterCubeTexture(pd3dcube, pcPalette, D3DX_DEFAULT, D3DX_FILTER_BOX);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("D3DXFilterCubeMap"));
                return FALSE;
            }
        }
    }
    else {

        LPDIRECT3DSURFACE8 pd3ds;
        LPDIRECT3DTEXTURE8 pd3dmip = (LPDIRECT3DTEXTURE8)pd3dt;

        hr = pd3dmip->GetSurfaceLevel(0, &pd3ds);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DMipMap8::GetSurfaceLevel"));
            return FALSE;
        }

        if (!SetTranslucency(pd3ds, pcPalette, fAlpha, cColorKey)) {
            pd3ds->Release();
            return FALSE;
        }

        pd3ds->Release();

        if (ptexd->tt & TTYPE_MIPMAP) {

            hr = D3DXFilterTexture(pd3dmip, pcPalette, D3DX_DEFAULT, D3DX_FILTER_BOX);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("D3DXFilterMipMap"));
                return FALSE;
            }
        }
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ColorToPixel
//
// Description:
//
//     Calculate the surface-specific colorkey value (the representation of
//     the pixel in surface memory) from the red, green, and blue
//     components of the given D3DCOLOR.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pdds       - Surface on which to calculate the 
//                                       pixel value
//
//     D3DCOLOR c                      - Color of the pixel
//
// Return Value:
//
//     The pixel representation in surface memory of the given color value on
//     success, zero on failure.
//
//******************************************************************************
DWORD ColorToPixel(LPDIRECT3DSURFACE8 pd3ds, PALETTEENTRY* pcPalette, D3DCOLOR c) {

    D3DSURFACE_DESC     d3dsd;
    DWORD               dwPixel;
    BYTE                r, g, b, a;

    // Parameter validation
    if (!pd3ds) {
        return 0;
    }

    r = (BYTE)RGBA_GETRED(c);
    g = (BYTE)RGBA_GETGREEN(c);
    b = (BYTE)RGBA_GETBLUE(c);
    a = (BYTE)RGBA_GETALPHA(c);

    // Get the format of the mipmap
    pd3ds->GetDesc(&d3dsd);

    // If the surface is palettized, select the nearest palette entry
    // to the given color
    if (d3dsd.Format == D3DFMT_P8) {

        UINT                i;
        int                 dr, dg, db, d, dLast;

        if (!pcPalette) {
            return 0;
        }

        dLast = INT_MAX;

        // Find the entry in the palette that most closely matches the
        // given pixel color
        for (i = 0; i < 256; i++) {

            dr = (int)pcPalette[i].peRed - r;
            dg = (int)pcPalette[i].peGreen - g;
            db = (int)pcPalette[i].peBlue - b;

            d = dr * dr + dg * dg + db * db;

            if (d < dLast) {
                dwPixel = i;
                dLast = d;
                if (d == 0) {
                    break;
                }
            }
        }

//        DebugString(TEXT("Color key set to index %d - r: %d, g: %d, b: %d"), 
//                match, ppe[match].peRed, ppe[match].peGreen, ppe[match].peBlue);
    }

    else {

        ARGBPIXELDESC       pixd;

        // Get the pixel format of the texture
        GetARGBPixelDesc(d3dsd.Format, &pixd);

        dwPixel = ((BYTE)((float)(pixd.dwAMask >> pixd.dwAShift) * ((float)a / 255.0f) + 0.5f) << pixd.dwAShift) |
                  ((BYTE)((float)(pixd.dwRMask >> pixd.dwRShift) * ((float)r / 255.0f) + 0.5f) << pixd.dwRShift) |
                  ((BYTE)((float)(pixd.dwGMask >> pixd.dwGShift) * ((float)g / 255.0f) + 0.5f) << pixd.dwGShift) |
                  ((BYTE)((float)(pixd.dwBMask >> pixd.dwBShift) * ((float)b / 255.0f) + 0.5f) << pixd.dwBShift);
    }

    return dwPixel;
}

//******************************************************************************
HRESULT CopyTexture(LPDIRECT3DBASETEXTURE8 pd3dtDst, PALETTEENTRY* pcPalDst, 
                    LPDIRECT3DBASETEXTURE8 pd3dtSrc, PALETTEENTRY* pcPalSrc, 
                    DWORD dwFilter) 
{
    PTEXTUREDESC        ptexdDst, ptexdSrc;
    LPDIRECT3DSURFACE8  pd3dsDst, pd3dsSrc;
    UINT                uLevelsDst, uLevelsSrc, i, j, k;
    HRESULT             hr;

    ptexdDst = GetTextureDesc(pd3dtDst);
    ptexdSrc = GetTextureDesc(pd3dtSrc);

    if (!ptexdDst || !ptexdSrc) {
        return E_INVALIDARG;
    }

    if ((((ptexdDst->tt & TTYPE_RMASK) == TTYPE_TEXTURE) &&
         ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_TEXTURE)) ||
        (((ptexdDst->tt & TTYPE_RMASK) == TTYPE_BUMPMAP) &&
         ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_BUMPMAP)))
    {
        uLevelsDst = pd3dtDst->GetLevelCount();
        uLevelsSrc = pd3dtSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;
            hr = ((LPDIRECT3DTEXTURE8)pd3dtDst)->GetSurfaceLevel(i, &pd3dsDst);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                return hr;
            }
            hr = ((LPDIRECT3DTEXTURE8)pd3dtSrc)->GetSurfaceLevel(j, &pd3dsSrc);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dsDst->Release();
                return hr;
            }
            hr = D3DXLoadSurfaceFromSurface(pd3dsDst, pcPalDst, NULL,
                                            pd3dsSrc, pcPalSrc, NULL,
                                            dwFilter, 0);

            pd3dsSrc->Release();
            pd3dsDst->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                return hr;
            }
        }

        return hr;
    }

    if ((((ptexdDst->tt & TTYPE_RMASK) == TTYPE_CUBEMAP) &&
         ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_TEXTURE)) ||
        (((ptexdDst->tt & TTYPE_RMASK) == TTYPE_CUBEMAP) &&
         ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_CUBEMAP)))
    {
        uLevelsDst = pd3dtDst->GetLevelCount();
        uLevelsSrc = pd3dtSrc->GetLevelCount();

        for (k = 0; k < 6; k++) {

            for (i = 0, j = 0; i < uLevelsDst; i++) {

                j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;
                hr = ((LPDIRECT3DCUBETEXTURE8)pd3dtDst)->GetCubeMapSurface((D3DCUBEMAP_FACES)k, i, &pd3dsDst);
                if (ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetSurfaceLevel"))) {
                    return hr;
                }
                if (ptexdSrc->tt & TTYPE_CUBEMAP) {
                    hr = ((LPDIRECT3DCUBETEXTURE8)pd3dtSrc)->GetCubeMapSurface((D3DCUBEMAP_FACES)k, j, &pd3dsSrc);
                }
                else {
                    hr = ((LPDIRECT3DTEXTURE8)pd3dtSrc)->GetSurfaceLevel(j, &pd3dsSrc);
                }
                if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                    pd3dsDst->Release();
                    return hr;
                }
                hr = D3DXLoadSurfaceFromSurface(pd3dsDst, pcPalDst, NULL,
                                                pd3dsSrc, pcPalSrc, NULL,
                                                dwFilter, 0);

                pd3dsSrc->Release();
                pd3dsDst->Release();

                if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    return hr;
                }
            }
        }

        return hr;
    }

    if (((ptexdDst->tt & TTYPE_RMASK) == TTYPE_BUMPMAP) &&
        ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_TEXTURE))
    {
        LPDIRECT3DSURFACE8  pd3dsH, pd3dsB, pd3ds;
        D3DSURFACE_DESC     d3dsd;
        D3DLOCKED_RECT      d3dlr;
        LPDWORD             pdwPixel;
        LPWORD              pwBump;
        float               fOrder;
        float               *pfHeightMap, *pfHeight;
        D3DXVECTOR3         v1, v2, n;
        UINT                x, y;

        uLevelsDst = pd3dtDst->GetLevelCount();
        uLevelsSrc = pd3dtSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;

            ((LPDIRECT3DTEXTURE8)pd3dtSrc)->GetLevelDesc(j, &d3dsd);

            // Create a 32 bit height map
            if (ptexdSrc->fmt == D3DFMT_X8R8G8B8 ||
                ptexdSrc->fmt == D3DFMT_A8R8G8B8)
            {
                hr = ((LPDIRECT3DTEXTURE8)pd3dtSrc)->GetSurfaceLevel(j, &pd3dsH);
                if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                    return hr;
                }
            }
            else {

                hr = g_pd3dDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_A8R8G8B8, &pd3dsH);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                    return hr;
                }
            
                hr = ((LPDIRECT3DTEXTURE8)pd3dtSrc)->GetSurfaceLevel(j, &pd3ds);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetSurfaceLevel"))) {
                    pd3dsH->Release();
                    return hr;
                }

                hr = D3DXLoadSurfaceFromSurface(pd3dsH, NULL, NULL, pd3ds, pcPalSrc, NULL, dwFilter, 0);
                pd3ds->Release();
                if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    pd3dsH->Release();
                    return hr;
                }
            }

            // Create a 16 bit bump map
            hr = g_pd3dDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_V8U8, &pd3dsB);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                pd3dsH->Release();
                return hr;
            }

            // Convert the pixel intensities in the source image into height values
            // ranging from 0.0 to 1.0
            pfHeightMap = (float*)MemAlloc(d3dsd.Width * d3dsd.Height * sizeof(float));
            if (!pfHeightMap) {
                DebugString(TEXT("Memory allocation failed for height map"));
                pd3dsB->Release();
                pd3dsH->Release();
                return E_OUTOFMEMORY;
            }

            hr = pd3dsH->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                pd3dsH->Release();
                return hr;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            for (y = 0; y < d3dsd.Height; y++) {

                for (x = 0; x < d3dsd.Width; x++) {

                    pfHeightMap[y * d3dsd.Width + x] = 
                           (float)(((pdwPixel[x] >> 16) & 0xFF) +
                                   ((pdwPixel[x] >> 8)  & 0xFF) +
                                   ((pdwPixel[x])       & 0xFF)) / 3.0f / 255.0f;
                }

                pdwPixel += d3dlr.Pitch / 4;
            }

            pd3dsH->UnlockRect();
            pd3dsH->Release();
       
            // Calculate bump map offsets from the height map information
            hr = pd3dsB->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                return hr;
            }

            pwBump = (LPWORD)d3dlr.pBits;
            v1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            v2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

            for (y = 0; y < d3dsd.Height; y++) {

                for (x = 0; x < d3dsd.Width; x++) {

                    pfHeight = &pfHeightMap[y * d3dsd.Width + x];
                    fOrder = 1.0f;
                    if (y == d3dsd.Height - 1) {
                        v2.y = *(pfHeight - d3dsd.Width) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v2.y = pfHeight[d3dsd.Width] - pfHeight[0];
                    }
                    if (x == d3dsd.Width - 1) {
                        v1.y = *(pfHeight - 1) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v1.y = pfHeight[1] - pfHeight[0];
                    }
                    v1.x = (float)sqrt(1.0f - v1.y * v1.y);
                    v2.z = -(float)sqrt(1.0f - v2.y * v2.y);
                    if (fOrder > 0.0f) {
                        D3DXVec3Cross(&n, &v1, &v2);
                    }
                    else {
                        D3DXVec3Cross(&n, &v2, &v1);
                    }
                    D3DXVec3Normalize(&n, &n);
                    pwBump[x] = (int)(-n.z * 64.0f) << 8 |
                                (int)(n.x * 64.0f);
                }

                pwBump += d3dlr.Pitch / 2;
            }

            MemFree(pfHeightMap);

            hr = pd3dsB->UnlockRect();
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"));
                pd3dsB->Release();
                return hr;
            }

            // Copy the bump map to the destination bump map
            hr = ((LPDIRECT3DTEXTURE8)pd3dtDst)->GetSurfaceLevel(i, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dsB->Release();
                return hr;
            }

            hr = D3DXLoadSurfaceFromSurface(pd3ds, pcPalDst, NULL, pd3dsB, NULL, NULL, dwFilter, 0);

            pd3ds->Release();
            pd3dsB->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                return hr;
            }
        }

        return hr;
    }

    if (((ptexdDst->tt & TTYPE_RMASK) == TTYPE_VOLUME) &&
        ((ptexdSrc->tt & TTYPE_RMASK) == TTYPE_VOLUME))
    {

    }

    return E_NOTIMPL;
}

//******************************************************************************
LPDIRECT3DTEXTURE8 CreateDiffuseMap(UINT uWidth, UINT uHeight, D3DCOLOR c,
                                    D3DFORMAT fmt, BOOL bMipmap, 
                                    PALETTEENTRY* pcPalette, D3DPOOL pool)
{
    LPDIRECT3DTEXTURE8  pd3dt;
    D3DSURFACE_DESC     d3dsd;
    D3DLOCKED_RECT      d3dlr;
    D3DFORMAT           fmtt;
    DWORD               dwDepth;
    ARGBPIXELDESC       pixd;
    D3DXVECTOR3         vNormal, vLight;
    float               fIntensity, fx, fy, fSin, fMag, fMP, fRed, fGreen, fBlue, fU, fV;
    UINT                uLevels;
    UINT                i, j, k;
    HRESULT             hr;

    pd3dt = (LPDIRECT3DTEXTURE8)CreateTexture(uWidth, uHeight, fmt, bMipmap ? TTYPE_MIPMAP : TTYPE_TEXTURE, pool);
    if (!pd3dt) {
        return NULL;
    }

    ((LPDIRECT3DTEXTURE8)pd3dt)->GetLevelDesc(0, &d3dsd);
    fmtt = d3dsd.Format;

    dwDepth = FormatToBitDepth(fmtt);
    if (!(dwDepth == 16 || dwDepth == 32)) {

        LPDIRECT3DTEXTURE8  pd3dtMap;
        D3DFORMAT           fmtMap[] = {
                                D3DFMT_X8R8G8B8,
                                D3DFMT_A8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A4R4G4B4
                            };

        for (i = 0; i < 6; i++) {
            pd3dtMap = (LPDIRECT3DTEXTURE8)CreateDiffuseMap(uWidth, uHeight, 
                        c, fmtMap[i], bMipmap, NULL, D3DPOOL_SYSTEMMEM);
            if (pd3dtMap) {
                hr = CopyTexture(pd3dt, pcPalette, pd3dtMap, NULL, 
                                    D3DX_FILTER_LINEAR);
                ReleaseTexture(pd3dtMap);
                if (!ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    return pd3dt;
                }
            }
        }

        ReleaseTexture(pd3dt);
        return NULL;
    }

    vLight = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    fRed = (float)RGBA_GETRED(c);
    fGreen = (float)RGBA_GETGREEN(c);
    fBlue = (float)RGBA_GETBLUE(c);

    uLevels = pd3dt->GetLevelCount();

    for (i = 0; i < uLevels; i++) {

        pd3dt->GetLevelDesc(i, &d3dsd);

        // Get the pixel format of the texture
        GetARGBPixelDesc(d3dsd.Format, &pixd);

        hr = pd3dt->LockRect(i, &d3dlr, NULL, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
            ReleaseTexture(pd3dt);
            return NULL;
        }

        switch (d3dsd.Format) {

            case D3DFMT_X8R8G8B8:
            case D3DFMT_A8R8G8B8: {

                LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;

                for (j = 0, fV = 0.0f; j < d3dsd.Height; j++, fV += (1.0f / (float)(d3dsd.Height-1))) {

                    for (k = 0, fU = 0.0f; k < d3dsd.Width; k++, fU += (1.0f / (float)(d3dsd.Width-1))) {

                        fx = (fU - 0.5f) * 2.0f;
                        fy = (fV - 0.5f) * 2.0f;
                        fMag = (float)sqrt(fx * fx + fy * fy);
                        if (fMag > 1.0f) {
                            vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                        }
                        else {
                            fMP = fMag * M_PI;
                            fSin = (float)sin(fMP);
                            vNormal.y = (float)cos(fMP);
                            fx /= fMag; // Normalize
                            fy /= fMag;
                            vNormal.x = fx * fSin;
                            vNormal.z = fy * fSin;
                        }

                        fIntensity = D3DXVec3Dot(&vLight, &vNormal);
                        if (fIntensity < 0.0f) {
                            fIntensity = 0.0f;
                        }
                        pdwPixel[k] = 0xFF                        << pixd.dwAShift |
                                      (BYTE)(fIntensity * fRed)   << pixd.dwRShift |
                                      (BYTE)(fIntensity * fGreen) << pixd.dwGShift |
                                      (BYTE)(fIntensity * fBlue)  << pixd.dwBShift;
                    }

                    pdwPixel += (d3dlr.Pitch / pixd.uStride);
                }

                break;
            }

            case D3DFMT_R5G6B5:
            case D3DFMT_X1R5G5B5:
            case D3DFMT_A1R5G5B5:
            case D3DFMT_A4R4G4B4: {

                LPWORD pwPixel = (LPWORD)d3dlr.pBits;

                fRed = fRed / 255.0f * (float)(pixd.dwRMask >> pixd.dwRShift);
                fGreen = fGreen / 255.0f * (float)(pixd.dwGMask >> pixd.dwGShift);
                fBlue = fBlue / 255.0f * (float)(pixd.dwBMask >> pixd.dwBShift);

                for (j = 0, fV = 0.0f; j < d3dsd.Height; j++, fV += (1.0f / (float)(d3dsd.Height-1))) {

                    for (k = 0, fU = 0.0f; k < d3dsd.Width; k++, fU += (1.0f / (float)(d3dsd.Width-1))) {

                        fx = (fU - 0.5f) * 2.0f;
                        fy = (fV - 0.5f) * 2.0f;
                        fMag = (float)sqrt(fx * fx + fy * fy);
                        if (fMag > 1.0f) {
                            vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
                        }
                        else {
                            fMP = fMag * M_PI;
                            fSin = (float)sin(fMP);
                            vNormal.y = (float)cos(fMP);
                            fx /= fMag; // Normalize
                            fy /= fMag;
                            vNormal.x = fx * fSin;
                            vNormal.z = fy * fSin;
                        }

                        fIntensity = D3DXVec3Dot(&vLight, &vNormal);
                        if (fIntensity < 0.0f) {
                            fIntensity = 0.0f;
                        }

                        pwPixel[k] = (WORD)(pixd.dwAMask |
                                      ((UINT)(fIntensity * fRed)   << pixd.dwRShift) & pixd.dwRMask |
                                      ((UINT)(fIntensity * fGreen) << pixd.dwGShift) & pixd.dwGMask |
                                      ((UINT)(fIntensity * fBlue)  << pixd.dwBShift) & pixd.dwBMask);
                    }

                    pwPixel += (d3dlr.Pitch / pixd.uStride);
                }

                break;
            }
        }

        hr = pd3dt->UnlockRect(i);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"));
            ReleaseTexture(pd3dt);
            return NULL;
        }
    }

    return pd3dt;
}

//******************************************************************************
BOOL SetDiffuseMapCoords(D3DVECTOR* pvNormal, UINT uNStride, PTCOORD ptcTCoord, 
                        UINT uTStride, UINT uVertices, D3DMATRIX* pmWorld, 
                        D3DVECTOR* pvLightDir, float fTheta)
{
    D3DXVECTOR3     vL, vLDir, vNormal, vCross;
    LPD3DXVECTOR3   pvN;
    LPBYTE          pNormal, pTexCoord;
    float           fSin, fCos, fSin2, fCos2;
    float           fNU, fNV, fMagUV, fMagXZ;
    UINT            i;

    if (!pvNormal || !ptcTCoord || !pmWorld || !pvLightDir) {
        return FALSE;
    }

    fTheta = M_PI - fTheta;
    fCos2 = (float)cos(fTheta);
    fSin2 = (float)sin(fTheta);

    // Normalize the light direction vector
    D3DXVec3Normalize(&vLDir, (LPD3DXVECTOR3)pvLightDir);

    // Transform the light direction vector into model space
    // (multiply the vector by the transposed rotational component of the
    // world matrix) and negate it
    vL.x = -(vLDir.x * pmWorld->_11 + vLDir.y * pmWorld->_12 + vLDir.z * pmWorld->_13);
    vL.y = -(vLDir.x * pmWorld->_21 + vLDir.y * pmWorld->_22 + vLDir.z * pmWorld->_23);
    vL.z = -(vLDir.x * pmWorld->_31 + vLDir.y * pmWorld->_32 + vLDir.z * pmWorld->_33);

    // Generate texture coordinates for the vertices
    pNormal = (LPBYTE)pvNormal;
    pTexCoord = (LPBYTE)ptcTCoord;

    for (i = 0; i < uVertices; i++) {

        pvN = (LPD3DXVECTOR3)pNormal;
        ptcTCoord = (PTCOORD)pTexCoord;

        fCos = D3DXVec3Dot(&vL, pvN);

        D3DXVec3Cross(&vCross, &vL, pvN);
        fSin = D3DXVec3Length(&vCross);

        vNormal.x = -fSin * fCos2;
        vNormal.y = fCos;
        vNormal.z = -fSin * -fSin2;

        // Find the normal vector offsets in the light map
        fMagUV = (float)acos(vNormal.y) / M_PI;

        if (fMagUV < 0.0001f) {
            fNU = 0.5f;
            fNV = 0.5f;
        }
        else if (fMagUV > 0.9999) {
            fNU = 0.0f;
            fNV = 0.5f;
        }
        else {
            fMagXZ = (float)sqrt(vNormal.x * vNormal.x + vNormal.z * vNormal.z);
            fNU = vNormal.x / fMagXZ;
            fNV = vNormal.z / fMagXZ;
           
            fNU *= fMagUV;
            fNV *= fMagUV;

            fNU = 0.5f + (fNU / 2.0f);
            fNV = 0.5f - (fNV / 2.0f);
        }

        ptcTCoord->u = fNU;
        ptcTCoord->v = fNV;

        pNormal += uNStride;
        pTexCoord += uTStride;
    }

    return TRUE;
}

/*
//******************************************************************************
//
// Function:
//
//     SetColorKey
//
// Description:
//
//     Add source colorkey information to a texture.  This can be done in one
//     of two ways: using the SetColorKey method of DirectDraw or using
//     alpha channel information in each pixel to simulate the colorkey.
//     If the driver supports source colorkeying (using either the hardware or
//     under emulation), set the colorkey using the SetColorKey method.
//     Otherwise, if the texture contains alpha information set the alpha bits 
//     to zero for the colorkeyed pixels.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pd3dt             - Texture on which to set the colorkey
//
//     D3DCOLOR cColorKey                   - Color value of the pixels to
//                                            make transparent
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetColorKey(LPDIRECTDRAWSURFACE7 pd3dt, D3DCOLOR cColorKey) {

    DDSURFACEDESC2       ddsd;
    DWORD                dwColorKey;
    HRESULT              hr;

    // Parameter validation
    if (!pd3dt) {
        return FALSE;
    }

    // Get the surface-specific colorkey value (the representation of
    // the pixel in surface memory) from the red, green, and blue
    // components of the given D3DCOLOR
    dwColorKey = ColorToPixel(pd3dt, RGB_MAKE(RGBA_GETRED(cColorKey),
                                              RGBA_GETGREEN(cColorKey), 
                                              RGBA_GETBLUE(cColorKey)));

    // Get the description of the texture surface
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    hr = pd3dt->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetSurfaceDesc"))) {
        return FALSE;
    }

    // If the texture is palettized, attempt to use alpha in the palette
    // to simulate the colorkeying
    if ((ddsd.ddpfPixelFormat.dwRGBBitCount < 16) &&
        (g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) 
    {
        LPDIRECTDRAWPALETTE pddp;
        PALETTEENTRY        pe;
        DWORD               dwCaps;

        hr = pd3dt->GetPalette(&pddp);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetPalette"))) {
            return FALSE;
        }

        hr = pddp->GetCaps(&dwCaps);
        if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetCaps"))) {
            pddp->Release();
            return FALSE;
        }

        if (dwCaps & DDPCAPS_ALPHA) {

            hr = pddp->GetEntries(0, dwColorKey, 1, &pe);
            if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetEntries"))) {
                pddp->Release();
                return FALSE;
            }

            pe.peFlags = 0;

            hr = pddp->SetEntries(0, dwColorKey, 1, &pe);

            pddp->Release();

            if (ResultFailed(hr, TEXT("IDirectDrawPalette::SetEntries"))) {
                return FALSE;
            }

            return TRUE;
        }

        pddp->Release();        
    }

    // If the texture contains alpha information, use the alpha bits
    // to simulate the colorkeying
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS
        && (g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps 
            & D3DPTEXTURECAPS_TRANSPARENCY)) 
    {
        DWORD   dwPitch;
        UINT    i, j;

        // Lock the surface
        memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        hr = pd3dt->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface4::Lock"))) {
            return FALSE;
        }

        switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {

            case 16: {

                LPWORD pwPixel = (LPWORD)ddsd.lpSurface;

                dwPitch = ddsd.lPitch / (sizeof(WORD) / sizeof(BYTE));

                for (i = 0; i < ddsd.dwHeight; i++) {

                    for (j = 0; j < ddsd.dwWidth; j++) {

                        if ((DWORD)(*(pwPixel + j) & ~(WORD)ddsd.ddpfPixelFormat.dwRGBAlphaBitMask) == (WORD)dwColorKey) {
                            *(pwPixel + j) &= ~ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
                        }
                    }

                    pwPixel += dwPitch;
                }

                break;
            }

            case 32: {

                LPDWORD pdwPixel = (LPDWORD)ddsd.lpSurface;

                dwPitch = ddsd.lPitch / (sizeof(DWORD) / sizeof(BYTE));

                for (i = 0; i < ddsd.dwHeight; i++) {

                    for (j = 0; j < ddsd.dwWidth; j++) {

                        if ((*(pdwPixel + j) & ~ddsd.ddpfPixelFormat.dwRGBAlphaBitMask) == dwColorKey) {
                            *(pdwPixel + j) &= ~ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
                        }
                    }

                    pdwPixel += dwPitch;
                }

                break;
            }
        }

        // Unlock the surface
        hr = pd3dt->Unlock(NULL);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Unlock"))) {
            return FALSE;
        }
    }

    // If source colorkeying is supported, use it
    else if ((g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_COLORKEYBLEND) &&
             ((g_pdevdCurrent->d3ddDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION && 
             (g_pdrvdCurrent->ddcHAL.dwCKeyCaps & DDCKEYCAPS_SRCBLT)) ||
             (!(g_pdevdCurrent->d3ddDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) && 
             (g_pdrvdCurrent->ddcHEL.dwCKeyCaps & DDCKEYCAPS_SRCBLT))))
             
    {
        DDCOLORKEY ddck;

        ddck.dwColorSpaceLowValue = dwColorKey;
        ddck.dwColorSpaceHighValue = dwColorKey;
        hr = pd3dt->SetColorKey(DDCKEY_SRCBLT, &ddck);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface7::SetColorKey"))) {
            return FALSE;
        }
    }

    // Otherwise a source colorkey cannot be applied to the texture
    else {

        DebugString(TEXT("A source colorkey cannot be applied to the texture ")
                    TEXT("without either source colorkey support or an alpha ")
                    TEXT("channel in the texture/palette"));
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     EnableColorKey
//
// Description:
//
//     Set the appropriate render state to enable or disable colorkeyed
//     rendering.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pd3dt       - Direct3DTexture object to be 
//                                        colorkeyed
//
//     BOOL bEnable                     - Set to TRUE to enable colorkeying,
//                                        set to FALSE to disable it.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL EnableColorKey(LPDIRECTDRAWSURFACE7 pd3dt, BOOL bEnable) 
{
    DDSURFACEDESC2       ddsd;
    HRESULT              hr;

    if (!pd3dt) {
        return FALSE;
    }

    // Get the description of the texture surface
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    hr = pd3dt->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface7::GetSurfaceDesc"))) {
        return FALSE;
    }

    // If the texture contains alpha information and the device supports
    // transparency, enable alpha testing
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS
        && (g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps 
            & D3DPTEXTURECAPS_TRANSPARENCY)) 
    {
        g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, bEnable);
    }

    // If the texture contains alpha information and the device supports
    // translucency, enable alpha blending
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS
        && (g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps 
            & D3DPTEXTURECAPS_ALPHA)) 
    {
        g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, bEnable);
    }

    // If source colorkeying is supported by either hardware or emulation, use it
    else if ((g_pdevdCurrent->d3ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_COLORKEYBLEND) &&
             ((g_pdevdCurrent->d3ddDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION && 
             (g_pdrvdCurrent->ddcHAL.dwCKeyCaps & DDCKEYCAPS_SRCBLT)) ||
             (!(g_pdevdCurrent->d3ddDesc.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) && 
             (g_pdrvdCurrent->ddcHEL.dwCKeyCaps & DDCKEYCAPS_SRCBLT))))
    {
        g_pd3dDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, bEnable);
    }

    // Otherwise colorkeying cannot be enabled/disabled
    else {
        return FALSE;
    }

    return true;
}

//******************************************************************************
//
// Function:
//
//     RestoreTextures
//
// Description:
//
//     Restore all textures.
//
// Arguments:
//
//     LPDIRECT3DDEVICE7 pd3dDevice         - Current device object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL RestoreTextures(LPDIRECT3DDEVICE7 pd3dDevice) {

    PTEXTUREDESC ptexd;
    BOOL         bRet;
    HRESULT      hr;

    for (ptexd = g_ptexdList; ptexd; ptexd = ptexd->ptexdNext) {

        if (ptexd->pdds->IsLost() == DDERR_SURFACELOST) {

            hr = ptexd->pdds->Restore();
            if (ResultFailed(hr, TEXT("IDirectDrawSurface7::Restore"))) {
                bRet = FALSE;
                continue;
            }

            if (*(ptexd->szName) || ptexd->wResourceID) {

                if (!PopulateTexture(ptexd->pdds, ptexd->wResourceID ? 
                    MAKEINTRESOURCE(ptexd->wResourceID) : ptexd->szName, 
                    ptexd->fAlpha, ptexd->cColorKey)) 
                {
                    bRet = FALSE;
                }
            }
        }
    }

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ColorToPixel
//
// Description:
//
//     Calculate the surface-specific colorkey value (the representation of
//     the pixel in surface memory) from the red, green, and blue
//     components of the given D3DCOLOR.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pdds       - Surface on which to calculate the 
//                                       pixel value
//
//     D3DCOLOR c                      - Color of the pixel
//
// Return Value:
//
//     The pixel representation in surface memory of the given color value on
//     success, zero on failure.
//
//******************************************************************************
DWORD ColorToPixel(LPDIRECTDRAWSURFACE7 pdds, D3DCOLOR c) {

    HRESULT         hr;
    DDSURFACEDESC2  ddsd;
    BYTE            r, g, b, a;
    DWORD           dwPixel;

    // Parameter validation
    if (!pdds) {
        return 0;
    }

    // Get the description of the surface
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);

    hr = pdds->GetSurfaceDesc(&ddsd);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetSurfaceDesc"))) {
        return 0;
    }

    r = (BYTE)RGBA_GETRED(c);
    g = (BYTE)RGBA_GETGREEN(c);
    b = (BYTE)RGBA_GETBLUE(c);
    a = (BYTE)RGBA_GETALPHA(c);

    // If the surface is palettized, select the nearest palette entry
    // to the given color
    if (ddsd.ddpfPixelFormat.dwRGBBitCount < 16) {

        LPDIRECTDRAWPALETTE pddp;
        PALETTEENTRY        ppe[256];
        UINT                uNumEntries, 
                            i;
        BYTE                match;
        int                 dr, dg, db, d, dLast;

        // Get the palette
        hr = pdds->GetPalette(&pddp);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetPalette"))) {
            return 0;
        }

        uNumEntries = (0x1 << ddsd.ddpfPixelFormat.dwRGBBitCount);

        // Retrieve the palette entries
        hr = pddp->GetEntries(0, 0, uNumEntries, ppe);
        pddp->Release();
        if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetEntries"))) {
            return 0;
        }

        dLast = INT_MAX;

        // Find the entry in the palette that most closely matches the
        // given pixel color
        for (i = 0; i < uNumEntries; i++) {

            dr = (int)ppe[i].peRed - r;
            dg = (int)ppe[i].peGreen - g;
            db = (int)ppe[i].peBlue - b;

            d = dr * dr + dg * dg + db * db;

            if (d < dLast) {
                match = i;
                dLast = d;
                if (d == 0) {
                    break;
                }
            }
        }

	    dwPixel = match;

        DebugString(TEXT("Color key set to index %d - r: %d, g: %d, b: %d"), 
                match, ppe[match].peRed, ppe[match].peGreen, ppe[match].peBlue);
    }

    else {

        UINT            sa, sr, sg, sb, s;
        DWORD           ma, mr, mg, mb;

        // Calculate mask values for getting at the alpha, 
        // red, green, and blue components of a color
        ma = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
        mr = ddsd.ddpfPixelFormat.dwRBitMask;
        mg = ddsd.ddpfPixelFormat.dwGBitMask;
        mb = ddsd.ddpfPixelFormat.dwBBitMask;

        // Calculate shift values for getting at the alpha, 
        // red, green, and blue components of a color
        if (ma) {
    	    for (sa = 0, s = ma; !(s & 0x1); sa++, s >>= 1);
        }
    	for (sr = 0, s = mr; !(s & 0x1); sr++, s >>= 1);
    	for (sg = 0, s = mg; !(s & 0x1); sg++, s >>= 1);
    	for (sb = 0, s = mb; !(s & 0x1); sb++, s >>= 1);

        dwPixel = ((BYTE)((float)(ma >> sa) * ((float)a / 255.0f) + 0.5f) << sa) |
                  ((BYTE)((float)(mr >> sr) * ((float)r / 255.0f) + 0.5f) << sr) |
                  ((BYTE)((float)(mg >> sg) * ((float)g / 255.0f) + 0.5f) << sg) |
                  ((BYTE)((float)(mb >> sb) * ((float)b / 255.0f) + 0.5f) << sb);
    }

    return dwPixel;
}

//******************************************************************************
//
// Function:
//
//     ReduceResolution
//
// Description:
//
//     Reduce the resolution of the given mipmap surface by a factor of two and 
//     copy the lower resolution image into the next level of the mipmap.
//
// Arguments:
//
//     LPDIRECTDRAWSURFACE7 pddsSrc    - Mipmap surface to be reduced
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL ReduceResolution(LPDIRECTDRAWSURFACE7 pddsSrc) {

    LPDIRECTDRAWSURFACE7 pddsDst;
    DDSURFACEDESC2       ddsdSrc,
                         ddsdDst;
    DDSCAPS2             ddscaps;
    DDCOLORKEY           ddck;
    BOOL                 bColorKey;
    UINT                 uPixels;
    UINT                 i, j, k;
    BOOL                 bRet;
    HRESULT              hr;

    // Parameter validation
    if (!pddsSrc) {
        return FALSE;
    }

    memset(&ddscaps, 0, sizeof(DDSCAPS2));
    ddscaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

    // Get the next level in the mipmap chain
    hr = pddsSrc->GetAttachedSurface(&ddscaps, &pddsDst);
    if (hr == DDERR_NOTFOUND) {
        // There are no more levels in the mipmap chain
        return TRUE;
    }
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetAttachedSurface"))) {
        return FALSE;
    }

    // Get the colorkey (if one was set)
    if (g_pdrvdCurrent->ddcHAL.dwCKeyCaps & DDCKEYCAPS_SRCBLT
        || g_pdrvdCurrent->ddcHEL.dwCKeyCaps & DDCKEYCAPS_SRCBLT) 
    {
        hr = pddsSrc->GetColorKey(DDCKEY_SRCBLT, &ddck);
        bColorKey = SUCCEEDED(hr);
        if (bColorKey) {
            // Set the colorkey for the destination
            hr = pddsDst->SetColorKey(DDCKEY_SRCBLT, &ddck);
            if (ResultFailed(hr, TEXT("IDirectDrawSurface4::SetColorKey"))) {
                pddsDst->Release();
                return FALSE;
            }
        }
    }

    // Lock the higher resolution (source) surface
    memset(&ddsdSrc, 0, sizeof(DDSURFACEDESC2));
    ddsdSrc.dwSize = sizeof(DDSURFACEDESC2);
    hr = pddsSrc->Lock(NULL, &ddsdSrc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::Lock"))) {
        pddsDst->Release();
        return FALSE;
    }

    // Lock the lower resolution (destination) surface
    memset(&ddsdDst, 0, sizeof(DDSURFACEDESC2));
    ddsdDst.dwSize = sizeof(DDSURFACEDESC2);
    hr = pddsDst->Lock(NULL, &ddsdDst, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::Lock"))) {
        pddsDst->Release();
        pddsSrc->Unlock(NULL);
        return FALSE;
    }

    // Reduce the resolution
    if (ddsdDst.ddpfPixelFormat.dwRGBBitCount < 16) {

        LPDIRECTDRAWPALETTE pddp;
        PALETTEENTRY        ppe[256];
        UINT                uNumEntries = (0x1 << ddsdDst.ddpfPixelFormat.dwRGBBitCount);
        BOOL                bPal8 = (BOOL)(ddsdDst.ddpfPixelFormat.dwRGBBitCount == 8);
        BOOL                bAlphaPal;
        DWORD               dwCaps;
        LPBYTE              psrc = (LPBYTE)ddsdSrc.lpSurface,
                            pdstRow = (LPBYTE)ddsdDst.lpSurface,
                            pdst;
        BYTE                c[4], match;
        PALETTEENTRY        ppePixel[4];
        int                 nRed, nGreen, nBlue, 
                            nRedDiff, nGreenDiff, nBlueDiff, 
                            nDiff, nLast;
        BOOL                bMatchFound;
        const UINT          uCacheEntries = 5;
        UINT                uNextCacheEntry = 0;
        struct {
            int             nRed, nGreen, nBlue;
            BYTE            match;
        } mcMatchCache[uCacheEntries];

        // Initialize the cache
        for (i = 0; i < uCacheEntries; i++) {
            mcMatchCache[i].nRed = mcMatchCache[i].nGreen = mcMatchCache[i].nBlue = -1;
            mcMatchCache[i].match = 0;
        }

        // Get the palette from the source
        hr = pddsSrc->GetPalette(&pddp);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface4::GetPalette"))) {
            pddsSrc->Unlock(NULL);
            pddsDst->Unlock(NULL);
            pddsDst->Release();
            return FALSE;
        }

        // Set the palette for the destination
        hr = pddsDst->SetPalette(pddp);
        if (ResultFailed(hr, TEXT("IDirectDrawSurface4::SetPalette"))) {
            pddp->Release();
            pddsSrc->Unlock(NULL);
            pddsDst->Unlock(NULL);
            pddsDst->Release();
            return FALSE;
        }

        // Check if the palette contains alpha information
        hr = pddp->GetCaps(&dwCaps);
        if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetCaps"))) {
            pddp->Release();
            pddsSrc->Unlock(NULL);
            pddsDst->Unlock(NULL);
            pddsDst->Release();
            return FALSE;
        }

        bAlphaPal = (dwCaps & DDPCAPS_ALPHA) == DDPCAPS_ALPHA;

        // Obtain the palette entries
        hr = pddp->GetEntries(0, 0, uNumEntries, ppe);
        pddp->Release();
        if (ResultFailed(hr, TEXT("IDirectDrawPalette::GetEntries"))) {
            pddsSrc->Unlock(NULL);
            pddsDst->Unlock(NULL);
            pddsDst->Release();
            return FALSE;
        }

        for (i = 0; i < ddsdDst.dwHeight; i++) {

            pdst = pdstRow;

            for (j = 0; j < ddsdDst.dwWidth; j++) {

                // Get a four pixel group from the high resolution surface.
                // These pixels will be interpolated into a single pixel in
                // the low resolution surface
                if (!bPal8) {
                    c[0] = psrc[(ddsdSrc.lPitch * i * 2) + j] >> 4;
                    c[1] = psrc[(ddsdSrc.lPitch * i * 2) + j] & 0xF; 
                    c[2] = psrc[(ddsdSrc.lPitch * (i * 2 + 1)) + j] >> 4;
                    c[3] = psrc[(ddsdSrc.lPitch * (i * 2 + 1)) + j] & 0xF;
                }
                else {
                    c[0] = psrc[(ddsdSrc.lPitch * i * 2) + j * 2];
                    c[1] = psrc[(ddsdSrc.lPitch * i * 2) + (j * 2 + 1)]; 
                    c[2] = psrc[(ddsdSrc.lPitch * (i * 2 + 1)) + j * 2];
                    c[3] = psrc[(ddsdSrc.lPitch * (i * 2 + 1)) + (j * 2 + 1)];
                }

                // If a pixel is colorkeyed (using SetColorKey or alpha blending), 
                // exclude it from the interpolation
                uPixels = 0;

                for (k = 0; k < 4; k++) {
                    if ((bColorKey && 
                        c[k] == ddck.dwColorSpaceLowValue) ||
                        (bAlphaPal && !ppe[c[k]].peFlags)) 
                    {
                        memset(&ppePixel[k], 0, sizeof(PALETTEENTRY));
                    }
                    else {
                        ppePixel[k] = ppe[c[k]];
                        uPixels++;
                    }
                }

                // If at least three of the four pixels to be combined are colorkeyed,
                // the combined pixel should also be colorkeyed
                if (uPixels < 2) {
                    if (bColorKey) {
                        if (bPal8) {
                            *pdst = (BYTE)ddck.dwColorSpaceLowValue;
                        }
                        else {
                            if (j % 2) {
                                *pdst |= (BYTE)ddck.dwColorSpaceLowValue;
                            }
                            else {
                                *pdst = (BYTE)ddck.dwColorSpaceLowValue << 4;
                            }
                        }
                    }
                    else {
                        for (k = 0; k < 4; k++) {
                            if (!ppe[c[k]].peFlags) {
                                if (bPal8) {
                                    *pdst = c[k];
                                }
                                else {
                                    if (j % 2) {
                                        *pdst |= c[k];
                                    }
                                    else {
                                        *pdst = c[k] << 4;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                // Otherwise perform the interpolation
                else {

                    nRed   = ((int)ppePixel[0].peRed + 
                                           ppePixel[1].peRed +
                                           ppePixel[2].peRed + 
                                           ppePixel[3].peRed) / uPixels;

                    nGreen = ((int)ppePixel[0].peGreen + 
                                           ppePixel[1].peGreen +
                                           ppePixel[2].peGreen + 
                                           ppePixel[3].peGreen) / uPixels;

                    nBlue  = ((int)ppePixel[0].peBlue + 
                                           ppePixel[1].peBlue +
                                           ppePixel[2].peBlue + 
                                           ppePixel[3].peBlue) / uPixels;

                    // First check the cache for a match
                    bMatchFound = false;
                    for (k = 0; k < uCacheEntries; k++) {
                        if (nRed == mcMatchCache[i].nRed && 
                            nGreen == mcMatchCache[i].nGreen && 
                            nBlue == mcMatchCache[i].nBlue)
                        {
                            match = mcMatchCache[i].match;
                            bMatchFound = true;
                            break;
                        }
                    }

                    if (!bMatchFound) {

                        nLast = INT_MAX;

                        // Find the entry in the palette that most closely matches the
                        // interpolated pixel
                        for (k = 0; k < uNumEntries; k++) {
                            nRedDiff   = (int)ppe[k].peRed - nRed;
                            nGreenDiff = (int)ppe[k].peGreen - nGreen;
                            nBlueDiff  = (int)ppe[k].peBlue - nBlue;
                            nDiff = nRedDiff * nRedDiff + 
                                    nGreenDiff * nGreenDiff + 
                                    nBlueDiff * nBlueDiff;
                            if (nDiff < nLast) {
                                match = k;
                                nLast = nDiff;
                                if (nDiff == 0) {
                                    break;
                                }
                            }
                        }

                        // Update the cache
                        mcMatchCache[uNextCacheEntry].nRed = nRed;
                        mcMatchCache[uNextCacheEntry].nGreen = nGreen;
                        mcMatchCache[uNextCacheEntry].nBlue = nBlue;
                        mcMatchCache[uNextCacheEntry].match = match;
                        if (++uNextCacheEntry >= uCacheEntries) {
                            uNextCacheEntry = 0;
                        }
                    }

                    if (bPal8) {
                        *pdst = match;
                    }
                    else {
                        if (j % 2) {
                            *pdst |= match;
                        }
                        else {
                            *pdst = match << 4;
                        }
                    }
                }

                if (bPal8 || j % 2) {
                    pdst++;
                }
            }

            pdstRow += ddsdDst.lPitch;
        }
    }

    else if (ddsdDst.ddpfPixelFormat.dwRGBBitCount == 16) {

        LPWORD          pwSrc = (LPWORD)ddsdSrc.lpSurface,
                        pwDst = (LPWORD)ddsdDst.lpSurface;
        DWORD           dwPitchSrc = ddsdSrc.lPitch 
                                     / (sizeof(WORD) / sizeof(BYTE)),
                        dwPitchDst = ddsdDst.lPitch 
                                     / (sizeof(WORD) / sizeof(BYTE));
        WORD            w[4];
        UINT            sa, sr, sg, sb, s;
        DWORD           ma, mr, mg, mb;

        // Calculate mask values for getting at the alpha, 
        // red, green, and blue components of a color
        ma = ddsdSrc.ddpfPixelFormat.dwRGBAlphaBitMask;
        mr = ddsdSrc.ddpfPixelFormat.dwRBitMask;
        mg = ddsdSrc.ddpfPixelFormat.dwGBitMask;
        mb = ddsdSrc.ddpfPixelFormat.dwBBitMask;

        // Calculate shift values for getting at the alpha, 
        // red, green, and blue components of a color
        if (ma) {
    	    for (sa = 0, s = ma; !(s & 0x1); sa++, s >>= 1);
        }
    	for (sr = 0, s = mr; !(s & 0x1); sr++, s >>= 1);
    	for (sg = 0, s = mg; !(s & 0x1); sg++, s >>= 1);
    	for (sb = 0, s = mb; !(s & 0x1); sb++, s >>= 1);

        // Reduce the resolution by taking groups of four pixels from the high 
        // resolution surface and interpolating them to produce the 
        // corresponding pixel in the low resolution surface
        for (i = 0; i < ddsdDst.dwHeight; i++) {

            for (j = 0; j < ddsdDst.dwWidth; j++) {

                // Get a four pixel group from the high resolution surface.
                // These pixels will be interpolated into a single pixel in
                // the low resolution surface
                w[0] = pwSrc[(dwPitchSrc * i * 2) + j * 2];
                w[1] = pwSrc[(dwPitchSrc * i * 2) + (j * 2 + 1)]; 
                w[2] = pwSrc[(dwPitchSrc * (i * 2 + 1)) + j * 2];
                w[3] = pwSrc[(dwPitchSrc * (i * 2 + 1)) + (j * 2 + 1)];

                // If a pixel is colorkeyed (using SetColorKey or alpha blending), 
                // exclude it from the interpolation
                uPixels = 0;

                for (k = 0; k < 4; k++) {
                    if (bColorKey) {
                        if ((w[k] & ~ma) == (ddck.dwColorSpaceLowValue & ~ma)) {
                            w[k] = 0;
                        }
                        else {
                            uPixels++;
                        }
                    }
                    else {
                        if (!ma || w[k] & ma) {
                            uPixels++;
                        }
                        else {
                            w[k] = 0;
                        }
                    }
                }

                // If at least three of the four pixels to be combined are colorkeyed,
                // the combined pixel should also be colorkeyed
                if (uPixels < 2) {
                    if (bColorKey) {
                        pwDst[j] = (WORD)ddck.dwColorSpaceLowValue;
                    }
                    else {
                        pwDst[j] &= ~ddsdDst.ddpfPixelFormat.dwRGBAlphaBitMask;
                    }
                }

                // Otherwise perform the interpolation
                else {
                    pwDst[j] = (WORD)(
                                   (((((w[0] & ma) >> sa) + 
                                      ((w[1] & ma) >> sa) + 
                                      ((w[2] & ma) >> sa) + 
                                      ((w[3] & ma) >> sa)) / uPixels) << sa) |
                                   (((((w[0] & mr) >> sr) + 
                                      ((w[1] & mr) >> sr) + 
                                      ((w[2] & mr) >> sr) + 
                                      ((w[3] & mr) >> sr)) / uPixels) << sr) |
                                   (((((w[0] & mg) >> sg) + 
                                      ((w[1] & mg) >> sg) + 
                                      ((w[2] & mg) >> sg) + 
                                      ((w[3] & mg) >> sg)) / uPixels) << sg) |
                                   (((((w[0] & mb) >> sb) + 
                                      ((w[1] & mb) >> sb) + 
                                      ((w[2] & mb) >> sb) + 
                                      ((w[3] & mb) >> sb)) / uPixels) << sb)
                               );
                }
            }

            pwDst += dwPitchDst;
        }
    }

    else if (ddsdDst.ddpfPixelFormat.dwRGBBitCount == 24) {

        LPBYTE          pDst;
        DWORD           dwPixel;
        DWORD           dw[4];
        UINT            sr, sg, sb, s;
        DWORD           mr, mg, mb;

        // Set mask values for getting at the 
        // red, green, and blue components of a color
        mr = ddsdSrc.ddpfPixelFormat.dwRBitMask << 8;
        mg = ddsdSrc.ddpfPixelFormat.dwGBitMask << 8;
        mb = ddsdSrc.ddpfPixelFormat.dwBBitMask << 8;

        // Set shift values for getting at the
        // red, green, and blue components of a color
    	for (sr = 0, s = mr; !(s & 0x1); sr++, s >>= 1);
    	for (sg = 0, s = mg; !(s & 0x1); sg++, s >>= 1);
    	for (sb = 0, s = mb; !(s & 0x1); sb++, s >>= 1);

        // Reduce the resolution by taking groups of four pixels from the high 
        // resolution surface and interpolating them to produce the 
        // corresponding pixel in the low resolution surface
        for (i = 0; i < ddsdDst.dwHeight; i++) {

            for (j = 0; j < ddsdDst.dwWidth; j++) {

                // REVIEW: This method will step past the surface by one byte

                // Get a four pixel group from the high resolution surface.
                // These pixels will be interpolated into a single pixel in
                // the low resolution surface
                dw[0] = *(LPDWORD)((LPBYTE)ddsdSrc.lpSurface + (ddsdSrc.lPitch * i * 2) + (j * 2 * 3));
                dw[1] = *(LPDWORD)((LPBYTE)ddsdSrc.lpSurface + (ddsdSrc.lPitch * i * 2) + ((j * 2 + 1) * 3));
                dw[2] = *(LPDWORD)((LPBYTE)ddsdSrc.lpSurface + (ddsdSrc.lPitch * (i * 2 + 1)) + (j * 2 * 3));
                dw[3] = *(LPDWORD)((LPBYTE)ddsdSrc.lpSurface + (ddsdSrc.lPitch * (i * 2 + 1)) + ((j * 2 + 1) * 3));

                // Perform the interpolation
                dwPixel = (DWORD)(
                               (((((dw[0] & mr) >> sr) + 
                                  ((dw[1] & mr) >> sr) + 
                                  ((dw[2] & mr) >> sr) + 
                                  ((dw[3] & mr) >> sr)) / 4) << sr) |
                               (((((dw[0] & mg) >> sg) + 
                                  ((dw[1] & mg) >> sg) + 
                                  ((dw[2] & mg) >> sg) + 
                                  ((dw[3] & mg) >> sg)) / 4) << sg) |
                               (((((dw[0] & mb) >> sb) + 
                                  ((dw[1] & mb) >> sb) + 
                                  ((dw[2] & mb) >> sb) + 
                                  ((dw[3] & mb) >> sb)) / 4) << sb)
                           );

                pDst = (LPBYTE)ddsdDst.lpSurface + (ddsdDst.lPitch * i) + (j * 3);
                if (sr > sb) {
                    *pDst++ = (BYTE)(dwPixel >> sr);
                    *pDst++ = (BYTE)(dwPixel >> sg);
                    *pDst++ = (BYTE)(dwPixel >> sb);
                }
                else {
                    *pDst++ = (BYTE)(dwPixel >> sb);
                    *pDst++ = (BYTE)(dwPixel >> sg);
                    *pDst++ = (BYTE)(dwPixel >> sr);
                }
            }
        }
    }

    else if (ddsdDst.ddpfPixelFormat.dwRGBBitCount == 32) {

        LPDWORD         pdwSrc = (LPDWORD)ddsdSrc.lpSurface,
                        pdwDst = (LPDWORD)ddsdDst.lpSurface;
        DWORD           dwPitchSrc = ddsdSrc.lPitch 
                                     / (sizeof(DWORD) / sizeof(BYTE)),
                        dwPitchDst = ddsdDst.lPitch 
                                     / (sizeof(DWORD) / sizeof(BYTE));
        DWORD           dw[4];
        UINT            sa, sr, sg, sb, s;
        DWORD           ma, mr, mg, mb;

        // Calculate mask values for getting at the alpha, 
        // red, green, and blue components of a color
        ma = ddsdSrc.ddpfPixelFormat.dwRGBAlphaBitMask;
        mr = ddsdSrc.ddpfPixelFormat.dwRBitMask;
        mg = ddsdSrc.ddpfPixelFormat.dwGBitMask;
        mb = ddsdSrc.ddpfPixelFormat.dwBBitMask;

        // Calculate shift values for getting at the alpha, 
        // red, green, and blue components of a color
        if (ma) {
    	    for (sa = 0, s = ma; !(s & 0x1); sa++, s >>= 1);
        }
    	for (sr = 0, s = mr; !(s & 0x1); sr++, s >>= 1);
    	for (sg = 0, s = mg; !(s & 0x1); sg++, s >>= 1);
    	for (sb = 0, s = mb; !(s & 0x1); sb++, s >>= 1);

        // Reduce the resolution by taking groups of four pixels from the high 
        // resolution surface and interpolating them to produce the 
        // corresponding pixel in the low resolution surface
        for (i = 0; i < ddsdDst.dwHeight; i++) {

            for (j = 0; j < ddsdDst.dwWidth; j++) {

                // Get a four pixel group from the high resolution surface.
                // These pixels will be interpolated into a single pixel in
                // the low resolution surface
                dw[0] = pdwSrc[(dwPitchSrc * i * 2) + j * 2];
                dw[1] = pdwSrc[(dwPitchSrc * i * 2) + (j * 2 + 1)]; 
                dw[2] = pdwSrc[(dwPitchSrc * (i * 2 + 1)) + j * 2];
                dw[3] = pdwSrc[(dwPitchSrc * (i * 2 + 1)) + (j * 2 + 1)];

                // If a pixel is colorkeyed (using SetColorKey or alpha blending), 
                // exclude it from the interpolation
                uPixels = 0;

                for (k = 0; k < 4; k++) {
                    if (bColorKey) {
                        if ((dw[k] & ~ma) == (ddck.dwColorSpaceLowValue & ~ma)) {
                            dw[k] = 0;
                        }
                        else {
                            uPixels++;
                        }
                    }
                    else {
                        if (!ma || dw[k] & ma) {
                            uPixels++;
                        }
                        else {
                            dw[k] = 0;
                        }
                    }
                }

                // If at least three of the four pixels to be combined are colorkeyed,
                // the combined pixel should also be colorkeyed
                if (uPixels < 2) {
                    if (bColorKey) {
                        pdwDst[j] = (WORD)ddck.dwColorSpaceLowValue;
                    }
                    else {
                        pdwDst[j] &= ~ddsdDst.ddpfPixelFormat.dwRGBAlphaBitMask;
                    }
                }

                // Otherwise perform the interpolation
                else {
                    pdwDst[j] = (DWORD)(
                                   (((((dw[0] & ma) >> sa) + 
                                      ((dw[1] & ma) >> sa) + 
                                      ((dw[2] & ma) >> sa) + 
                                      ((dw[3] & ma) >> sa)) / uPixels) << sa) |
                                   (((((dw[0] & mr) >> sr) + 
                                      ((dw[1] & mr) >> sr) + 
                                      ((dw[2] & mr) >> sr) + 
                                      ((dw[3] & mr) >> sr)) / uPixels) << sr) |
                                   (((((dw[0] & mg) >> sg) + 
                                      ((dw[1] & mg) >> sg) + 
                                      ((dw[2] & mg) >> sg) + 
                                      ((dw[3] & mg) >> sg)) / uPixels) << sg) |
                                   (((((dw[0] & mb) >> sb) + 
                                      ((dw[1] & mb) >> sb) + 
                                      ((dw[2] & mb) >> sb) + 
                                      ((dw[3] & mb) >> sb)) / uPixels) << sb)
                               );
                }
            }

            pdwDst += dwPitchDst;
        }
    }

    // Unlock the destination surface
    hr = pddsDst->Unlock(NULL);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::Unlock"))) {
        pddsDst->Release();
        pddsSrc->Unlock(NULL);
        return FALSE;
    }

    // Unlock the source surface
    hr = pddsSrc->Unlock(NULL);
    if (ResultFailed(hr, TEXT("IDirectDrawSurface4::Unlock"))) {
        pddsDst->Release();
        return FALSE;
    }

    // Recursively reduce the resolution on the chain
    bRet = ReduceResolution(pddsDst);
    
    pddsDst->Release();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     InitPalette
//
// Description:
//
//     Initialize a given number of palette entries with evenly spaced
//     color values.
//
// Arguments:
//
//     LPPALETTEENTRY ppe              - Pointer to an array of palette entries
//                                       to intialize
//
//     UINT uNumEntries                - Number of palette entries to initialize
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL InitPalette(LPPALETTEENTRY ppe, UINT uNumEntries) {

    UINT    i;
    BYTE    m = 1,
            c;
    float   f = 256.0f / ((float)uNumEntries / 7.0f),
            t = 0;

    // Parameter validation
    if (!ppe || uNumEntries <= 0) {
        return FALSE;
    }

    // The first entry will be black
    memset(ppe, 0, sizeof(PALETTEENTRY));

    // Take a basic iterative approach to filling the
    // palette entries with some evenly spaced color values
    for (i = 1; i < uNumEntries; i++) {

        if (m == 1) {
            t += f;
            c = (BYTE)t;
        }

        ppe[i].peRed = (m & 0x4) ? c : 0;
        ppe[i].peGreen = (m & 0x2) ? c : 0;
        ppe[i].peBlue = (m & 0x1) ? c : 0;
        ppe[i].peFlags = 0xFF;

        if (++m > 7) {
            m = 1;
        }
    }

    return TRUE;
}
*/

} // namespace DXCONIO
