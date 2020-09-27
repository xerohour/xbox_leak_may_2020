/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    texture.h

Author:

    Matt Bronder

Description:

    Direct3D texture routines.

*******************************************************************************/

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define COLORKEY_NONE       0xFF000000
#define COLORKEY_PIXEL1     0x01000000

#define TTYPE_TEXTURE       0x00000000
#define TTYPE_CUBEMAP       0x00000001
#define TTYPE_VOLUME        0x00000002
#define TTYPE_BUMPMAP       0x00000004
#define TTYPE_NORMALMAP     0x00000008
#define TTYPE_MIPMAP        0x00000100
#define TTYPE_TARGET        0x00000200
#define TTYPE_DEPTH         0x00000400
#define TTYPE_RESOURCEMASK  0x000000FF
#define TTYPE_MIPLEVELMASK  0x0F000000

#define TTYPE_MIPLEVELS(x)  (TTYPE_MIPMAP | ((x) << 24))

#ifndef UNDER_XBOX
#define PALETTECOLOR        PALETTEENTRY
#else
#define PALETTECOLOR        D3DCOLOR
#endif // UNDER_XBOX

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD               TEXTURETYPE;

//******************************************************************************
// Structures
//******************************************************************************

//******************************************************************************
typedef struct _TEXTUREDESC {
    CBaseTexture8*          pd3dtb;
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

#ifdef UNDER_XBOX

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD FAR* LPRGBQUAD;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO, FAR *LPBITMAPINFO, *PBITMAPINFO;

#include <pshpack2.h>
typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#include <poppack.h>

#endif // UNDER_XBOX

//******************************************************************************
// Function prototypes
//******************************************************************************

CBaseTexture8*  CreateTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight,
                                D3DFORMAT fmt = D3DFMT_R5G6B5, 
                                TEXTURETYPE tt = TTYPE_TEXTURE,
                                D3DPOOL pool = POOL_DEFAULT);
CBaseTexture8*  CreateTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight, 
                                UINT uDepth, D3DFORMAT fmt = D3DFMT_R5G6B5, 
                                TEXTURETYPE tt = TTYPE_TEXTURE,
                                D3DPOOL pool = POOL_DEFAULT);
CBaseTexture8*  CreateTexture(CDevice8* pDevice, LPCTSTR szName, 
                                D3DFORMAT fmt = D3DFMT_R5G6B5,
                                TEXTURETYPE tt = TTYPE_TEXTURE,
                                PALETTECOLOR* ppcPalette = NULL,
                                float fAlpha = 1.0f,
                                D3DCOLOR cColorKey = COLORKEY_NONE,
                                D3DPOOL pool = POOL_DEFAULT,
                                UINT uWidth = D3DX_DEFAULT, 
                                UINT uHeight = D3DX_DEFAULT);
void            ReleaseTexture(CBaseTexture8* pd3dt);
void            ReleaseTextures();
void            RemoveTexture(CBaseTexture8* pd3dt);

BOOL            FilterTexture(CBaseTexture8* pd3dt, PALETTECOLOR* ppcPalette = NULL, 
                                UINT uSrcLevel = D3DX_DEFAULT, 
                                DWORD dwFilter = D3DX_FILTER_BOX);

BOOL            SyncTexture(CBaseTexture8* pd3dtb);
BOOL            SyncSurface(CSurface8* pd3ds);
BOOL            SyncVolume(CVolume8* pd3dv);

BOOL            SetTranslucency(CBaseTexture8* pd3dt, 
                                PALETTECOLOR* ppcPalette, float fAlpha, 
                                D3DCOLOR cColorKey = COLORKEY_NONE);

DWORD           ColorToPixel(D3DFORMAT fmt, PALETTECOLOR* ppcPalette, 
                                D3DCOLOR c);

HRESULT         CopyTexture(CBaseTexture8* pd3dtDst,
                                PALETTECOLOR* ppcPalDst,
                                CBaseTexture8* pd3dtSrc,
                                PALETTECOLOR* ppcPalSrc, DWORD dwFilter);

CCubeTexture8*  CreateNormalMap(CDevice8* pDevice, UINT uLength = 256, 
                                float fBumpDelta = 0.0f,
                                D3DFORMAT fmt = D3DFMT_A8R8G8B8, 
                                BOOL bMipmap = TRUE, 
                                D3DPOOL pool = POOL_DEFAULT);

CCubeTexture8*  CreateHalfVectorMap(CDevice8* pDevice, 
                                D3DVECTOR* pvLightDir, 
                                UINT uLength = 32,
                                D3DFORMAT fmt = D3DFMT_A8R8G8B8,
                                BOOL bMipmap = TRUE,
                                D3DPOOL pool = POOL_DEFAULT);

BOOL            UpdateHalfVectorMap(CCubeTexture8* pd3dtc,
                                D3DVECTOR* pvLightDir);

