/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    texture.cpp

Author:

    Matt Bronder

Description:

    Direct3D texture routines.

*******************************************************************************/

#include "d3dlocus.h"
#include <limits.h>

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

#ifdef DEMO_HACK
#define D3DFMT_LIN_V8U8 D3DFMT_LIN_G8B8
#endif

#define GET_TTYPE_MIPLEVELS(ttype)  (((ttype) & TTYPE_MIPLEVELMASK) >> 24)

//******************************************************************************
// Local function prototypes
//******************************************************************************

static BOOL             SetTranslucency(CSurface8* pd3ds, 
                            PALETTECOLOR* ppcPalette, float fAlpha, 
                            D3DCOLOR cColorKey);
static BOOL             SetTranslucency(CVolume8* pd3dv, 
                            PALETTECOLOR* ppcPalette, float fAlpha, 
                            D3DCOLOR cColorKey);
static BOOL             CalculateTangentTerms(D3DXVECTOR3* pvTangent, 
                            D3DXVECTOR3* pvBinormal, D3DVECTOR* pvPosition, 
                            DWORD dwStridePosition, PTCOORD2 ptcTCoord, 
                            DWORD dwStrideTCoord, LPWORD pwIndices, 
                            DWORD dwNumPrimitives, D3DPRIMITIVETYPE d3dpt, 
                            BOOL bInterpolate);
static BOOL             VectorToTangentSpace(BOOL bHalfVector, 
                            PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                            D3DVECTOR* pvPosition, DWORD dwStridePosition,
                            PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                            LPWORD pwIndices, DWORD dwNumPrimitives,
                            D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                            D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                            BOOL bInterpolate);
static BOOL             SetLightMapCoords(BOOL bSpecular, 
                            PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                            D3DVECTOR* pvPosition, DWORD dwStridePosition,
                            PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                            LPWORD pwIndices, DWORD dwNumPrimitives,
                            D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                            D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                            BOOL bInterpolate);

#ifdef UNDER_XBOX
static BOOL             GetBitmapInfoHeader(LPCTSTR szTexture, 
                            BITMAPINFOHEADER* pbmih);
#endif

//******************************************************************************
// Globals
//******************************************************************************

PTEXTUREDESC            g_ptexdList = NULL;

//******************************************************************************
// Image loading functions
//******************************************************************************

//******************************************************************************
HRESULT D3DXCreateTextureFromImage(LPDIRECT3DDEVICE8 pDevice, LPCTSTR szImage,
                                UINT uWidth, UINT uHeight, UINT uLevels,
                                DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool,
                                DWORD dwFilter, DWORD dwMipFilter, 
                                D3DCOLOR cColorKey, D3DXIMAGE_INFO* pImageInfo,
                                PALETTECOLOR* ppc, LPDIRECT3DTEXTURE8* ppTexture)
{
    LPVOID  pvData;
    DWORD   dwSize;
    HRESULT hr;

    if (LoadResourceData(szImage, &pvData, &dwSize)) {

        hr = D3DXCreateTextureFromFileInMemoryEx(pDevice, pvData, dwSize,
                                uWidth, uHeight, uLevels, dwUsage, fmt,
                                pool, dwFilter, dwMipFilter, cColorKey,
                                pImageInfo, (PALETTEENTRY*)ppc, ppTexture);

        UnloadResourceData(szImage);

        if (SUCCEEDED(hr)) {
#ifdef UNDER_XBOX
            ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppc, 256);
#endif
            return hr;
        }
    }

#ifndef UNDER_XBOX
    hr = D3DXCreateTextureFromResourceEx(pDevice, GetTestInstance(), szImage,
                                uWidth, uHeight, uLevels, dwUsage, fmt,
                                pool, dwFilter, dwMipFilter, cColorKey,
                                pImageInfo, (PALETTEENTRY*)ppc, ppTexture);
    if (SUCCEEDED(hr)) {
#ifdef UNDER_XBOX
        ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppc, 256);
#endif
        return hr;
    }
#endif // !UNDER_XBOX

#if defined(UNDER_XBOX) && defined(UNICODE)
    char aszImage[MAX_PATH];
    wcstombs(aszImage, szImage, MAX_PATH);
    hr = D3DXCreateTextureFromFileEx(pDevice, aszImage, uWidth, uHeight, 
                                uLevels, dwUsage, fmt, pool, dwFilter,
                                dwMipFilter, cColorKey, pImageInfo, 
                                (PALETTEENTRY*)ppc, ppTexture);
#else
    hr = D3DXCreateTextureFromFileEx(pDevice, szImage, uWidth, uHeight, 
                                uLevels, dwUsage, fmt, pool, dwFilter,
                                dwMipFilter, cColorKey, pImageInfo, 
                                (PALETTEENTRY*)ppc, ppTexture);
#endif

    if (FAILED(hr)) {
        TCHAR szError[128];
        D3DXGetErrorString(hr, szError, 512);
        DebugString(TEXT("D3DXCreateTextureFromImage failed on %s with %s [0x%X]"), szImage, szError, hr);
    }
#ifdef UNDER_XBOX
    else {
        ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppc, 256);
    }
#endif

    return hr;
}

//******************************************************************************
HRESULT D3DXLoadSurfaceFromImage(LPDIRECT3DSURFACE8 pDstSurface, 
                                CONST PALETTECOLOR* ppcDstPalette, 
                                CONST RECT* pDstRect, LPCTSTR szImage,
                                CONST RECT* pSrcRect, DWORD dwFilter,
                                D3DCOLOR cColorKey, D3DXIMAGE_INFO* pImageInfo)
{
    LPVOID  pvData;
    DWORD   dwSize;
    HRESULT hr;

    if (LoadResourceData(szImage, &pvData, &dwSize)) {

        hr = D3DXLoadSurfaceFromFileInMemory(pDstSurface, (PALETTEENTRY*)ppcDstPalette, pDstRect,
                                pvData, dwSize, pSrcRect, dwFilter, cColorKey,
                                pImageInfo);

        UnloadResourceData(szImage);

        if (SUCCEEDED(hr)) {
#ifdef UNDER_XBOX
            ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppcDstPalette, 256);
#endif
            return hr;
        }
    }

#ifndef UNDER_XBOX
    hr = D3DXLoadSurfaceFromResource(pDstSurface, (PALETTEENTRY*)ppcDstPalette, pDstRect,
                                GetTestInstance(), szImage, pSrcRect, dwFilter,
                                cColorKey, pImageInfo);
    if (SUCCEEDED(hr)) {
#ifdef UNDER_XBOX
        ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppcDstPalette, 256);
#endif
        return hr;
    }
#endif // !UNDER_XBOX

#if defined(UNDER_XBOX) && defined(UNICODE)
    char aszImage[MAX_PATH];
    wcstombs(aszImage, szImage, MAX_PATH);
    hr = D3DXLoadSurfaceFromFile(pDstSurface, (PALETTEENTRY*)ppcDstPalette, pDstRect, aszImage,
                                pSrcRect, dwFilter, cColorKey, pImageInfo);
#else
    hr = D3DXLoadSurfaceFromFile(pDstSurface, (PALETTEENTRY*)ppcDstPalette, pDstRect, szImage,
                                pSrcRect, dwFilter, cColorKey, pImageInfo);
#endif
    if (FAILED(hr)) {
        TCHAR szError[128];
        D3DXGetErrorString(hr, szError, 512);
        DebugString(TEXT("D3DXLoadSurfaceFromImage failed on %s with %s [0x%X]"), szImage, szError, hr);
    }
#ifdef UNDER_XBOX
    else {
        ConvertPALETTEENTRYToD3DCOLOR((LPVOID)ppcDstPalette, 256);
    }
#endif

    return hr;
}

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
//     Create a texture object of the given dimensions, format, and type.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     DWORD dwWidth                        - Width of the texture
//
//     DWORD dwHeight                       - Height of the texture
//
//     D3DFORMAT fmt                        - Format of the texture
//
//     TEXTURETYPE tt                       - Type of texture to create (i.e.
//                                            mipmap, cubemap, bumpmap, etc.)
//
//     D3DPOOL pool                         - Location in which to create the
//                                            texture
//
// Return Value:
//
//     A pointer to the created texture object on success, NULL on failure.
//
//******************************************************************************
CBaseTexture8* CreateTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight, 
                                    D3DFORMAT fmt, TEXTURETYPE tt, D3DPOOL pool)
{
    return CreateTexture(pDevice, uWidth, uHeight, 0, fmt, tt, pool);
}

