/************************** Module Header *************************************
 *                                                                             *
 *  Module Name: nvsetdac.c                                                    *
 *                                                                             *
 *                                                                             *
 *   Copyright (C) 1997-present NVidia Corporation. All Rights Reserved.       *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "excpt.h"
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#include "ddminint.h"
#include "dmamacro.h"
#include "nvcm.h"

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );
extern ULONG GetPrevDevMaskRegValue(PPDEV ppdev);
extern VOID SetPrevDevMaskRegValue(PPDEV ppdev, ULONG ulVal);
extern BOOLEAN GetULONGRegValue(PPDEV ppdev, char *pcKeyName, ULONG *pulData);

// Enable Double Scan for height <= this value
#define HEIGHT_ENABLE_DOUBLE_SCAN       384

BOOL __cdecl NvSetDac(PDEV *ppdev, ULONG ulHead, RESOLUTION_INFO * pResolution)
{
	DAC_TIMING_VALUES sTimingDac;
    HEAD_RESOLUTION_INFO HeadResolution;
    DWORD ReturnedDataLength;
    DWORD ulFormat = 0;


    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    //
    // Fill up the head resolution info structure used for the common modeset code.
    //
    HeadResolution.ulDisplayWidth = pResolution->ulDisplayWidth;
    HeadResolution.ulDisplayHeight= pResolution->ulDisplayHeight;  	
    HeadResolution.ulDisplayRefresh = pResolution->ulDisplayRefresh;  	
    
    HeadResolution.ulHead = ulHead;              
    HeadResolution.ulDeviceMask = ppdev->ulDeviceMask[ulHead];        
    HeadResolution.ulDeviceType = ppdev->ulDeviceType[ulHead];        
    HeadResolution.ulTVFormat = ppdev->ulTVFormat[ulHead];
    HeadResolution.ulOption = HEAD_RESOLUTION_OPTION_DONT_BACKOFF;
    switch (pResolution->ulDisplayPixelDepth)
    {
        case BMF_32BPP:
            HeadResolution.ulDisplayPixelDepth = 32;
            break;
        case BMF_16BPP:
            HeadResolution.ulDisplayPixelDepth = 16;
            break;
        case BMF_8BPP:
            HeadResolution.ulDisplayPixelDepth = 8;
            break;
        default:
            DISPDBG((0,"Oops! Invalid color depth: 0x%x\n",pResolution->ulDisplayPixelDepth));
            return(FALSE);
    }


    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_GET_TIMING_DAC,
                           &HeadResolution,  // input buffer
                           sizeof(HEAD_RESOLUTION_INFO),
                           &sTimingDac,
                           sizeof(DAC_TIMING_VALUES),
                           &ReturnedDataLength))
    {
    	DISPDBG((0, "nvsetdac.c - IOCTL_VIDEO_GET_TIMING_DAC failed"));
		return FALSE;
    }

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_PREMODE_SET,
                           &ulHead,  // input buffer
                           sizeof(ULONG),
                           NULL,
                           0,
                           &ReturnedDataLength))
    {
    	DISPDBG((0, "nvsetdac.c - IOCTL_VIDEO_PREMODE_SET failed"));
		return FALSE;
    }

    DISPDBG((1,"\nNvSetDac() headResolutionInfo: ........................................."));

    DISPDBG((1,"    ulDisplayWidth: %d",HeadResolution.ulDisplayWidth));
    DISPDBG((1,"    ulDisplayheight: %d",HeadResolution.ulDisplayHeight));
    DISPDBG((1,"    ulDisplayRefresh: %d",HeadResolution.ulDisplayRefresh));
    DISPDBG((1,"    ulDisplayPixelDepth: %d",HeadResolution.ulDisplayPixelDepth));
    DISPDBG((1,"    ulHead: %d",HeadResolution.ulHead));
    DISPDBG((1,"    ulDeviceMask: 0x%x",HeadResolution.ulDeviceMask));
    DISPDBG((1,"    ulDeviceType: %d",HeadResolution.ulDeviceType));
    DISPDBG((1,"    ulTVFormat: %d",HeadResolution.ulTVFormat));
    DISPDBG((1,""));

    DISPDBG((1,"TimingParameters: "));
    DISPDBG((1,"    sTimingDac.HorizontalVisible: 0x%x",sTimingDac.HorizontalVisible));
    DISPDBG((1,"    sTimingDac.VerticalVisible: 0x%x",sTimingDac.VerticalVisible));
    DISPDBG((1,"    sTimingDac.Refresh: 0x%x",sTimingDac.Refresh));
    DISPDBG((1,"    sTimingDac.HorizontalTotal: 0x%x",sTimingDac.HorizontalTotal));
    DISPDBG((1,"    sTimingDac.VerticalTotal: 0x%x",sTimingDac.VerticalTotal));
    DISPDBG((1,"    sTimingDac.HorizontalBlankStart: 0x%x",sTimingDac.HorizontalBlankStart));
    DISPDBG((1,"    sTimingDac.VerticalBlankStart: 0x%x",sTimingDac.VerticalBlankStart));
    DISPDBG((1,"    sTimingDac.HorizontalRetraceStart: 0x%x",sTimingDac.HorizontalRetraceStart));
    DISPDBG((1,"    sTimingDac.VerticalRetraceStart: 0x%x",sTimingDac.VerticalRetraceStart));
    DISPDBG((1,"    sTimingDac.HorizontalRetraceEnd: 0x%x",sTimingDac.HorizontalRetraceEnd));
    DISPDBG((1,"    sTimingDac.VerticalRetraceEnd: 0x%x",sTimingDac.VerticalRetraceEnd));
    DISPDBG((1,"    sTimingDac.HorizontalBlankEnd: 0x%x",sTimingDac.HorizontalBlankEnd));
    DISPDBG((1,"    sTimingDac.VerticalBlankEnd: 0x%x",sTimingDac.VerticalBlankEnd));
    DISPDBG((1,"    sTimingDac.PixelClock: 0x%x",sTimingDac.PixelClock));
    DISPDBG((1,"    sTimingDac.HSyncpolarity: 0x%x",sTimingDac.HSyncpolarity));
    DISPDBG((1,"    sTimingDac.VSyncpolarity: 0x%x",sTimingDac.VSyncpolarity));

    
    DISPDBG((1,"NvSetDac() End........................................................\n"));

#if _WIN32_WINNT >= 0x0500

    // setup mode set using DMA push channel
    
    NV_DMAPUSH_CHECKFREE(( (ULONG) (20) ));  

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_NOTIFIES, NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER);

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_IMAGE_SIZE(0), (sTimingDac.VerticalVisible << 16) | sTimingDac.HorizontalVisible);

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_HORIZONTAL_BLANK(0), sTimingDac.HorizontalBlankStart | 
                                                ((sTimingDac.HorizontalBlankEnd -   sTimingDac.HorizontalBlankStart ) << 16));


    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_HORIZONTAL_SYNC(0),  sTimingDac.HorizontalRetraceStart | 
                                                ((sTimingDac.HorizontalRetraceEnd - sTimingDac.HorizontalRetraceStart ) << 16));

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_VERTICAL_BLANK(0), sTimingDac.VerticalBlankStart | 
                                                ((sTimingDac.VerticalBlankEnd -   sTimingDac.VerticalBlankStart ) << 16));
                                                                        
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_VERTICAL_SYNC(0),  sTimingDac.VerticalRetraceStart | 
                                                ((sTimingDac.VerticalRetraceEnd - sTimingDac.VerticalRetraceStart ) << 16));

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_TOTAL_SIZE(0),  sTimingDac.VerticalTotal << 16 | sTimingDac.HorizontalTotal);

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_PIXEL_CLOCK(0), sTimingDac.PixelClock);

#else

    // setup mode set using Direct Method Call
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_CONTEXT_DMA_NOTIFIES,
            NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set context DMA notifies"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_IMAGE_SIZE(0),
            (sTimingDac.VerticalVisible << 16) | sTimingDac.HorizontalVisible
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC image size"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_HORIZONTAL_BLANK(0),
            sTimingDac.HorizontalBlankStart | ((sTimingDac.HorizontalBlankEnd - sTimingDac.HorizontalBlankStart) << 16)
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC horizontal blank"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_HORIZONTAL_SYNC(0),
            sTimingDac.HorizontalRetraceStart | ((sTimingDac.HorizontalRetraceEnd - sTimingDac.HorizontalRetraceStart) << 16)
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC horizontal sync"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_VERTICAL_BLANK(0),
            sTimingDac.VerticalBlankStart | ((sTimingDac.VerticalBlankEnd - sTimingDac.VerticalBlankStart) << 16)
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC vertical blank"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_VERTICAL_SYNC(0),
            sTimingDac.VerticalRetraceStart | ((sTimingDac.VerticalRetraceEnd - sTimingDac.VerticalRetraceStart) << 16)
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC vertical sync"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_TOTAL_SIZE(0),
            sTimingDac.VerticalTotal << 16 | sTimingDac.HorizontalTotal
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC total size"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_PIXEL_CLOCK(0),
            sTimingDac.PixelClock
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC pixel clock"));
    }
        
#endif // _WIN32_WINNT >= 0x0500

    if (sTimingDac.VerticalVisible <= HEIGHT_ENABLE_DOUBLE_SCAN)
    {
        ulFormat |= NV046_SET_DAC_FORMAT_DOUBLE_SCAN_ENABLED;
    }

    // map to VIDEO_LUT_CURSOR_DAC class values
    if((sTimingDac.HSyncpolarity == 1) || (sTimingDac.HSyncpolarity == 4))
        sTimingDac.HSyncpolarity = NV046_SET_DAC_FORMAT_HORIZONTAL_SYNC_NEGATIVE;
    else
        sTimingDac.HSyncpolarity = NV046_SET_DAC_FORMAT_HORIZONTAL_SYNC_POSITIVE;

    if((sTimingDac.VSyncpolarity == 1) || (sTimingDac.VSyncpolarity == 8))
        sTimingDac.VSyncpolarity = NV046_SET_DAC_FORMAT_VERTICAL_SYNC_NEGATIVE;
    else
        sTimingDac.VSyncpolarity = NV046_SET_DAC_FORMAT_VERTICAL_SYNC_POSITIVE;

    ulFormat |= (sTimingDac.HSyncpolarity << 2);
    ulFormat |= (sTimingDac.VSyncpolarity << 3);
    ulFormat |= (ulHead << 26);

    switch (pResolution->ulDisplayPixelDepth)
    {
        case BMF_32BPP:
            ulFormat |= (NV046_SET_DAC_FORMAT_COLOR_LE_BYPASS1X7R8G8B8 << 16);
            break;
        case BMF_16BPP:
            ulFormat |= (NV046_SET_DAC_FORMAT_COLOR_BYPASS_LE_R5G6B5 << 16);
            break;
        case BMF_8BPP:
            ulFormat |= (NV046_SET_DAC_FORMAT_COLOR_LUT_LE_Y8 << 16);
            break;
        default:
            return(FALSE);
    }

    switch(ppdev->ulDeviceType[ulHead])
    {
        case MONITOR_TYPE_VGA:
            ulFormat |= (NV046_SET_DAC_FORMAT_DISPLAY_TYPE_CRT << 20);
            break;
        case MONITOR_TYPE_NTSC:      
        case MONITOR_TYPE_PAL:       
            ulFormat |= (NV046_SET_DAC_FORMAT_DISPLAY_TYPE_TV << 20);
            ulFormat |= ((ppdev->ulTVFormat[ulHead] & 0x07) << 22);
            break;
        case MONITOR_TYPE_FLAT_PANEL:
            ulFormat |= (NV046_SET_DAC_FORMAT_DISPLAY_TYPE_DFP << 20);
            break;
        case INVALID_DEVICE_TYPE:
            DISPDBG((0,"Oops! Invalid device type for Head: %d. Lets assume a CRT",ulHead));
            ulFormat |= (NV046_SET_DAC_FORMAT_DISPLAY_TYPE_CRT << 20);
            break;
    }

    // Specify if this head is the 'primary' head or not.
    if (ulHead == ppdev->ulDeviceDisplay[0] && ppdev->ulDualViewSecondary == 0)
    {
        // This is the primary head.
        ulFormat |= (NV07C_SET_DAC_FORMAT_PRIMARY_DEVICE_TRUE << 28);
    }
    else
    {
        // This is not the primary head so make sure the bit is turned off.
        ulFormat |= (NV07C_SET_DAC_FORMAT_PRIMARY_DEVICE_FALSE << 28);
    }


#if _WIN32_WINNT >= 0x0500

    // perform mode set using DMA push channel
    
    ulFormat |= (NV046_SET_DAC_FORMAT_NOTIFY_WRITE_ONLY << 31);
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_DAC(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_DAC_FORMAT(0), ulFormat);


    //**************************************************************************
    // Make sure to update the DMA count before we kickoff!
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Initialize patch
    //**************************************************************************
    NV4_DmaPushSend(ppdev);

    // Wait for the mode is set.
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_DAC(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    
#else

    // perform mode set using Direct Method Call -- no notification needed
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_DAC_FORMAT(0),
            ulFormat
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set DAC format"));
    }
        
#endif // _WIN32_WINNT >= 0x0500

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_POSTMODE_SET,
                           &ulHead,  // input buffer
                           sizeof(ULONG),
                           NULL,
                           0,
                           &ReturnedDataLength))
    {
    	DISPDBG((1, "nvsetdac.c - IOCTL_VIDEO_POSTMODE_SET failed"));
		return FALSE;
    }

#if _WIN32_WINNT >= 0x0500

    // Setup the DMA context of LUT using DMA channel
    NV_DMAPUSH_CHECKFREE(( (ULONG) (2) ));  
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_LUT(0), NV_WIN_COLORMAP_CONTEXT+ulHead);
    
#else

    // setup the DMA context of LUT using Direct Method Call
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_CONTEXT_DMA_LUT(0),
            NV_WIN_COLORMAP_CONTEXT + ulHead
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set LUT offset"));
    }
        
#endif // _WIN32_WINNT >= 0x0500
    
    // Since the VIDEO can call this function directly, the gamma values will be set multiple times.
    if (pResolution->ulDisplayPixelDepth != BMF_8BPP)
    {
        if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_RESTORE_GAMMA_VALUES,
                           NULL,
                           0,
                           ppdev->ajClutData,  
                           sizeof(NV_VIDEO_CLUTDATA) * NV_NO_DACS * 256,
                           &ReturnedDataLength))
        {
    	    DISPDBG((1, "nvsetdac.c - IOCTL_VIDEO_POSTMODE_SET failed"));
        }
    }
    
    
        // Cannot call un-initialized function ppdev->pfnSetPalette
#if _WIN32_WINNT >= 0x0500

    // setup LUT using DMA push channel
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_OFFSET(0), 0);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_FORMAT(0), NV046_SET_LUT_FORMAT_NOTIFY_WRITE_ONLY);
    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    
#else

    // setup LUT using Direct Method Call -- no notification needed
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_LUT_OFFSET(0),
            0
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set LUT offset"));
    }
    if (
        NvDirectMethodCall(
            ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDmaChannel,
            NV_VIDEO_LUT_CURSOR_DAC + ulHead,
            NV046_SET_LUT_FORMAT(0),
            0                                   // don't need notification w/DMC
        ) != NVOS1D_STATUS_SUCCESS
    )
    {
        DISPDBG((2, "NVDD: Cannot perform set LUT format"));
    }
        
#endif // _WIN32_WINNT >= 0x0500



    return(TRUE);

}

BOOL NvInitialDac(PDEV *ppdev)
{

    NV07C_ALLOCATION_PARAMETERS nv07cAllocParms;
    RESOLUTION_INFO Res;
    ULONG i;
    ULONG ulHead;
    PVOID parms;
    ULONG ulDisplayWidth, ulDisplayHeight, ulOldValue;
    ULONG ulCombinedMask;
    ULONG ulPrimaryHead;
    NV_CFGEX_DISPLAY_CHANGE_START_PARAMS BracketParams;
    ULONG ulPanOffset;
    ULONG ulPrevDevMask;
    BOOLEAN bDoBracketing = FALSE;
    ULONG ulStopImage;
    
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    DISPDBG((1, "\nNvInitializeDAC: ppdev = 0x%08x", ppdev));
    DISPDBG((1, "ppdev->pjScreen               = 0x%08x", ppdev->pjScreen));
    DISPDBG((1, "ppdev->ulPrimarySurfaceOffset = 0x%08x", ppdev->ulPrimarySurfaceOffset));
    DISPDBG((1, "ppdev->pjFrameBufbase         = 0x%08x", ppdev->pjFrameBufbase));


    // We need to let resman know a combined mask for the devices that ae going to
    // be used in this mode.
    ulCombinedMask = 0;
    for (i = 0;
         i < ((ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL) ? ppdev->ulNumberDacsActive : ppdev->ulNumberDacsConnected);
         i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        ulCombinedMask |= ppdev->ulDeviceMask[ulHead];
    }
    BracketParams.NewDevices = (ulCombinedMask | NV_CFGEX_DISPLAY_CHANGE_BRACKETS_MODESET);
    if (ppdev->TwinView_State == NVTWINVIEW_STATE_SPAN)
    {
        BracketParams.Properties = 1;
    }
    else
    {
        BracketParams.Properties = 0;
    }

    ulPrevDevMask = GetPrevDevMaskRegValue(ppdev);

    if (ulPrevDevMask == 0)
    {
        //
        // we are booting up, so we need to do the bracketing.
        //
        bDoBracketing = TRUE;
    }
    else
    if (ulPrevDevMask != ulCombinedMask)
    {
        //
        // the previous mode dev mask is different than the new mode device mask, so we need to do the bracketing.
        //
        bDoBracketing = TRUE;
    }


    if (bDoBracketing)
    {
        DISPDBG((1, "Doing the modeset bracketing: PrevDevMask: 0x%x, CurrentMask: 0x%x",ulPrevDevMask, ulCombinedMask));
    }
    else
    {
        DISPDBG((1, "Not Doing the modeset bracketing: PrevDevMask: 0x%x, CurrentMask: 0x%x",ulPrevDevMask, ulCombinedMask));
    }
   
    
    if (bDoBracketing)
    {
        // Let ResMan know we are starting the modeset process for the LUT_CURSOR objects.
        if (NvConfigSetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                         NV_CFGEX_DISPLAY_CHANGE_START, &BracketParams, sizeof(BracketParams))) {
            ASSERTDD(1,"Error in Notifying Start bracket of modeset to ResMan");
        }
    }
    else
    {
        if (ppdev->TwinView_State == NVTWINVIEW_STATE_SPAN || ppdev->TwinView_State == NVTWINVIEW_STATE_DUALVIEW)
        {
            if (NvConfigSet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                     NV_CFG_DISPLAY_CHANGE_CONFIG, 1, &ulOldValue)) 
            {
                    ASSERTDD(1,"Error in Setting NV_CFG_DISPLAY_CHANGE_CONFIG");
            }
        }
        else
        {
            if (NvConfigSet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                     NV_CFG_DISPLAY_CHANGE_CONFIG, 0, &ulOldValue)) 
            {
                    ASSERTDD(1,"Error in Setting NV_CFG_DISPLAY_CHANGE_CONFIG");
            }
        }
    }

    // Initialize status of all Notifiers to "nothing pending"
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_DAC(0)*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_CURSOR_IMAGE(0)*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_CURSOR_IMAGE(1)*sizeof(NvNotification)])))->status = NV046_NOTIFICATION_STATUS_DONE_SUCCESS;
    
    Res.ulDisplayPixelDepth = ppdev->cBitsPerPel;

    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        NV_CREATE_OBJECT_SETUP();

        ulHead = ppdev->ulDeviceDisplay[i];

        if (ulHead > 0) {
            nv07cAllocParms.logicalHeadId = ulHead;
            parms = &nv07cAllocParms;
        } else {
            parms = NULL;
        }
        NV_CREATE_OBJECT_INIT();
        // NV5 will use NV04_VIDEO_LUT_CURSOR_DAC class because the method offset of NV05_VIDEO_LUT_CURSOR_DAC 
        // is different from other NV??_VIDEO_LUT_CURSOR_DAC classes.
        NV_CREATE_DMA3(parms,NV_VIDEO_LUT_CURSOR_DAC+ulHead, 
                       NV15_VIDEO_LUT_CURSOR_DAC,NV10_VIDEO_LUT_CURSOR_DAC,
                       NV04_VIDEO_LUT_CURSOR_DAC);
        if (NV_CREATE_OBJECT_FAIL()) {
            return (FALSE);
        } else {
            ppdev->CurrentClass.VideoLutCursorDac = NV_CREATE_OBJECT_CLASS();
        }

        Res.ulDisplayWidth = ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left;
        Res.ulDisplayHeight = ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top;
        Res.ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];  
        Res.ulDisplayPixelDepth = ppdev->iBitmapFormat;

        NV_DMAPUSH_CHECKFREE( ((ULONG)(4)));
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_IMAGE(0), DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);

        UPDATE_PDEV_DMA_COUNT;
        NvSetDac(ppdev, ulHead, &Res);
    	UPDATE_LOCAL_DMA_COUNT;

         // Wait for previous image completed
        ppdev->ulHeadDisplayOffset[ulHead] = ppdev->rclDisplayDesktop[ulHead].left * ppdev->cjPelSize + ppdev->rclDisplayDesktop[ulHead].top * ppdev->lDelta
                                             + ppdev->ulPrimarySurfaceOffset;
        ulPanOffset = ppdev->rclCurrentDisplay[ulHead].left * ppdev->cjPelSize + ppdev->rclCurrentDisplay[ulHead].top * ppdev->lDelta 
                                             + ppdev->ulPrimarySurfaceOffset - ppdev->ulHeadDisplayOffset[ulHead];

        DISPDBG((1, "ppdev->ulHeadDisplayOffset[%d] = 0x%08x", ulHead, ppdev->ulHeadDisplayOffset[ulHead]));
        DISPDBG((1, "ulPanOffset = 0x%08x", ulPanOffset));

        while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
        ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
        NV_DMAPUSH_CHECKFREE( ((ULONG)(6)));
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_OFFSET(0), ppdev->ulHeadDisplayOffset[ulHead]);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_PAN_OFFSET, ulPanOffset);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_FORMAT(0),
                          ppdev->lDelta | NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31);
        
        //**************************************************************************
        // Make sure to update the DMA count before we kickoff!
        //**************************************************************************

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);
    }


    if (NvConfigSet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                     NV_CFG_SCREEN_WIDTH, ppdev->cxScreen, &ulOldValue)) {
        ASSERTDD(1,"Error in Setting DAC");
    }


    if (NvConfigSet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                     NV_CFG_SCREEN_HEIGHT, ppdev->cyScreen, &ulOldValue)) {
        ASSERTDD(1,"Error in Setting DAC");
    }

    NV_DMAPUSH_CHECKFREE( ((ULONG)(6)));
    ulPrimaryHead = ppdev->ulDeviceDisplay[0];
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC+ulPrimaryHead);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_NOTIFY, NV046_NOTIFY_WRITE_ONLY);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_IMAGE(0), DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    

    //**************************************************************************
    // Make sure to update the DMA count before we kickoff!
    //**************************************************************************
    UPDATE_PDEV_DMA_COUNT;

    // Set up the notifier after waitinf for any existing notification to finish.
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;

    NV4_DmaPushSend(ppdev);

    // Now wait for the DMA to complete.
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);

    if (bDoBracketing)
    {
        // Let ResMan know we are finished with the modeset process for the LUT_CURSOR objects.
        if (NvConfigSetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                         NV_CFGEX_DISPLAY_CHANGE_END, &BracketParams, sizeof(BracketParams))) {
            ASSERTDD(1,"Error in Notifying End bracket of modeset to ResMan");
        }
    }

    //
    // Now write the current modeset device mask to the registry as PrevDevMask.
    //
    SetPrevDevMaskRegValue(ppdev, ulCombinedMask);



    //
    // We need to in effect, defer this modeset if we have scheduled a modeset via NVSVC.
    // This way, the user does not 'see' two modesets happen. The cursor needs to be disabled too.
    // Currently we do this only for Win2K and later since we are not supporting laptops in NT4.0
    //
    ulStopImage = 0;
    if (ppdev->ulInduceModeChangeDeviceMask)
    {
        ulStopImage = 1;
    }

    //
    // If the ModesetStopImage feature for sony has been enabled and the registry entry tells us to
    // stop the image, we invoke the STOP_IMAGE method.
    //
    if (ppdev->EnableModesetStopImage)
    {
        ULONG ModesetStopImage = 0;
        GetULONGRegValue(ppdev, "ModesetStopImage", &ModesetStopImage);
        if (ModesetStopImage == 1)
        {
            ulStopImage = 1;
        }
    }
     
    //
    // stop the image if needed.    
    //
    if (ulStopImage)
    {   
        for(i = 0; i < ppdev->ulNumberDacsActive; i++)
        {
        
            ulHead = ppdev->ulDeviceDisplay[i];

            DISPDBG((1, "Stopping Image to defer the first modeset since NVSVC is scheduled for a modeset"));
            NV_DMAPUSH_CHECKFREE(( (ULONG) (8) ));  
            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);
            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_NOTIFIES, NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER);

            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_NOTIFY, NV046_NOTIFY_WRITE_ONLY);
            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_STOP_IMAGE(0), NV046_STOP_IMAGE_AS_SOON_AS_POSSIBLE);
            //**************************************************************************
            // Make sure to update the DMA count before we kickoff!
            //**************************************************************************
            UPDATE_PDEV_DMA_COUNT;
            // Set up the notifier after waitinf for any existing notification to finish.
            while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
            ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;

            NV4_DmaPushSend(ppdev);

            // Now wait for the DMA to complete.
            while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_NOTIFY*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
        }
    }
    
    
    return(TRUE);
}

//**************************************************************************
// Restores DACs that may have been flipped by OpenGL to the primary surface
// PaulP
//**************************************************************************

void NvRestoreDacs(PDEV *ppdev)
{
    void __cdecl NVMoveDisplayWindow(PDEV* ppdev, ULONG ulHead, RECTL * prclCurrentDisplay);

    RECTL *prclCurrentDisplay;
    ULONG ulHead;
    ULONG i;

    if (ppdev->ulFlipBase != ppdev->ulPrimarySurfaceOffset)
    {
        ASSERT(OGL_FLIPPED());

        ppdev->ulFlipBase = ppdev->ulPrimarySurfaceOffset;

        for (i=0; i < ppdev->ulNumberDacsActive; i++)
        {
            ulHead = ppdev->ulDeviceDisplay[i];
            prclCurrentDisplay = &ppdev->rclCurrentDisplay[ulHead];
            NvSetDacImageOffset(ppdev, ulHead,
                ppdev->rclDisplayDesktop[ulHead].left * ppdev->cjPelSize + ppdev->rclDisplayDesktop[ulHead].top * ppdev->lDelta);
            #ifdef NV3
                // Pan and Scan feature is not supported in NV3.
            #else
                NVMoveDisplayWindow(ppdev, ulHead, prclCurrentDisplay);
            #endif
        }
    }
}


