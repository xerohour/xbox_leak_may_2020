/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       overlay.cpp
 *  Content:    Implementation of overlays.
 *
 ****************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#define NV_OVERLAY_BYTE_ALIGNMENT_PAD   63L

//----------------------------------------------------------------------------
// D3DDevice_UpdateOverlay
//
extern "C"
void WINAPI D3DDevice_UpdateOverlay(
    D3DSurface *pSurface, 
    CONST RECT *SrcRect,
    CONST RECT *DstRect, 
    BOOL EnableColorKey, 
    D3DCOLOR ColorKey)
{
    COUNT_API(API_D3DDEVICE_UPDATEOVERLAY);

    CDevice *pDevice = g_pDevice;
    HWREG *reg = pDevice->m_NvBase;
    ULONG data, SrcWidth, SrcHeight, DstWidth, DstHeight, Pitch;
    ULONG Offset, PointIn, SizeIn;

    if (DBG_CHECK(TRUE))
    {
        if (!pSurface || !SrcRect || !DstRect)
        {
            DXGRIP("D3DDevice_UpdateOverlay - One of pSurface, SrcRect or DstRect is null");
        }

        if (PixelJar::GetFormat(pSurface) != D3DFMT_YUY2)
        {
            DXGRIP("D3DDevice_UpdateOverlay - Invalid surface format.");
        }
    }

#if DBG
    data = REG_RD32(reg, NV_PVIDEO_INTR);
    if (data & DRF_NUM(_PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_PENDING))
    {
        DXGRIP("Hardware overlay buffer not ready");
    }
#endif

    pDevice->m_Miniport.m_OverlayVBlank = pDevice->m_Miniport.m_VBlankCount;

    SrcWidth = SrcRect->right - SrcRect->left;
    SrcHeight = SrcRect->bottom - SrcRect->top;

    DstWidth = DstRect->right - DstRect->left;
    DstHeight = DstRect->bottom - DstRect->top;

    REG_WR32(reg, NV_PVIDEO_STOP,
            DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _INACTIVE) |
            DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));

    // Color key
    REG_WR32(reg, NV_PVIDEO_COLOR_KEY, ColorKey);

    Pitch = PixelJar::GetPitch(pSurface);
    Offset = GetGPUAddress(GetDataFromResource(pSurface));

    // This magic is what the nvidia driver does to get the right offset
    // for the overlay YUV surfaces.
    Offset += (SrcRect->top & ~0x1) * Pitch + ((SrcRect->left & ~0x1) << 1);
    PointIn = (Offset & NV_OVERLAY_BYTE_ALIGNMENT_PAD) << 3;
    SizeIn = (SrcHeight << 16) | SrcWidth;
    SizeIn += PointIn >> 4;
    Offset &= ~NV_OVERLAY_BYTE_ALIGNMENT_PAD;

    // Address of the data
    REG_WR32(reg, NV_PVIDEO_OFFSET(0), Offset);

    // Location of the source
    REG_WR32(reg, NV_PVIDEO_POINT_IN(0), PointIn);

    // Size of the source
    REG_WR32(reg, NV_PVIDEO_SIZE_IN(0), SizeIn);

    // Horizontal du/dx ratio
    if (DstWidth <= 1) {
        data = 0x100000;
    } else {
        data = ((SrcWidth - 1) << 20) / (DstWidth - 1);
    }
    REG_WR32(reg, NV_PVIDEO_DS_DX(0), data);

    // Vertical dv/dy ratio
    if (DstHeight <= 1) {
        data = 0x100000;
    } else {
        data = ((SrcHeight - 1) << 20) / (DstHeight - 1);
    }
    REG_WR32(reg, NV_PVIDEO_DT_DY(0), data);

    // Location of the destination
    data = (DstRect->top << 16) | DstRect->left;
    REG_WR32(reg, NV_PVIDEO_POINT_OUT(0), data);

    // Size of the destination
    data = (DstHeight << 16) | DstWidth;
    REG_WR32(reg, NV_PVIDEO_SIZE_OUT(0), data);


    // Format - always assumes YUY2
    data = (NV07A_SET_OVERLAY_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16);
    data |= Pitch;
    if (EnableColorKey)
    {
        data |= (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_IMAGE << 20);
    }
    else
    {
        data |= (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_OVERLAY << 20);
    }
    REG_WR32(reg, NV_PVIDEO_FORMAT(0), data);

    data = AGP_APERTURE_BYTES - 1;

    REG_WR32(reg, NV_PVIDEO_LIMIT(0), data);

    // Enable the interrupt for the overlay and point the hardware to
    // read from buffer0
    data = REG_RD32(reg, NV_PVIDEO_INTR_EN);
    REG_WR32(reg, NV_PVIDEO_INTR_EN, data | DRF_NUM(_PVIDEO, _INTR_EN, _BUFFER_0, NV_PVIDEO_INTR_EN_BUFFER_0_ENABLED));
    REG_WR32(reg, NV_PVIDEO_BUFFER, DRF_NUM(_PVIDEO, _BUFFER, _0_USE, NV_PVIDEO_BUFFER_0_USE_SET));
}

