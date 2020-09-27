/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pixeljar.hpp
 *  Content:    Implementation of D3DPixelContainer.
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//
// Random helpers.
//

//------------------------------------------------------------------------
// Table for determining various attributes of the texture format.
//

#define FMT_RENDERTARGET 0x80
#define FMT_DEPTHBUFFER 0x40
#define FMT_BITSPERPIXEL 0x3c
#define FMT_LINEAR 0x02
#define FMT_SWIZZLED 0x01

#define FMT_32BPP 0x20
#define FMT_16BPP 0x10
#define FMT_8BPP 0x08
#define FMT_4BPP 0x04

D3DCONST BYTE g_TextureFormat[] =
{
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x00 D3DFMT_L8
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x01 D3DFMT_AL8
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x02 D3DFMT_A1R5G5B5
    FMT_16BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x03 D3DFMT_X1R5G5B5
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x04 D3DFMT_A4R4G4B4
    FMT_16BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x05 D3DFMT_R5G6B5
    FMT_32BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x06 D3DFMT_A8R8G8B8
    FMT_32BPP | FMT_RENDERTARGET | FMT_SWIZZLED,  // 0x07 D3DFMT_X8R8G8B8/D3DFMT_X8L8V8U8
    0,                                            // 0x08
    0,                                            // 0x09
    0,                                            // 0x0A
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x0B D3DFMT_P8
    FMT_4BPP,                                     // 0x0C D3DFMT_DXT1
    0,                                            // 0x0D
    FMT_8BPP,                                     // 0x0E D3DFMT_DXT2/D3DFMT_DXT3
    FMT_8BPP,                                     // 0x0F D3DFMT_DXT4/D3DFMT_DXT5
    FMT_16BPP |                    FMT_LINEAR,    // 0x10 D3DFMT_LIN_A1R5G5B5
    FMT_16BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x11 D3DFMT_LIN_R5G6B5
    FMT_32BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x12 D3DFMT_LIN_A8R8G8B8
    FMT_8BPP  | FMT_RENDERTARGET | FMT_LINEAR,    // 0x13 D3DFMT_LIN_L8
    0,                                            // 0x14
    0,                                            // 0x15
    FMT_16BPP |                    FMT_LINEAR,    // 0x16 D3DFMT_LIN_R8B8
    FMT_16BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x17 D3DFMT_LIN_G8B8
    0,                                            // 0x18
    FMT_8BPP  |                    FMT_SWIZZLED,  // 0x19 D3DFMT_A8
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x1A D3DFMT_A8L8
    FMT_8BPP  |                    FMT_LINEAR,    // 0x1B D3DFMT_LIN_AL8
    FMT_16BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x1C D3DFMT_LIN_X1R5G5B5
    FMT_16BPP |                    FMT_LINEAR,    // 0x1D D3DFMT_LIN_A4R4G4B4
    FMT_32BPP | FMT_RENDERTARGET | FMT_LINEAR,    // 0x1E D3DFMT_LIN_X8R8G8B8
    FMT_8BPP  |                    FMT_LINEAR,    // 0x1F D3DFMT_LIN_A8
    FMT_16BPP |                    FMT_LINEAR,    // 0x20 D3DFMT_LIN_A8L8
    0,                                            // 0x21
    0,                                            // 0x22
    0,                                            // 0x23
    FMT_16BPP,                                    // 0x24 D3DFMT_UYVY
    FMT_16BPP,                                    // 0x25 D3DFMT_YUY2
    0,                                            // 0x26
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x27 D3DFMT_R6G5B5/D3DFMT_L6V5U5
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x28 D3DFMT_G8B8/D3DFMT_V8U8
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x29 D3DFMT_R8B8
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2A D3DFMT_D24S8
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2B D3DFMT_F24S8
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2C D3DFMT_D16_LOCKABLE/D3DFMT_D16
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_SWIZZLED,  // 0x2D D3DFMT_F16
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x2E D3DFMT_LIN_D24S8
    FMT_32BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x2F D3DFMT_LIN_F24S8
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x30 D3DFMT_LIN_D16
    FMT_16BPP | FMT_DEPTHBUFFER |  FMT_LINEAR,    // 0x31 D3DFMT_LIN_F16
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x32 D3DFMT_L16
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x33 D3DFMT_V16U16
    0,                                            // 0x34
    FMT_16BPP |                    FMT_LINEAR,    // 0x35 D3DFMT_LIN_L16
    0,                                            // 0x36
    FMT_16BPP |                    FMT_LINEAR,    // 0x37 D3DFMT_LIN_R6G5B5
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x38 D3DFMT_R5G5B5A1
    FMT_16BPP |                    FMT_SWIZZLED,  // 0x39 D3DFMT_R4G4B4A4
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3A D3DFMT_A8B8G8R8/D3DFMT_Q8W8V8U8
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3B D3DFMT_B8G8R8A8
    FMT_32BPP |                    FMT_SWIZZLED,  // 0x3C D3DFMT_R8G8B8A8
    FMT_16BPP |                    FMT_LINEAR,    // 0x3D D3DFMT_LIN_R5G5B5A1
    FMT_16BPP |                    FMT_LINEAR,    // 0x3E D3DFMT_LIN_R4G4B4A4
    FMT_32BPP |                    FMT_LINEAR,    // 0x3F D3DFMT_LIN_A8B8G8R8
    FMT_32BPP |                    FMT_LINEAR,    // 0x40 D3DFMT_LIN_B8G8R8A8
    FMT_32BPP |                    FMT_LINEAR,    // 0x41 D3DFMT_LIN_R8G8B8A8
};