//******************************************************************************
//
// Function:
//
//     CreateTexture
//
// Description:
//
//     Create a texture object of the given dimensions, format, and type.
//
// Arguments:
//
//     CDevice8* pDevice                    - Pointer to the device object
//
//     DWORD dwWidth                        - Width of the texture
//
//     DWORD dwHeight                       - Height of the texture
//
//     DWORD dwDepth                        - Depth of the texture (for a
//                                            volume)
//
//     D3DFORMAT fmt                        - Format of the texture
//
//     TEXTURETYPE tt                       - Type of texture to create (i.e.
//                                            mipmap, cubemap, bumpmap, etc.)
//
//     D3DPOOL pool                         - Location in which to create the
//                                            texture
// Return Value:
//
//     A pointer to the created texture object on success, NULL on failure.
//
//******************************************************************************
CBaseTexture8* CreateTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight, 
                                    UINT uDepth, D3DFORMAT fmt, TEXTURETYPE tt, 
                                    D3DPOOL pool)
{
    CBaseTexture8*      pd3dtb;
    D3DFORMAT           fmtt;
    PTEXTUREDESC        ptexdNode;
    UINT                uLevels;
    DWORD               dwUsage = 0;
    D3DRESOURCETYPE     rt;
    HRESULT             hr;

    // Verify the current device supports the given format
    if (tt & TTYPE_DEPTH) {
        dwUsage |= D3DUSAGE_DEPTHSTENCIL;
        pool = D3DPOOL_DEFAULT;
    }
    else if (tt & TTYPE_TARGET) {
        dwUsage |= D3DUSAGE_RENDERTARGET;
        pool = D3DPOOL_DEFAULT;
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

    hr = CheckCurrentDeviceFormat(pDevice, dwUsage, rt, fmt);
    if (FAILED(hr)) {
        if (hr != D3DERR_NOTAVAILABLE) {
            ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
        }
        return NULL;
    }

    // Get the number of levels for the texture
    if (tt & TTYPE_MIPMAP) {
        uLevels = GET_TTYPE_MIPLEVELS(tt);
    }
    else {
        uLevels = 1;
    }

    // Create a texture of the specified type
    if (tt & TTYPE_VOLUME) {

        D3DVOLUME_DESC d3dvd;

        hr = pDevice->CreateVolumeTexture(uWidth, uHeight, uDepth, 
                                    uLevels, dwUsage, fmt, pool, 
                                    (CVolumeTexture8**)&pd3dtb);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateVolumeTexture"));
            return NULL;
        }

        hr = ((CVolumeTexture8*)pd3dtb)->GetLevelDesc(0, &d3dvd);
        if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetLevelDesc"))) {
            pd3dtb->Release();
            return NULL;
        }
        fmtt = d3dvd.Format;
    }
    else if (tt & TTYPE_CUBEMAP) {

        D3DSURFACE_DESC d3dsd;

        hr = pDevice->CreateCubeTexture(uWidth > uHeight ? uWidth : uHeight,
                                    uLevels, dwUsage, fmt, pool, 
                                    (CCubeTexture8**)&pd3dtb);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"));
            return NULL;
        }

        hr = ((CCubeTexture8*)pd3dtb)->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetLevelDesc"))) {
            pd3dtb->Release();
            return NULL;
        }
        fmtt = d3dsd.Format;
    }
    else {

        D3DSURFACE_DESC d3dsd;

        hr = pDevice->CreateTexture(uWidth, uHeight,
                                    uLevels, dwUsage, fmt, pool, 
                                    (CTexture8**)&pd3dtb);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateTexture"));
            return NULL;
        }

        hr = ((CTexture8*)pd3dtb)->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
            pd3dtb->Release();
            return NULL;
        }
        fmtt = d3dsd.Format;
    }

    // Add the texture to the texture list
    ptexdNode = (PTEXTUREDESC)MemAlloc(sizeof(TEXTUREDESC));
    if (!ptexdNode) {
        pd3dtb->Release();
        return NULL;
    }
    memset(ptexdNode, 0, sizeof(TEXTUREDESC));
    ptexdNode->pd3dtb = pd3dtb;
    ptexdNode->uWidth = uWidth;
    ptexdNode->uHeight = uHeight;
    ptexdNode->uDepth = uDepth;
    ptexdNode->fmt = fmtt;
    ptexdNode->pool = pool;
    ptexdNode->tt = tt;
    ptexdNode->ptexdNext = g_ptexdList;
    g_ptexdList = ptexdNode;

    return pd3dtb;
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
//     CDevice8* pDevice                    - Pointer to the device object
//
//     LPCTSTR szName                       - File or resource name of the 
//                                            bitmap from which to create the 
//                                            texture
//
//     D3DFORMAT fmt                        - Format of the texture
//
//     TEXTURETYPE tt                       - Type of texture to create (i.e.
//                                            mipmap, cubemap, bumpmap, etc.)
//
//     PALETTECOLOR* ppcPalette             - Pointer to the first entry of
//                                            a palette to be initialized (if
//                                            palettized)
//
//     float fAlpha                         - Optional transparency value for
//                                            the texture
//
//     D3DCOLOR cColorKey                   - Optional color key for the
//                                            texture
//
//     D3DPOOL pool                         - Location in which to create the
//                                            texture
//
//     UINT uWidth                          - Optional width of the texture
//                                            (if not given the texture will
//                                            take on the size of the bitmap
//                                            used to populate it)
//
//     UINT uHeight                         - Optional height of the texture
//
// Return Value:
//
//     A pointer to the created texture object on success, NULL on failure.
//
//******************************************************************************
CBaseTexture8* CreateTexture(CDevice8* pDevice, LPCTSTR szName, 
                                 D3DFORMAT fmt, TEXTURETYPE tt,
                                 PALETTECOLOR* ppcPalette, float fAlpha,
                                 D3DCOLOR cColorKey, D3DPOOL pool,
                                 UINT uWidth, UINT uHeight)
{
    CBaseTexture8*          pd3dtb = NULL;
    D3DRESOURCETYPE         rt;
    D3DFORMAT               fmtt;
    UINT                    uLevels;
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
    if (tt & TTYPE_DEPTH) {
        dwUsage |= D3DUSAGE_DEPTHSTENCIL;
        pool = D3DPOOL_DEFAULT;
    }
    else if (tt & TTYPE_TARGET) {
        dwUsage |= D3DUSAGE_RENDERTARGET;
        pool = D3DPOOL_DEFAULT;
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

    hr = CheckCurrentDeviceFormat(pDevice, dwUsage, rt, fmt);
    if (FAILED(hr)) {
        if (hr != D3DERR_NOTAVAILABLE) {
            ResultFailed(hr, TEXT("IDirect3D8::CheckDeviceFormats"));
        }
        return NULL;
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

    // Get the number of levels for the texture
    if (tt & TTYPE_MIPMAP) {
        uLevels = GET_TTYPE_MIPLEVELS(tt);
    }
    else {
        uLevels = 1;
    }

    if (tt & TTYPE_VOLUME) {

        return NULL;
    }

    else if (tt & TTYPE_CUBEMAP) {

        CCubeTexture8*          pd3dtc;
        CSurface8*              pSurface;
        D3DSURFACE_DESC         d3dsd;
        TCHAR                   szFace[6][MAX_PATH];
        TCHAR                   szTemp[MAX_PATH];
        TCHAR*                  sz;
        UINT                    i;

        if (uWidth == D3DX_DEFAULT || uHeight == D3DX_DEFAULT) {

            LPDIRECT3DTEXTURE8 pd3dtTemp;

            hr = D3DXCreateTextureFromImage(pDevice->GetIDirect3DDevice8(),
                                szName, D3DX_DEFAULT, D3DX_DEFAULT, 1, dwUsage,
                                fmt, pool, D3DX_FILTER_POINT, D3DX_FILTER_POINT,
                                0, NULL, ppcPalette, &pd3dtTemp);
            if (FAILED(hr)) {
                return NULL;
            }

            hr = pd3dtTemp->GetLevelDesc(0, &d3dsd);
            pd3dtTemp->Release();
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
                return NULL;
            }

            if (uWidth == D3DX_DEFAULT) {
                uWidth = d3dsd.Width;
            }

            if (uHeight == D3DX_DEFAULT) {
                uHeight = d3dsd.Height;
            }
        }

        // Create the cubemap
        hr = pDevice->CreateCubeTexture(uWidth > uHeight ? uWidth : uHeight,
                            uLevels, dwUsage, fmt, pool, &pd3dtc);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DDevice8::CreateCubeTexture"));
            return NULL;
        }

        hr = pd3dtc->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetLevelDesc"))) {
            pd3dtc->Release();
            return NULL;
        }
        fmtt = d3dsd.Format;

        // Initialize the image names for the faces.  If the image name ends with the
        // number 0, assume that six separate image files/resources exist, one for each
        // face of the cube map.  They will all have the same base image name, being
        // differentiated only by the ending character of the name, with that character
        // being a number 0-5.  The images will be loaded into their corresponding faces
        // according to the enumeration order of the faces in the D3DCUBEMAP_FACES type.
        // If the given image name does not end with a 0, load the given image into
        // each of the faces of the cube map, resulting in identical images for each
        // face.
        _tcscpy(szTemp, szName);
        sz = szTemp + _tcslen(szTemp);
        if (*(sz-1) == TEXT('0')) {
            sz--;
        }
        else if (*(sz-5) == TEXT('0')) {
            sz -= 5;
        }
        if (*sz) {
            for (i = 0; i < 6; i++) {
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

            hr = pd3dtc->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pSurface);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetCubeMapSurface"));
                pd3dtc->Release();
                return NULL;
            }

            hr = D3DXLoadSurfaceFromImage(pSurface->GetIDirect3DSurface8(), ppcPalette, NULL,
                            szFace[i], NULL, D3DX_FILTER_LINEAR, 0, NULL);
            if (FAILED(hr)) {
                pSurface->Release();
                pd3dtc->Release();
                return NULL;
            }

            pSurface->Release();
        }

        pd3dtb = pd3dtc;
    }

    else if (tt & TTYPE_BUMPMAP || tt & TTYPE_NORMALMAP) {

        // The given image is a height map.  Use it to generate the bump map
        CTexture8*          pd3dtHeight;
        D3DSURFACE_DESC     d3dsd;

        // Load the height map
        pd3dtHeight = (CTexture8*)CreateTexture(pDevice, szName, D3DFMT_A8R8G8B8, 
                                            TTYPE_TEXTURE, NULL, 1.0f, 
                                            COLORKEY_NONE, pool, uWidth, uHeight);
        if (!pd3dtHeight) {
            return NULL;
        }

        hr = pd3dtHeight->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
            ReleaseTexture(pd3dtHeight);
            return NULL;
        }

        if (uWidth == D3DX_DEFAULT) {
            uWidth = d3dsd.Width;
        }

        if (uHeight == D3DX_DEFAULT) {
            uHeight = d3dsd.Height;
        }

        // Create the bump map
        pd3dtb = (CTexture8*)CreateTexture(pDevice, d3dsd.Width, d3dsd.Height, fmt, tt, pool);
        if (!pd3dtb) {
            ReleaseTexture(pd3dtHeight);
            return NULL;
        }
        
        hr = CopyTexture(pd3dtb, ppcPalette, pd3dtHeight, NULL, D3DX_FILTER_LINEAR);

        ReleaseTexture(pd3dtHeight);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dtb);
            return NULL;
        }

        hr = ((CTexture8*)pd3dtb)->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
            ReleaseTexture(pd3dtb);
            return NULL;
        }
        fmtt = d3dsd.Format;

        return pd3dtb;
    }

    else {

        LPDIRECT3DTEXTURE8  pd3dt;
        LPDIRECT3DSURFACE8  pd3ds;
        CSurface8*          pSurface;
        D3DSURFACE_DESC     d3dsd;

        // Attempt to load the image as a resource.  Use a point mip filter flag
        // since the mipmap will get re-filtered using a linear filter during
        // the SetTranslucency stage.
        hr = D3DXCreateTextureFromImage(pDevice->GetIDirect3DDevice8(),
                                szName, uWidth, uHeight, 1, dwUsage, fmt,
                                pool, D3DX_FILTER_LINEAR, D3DX_FILTER_POINT,
                                0, NULL, ppcPalette, &pd3dt);
        if (FAILED(hr)) {
            return NULL;
        }

        hr = pd3dt->GetLevelDesc(0, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
            pd3dt->Release();
            return NULL;
        }

        if (uWidth == D3DX_DEFAULT) {
            uWidth = d3dsd.Width;
        }

        if (uHeight == D3DX_DEFAULT) {
            uHeight = d3dsd.Height;
        }

        hr = pDevice->CreateTexture(d3dsd.Width, d3dsd.Height, uLevels, 0, fmt, pool, (CTexture8**)&pd3dtb);
        if (FAILED(hr)) {
            pd3dt->Release();
            return NULL;
        }

        hr = pd3dt->GetSurfaceLevel(0, &pd3ds);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            pd3dt->Release();
            pd3dtb->Release();
            return NULL;
        }

        hr = ((CTexture8*)pd3dtb)->GetSurfaceLevel(0, &pSurface);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            pd3ds->Release();
            pd3dt->Release();
            pd3dtb->Release();
            return NULL;
        }

        hr = D3DXLoadSurfaceFromSurface(pSurface->GetIDirect3DSurface8(), NULL, NULL, pd3ds, NULL, NULL, D3DX_FILTER_POINT, 0);

        pSurface->Release();
        pd3ds->Release();
        pd3dt->Release();

        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"));
            pd3dtb->Release();
            return NULL;
        }

        fmtt = d3dsd.Format;
    }

    // Add the texture to the texture list
    ptexdNode = (PTEXTUREDESC)MemAlloc(sizeof(TEXTUREDESC));
    if (!ptexdNode) {
        pd3dtb->Release();
        return NULL;
    }
    memset(ptexdNode, 0, sizeof(TEXTUREDESC));
    ptexdNode->pd3dtb = pd3dtb;
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
    ptexdNode->pool = pool;
    ptexdNode->tt = tt;
    ptexdNode->fAlpha = fAlpha;
    ptexdNode->cColorKey = cColorKey;
    ptexdNode->ptexdNext = g_ptexdList;
    g_ptexdList = ptexdNode;

    // Set the translucency.  This will also perform a filter of the highest
    // resolution level(s) to all lower resolution levels (if present)
    // and sync the server images of all levels
    if (!SetTranslucency(pd3dtb, ppcPalette, fAlpha, cColorKey)) {
        if (fAlpha == 1.0f && cColorKey == COLORKEY_NONE) {
            if (tt & TTYPE_MIPMAP) {
                FilterTexture(pd3dtb, ppcPalette);
            }
            SyncTexture(pd3dtb);
        }
        else {
            DebugString(TEXT("Warning: Failed to set translucency information on texture %s"), szImage);
        }
    }

    return pd3dtb;
}

//******************************************************************************
//
// Function:
//
//     ReleaseTexture
//
// Description:
//
//     Release the given texture object and remove it from the texture table.
//
// Arguments:
//
//     CBaseTexture8* pd3dtb               - Pointer to the texture object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseTexture(CBaseTexture8* pd3dtb) {

    PTEXTUREDESC ptexd, ptexdDel;
#if defined(DEBUG) || defined(_DEBUG)
    TCHAR        szName[MAX_PATH];
    UINT         uRef;
#endif

    if (!pd3dtb) {
        return;
    }

