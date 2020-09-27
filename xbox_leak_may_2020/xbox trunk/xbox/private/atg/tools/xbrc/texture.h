//-----------------------------------------------------------------------------
// File: Texture.h
//
// Desc: Header file containing class definition for the 2d texture-related 
//       functionality of the bundler tool
//
// Hist: 2001.02.06 - New for March XDK release
//       2001.04.13 - Reorganized for May XDK release
//       2002.01.31 - Updated to avoid using D3D
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
	
    CImage* m_pImage;
};


//-----------------------------------------------------------------------------
// Name: class CTexture2DFill
// Desc: Adds the ability to fill a texture texel-by-texel.
//-----------------------------------------------------------------------------
class CTexture2DFill : public CTexture2D
{
public:
	CTexture2DFill(CBundler *pBundler) : CTexture2D(pBundler) {}
	LONG m_iTexel;	// current position within texture
	HRESULT SetNextTexel(DWORD dwColor);
};


#endif // TEXTURE_H