//------------------------------------------------------------------------------
// Permissible tile pitches.
//
D3DCONST DWORD g_TilePitches[] =
{
    D3DTILE_PITCH_0200,
    D3DTILE_PITCH_0300,
    D3DTILE_PITCH_0400,
    D3DTILE_PITCH_0500,
    D3DTILE_PITCH_0600,
    D3DTILE_PITCH_0700,
    D3DTILE_PITCH_0800,
    D3DTILE_PITCH_0A00,
    D3DTILE_PITCH_0C00,
    D3DTILE_PITCH_0E00,
    D3DTILE_PITCH_1000,
    D3DTILE_PITCH_1400,
    D3DTILE_PITCH_1800,
    D3DTILE_PITCH_1C00,
    D3DTILE_PITCH_2000,
    D3DTILE_PITCH_2800,
    D3DTILE_PITCH_3000,
    D3DTILE_PITCH_3800,
    D3DTILE_PITCH_4000,
    D3DTILE_PITCH_5000,
    D3DTILE_PITCH_6000,
    D3DTILE_PITCH_7000,
    D3DTILE_PITCH_8000,
    D3DTILE_PITCH_A000,
    D3DTILE_PITCH_C000,
    D3DTILE_PITCH_E000,
};

//----------------------------------------------------------------------------
// Calculates the log2 of a number that is a power-of-two.
//
DWORD __fastcall Log2(
    DWORD Value
    );

//------------------------------------------------------------------------
// Helper which decodes the D3DFORMAT.
//
inline D3DFORMAT DecodeD3DFORMAT(
    DWORD Format
    )
{
    return (D3DFORMAT)((Format & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT);
}

//------------------------------------------------------------------------
// Gets the log2 width.
//
inline DWORD DecodeLogWidth(
    DWORD Format
    )
{
    return (Format & D3DFORMAT_USIZE_MASK) >> D3DFORMAT_USIZE_SHIFT;
}

//------------------------------------------------------------------------
// Gets the log2 height.
//
inline DWORD DecodeLogHeight(
    DWORD Format
    )
{
    return (Format & D3DFORMAT_VSIZE_MASK) >> D3DFORMAT_VSIZE_SHIFT;
}

//------------------------------------------------------------------------
// Gets the log2 depth.
//
inline DWORD DecodeLogDepth(
    DWORD Format
    )
{
    return (Format & D3DFORMAT_PSIZE_MASK) >> D3DFORMAT_PSIZE_SHIFT;
}

//------------------------------------------------------------------------
// Maps a swizzled D3DFORMAT to its linear counterpart.  This should
// only be used from CDevice::InitializeFrameBuffers as we want to keep
// our lies and gibberish to a minimum.
//
D3DFORMAT MapToLinearD3DFORMAT(
    D3DFORMAT Format
    );

//------------------------------------------------------------------------
// Determines whether this is a valid render target format.
//
inline BOOL IsValidRenderTargetD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_RENDERTARGET;
}