pd3dtb->AddRef();
pd3dtb->Release();
VerifyTextureListIntegrity();

    if (g_ptexdList) {

        if (g_ptexdList->pd3dtb == pd3dtb) {
            ptexd = g_ptexdList->ptexdNext;
#if defined(DEBUG) || defined(_DEBUG)
            _tcscpy(szName, g_ptexdList->szName);
#endif
            MemFree(g_ptexdList);
            g_ptexdList = ptexd;
        }

        else {

            for (ptexd = g_ptexdList; 
                 ptexd->ptexdNext && ptexd->ptexdNext->pd3dtb != pd3dtb; 
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

VerifyTextureListIntegrity();
pd3dtb->AddRef();
pd3dtb->Release();

#if defined(DEBUG) || defined(_DEBUG)
    uRef = pd3dtb->Release();
    if (uRef != 0) {
        DebugString(TEXT("WARNING: Texture %s has a ref count of %d on release"), szName, uRef);
    }
#else
    pd3dtb->Release();
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
//     Release all textures in the texture table.
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

VerifyTextureListIntegrity();
    while (g_ptexdList) {
VerifyTextureListIntegrity();
        ReleaseTexture(g_ptexdList->pd3dtb);
    }
}

//******************************************************************************
//
// Function:
//
//     RemoveTexture
//
// Description:
//
//     Remove the given texture from the texture table (if present)
//
// Arguments:
//
//     CBaseTexture8* pd3dtb         - Pointer to the texture object
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RemoveTexture(CBaseTexture8* pd3dtb) {

    PTEXTUREDESC ptexd, ptexdDel;

    if (!pd3dtb) {
        return;
    }

    if (g_ptexdList) {

        if (g_ptexdList->pd3dtb == pd3dtb) {
            ptexd = g_ptexdList->ptexdNext;
            MemFree(g_ptexdList);
            g_ptexdList = ptexd;
        }

        else {

            for (ptexd = g_ptexdList; 
                 ptexd->ptexdNext && ptexd->ptexdNext->pd3dtb != pd3dtb; 
                 ptexd = ptexd->ptexdNext
            );

            if (ptexd->ptexdNext) {
                ptexdDel = ptexd->ptexdNext;
                ptexd->ptexdNext = ptexdDel->ptexdNext;
                MemFree(ptexdDel);
            }
        }
    }
}

//******************************************************************************
//
// Function:
//
//     SyncTexture
//
// Description:
//
//     Synchronize the server image of the given texture.
//
// Arguments:
//
//     CBaseTexture8* pd3dtb              - Pointer to the texture object
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SyncTexture(CBaseTexture8* pd3dtb) {

    PTEXTUREDESC ptexd;
    UINT         i;
    HRESULT      hr;

    ptexd = GetTextureDesc(pd3dtb);
    if (!ptexd) {
        return FALSE;
    }

    if (ptexd->tt & TTYPE_VOLUME) {

        CVolumeTexture8* pd3dtv = (CVolumeTexture8*)pd3dtb;
        D3DLOCKED_BOX  d3dlb;

        for (i = 0; i < pd3dtv->GetLevelCount(); i++) {
            hr = pd3dtv->LockBox(i, &d3dlb, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockBox"));
                return FALSE;
            }
            hr = pd3dtv->UnlockBox(i);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::UnlockBox"));
                return FALSE;
            }
        }
    }
    else if (ptexd->tt & TTYPE_CUBEMAP) {

        CCubeTexture8*  pd3dtc = (CCubeTexture8*)pd3dtb;
        D3DLOCKED_RECT  d3dlr;
        UINT            j, uCount;

        uCount = pd3dtc->GetLevelCount();

        for (j = 0; j < 6; j++) {

            for (i = 0; i < uCount; i++) {
                hr = pd3dtc->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlr, NULL, 0);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                    return FALSE;
                }
                hr = pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                    return FALSE;
                }
            }

        }
    }
    else {

        CTexture8*      pd3dt = (CTexture8*)pd3dtb;
        D3DLOCKED_RECT  d3dlr;

        for (i = 0; i < pd3dt->GetLevelCount(); i++) {
            hr = pd3dt->LockRect(i, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
                return FALSE;
            }
            hr = pd3dt->UnlockRect(i);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"));
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
//     SyncSurface
//
// Description:
//
//     Synchronize the server image of the given surface.
//
// Arguments:
//
//     CSurface8* pd3ds              - Pointer to the surface object
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SyncSurface(CSurface8* pd3ds) {

    D3DLOCKED_RECT  d3dlr;
    HRESULT         hr;

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        return FALSE;
    }

    hr = pd3ds->UnlockRect();
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     SyncVolume
//
// Description:
//
//     Synchronize the server image of the given volume.
//
// Arguments:
//
//     CVolume8* pd3dv               - Pointer to the volume object
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SyncVolume(CVolume8* pd3dv) {

    D3DLOCKED_BOX   d3dlb;
    HRESULT         hr;

    hr = pd3dv->LockBox(&d3dlb, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVolume8::LockBox"))) {
        return FALSE;
    }

    hr = pd3dv->UnlockBox();
    if (ResultFailed(hr, TEXT("IDirect3DVolume8::UnlockBox"))) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     GetARGBPixelDesc
//
// Description:
//
//     Obtain the memory layout of the given ARGB pixel format.
//
// Arguments:
//
//     D3DFORMAT fmt                        - Format of the pixel
//
//     PARGBPIXELDESC ppixd                 - Pointer to a structure to be
//                                            filled with information about
//                                            the pixel
//
// Return Value:
//
//     None.
//
//******************************************************************************
void GetARGBPixelDesc(D3DFORMAT fmt, PARGBPIXELDESC ppixd) {

    if (!ppixd) {
        return;
    }

    memset(ppixd, 0, sizeof(ARGBPIXELDESC));

    switch (fmt) {

        case D3DFMT_P8:
        case D3DFMT_A8R8G8B8:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8R8G8B8:
#endif // UNDER_XBOX
            ppixd->dwAMask     = 0xFF000000;
            ppixd->dwAShift    = 24;

        case D3DFMT_X8R8G8B8:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_X8R8G8B8:
#endif // UNDER_XBOX
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
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A1R5G5B5:
#endif // UNDER_XBOX
            ppixd->dwAMask     = 0x8000;
            ppixd->dwAShift    = 15;

        case D3DFMT_X1R5G5B5:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_X1R5G5B5:
#endif // UNDER_XBOX
            ppixd->dwRMask     = 0x7C00;
            ppixd->dwGMask     = 0x03E0;
            ppixd->dwBMask     = 0x001F;
            ppixd->dwRShift    = 10;
            ppixd->dwGShift    = 5;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_R5G6B5:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_R5G6B5:
#endif // UNDER_XBOX
            ppixd->dwRMask     = 0xF800;
            ppixd->dwGMask     = 0x07E0;
            ppixd->dwBMask     = 0x001F;
            ppixd->dwRShift    = 11;
            ppixd->dwGShift    = 5;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_A4R4G4B4:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A4R4G4B4:
#endif // UNDER_XBOX
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

        case D3DFMT_A8:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8:
#endif //UNDER_XBOX
            ppixd->dwAMask     = 0xFF;
            ppixd->dwAShift    = 0;
            ppixd->dwRMask     = 0;
            ppixd->dwGMask     = 0;
            ppixd->dwBMask     = 0;
            ppixd->dwRShift    = 0;
            ppixd->dwGShift    = 0;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 1;
            break;

#ifdef UNDER_XBOX

        case D3DFMT_A8B8G8R8:
        case D3DFMT_LIN_A8B8G8R8:
            ppixd->dwAMask     = 0xFF000000;
            ppixd->dwAShift    = 24;
            ppixd->dwRMask     = 0x000000FF;
            ppixd->dwGMask     = 0x0000FF00;
            ppixd->dwBMask     = 0x00FF0000;
            ppixd->dwRShift    = 0;
            ppixd->dwGShift    = 8;
            ppixd->dwBShift    = 16;
            ppixd->uStride     = 4;
            break;

        case D3DFMT_B8G8R8A8:
        case D3DFMT_LIN_B8G8R8A8:
            ppixd->dwAMask     = 0x000000FF;
            ppixd->dwAShift    = 0;
            ppixd->dwRMask     = 0x0000FF00;
            ppixd->dwGMask     = 0x00FF0000;
            ppixd->dwBMask     = 0xFF000000;
            ppixd->dwRShift    = 8;
            ppixd->dwGShift    = 16;
            ppixd->dwBShift    = 24;
            ppixd->uStride     = 4;
            break;

        case D3DFMT_R8G8B8A8:
        case D3DFMT_LIN_R8G8B8A8:
            ppixd->dwAMask     = 0x000000FF;
            ppixd->dwAShift    = 0;
            ppixd->dwRMask     = 0xFF000000;
            ppixd->dwGMask     = 0x00FF0000;
            ppixd->dwBMask     = 0x0000FF00;
            ppixd->dwRShift    = 24;
            ppixd->dwGShift    = 16;
            ppixd->dwBShift    = 8;
            ppixd->uStride     = 4;
            break;

        case D3DFMT_R8B8:
        case D3DFMT_LIN_R8B8:
            ppixd->dwAMask     = 0;
            ppixd->dwRMask     = 0xFF00;
            ppixd->dwGMask     = 0;
            ppixd->dwBMask     = 0x00FF;
            ppixd->dwAShift    = 0;
            ppixd->dwRShift    = 8;
            ppixd->dwGShift    = 0;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_G8B8:
        case D3DFMT_LIN_G8B8:
            ppixd->dwAMask     = 0;
            ppixd->dwRMask     = 0;
            ppixd->dwGMask     = 0xFF00;
            ppixd->dwBMask     = 0x00FF;
            ppixd->dwAShift    = 0;
            ppixd->dwRShift    = 0;
            ppixd->dwGShift    = 8;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_R4G4B4A4:
        case D3DFMT_LIN_R4G4B4A4:
            ppixd->dwAMask     = 0x000F;
            ppixd->dwAShift    = 0;
            ppixd->dwRMask     = 0xF000;
            ppixd->dwGMask     = 0x0F00;
            ppixd->dwBMask     = 0x00F0;
            ppixd->dwRShift    = 12;
            ppixd->dwGShift    = 8;
            ppixd->dwBShift    = 4;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_R5G5B5A1:
        case D3DFMT_LIN_R5G5B5A1:
            ppixd->dwAMask     = 0x0001;
            ppixd->dwRMask     = 0xF800;
            ppixd->dwGMask     = 0x07C0;
            ppixd->dwBMask     = 0x002E;
            ppixd->dwAShift    = 0;
            ppixd->dwRShift    = 11;
            ppixd->dwGShift    = 6;
            ppixd->dwBShift    = 1;
            ppixd->uStride     = 2;
            break;

        case D3DFMT_L6V5U5:
        case D3DFMT_LIN_R6G5B5:
            ppixd->dwAMask     = 0;
            ppixd->dwRMask     = 0xFC00;
            ppixd->dwGMask     = 0x03E0;
            ppixd->dwBMask     = 0x001F;
            ppixd->dwAShift    = 0;
            ppixd->dwRShift    = 11;
            ppixd->dwGShift    = 5;
            ppixd->dwBShift    = 0;
            ppixd->uStride     = 2;
            break;

#endif //UNDER_XBOX
    }
}

//******************************************************************************
//
// Function:
//
//     GetTextureDesc
//
// Description:
//
//     Locate the given texture in the texture list and return a pointer to
//     its entry.
//
// Arguments:
//
//     CBaseTexture8* pd3dtb         - Pointer to the texture object
//
// Return Value:
//
//     A pointer to the corresponding entry in the texture table.
//
//******************************************************************************
static PTEXTUREDESC GetTextureDesc(CBaseTexture8* pd3dtb) {

    PTEXTUREDESC ptexd;

    for (ptexd = g_ptexdList; 
         ptexd && ptexd->pd3dtb != pd3dtb; 
         ptexd = ptexd->ptexdNext
    );

    return ptexd;
}

//******************************************************************************
//
// Function:
//
//     FilterTexture
//
// Description:
//
//     Filter the given level of the given mipmap to all lower levels using
//     the given filter.
//
// Arguments:
//
//     CBaseTexture8* pd3dt          - Pointer to the texture object
//
//     PALETTECOLOR* ppcPalette      - Pointer to the first entry of
//                                     a palette to be initialized (if
//                                     palettized)
//
//     UINT uSrcLevel                - Level of resolution to filter
//
//     DWORD dwFilter                - Filter to use
//
// Return Value:
//
//     A pointer to the corresponding entry in the texture table.
//
//******************************************************************************
BOOL FilterTexture(CBaseTexture8* pd3dt, PALETTECOLOR* ppcPalette, UINT uSrcLevel, 
                                                                DWORD dwFilter)
{
    PTEXTUREDESC ptexd;
    PALETTEENTRY *ppe = (PALETTEENTRY*)ppcPalette;
    HRESULT      hr;
#ifdef UNDER_XBOX
    PALETTEENTRY ppeEntries[256];
#endif // UNDER_XBOX

    ptexd = GetTextureDesc(pd3dt);
    if (!ptexd) {
        return FALSE;
    }

    if (!(ptexd->tt & TTYPE_MIPMAP)) {
        return TRUE;
    }

#ifdef UNDER_XBOX
    if (ppcPalette) {
        memcpy(ppeEntries, ppcPalette, 256 * sizeof(PALETTEENTRY));
        ConvertD3DCOLORToPALETTEENTRY(ppeEntries, 256);
        ppe = ppeEntries;
    }
#endif // UNDER_XBOX

    if (ptexd->tt & TTYPE_VOLUME) {
        hr = D3DXFilterVolumeTexture(((CVolumeTexture8*)pd3dt)->GetIDirect3DVolumeTexture8(), 
                                ppe, uSrcLevel, dwFilter);
    }
    else if (ptexd->tt & TTYPE_CUBEMAP) {
        hr = D3DXFilterCubeTexture(((CCubeTexture8*)pd3dt)->GetIDirect3DCubeTexture8(), 
                                ppe, uSrcLevel, dwFilter);
    }
    else {
        hr = D3DXFilterTexture(((CTexture8*)pd3dt)->GetIDirect3DTexture8(), 
                                ppe, uSrcLevel, dwFilter);
    }

    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("D3DXFilterXTexture"));
        return FALSE;
    }

    return SyncTexture(pd3dt);
}

//******************************************************************************
//
// Function:
//
//     SetTranslucency
//
// Description:
//
//     Set the alpha channel of the given surface object to reflect the given
//     alpha and color key values.
//
// Arguments:
//
//     CSurface8* pSurface                  - Pointer to the surface object
//
//     PALETTECOLOR* ppcPalette             - Pointer to the first entry of
//                                            the palette for a palettized
//                                            surface
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
//     D3DCOLOR cColorKey                   - Color value of the pixels to
//                                            make transparent
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL SetTranslucency(CSurface8* pSurface, PALETTECOLOR* ppcPalette, 
                            float fAlpha, D3DCOLOR cColorKey) 
{
    D3DSURFACE_DESC     d3dsd;
    D3DLOCKED_RECT      d3dlr;
    ARGBPIXELDESC       pixd;
    DWORD               dwPitch;
    DWORD               dwColorKey;
    UINT                i, j;
    HRESULT             hr;

    // Get the format of the surface
    hr = pSurface->GetDesc(&d3dsd);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        return FALSE;
    }

    if (!(cColorKey == COLORKEY_NONE || cColorKey == COLORKEY_PIXEL1)) {
        // Get the surface-specific colorkey value (the representation of
        // the pixel in surface memory) from the red, green, and blue
        // components of the given D3DCOLOR
        dwColorKey = ColorToPixel(d3dsd.Format, ppcPalette, cColorKey & 0xFFFFFF);
    }

    // Lock the texture
    hr = pSurface->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
        return FALSE;
    }

    // If the surface is palettized, set the translucency in the palette entries
    if (d3dsd.Format == D3DFMT_P8) {

        CDevice8* pDevice;
        D3DCAPS8  d3dcaps;
        UINT      i;

        hr = pSurface->GetDevice(&pDevice);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDevice"))) {
            return FALSE;
        }

        hr = pDevice->GetDeviceCaps(&d3dcaps);
        pDevice->Release();
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
            return FALSE;
        }

        if (!ppcPalette || !(d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) {
            return FALSE;
        }

        for (i = 0; i < 256; i++) {
#ifndef UNDER_XBOX
            ppcPalette[i].peFlags = (BYTE)(fAlpha * 255.0f);
#else
            ppcPalette[i] = (ppcPalette[i] & 0x00FFFFFF) | ((BYTE)(fAlpha * 255.0f) << 24);
#endif // UNDER_XBOX
        }

        if (cColorKey == COLORKEY_PIXEL1) {
            dwColorKey = *(LPBYTE)d3dlr.pBits;
        }
        if (cColorKey != COLORKEY_NONE) {
#ifndef UNDER_XBOX
            ppcPalette[dwColorKey].peFlags = 0;
#else
            ppcPalette[dwColorKey] &= 0x00FFFFFF;
#endif // UNDER_XBOX
        }
    }

    else {

        // Get the pixel format of the surface
        GetARGBPixelDesc(d3dsd.Format, &pixd);

        if (cColorKey == COLORKEY_NONE) {
            dwColorKey = COLORKEY_NONE;
        }

        // Set the translucency
        switch (d3dsd.Format) {

#ifdef UNDER_XBOX
            case D3DFMT_LIN_A8R8G8B8:
#endif // UNDER_XBOX
            case D3DFMT_A8R8G8B8:

                LPDWORD pdwPixel;
                DWORD   dwAlpha;

                dwAlpha = (DWORD)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask;
                pdwPixel = (LPDWORD)d3dlr.pBits;

                // Assume an aligned pitch value (i.e. the quotient of the following
                // division will not have a remainder)
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

#ifdef UNDER_XBOX
            case D3DFMT_LIN_A1R5G5B5:
            case D3DFMT_LIN_A4R4G4B4:
#endif // UNDER_XBOX
            case D3DFMT_A1R5G5B5:
            case D3DFMT_A4R4G4B4: {

                LPWORD pwPixel;
                WORD   wAlpha;

                wAlpha = (WORD)((UINT)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask);
                pwPixel = (LPWORD)d3dlr.pBits;

                // Assume an aligned pitch value (i.e. the quotient of the following
                // division will not have a remainder)
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
    }

    // Unlock the surface
    hr = pSurface->UnlockRect();
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
//     Set the alpha channel of the given volume object to reflect the given
//     alpha and color key values.
//
// Arguments:
//
//     CVolume8* pVolume                    - Pointer to the volume object
//
//     PALETTECOLOR* ppcPalette             - Pointer to the first entry of
//                                            the palette for a palettized
//                                            surface
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
//     D3DCOLOR cColorKey                   - Color value of the pixels to
//                                            make transparent
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
static BOOL SetTranslucency(CVolume8* pVolume, PALETTECOLOR* ppcPalette, 
                            float fAlpha, D3DCOLOR cColorKey) 
{
    D3DVOLUME_DESC      d3dvd;
    D3DLOCKED_BOX       d3dlb;
    ARGBPIXELDESC       pixd;
    DWORD               dwRPitch, dwSPitch;
    DWORD               dwColorKey;
    UINT                i, j, k;
    HRESULT             hr;

    // Get the format of the volume
    hr = pVolume->GetDesc(&d3dvd);
    if (ResultFailed(hr, TEXT("IDirect3DVolume8::GetDesc"))) {
        return FALSE;
    }

    if (!(cColorKey == COLORKEY_NONE || cColorKey == COLORKEY_PIXEL1)) {
        // Get the surface-specific colorkey value (the representation of
        // the pixel in surface memory) from the red, green, and blue
        // components of the given D3DCOLOR
        dwColorKey = ColorToPixel(d3dvd.Format, ppcPalette, cColorKey & 0xFFFFFF);
    }

    // Lock the volume
    hr = pVolume->LockBox(&d3dlb, NULL, 0);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVolume8::LockBox"));
        return FALSE;
    }

    // If the texture is palettized, set the translucency in the palette entries
    if (d3dvd.Format == D3DFMT_P8) {

        CDevice8* pDevice;
        D3DCAPS8  d3dcaps;
        UINT      i;

        hr = pVolume->GetDevice(&pDevice);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDevice"))) {
            return FALSE;
        }

        hr = pDevice->GetDeviceCaps(&d3dcaps);
        pDevice->Release();
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
            return FALSE;
        }

        if (!ppcPalette || !(d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) {
            return FALSE;
        }

        for (i = 0; i < 256; i++) {
#ifndef UNDER_XBOX
            ppcPalette[i].peFlags = (BYTE)(fAlpha * 255.0f);
#else
            ppcPalette[i] = (ppcPalette[i] & 0x00FFFFFF) | ((BYTE)(fAlpha * 255.0f) << 24);
#endif // UNDER_XBOX
        }

        if (cColorKey == COLORKEY_PIXEL1) {
            dwColorKey = *(LPBYTE)d3dlb.pBits;
        }
        if (cColorKey != COLORKEY_NONE) {
#ifndef UNDER_XBOX
            ppcPalette[dwColorKey].peFlags = 0;
#else
            ppcPalette[dwColorKey] &= 0x00FFFFFF;
#endif // UNDER_XBOX
        }

        return TRUE;
    }

    // Get the pixel format of the volume
    GetARGBPixelDesc(d3dvd.Format, &pixd);

    if (cColorKey == COLORKEY_NONE) {
        dwColorKey = COLORKEY_NONE;
    }

    // Set the translucency
    switch (d3dvd.Format) {

#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8R8G8B8:
#endif // UNDER_XBOX
        case D3DFMT_A8R8G8B8:

            LPDWORD pdwPixel;
            DWORD   dwAlpha;

            dwAlpha = (DWORD)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask;
            pdwPixel = (LPDWORD)d3dlb.pBits;

            // Assume aligned pitch values (i.e. the quotients of the following
            // divisions will not have remainders)
            dwRPitch = d3dlb.RowPitch / pixd.uStride;
            dwSPitch = (d3dlb.SlicePitch - (d3dlb.RowPitch * d3dvd.Height)) / pixd.uStride;

            if (cColorKey == COLORKEY_PIXEL1) {
                dwColorKey = *pdwPixel;
            }

            for (i = 0; i < d3dvd.Depth; i++) {

                for (j = 0; j < d3dvd.Height; j++) {

                    for (k = 0; k < d3dvd.Width; k++) {

                        if ((*(pdwPixel + k) & ~pixd.dwAMask) == dwColorKey) {
                            *(pdwPixel + k) &= ~pixd.dwAMask;
                        }
                        else {
                            *(pdwPixel + k) = dwAlpha | (~pixd.dwAMask & *(pdwPixel + k));
                        }
                    }

                    pdwPixel += dwRPitch;
                }

                pdwPixel += dwSPitch;
            }

            break;

#ifdef UNDER_XBOX
        case D3DFMT_LIN_A1R5G5B5:
        case D3DFMT_LIN_A4R4G4B4:
#endif // UNDER_XBOX
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4: {

            LPWORD pwPixel;
            WORD   wAlpha;

            wAlpha = (WORD)((UINT)((float)pixd.dwAMask * fAlpha) & pixd.dwAMask);
            pwPixel = (LPWORD)d3dlb.pBits;

            // Assume aligned pitch values (i.e. the quotients of the following
            // divisions will not have remainders)
            dwRPitch = d3dlb.RowPitch / pixd.uStride;
            dwSPitch = (d3dlb.SlicePitch - (d3dlb.RowPitch * d3dvd.Height)) / pixd.uStride;

            if (cColorKey == COLORKEY_PIXEL1) {
                dwColorKey = *pwPixel;
            }

            for (i = 0; i < d3dvd.Depth; i++) {

                for (j = 0; j < d3dvd.Height; j++) {

                    for (k = 0; k < d3dvd.Width; k++) {

                        if ((*(pwPixel + k) & ~pixd.dwAMask) == dwColorKey) {
                            *(pwPixel + k) &= ~pixd.dwAMask;
                        }
                        else {
                            *(pwPixel + k) = wAlpha | (~(WORD)pixd.dwAMask & *(pwPixel + k));
                        }
                    }

                    pwPixel += dwRPitch;
                }

                pwPixel += dwSPitch;
            }

            break;
        }
    }

    // Unlock the surface
    hr = pVolume->UnlockBox();
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("IDirect3DVolume8::UnlockBox"));
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
//     Set the alpha channel of the given texture object to reflect the given
//     alpha and color key values.
//
// Arguments:
//
//     CBaseTexture8* pd3dtb                - Pointer to the texture object
//
//     PALETTECOLOR* ppcPalette             - Pointer to the first entry of
//                                            the palette for a palettized
//                                            texture
//
//     float fAlpha                         - Translucency value ranging from
//                                            0.0 to 1.0, where 0.0 is 
//                                            transparent and 1.0 is opaque
//
//     D3DCOLOR cColorKey                   - Color value of the pixels to
//                                            make transparent
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL SetTranslucency(CBaseTexture8* pd3dtb, PALETTECOLOR* ppcPalette, 
                        float fAlpha, D3DCOLOR cColorKey) 
{
    CDevice8*       pDevice;
    D3DCAPS8        d3dcaps;
    PTEXTUREDESC    ptexd;
    HRESULT         hr;
    BOOL            bRet = TRUE;

    // Parameter validation
    if (!pd3dtb) {
        return FALSE;
    }

    hr = pd3dtb->GetDevice(&pDevice);
    if (ResultFailed(hr, TEXT("IDirect3DResource8::GetDevice"))) {
        return FALSE;
    }

    hr = pDevice->GetDeviceCaps(&d3dcaps);
    pDevice->Release();
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return FALSE;
    }

    // Fail if the current device does not support translucency
    if (!(d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHA)) {
        bRet = FALSE;
    }

    ptexd = GetTextureDesc(pd3dtb);
    if (!ptexd) {
        return FALSE;
    }

    if (ptexd->tt & TTYPE_VOLUME) {

        CVolume8*        pVolume;
        CVolumeTexture8* pd3dtv = (CVolumeTexture8*)pd3dtb;

        hr = pd3dtv->GetVolumeLevel(0, &pVolume);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetVolumeLevel"));
            return FALSE;
        }

        if (!SetTranslucency(pVolume, ppcPalette, fAlpha, cColorKey)) {
            pVolume->Release();
            return FALSE;
        }

        pVolume->Release();
    }
    else if (ptexd->tt & TTYPE_CUBEMAP) {

        CSurface8*              pSurface;
        CCubeTexture8*          pd3dtc = (CCubeTexture8*)pd3dtb;
        UINT                    i;

        for (i = 0; i < 6; i++) {

            hr = pd3dtc->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pSurface);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetCubeMapSurface"));
                return FALSE;
            }

            if (!SetTranslucency(pSurface, ppcPalette, fAlpha, cColorKey)) {
                pSurface->Release();
                return FALSE;
            }

            pSurface->Release();
        }
    }
    else {

        CSurface8* pSurface;
        CTexture8* pd3dt = (CTexture8*)pd3dtb;

        hr = pd3dt->GetSurfaceLevel(0, &pSurface);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"));
            return FALSE;
        }

        if (!SetTranslucency(pSurface, ppcPalette, fAlpha, cColorKey)) {
            pSurface->Release();
            return FALSE;
        }

        pSurface->Release();
    }

    if (ptexd->tt & TTYPE_MIPMAP) {
        if (!FilterTexture(pd3dtb, ppcPalette)) {
            return FALSE;
        }
    }

    if (!SyncTexture(pd3dtb)) {
        bRet = FALSE;
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
//     Calculate the format-specific color value (the representation of
//     the pixel in surface/volume memory) from the red, green, and blue
//     components of the given D3DCOLOR.
//
// Arguments:
//
//     D3DFORMAT fmt                   - Resource format from which to
//                                       calculate the pixel value
//
//     PALETTECOLOR* ppcPalette        - Pointer to the first entry of
//                                       the palette for a palettized
//                                       texture
//
//     D3DCOLOR c                      - Color of the pixel
//
// Return Value:
//
//     The pixel representation in surface memory of the given color value on
//     success, zero on failure.
//
//******************************************************************************
DWORD ColorToPixel(D3DFORMAT fmt, PALETTECOLOR* ppcPalette, D3DCOLOR c) {

    DWORD dwPixel;
    BYTE  r, g, b, a;

    r = (BYTE)RGBA_GETRED(c);
    g = (BYTE)RGBA_GETGREEN(c);
    b = (BYTE)RGBA_GETBLUE(c);
    a = (BYTE)RGBA_GETALPHA(c);

    // If the format is palettized, select the nearest palette entry
    // to the given color
    if (fmt == D3DFMT_P8) {

        UINT                i;
        int                 dr, dg, db, d, dLast;

        if (!ppcPalette) {
            return 0;
        }

        dLast = INT_MAX;

        // Find the entry in the palette that most closely matches the
        // given pixel color
        for (i = 0; i < 256; i++) {

#ifndef UNDER_XBOX
            dr = (int)ppcPalette[i].peRed - r;
            dg = (int)ppcPalette[i].peGreen - g;
            db = (int)ppcPalette[i].peBlue - b;
#else
            dr = (int)RGBA_GETRED(ppcPalette[i]) - r;
            dg = (int)RGBA_GETGREEN(ppcPalette[i]) - g;
            db = (int)RGBA_GETBLUE(ppcPalette[i]) - b;
#endif // UNDER_XBOX

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
        GetARGBPixelDesc(fmt, &pixd);

        dwPixel = ((BYTE)((float)(pixd.dwAMask >> pixd.dwAShift) * ((float)a / 255.0f) + 0.5f) << pixd.dwAShift) |
                  ((BYTE)((float)(pixd.dwRMask >> pixd.dwRShift) * ((float)r / 255.0f) + 0.5f) << pixd.dwRShift) |
                  ((BYTE)((float)(pixd.dwGMask >> pixd.dwGShift) * ((float)g / 255.0f) + 0.5f) << pixd.dwGShift) |
                  ((BYTE)((float)(pixd.dwBMask >> pixd.dwBShift) * ((float)b / 255.0f) + 0.5f) << pixd.dwBShift);
    }

    return dwPixel;
}

//******************************************************************************
//
// Function:
//
//     CopyTexture
//
// Description:
//
//     Copy the contents of the given source texture into the given destination
//     texture using the given filter.  If the destination texture is a cube map 
//     and the source is not, the source texture will be copied into all faces 
//     of the cube map.  If the destination texture is a bump map and the source 
//     texture is not, the source texture is treated as a height map.  Only 
//     textures that are described in the texture table may be used as source or 
//     destination textures.
//
// Arguments:
//
//     CBaseTexture8* pd3dtbDst             - Pointer to the destination texture 
//                                            object
//
//     PALETTECOLOR* ppcPalDst              - Pointer to the first entry of
//                                            the destination palette
//
//     CBaseTexture8* pd3dtbSrc             - Pointer to the source texture 
//                                            object
//
//     PALETTECOLOR* ppcPalSrc              - Pointer to the first entry of
//                                            the source palette
//
//     DWORD dwFilter                       - Filter to apply during the copy
//
// Return Value:
//
//     An HRESULT indicating the result of the copy operation.
//
//******************************************************************************
HRESULT CopyTexture(CBaseTexture8* pd3dtbDst, PALETTECOLOR* ppcPalDst, 
                    CBaseTexture8* pd3dtbSrc, PALETTECOLOR* ppcPalSrc, 
                    DWORD dwFilter) 
{
    PTEXTUREDESC        ptexdDst, ptexdSrc;
    CSurface8*          pd3dsDst, *pd3dsSrc;
    CVolume8*           pd3dvDst, *pd3dvSrc;
    UINT                uLevelsDst, uLevelsSrc, i, j, k;
    PALETTEENTRY*       ppeDst = (PALETTEENTRY*)ppcPalDst, *ppeSrc = (PALETTEENTRY*)ppcPalSrc;
    HRESULT             hr;
#ifdef UNDER_XBOX
    PALETTEENTRY        ppeEntriesDst[256];
    PALETTEENTRY        ppeEntriesSrc[256];
#endif // UNDER_XBOX

    ptexdDst = GetTextureDesc(pd3dtbDst);
    ptexdSrc = GetTextureDesc(pd3dtbSrc);

    if (!ptexdDst || !ptexdSrc) {
        return E_INVALIDARG;
    }

#ifdef UNDER_XBOX
    if (ppcPalDst) {
        memcpy(ppeEntriesDst, ppcPalDst, 256 * sizeof(PALETTEENTRY));
        ConvertD3DCOLORToPALETTEENTRY(ppeEntriesDst, 256);
        ppeDst = ppeEntriesDst;
    }
    if (ppcPalSrc) {
        memcpy(ppeEntriesSrc, ppcPalSrc, 256 * sizeof(PALETTEENTRY));
        ConvertD3DCOLORToPALETTEENTRY(ppeEntriesSrc, 256);
        ppeSrc = ppeEntriesSrc;
    }
#endif // UNDER_XBOX

    if ((((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_TEXTURE) &&
         ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_TEXTURE)) ||
        (((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_BUMPMAP) &&
         ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_BUMPMAP)))
    {
        uLevelsDst = pd3dtbDst->GetLevelCount();
        uLevelsSrc = pd3dtbSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;
            hr = ((CTexture8*)pd3dtbDst)->GetSurfaceLevel(i, &pd3dsDst);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                return hr;
            }
            hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3dsSrc);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dsDst->Release();
                return hr;
            }
            hr = D3DXLoadSurfaceFromSurface(pd3dsDst->GetIDirect3DSurface8(), ppeDst, NULL,
                                            pd3dsSrc->GetIDirect3DSurface8(), ppeSrc, NULL,
                                            dwFilter, 0);

            pd3dsSrc->Release();
            pd3dsDst->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                return hr;
            }
        }

        if (!SyncTexture(pd3dtbDst)) {
            hr = E_FAIL;
        }

        return hr;
    }

    if ((((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_CUBEMAP) &&
         ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_TEXTURE)) ||
        (((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_CUBEMAP) &&
         ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_CUBEMAP)))
    {
        uLevelsDst = pd3dtbDst->GetLevelCount();
        uLevelsSrc = pd3dtbSrc->GetLevelCount();

        for (k = 0; k < 6; k++) {

            for (i = 0, j = 0; i < uLevelsDst; i++) {

                j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;
                hr = ((CCubeTexture8*)pd3dtbDst)->GetCubeMapSurface((D3DCUBEMAP_FACES)k, i, &pd3dsDst);
                if (ResultFailed(hr, TEXT("IDirect3DCubeMap8::GetSurfaceLevel"))) {
                    return hr;
                }
                if (ptexdSrc->tt & TTYPE_CUBEMAP) {
                    hr = ((CCubeTexture8*)pd3dtbSrc)->GetCubeMapSurface((D3DCUBEMAP_FACES)k, j, &pd3dsSrc);
                }
                else {
                    hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3dsSrc);
                }
                if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                    pd3dsDst->Release();
                    return hr;
                }
                hr = D3DXLoadSurfaceFromSurface(pd3dsDst->GetIDirect3DSurface8(), ppeDst, NULL,
                                                pd3dsSrc->GetIDirect3DSurface8(), ppeSrc, NULL,
                                                dwFilter, 0);

                pd3dsSrc->Release();
                pd3dsDst->Release();

                if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    return hr;
                }
            }
        }

        if (!SyncTexture(pd3dtbDst)) {
            hr = E_FAIL;
        }

        return hr;
    }

    if (((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_BUMPMAP) &&
        ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_TEXTURE))
    {
        CDevice8*           pDevice;
        CSurface8*          pd3dsH, *pd3dsB, *pd3ds;
        D3DSURFACE_DESC     d3dsd;
        D3DLOCKED_RECT      d3dlr;
        LPDWORD             pdwPixel;
        LPWORD              pwBump;
//        float               fOrder;
        float               *pfHeightMap, *pfHeight;
        D3DXVECTOR3         v1, v2, n;
        UINT                x, y;

        hr = pd3dtbDst->GetDevice(&pDevice);
        if (ResultFailed(hr, TEXT("IDirect3DResource8::GetDevice"))) {
            return hr;
        }

        uLevelsDst = pd3dtbDst->GetLevelCount();
        uLevelsSrc = pd3dtbSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;

            hr = ((CTexture8*)pd3dtbSrc)->GetLevelDesc(j, &d3dsd);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetDesc"))) {
                pDevice->Release();
                return hr;
            }

            // Create a 32 bit height map
