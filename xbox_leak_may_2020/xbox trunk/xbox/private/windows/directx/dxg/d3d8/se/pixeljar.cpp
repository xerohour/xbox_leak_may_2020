/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       pixeljar.cpp
 *  Content:    Implementation for all D3D classes that contain pixels.
 *
 ***************************************************************************/

#include "precomp.hpp"      

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//
// Public helper methods
//

//---------------------------------------------------------------------------- 
// Calculates the log2 of a number that is a power-of-two.
//
DWORD __fastcall Log2(
    DWORD Value
    )
{
    ASSERT(Value);
    ASSERT((Value & (Value - 1)) == 0);

    // The asserts force the parameter out of ecx, reload it manually.
    __asm 
    {
        mov ecx, [Value]
        bsf eax,ecx 
    }
}

//----------------------------------------------------------------------------
// Maps a swizzled D3DFORMAT to its linear counterpart.  This should
// only be used from CDevice::InitializeFrameBuffers as we want to keep
// our lies and gibberish to a minimum.
//
D3DFORMAT MapToLinearD3DFORMAT(
    D3DFORMAT Format
    )
{
    switch (Format)
    {
    case D3DFMT_A8R8G8B8:
        Format = D3DFMT_LIN_A8R8G8B8;
        break;

    case D3DFMT_X8R8G8B8:
        Format = D3DFMT_LIN_X8R8G8B8;
        break;

    case D3DFMT_R5G6B5:
        Format = D3DFMT_LIN_R5G6B5;
        break;

    case D3DFMT_X1R5G5B5:
        Format = D3DFMT_LIN_X1R5G5B5;
        break;

    case D3DFMT_A1R5G5B5:
        Format = D3DFMT_LIN_A1R5G5B5;
        break;

    case D3DFMT_D16:
        Format = D3DFMT_LIN_D16;
        break;

    case D3DFMT_D24S8:
        Format = D3DFMT_LIN_D24S8;
        break;

    case D3DFMT_F16:
        Format = D3DFMT_LIN_F16;
        break;

    case D3DFMT_F24S8:
        Format = D3DFMT_LIN_F24S8;
        break;
    }

    return Format;
}

//------------------------------------------------------------------------
// Determine the number of bits per pixel required for this format for
// video mode changes.
//
DWORD VideoBitsPerPixelOfD3DFORMAT(
    DWORD Format
    )
{
    DWORD bitsPerPixel;

    if ((Format == D3DFMT_X1R5G5B5) || (Format == D3DFMT_LIN_X1R5G5B5))
    {
        bitsPerPixel = 15;
    }
    else
    {
        bitsPerPixel = BitsPerPixelOfD3DFORMAT(Format);
    }

    return bitsPerPixel;
}

//
// Private helpers
//

//----------------------------------------------------------------------------
// Determine if this is a compressed format.
//
bool IsCompressedD3DFORMAT(
    DWORD Format
    )
{
    bool retval = false;

    switch(Format)
    {
    case D3DFMT_DXT1:
    case D3DFMT_DXT2:  // DXT3
    case D3DFMT_DXT4:  // DXT4
        retval = true;
        break;
    }

    return retval;
}

//----------------------------------------------------------------------------
// Calculate pitch taking tiling pitch values into account
//
DWORD CalcTilePitch(
    DWORD Width,
    D3DFORMAT D3DFormat
    )
{
    DWORD Pitch, i;

    Pitch = CalcPitch(Width, BitsPerPixelOfD3DFORMAT(D3DFormat));

    for (i = 0; i < ARRAYSIZE(g_TilePitches); i++)
    {
        if (Pitch <= g_TilePitches[i])
        {
            Pitch = g_TilePitches[i];
            break;
        }
    }

    return Pitch;
}

//
// PixelContainer implementation
//

namespace PixelJar
{

