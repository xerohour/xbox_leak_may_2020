/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       clear.cpp
 *  Content:    Handles wacking the hardware for the Clear command
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

//----------------------------------------------------------------------------
// D3DDevice_Clear
//
extern "C"
void WINAPI D3DDevice_Clear(
    DWORD Count,
    CONST D3DRECT *pRects,
    DWORD Flags,
    D3DCOLOR Color,
    float Z,
    DWORD Stencil) 
{ 
    CDevice *pDevice = g_pDevice;
    D3DRECT rect;

    PPUSH pPush;

    COUNT_API(API_D3DDEVICE_CLEAR);

#if DBG

    if (DBG_CHECK(TRUE))
    {
        if ((Flags == 0) ||
            (Flags & ~(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)))
        {
            D3D_ERR("Invalid flags passed to Clear");
        }
        if ((Flags & D3DCLEAR_STENCIL) && (Stencil > 0xff))
        {
            D3D_ERR("Invalid stencil fill value");
        }
        if ((Flags & D3DCLEAR_ZBUFFER) && (Z < 0.0f || Z > 1.0f))
        {
            D3D_ERR("Invalid Z fill value");
        }
        if ((Flags & D3DCLEAR_ZBUFFER) &&
            !(Flags & D3DCLEAR_STENCIL) &&
            (pDevice->m_pZBuffer) &&
            (PixelJar::GetBitsPerPixel(pDevice->m_pZBuffer) == 32) &&
            !(g_StencilEverEnabled) &&
            !(g_WarnedAboutStencil))
        {
            WARNING("D3DCLEAR_ZBUFFER for 32-bit depth buffers is faster when "
                    "D3DCLEAR_STENCIL set too,\neven when not using stencil");
            g_WarnedAboutStencil = TRUE;
        }
        if(pDevice->m_dwSnapshot)
        {
            HandleShaderSnapshot_Clear(Count, pRects, Flags, Color, Z, Stencil);
            return;
        }
    }

#endif

    // Get information about the render target.

    D3DSurface *pTarget = pDevice->m_pRenderTarget;
    D3DSurface *pZBuffer = pDevice->m_pZBuffer;

    DWORD FillDepth = 0;
    DWORD SavedFormat = 0;

    // Kelvin doesn't handle clearing rectangles in swizzled textures.
    // Munge it around to allow us to clear the entire texture as a linear
    // one.  This needs to be fixed.

    if (PixelJar::IsSwizzled(pTarget))
    {
        if (DBG_CHECK(TRUE))
        {
            DWORD Width = PixelJar::GetWidth(pTarget);
            DWORD Height = PixelJar::GetHeight(pTarget);

            if (DBG_CHECK(Count))
            {
                if (Count != 1
                    || pRects->x1 != 0
                    || pRects->y1 != 0
                    || pRects->x2 != (long)Width
                    || pRects->y2 != (long)Height)
                {
                    D3D_ERR("Swizzled render targets can only be cleared in their entirety.");
                }
            }

            if (pDevice->m_Viewport.X != 0
                || pDevice->m_Viewport.Y
                || (pDevice->m_Viewport.Width != Width)
                || (pDevice->m_Viewport.Height != Height))
            {
                D3D_ERR("Swizzled render targets can only be cleared in their entirety and cannot be restricted by a viewport.");
            }
        }

        SavedFormat = PixelJar::GetSurfaceFormat(pTarget, pZBuffer);

        DWORD NewFormat = SavedFormat & ~DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _SWIZZLE);
        NewFormat |= DRF_DEF(097, _SET_SURFACE_FORMAT, _TYPE, _PITCH);

        pPush = pDevice->StartPush();

        Push1(pPush, NV097_SET_SURFACE_FORMAT, NewFormat);

        pDevice->EndPush(pPush + 2);
    }

    // Get the color to set the target to.

    if (Flags & D3DCLEAR_TARGET)
    {
        D3DFORMAT Format = PixelJar::GetFormat(pTarget);

        switch(Format)
        {
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_A8R8G8B8:
            break;

        case D3DFMT_R5G6B5:
        case D3DFMT_LIN_R5G6B5:
            Color = ((Color & 0x00F80000) >> 8)
                  | ((Color & 0x0000FC00) >> 5)
                  | ((Color & 0x000000F8) >> 3);
            break;

        case D3DFMT_X1R5G5B5:
        case D3DFMT_LIN_X1R5G5B5:
            Color = ((Color & 0x00F80000) >> 9)
                  | ((Color & 0x0000F800) >> 6)
                  | ((Color & 0x000000F8) >> 3);
            break;

#if DBG

        case D3DFMT_LIN_L8:
        case D3DFMT_LIN_G8B8:
            D3D_ERR("D3DFMT_LIN_L8 and D3DFMT_LIN_G8B8 cannot be cleared via D3DDevice_Clear.");
            break;

#endif

        }
    }

    if (!pZBuffer)
    {
        Flags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);

        // Nothing to do?  Then do nothing.  (Our init code hits this
        // case when it does the initial Clear and there's no Z-buffer.)

        if (!Flags)
        {
            return;
        }
    }

    if (Flags & D3DCLEAR_ZBUFFER)
    {
        // Adjust the zbuffer fill value.  

        D3DFORMAT Format = PixelJar::GetFormat(pZBuffer);

        // Adjust the z-buffer depth value.  We use doubles for the best
        // accuracy in the conversion to integer:

        switch(Format)
        {
        case D3DFMT_D16:
        case D3DFMT_LIN_D16:

            // Convert to a 16-bit number, with clamp.  Note that to match
            // the hardware, we never round.
            {
                long RawFillDepth;

                RawFillDepth = FloatToLong(Z * (float)D3DZ_MAX_D16);   
                RawFillDepth = (RawFillDepth > 0xffff) ? 0xffff : RawFillDepth;
                RawFillDepth = (RawFillDepth < 0)      ? 0      : RawFillDepth;

                FillDepth  = (DWORD)RawFillDepth;
            }
            break;

        case D3DFMT_D24S8:
        case D3DFMT_LIN_D24S8:

            // Convert to a 24-bit number, with clamp.  Note that to match
            // the hardware, we never round.
            {
                long RawFillDepth;

                RawFillDepth = FloatToLong(Z * (float)D3DZ_MAX_D24S8); 
                RawFillDepth = (RawFillDepth > 0xffffff) ? 0xffffff : RawFillDepth;
                RawFillDepth = (RawFillDepth < 0)        ? 0        : RawFillDepth;

                // The z-buffer occupies the upper 3 bytes:

                FillDepth  = (DWORD)RawFillDepth << 8;  
            }
            break;

        case D3DFMT_F16:
        case D3DFMT_LIN_F16:

            if (Z == 0.0F)
            {
                FillDepth = 0;
            }           
            else
            {
                double ZScaled = (double)Z * D3DZ_MAX_F16;

                DWORD ZBitsH = *((DWORD *)&ZScaled + 1);

                FillDepth = ((ZBitsH >> 8) - 0x3F8000) & 0xFFFF;
            }

            break;

        case D3DFMT_F24S8:
        case D3DFMT_LIN_F24S8:

            if (Z == 0.0F)
            {
                FillDepth = 0;
            }
            else
            {
                double ZScaled = (double)Z * D3DZ_MAX_F24S8;

                DWORD ZBitsH = *((DWORD *)&ZScaled + 1);

                FillDepth = ((ZBitsH - 0x38000000) << 4) & 0xFFFFFF00;
            }

            break;

        default:
            NODEFAULT("D3DDevice_Clear - Unknown depth buffer format\n");
        }
    }

    // We're going to clip to the viewport, so copy it to a more usable
    // form:

    INT x1Clip = pDevice->m_Viewport.X;
    INT y1Clip = pDevice->m_Viewport.Y;
    INT x2Clip = pDevice->m_Viewport.X + pDevice->m_Viewport.Width;
    INT y2Clip = pDevice->m_Viewport.Y + pDevice->m_Viewport.Height;

    // No supplied rectangles means we need to clear the whole surface.

    if (!Count)
    {
        rect.x1 = x1Clip;
        rect.y1 = y1Clip;
        rect.x2 = x2Clip;
        rect.y2 = y2Clip;

        pRects = &rect;
        Count = 1;
    }    

    // Iterate through the rectangles.  Note that we could do a little
    // less work in the case of multiple clear rectangles, but that
    // will be an infrequent case:

    do {
        // Clip the rectangle to the viewport:

        INT x1 = max(pRects->x1, x1Clip);
        INT y1 = max(pRects->y1, y1Clip);
        INT x2 = min(pRects->x2, x2Clip);
        INT y2 = min(pRects->y2, y2Clip);

        // Make sure the result is not empty:

        if ((x1 < x2) && (y1 < y2))
        {
            x1 = Round(x1 * pDevice->m_SuperSampleScaleX);
            x2 = Round(x2 * pDevice->m_SuperSampleScaleX);
            y1 = Round(y1 * pDevice->m_SuperSampleScaleY);
            y2 = Round(y2 * pDevice->m_SuperSampleScaleY);

            pPush = pDevice->StartPush();
    
            PushCount(pPush, NV097_SET_CLEAR_RECT_HORIZONTAL, 2);
    
            // NV097_SET_CLEAR_RECT_HORIZONTAL
    
            *(pPush + 1) 
                = DRF_NUMFAST(097, _SET_CLEAR_RECT_HORIZONTAL, _XMIN, x1) |
                  DRF_NUMFAST(097, _SET_CLEAR_RECT_HORIZONTAL, _XMAX, x2 - 1);
    
            // NV097_SET_CLEAR_RECT_VERTICAL
    
            *(pPush + 2) 
                = DRF_NUMFAST(097, _SET_CLEAR_RECT_VERTICAL, _YMIN, y1) |
                  DRF_NUMFAST(097, _SET_CLEAR_RECT_VERTICAL, _YMAX, y2 - 1);
    
            PushCount(pPush + 3, NV097_SET_ZSTENCIL_CLEAR_VALUE, 3);
    
            // NV097_SET_ZSTENCIL_CLEAR_VALUE
    
            *(pPush + 4) = FillDepth | Stencil;
    
            // NV097_SET_COLOR_CLEAR_VALUE
    
            *(pPush + 5) = Color;
    
            // NV097_CLEAR_SURFACE
            //
            // Writing to this register triggers the clear, so it has to come
            // last:
    
            *(pPush + 6) = Flags;

            // PERF: The Nvidia driver clears in 256-byte vertical bands!

            pDevice->EndPush(pPush + 7);
        }

    } while (++pRects, --Count != 0);

    // Put the original format back.

    if (SavedFormat)
    {
        pPush = pDevice->StartPush();

        Push1(pPush, NV097_SET_SURFACE_FORMAT, SavedFormat);

        pDevice->EndPush(pPush + 2);
    }
}

} // end namespace