#ifndef UNDER_XBOX
            if (ptexdSrc->fmt == D3DFMT_X8R8G8B8 ||
                ptexdSrc->fmt == D3DFMT_A8R8G8B8)
#else
            if (ptexdSrc->fmt == D3DFMT_LIN_X8R8G8B8 ||
                ptexdSrc->fmt == D3DFMT_LIN_A8R8G8B8)
#endif
            {
                hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3dsH);
                if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                    pDevice->Release();
                    return hr;
                }
            }
            else {

#ifndef UNDER_XBOX
                hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_A8R8G8B8, &pd3dsH);
#else
                hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_A8R8G8B8, &pd3dsH);
#endif
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                    pDevice->Release();
                    return hr;
                }
            
                hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3ds);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetSurfaceLevel"))) {
                    pd3dsH->Release();
                    pDevice->Release();
                    return hr;
                }

                hr = D3DXLoadSurfaceFromSurface(pd3dsH->GetIDirect3DSurface8(), NULL, NULL, pd3ds->GetIDirect3DSurface8(), ppeSrc, NULL, dwFilter, 0);
                pd3ds->Release();
                if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    pd3dsH->Release();
                    pDevice->Release();
                    return hr;
                }
            }

            // Create a 16 bit bump map
#ifndef UNDER_XBOX
            hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_V8U8, &pd3dsB);
