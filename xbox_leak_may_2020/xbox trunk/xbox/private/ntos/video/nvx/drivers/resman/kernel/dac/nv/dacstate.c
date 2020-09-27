 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************************** DAC Manager ******************************\
*                                                                           *
* Module: DACSTATE.C                                                        *
*   The DAC state is changed in this module.                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <dma.h>
#include <dac.h>
#include <os.h>
#include <vga.h>
#include "nvhw.h"

VOID enableCursor(PHWINFO, U032);
VOID disableCursor(PHWINFO, U032);

//
// Change DAC hardware state.
//
RM_STATUS stateDac
(
    PHWINFO pDev,
    U032    msg
)
{
    U032 i;

    switch (msg)
    {
        case STATE_UPDATE:
            //
            // Only the Dpm Level and APLL are changed here.  All other
            // updates occur in the VBlank interrupt handler.
            //
            if (pDev->Dac.UpdateFlags & UPDATE_HWINFO_DAC_VPLL)
            {
                dacCalcPLL(pDev);
                pDev->Dac.UpdateFlags &= ~UPDATE_HWINFO_DAC_VPLL;
            }

            // NOTE: MCLK and NVCLK updates are done in mcState.c:stateMc
            
            if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_DISPLAY_PARAMS)
            {

                //
                // Program the new DAC settings
                //    
                // NT NOW calls dacSetMode to do the modesets !!! (Miniport no longer sets the mode)
                // However, the NT driver does NOT use the class interface to do the modeset.
                // Instead it calls RmSetMode.

                dacSetMode(pDev, 0);

#ifdef NTRM
                osSetVideoMode(pDev);
#endif                
                //
                // Perform any necessary OS-specific function
                // after the change to the DAC
                //
                osStateDacEx(pDev, msg);

                pDev->Framebuffer.UpdateFlags &= ~UPDATE_HWINFO_DISPLAY_PARAMS;
            }
            
            if (!pDev->Vga.Enabled)
            {
                for (i = 0; i < MAX_CRTCS; i++)
                {
                    enableCursor(pDev, i);
                    pDev->Dac.CrtcInfo[i].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE;	// force an update from image in nvInfo
                }
            }
            else
            {
                for (i = 0; i < MAX_CRTCS; i++)
                    disableCursor(pDev, i);
            }
                
            break;
        case STATE_LOAD:
            //
            // Initialize DAC cursor emulation.
            //
#ifdef NTRM
            for (i = 0; i < MAX_CRTCS; i++)
                pDev->Dac.CrtcInfo[i].CursorType = DAC_CURSOR_TWO_COLOR_XOR;
#else
// Don't turn on cursor until asked to by Windows.
//            pDev->Dac.CursorType      = DAC_CURSOR_TWO_COLOR_XOR;
#endif

            //
            // Make sure we enable the backend video overlay, if req'd
            //
            // KJK: Move this to the new video engine code
            //
            /*
            //
            // New DDraw protocol will enable the overlay on its own
            //
            if ((pDev->Video.Enabled == 1) && !pDev->Vga.Enabled)
                FLD_WR_DRF_DEF(_PVIDEO, _OVERLAY, _VIDEO, _ON);
            */                

            if (!pDev->Vga.Enabled)
            {
#ifdef NTRM
                osSetVideoMode(pDev);
#endif            
                //
                // Perform any necessary OS-specific function
                // after the change to the DAC
                //
                osStateDacEx(pDev, msg);
            
                //
                // Load cursor on head 0 only.
                //
                enableCursor(pDev, 0);
                pDev->Dac.CrtcInfo[0].UpdateFlags |= UPDATE_HWINFO_DAC_CURSOR_IMAGE;	// force an update from image in nvInfo

                //
                // Load pallette.
                //
                
				// set flag to update palette in vblank
#ifndef NTRM
                // There is a synchronization issue with NT/Win2K SMP where this code could
                // be running at the same time as the mode set code on another CPU which will
                // cause corruption in the LUT.  This code is, strictly speaking, not necessary 
                // for NT/Win2K.  However, the kernel code is NOT supposed to contain OS dependencies, 
                // so this must be changed to be OS independent.
	            pDev->Dac.UpdateFlags |= UPDATE_HWINFO_DAC_COLORMAP;
#endif            

                //
                // Enable the vertical blank interrupt on primary device.
                // XXXscottl - assume it's on head 0 for now.
                //
                DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, 0);
                DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_EN_0, _VBLANK, _ENABLED, 0);
                
                pDev->Dac.CrtcInfo[0].StateFlags |= DAC_CRTCINFO_VBLANK_ENABLED;
            }
			else
            {
                for (i = 0; i < MAX_CRTCS; i++)
                {
                    DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 0, i);
                    disableCursor(pDev, i);
                }
            }
            break;
        case STATE_UNLOAD:

            // Disable the cursor and the vertical blank interrupt(s).
            //
            for (i = 0; i < MAX_CRTCS; i++)
            {
                disableCursor(pDev, i);
                DAC_REG_WR_DRF_DEF(_PCRTC, _INTR_0, _VBLANK, _RESET, i);
                DAC_REG_WR32(NV_PCRTC_INTR_EN_0, 0, i);
                pDev->Dac.CrtcInfo[i].StateFlags &= ~DAC_CRTCINFO_VBLANK_ENABLED;
            }
            break;
        case STATE_INIT:
            initDac(pDev);
            break;
        case STATE_DESTROY:
            break;
    }
    return (RM_OK);
}
