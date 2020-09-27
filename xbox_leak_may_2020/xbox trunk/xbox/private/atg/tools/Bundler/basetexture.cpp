//-----------------------------------------------------------------------------
// File: BaseTexture.cpp
//
// Desc: Contains the basic texture-specific logic for the bundler tool
//
// Hist: 04.13.01 - New for May XDK release
//       01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "BaseTexture.h"
#include "Bundler.h"
#include "LoadImage.h"
#include "xgraphics.h"
#include "cd3dxblt.h"
#include <d3d8.h>
#include <d3d8types.h>


// Texture formats
const FORMATSPEC g_TextureFormats[] = 
{
    // Format name           Xbox save format        Type           #alphabits
    { "D3DFMT_A8R8G8B8",     X_D3DFMT_A8R8G8B8,      FMT_SWIZZLED,   8    },
    { "D3DFMT_X8R8G8B8",     X_D3DFMT_X8R8G8B8,      FMT_SWIZZLED,   8    },
    { "D3DFMT_A8B8G8R8",     X_D3DFMT_A8B8G8R8,      FMT_SWIZZLED,   8    },
    { "D3DFMT_B8G8R8A8",     X_D3DFMT_B8G8R8A8,      FMT_SWIZZLED,   8    },
    { "D3DFMT_R8G8B8A8",     X_D3DFMT_R8G8B8A8,      FMT_SWIZZLED,   8    },
    { "D3DFMT_X8L8V8U8",     X_D3DFMT_X8L8V8U8,      FMT_SWIZZLED,   0    },
    { "D3DFMT_Q8W8V8U8",     X_D3DFMT_Q8W8V8U8,      FMT_SWIZZLED,   0    },
    { "D3DFMT_V16U16",       X_D3DFMT_V16U16,        FMT_SWIZZLED,   0    },
    { "D3DFMT_A4R4G4B4",     X_D3DFMT_A4R4G4B4,      FMT_SWIZZLED,   4    },
    { "D3DFMT_R4G4B4A4",     X_D3DFMT_R4G4B4A4,      FMT_SWIZZLED,   4    },
    { "D3DFMT_X1R5G5B5",     X_D3DFMT_X1R5G5B5,      FMT_SWIZZLED,   0    },
    { "D3DFMT_A1R5G5B5",     X_D3DFMT_A1R5G5B5,      FMT_SWIZZLED,   1    },
    { "D3DFMT_R5G5B5A1",     X_D3DFMT_R5G5B5A1,      FMT_SWIZZLED,   1    },
    { "D3DFMT_R5G6B5",       X_D3DFMT_R5G6B5,        FMT_SWIZZLED,   0    },
    { "D3DFMT_R6G5B5",       X_D3DFMT_R6G5B5,        FMT_SWIZZLED,   0    },
    { "D3DFMT_L6V5U5",       X_D3DFMT_L6V5U5,        FMT_SWIZZLED,   0    },
    { "D3DFMT_R8B8",         X_D3DFMT_R8B8,          FMT_SWIZZLED,   0    },
    { "D3DFMT_G8B8",         X_D3DFMT_G8B8,          FMT_SWIZZLED,   0    },
    { "D3DFMT_V8U8",         X_D3DFMT_V8U8,          FMT_SWIZZLED,   0    },
    { "D3DFMT_A8L8",         X_D3DFMT_A8L8,          FMT_SWIZZLED,   8    },
    { "D3DFMT_AL8",          X_D3DFMT_AL8,           FMT_SWIZZLED,   8    },
    { "D3DFMT_A8",           X_D3DFMT_A8,            FMT_SWIZZLED,   8    },
    { "D3DFMT_L8",           X_D3DFMT_L8,            FMT_SWIZZLED,   0    },
//  { "D3DFMT_P8",           X_D3DFMT_P8,            FMT_SWIZZLED,   8    }, // Bundler doesn't support (yet)
    { "D3DFMT_L16",          X_D3DFMT_L16,           FMT_SWIZZLED,   0    },
    { "D3DFMT_DXT1",         X_D3DFMT_DXT1,          FMT_COMPRESSED, 1    },
    { "D3DFMT_DXT2",         X_D3DFMT_DXT2,          FMT_COMPRESSED, 8    },
    { "D3DFMT_DXT3",         X_D3DFMT_DXT3,          FMT_COMPRESSED, 8    },
    { "D3DFMT_DXT4",         X_D3DFMT_DXT4,          FMT_COMPRESSED, 8    },
    { "D3DFMT_DXT5",         X_D3DFMT_DXT5,          FMT_COMPRESSED, 8    },
    { "D3DFMT_LIN_A8B8G8R8", X_D3DFMT_LIN_A8B8G8R8,  FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_A8R8G8B8", X_D3DFMT_LIN_A8R8G8B8,  FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_B8G8R8A8", X_D3DFMT_LIN_B8G8R8A8,  FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_R8G8B8A8", X_D3DFMT_LIN_R8G8B8A8,  FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_X8R8G8B8", X_D3DFMT_LIN_X8R8G8B8,  FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_X8L8V8U8", X_D3DFMT_LIN_X8L8V8U8,  FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_Q8W8V8U8", X_D3DFMT_LIN_Q8W8V8U8,  FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_V16U16",   X_D3DFMT_LIN_V16U16,    FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_A4R4G4B4", X_D3DFMT_LIN_A4R4G4B4,  FMT_LINEAR    , 4    },
    { "D3DFMT_LIN_R4G4B4A4", X_D3DFMT_LIN_R4G4B4A4,  FMT_LINEAR    , 4    },
    { "D3DFMT_LIN_A1R5G5B5", X_D3DFMT_LIN_A1R5G5B5,  FMT_LINEAR    , 1    },
    { "D3DFMT_LIN_R5G5B5A1", X_D3DFMT_LIN_R5G5B5A1,  FMT_LINEAR    , 1    },
    { "D3DFMT_LIN_X1R5G5B5", X_D3DFMT_LIN_X1R5G5B5,  FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_R5G6B5",   X_D3DFMT_LIN_R5G6B5,    FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_R6G5B5",   X_D3DFMT_LIN_R6G5B5,    FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_L6V5U5",   X_D3DFMT_LIN_L6V5U5,    FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_G8B8",     X_D3DFMT_LIN_G8B8,      FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_R8B8",     X_D3DFMT_LIN_R8B8,      FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_A8L8",     X_D3DFMT_LIN_A8L8,      FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_V8U8",     X_D3DFMT_LIN_V8U8,      FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_AL8",      X_D3DFMT_LIN_AL8,       FMT_LINEAR    , 8    },
    { "D3DFMT_LIN_L16",      X_D3DFMT_LIN_L16,       FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_L8",       X_D3DFMT_LIN_L8,        FMT_LINEAR    , 0    },
    { "D3DFMT_LIN_A8",       X_D3DFMT_LIN_A8,        FMT_LINEAR    , 8    },
};



//-----------------------------------------------------------------------------
// Name: ConvertTextureFormat()
// Desc: 
// TODO: Make this handle the pitch requirements of small textures
//-----------------------------------------------------------------------------
HRESULT ConvertTextureFormat( VOID* pSrcData, 
                              DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, 
                              VOID* pDstData, DWORD dwDstFormat )
{
    // Create convenient access ptrs
    DWORD* pSrcData32 = (DWORD*)pSrcData;
    BYTE*  pDstData8  =  (BYTE*)pDstData;
    WORD*  pDstData16 =  (WORD*)pDstData;
    DWORD* pDstData32 = (DWORD*)pDstData;

    // Look through all pixels
    for( DWORD z=0; z<dwDepth; z++ )
    {
        for( DWORD y=0; y<dwHeight; y++ )
        {
            for( DWORD x=0; x<dwWidth; x++ )
            {
                // Read src pixel
                FLOAT a = (((*pSrcData32)&0xff000000)>>24L)/255.0f;
                FLOAT r = (((*pSrcData32)&0x00ff0000)>>16L)/255.0f;
                FLOAT g = (((*pSrcData32)&0x0000ff00)>> 8L)/255.0f;
                FLOAT b = (((*pSrcData32)&0x000000ff)>> 0L)/255.0f;
                pSrcData32++;

                // Assign values to non-ARGB channels
                FLOAT q = a;
                FLOAT w = r;
                FLOAT v = g;
                FLOAT u = b;
                FLOAT l = (r+g+b)/3;

                // Write dst pixel
                switch( dwDstFormat )
                {
                    case X_D3DFMT_A8R8G8B8:     // case X_D3DFMT_Q8W8V8U8:
                    case X_D3DFMT_LIN_A8R8G8B8: // case X_D3DFMT_LIN_Q8W8V8U8:
                        (*pDstData32++) = (((DWORD)(a*0xff))<<24L) | 
                                          (((DWORD)(r*0xff))<<16L) | 
                                          (((DWORD)(g*0xff))<< 8L) | 
                                          (((DWORD)(b*0xff))<< 0L);
                        break;
                    case X_D3DFMT_X8R8G8B8:     // case X_D3DFMT_X8L8V8U8:
                    case X_D3DFMT_LIN_X8R8G8B8: // case X_D3DFMT_LIN_X8L8V8U8:
                        (*pDstData32++) = (((DWORD)(r*0xff))<<16L) | 
                                          (((DWORD)(g*0xff))<< 8L) | 
                                          (((DWORD)(b*0xff))<< 0L);
                        break;
                    case X_D3DFMT_A8B8G8R8:
                    case X_D3DFMT_LIN_A8B8G8R8:
                        (*pDstData32++) = (((DWORD)(a*0xff))<<24L) | 
                                          (((DWORD)(b*0xff))<<16L) | 
                                          (((DWORD)(g*0xff))<< 8L) | 
                                          (((DWORD)(r*0xff))<< 0L);
                        break;
                    case X_D3DFMT_B8G8R8A8:
                    case X_D3DFMT_LIN_B8G8R8A8:
                        (*pDstData32++) = (((DWORD)(b*0xff))<<24L) | 
                                          (((DWORD)(g*0xff))<<16L) | 
                                          (((DWORD)(r*0xff))<< 8L) | 
                                          (((DWORD)(a*0xff))<< 0L);
                        break;
                    case X_D3DFMT_R8G8B8A8:
                    case X_D3DFMT_LIN_R8G8B8A8:
                        (*pDstData32++) = (((DWORD)(r*0xff))<<24L) | 
                                          (((DWORD)(g*0xff))<<16L) | 
                                          (((DWORD)(b*0xff))<< 8L) | 
                                          (((DWORD)(a*0xff))<< 0L);
                        break;
                    case X_D3DFMT_A1R5G5B5:
                    case X_D3DFMT_LIN_A1R5G5B5:
                        (*pDstData16++) = (((WORD)(a*0x01))<<15L) | 
                                          (((WORD)(r*0x1f))<<10L) | 
                                          (((WORD)(g*0x1f))<< 5L) | 
                                          (((WORD)(b*0x1f))<< 0L);
                        break;
                    case X_D3DFMT_X1R5G5B5:
                    case X_D3DFMT_LIN_X1R5G5B5:
                        (*pDstData16++) = (((WORD)(r*0x1f))<<10L) | 
                                          (((WORD)(g*0x1f))<< 5L) | 
                                          (((WORD)(b*0x1f))<< 0L);
                        break;
                    case X_D3DFMT_R5G5B5A1:
                    case X_D3DFMT_LIN_R5G5B5A1:
                        (*pDstData16++) = (((WORD)(r*0x1f))<<11L) | 
                                          (((WORD)(g*0x1f))<< 6L) | 
                                          (((WORD)(b*0x1f))<< 1L) | 
                                          (((WORD)(a*0x01))<< 0L);
                        break;
                    case X_D3DFMT_R5G6B5:
                    case X_D3DFMT_LIN_R5G6B5:
                        (*pDstData16++) = (((WORD)(r*0x1f))<<11L) | 
                                          (((WORD)(g*0x3f))<< 5L) | 
                                          (((WORD)(b*0x1f))<< 0L);
                        break;
                    case X_D3DFMT_R6G5B5:     // case X_D3DFMT_L6V5U5:
                    case X_D3DFMT_LIN_R6G5B5: // case X_D3DFMT_LIN_L6V5U5:
                        (*pDstData16++) = (((WORD)(r*0x3f))<<10L) | 
                                          (((WORD)(g*0x1f))<< 5L) | 
                                          (((WORD)(b*0x1f))<< 0L);
                        break;
                    case X_D3DFMT_A4R4G4B4:
                    case X_D3DFMT_LIN_A4R4G4B4:
                        (*pDstData16++) = (((WORD)(a*0x0f))<<12L) | 
                                          (((WORD)(r*0x0f))<< 8L) | 
                                          (((WORD)(g*0x0f))<< 4L) | 
                                          (((WORD)(b*0x0f))<< 0L);
                        break;
                    case X_D3DFMT_R4G4B4A4:
                    case X_D3DFMT_LIN_R4G4B4A4:
                        (*pDstData16++) = (((WORD)(r*0x0f))<<12L) | 
                                          (((WORD)(g*0x0f))<< 8L) | 
                                          (((WORD)(b*0x0f))<< 4L) | 
                                          (((WORD)(a*0x0f))<< 0L);
                        break;
                    case X_D3DFMT_R8B8:
                    case X_D3DFMT_LIN_R8B8:
                        (*pDstData16++) = (((WORD)(r*0xff))<< 8L) | 
                                          (((WORD)(b*0xff))<< 0L);
                        break;
                    case X_D3DFMT_G8B8:     // case X_D3DFMT_V8U8:
                    case X_D3DFMT_LIN_G8B8: // case X_D3DFMT_LIN_V8U8:
                        (*pDstData16++) = (((WORD)(g*0xff))<< 8L) | 
                                          (((WORD)(b*0xff))<< 0L);
                        break;
                    case X_D3DFMT_A8L8:
                    case X_D3DFMT_LIN_A8L8:
                        (*pDstData16++) = (((WORD)(a*0xff))<< 8L) | 
                                          (((WORD)(l*0xff))<< 0L);
                        break;
                    case X_D3DFMT_L16:
                    case X_D3DFMT_LIN_L16:
                        (*pDstData16++) = (((WORD)(l*0xffff))<< 0L);
                        break;
                    case X_D3DFMT_L8:
                    case X_D3DFMT_LIN_L8:
                        (*pDstData8++)  = (((BYTE)(l*0xff))<< 0L);
                        break;
                    case X_D3DFMT_A8:
                    case X_D3DFMT_LIN_A8:
                        (*pDstData8++)  = (((BYTE)(a*0xff))<< 0L);
                        break;
                    case X_D3DFMT_AL8:
                    case X_D3DFMT_LIN_AL8:
                        (*pDstData8++)  = (((BYTE)(l*0xff))<< 0L);
                        break;
                    case X_D3DFMT_V16U16:
                    case X_D3DFMT_LIN_V16U16:
                        (*pDstData32++) = (((DWORD)(v*0xffff))<<16L) | 
                                          (((DWORD)(u*0xffff))<< 0L);
                        break;
                }
            }
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CBaseTexture()
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CBaseTexture::CBaseTexture( CBundler * pBundler )
{
    m_nFormat      = -1;
    m_strFormat[0] = 0;
    m_dwFilter     = D3DX_FILTER_TRIANGLE;
    m_dwLevels     = 1;
    m_pBundler     = pBundler;
}




//-----------------------------------------------------------------------------
// Name: ~CBaseTexture()
// Desc: Performs any cleanup needed before the object is destroyed
//-----------------------------------------------------------------------------
CBaseTexture::~CBaseTexture()
{
}




//-----------------------------------------------------------------------------
// Name: FormatFromString()
// Desc: Returns an index into the format spec array corresponding to the
//       given string
//-----------------------------------------------------------------------------
int CBaseTexture::FormatFromString( char* strFormat )
{
    if( strFormat[0] == 0 )
        return 0;

    DWORD dwNumFormats = sizeof(g_TextureFormats) / sizeof(g_TextureFormats[0]);

    for( DWORD i = 0; i < dwNumFormats; i++ )
    {
        if( !lstrcmpi( strFormat, g_TextureFormats[i].strFormat ) )
            return i;
    }

    return -2;
}




//-----------------------------------------------------------------------------
// Name: FormatFromString()
// Desc: Returns an index into the format spec array corresponding to the
//       given string
//-----------------------------------------------------------------------------
int CBaseTexture::FormatFromPCFormat( D3DFORMAT d3dFormat )
{
    switch( d3dFormat )
    {
        case D3DFMT_A8R8G8B8: return FormatFromString( "D3DFMT_A8R8G8B8" );
        case D3DFMT_X8R8G8B8: return FormatFromString( "D3DFMT_X8R8G8B8" );
//      case D3DFMT_A8B8G8R8: return FormatFromString( "D3DFMT_A8B8G8R8" );
//      case D3DFMT_B8G8R8A8: return FormatFromString( "D3DFMT_B8G8R8A8" );
//      case D3DFMT_R8G8B8A8: return FormatFromString( "D3DFMT_R8G8B8A8" );
        case D3DFMT_X8L8V8U8: return FormatFromString( "D3DFMT_X8L8V8U8" );
        case D3DFMT_Q8W8V8U8: return FormatFromString( "D3DFMT_Q8W8V8U8" );
        case D3DFMT_V16U16:   return FormatFromString( "D3DFMT_V16U16" );
        case D3DFMT_A4R4G4B4: return FormatFromString( "D3DFMT_A4R4G4B4" );
//      case D3DFMT_R4G4B4A4: return FormatFromString( "D3DFMT_R4G4B4A4" );
        case D3DFMT_X1R5G5B5: return FormatFromString( "D3DFMT_X1R5G5B5" );
        case D3DFMT_A1R5G5B5: return FormatFromString( "D3DFMT_A1R5G5B5" );
//      case D3DFMT_R5G5B5A1: return FormatFromString( "D3DFMT_R5G5B5A1" );
        case D3DFMT_R5G6B5:   return FormatFromString( "D3DFMT_R5G6B5" );
//      case D3DFMT_R6G5B5:   return FormatFromString( "D3DFMT_R6G5B5" );
        case D3DFMT_L6V5U5:   return FormatFromString( "D3DFMT_L6V5U5" );
//      case D3DFMT_R8B8:     return FormatFromString( "D3DFMT_R8B8" );
//      case D3DFMT_G8B8:     return FormatFromString( "D3DFMT_G8B8" );
        case D3DFMT_V8U8:     return FormatFromString( "D3DFMT_V8U8" );
        case D3DFMT_A8L8:     return FormatFromString( "D3DFMT_A8L8" );
//      case D3DFMT_AL8:      return FormatFromString( "D3DFMT_AL8" );
        case D3DFMT_A8:       return FormatFromString( "D3DFMT_A8" );
        case D3DFMT_L8:       return FormatFromString( "D3DFMT_L8" );
//      case D3DFMT_A8R8G8B8: return FormatFromString( "D3DFMT_P8" );
//      case D3DFMT_L16:      return FormatFromString( "D3DFMT_L16" );
        case D3DFMT_DXT1:     return FormatFromString( "D3DFMT_DXT1" );
        case D3DFMT_DXT2:     return FormatFromString( "D3DFMT_DXT2" );
//      case D3DFMT_DXT3:     return FormatFromString( "D3DFMT_DXT3" );
        case D3DFMT_DXT4:     return FormatFromString( "D3DFMT_DXT4" );
//      case D3DFMT_DXT5:     return FormatFromString( "D3DFMT_DXT5" );
    }

    return -2;
}




//-----------------------------------------------------------------------------
// Name: LoadImage()
// Desc: Loads source and alphasource surfaces
//-----------------------------------------------------------------------------
HRESULT CBaseTexture::LoadImage( CHAR* strSource, CHAR* strAlphaSource, CImage** ppImage )
{
    CImage             ColorImage;
    CImage             AlphaImage;
    CImage*            pResizedColorImage = NULL;
    CImage*            pResizedAlphaImage = NULL;
    HRESULT hr; 

    // Add the path (if necessary) to the source filename
    CHAR strImageFilePath[MAX_PATH];
    if( strchr( strSource, ':' ) )
        strcpy( strImageFilePath, strSource );
    else
    {
        strcpy( strImageFilePath, m_pBundler->m_strPath );
        strcat( strImageFilePath, strSource );
    }

    // Load the texture from the source file
    hr = ColorImage.Load( strImageFilePath );
    if( FAILED( hr ) )
        return hr;

    // Depalettize any palettized images
    if( D3DFMT_P8 == ColorImage.m_Format )
        ColorImage.Depalettize();

    if( strAlphaSource[0] )
    {
        // Add the path (if necessary) to the source filename
        CHAR strImageFilePath[MAX_PATH];
        if( strchr( strSource, ':' ) )
            strcpy( strImageFilePath, strAlphaSource );
        else
        {
            strcpy( strImageFilePath, m_pBundler->m_strPath );
            strcat( strImageFilePath, strAlphaSource );
        }

        // Load the texture from the alpha source file
        hr = AlphaImage.Load( strImageFilePath );
        if( FAILED( hr ) )
            return hr;

        // Can't currently deal with palettized images
        if( D3DFMT_P8 == AlphaImage.m_Format )
            return E_FAIL;
    }

    // Determine final texture size
    DWORD dwWidth  = ColorImage.m_Width;
    DWORD dwHeight = ColorImage.m_Height;
    
    if( strAlphaSource[0] )
    {
        if( dwWidth  < AlphaImage.m_Width  )   dwWidth  = AlphaImage.m_Width;
        if( dwHeight < AlphaImage.m_Height )   dwHeight = AlphaImage.m_Height;
    }

    // Do the size conversion for the source image
    pResizedColorImage = new CImage( ColorImage.m_Width, ColorImage.m_Height, D3DFMT_A8R8G8B8 );
    hr = LoadImageFromImage( pResizedColorImage, &ColorImage, m_dwFilter );

    if( strAlphaSource[0] )
    {
        // Do the size conversion for the alpha source image
        pResizedAlphaImage = new CImage( AlphaImage.m_Width, AlphaImage.m_Height, AlphaImage.m_Format );
        hr = LoadImageFromImage( pResizedAlphaImage, &AlphaImage, m_dwFilter );
    }

    // Merge in the alpha channel
    if( strAlphaSource[0] )
    {
        DWORD* pColorBits = (DWORD*)pResizedColorImage->m_pData;
        DWORD* pAlphaBits = (DWORD*)pResizedAlphaImage->m_pData;

        for( DWORD yp = 0; yp < dwHeight; yp++ )
        {
            for( DWORD xp = 0; xp < dwWidth; xp++ )
            {
                DWORD dwAlpha = (*pAlphaBits) << 24;
                (*pColorBits) &= 0x00ffffff;
                (*pColorBits) |= dwAlpha;

                pColorBits++;
                pAlphaBits++;
            }
        }
    }
        
    // Return the surface in the output parameter
    (*ppImage) = pResizedColorImage;

    delete pResizedAlphaImage;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadImageUsingD3DX()
// Desc: Loads source and alphasource surfaces. This should be used after other
//       load attempts failed, since D3DX forces surfaces to have dimensions
//       that are a power of two.
//-----------------------------------------------------------------------------
HRESULT CBaseTexture::LoadImageUsingD3DX( CHAR* strSource, CHAR* strAlphaSource, 
                                            CImage** ppImage )
{
    D3DSURFACE_DESC    ColorImageDesc;
    D3DSURFACE_DESC    AlphaImageDesc;
    LPDIRECT3DSURFACE8 pOrigColorSurface;
    LPDIRECT3DSURFACE8 pOrigAlphaSurface;
    LPDIRECT3DSURFACE8 pResizedColorSurface;
    LPDIRECT3DSURFACE8 pResizedAlphaSurface;
    HRESULT hr; 

    if( strAlphaSource[0] )
        printf( "WARNING: Couldn't load %s and/or %s natively - attempting to use D3D runtime.\n", strSource, strAlphaSource );
    else
        printf( "WARNING: Couldn't load %s natively - attempting to use D3D runtime.\n", strSource );

    hr = m_pBundler->InitD3D();
    if( FAILED( hr ) )
    {
        m_pBundler->ErrorMsg( "The bundler requires the D3D reference rasterizer in order\n" );
        m_pBundler->ErrorMsg( "to load certain types of files, but was unable to create it.\n" );
        m_pBundler->ErrorMsg( "Try using BMP or TGA files instead of DDS or JPG files.\n" );
        return hr;
    }

    // Add the path (if necessary) to the source filename
    CHAR strImageFilePath[MAX_PATH];
    if( strchr( strSource, ':' ) )
        strcpy( strImageFilePath, strSource );
    else
    {
        strcpy( strImageFilePath, m_pBundler->m_strPath );
        strcat( strImageFilePath, strSource );
    }


    // Load the texture from the source file
    LPDIRECT3DTEXTURE8 pImageTexture;
    hr = D3DXCreateTextureFromFileEx( m_pBundler->m_pd3ddev, strImageFilePath, 
                                      D3DX_DEFAULT, D3DX_DEFAULT, 1,
                                      0, D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM,
                                      D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                      NULL, NULL, &pImageTexture );
    if( FAILED( hr ) )
    {
        m_pBundler->ErrorMsg( "Couldn't load image <%s>\n", strSource );
        return hr;
    }

    // Get a surface for the image
    pImageTexture->GetSurfaceLevel( 0, &pOrigColorSurface );
    pImageTexture->Release();
    pOrigColorSurface->GetDesc( &ColorImageDesc );

    if( m_nFormat < 0 )
    {
        m_nFormat   = FormatFromPCFormat( ColorImageDesc.Format );
        if( m_nFormat < 0 )
        {
            m_pBundler->ErrorMsg( "Unssupported format for image <%s>\n", strSource );
            return E_FAIL;
        }
        lstrcpyA( m_strFormat, g_TextureFormats[m_nFormat].strFormat );
    }

    if( strAlphaSource[0] )
    {
        // Add the path (if necessary) to the source filename
        CHAR strImageFilePath[MAX_PATH];
        if( strchr( strSource, ':' ) )
            strcpy( strImageFilePath, strAlphaSource );
        else
        {
            strcpy( strImageFilePath, m_pBundler->m_strPath );
            strcat( strImageFilePath, strAlphaSource );
        }

        // Load the texture from the alpha source file
        LPDIRECT3DTEXTURE8 pAlphaTexture;
        hr = D3DXCreateTextureFromFileEx( m_pBundler->m_pd3ddev, strImageFilePath, 
                                          D3DX_DEFAULT, D3DX_DEFAULT, 1,
                                          0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
                                          D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                          NULL, NULL, &pAlphaTexture );
        if( FAILED( hr ) )
        {
            m_pBundler->ErrorMsg( "Couldn't load image <%s>\n", strAlphaSource );
            return hr;
        }

        pAlphaTexture->GetSurfaceLevel( 0, &pOrigAlphaSurface );
        pAlphaTexture->Release();
        pOrigAlphaSurface->GetDesc( &AlphaImageDesc );
    }

    // Determine final texture size
    DWORD dwWidth  = ColorImageDesc.Width;
    DWORD dwHeight = ColorImageDesc.Height;
    
    if( strAlphaSource[0] )
    {
        if( dwWidth  < AlphaImageDesc.Width  )   dwWidth  = AlphaImageDesc.Width;
        if( dwHeight < AlphaImageDesc.Height )   dwHeight = AlphaImageDesc.Height;
    }

    // Do the size conversion for the source image
    hr = m_pBundler->m_pd3ddev->CreateImageSurface( dwWidth, dwHeight, D3DFMT_A8R8G8B8,
                                                    &pResizedColorSurface );

    hr = D3DXLoadSurfaceFromSurface( pResizedColorSurface, NULL, NULL, 
                                     pOrigColorSurface, NULL, NULL,
                                     m_dwFilter, 0 );

    if( strAlphaSource[0] )
    {
        // Do the size conversion for the alpha source image
        hr = m_pBundler->m_pd3ddev->CreateImageSurface( dwWidth, dwHeight, D3DFMT_A8R8G8B8,
                                                        &pResizedAlphaSurface );

        hr = D3DXLoadSurfaceFromSurface( pResizedAlphaSurface, NULL, NULL, 
                                         pOrigAlphaSurface, NULL, NULL,
                                         m_dwFilter, 0 );
    }


    // Merge in the alpha channel
    if( strAlphaSource[0] )
    {
        D3DLOCKED_RECT lockColor;
        D3DLOCKED_RECT lockAlpha;
        hr = pResizedColorSurface->LockRect( &lockColor, NULL, 0 );
        hr = pResizedAlphaSurface->LockRect( &lockAlpha, NULL, 0 );

        DWORD* pColorBits = (DWORD*)lockColor.pBits;
        DWORD* pAlphaBits = (DWORD*)lockAlpha.pBits;

        for( DWORD yp = 0; yp < dwHeight; yp++ )
        {
            for( DWORD xp = 0; xp < dwWidth; xp++ )
            {
                DWORD dwAlpha = (*pAlphaBits) << 24;
                (*pColorBits) &= 0x00ffffff;
                (*pColorBits) |= dwAlpha;

                pColorBits++;
                pAlphaBits++;
            }
        }

        pResizedColorSurface->UnlockRect();
        pResizedAlphaSurface->UnlockRect();
    }

    
    // Copy everything over to the output CImage
    (*ppImage) = new CImage( dwWidth, dwHeight, D3DFMT_A8R8G8B8 );
    D3DLOCKED_RECT lr;
    pResizedColorSurface->LockRect( &lr, NULL, D3DLOCK_READONLY );
    memcpy( (*ppImage)->m_pData, lr.pBits, lr.Pitch * dwHeight );
    pResizedColorSurface->UnlockRect();

    // Release the color surfaces
    pOrigColorSurface->Release();
    pResizedColorSurface->Release();

    // Release the alpha surfaces
    if( strAlphaSource[0] )
    {
        pOrigAlphaSurface->Release();
        pResizedAlphaSurface->Release();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ResizeImage()
// Desc: Convert the surface to the new width, height, and format
//-----------------------------------------------------------------------------
HRESULT CBaseTexture::ResizeImage( DWORD dwWidth, DWORD dwHeight, 
                                     CImage** ppImage )
{
    CImage* pOldImage = *ppImage;
    HRESULT hr; 

    *ppImage = new CImage( dwWidth, dwHeight, D3DFMT_A8R8G8B8 );

    hr = LoadImageFromImage( *ppImage, pOldImage, m_dwFilter );

    // Release the old surface
    delete pOldImage;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteSwizzledTextureData()
// Desc: Swizzles the texture data to NVidia's swizzled format.  
//       XGSwizzleRect requires that the memory being swizzled to is 16-byte
//       aligned.
//-----------------------------------------------------------------------------
DWORD CBaseTexture::WriteSwizzledTextureData( VOID* pBits, DWORD dwWidth, 
                                              DWORD dwHeight, DWORD dwDepth )
{
    // Get info about the texture. Note that the pitch of a sizzled texture
    // is Width * BytesPerTexel
    DWORD  dwBytesPerTexel = XGBytesPerPixelFromFormat( D3DFORMAT(g_TextureFormats[m_nFormat].dwXboxFormat) );
    DWORD  dwPitch         = dwWidth*dwBytesPerTexel;
    DWORD  dwTextureSize   = dwPitch * dwHeight * dwDepth;
    BYTE*  pBitsUnaligned  = new BYTE[ dwTextureSize + 16 ];
    int    temp      = (int)pBitsUnaligned;
    BYTE*  pBitsTemp = (BYTE*)(( temp + 15 ) & ~15);   // 16-byte aligned ptr.

    // Swizzle the data into a temporary buffer, and then copy it back
    XGSwizzleBox( pBits, 0, 0, NULL, pBitsTemp, dwWidth, dwHeight, dwDepth,
                  NULL, dwBytesPerTexel );

    if( FAILED( m_pBundler->WriteData( pBitsTemp, dwTextureSize ) ) )
        return 0;

    delete[] pBitsUnaligned;

    return dwTextureSize;
}




//-----------------------------------------------------------------------------
// Name: WriteCompressedTextureData()
// Desc: Compresses the texture data
//-----------------------------------------------------------------------------
DWORD CBaseTexture::WriteCompressedTextureData( VOID* pSrcBits, DWORD dwWidth, 
                                                DWORD dwHeight, DWORD dwDepth )
{
    // Get info about the texture. Note that the pitch of a compressed texture
    // is Width*2 or Width*4, depending on the compression 
    DWORD dwBlockSize;
    DWORD dwCompressedSize;
//    DWORD dwSrcPitch   = (dwWidth*4+D3DTEXTURE_PITCH_ALIGNMENT-1)&~(D3DTEXTURE_PITCH_ALIGNMENT-1);
    DWORD dwSrcPitch   = dwWidth*sizeof(DWORD);
    BOOL  bPreMultiply = g_TextureFormats[m_nFormat].strFormat[10] == '2' ||
                         g_TextureFormats[m_nFormat].strFormat[10] == '4';

    switch( g_TextureFormats[m_nFormat].dwXboxFormat )
    {
        case X_D3DFMT_DXT1: 
            dwBlockSize      = 8;
            dwCompressedSize = dwWidth*dwHeight/2; 
            break;
        case X_D3DFMT_DXT2:
            dwBlockSize      = 16;
            dwCompressedSize = dwWidth*dwHeight; 
            break;
        case X_D3DFMT_DXT4:
            dwBlockSize      = 16;
            dwCompressedSize = dwWidth*dwHeight; 
            break;
        default:
            return E_FAIL;
    }

    // Allocate space for the compressed data
    BYTE* pCompressedData = new BYTE[dwDepth*dwCompressedSize];
    BYTE* pDstBits        = pCompressedData;

    for( DWORD i=0; i<dwDepth; i++ )
    {
        // Compress the data
        if( FAILED( XGCompressRect( pDstBits,
                                    D3DFORMAT(g_TextureFormats[m_nFormat].dwXboxFormat),
                                    0, dwWidth, dwHeight, pSrcBits, 
                                    D3DFORMAT(X_D3DFMT_LIN_A8R8G8B8), 
                                    dwSrcPitch, 0.5,
                                    bPreMultiply ? XGCOMPRESS_PREMULTIPLY : 0 ) ) )
            return 0;

        pSrcBits = (BYTE*)pSrcBits + dwSrcPitch*dwHeight;
        pDstBits = (BYTE*)pDstBits + dwCompressedSize;
    }

    if( dwDepth == 1 )
    {
        // Write the compressed data
        if( FAILED( m_pBundler->WriteData( pCompressedData, dwCompressedSize ) ) )
            return 0;
    }
    else
    {
        // For volume textures, compressed blocks are stored in block linear
        // format, so we need to write them in the correct order.
        DWORD dwNumXBlocks = dwWidth/4;
        DWORD dwNumYBlocks = dwHeight/4;
        DWORD dwNumWBlocks = (dwDepth<4) ? 1 : dwDepth/4;

        for( DWORD w=0; w<dwDepth; w+=4 )
        {
            for( DWORD by=0; by<dwNumYBlocks; by++ )
            {
                for( DWORD bx=0; bx<dwNumXBlocks; bx++ )
                {
                    BYTE* pBits0 = &pCompressedData[ dwBlockSize * ( bx + by*dwNumXBlocks + (w+0)*dwNumXBlocks*dwNumYBlocks ) ];
                    BYTE* pBits1 = &pCompressedData[ dwBlockSize * ( bx + by*dwNumXBlocks + (w+1)*dwNumXBlocks*dwNumYBlocks ) ];
                    BYTE* pBits2 = &pCompressedData[ dwBlockSize * ( bx + by*dwNumXBlocks + (w+2)*dwNumXBlocks*dwNumYBlocks ) ];
                    BYTE* pBits3 = &pCompressedData[ dwBlockSize * ( bx + by*dwNumXBlocks + (w+3)*dwNumXBlocks*dwNumYBlocks ) ];
    
                    // Write the compressed data
                    if( dwDepth >= 1 ) 
                        m_pBundler->WriteData( pBits0, dwBlockSize );
                    if( dwDepth >= 2 ) 
                        m_pBundler->WriteData( pBits1, dwBlockSize );
                    if( dwDepth >= 3 ) 
                        m_pBundler->WriteData( pBits2, dwBlockSize );
                    if( dwDepth >= 4 ) 
                        m_pBundler->WriteData( pBits3, dwBlockSize );
                }
            }
        }
    }

    // Cleanup
    delete[] pCompressedData;

    // Return the number of bytes written
    return dwDepth*dwCompressedSize;
}




//-----------------------------------------------------------------------------
// Name: WriteLinearTextureData()
// Desc: 
//-----------------------------------------------------------------------------
DWORD CBaseTexture::WriteLinearTextureData( VOID* pBits, DWORD dwWidth, 
                                            DWORD dwHeight, DWORD dwDepth )
{
    // Get info about the texture. Note that the pitch of a linear texture
    // needs to be a multiple of 64 bytes.
    DWORD dwBytesPerTexel = XGBytesPerPixelFromFormat( D3DFORMAT(g_TextureFormats[m_nFormat].dwXboxFormat) );
    DWORD dwPitch         = (dwWidth*dwBytesPerTexel+D3DTEXTURE_PITCH_ALIGNMENT-1)&~(D3DTEXTURE_PITCH_ALIGNMENT-1);
    DWORD dwTextureSize   = dwPitch * dwHeight * dwDepth;

    // Write the texture
    if( dwPitch == dwWidth*dwBytesPerTexel )
    {
        if( FAILED( m_pBundler->WriteData( pBits, dwTextureSize ) ) )
            return 0;
    }
    else
    {
        // For textures with non-conforming pitches, write the data aligned
        // according to the pitch
        BYTE* pTempBits = (BYTE*)pBits;
        BYTE  pZeroes[64];
        ZeroMemory( pZeroes, 64 );
        
        for( DWORD z=0; z<dwDepth; z++ )
        {
            for( DWORD y=0; y<dwHeight; y++ )
            {
                // Write data
                if( FAILED( m_pBundler->WriteData( pTempBits, dwWidth*dwBytesPerTexel ) ) )
                    return 0;
                pTempBits += dwWidth*dwBytesPerTexel;

                // Pad to 64-bytes
                if( FAILED( m_pBundler->WriteData( pZeroes, dwPitch - dwWidth*dwBytesPerTexel ) ) )
                    return 0;
            }
        }
    }

    return dwTextureSize;
}




//-----------------------------------------------------------------------------
// Name: SaveImage()
// Desc: Saves the raw surface data to the XPR file
//-----------------------------------------------------------------------------
HRESULT CBaseTexture::SaveImage( DWORD* pcbData, DWORD dwLevels, CImage* pImage )
{
    HRESULT hr;

    // Pad data file to proper alignment for the start of the texture
    hr = m_pBundler->PadToAlignment( D3DTEXTURE_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    DWORD dwWidth = pImage->m_Width;
    DWORD dwHeight = pImage->m_Height;

    // Allocate memory for converting the surface format
    VOID* pSurfaceData = (VOID*)new BYTE[dwWidth*dwHeight*4];

    // Loop over each mipmap level of the texture
    for( DWORD dwLevel = 0; dwLevel < dwLevels; dwLevel++ )
    {
        CImage* pMipmap = new CImage( dwWidth, dwHeight, pImage->m_Format );
        hr = LoadImageFromImage( pMipmap, pImage, m_dwFilter );

        if( g_TextureFormats[m_nFormat].Type == FMT_COMPRESSED )
        {
            // If we're writing a compressed format, compress the texture data
            (*pcbData) += WriteCompressedTextureData( pMipmap->m_pData, dwWidth, dwHeight, 1 );
        }
        else
        {
            // Convert the surface data to the destination format
            ConvertTextureFormat( pMipmap->m_pData, dwWidth, dwHeight, 1,
                                  pSurfaceData, g_TextureFormats[m_nFormat].dwXboxFormat );

            if( g_TextureFormats[m_nFormat].Type == FMT_SWIZZLED )
            {
                // If we're writing a swizzled format, swizzle the texture data
                (*pcbData) += WriteSwizzledTextureData( pSurfaceData, dwWidth, dwHeight, 1 );
            }
            else
            {
                // Write out linear surface data
                (*pcbData) += WriteLinearTextureData( pSurfaceData, dwWidth, dwHeight, 1 );
            }
        }

        // Shrink dimensions down to next lower mipmap level
        if( dwWidth >= 2 )
            dwWidth >>= 1;
        if( dwHeight >= 2 )
            dwHeight >>= 1;

        if( g_TextureFormats[m_nFormat].Type == FMT_COMPRESSED )
        {
            dwWidth  = max( dwWidth, 4 );
            dwHeight = max( dwHeight, 4 );
        }
            
        // Release the mipmap level
    }

    // Cleanup
    delete[] pSurfaceData;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SaveVolumeTexture()
// Desc: Saves the raw surface data to the XPR file
//-----------------------------------------------------------------------------
HRESULT CBaseTexture::SaveVolumeTexture( DWORD* pcbData, DWORD dwLevels, CVolume* pVolume )
{
    HRESULT        hr;

    // Pad data file to proper alignment for the start of the texture
    hr = m_pBundler->PadToAlignment( D3DTEXTURE_ALIGNMENT );
    if( FAILED( hr ) )
        return hr;

    // Get the surface desc, used to create mipmap levels
    DWORD dwWidth  = pVolume->m_Width;
    DWORD dwHeight = pVolume->m_Height;
    DWORD dwDepth  = pVolume->m_Depth;

    // Allocate memory for converting the surface format
    VOID* pVolumeData = (VOID*)new BYTE[dwWidth*dwHeight*dwDepth*4];

    for( DWORD dwLevel = 0; dwLevel < dwLevels; dwLevel++ )
    {
        CVolume* pTempVolume = new CVolume( dwWidth, dwHeight, dwDepth, pVolume->m_Format );

        LoadVolumeFromVolume( pTempVolume, pVolume, m_dwFilter );

        if( g_TextureFormats[m_nFormat].Type == FMT_COMPRESSED )
        {
            // If we're writing a compressed format, compress the texture data
            (*pcbData) += WriteCompressedTextureData( pTempVolume->m_pData, dwWidth, 
                                                      dwHeight, dwDepth );
        }
        else
        {
            // Convert the surface data to the destination format
            ConvertTextureFormat( pTempVolume->m_pData, dwWidth, dwHeight, dwDepth,
                                  pVolumeData, g_TextureFormats[m_nFormat].dwXboxFormat );

            // If we're writing a swizzled format, swizzle the texture data
            (*pcbData) += WriteSwizzledTextureData( pVolumeData, dwWidth, dwHeight, dwDepth );
        }

        // Shrink dimensions down to next lower mipmap level
        if( dwWidth >= 2 )
            dwWidth >>= 1;
        if( dwHeight >= 2 )
            dwHeight >>= 1;
        if( dwDepth >= 2 )
            dwDepth >>= 1;

        if( g_TextureFormats[m_nFormat].Type == FMT_COMPRESSED )
        {
            dwWidth  = max( dwWidth, 4 );
            dwHeight = max( dwHeight, 4 );
        }
            
        // Release the mipmap level
        delete pTempVolume;
    }

    // Cleanup
    delete[] pVolumeData;

    return S_OK;
}