#else
            hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_V8U8, &pd3dsB);
#endif
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                pd3dsH->Release();
                pDevice->Release();
                return hr;
            }

            // Convert the pixel intensities in the source image into height values
            // ranging from 0.0 to 1.0
            pfHeightMap = (float*)MemAlloc(d3dsd.Width * d3dsd.Height * sizeof(float));
            if (!pfHeightMap) {
                DebugString(TEXT("Memory allocation failed for height map"));
                pd3dsB->Release();
                pd3dsH->Release();
                pDevice->Release();
                return E_OUTOFMEMORY;
            }

            hr = pd3dsH->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                pd3dsH->Release();
                pDevice->Release();
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

                pdwPixel += d3dlr.Pitch >> 2;
            }

            pd3dsH->UnlockRect();
            pd3dsH->Release();
       
            // Calculate bump map offsets from the height map information
            hr = pd3dsB->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            pwBump = (LPWORD)d3dlr.pBits;
            v1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            v2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

            for (y = 0; y < d3dsd.Height; y++) {

                for (x = 0; x < d3dsd.Width; x++) {

                    // Tiled
                    pfHeight = &pfHeightMap[y * d3dsd.Width + x];
//                    fOrder = 1.0f;
                    if (y == d3dsd.Height - 1) {
                        v2.z = -(pfHeightMap[x] - *pfHeight);
                    }
                    else {
                        v2.z = -(pfHeight[d3dsd.Width] - pfHeight[0]);
                    }
                    if (x == d3dsd.Width - 1) {
                        v1.z = -(*(pfHeight - x) - *pfHeight);
                    }
                    else {
                        v1.z = -(pfHeight[1] - pfHeight[0]);
                    }
                    v1.x = (float)sqrt(1.0f - v1.z * v1.z);
                    v2.y = (float)sqrt(1.0f - v2.z * v2.z);
                    D3DXVec3Cross(&n, &v2, &v1);
                    D3DXVec3Normalize(&n, &n);
                    pwBump[x] = (int)(-n.y * 64.0f) << 8 |
                                (int)(n.x * 64.0f);

/*
                    // Non-tiled
                    pfHeight = &pfHeightMap[y * d3dsd.Width + x];
                    fOrder = 1.0f;
                    if (y == d3dsd.Height - 1) {
                        v2.z = *(pfHeight - d3dsd.Width) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v2.z = pfHeight[d3dsd.Width] - pfHeight[0];
                    }
                    if (x == d3dsd.Width - 1) {
                        v1.z = *(pfHeight - 1) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v1.z = pfHeight[1] - pfHeight[0];
                    }
                    v1.x = (float)sqrt(1.0f - v1.z * v1.z);
                    v2.y = -(float)sqrt(1.0f - v2.z * v2.z);
                    if (fOrder > 0.0f) {
                        D3DXVec3Cross(&n, &v1, &v2);
                    }
                    else {
                        D3DXVec3Cross(&n, &v2, &v1);
                    }
                    D3DXVec3Normalize(&n, &n);
                    pwBump[x] = (int)(-n.y * 64.0f) << 8 |
                                (int)(n.x * 64.0f);
*/
                }

                pwBump += d3dlr.Pitch >> 1;
            }

            MemFree(pfHeightMap);

            hr = pd3dsB->UnlockRect();
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"));
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            // Copy the bump map to the destination bump map
            hr = ((CTexture8*)pd3dtbDst)->GetSurfaceLevel(i, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            hr = D3DXLoadSurfaceFromSurface(pd3ds->GetIDirect3DSurface8(), ppeDst, NULL, pd3dsB->GetIDirect3DSurface8(), NULL, NULL, dwFilter, 0);

            pd3ds->Release();
            pd3dsB->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                pDevice->Release();
                return hr;
            }
        }

        pDevice->Release();

        if (!SyncTexture(pd3dtbDst)) {
            hr = E_FAIL;
        }

        return hr;
    }

    if (((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_NORMALMAP) &&
        ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_TEXTURE))
    {
        CDevice8*           pDevice;
        CSurface8*          pd3dsH, *pd3dsB, *pd3ds;
        D3DSURFACE_DESC     d3dsd;
        D3DLOCKED_RECT      d3dlr;
        LPDWORD             pdwPixel;
        LPDWORD             pdwNormal;
//        float               fOrder;
        float               *pfHeightMap, *pfHeight;
        D3DXVECTOR3         v1, v2, n;
        UINT                x, y;

        hr = pd3dtbDst->GetDevice(&pDevice);
        if (ResultFailed(hr, TEXT("IDirect3DResource8::GetDevice"))) {
            return hr;
        }

        uLevelsDst = pd3dtbDst->GetLevelCount();
        uLevelsSrc = pd3dtbSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;

            hr = ((CTexture8*)pd3dtbSrc)->GetLevelDesc(j, &d3dsd);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetDesc"))) {
                pDevice->Release();
                return hr;
            }

            // Create a 32 bit height map
#ifndef UNDER_XBOX
            if (ptexdSrc->fmt == D3DFMT_X8R8G8B8 ||
                ptexdSrc->fmt == D3DFMT_A8R8G8B8)
#else
            if (ptexdSrc->fmt == D3DFMT_LIN_X8R8G8B8 ||
                ptexdSrc->fmt == D3DFMT_LIN_A8R8G8B8)
#endif
            {
                hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3dsH);
                if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                    pDevice->Release();
                    return hr;
                }
            }
            else {

#ifndef UNDER_XBOX
                hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_A8R8G8B8, &pd3dsH);
#else
                hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_A8R8G8B8, &pd3dsH);
#endif
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                    pDevice->Release();
                    return hr;
                }
            
                hr = ((CTexture8*)pd3dtbSrc)->GetSurfaceLevel(j, &pd3ds);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetSurfaceLevel"))) {
                    pd3dsH->Release();
                    pDevice->Release();
                    return hr;
                }

                hr = D3DXLoadSurfaceFromSurface(pd3dsH->GetIDirect3DSurface8(), NULL, NULL, pd3ds->GetIDirect3DSurface8(), ppeSrc, NULL, dwFilter, 0);
                pd3ds->Release();
                if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                    pd3dsH->Release();
                    pDevice->Release();
                    return hr;
                }
            }

            // Create a 32 bit normal map
#ifndef UNDER_XBOX
            hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_A8R8G8B8, &pd3dsB);
#else
            hr = pDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_A8R8G8B8, &pd3dsB);
#endif
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                pd3dsH->Release();
                pDevice->Release();
                return hr;
            }

            // Convert the pixel intensities in the source image into height values
            // ranging from 0.0 to 1.0
            pfHeightMap = (float*)MemAlloc(d3dsd.Width * d3dsd.Height * sizeof(float));
            if (!pfHeightMap) {
                DebugString(TEXT("Memory allocation failed for height map"));
                pd3dsB->Release();
                pd3dsH->Release();
                pDevice->Release();
                return E_OUTOFMEMORY;
            }

            hr = pd3dsH->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                pd3dsH->Release();
                pDevice->Release();
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

                pdwPixel += d3dlr.Pitch >> 2;
            }

            pd3dsH->UnlockRect();
            pd3dsH->Release();
       
            // Calculate normal map vectors from the height map information
            hr = pd3dsB->LockRect(&d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"));
                MemFree(pfHeightMap);
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            pdwNormal = (LPDWORD)d3dlr.pBits;
            v1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            v2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

            for (y = 0; y < d3dsd.Height; y++) {

                for (x = 0; x < d3dsd.Width; x++) {

                    // Tiled
                    pfHeight = &pfHeightMap[y * d3dsd.Width + x];
//                    fOrder = 1.0f;
                    if (y == d3dsd.Height - 1) {
                        v2.z = -(pfHeightMap[x] - *pfHeight);
                    }
                    else {
                        v2.z = -(pfHeight[d3dsd.Width] - pfHeight[0]);
                    }
                    if (x == d3dsd.Width - 1) {
                        v1.z = -(*(pfHeight - x) - *pfHeight);
                    }
                    else {
                        v1.z = -(pfHeight[1] - pfHeight[0]);
                    }
                    v1.x = (float)sqrt(1.0f - v1.z * v1.z);
                    v2.y = (float)sqrt(1.0f - v2.z * v2.z);
                    D3DXVec3Cross(&n, &v2, &v1);
                    D3DXVec3Normalize(&n, &n);
                    pdwNormal[x] = VectorToColor(&n);

/*
                    // Non-tiled
                    pfHeight = &pfHeightMap[y * d3dsd.Width + x];
                    fOrder = 1.0f;
                    if (y == d3dsd.Height - 1) {
                        v2.z = *(pfHeight - d3dsd.Width) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v2.z = pfHeight[d3dsd.Width] - pfHeight[0];
                    }
                    if (x == d3dsd.Width - 1) {
                        v1.z = *(pfHeight - 1) - *pfHeight;
                        fOrder *= -1.0f;
                    }
                    else {
                        v1.z = pfHeight[1] - pfHeight[0];
                    }
                    v1.x = (float)sqrt(1.0f - v1.z * v1.z);
                    v2.y = (float)sqrt(1.0f - v2.z * v2.z);
                    if (fOrder > 0.0f) {
                        D3DXVec3Cross(&n, &v2, &v1);
                    }
                    else {
                        D3DXVec3Cross(&n, &v1, &v2);
                    }
                    D3DXVec3Normalize(&n, &n);
                    pdwNormal[x] = VectorToColor(&n);
*/
                }

                pdwNormal += d3dlr.Pitch >> 2;
            }

            MemFree(pfHeightMap);

            hr = pd3dsB->UnlockRect();
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DSurface8::UnlockRect"));
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            // Copy the normal map to the destination normal map
            hr = ((CTexture8*)pd3dtbDst)->GetSurfaceLevel(i, &pd3ds);
            if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
                pd3dsB->Release();
                pDevice->Release();
                return hr;
            }

            hr = D3DXLoadSurfaceFromSurface(pd3ds->GetIDirect3DSurface8(), ppeDst, NULL, pd3dsB->GetIDirect3DSurface8(), NULL, NULL, dwFilter, 0);

            pd3ds->Release();
            pd3dsB->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                pDevice->Release();
                return hr;
            }
        }

        pDevice->Release();

        if (!SyncTexture(pd3dtbDst)) {
            hr = E_FAIL;
        }

        return hr;
    }

    if (((ptexdDst->tt & TTYPE_RESOURCEMASK) == TTYPE_VOLUME) &&
        ((ptexdSrc->tt & TTYPE_RESOURCEMASK) == TTYPE_VOLUME))
    {
        uLevelsDst = pd3dtbDst->GetLevelCount();
        uLevelsSrc = pd3dtbSrc->GetLevelCount();

        for (i = 0, j = 0; i < uLevelsDst; i++) {

            j = (uLevelsDst - i >= uLevelsSrc) ? 0 : j + 1;
            hr = ((CVolumeTexture8*)pd3dtbDst)->GetVolumeLevel(i, &pd3dvDst);
            if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetSurfaceLevel"))) {
                return hr;
            }
            hr = ((CVolumeTexture8*)pd3dtbSrc)->GetVolumeLevel(j, &pd3dvSrc);
            if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::GetSurfaceLevel"))) {
                pd3dvDst->Release();
                return hr;
            }
            hr = D3DXLoadVolumeFromVolume(pd3dvDst->GetIDirect3DVolume8(), ppeDst, NULL,
                                            pd3dvSrc->GetIDirect3DVolume8(), ppeSrc, NULL,
                                            dwFilter, 0);

            pd3dvSrc->Release();
            pd3dvDst->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadVolumeFromVolume"))) {
                return hr;
            }
        }

        if (!SyncTexture(pd3dtbDst)) {
            hr = E_FAIL;
        }

        return hr;
    }

    return E_NOTIMPL;
}

