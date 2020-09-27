//-----------------------------------------------------------------------------
// File: Cubemap.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the cubemap-related functionality of the bundler tool
//
// Hist: 04.11.01 - New for May XDK release
//       01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "Texture.h"




//-----------------------------------------------------------------------------
// Name: class CCubemap
// Desc: Handles all texture-specific processing, such as loading textures,
//       format conversion, swizzling, etc.
//-----------------------------------------------------------------------------
class CCubemap : public CBaseTexture
{
    CImage* m_pImageXP;
    CImage* m_pImageXN;
    CImage* m_pImageYP;
    CImage* m_pImageYN;
    CImage* m_pImageZP;
    CImage* m_pImageZN;

    HRESULT LoadCubemap();
    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader );

public:
    char  m_strSourceXP[MAX_PATH];
    char  m_strSourceXN[MAX_PATH];
    char  m_strSourceYP[MAX_PATH];
    char  m_strSourceYN[MAX_PATH];
    char  m_strSourceZP[MAX_PATH];
    char  m_strSourceZN[MAX_PATH];
    char  m_strAlphaSourceXP[MAX_PATH];
    char  m_strAlphaSourceXN[MAX_PATH];
    char  m_strAlphaSourceYP[MAX_PATH];
    char  m_strAlphaSourceYN[MAX_PATH];
    char  m_strAlphaSourceZP[MAX_PATH];
    char  m_strAlphaSourceZN[MAX_PATH];
    DWORD m_dwSize;

    HRESULT SaveToBundle( DWORD* pcbHeader, DWORD* pcbData );

    CCubemap( CBundler* pBundler );
    ~CCubemap();
};


#endif // CUBEMAP_H