//------------------------------------------------------------------------
// Determines whether this is a valid z-buffer format.
//
inline BOOL IsValidDepthBufferD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_DEPTHBUFFER;
}

//----------------------------------------------------------------------------
// Helper to determine if this is a swizzled texture or not.
//
inline BOOL IsSwizzledD3DFORMAT(
    D3DFORMAT Format
    )
{
    return g_TextureFormat[Format] & FMT_SWIZZLED;
}

//----------------------------------------------------------------------------
// Helper to determine if this is a linear texture or not.
//
inline BOOL IsLinearD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_LINEAR;
}

//------------------------------------------------------------------------
// Determine the number of bits per pixel are required for this format.
//
inline DWORD BitsPerPixelOfD3DFORMAT(
    DWORD Format
    )
{
    return g_TextureFormat[Format] & FMT_BITSPERPIXEL;
}

//------------------------------------------------------------------------
// Determine the number of bits per pixel required for this format for
// video mode changes.
//
DWORD VideoBitsPerPixelOfD3DFORMAT(
    DWORD Format
    );

//----------------------------------------------------------------------------
// Determine if this is a compressed format.
//
bool IsCompressedD3DFORMAT(
    DWORD Format
    );

//------------------------------------------------------------------------
// Get the minimum log2 texture size for a format.
//
inline DWORD MinimumTextureSizeOfD3DFORMAT(
    DWORD Format
    )
{
    return IsCompressedD3DFORMAT(Format) ? 2 : 0;
}

//------------------------------------------------------------------------
// Calculate pitch from width and bpp
//
inline DWORD CalcPitch(
    DWORD Width,
    DWORD TexelSize
    )
{
    return (Width * TexelSize / 8 + D3DTEXTURE_PITCH_ALIGNMENT - 1) &
            ~(D3DTEXTURE_PITCH_ALIGNMENT - 1);
}

//----------------------------------------------------------------------------
// Calculate pitch taking tiling pitch values into account.
//
DWORD CalcTilePitch(
    DWORD Width,
    D3DFORMAT D3DFormat
    );

//
// PixelContainer implementation.
//

//----------------------------------------------------------------------------
// The PixelJar namespace  contains the methods used to implement all of the
// D3D types that contain pixels, including:
//
//      D3DSurface
//      D3DVolume
//      D3DBaseTexture
//      D3DTexture
//      D3DCubeTexture
//      D3DVolumeTexture
//
// All of these methods take an D3DPixelContainer on which to operate.
//
namespace PixelJar
{

