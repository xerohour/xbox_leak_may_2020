/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    texture.h

Description:

    Direct3D texture routines.

*******************************************************************************/

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#define COLORKEY_NONE       0xFF000000
#define COLORKEY_PIXEL1     0x01000000

#define TTYPE_TEXTURE       0x0000
#define TTYPE_CUBEMAP       0x0001
#define TTYPE_VOLUME        0x0002
#define TTYPE_BUMPMAP       0x0004
#define TTYPE_MIPMAP        0x0100
#define TTYPE_TARGET        0x0200
#define TTYPE_DEPTH         0x0400
#define TTYPE_RMASK         0x00FF

namespace DXCONIO {

typedef DWORD               TEXTURETYPE;

//******************************************************************************
// Function prototypes
//******************************************************************************

LPDIRECT3DBASETEXTURE8  CreateTexture(UINT uWidth, UINT uHeight,
                                    D3DFORMAT fmt, 
                                    TEXTURETYPE tt = TTYPE_TEXTURE,
                                    D3DPOOL pool = D3DPOOL_DEFAULT);
LPDIRECT3DBASETEXTURE8  CreateTexture(UINT uWidth, UINT uHeight, UINT uDepth,
                                    D3DFORMAT fmt, 
                                    TEXTURETYPE tt = TTYPE_TEXTURE,
                                    D3DPOOL pool = D3DPOOL_DEFAULT);
LPDIRECT3DBASETEXTURE8  CreateTexture(LPCTSTR szName, D3DFORMAT fmt,
                                    TEXTURETYPE tt = TTYPE_TEXTURE,
                                    PALETTEENTRY* pcPalette = NULL,
                                    float fAlpha = 1.0f,
                                    D3DCOLOR cColorKey = COLORKEY_NONE,
                                    UINT uWidth = D3DX_DEFAULT, 
                                    UINT uHeight = D3DX_DEFAULT);
void                    ReleaseTexture(LPDIRECT3DBASETEXTURE8 pd3dt);
void                    ReleaseTextures();

BOOL                    SetTranslucency(LPDIRECT3DBASETEXTURE8 pd3dt, 
                                    PALETTEENTRY* pcPalette, float fAlpha, 
                                    D3DCOLOR cColorKey = COLORKEY_NONE);

DWORD                   ColorToPixel(LPDIRECT3DSURFACE8 pd3dt, 
                                    PALETTEENTRY* pcPalette, D3DCOLOR c);

HRESULT                 CopyTexture(LPDIRECT3DBASETEXTURE8 pd3dtDst,
                                    PALETTEENTRY* pcPalDst,
                                    LPDIRECT3DBASETEXTURE8 pd3dtSrc,
                                    PALETTEENTRY* pcPalSrc, DWORD dwFilter);

LPDIRECT3DTEXTURE8      CreateDiffuseMap(UINT uWidth, UINT uHeight, D3DCOLOR c,
                                    D3DFORMAT fmt, BOOL bMipmap = FALSE,
                                    PALETTEENTRY* pcPalette = NULL,
                                    D3DPOOL pool = D3DPOOL_DEFAULT);

BOOL                    SetDiffuseMapCoords(D3DVECTOR* pvNormal, UINT uNStride, 
                                    PTCOORD ptcTCoord, UINT uTStride, 
                                    UINT uVertices, D3DMATRIX* pmWorld, 
                                    D3DVECTOR* pvLightDir, float fTheta);

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
inline HRESULT SetColorStage(DWORD dwStage, DWORD dwArg1, DWORD dwArg2, D3DTEXTUREOP d3dtop)
{
    HRESULT hr;
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_COLOROP, d3dtop);
    if (FAILED(hr)) {
        return hr;
    }
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_COLORARG1, dwArg1);
    if (FAILED(hr)) {
        return hr;
    }
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_COLORARG2, dwArg2);
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
inline HRESULT SetAlphaStage(DWORD dwStage, DWORD dwArg1, DWORD dwArg2, D3DTEXTUREOP d3dtop)
{
    HRESULT hr;
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAOP, d3dtop);
    if (FAILED(hr)) {
        return hr;
    }
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAARG1, dwArg1);
    if (FAILED(hr)) {
        return hr;
    }
    hr = g_pd3dDevice->SetTextureStageState(dwStage, D3DTSS_ALPHAARG2, dwArg2);
    return hr;
}

} // namespace DXCONIO

#endif //__TEXTURE_H__