//******************************************************************************
//
// Function:
//
//     CreateNormalMap
//
// Description:
//
//     Create a cube map and populate it with normal vector information.
//
// Arguments:
//
//     CDevice8* pDevice               - Pointer to the device object
//
//     UINT uLength                    - Dimensions of the map
//
//     float fBumpDelta                - Maximum component offset to randomly 
//                                       perturb the surface normals
//
//     D3DFORMAT fmt                   - Format of the texture
//
//     BOOL bMipmap                    - Indicates whether or not to
//                                       use a mip map
//
//     D3DPOOL pool                    - Memory pool in which to create the map
//
// Return Value:
//
//     A pointer to the created diffuse map on success, NULL on failure.
//
//******************************************************************************
CCubeTexture8* CreateNormalMap(CDevice8* pDevice, UINT uLength, float fBumpDelta, 
                                    D3DFORMAT fmt, BOOL bMipmap, D3DPOOL pool)
{
    CCubeTexture8*  pd3dtc;
    CCubeTexture8*  pd3dtcBump = NULL;
    TEXTURETYPE     ttype;
    UINT            uLevels;
    D3DLOCKED_RECT  d3dlr, d3dlrB;
    LPDWORD         pdwPixel, pdwPixelB;
    D3DXVECTOR3     vDir;
    float           fU, fV;
    UINT            i, j, u, v;
    HRESULT         hr;
#ifdef UNDER_XBOX
    Swizzler        swz(uLength, uLength, 1);
    BOOL            bSwizzled = XGIsSwizzledFormat(fmt);
#endif

    // Currently only support 32 bpp formats
#ifndef UNDER_XBOX
    if (!(fmt == D3DFMT_A8R8G8B8 || fmt == D3DFMT_X8R8G8B8)) {
#else
    if (!(fmt == D3DFMT_A8R8G8B8 || fmt == D3DFMT_X8R8G8B8 || fmt == D3DFMT_LIN_A8R8G8B8 || fmt == D3DFMT_LIN_X8R8G8B8)) {
#endif
        return NULL;
    }

    ttype = TTYPE_CUBEMAP;
    if (bMipmap) {
        ttype |= TTYPE_MIPMAP;
    }

    pd3dtc = (CCubeTexture8*)CreateTexture(pDevice, uLength, uLength, fmt, ttype, pool);
    if (!pd3dtc) {
        return NULL;
    }

    // If the normal map has multiple levels and a delta is to be applied to the normals,
    // generate the random offset values to be applied and store them in the highest
    // level of a second cube map.  Filter the offsets to the lower levels for consistent
    // transitions.
    if (bMipmap && fBumpDelta != 0.0f) {

        pd3dtcBump = (CCubeTexture8*)CreateTexture(pDevice, uLength, uLength, D3DFMT_A8R8G8B8, ttype);
        if (!pd3dtcBump) {
            ReleaseTexture(pd3dtc);
            return NULL;
        }

        // Populate the cube texture with random offsets to be applied to the normal map
        for (i = 0; i < 6; i++) {

            hr = pd3dtcBump->LockRect((D3DCUBEMAP_FACES)i, 0, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

#ifdef UNDER_XBOX
            swz.SetU(0);
            swz.SetV(0);
#endif

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

				    vDir = D3DXVECTOR3(SFRND(fBumpDelta), SFRND(fBumpDelta), SFRND(fBumpDelta));
#ifdef UNDER_XBOX
                    if (!bSwizzled) {
#endif
                        pdwPixel[u] = VectorToColor(&vDir);
#ifdef UNDER_XBOX
                    }
                    else {
                        pdwPixel[swz.Get2D()] = VectorToColor(&vDir);
                        swz.IncU();
                    }
#endif
                }

#ifdef UNDER_XBOX
                if (!bSwizzled) {
#endif
                    pdwPixel += (d3dlr.Pitch >> 2);
#ifdef UNDER_XBOX
                }
                else {
                    swz.IncV();
                }
#endif
            }

            hr = pd3dtcBump->UnlockRect((D3DCUBEMAP_FACES)i, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }
        }

        // Filter the offsets to the lower levels
        hr = D3DXFilterCubeTexture(pd3dtcBump->GetIDirect3DCubeTexture8(), NULL, 0, D3DX_FILTER_BOX);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("D3DXFilterCubeTexture"));
            ReleaseTexture(pd3dtcBump);
            ReleaseTexture(pd3dtc);
            return NULL;
        }
    }

    // Generate the normals
    uLevels = pd3dtc->GetLevelCount();

    for (i = 0; i < uLevels; i++) {

        for (j = 0; j < 6; j++) {

            hr = pd3dtc->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }

            if (pd3dtcBump) {
                hr = pd3dtcBump->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlrB, NULL, 0);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                    ReleaseTexture(pd3dtcBump);
                    ReleaseTexture(pd3dtc);
                    return NULL;
                }
                pdwPixelB = (LPDWORD)d3dlrB.pBits;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            if (uLevels > 1) {
                uLength = 0x1 << ((uLevels - 1) - i);
            }

#ifdef UNDER_XBOX
            Swizzler swz(uLength, uLength, 1);
            swz.SetU(0);
            swz.SetV(0);
#endif

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

                    if (uLength > 1) {
                        fU = (float)u / (float)(uLength - 1);
                        fV = (float)v / (float)(uLength - 1);
                    }
                    else {
                        fU = 0.5f;
                        fV = 0.5f;
                    }

                    switch ((D3DCUBEMAP_FACES)j) {
                        case D3DCUBEMAP_FACE_POSITIVE_X:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(1.0f, 1.0f - (fV * 2.0f), 1.0f - (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_X:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f, 1.0f - (fV * 2.0f), -1.0f + (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Y:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f, -1.0f + (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Y:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), -1.0f, 1.0f - (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Z:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f - (fV * 2.0f), 1.0f));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Z:
                            D3DXVec3Normalize(&vDir, &D3DXVECTOR3(1.0f - (fU * 2.0f), 1.0f - (fV * 2.0f), -1.0f));
                            break;
                    }
                    vDir = -vDir;
                    if (!pd3dtcBump) {
    				    vDir += D3DXVECTOR3(SFRND(fBumpDelta), SFRND(fBumpDelta), SFRND(fBumpDelta));
                    }
                    else {
                        vDir += ColorToVector(pdwPixelB[u]);
                    }
				    D3DXVec3Normalize(&vDir, &vDir);

#ifdef UNDER_XBOX
                    if (!bSwizzled) {
#endif
                        pdwPixel[u] = VectorToColor(&vDir);
#ifdef UNDER_XBOX
                    }
                    else {
                        pdwPixel[swz.Get2D()] = VectorToColor(&vDir);

                        swz.IncU();
                    }
#endif
                }

#ifdef UNDER_XBOX
                if (!bSwizzled) {
#endif
                    pdwPixel += (d3dlr.Pitch >> 2);
#ifdef UNDER_XBOX
                }
                else {
                    swz.IncV();
                }
#endif
                if (pd3dtcBump) {
                    pdwPixelB += (d3dlrB.Pitch >> 2);
                }
            }

            if (pd3dtcBump) {
                hr = pd3dtcBump->UnlockRect((D3DCUBEMAP_FACES)j, i);
                if (FAILED(hr)) {
                    ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                    if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                    ReleaseTexture(pd3dtc);
                    return NULL;
                }
            }

            hr = pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                if (pd3dtcBump) ReleaseTexture(pd3dtcBump);
                ReleaseTexture(pd3dtc);
                return NULL;
            }
        }
    }

    if (pd3dtcBump) {
        ReleaseTexture(pd3dtcBump);
    }

    return pd3dtc;
}

//******************************************************************************
//
// Function:
//
//     CreateHalfVectorMap
//
// Description:
//
//     Create a cube map and populate it with lookup information which,
//     given a normalized vector from a vertex to the viewer (supplied in the 
//     texture coordinates of the vertex), will return a normalized vector 
//     located halfway between the vertex-eye vector and the given light vector.
//     This normalized half-vector can then be used in specular lighting by
//     applying it in another stage with a normal cube map and 
//     D3DTOP_DOTPRODUCT3.
//
// Arguments:
//
//     CDevice8* pDevice               - Pointer to the device object
//
//     D3DMATRIX* pmWorld              - World transform
//
//     D3DVECTOR* pvLightDir           - Direction vector of the light
//
//     UINT uLength                    - Dimensions of the map
//
//     D3DFORMAT fmt                   - Format of the texture
//
//     BOOL bMipmap                    - Indicates whether or not to
//                                       use a mip map
//
//     D3DPOOL pool                    - Memory pool in which to create the map
//
// Return Value:
//
//     A pointer to the created diffuse map on success, NULL on failure.
//
//******************************************************************************
CCubeTexture8* CreateHalfVectorMap(CDevice8* pDevice, D3DVECTOR* pvLightDir, 
                                UINT uLength, D3DFORMAT fmt, BOOL bMipmap, 
                                D3DPOOL pool)
{
    CCubeTexture8*  pd3dtc;
    TEXTURETYPE     ttype;

    // Currently only support 32 bpp formats
    if (!(fmt == D3DFMT_A8R8G8B8 || fmt == D3DFMT_X8R8G8B8)) {
        return NULL;
    }

    ttype = TTYPE_CUBEMAP;
    if (bMipmap) {
        ttype |= TTYPE_MIPMAP;
    }

    pd3dtc = (CCubeTexture8*)CreateTexture(pDevice, uLength, uLength, fmt, ttype, pool);
    if (!pd3dtc) {
        return NULL;
    }

    if (!UpdateHalfVectorMap(pd3dtc, pvLightDir)) {
        ReleaseTexture(pd3dtc);
        return NULL;
    }

    return pd3dtc;
}

//******************************************************************************
BOOL UpdateHalfVectorMap(CCubeTexture8* pd3dtc, D3DVECTOR* pvLightDir) {

    D3DXVECTOR3     vLDir;
    UINT            uLength;
    UINT            uLevels;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    D3DXVECTOR3     vHalf;
    float           fU, fV;
    UINT            i, j, u, v;
    HRESULT         hr;
#ifdef UNDER_XBOX
    BOOL            bSwizzled;

    pd3dtc->GetLevelDesc(0, &d3dsd);    
    bSwizzled = XGIsSwizzledFormat(d3dsd.Format);
#endif

    // Normalize the light direction vector
    D3DXVec3Normalize(&vLDir, (LPD3DXVECTOR3)pvLightDir);
/*
    // Transform the light direction vector into model space
    // (multiply the vector by the transposed rotational component of the
    // world matrix) and negate it
    vL.x = -(vLDir.x * pmWorld->_11 + vLDir.y * pmWorld->_12 + vLDir.z * pmWorld->_13);
    vL.y = -(vLDir.x * pmWorld->_21 + vLDir.y * pmWorld->_22 + vLDir.z * pmWorld->_23);
    vL.z = -(vLDir.x * pmWorld->_31 + vLDir.y * pmWorld->_32 + vLDir.z * pmWorld->_33);
*/

    // Generate the half-vectors
    uLevels = pd3dtc->GetLevelCount();

    for (i = 0; i < uLevels; i++) {

        for (j = 0; j < 6; j++) {

            hr = pd3dtc->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlr, NULL, 0);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::LockRect"));
                return FALSE;
            }

            pdwPixel = (LPDWORD)d3dlr.pBits;

            if (uLevels > 1) {
                uLength = 0x1 << ((uLevels - 1) - i);
            }
            else {
                hr = pd3dtc->GetLevelDesc(0, &d3dsd);
                if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetLevelDesc"))) {
                    pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
                    return FALSE;
                }
                uLength = d3dsd.Width;
            }

#ifdef UNDER_XBOX
            Swizzler swz(uLength, uLength, 1);
            swz.SetU(0);
            swz.SetV(0);
#endif

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

                    if (uLength > 1) {
                        fU = (float)u / (float)(uLength - 1);
                        fV = (float)v / (float)(uLength - 1);
                    }
                    else {
                        fU = 0.5f;
                        fV = 0.5f;
                    }

                    switch ((D3DCUBEMAP_FACES)j) {
                        case D3DCUBEMAP_FACE_POSITIVE_X:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(1.0f, 1.0f - (fV * 2.0f), 1.0f - (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_X:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(-1.0f, 1.0f - (fV * 2.0f), -1.0f + (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Y:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f, -1.0f + (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Y:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(-1.0f + (fU * 2.0f), -1.0f, 1.0f - (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Z:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(-1.0f + (fU * 2.0f), 1.0f - (fV * 2.0f), 1.0f));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Z:
                            D3DXVec3Normalize(&vHalf, &D3DXVECTOR3(1.0f - (fU * 2.0f), 1.0f - (fV * 2.0f), -1.0f));
                            break;
                    }
                    D3DXVec3Normalize(&vHalf, &(vHalf + vLDir));
//                    vHalf = -vHalf;
#ifdef UNDER_XBOX
                    if (!bSwizzled) {
#endif
                        pdwPixel[u] = VectorToColor(&vHalf);
#ifdef UNDER_XBOX
                    }
                    else {
                        pdwPixel[swz.Get2D()] = VectorToColor(&vHalf);
                        swz.IncU();
                    }
#endif
                }

#ifdef UNDER_XBOX
                if (!bSwizzled) {
#endif
                    pdwPixel += (d3dlr.Pitch >> 2);
#ifdef UNDER_XBOX
                }
                else {
                    swz.IncV();
                }
#endif
            }

            hr = pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
            if (FAILED(hr)) {
                ResultFailed(hr, TEXT("IDirect3DCubeTexture8::UnlockRect"));
                return FALSE;
            }
        }
    }

    return TRUE;
}

//******************************************************************************
// Calculate non-normalized tangent and binormal vector terms for use in
// constructing an othonormal basis transform to rotate a vector from model
// space to tangent space.  This function assumes two vector arrays have been
// allocated, each with at least as many elements as three times the given 
// number of primitives.  These arrays will hold the calculated vector terms.
//******************************************************************************
BOOL CalculateTangentTerms(D3DXVECTOR3* pvTangent, D3DXVECTOR3* pvBinormal, 
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, BOOL bInterpolate)
{
    D3DXVECTOR3     vEdge0, vEdge1, vTemp;
    D3DXVECTOR3     vPlane[3];
    D3DXVECTOR3     *pvP[3];
    PTCOORD2        ptc2[3];
    WORD            w0, w1, w2;
    float           fTemp;
    UINT            uNumIndices;
    UINT            i, j, k;

    uNumIndices = 3 * dwNumPrimitives;

    memset(pvTangent, 0, uNumIndices * sizeof(D3DXVECTOR3));
    memset(pvBinormal, 0, uNumIndices * sizeof(D3DXVECTOR3));

    // Calculate the tangent and binormal vectors for each vertex.  If the vertex is
    // indexed by more than one triangle, add the vectors for each triangle together
    // to obtain an average of the vectors for all triangles formed by the vertex.
    for (i = 0; i < uNumIndices; i+=3) {

        w0 = pwIndices[i];
        w1 = pwIndices[i+1];
        w2 = pwIndices[i+2];

        pvP[0] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w0 * dwStridePosition);
        pvP[1] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w1 * dwStridePosition);
        pvP[2] =  (D3DXVECTOR3*)((LPBYTE)pvPosition + w2 * dwStridePosition);
        ptc2[0] = (PTCOORD2)((LPBYTE)ptcTCoord + w0 * dwStrideTCoord);
        ptc2[1] = (PTCOORD2)((LPBYTE)ptcTCoord + w1 * dwStrideTCoord);
        ptc2[2] = (PTCOORD2)((LPBYTE)ptcTCoord + w2 * dwStrideTCoord);

        vEdge0 = D3DXVECTOR3(pvP[1]->x - pvP[0]->x, ptc2[1]->u - ptc2[0]->u, ptc2[1]->v - ptc2[0]->v);
        vEdge1 = D3DXVECTOR3(pvP[2]->x - pvP[0]->x, ptc2[2]->u - ptc2[0]->u, ptc2[2]->v - ptc2[0]->v);
        D3DXVec3Cross(&vPlane[0], &vEdge0, &vEdge1);
        vEdge0.x = pvP[1]->y - pvP[0]->y;
        vEdge1.x = pvP[2]->y - pvP[0]->y;
        D3DXVec3Cross(&vPlane[1], &vEdge0, &vEdge1);
        vEdge0.x = pvP[1]->z - pvP[0]->z;
        vEdge1.x = pvP[2]->z - pvP[0]->z;
        D3DXVec3Cross(&vPlane[2], &vEdge0, &vEdge1);

        if (FABS(vPlane[0].x) < 0.000000001f || FABS(vPlane[1].x) < 0.000000001f || FABS(vPlane[2].x) < 0.000000001f) {
            return FALSE;
        }

        vTemp = D3DXVECTOR3(-vPlane[0].y / vPlane[0].x, -vPlane[1].y / vPlane[1].x, -vPlane[2].y / vPlane[1].x);
        pvTangent[w0] += vTemp;
        pvTangent[w1] += vTemp;
        pvTangent[w2] += vTemp;

        vTemp = D3DXVECTOR3(-vPlane[0].z / vPlane[0].x, -vPlane[1].z / vPlane[1].x, -vPlane[2].z / vPlane[1].x);
        pvBinormal[w0] += vTemp;
        pvBinormal[w1] += vTemp;
        pvBinormal[w2] += vTemp;
    }

    if (bInterpolate) {

        D3DXVECTOR3     *pv0, *pv1;
        LPWORD          pwMerge, pwProcessed;
        UINT            uNumMerges, uNumProcessed = 0;

        pwMerge = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwMerge) {
            return FALSE;
        }

        pwProcessed = (LPWORD)MemAlloc32(uNumIndices * sizeof(WORD));
        if (!pwProcessed) {
            MemFree32(pwMerge);
            return FALSE;
        }

        for (i = 0; i < uNumIndices; i++) {

            // Verify pwIndices[i] has to already been processed...if it has continue
            for (j = 0; j < uNumProcessed; j++) {
                if (pwIndices[i] == pwProcessed[j]) {
                    break;
                }
            }
            if (j < uNumProcessed) {
                continue;
            }

            pv0 = (D3DXVECTOR3*)((LPBYTE)pvPosition + pwIndices[i] * dwStridePosition);
            pwMerge[0] = pwIndices[i];
            pwProcessed[uNumProcessed++] = pwIndices[i];
            uNumMerges = 1;

            // Traverse the vertex list, identifying all vertices whose positions are
            // equal to the current vertex position
            for (j = i + 1; j < uNumIndices; j++) {

                pv1 = (D3DXVECTOR3*)((LPBYTE)pvPosition + pwIndices[j] * dwStridePosition);
                fTemp = D3DXVec3LengthSq(&(*pv0 - *pv1));
                if (fTemp < 0.0000001f) {

                    // See whether the matching vertex has already been added to the merge list
                    for (k = 0; k < uNumMerges; k++) {
                        if (pwIndices[j] == pwMerge[k]) {
                            break;
                        }
                    }
                    if (k == uNumMerges) {
                        pwMerge[uNumMerges++] = pwIndices[j];
                        pwProcessed[uNumProcessed++] = pwIndices[j];
                    }
                }
            }

            if (uNumMerges > 1 && uNumMerges < 5) {
                w0 = pwMerge[0];
                D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
                D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);
                for (j = 1; j < uNumMerges; j++) {
                    w1 = pwMerge[j];
                    D3DXVec3Normalize(&pvTangent[w1], &pvTangent[w1]);
                    D3DXVec3Normalize(&pvBinormal[w1], &pvBinormal[w1]);
                    pvTangent[w0] += pvTangent[w1];
                    pvBinormal[w0] += pvBinormal[w1];
                }
                for (j = 1; j < uNumMerges; j++) {
                    pvTangent[pwMerge[j]] = pvTangent[w0];
                    pvBinormal[pwMerge[j]] = pvBinormal[w0];
                }
            }
        }

        MemFree32(pwProcessed);
        MemFree32(pwMerge);
    }

    return TRUE;
}

