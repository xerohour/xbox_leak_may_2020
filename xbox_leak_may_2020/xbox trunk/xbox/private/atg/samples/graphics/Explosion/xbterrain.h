//-----------------------------------------------------------------------------
// File: XBTerrain.h
//
// Desc: Defines the CXBTerrain object, which provides an application-
//       customizable terrain
//
// Hist: 04.03.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBTERRAIN_H
#define XBTERRAIN_H

#include <xtl.h>

typedef FLOAT (__stdcall FNHEIGHTFUNCTION( FLOAT fX, FLOAT fZ ));

typedef struct {
    D3DXVECTOR3 p;
    D3DXVECTOR2 t;
} XBTERRAIN_VERTEX;
#define FVF_XBTERRAIN_VERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

FLOAT WINAPI fDummy( FLOAT fX, FLOAT fZ );

class CXBTerrain
{
public:
    CXBTerrain();
    ~CXBTerrain();

    HRESULT Initialize( LPDIRECT3DDEVICE8 pDevice );
    HRESULT Generate( DWORD dwXSlices, 
                      DWORD dwZSlices,
                      D3DXVECTOR2 vXZMin, 
                      D3DXVECTOR2 vXZMax, 
                      LPDIRECT3DTEXTURE8 pTexture,
                      FNHEIGHTFUNCTION pfnHeight = fDummy,
                      FLOAT fXRepeat = 1.0f,
                      FLOAT fZRepeat = 1.0f );
    HRESULT Render();

private:
    LPDIRECT3DDEVICE8           m_pd3dDevice;
    LPDIRECT3DTEXTURE8          m_pTexture;
    D3DXVECTOR2                 m_vXZMin;
    D3DXVECTOR2                 m_vXZMax;
    FLOAT                       m_fXTextureRepeat;
    FLOAT                       m_fZTextureRepeat;
    FNHEIGHTFUNCTION *          m_pfnHeight;
    DWORD                       m_dwNumIndices;

    LPDIRECT3DVERTEXBUFFER8     m_pvbTerrain;
    WORD *                      m_pibTerrain;
};

#endif // XBTERRAIN_H