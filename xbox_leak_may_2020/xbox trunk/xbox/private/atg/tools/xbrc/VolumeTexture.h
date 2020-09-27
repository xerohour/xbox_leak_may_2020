//-----------------------------------------------------------------------------
// File: VolumeTexture.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the volume texture-related functionality of the bundler tool
//
// Hist: 2001.05.25 - New for July XDK release
//       2002.01.31 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef VOLUMETEXTURE_H
#define VOLUMETEXTURE_H

#include "Texture.h"


typedef CHAR FILENAME[80];

//-----------------------------------------------------------------------------
// Name: class CVolumeTexture
// Desc: Handles all VolumeTexture-specific processing, such as loading slices,
//       format conversion, swizzling, etc.
//-----------------------------------------------------------------------------
class CVolumeTexture : public CBaseTexture
{
    CVolume* m_pVolume;
    CImage** m_apImages;

    HRESULT LoadVolumeTexture();
    HRESULT SaveHeaderInfo( DWORD dwStart, DWORD* pcbHeader );

public:
    FILENAME* m_astrSource;
    FILENAME* m_astrAlphaSource;
    DWORD     m_dwWidth;
    DWORD     m_dwHeight;
    DWORD     m_dwDepth;

    HRESULT SaveToBundle( DWORD* pcbHeader, DWORD* pcbData );

    CVolumeTexture( CBundler* pBundler );
    ~CVolumeTexture();
};


#endif // VOLUMETEXTURE_H