    //------------------------------------------------------------------------
    // Differentiates between a surface/volume and a texture Texture instance.
    //
    inline bool IsSurface(
        D3DResource *pResource
        )
    {
        return (pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE;
    }

    //------------------------------------------------------------------------
    // Get the D3DFORMAT for the surface.
    //
    inline D3DFORMAT GetFormat(
        D3DPixelContainer *pPixels
        )
    {
        return (D3DFORMAT)((pPixels->Format & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT);
    }

    //------------------------------------------------------------------------
    // Get the surface format value from a render target and its Zbuffer
    //
    DWORD GetSurfaceFormat(
        D3DPixelContainer *pRenderTarget,
        D3DPixelContainer *pZBuffer
        );

    //------------------------------------------------------------------------
    // Are the pixels in this jar swizzled?
    //
    inline BOOL IsSwizzled(
        D3DPixelContainer *pPixels
        )
    {
        return IsSwizzledD3DFORMAT(GetFormat(pPixels));
    }

    //------------------------------------------------------------------------
    // Can this PixelContainer be used as a render target?
    //
    inline BOOL IsValidRenderTarget(
        D3DPixelContainer *pPixels
        )
    {
        return IsValidRenderTargetD3DFORMAT(GetFormat(pPixels));
    }

    //------------------------------------------------------------------------
    // Can this PixelContainer be used as a z-buffer?
    //
    inline BOOL IsValidDepthBuffer(
        D3DPixelContainer *pPixels
        )
    {
        return IsValidDepthBufferD3DFORMAT(GetFormat(pPixels));
    }

    //------------------------------------------------------------------------
    // Gets the parent for this Surface.  Returns NULL for any jar that
    // doesn't have a parent, including all textures.
    //
    inline D3DBaseTexture *GetParent(
        D3DResource *pResource
        )
    {
        if (PixelJar::IsSurface(pResource))
        {
            return ((D3DSurface *)pResource)->Parent;
        }
        else
        {
            return NULL;
        }
    }

    //------------------------------------------------------------------------
    // Gets the # of mipmap levels in a texture.
    //
    inline DWORD GetMipmapLevelCount(
        D3DPixelContainer *pPixels
        )
    {
        return (pPixels->Format & D3DFORMAT_MIPMAP_MASK) >> D3DFORMAT_MIPMAP_SHIFT;
    }

    //------------------------------------------------------------------------
    // Gets the log2 width.
    //
    inline DWORD GetLogWidth(
        D3DPixelContainer *pPixels
        )
    {
        return DecodeLogWidth(pPixels->Format);
    }

    //------------------------------------------------------------------------
    // Gets the log2 height.
    //
    inline DWORD GetLogHeight(
        D3DPixelContainer *pPixels
        )
    {
        return DecodeLogHeight(pPixels->Format);
    }

    //------------------------------------------------------------------------
    // Gets the log2 depth.
    //
    inline DWORD GetLogDepth(
        D3DPixelContainer *pPixels
        )
    {
        return DecodeLogDepth(pPixels->Format);
    }


    //------------------------------------------------------------------------
    // Get the pixel/texel size of a format.
    //
    inline DWORD GetBitsPerPixel(
        D3DPixelContainer *pPixels
        )
    {
        return BitsPerPixelOfD3DFORMAT(GetFormat(pPixels));
    }

    //----------------------------------------------------------------------------
    // Determine if this is a compressed format.
    //
    inline bool IsCompressed(
        D3DPixelContainer *pPixels
        )
    {
        return IsCompressedD3DFORMAT(GetFormat(pPixels));
    }

    //------------------------------------------------------------------------
    // Calculates the scaling factor for a given depth buffer format.
    //
    void GetDepthBufferScale(
        D3DPixelContainer *pPixels,
        float *pScale
        );

    //------------------------------------------------------------------------
    // Gets the height/width of a pixel container.
    //
    void GetSize(
        D3DPixelContainer *pPixels,
        DWORD Level,
        DWORD *pWidth,
        DWORD *pHeight,
        DWORD *pDepth,
        DWORD *pPitch,
        DWORD *pSlice
        );

    //------------------------------------------------------------------------
    // Decodes the Pitch of a surface from the Size field.
    //
    __forceinline DWORD PitchFromSize(
        DWORD Size
        )
    {
        return (((Size & D3DSIZE_PITCH_MASK) >> D3DSIZE_PITCH_SHIFT) + 1) * D3DTEXTURE_PITCH_ALIGNMENT;
    }

    //------------------------------------------------------------------------
    // Gets the width of a linear surface.
    //
    __forceinline DWORD GetLinearWidth(
        D3DPixelContainer *pPixels
        )
    {
        ASSERT(pPixels->Size != 0);

        return (pPixels->Size & D3DSIZE_WIDTH_MASK) + 1;
    }

    //------------------------------------------------------------------------
    // Gets the height of a linear surface.
    //
    __forceinline DWORD GetLinearHeight(
        D3DPixelContainer *pPixels
        )
    {
        ASSERT(pPixels->Size != 0);

        return ((pPixels->Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
    }

    //------------------------------------------------------------------------
    // Gets the pitch of a linear surface.
    //
    inline DWORD GetLinearPitch(
        D3DPixelContainer *pPixels
        )
    {
        ASSERT(pPixels->Size != 0);

        return PitchFromSize(pPixels->Size);
    }

    //------------------------------------------------------------------------
    // Gets the width of a pixel container.
    //
    __forceinline DWORD GetWidth(
        D3DPixelContainer *pPixels
        )
    {
        DWORD Size = pPixels->Size;

        if (!Size)
        {
            return (1 << GetLogWidth(pPixels));
        }
        else
        {
            return GetLinearWidth(pPixels);
        }
    }

    //------------------------------------------------------------------------
    // Gets the height of a pixel container.
    //
    __forceinline DWORD GetHeight(
        D3DPixelContainer *pPixels
        )
    {
        DWORD Size = pPixels->Size;

        if (!Size)
        {
            return (1 << GetLogHeight(pPixels));
        }
        else
        {
            return GetLinearHeight(pPixels);
        }
    }

    //------------------------------------------------------------------------
    // Gets the depth of a pixel container.
    //
    __forceinline DWORD GetDepth(
        D3DPixelContainer *pPixels
        )
    {
        DWORD Size = pPixels->Size;

        if (!Size)
        {
            return (1 << GetLogDepth(pPixels));
        }
        else
        {
            return 1;
        }
    }

    //------------------------------------------------------------------------
    // Gets the pitch of a pixel container.
    //
    inline DWORD GetPitch(
        D3DPixelContainer *pPixels
        )
    {
        DWORD Size = pPixels->Size;

        if (!Size)
        {
            DWORD Width = 1 << GetLogWidth(pPixels);

            switch(GetFormat(pPixels))
            {
            case D3DFMT_DXT1:
                return Width * 2;

            case D3DFMT_DXT2:
            case D3DFMT_DXT4:
                return Width * 4;

            default:
                return Width * GetBitsPerPixel(pPixels) / 8;
            }
        }
        else
        {
            return GetLinearPitch(pPixels);
        }
    }

    //------------------------------------------------------------------------
    // Gets the slice size of the surface.  Probably should be uninlined.
    //
    inline DWORD GetSlice(
        D3DPixelContainer *pPixels
        )
    {
        DWORD Size = pPixels->Size;
        DWORD BitsPerPixel = PixelJar::GetBitsPerPixel(pPixels);
        DWORD LogMin = MinimumTextureSizeOfD3DFORMAT(GetFormat(pPixels));

        if (!Size)
        {
            DWORD LogWidth = max(LogMin, GetLogWidth(pPixels));
            DWORD LogHeight = max(LogMin, GetLogHeight(pPixels));

            return (1 << (LogWidth + LogHeight)) * BitsPerPixel / 8;
        }
        else
        {
            return PitchFromSize(Size)
                   * (/* GetHeight */((Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1);
        }
    }

    //------------------------------------------------------------------------------
    // Gets the surface desc information for a mipmap level.
    //
    void Get2DSurfaceDesc(
        D3DPixelContainer *pPixels,
        UINT Level,
        D3DSURFACE_DESC *pDesc
        );

    //------------------------------------------------------------------------------
    // Gets the surface desc information for a mipmap level.  This does not
    // set the type.
    //
    void Get3DSurfaceDesc(
        D3DPixelContainer *pPixels,
        UINT Level,
        D3DVOLUME_DESC *pDesc
        );

    //------------------------------------------------------------------------------
    // Finds the location of a surface within a texture.
    //
    void FindSurfaceWithinTexture(
        D3DPixelContainer *pPixels,
        D3DCUBEMAP_FACES FaceType,
        UINT Level,
        BYTE **ppbData,
        DWORD *pRowPitch,
        DWORD *pSlicePitch,
        DWORD *pFormat,
        DWORD *pSize
        );

    //------------------------------------------------------------------------------
    // Lock a 2D surface.
    //
    void Lock2DSurface(
        D3DPixelContainer *pPixels,
        D3DCUBEMAP_FACES FaceType,
        UINT Level,
        D3DLOCKED_RECT *pLockedRect,
        CONST RECT *pRect,
        DWORD Flags
        );

    //------------------------------------------------------------------------------
    // Lock a 3D surface.
    //
    void Lock3DSurface(
        D3DPixelContainer *pPixels,
        UINT Level,
        D3DLOCKED_BOX *pLockedVolume,
        CONST D3DBOX *pBox,
        DWORD Flags
        );

    //------------------------------------------------------------------------------
    // Encodes the Format and Size parameters for a surface and returns the number
    // of bytes required to the data for the surface.
    //
    DWORD EncodeFormat(
        DWORD Width,
        DWORD Height,
        DWORD Depth,
        DWORD Levels,
        D3DFORMAT D3DFormat,
        DWORD Pitch,
        bool isCubeMap,
        bool isVolume,
        DWORD *pFormat,
        DWORD *pSize
        );
};

} // end namespace
