//-----------------------------------------------------------------------------
// File: BaseTexture.h
//
// Desc: Header file containing class, struct, and constant definitions
//       for the basic texture-related functionality of the bundler tool
//
// Hist: 04.13.01 - New for May XDK release
//       01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef BASETEXTURE_H
#define BASETEXTURE_H

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>


class CBundler;
class CImage;
class CVolume;


// Xbox-specific defines, taken from d3d8types-xbox.h and d3d8-xbox.h
#define X_D3DFMT_A8R8G8B8              0x00000006
#define X_D3DFMT_X8R8G8B8              0x00000007
#define X_D3DFMT_R5G6B5                0x00000005
#define X_D3DFMT_R6G5B5                0x00000027
#define X_D3DFMT_X1R5G5B5              0x00000003
#define X_D3DFMT_A1R5G5B5              0x00000002
#define X_D3DFMT_A4R4G4B4              0x00000004
#define X_D3DFMT_A8                    0x00000019
#define X_D3DFMT_A8B8G8R8              0x0000003A   
#define X_D3DFMT_B8G8R8A8              0x0000003B   
#define X_D3DFMT_R4G4B4A4              0x00000039   
#define X_D3DFMT_R5G5B5A1              0x00000038   
#define X_D3DFMT_R8G8B8A8              0x0000003C   
#define X_D3DFMT_R8B8                  0x00000029   
#define X_D3DFMT_G8B8                  0x00000028   
#define X_D3DFMT_P8                    0x0000000B
#define X_D3DFMT_L8                    0x00000000
#define X_D3DFMT_A8L8                  0x0000001A
#define X_D3DFMT_AL8                   0x00000001   
#define X_D3DFMT_L16                   0x00000032   
#define X_D3DFMT_V8U8                  0x00000028
#define X_D3DFMT_L6V5U5                0x00000027
#define X_D3DFMT_X8L8V8U8              0x00000007
#define X_D3DFMT_Q8W8V8U8              0x0000003A
#define X_D3DFMT_V16U16                0x00000033
#define X_D3DFMT_DXT1                  0x0000000C
#define X_D3DFMT_DXT2                  0x0000000E
#define X_D3DFMT_DXT3                  0x0000000E // Same constant as DXT2
#define X_D3DFMT_DXT4                  0x0000000F
#define X_D3DFMT_DXT5                  0x0000000F // Same constant as DXT4
#define X_D3DFMT_LIN_A1R5G5B5          0x00000010   
#define X_D3DFMT_LIN_A4R4G4B4          0x0000001D   
#define X_D3DFMT_LIN_A8                0x0000001F   
#define X_D3DFMT_LIN_A8B8G8R8          0x0000003F   
#define X_D3DFMT_LIN_A8R8G8B8          0x00000012   
#define X_D3DFMT_LIN_B8G8R8A8          0x00000040   
#define X_D3DFMT_LIN_G8B8              0x00000017   
#define X_D3DFMT_LIN_R4G4B4A4          0x0000003E   
#define X_D3DFMT_LIN_R5G5B5A1          0x0000003D   
#define X_D3DFMT_LIN_R5G6B5            0x00000011   
#define X_D3DFMT_LIN_R6G5B5            0x00000037   
#define X_D3DFMT_LIN_R8B8              0x00000016   
#define X_D3DFMT_LIN_R8G8B8A8          0x00000041   
#define X_D3DFMT_LIN_X1R5G5B5          0x0000001C   
#define X_D3DFMT_LIN_X8R8G8B8          0x0000001E   
#define X_D3DFMT_LIN_A8L8              0x00000020   
#define X_D3DFMT_LIN_AL8               0x0000001B   
#define X_D3DFMT_LIN_L16               0x00000035   
#define X_D3DFMT_LIN_L8                0x00000013   
#define X_D3DFMT_LIN_V16U16            0x00000036
#define X_D3DFMT_LIN_V8U8              0x00000017
#define X_D3DFMT_LIN_L6V5U5            0x00000037
#define X_D3DFMT_LIN_X8L8V8U8          0x0000001E
#define X_D3DFMT_LIN_Q8W8V8U8          0x00000012

#define D3DTEXTURE_ALIGNMENT            128

#define D3DFORMAT_DMACHANNEL_MASK       0x00000003
#define D3DFORMAT_DMACHANNEL_A          0x00000001
#define D3DFORMAT_DMACHANNEL_B          0x00000002

#define D3DFORMAT_CUBEMAP               0x00000004
#define D3DFORMAT_BORDERSOURCE_COLOR    0x00000008
#define D3DFORMAT_DIMENSION_MASK        0x000000F0
#define D3DFORMAT_DIMENSION_SHIFT       4
#define D3DFORMAT_FORMAT_MASK           0x0000FF00
#define D3DFORMAT_FORMAT_SHIFT          8
#define D3DFORMAT_MIPMAP_MASK           0x000F0000
#define D3DFORMAT_MIPMAP_SHIFT          16
#define D3DFORMAT_USIZE_MASK            0x00F00000
#define D3DFORMAT_USIZE_SHIFT           20
#define D3DFORMAT_VSIZE_MASK            0x0F000000
#define D3DFORMAT_VSIZE_SHIFT           24
#define D3DFORMAT_PSIZE_MASK            0xF0000000
#define D3DFORMAT_PSIZE_SHIFT           28

#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
#define D3DCOMMON_VIDEOMEMORY        0x00800000

#define D3DTEXTURE_PITCH_ALIGNMENT   64


// Texture Resource Format
struct D3DTexture
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
    DWORD Format;
    DWORD Size;
};


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
