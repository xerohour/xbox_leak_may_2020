//-----------------------------------------------------------------------------
// File: Texture.h
//
// Desc: Header file containing class definition for the 2d texture-related 
//       functionality of the bundler tool
//
// Hist: 02.06.01 - New for March XDK release
//       04.13.01 - Reorganized for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef TEXTURE_H
#define TEXTURE_H

#include "BaseTexture.h"




//-----------------------------------------------------------------------------
// Name: class CTexture2D
// Desc: Handles all texture-specific processing, such as loading textures,
//       format conversion, swizzling, etc.
//-----------------------------------------------------------------------------
class CTexture2D : public CBaseTexture
{
public:
    CTexture2D( CBundler* pBundler );
    ~CTexture2D();

    HRESULT SaveToBundle( DWORD* pcbHeader, DWORD* pcbData );

    CHAR  m_strSource[MAX_PATH];
    CHAR  m_strAlphaSource[MAX_PATH];
    DWORD m_dwWidth;
    DWORD m_dwHeight;

protected:
    HRESULT LoadTexture();
    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader );

    LPDIRECT3DSURFACE8 m_pSurface;
};


#endif // TEXTURE_H