//******************************************************************************
BOOL WorldVectorToTangentSpace(PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvVector, BOOL bInterpolate)
{
    return VectorToTangentSpace(FALSE, ptcTangent, dwStrideTangent, pvPosition,
                                dwStridePosition, ptcTCoord, dwStrideTCoord, 
                                pwIndices, dwNumPrimitives, d3dpt, pmWorld, 
                                pvVector, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
                                bInterpolate);
}

//******************************************************************************
BOOL HalfVectorToTangentSpace(PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate)
{
    return VectorToTangentSpace(FALSE, ptcTangent, dwStrideTangent, pvPosition,
                                dwStridePosition, ptcTCoord, dwStrideTCoord, 
                                pwIndices, dwNumPrimitives, d3dpt, pmWorld, 
                                pvLightDir, pvViewPos, bInterpolate);
}

//******************************************************************************
BOOL VectorToTangentSpace(BOOL bHalfVector, 
                                PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate)
{
    D3DXVECTOR3     vNormal, vLDir, vVPos, vEye, vTemp;
    D3DXVECTOR3     *pvTangent, *pvBinormal;
    D3DXVECTOR3     vPlane[3];
    PTCOORD3        ptc3;
    WORD            w0;
    D3DXMATRIX      mTransform;
    TCOORD3         tcZero;
    UINT            uNumIndices;
    UINT            i;
    BOOL            bWorldSpace;

#ifndef DEBUG
    // For now only indexed triangle lists are supported
    if (d3dpt != D3DPT_TRIANGLELIST || !pwIndices) {
        return FALSE;
    }
#endif // DEBUG

    tcZero.u = tcZero.v = tcZero.t = 0.0f;

    uNumIndices = 3 * dwNumPrimitives;

    D3DXMatrixIdentity(&mTransform);
    bWorldSpace = (BOOL)memcmp(&mTransform, pmWorld, sizeof(D3DMATRIX));

    // Allocate buffers to accumulate the averages of the tangent and
    // binormal terms for the basis transforms
    pvTangent = (D3DXVECTOR3*)MemAlloc32(2 * uNumIndices * sizeof(D3DXVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + uNumIndices;

    // Normalize the light direction
    D3DXVec3Normalize(&vTemp, (D3DXVECTOR3*)pvLightDir);

    // Transform the light vector from world space to model space (multiply the vector 
    // by the transposed rotational component of the world matrix) and negate it (for 
    // dotproduct calculations with the surface normals) 
    if (bWorldSpace) {
        vTemp = -vTemp;
        vLDir.x = vTemp.x * pmWorld->_11 + vTemp.y * pmWorld->_12 + vTemp.z * pmWorld->_13;
        vLDir.y = vTemp.x * pmWorld->_21 + vTemp.y * pmWorld->_22 + vTemp.z * pmWorld->_23;
        vLDir.z = vTemp.x * pmWorld->_31 + vTemp.y * pmWorld->_32 + vTemp.z * pmWorld->_33;
    }
    else {
        vLDir = -vTemp;
    }

    // First set all texture coordinates corresponding to the vectors in tangent 
    // space to zero
    for (i = 0; i < uNumIndices; i++) {
        memcpy((LPBYTE)ptcTangent + pwIndices[i] * dwStrideTangent, &tcZero, sizeof(TCOORD3));
    }

    if (!CalculateTangentTerms(pvTangent, pvBinormal, pvPosition, dwStridePosition, 
                                ptcTCoord, dwStrideTCoord, pwIndices, 
                                dwNumPrimitives, d3dpt, bInterpolate))
    {
        MemFree32(pvTangent);
        return FALSE;
    }

    for (i = 0; i < uNumIndices; i++) {

        w0 = pwIndices[i];
        ptc3 = (PTCOORD3)((LPBYTE)ptcTangent + w0 * dwStrideTangent);
        if (!memcmp(ptc3, &tcZero, sizeof(TCOORD3))) {

//            pvN = (D3DXVECTOR3*)((LPBYTE)pvNormal + w0 * dwStrideNormal);

            D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            D3DXVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

//            fDot = D3DXVec3Dot(&vNormal, pvN);
//            if (fDot < 0.0f) {
//                vNormal = -vNormal;
//            }

            if (bHalfVector) {

                vTemp = *(D3DXVECTOR3*)pvViewPos - *((D3DXVECTOR3*)((LPBYTE)pvPosition + w0 * dwStridePosition));
                if (bWorldSpace) {
                    vEye.x = vTemp.x * pmWorld->_11 + vTemp.y * pmWorld->_12 + vTemp.z * pmWorld->_13;
                    vEye.y = vTemp.x * pmWorld->_21 + vTemp.y * pmWorld->_22 + vTemp.z * pmWorld->_23;
                    vEye.z = vTemp.x * pmWorld->_31 + vTemp.y * pmWorld->_32 + vTemp.z * pmWorld->_33;
                    D3DXVec3Normalize(&vEye, &vEye);
                }
                else {
                    D3DXVec3Normalize(&vEye, &vTemp);
                }
                vTemp = vEye + vLDir;
D3DXVec3Normalize(&vTemp, &vTemp);
                ptc3->u = vTemp.x * pvTangent[w0].x + vTemp.y * pvTangent[w0].y + vTemp.z * pvTangent[w0].z;
                ptc3->v = vTemp.x * pvBinormal[w0].x + vTemp.y * pvBinormal[w0].y + vTemp.z * pvBinormal[w0].z;
                ptc3->t = vTemp.x * vNormal.x + vTemp.y * vNormal.y + vTemp.z * vNormal.z;
            }
            else {
                ptc3->u = vLDir.x * pvTangent[w0].x + vLDir.y * pvTangent[w0].y + vLDir.z * pvTangent[w0].z;
                ptc3->v = vLDir.x * pvBinormal[w0].x + vLDir.y * pvBinormal[w0].y + vLDir.z * pvBinormal[w0].z;
                ptc3->t = vLDir.x * vNormal.x + vLDir.y * vNormal.y + vLDir.z * vNormal.z;
            }
        }
    }

    MemFree32(pvTangent);

    return TRUE;
}

//******************************************************************************
BOOL SetTangentTransforms(PMATRIX3X3 pmTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld,
                                BOOL bInterpolate)
{
    D3DXVECTOR3     vNormal, vTemp;
    D3DXVECTOR3     *pvTangent, *pvBinormal;
    DWORD           dwNumIndices;
    WORD            w0;
    PMATRIX3X3      pmT;
//    float           fDot;
    MATRIX3X3       mZero;
	UINT            i;

    dwNumIndices = dwNumPrimitives * 3;

    memset(&mZero, 0, sizeof(MATRIX3X3));

    for (i = 0; i < dwNumIndices; i++) {
        memcpy((LPBYTE)pmTangent + pwIndices[i] * dwStrideTangent, &mZero, sizeof(MATRIX3X3));
    }

    pvTangent = (D3DXVECTOR3*)MemAlloc32(2 * dwNumIndices * sizeof(D3DXVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + dwNumIndices;

    if (!CalculateTangentTerms(pvTangent, pvBinormal, pvPosition, dwStridePosition, ptcTCoord, dwStrideTCoord, pwIndices, dwNumPrimitives, d3dpt, bInterpolate)) {
        MemFree32(pvTangent);
        return FALSE;
    }

    for (i = 0; i < dwNumIndices; i++) {

        w0 = pwIndices[i];
        pmT = (PMATRIX3X3)((LPBYTE)pmTangent + w0 * dwStrideTangent);
        if (!memcmp(pmT, &mZero, sizeof(MATRIX3X3))) {

//            pvN = (D3DXVECTOR3*)((LPBYTE)pvNormal + w0 * dwStrideNormal);

            D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            D3DXVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

//            fDot = D3DXVec3Dot(&vNormal, pvN);
//            if (fDot < 0.0f) {
//                // Either negate vNormal or pvBinormal...
//                vNormal = -vNormal;
//            }

            pmT->_11 = -pvTangent[w0].x;
            pmT->_21 = -pvTangent[w0].y;
            pmT->_31 = -pvTangent[w0].z;

            pmT->_12 = -pvBinormal[w0].x;
            pmT->_22 = -pvBinormal[w0].y;
            pmT->_32 = -pvBinormal[w0].z;

            // Negate the normal vector so the light or half-vector
            // transformed by the basis does not itself have to
            // be negated
//            pmT->_13 = -vNormal.x;
//            pmT->_23 = -vNormal.y;
//            pmT->_33 = -vNormal.z;
pmT->_13 = vNormal.x;
pmT->_23 = vNormal.y;
pmT->_33 = vNormal.z;
        }
    }

    MemFree32(pvTangent);

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     CreateDiffuseMap
//
// Description:
//
//     Create a diffuse light map for use in diffuse bump map operations.
//
// Arguments:
//
//     CDevice8* pDevice               - Pointer to the device object
//
//     UINT uWidth                     - Width of the map
//
//     UINT uHeight                    - Height of the map
//
//     D3DCOLOR c                      - Color for the diffuse light
//
//     D3DFORMAT fmt                   - Format of the texture
//
//     BOOL bMipmap                    - Indicates whether or not to
//                                       use a mip map
//
//     PALETTECOLO* ppcPalette        - Pointer to the first entry of
//                                       the palette for a palettized
//                                       texture
//
//     D3DPOOL pool                    - Memory pool in which to create the map
//
// Return Value:
//
//     A pointer to the created diffuse map on success, NULL on failure.
//
//******************************************************************************
CTexture8* CreateDiffuseMap(CDevice8* pDevice, UINT uWidth, UINT uHeight, 
                                    D3DCOLOR c, D3DFORMAT fmt, BOOL bMipmap, 
                                    PALETTECOLOR* ppcPalette, D3DPOOL pool)
{
    CTexture8*          pd3dt;
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
#ifdef UNDER_XBOX
    BOOL                bSwizzled = XGIsSwizzledFormat(fmt);
#endif

    pd3dt = (CTexture8*)CreateTexture(pDevice, uWidth, uHeight, fmt, bMipmap ? TTYPE_MIPMAP : TTYPE_TEXTURE, pool);
    if (!pd3dt) {
        return NULL;
    }

    hr = ((CTexture8*)pd3dt)->GetLevelDesc(0, &d3dsd);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }
    fmtt = d3dsd.Format;

    dwDepth = FormatToBitDepth(fmtt);
    if (!(dwDepth == 16 || dwDepth == 32)) {

        CTexture8*          pd3dtMap;
        D3DFORMAT           fmtMap[] = {
                                D3DFMT_X8R8G8B8,
                                D3DFMT_A8R8G8B8,
                                D3DFMT_R5G6B5,
                                D3DFMT_A1R5G5B5,
                                D3DFMT_X1R5G5B5,
                                D3DFMT_A4R4G4B4
                            };

        for (i = 0; i < 6; i++) {
#ifndef UNDER_XBOX
            pd3dtMap = (CTexture8*)CreateDiffuseMap(pDevice, uWidth, uHeight, 
                        c, fmtMap[i], bMipmap, NULL, D3DPOOL_SYSTEMMEM);
#else
            pd3dtMap = (CTexture8*)CreateDiffuseMap(pDevice, uWidth, uHeight, 
                        c, fmtMap[i], bMipmap, NULL, POOL_DEFAULT);
#endif // UNDER_XBOX
            if (pd3dtMap) {
                hr = CopyTexture(pd3dt, ppcPalette, pd3dtMap, NULL, 
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

        hr = pd3dt->GetLevelDesc(i, &d3dsd);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        // Get the pixel format of the texture
        GetARGBPixelDesc(d3dsd.Format, &pixd);

        hr = pd3dt->LockRect(i, &d3dlr, NULL, 0);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"));
            ReleaseTexture(pd3dt);
            return NULL;
        }

#ifdef UNDER_XBOX
        Swizzler swz(d3dsd.Width, d3dsd.Height, 1);
        swz.SetU(0);
        swz.SetV(0);
#endif

        switch (d3dsd.Format) {

#ifdef UNDER_XBOX
            case D3DFMT_LIN_X8R8G8B8:
            case D3DFMT_LIN_A8R8G8B8:
#endif
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
#ifdef UNDER_XBOX
                        if (!bSwizzled) {
#endif
                            pdwPixel[k] = 0xFF                        << pixd.dwAShift |
                                          (BYTE)(fIntensity * fRed)   << pixd.dwRShift |
                                          (BYTE)(fIntensity * fGreen) << pixd.dwGShift |
                                          (BYTE)(fIntensity * fBlue)  << pixd.dwBShift;
#ifdef UNDER_XBOX
                        }
                        else {
                            pdwPixel[swz.Get2D()] = 0xFF              << pixd.dwAShift |
                                          (BYTE)(fIntensity * fRed)   << pixd.dwRShift |
                                          (BYTE)(fIntensity * fGreen) << pixd.dwGShift |
                                          (BYTE)(fIntensity * fBlue)  << pixd.dwBShift;
                            swz.IncU();
                        }
#endif
                    }

#ifdef UNDER_XBOX
                    if (!bSwizzled) {
#endif
                        pdwPixel += (d3dlr.Pitch / pixd.uStride);
#ifdef UNDER_XBOX
                    }
                    else {
                        swz.IncV();
                    }
#endif
                }

                break;
            }

#ifdef UNDER_XBOX
            case D3DFMT_LIN_R5G6B5:
            case D3DFMT_LIN_X1R5G5B5:
            case D3DFMT_LIN_A1R5G5B5:
            case D3DFMT_LIN_A4R4G4B4:
#endif
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

#ifdef UNDER_XBOX
                        if (!bSwizzled) {
#endif
                            pwPixel[k] = (WORD)(pixd.dwAMask |
                                          ((UINT)(fIntensity * fRed)   << pixd.dwRShift) & pixd.dwRMask |
                                          ((UINT)(fIntensity * fGreen) << pixd.dwGShift) & pixd.dwGMask |
                                          ((UINT)(fIntensity * fBlue)  << pixd.dwBShift) & pixd.dwBMask);
#ifdef UNDER_XBOX
                        }
                        else {
                            pwPixel[swz.Get2D()] = (WORD)(pixd.dwAMask |
                                          ((UINT)(fIntensity * fRed)   << pixd.dwRShift) & pixd.dwRMask |
                                          ((UINT)(fIntensity * fGreen) << pixd.dwGShift) & pixd.dwGMask |
                                          ((UINT)(fIntensity * fBlue)  << pixd.dwBShift) & pixd.dwBMask);
                            swz.IncU();
                        }
#endif
                    }

#ifdef UNDER_XBOX
                    if (!bSwizzled) {
#endif
                        pwPixel += (d3dlr.Pitch / pixd.uStride);
#ifdef UNDER_XBOX
                    }
                    else {
                        swz.IncV();
                    }
#endif
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
//
// Function:
//
//     SetDiffuseMapCoords
//
// Description:
//
//     Calculate the texture coordinates to index into a diffuse light map
//     based on the given world transform, light direction, normal values of the
//     vertices, and rotation angle.
//
// Arguments:
//
//     D3DVECTOR* pvNormal             - Pointer to the first normal value in
//                                       the vertex list
//
//     UINT uNStride                   - Stride between normals in the vertex
//                                       list
//
//     PTCOORD2 ptcTCoord              - Pointer to the first light map texture 
//                                       coordinate set in the vertex list
//
//     UINT uTStride                   - Stride between coordinate sets in the
//                                       vertex list
//
//     UINT uVertices                  - Number of vertices in the vertex list
//
//     D3DMATRIX* pmWorld              - World transform
//
//     D3DVECTOR* pvLightDir           - Direction vector of the light
//
//     float fTheta                    - Rotation angle of the light relative
//                                       to the viewer
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************

//******************************************************************************
BOOL SetDiffuseMapCoords(PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, BOOL bInterpolate)
{
    return SetLightMapCoords(FALSE, ptcLightMap, dwStrideLMap, pvPosition,
                                dwStridePosition, ptcOffsetMap, dwStrideOMap,
                                pwIndices, dwNumPrimitives, d3dpt, pmWorld,
                                pvLightDir, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
                                bInterpolate);
}

//******************************************************************************
BOOL SetSpecularMapCoords(PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate)
{
    return SetLightMapCoords(TRUE, ptcLightMap, dwStrideLMap, pvPosition,
                                dwStridePosition, ptcOffsetMap, dwStrideOMap,
                                pwIndices, dwNumPrimitives, d3dpt, pmWorld,
                                pvLightDir, pvViewPos, bInterpolate);
}

//******************************************************************************
BOOL SetLightMapCoords(BOOL bSpecular, PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate)
{
    D3DXVECTOR3     vLDir, vVPos, vEye, vTemp;
    D3DXVECTOR3     *pvTangent, *pvBinormal;
    D3DXVECTOR3     vNormal, vLTangent;
    D3DXVECTOR3     vPlane[3];
    PTCOORD2        ptcLight;
    WORD            w0;
    D3DXMATRIX      mTransform;
    TCOORD2         tcSet;
    float           fNU, fNV, fMagUV, fMagXY;
    UINT            uNumIndices;
    UINT            i;
    BOOL            bWorldSpace;

#ifndef DEBUG
    // For now only indexed triangle lists are supported
    if (d3dpt != D3DPT_TRIANGLELIST || !pwIndices) {
        return FALSE;
    }
#endif // DEBUG

    tcSet.u = tcSet.v = -1.0f;

    uNumIndices = 3 * dwNumPrimitives;

    D3DXMatrixIdentity(&mTransform);
    bWorldSpace = (BOOL)memcmp(&mTransform, pmWorld, sizeof(D3DMATRIX));

    // Transform the light vector from world space to model space (multiply the vector 
    // by the transposed rotational component of the world matrix)
    if (bWorldSpace) {
        D3DXVec3Normalize(&vTemp, (D3DXVECTOR3*)pvLightDir);
        vLDir.x = vTemp.x * pmWorld->_11 + vTemp.y * pmWorld->_12 + vTemp.z * pmWorld->_13;
        vLDir.y = vTemp.x * pmWorld->_21 + vTemp.y * pmWorld->_22 + vTemp.z * pmWorld->_23;
        vLDir.z = vTemp.x * pmWorld->_31 + vTemp.y * pmWorld->_32 + vTemp.z * pmWorld->_33;
    }
    else {
        D3DXVec3Normalize(&vLDir, (D3DXVECTOR3*)pvLightDir);
    }

    // First set all texture coordinates corresponding to the vectors in tangent 
    // space to a degenerate direction
    for (i = 0; i < uNumIndices; i++) {
        memcpy((LPBYTE)ptcLightMap + pwIndices[i] * dwStrideLMap, &tcSet, sizeof(TCOORD2));
    }

    pvTangent = (D3DXVECTOR3*)MemAlloc32(2 * uNumIndices * sizeof(D3DXVECTOR3));
    if (!pvTangent) {
        return FALSE;
    }
    pvBinormal = pvTangent + uNumIndices;

    if (!CalculateTangentTerms(pvTangent, pvBinormal, pvPosition, dwStridePosition, ptcOffsetMap, dwStrideOMap, pwIndices, dwNumPrimitives, d3dpt, bInterpolate)) {
        MemFree32(pvTangent);
        return FALSE;
    }

    for (i = 0; i < uNumIndices; i++) {

        w0 = pwIndices[i];
        ptcLight = (PTCOORD2)((LPBYTE)ptcLightMap + w0 * dwStrideLMap);
        if (!memcmp(ptcLight, &tcSet, sizeof(TCOORD2))) {

//            pvN = (D3DXVECTOR3*)((LPBYTE)pvNormal + w0 * dwStrideNormal);

            D3DXVec3Normalize(&pvTangent[w0], &pvTangent[w0]);
            D3DXVec3Normalize(&pvBinormal[w0], &pvBinormal[w0]);

            D3DXVec3Cross(&vNormal, &pvTangent[w0], &pvBinormal[w0]);

//            fDot = D3DXVec3Dot(&vNormal, pvN);
//            if (fDot < 0.0f) {
//                vNormal = -vNormal;
//            }

            if (bSpecular) {

                vTemp = *(D3DXVECTOR3*)pvViewPos - *((D3DXVECTOR3*)((LPBYTE)pvPosition + w0 * dwStridePosition));
                if (bWorldSpace) {
                    vEye.x = vTemp.x * pmWorld->_11 + vTemp.y * pmWorld->_12 + vTemp.z * pmWorld->_13;
                    vEye.y = vTemp.x * pmWorld->_21 + vTemp.y * pmWorld->_22 + vTemp.z * pmWorld->_23;
                    vEye.z = vTemp.x * pmWorld->_31 + vTemp.y * pmWorld->_32 + vTemp.z * pmWorld->_33;
                    D3DXVec3Normalize(&vEye, &vEye);
                }
                else {
                    D3DXVec3Normalize(&vEye, &vTemp);
                }
                vTemp = vEye + vLDir;
//D3DXVec3Normalize(&vTemp, &vTemp);
                vLTangent.x = -(vTemp.x * pvTangent[w0].x + vTemp.y * pvTangent[w0].y + vTemp.z * pvTangent[w0].z);
                vLTangent.y = -(vTemp.x * pvBinormal[w0].x + vTemp.y * pvBinormal[w0].y + vTemp.z * pvBinormal[w0].z);
                vLTangent.z = (vTemp.x * vNormal.x + vTemp.y * vNormal.y + vTemp.z * vNormal.z);

                // Find the tangent vector offsets in the light map
                fMagUV = (float)acos(vLTangent.z) / M_PI;

                if (fMagUV < 0.0001f) {
                    fNU = 0.5f;
                    fNV = 0.5f;
                }
                else if (fMagUV > 0.9999) {
                    fNU = 0.0f;
                    fNV = 0.5f;
                }
                else {
                    fMagXY = (float)sqrt(vLTangent.x * vLTangent.x + vLTangent.y * vLTangent.y);
                    fNU = vLTangent.x / fMagXY;
                    fNV = vLTangent.y / fMagXY;
   
                    fNU *= fMagUV;
                    fNV *= fMagUV;

                    fNU = 0.5f + (fNU / 2.0f);
                    fNV = 0.5f - (fNV / 2.0f);
                }

                ptcLight->u = fNU;
                ptcLight->v = fNV;
            }
            else {
                vLTangent.x = -(vLDir.x * pvTangent[w0].x + vLDir.y * pvTangent[w0].y + vLDir.z * pvTangent[w0].z);
                vLTangent.y = -(vLDir.x * pvBinormal[w0].x + vLDir.y * pvBinormal[w0].y + vLDir.z * pvBinormal[w0].z);
                vLTangent.z = (vLDir.x * vNormal.x + vLDir.y * vNormal.y + vLDir.z * vNormal.z);

                // Find the tangent vector offsets in the light map
                fMagUV = (float)acos(vLTangent.z) / M_PI;

                if (fMagUV < 0.0001f) {
                    fNU = 0.5f;
                    fNV = 0.5f;
                }
                else if (fMagUV > 0.9999) {
                    fNU = 0.0f;
                    fNV = 0.5f;
                }
                else {
                    fMagXY = (float)sqrt(vLTangent.x * vLTangent.x + vLTangent.y * vLTangent.y);
                    fNU = vLTangent.x / fMagXY;
                    fNV = vLTangent.y / fMagXY;
           
                    fNU *= fMagUV;
                    fNV *= fMagUV;

                    fNU = 0.5f + (fNU / 2.0f);
                    fNV = 0.5f - (fNV / 2.0f);
                }

                ptcLight->u = fNU;
                ptcLight->v = fNV;
            }
        }
    }

    MemFree32(pvTangent);

    return TRUE;
}

//******************************************************************************
void ConvertPALETTEENTRYToD3DCOLOR(LPVOID pvPalette, UINT uNumEntries) {

    PALETTEENTRY* ppe = (PALETTEENTRY*)pvPalette;
    D3DCOLOR c;
    UINT i;

    if (!ppe) {
        return;
    }

    for (i = 0; i < uNumEntries; i++) {
        c = D3DCOLOR_ARGB(ppe[i].peFlags, ppe[i].peRed, ppe[i].peGreen, ppe[i].peBlue);
        ppe[i] = *(PALETTEENTRY*)&c;
    }
}

//******************************************************************************
void ConvertD3DCOLORToPALETTEENTRY(LPVOID pvPalette, UINT uNumEntries) {

    D3DCOLOR* pc = (D3DCOLOR*)pvPalette;
    PALETTEENTRY e;
    UINT i;

    if (!pc) {
        return;
    }

    for (i = 0; i < uNumEntries; i++) {
        e.peRed = (BYTE)RGBA_GETRED(pc[i]);
        e.peGreen = (BYTE)RGBA_GETGREEN(pc[i]);
        e.peBlue = (BYTE)RGBA_GETBLUE(pc[i]);
        e.peFlags = (BYTE)RGBA_GETALPHA(pc[i]);
        pc[i] = *(D3DCOLOR*)&e;
    }
}

#ifdef UNDER_XBOX

//******************************************************************************
BOOL GetBitmapInfoHeader(LPCTSTR szTexture, BITMAPINFOHEADER* pbmih) {

    HANDLE                  hFile;
    PBYTE                   pdata;
    DWORD                   dwSize, dwRead;
	LPWORD				    pwDst;
    DWORD                   dwPitchDst;
    UNALIGNED BITMAPINFO*   pbmi;
    UINT                    i, j;
    HRESULT                 hr;
    char                    aszTexture[MAX_PATH];

    if (LoadResourceData(szTexture, (LPVOID*)&pdata, &dwSize)) {
        pbmi = (BITMAPINFO*)(pdata + sizeof(BITMAPFILEHEADER));
        memcpy(pbmih, &pbmi->bmiHeader, sizeof(BITMAPINFOHEADER));
        UnloadResourceData(szTexture);
        return TRUE;
    }

    wcstombs(aszTexture, szTexture, MAX_PATH);

    // Open the bitmap file
    hFile = CreateFile(aszTexture, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DebugString(TEXT("File %s not found"), szTexture);
        return FALSE;
    }

    // Get the size of the file
    dwSize = GetFileSize(hFile, NULL);
    if (dwSize == 0xFFFFFFFF) {
        CloseHandle(hFile);
        return FALSE;
    }

    // Allocate a buffer for the file data
    pdata = new BYTE[dwSize];
    if (!pdata) {
        CloseHandle(hFile);
        return FALSE;
    }                

    // Read the file into the buffer
    if (!ReadFile(hFile, pdata, dwSize, &dwRead, NULL)
        || dwSize != dwRead)
    {
        delete [] pdata;
        CloseHandle(hFile);
        return FALSE;
    }

    // Close the file
    CloseHandle(hFile);

    // Verify the given file is a bitmap file
    if (((UNALIGNED BITMAPFILEHEADER*)pdata)->bfType != 0x4D42) { // 0x4D42 == "BM"
        DebugString(TEXT("%s is not a valid bitmap file"), szTexture);
        delete [] pdata;
        return FALSE;
    }

    pbmi = (BITMAPINFO*)(pdata + sizeof(BITMAPFILEHEADER));

    memcpy(pbmih, &pbmi->bmiHeader, sizeof(BITMAPINFOHEADER));

    // Delete the bitmap file buffer
    delete [] pdata;

    return TRUE;
}

#endif // UNDER_XBOX

//******************************************************************************
void VerifyTextureListIntegrity() {

    PTEXTUREDESC ptexd;
    UINT uWidth;

    for (ptexd = g_ptexdList; ptexd; ptexd = ptexd->ptexdNext) {
        uWidth = ptexd->uWidth;  // Verify the node
        ptexd->pd3dtb->AddRef(); // Verify the texture
        ptexd->pd3dtb->Release();
    }
}

