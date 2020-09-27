//-----------------------------------------------------------------------------
// File: BaseTexture.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the basic texture-related functionality of the bundler tool
//
// Hist: 2001.04.13 - New for May XDK release
//       2002.01.31 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef BASETEXTURE_H
#define BASETEXTURE_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>
#include "xd3d.h"
#include "LoadImage.h"

class CBundler;



// Enum for format definitions
typedef enum { FMT_LINEAR, FMT_SWIZZLED, FMT_COMPRESSED } FMT_TYPE;


// Format definition
struct FORMATSPEC
{
    CHAR*    strFormat;
    DWORD    dwXboxFormat;
    FMT_TYPE Type;
	DWORD    dwNumAlphaBits;
};
extern const FORMATSPEC g_TextureFormats[];




//-----------------------------------------------------------------------------
// Name: class CBaseTexture
// Desc: Handles all texture-specific processing, such as loading textures,
//       format conversion, swizzling, etc.
//-----------------------------------------------------------------------------
class CBaseTexture
{
public:
    CBaseTexture( CBundler* pBundler );
    ~CBaseTexture();

    CHAR  m_strFormat[MAX_PATH];
    int   m_nFormat;
    DWORD m_dwFilter;
    DWORD m_dwLevels;
	D3DCOLOR m_ColorKey;
	
protected:
    int     FormatFromString( CHAR* strFormat );            // Format processing
    int     FormatFromPCFormat( D3DFORMAT d3dFormat );
    HRESULT LoadImage( CHAR* strSource, CHAR* strAlphaSource, CImage** ppImage );
    HRESULT LoadImageUsingD3DX( CHAR* strSource, CHAR* strAlphaSource,
                                CImage** ppImage );
    HRESULT ResizeImage( DWORD dwWidth, DWORD dwHeight, CImage** ppImage );
    HRESULT SaveImage( DWORD* pcbData, DWORD dwLevels, CImage* pImage );

    HRESULT SaveVolumeTexture( DWORD* pcbData, DWORD dwLevels, CVolume* pVolume );

    DWORD   WriteLinearTextureData( VOID* pBits, DWORD dwWidth, 
                                    DWORD dwHeight, DWORD dwDepth );
    DWORD   WriteSwizzledTextureData( VOID* pBits, DWORD dwWidth, 
                                      DWORD dwHeight, DWORD dwDepth );
    DWORD   WriteCompressedTextureData( VOID* pBits, DWORD dwWidth, 
                                        DWORD dwHeight, DWORD dwDepth );

    CBundler* m_pBundler;
};


#endif // BASETEXTURE_H