BOOL            WorldVectorToTangentSpace(PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvVector, BOOL bInterpolate);

BOOL            HalfVectorToTangentSpace(PTCOORD3 ptcTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate);

BOOL            SetTangentTransforms(PMATRIX3X3 pmTangent, DWORD dwStrideTangent,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcTCoord, DWORD dwStrideTCoord,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld,
                                BOOL bInterpolate);

CTexture8*      CreateDiffuseMap(CDevice8* pDevice, UINT uWidth, UINT uHeight, 
                                D3DCOLOR c, D3DFORMAT fmt, BOOL bMipmap = FALSE,
                                PALETTECOLOR* ppcPalette = NULL,
                                D3DPOOL pool = POOL_DEFAULT);

// ##TODO: CreateSpecularMap which takes a power argument and creates a corresponding map

BOOL            SetDiffuseMapCoords(PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, BOOL bInterpolate);
BOOL            SetSpecularMapCoords(PTCOORD2 ptcLightMap, DWORD dwStrideLMap,
                                D3DVECTOR* pvPosition, DWORD dwStridePosition,
                                PTCOORD2 ptcOffsetMap, DWORD dwStrideOMap,
                                LPWORD pwIndices, DWORD dwNumPrimitives,
                                D3DPRIMITIVETYPE d3dpt, D3DMATRIX* pmWorld, 
                                D3DVECTOR* pvLightDir, D3DVECTOR* pvViewPos,
                                BOOL bInterpolate);

PTEXTUREDESC    GetTextureDesc(CBaseTexture8* pd3dt);
void            GetARGBPixelDesc(D3DFORMAT fmt, PARGBPIXELDESC ppixd);

void            ConvertPALETTEENTRYToD3DCOLOR(LPVOID pvPalette, UINT uNumEntries);
void            ConvertD3DCOLORToPALETTEENTRY(LPVOID pvPalette, UINT uNumEntries);

void            VerifyTextureListIntegrity();

//******************************************************************************
// Inline functions
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     SetColorStage
//
// Description:
//
//     Set the texture stage color states for the given stage and operation
//     to the given arguments.
//
// Arguments:
//
//     CDevice* pDevice                     - Pointer to the device object
//
//     DWORD dwStage                        - Texture stage
//
//     DWORD dwArg1                         - First color argument
//
//     DWORD dwArg2                         - Second color argument
//
//     D3DTEXTUREOP                         - Color operation
//
// Return Value:
//
//     An HRESULT returned by IDirect3DDevice3::SetTextureStageState
//
//******************************************************************************
inline HRESULT SetColorStage(CDevice8* pDevice, DWORD dwStage, DWORD dwArg1, DWORD dwArg2, D3DTEXTUREOP d3dtop)
{
    HRESULT hr;
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_COLOROP, d3dtop);
    if (FAILED(hr)) {
        return hr;
    }
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_COLORARG1, dwArg1);
    if (FAILED(hr)) {
        return hr;
    }
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_COLORARG2, dwArg2);
    return hr;
}

//******************************************************************************
//
// Function:
//
//     SetAlphaStage
//
// Description:
//
//     Set the texture stage alpha states for the given stage and operation
//     to the given arguments.
//
// Arguments:
//
//     CDevice* pDevice                     - Pointer to the device object
//
//     DWORD dwStage                        - Texture stage
//
//     DWORD dwArg1                         - First alpha argument
//
//     DWORD dwArg2                         - Second alpha argument
//
//     D3DTEXTUREOP                         - Alpha operation
//
// Return Value:
//
//     An HRESULT returned by IDirect3DDevice3::SetTextureStageState
//
//******************************************************************************
inline HRESULT SetAlphaStage(CDevice8* pDevice, DWORD dwStage, DWORD dwArg1, DWORD dwArg2, D3DTEXTUREOP d3dtop)
{
    HRESULT hr;
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAOP, d3dtop);
    if (FAILED(hr)) {
        return hr;
    }
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAARG1, dwArg1);
    if (FAILED(hr)) {
        return hr;
    }
    hr = pDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAARG2, dwArg2);
    return hr;
}

//******************************************************************************
inline D3DCOLOR VectorToColor(D3DXVECTOR3* pv) {
    
    return ((BYTE)((pv->x + 1.0f) / 2.0f * 255.0f) << 16) |
           ((BYTE)((pv->y + 1.0f) / 2.0f * 255.0f) << 8)  |
            (BYTE)((pv->z + 1.0f) / 2.0f * 255.0f);
}

//******************************************************************************
inline D3DXVECTOR3 ColorToVector(D3DCOLOR c) {

    return D3DXVECTOR3((float)((c >> 16) & 0xFF) / 127.5f - 1.0f,
                       (float)((c >> 8 ) & 0xFF) / 127.5f - 1.0f,
                       (float)((c      ) & 0xFF) / 127.5f - 1.0f);
}

#endif //__TEXTURE_H__