    //------------------------------------------------------------------------
    // Get the surface format value from a render target and its Zbuffer
    //
    DWORD GetSurfaceFormat(
        D3DPixelContainer *pRenderTarget,
        D3DPixelContainer *pZBuffer // Can be NULL
        )
    {
        DWORD Format;
        bool Swizzled;

        switch(GetFormat(pRenderTarget))
        {
        case D3DFMT_A8R8G8B8: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8; 
            Swizzled = TRUE;
            break;

        case D3DFMT_LIN_A8R8G8B8: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8; 
            Swizzled = FALSE;
            break;

        case D3DFMT_X8R8G8B8: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8; 
            Swizzled = TRUE;
            break;

        case D3DFMT_LIN_X8R8G8B8: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8; 
            Swizzled = FALSE;
            break;

        case D3DFMT_R5G6B5:   
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5; 
            Swizzled = TRUE;
            break;

        case D3DFMT_LIN_R5G6B5:   
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5; 
            Swizzled = FALSE;
            break;

        case D3DFMT_X1R5G5B5: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5; 
            Swizzled = TRUE;
            break;

        case D3DFMT_LIN_X1R5G5B5: 
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5; 
            Swizzled = FALSE;
            break;

        case D3DFMT_LIN_L8:
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_B8; 
            Swizzled = FALSE;
            break;

        case D3DFMT_LIN_G8B8:
            Format = NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8;
            Swizzled = FALSE;
            break;

        default: 
            NODEFAULT("Unexpected format\n"); 
            break;
        }

        if (Swizzled)
        {
            Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _SWIZZLE)
                    | DRF_NUM(097, _SET_SURFACE_FORMAT, _WIDTH, 
                              PixelJar::GetLogWidth(pRenderTarget))
                    | DRF_NUM(097, _SET_SURFACE_FORMAT, _HEIGHT, 
                              PixelJar::GetLogHeight(pRenderTarget));
        }
        else
        {
            Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _PITCH);
        }

        if (pZBuffer)
        {
            switch(GetFormat(pZBuffer))
            {
            case D3DFMT_D16:
            case D3DFMT_F16:
            case D3DFMT_LIN_D16:
            case D3DFMT_LIN_F16:
                Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _ZETA, _Z16);
                break;

            case D3DFMT_D24S8:
            case D3DFMT_F24S8:
            case D3DFMT_LIN_D24S8:
            case D3DFMT_LIN_F24S8:
                Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _ZETA, _Z24S8);
                break;
            }
        }

        // If the ZBuffer is not set then the bpp of the zbuffer must be
        // set to be the same as the render target, even though this value
        // will never be used.
        //
        else if (GetBitsPerPixel(pRenderTarget) == 32)
        {
            Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _ZETA, _Z24S8);
        }
        else
        {
            Format |= DRF_DEF(097, _SET_SURFACE_FORMAT, _ZETA, _Z16);
        }

        return Format;
    }

    //------------------------------------------------------------------------
    // Calculates the scaling factor for a given depth buffer format.
    //
    void GetDepthBufferScale(
        D3DPixelContainer *pPixels,
        float *pScale
        )
    {
        ASSERT(IsValidDepthBufferD3DFORMAT(GetFormat(pPixels)));

        switch(GetFormat(pPixels))
        {
        case D3DFMT_D16:
        case D3DFMT_LIN_D16:
            *pScale = (float)D3DZ_MAX_D16;
            break;

        case D3DFMT_D24S8:
        case D3DFMT_LIN_D24S8:
            *pScale = (float)D3DZ_MAX_D24S8;
            break;
            
        case D3DFMT_F16:
        case D3DFMT_LIN_F16:
            *pScale = (float)D3DZ_MAX_F16;
            break;

        case D3DFMT_F24S8:
        case D3DFMT_LIN_F24S8:
            *pScale = (float)D3DZ_MAX_F24S8;
            break;

        default:
            NODEFAULT("PixelJar::GetDepthBufferScale - unknown depth buffer format\n");
        }
    }

    //------------------------------------------------------------------------
    // Figures out the width/height of both power-of-2 and non-power-of-2 
    // surfaces, in pixels.
    //
    void GetSize(
        D3DPixelContainer *pPixels,
        DWORD Level,
        DWORD *pWidth,
        DWORD *pHeight,
        DWORD *pDepth,
        DWORD *pPitch,
        DWORD *pSlice
        )
    {
        DWORD Size = pPixels->Size;

        if (DBG_CHECK(Level >= PixelJar::GetMipmapLevelCount(pPixels)))
        {
            DXGRIP("PixelJar::GetSize - invalid mipmap level.");
        }

        if (!Size)
        {
            long LogWidth = GetLogWidth(pPixels) - Level;
            long LogHeight = GetLogHeight(pPixels) - Level;
            long LogDepth = GetLogDepth(pPixels) - Level;

            DWORD BitsPerPixel = GetBitsPerPixel(pPixels);

            *pHeight = 1 << (LogHeight > 0 ? LogHeight : 0);
            *pDepth = 1 << (LogDepth > 0 ? LogDepth : 0);
            *pWidth = 1 << (LogWidth > 0 ? LogWidth : 0);

            long LogMin = (long)MinimumTextureSizeOfD3DFORMAT(GetFormat(pPixels));

            DWORD Width = 1 << (max(LogMin, LogWidth));
            DWORD Height = 1 << (max(LogMin, LogHeight));

            switch(GetFormat(pPixels))
            {
            case D3DFMT_DXT1:
                *pPitch = Width * 2;
                break;

            case D3DFMT_DXT2:
            case D3DFMT_DXT4:
                *pPitch = Width * 4;
                break;

            default:
                *pPitch = Width * BitsPerPixel / 8;
            }

            *pSlice = Width * Height * BitsPerPixel / 8;
        }
        else
        {
            *pWidth = (Size & D3DSIZE_WIDTH_MASK) + 1;
            *pHeight = ((Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
            *pDepth = 1;
            *pPitch = PitchFromSize(Size);
            *pSlice = *pPitch * *pHeight;
        }
    }

    //------------------------------------------------------------------------------
    // Gets the surface desc information for a mipmap level.  This does not
    // set the type.
    //
    void Get2DSurfaceDesc(
        D3DPixelContainer *pPixels,
        UINT Level, 
        D3DSURFACE_DESC *pDesc
        )
    {
        if (DBG_CHECK(Level >= GetMipmapLevelCount(pPixels)))
        {
            DXGRIP("PixelJar::Get2DSurfaceDesc - invalid mipmap level.");
        }

        pDesc->Format = GetFormat(pPixels);
        pDesc->Type = pPixels->GetType();
        pDesc->Usage = 0;

        if (Level == 0)
        {
            if (IsValidRenderTarget(pPixels))
            {
                pDesc->Usage = D3DUSAGE_RENDERTARGET;
            }
            else if (IsValidDepthBuffer(pPixels))
            {
                pDesc->Usage = D3DUSAGE_DEPTHSTENCIL;
            }
        }

        // We don't store any multisampling attribute with the surface.
        // On the other hand, the only multisampling surface we allow is
        // the active back-buffer, so this is easy to check.
        //
        if (pPixels->Data == g_pDevice->m_pFrameBuffer[0]->Data)
        {
            pDesc->MultiSampleType = g_pDevice->m_MultiSampleType;
        }
        else
        {
            pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
        }

        DWORD Width, Height, Depth, Pitch, Slice;

        GetSize(pPixels, Level, &Width, &Height, &Depth, &Pitch, &Slice);

        pDesc->Width = Width;
        pDesc->Height = Height;
        pDesc->Size = Slice;
    }

    //------------------------------------------------------------------------------
    // Gets the surface desc information for a mipmap level.  This does not
    // set the type.
    //
    void Get3DSurfaceDesc(
        D3DPixelContainer *pPixels,
        UINT Level, 
        D3DVOLUME_DESC *pDesc
        )
    {
        if (DBG_CHECK(Level >= GetMipmapLevelCount(pPixels)))
        {
            DXGRIP("PixelJar::Get3DSurfaceDesc - invalid mipmap level.");
        }

        pDesc->Format = GetFormat(pPixels);
        pDesc->Type = pPixels->GetType();;
        pDesc->Usage = 0;

        DWORD Width, Height, Depth, Pitch, Slice;

        GetSize(pPixels, Level, &Width, &Height, &Depth, &Pitch, &Slice);

        pDesc->Width = Width;
        pDesc->Height = Height;
        pDesc->Depth = Depth;
        pDesc->Size = Slice * Depth;
    }

    //------------------------------------------------------------------------------
    // Finds the location of a surface within a texture.
    //
    // The Log sizes will return 0 for a non power-of-2 texture.
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
        )
    {
        BYTE *pbData = (BYTE *)GetDataFromResource(pPixels);

        // Figure out exactly the data we want to lock.
        DWORD TexelSize = GetBitsPerPixel(pPixels);
        DWORD RowPitch, SlicePitch;

        DWORD Size = pPixels->Size;

        // Power-of-2
        if (!Size)
        {
            DWORD TextureLogWidth = GetLogWidth(pPixels);
            DWORD TextureLogHeight = GetLogHeight(pPixels);
            DWORD TextureLogDepth = GetLogDepth(pPixels);

            DWORD LogMin = MinimumTextureSizeOfD3DFORMAT(GetFormat(pPixels));

            // Map to the right face.
            if (FaceType != D3DCUBEMAP_FACE_POSITIVE_X)
            {
                // Decode the size of each face.
                DWORD cLevels = GetMipmapLevelCount(pPixels);
                DWORD LogWidth = TextureLogWidth;
                DWORD LogHeight = TextureLogHeight;
                DWORD LogDepth = TextureLogDepth;

                DWORD FaceSize = 0;

                for (; cLevels; cLevels--)
                {
                    FaceSize += (1 << (max(LogWidth, LogMin) + max(LogHeight, LogMin))) * TexelSize / 8;

                    if (LogWidth > 0)
                    {
                        LogWidth--;
                    }

                    if (LogHeight > 0)
                    {
                        LogHeight--;
                    }

                    if (LogDepth > 0)
                    {
                        LogDepth--;
                    }
                }

                // Align the size.
                FaceSize = (FaceSize + D3DTEXTURE_CUBEFACE_ALIGNMENT - 1) & ~(D3DTEXTURE_CUBEFACE_ALIGNMENT - 1);

                // Move to the correct face.
                pbData += FaceSize * FaceType;
            }

            // Map to the right level.
            if (Level)
            {
                DWORD cLevels = Level;
                DWORD LogSize;

                for (; cLevels; cLevels--)
                {
                    LogSize = max(TextureLogWidth, LogMin) + max(TextureLogHeight, LogMin) + TextureLogDepth;

                    pbData += (1 << LogSize) * TexelSize / 8;

                    if (TextureLogWidth > 0)
                    {
                        TextureLogWidth--;
                    }

                    if (TextureLogHeight > 0)
                    {
                        TextureLogHeight--;
                    }

                    if (TextureLogDepth > 0)
                    {
                        TextureLogDepth--;
                    }
                }
            }

            // Munge the format to reflect the new size.
            DWORD Format = pPixels->Format & ~(D3DFORMAT_USIZE_MASK | D3DFORMAT_VSIZE_MASK | D3DFORMAT_PSIZE_MASK);

            Format |= TextureLogWidth << D3DFORMAT_USIZE_SHIFT;
            Format |= TextureLogHeight << D3DFORMAT_VSIZE_SHIFT;
            Format |= TextureLogDepth << D3DFORMAT_PSIZE_SHIFT;

            TextureLogWidth = max(TextureLogWidth, LogMin);
            TextureLogHeight = max(TextureLogHeight, LogMin);

            switch(GetFormat(pPixels))
            {
            case D3DFMT_DXT1:
                RowPitch = (1 << TextureLogWidth) * 2;
                break;

            case D3DFMT_DXT2:
            case D3DFMT_DXT4:
                RowPitch = (1 << TextureLogWidth) * 4;
                break;

            default:
                RowPitch = (1 << TextureLogWidth) * TexelSize / 8;
            }

            SlicePitch = (1 << (TextureLogWidth + TextureLogHeight)) * TexelSize / 8;

            *pFormat = Format;
            *pSize = pPixels->Size;
        }

        // Non power-of-2.
        else
        {
            DWORD TextureWidth = (Size & D3DSIZE_WIDTH_MASK) + 1;
            DWORD TextureHeight = ((Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
            DWORD Pitch = PitchFromSize(Size);

            RowPitch = Pitch;
            SlicePitch = RowPitch * TextureHeight;

            *pFormat = pPixels->Format;
            *pSize = pPixels->Size;
        }

        //
        // Return the results.
        //

        *ppbData = pbData;
        *pRowPitch = RowPitch;
        *pSlicePitch = SlicePitch;
    }

    //------------------------------------------------------------------------
    // Do all of the locking goo for a chunk of data.  The caller must ensure
    // that we are free to party on the memory.
    // 
    static void LockSurface(
        D3DPixelContainer *pPixels,
        D3DCUBEMAP_FACES FaceType, 
        UINT Level, 
        DWORD Flags,
        BYTE **ppbData,
        DWORD *pRowPitch,
        DWORD *pSlicePitch
        )
    {
        // Validate parameters.
        if (DBG_CHECK(TRUE))
        {
            if (Level >= GetMipmapLevelCount(pPixels))
            {
                DXGRIP("PixelJar::LockSurface - invalid mipmap level.");
            }
            if (Flags & ~(D3DLOCK_TILED | D3DLOCK_NOOVERWRITE | D3DLOCK_READONLY))
            {
                DXGRIP("PixelJar::LockSurface - invalid flag.");
            }
        }

        // Make sure the GPU isn't using this texture if we don't already have it
        // locked.
        if (!(Flags & D3DLOCK_NOOVERWRITE))
        {
            BlockOnResource(pPixels);
        }

        DWORD Format, Size;

        // Get a pointer to the actual surface.
        FindSurfaceWithinTexture(pPixels,
                                 FaceType, 
                                 Level, 
                                 ppbData,
                                 pRowPitch,
                                 pSlicePitch,
                                 &Format,
                                 &Size);

        // On DVT systems, give a pointer to the slower, but tiling-coherent,
        // mapping if requested:
        if (Flags & D3DLOCK_TILED)
        {
            *ppbData = (BYTE*) (((DWORD) *ppbData & ~0xF0000000) 
                                 | AGP_FRAME_BUFFER_HIGH_NIBBLE);
        }
    }

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
        )
    {
        DWORD RowPitch;
        DWORD SlicePitch;

        BYTE *pbData;

        if (DBG_CHECK(pRect))
        {
            DWORD Width, Height, Depth, Pitch, Slice;
            GetSize(pPixels, Level, &Width, &Height, &Depth, &Pitch, &Slice);

            // This exclusive check doesn't seem right but D3DX causes use to hit
            // this parameter check if we look for the inclusive ranges.  
            //
            if ((pRect->bottom > (long)Height) || (pRect->right > (long)Width))
            {
                DXGRIP("PixelJar::Lock2DSurface - Lock rectangle is beyond the bounds of the texture level.");
            }
            if ((pRect->top < 0) || (pRect->left < 0))
            {
                DXGRIP("PixelJar::Lock2DSurface - Lock rectangle is negative.");
            }
        }

        LockSurface(pPixels,
                    FaceType,
                    Level,
                    Flags,
                    &pbData,
                    &RowPitch,
                    &SlicePitch);

        // Move to the actual rectangle being locked.
        if (pRect)
        {
            DWORD TexelSize = GetBitsPerPixel(pPixels);

            pbData += pRect->top * RowPitch + pRect->left * TexelSize / 8;
        }

        // Prep it to be returned.
        pLockedRect->Pitch = RowPitch;
        pLockedRect->pBits = pbData;
    }

    //------------------------------------------------------------------------------
    // Lock a 3D surface.
    //
    void Lock3DSurface(
        D3DPixelContainer *pPixels,
        UINT Level, 
        D3DLOCKED_BOX *pLockedVolume, 
        CONST D3DBOX *pBox, 
        DWORD Flags
        )
    {
        DWORD RowPitch;
        DWORD SlicePitch;

        BYTE *pbData;

        if (DBG_CHECK(pBox))
        {
            DWORD Width, Height, Depth, Pitch, Slice;
            GetSize(pPixels, Level, &Width, &Height, &Depth, &Pitch, &Slice);

            if (   pBox->Bottom > Height
                || pBox->Right > Width
                || pBox->Back > Depth)
            {
                DXGRIP("PixelJar::Lock3DSurface - lock box is beyond the bounds of the texture level.");
            }
        }

        LockSurface(pPixels,
                    D3DCUBEMAP_FACE_POSITIVE_X,
                    Level, 
                    Flags,
                    &pbData,
                    &RowPitch,
                    &SlicePitch);

        // Move to the actual rectangle being locked.
        if (pBox)
        {
            DWORD TexelSize = GetBitsPerPixel(pPixels);

            pbData += pBox->Front * SlicePitch + pBox->Top * RowPitch + pBox->Left * TexelSize / 8;
        }

        // Prep it to be returned.
        pLockedVolume->RowPitch = RowPitch;
        pLockedVolume->SlicePitch = SlicePitch;
        pLockedVolume->pBits = pbData;
    }

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
        )
    {
        if (DBG_CHECK(TRUE))
        {
            if (!Width || !Height || !Depth)
            {
                DXGRIP("PixelJar::EncodeFormat - All texture dimensions must be at least 1 texel.");
            }

            if (IsSwizzledD3DFORMAT(D3DFormat) || IsCompressedD3DFORMAT(D3DFormat))
            {
                if (Depth > 1)
                {
                    if (Width > 512)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Volume texture width is larger than the 512 texel maximum.");
                    }

                    if (Height > 512)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Volume texture height is larger than the 512 texel maximum.");
                    }

                    if (Depth > 512)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Volume texture depth is larger than the 512 texel maximum.");
                    }

                    if (isCubeMap)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Volume textures cannot be cube mapped.");
                    }

                    if (IsValidDepthBufferD3DFORMAT(D3DFormat))
                    {
                        DXGRIP("PixelJar::EncodeFormat - Volume textures cannot have a depth format.");
                    }
                }
                else
                {
                    if (Width > 4096)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Texture width is larger than the 4096 texel maximum.");
                    }

                    if (Height > 4096)
                    {
                        DXGRIP("PixelJar::EncodeFormat - Texture height is larger than the 4096 texel maximum.");
                    }
                }

                if ((Width & (Width - 1)) || (Height & (Height - 1)) || (Depth & (Depth - 1)))
                {
                    DXGRIP("PixelJar::EncodeFormat - Each dimension of a swizzled texture must be a power of 2.");
                }

                if (Levels > 16)
                {
                    DXGRIP("PixelJar::EncodeFormat - Texture mipmap level count is larger than the 16 level maximum.");
                }

                if (isCubeMap && IsValidDepthBufferD3DFORMAT(D3DFormat))
                {
                    DXGRIP("PixelJar::EncodeFormat - Depth format textures cannot be cube mapped.");
                }

                if (isCubeMap && Width != Height)
                {
                    DXGRIP("PixelJar::EncodeFormat - Cube maps must be square (width == height).");
                }
            }
            else
            {
                if (Depth != 1)
                {
                    DXGRIP("PixelJar::EncodeFormat - Non swizzled and non compressed textures cannot be a volume texture.");
                }

                if (Width > 4096)
                {
                    DXGRIP("PixelJar::EncodeFormat - Texture width is larger than the 4096 texel maximum.");
                }

                if (Height > 4096)
                {
                    DXGRIP("PixelJar::EncodeFormat - Texture height is larger than the 4096 texel maximum.");
                }

                if (Levels > 1)
                {
                    DXGRIP("PixelJar::EncodeFormat - Non swizzled and non compressed textures cannot be mip mapped.");
                }

                if (isCubeMap)
                {
                    DXGRIP("PixelJar::EncodeFormat - Non swizzled and non compressed textures cannot be cube mapped.");
                }
            }
        }

        DWORD Size = 0;
        DWORD TexelSize = BitsPerPixelOfD3DFORMAT(D3DFormat);

        DWORD LogWidth;
        DWORD LogHeight;
        DWORD LogDepth;
        DWORD SizeWidth;
        DWORD SizeHeight;

        // Swizzed and compressed textures are always power-of-2.  Linear textures are always
        // treated a non power-of-2.
        //
        if (IsSwizzledD3DFORMAT(D3DFormat) || IsCompressedD3DFORMAT(D3DFormat))
        {
            LogWidth = Log2(Width);
            LogHeight = Log2(Height);
            LogDepth = Log2(Depth);
            SizeWidth = 0;
            SizeHeight = 0;

            DWORD LogMin = MinimumTextureSizeOfD3DFORMAT(D3DFormat);

            if (Levels == 0)
            {
                Levels = max(LogWidth, max(LogHeight, LogDepth)) + 1;
            }
            else if (DBG_CHECK(Levels > max(LogWidth, max(LogHeight, LogDepth)) + 1))
            {
                DXGRIP("PixelJar::EncodeFormat - too many mip levels were specified for this texture.");
            }

            // Figure out the total size of the texture.
            DWORD CurrentLevel;
            DWORD CurrentWidth = LogWidth;
            DWORD CurrentHeight = LogHeight;
            DWORD CurrentDepth = LogDepth;
            DWORD LogSize;

            for (CurrentLevel = Levels; CurrentLevel; CurrentLevel--)
            {
                LogSize = max(CurrentWidth, LogMin) + max(CurrentHeight, LogMin) + CurrentDepth;

                Size += (1 << LogSize) * TexelSize / 8;
        
                if (CurrentWidth > 0)
                {
                    CurrentWidth--;
                }

                if (CurrentHeight > 0)
                {
                    CurrentHeight--;
                }

                if (CurrentDepth > 0)
                {
                    CurrentDepth--;
                }
            }

            if (isCubeMap)
            {
                Size = (Size + D3DTEXTURE_CUBEFACE_ALIGNMENT - 1) 
                         & ~(D3DTEXTURE_CUBEFACE_ALIGNMENT - 1);

                Size *= 6;
            }
        }

        // Non Power-of-2 texture.
        else
        {
            // What should these be set to for one of these?
            LogWidth = LogHeight = LogDepth = 0;

            // Just set the level appropriately.  We don't support mipmaps on non-swizzled
            // textures so just set it to 1.
            //
            if (Levels == 0)
            {
                Levels = 1;
            }

            if (Pitch == 0)
            {
                Pitch = CalcPitch(Width, TexelSize);
            }

            SizeWidth = Width;
            SizeHeight = Height;

            Size = Pitch * Height;
        }

        // Build the format.
        *pFormat =    (isCubeMap ? 0x00000004 : 0)
                    | (isVolume ? 3 : 2) << D3DFORMAT_DIMENSION_SHIFT /* UNDONE: verify */
                    | D3DFormat << D3DFORMAT_FORMAT_SHIFT
                    | Levels << D3DFORMAT_MIPMAP_SHIFT
                    | LogWidth << D3DFORMAT_USIZE_SHIFT
                    | LogHeight << D3DFORMAT_VSIZE_SHIFT
                    | LogDepth << D3DFORMAT_PSIZE_SHIFT
                    | D3DFORMAT_DMACHANNEL_A
                    | D3DFORMAT_BORDERSOURCE_COLOR;

        if (SizeWidth)
        {
            *pSize   =    (SizeWidth - 1)
                        | (SizeHeight - 1) << D3DSIZE_HEIGHT_SHIFT
                        | ((Pitch  / D3DTEXTURE_PITCH_ALIGNMENT) - 1) << D3DSIZE_PITCH_SHIFT;

            // Make sure we didn't overflow/underflow.
            ASSERT((*pSize & D3DSIZE_WIDTH_MASK) + 1 == SizeWidth);
            ASSERT(((*pSize & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1 == SizeHeight);

            if (PitchFromSize(*pSize) != Pitch)
            {
                DXGRIP("PixelJar::EncodeFormat - The surface pitch is too large, possibly caused by using antialiasing with a large surface.");
            }
        }
        else 
        {
            *pSize = 0;
        }

        return Size;
    }
}

} // end of namespace