//----------------------------------------------------------------------------
// D3DDevice_EnableOverlay
//
extern "C"
void WINAPI D3DDevice_EnableOverlay(BOOL Enable)
{
    COUNT_API(API_D3DDEVICE_ENABLEOVERLAY);

    CDevice *pDevice = g_pDevice;
    HWREG *reg = pDevice->m_NvBase;

    if (REG_RD32(reg, NV_PVIDEO_BUFFER))
    {
        REG_WR32(reg, NV_PVIDEO_STOP,
                DRF_DEF(_PVIDEO, _STOP, _OVERLAY, _ACTIVE) |
                DRF_DEF(_PVIDEO, _STOP, _METHOD, _IMMEDIATELY));
    }

    // Make sure we're in an idle state
    while (REG_RD32(reg, NV_PVIDEO_BUFFER));

    // Reset overlay interrupts
    REG_WR32(reg, NV_PVIDEO_INTR,
            DRF_DEF(_PVIDEO, _INTR, _BUFFER_0, _RESET) |
            DRF_DEF(_PVIDEO, _INTR, _BUFFER_1, _RESET));

    REG_WR32(reg, NV_PVIDEO_OFFSET(0), 0);
    REG_WR32(reg, NV_PVIDEO_SIZE_IN(0), 0xFFFFFFFF);
    REG_WR32(reg, NV_PVIDEO_POINT_IN(0), 0);
    REG_WR32(reg, NV_PVIDEO_BASE(0), 0);

    REG_WR32(reg, NV_PVIDEO_LUMINANCE(0),
            DRF_DEF(_PVIDEO, _LUMINANCE, _CONTRAST, _UNITY) |
            DRF_DEF(_PVIDEO, _LUMINANCE, _BRIGHTNESS, _UNITY));

    REG_WR32(reg, NV_PVIDEO_CHROMINANCE(0),
            DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_COS, _UNITY) |
            DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_SIN, _UNITY));
}

//----------------------------------------------------------------------------
// D3DDevice_GetOverlayUpdateStatus
//
extern "C"
BOOL WINAPI D3DDevice_GetOverlayUpdateStatus()
{
    COUNT_API(API_D3DDEVICE_GETOVERLAYUPDATESTATUS);

    CDevice* pDevice = g_pDevice;

    if (pDevice->m_Miniport.m_OverlayVBlank == pDevice->m_Miniport.m_VBlankCount)
    {
        return FALSE;
    }

    return TRUE;
}

extern "C"
void WINAPI D3DDevice_EnableCC(BOOL Enable)
{
    COUNT_API(API_D3DDEVICE_ENABLECC);

    PVOID RegisterBase = g_pDevice->m_Miniport.m_RegisterBase;
    AvSendTVEncoderOption(RegisterBase, AV_OPTION_ENABLE_CC, Enable, NULL);
}

extern "C"
void WINAPI D3DDevice_SendCC(BOOL Field, BYTE cc1, BYTE cc2)
{
    COUNT_API(API_D3DDEVICE_SENDCC);

    PVOID RegisterBase = g_pDevice->m_Miniport.m_RegisterBase;
    ULONG Result = (cc1 << 16) | cc2;
    AvSendTVEncoderOption(RegisterBase, AV_OPTION_SEND_CC_DATA, Field, &Result);
}

extern "C"
void WINAPI D3DDevice_GetCCStatus(BOOL *pField1, BOOL *pField2)
{
    COUNT_API(API_D3DDEVICE_GETCCSTATUS);

    PVOID RegisterBase = g_pDevice->m_Miniport.m_RegisterBase;
    ULONG Result;
    AvSendTVEncoderOption(RegisterBase, AV_QUERY_CC_STATUS, 0, &Result);

    *pField1 = !(Result & 1);
    *pField2 = !(Result & 2);
}

}

