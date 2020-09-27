/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mpdac.cpp
 *  Content:    DAC related functions
 *
 ***************************************************************************/

#include "precomp.hpp"

// Optimize this module for size (favor code space)

#pragma optimize("s", on)

#include "avmode.h"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

VOID
CMiniport::DacProgramVideoStart(
    ULONG startAddr
    )
{
    CDevice *pDevice = g_pDevice;

    BYTE* regbase = (BYTE*)m_RegisterBase;
    BYTE i, lock, crtc_index;
    ULONG val32;

    BOOL FlippingToOddField;

    FlippingToOddField = !IsOddField();

    // The start address gets set as part of a mode change...so do 
    // nothing if we're processing one.
    //
    if (!m_FirstFlip)
    {        
        if ((m_CurrentAvInfo & AV_FLAGS_FIELD) 
            && (m_DisplayMode & AV_MODE_OUT_MASK) != AV_MODE_OUT_HDTV
            && FlippingToOddField)
        {
            startAddr -= m_SurfacePitch / 2;
        }

        else if ((m_DisplayMode & AV_MODE_OUT_MASK) == AV_MODE_OUT_HDTV
                 && (m_CurrentAvInfo & AV_FLAGS_INTERLACED)
                 && FlippingToOddField)
        {
            startAddr -= m_SurfacePitch;
        }

        REG_WR32(regbase, NV_PCRTC_START, startAddr);
    }
}


VOID
CMiniport::DacProgramGammaRamp(
    D3DGAMMARAMP* GammaRamp
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Start at 0 and autoincrement
    //
    REG_WR08(RegisterBase, NV_USER_DAC_WRITE_MODE_ADDRESS, 0);
    for (ULONG i = 0; i < 256; i++)
    {
        //
        // Three bytes are written to the same register.  The order of these
        // bytes must be Red, Green, Blue
        //
        REG_WR08(RegisterBase, NV_USER_DAC_PALETTE_DATA, GammaRamp->red[i]);
        REG_WR08(RegisterBase, NV_USER_DAC_PALETTE_DATA, GammaRamp->green[i]);
        REG_WR08(RegisterBase, NV_USER_DAC_PALETTE_DATA, GammaRamp->blue[i]);
    }
}

} // end of namespace

