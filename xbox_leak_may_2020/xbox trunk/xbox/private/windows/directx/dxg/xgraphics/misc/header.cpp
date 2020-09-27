#if XBOX
#include <xgrphseg.h>
#include <xtl.h>
#else
#include <windows.h>
#include <d3d8-xbox.h>
#endif

#include <xgraphics.h>
#include <debug.h>
#include "header.h"

namespace XGRAPHICS
{

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
        ASSERT(PitchFromSize(*pSize) == Pitch);
    }
    else 
    {
        *pSize = 0;
    }

    return Size;
}

void EncodeTexture(
    DWORD Width,
    DWORD Height,
    DWORD Depth,
    DWORD Levels,
    DWORD Usage,
    D3DFORMAT Format,
    DWORD Pitch,
    bool isCubeMap,
    bool isVolume,
    DWORD Data,
    D3DBaseTexture *pTexture)
{
    EncodeFormat(Width,
                 Height,
                 Depth,
                 Levels,
                 Format,
                 Pitch,
                 isCubeMap,
                 isVolume,
                 &pTexture->Format,
                 &pTexture->Size);

    if (Usage & D3DUSAGE_BORDERSOURCE_TEXTURE)
    {
        pTexture->Format &= ~D3DFORMAT_BORDERSOURCE_COLOR;
    }

    pTexture->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_TEXTURE | D3DCOMMON_VIDEOMEMORY;
    pTexture->Lock    = 0;
    pTexture->Data    = Data;
}

extern "C"
VOID WINAPI XGSetSurfaceHeader(
    UINT Width,
    UINT Height,
    D3DFORMAT Format,
    IDirect3DSurface8* pSurface,
    UINT Data,
    UINT Pitch
    )
{
    EncodeFormat(Width,
                 Height,
                 1,
                 1,
                 Format,
                 Pitch,
                 false,
                 false,
                 &pSurface->Format,
                 &pSurface->Size);

    pSurface->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_SURFACE | D3DCOMMON_VIDEOMEMORY;
    pSurface->Lock    = 0;
    pSurface->Data    = Data;
    pSurface->Parent  = NULL;
}

extern "C"
VOID WINAPI XGSetTextureHeader(
    UINT Width,
    UINT Height,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    IDirect3DTexture8* pTexture,
    UINT Data,
    UINT Pitch
    )
{
    EncodeTexture(Width,
                  Height,
                  1,
                  Levels,
                  Usage,
                  Format,
                  Pitch,
                  false,
                  false,
                  Data,
                  (D3DBaseTexture*)pTexture);
}

extern "C"
VOID WINAPI XGSetCubeTextureHeader(
    UINT EdgeLength,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    IDirect3DCubeTexture8* pCubeTexture,
    UINT Data,
    UINT Pitch
    )
{
    EncodeTexture(EdgeLength,
                  EdgeLength,
                  1,
                  Levels,
                  Usage,
                  Format,
                  Pitch,
                  true,
                  false,
                  Data,
                  (D3DBaseTexture*)pCubeTexture);
}

extern "C"
VOID WINAPI XGSetVolumeTextureHeader(
    UINT Width,
    UINT Height,
    UINT Depth,
    UINT Levels,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    IDirect3DVolumeTexture8* pVolumeTexture,
    UINT Data,
    UINT Pitch
    )
{
    EncodeTexture(Width,
                  Height,
                  Depth,
                  Levels,
                  Usage,
                  Format,
                  Pitch,
                  false,
                  true,
                  Data,
                  (D3DBaseTexture*)pVolumeTexture);
}

extern "C"
VOID WINAPI XGSetVertexBufferHeader(
    UINT Length,
    DWORD Usage,
    DWORD FVF,
    D3DPOOL Pool,
    IDirect3DVertexBuffer8 *pBuffer,
    UINT Data
    )
{
    pBuffer->Common = /* initial refcount */ 1 | D3DCOMMON_TYPE_VERTEXBUFFER;
    pBuffer->Data = Data;
    ((D3DResource*)pBuffer)->Lock = 0;
}

extern "C"
VOID WINAPI XGSetIndexBufferHeader(
    UINT Length,
    DWORD Usage,
    D3DFORMAT Format,
    D3DPOOL Pool,
    IDirect3DIndexBuffer8 *pBuffer,
    UINT Data
    )
{
    pBuffer->Common = /* initial refcount */ 1 | D3DCOMMON_TYPE_INDEXBUFFER;
    pBuffer->Data = Data;
    ((D3DResource*)pBuffer)->Lock = 0;
}

extern "C"
VOID WINAPI XGSetPaletteHeader(
    D3DPALETTESIZE Size, 
    IDirect3DPalette8 *pPalette,
    UINT Data
    )
{
    pPalette->Common  = /* initial refcount */ 1 | D3DCOMMON_TYPE_PALETTE 
                                                 | D3DCOMMON_VIDEOMEMORY
                                                 | D3DPALETTE_COMMON_VIDEOMEMORY;
    pPalette->Common |= Size << D3DPALETTE_COMMON_PALETTESIZE_SHIFT;
    pPalette->Data = Data;
    ((D3DResource*)pPalette)->Lock = 0;
}

extern "C"
VOID WINAPI XGSetPushBufferHeader(
    UINT Size,
    BOOL RunUsingCpuCopy,
    IDirect3DPushBuffer8 *pPushBuffer,
    UINT Data
    )
{
    pPushBuffer->Common = /* initial refcount */ 1 | D3DCOMMON_TYPE_PUSHBUFFER;

    if (RunUsingCpuCopy)
    {
        pPushBuffer->Common |= D3DPUSHBUFFER_RUN_USING_CPU_COPY;
    }

    pPushBuffer->AllocationSize = Size;
    pPushBuffer->Data = Data;
    pPushBuffer->Size = Size;
    pPushBuffer->Lock = 0;
}

extern "C"
VOID WINAPI XGSetFixupHeader(
    UINT Size,
    IDirect3DFixup8 *pFixup,
    UINT Data
    )
{
    pFixup->Common = /* initial refcount */ 1 | D3DCOMMON_TYPE_FIXUP;
    pFixup->Data = Data;
    pFixup->Size = Size;
    pFixup->Lock = 0;
}

} // namespace XGRAPHICS
