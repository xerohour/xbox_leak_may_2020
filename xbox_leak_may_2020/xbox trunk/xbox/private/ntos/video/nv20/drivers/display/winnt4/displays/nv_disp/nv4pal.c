//******************************Module*Header***********************************
// 
// Module Name: NV4PAL.C
// 
// Copyright (c) 1992-1996 Microsoft Corporation
// 
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"

#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "ddmini.h"
#endif

#include "nv32.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

#include "nvapi.h"


//******************************************************************************
// Extern declarations
//******************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );


//******************************************************************************
//
//  Function:   NV4DmaPushSetPalette
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV4DmaPushSetPalette(
PDEV* ppdev,
ULONG start,
ULONG length)
    {
    ULONG i;
    ULONG ulHead;

#if _WIN32_WINNT >= 0x0500

    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
	{
        ulHead = ppdev->ulDeviceDisplay[i];
#if (_WIN32_WINNT >= 0x0500) && defined(NVD3D) && !defined(_WIN64)

        if( (ppdev->pDriverData->vpp.dwOverlayFSNumSurfaces != 0))
        {
        // Disable pan & scan because Video is rendering on the head now.
            if(ppdev->pDriverData->vpp.dwOverlayFSHead == ulHead)
            {
                continue;
            }
        }
#endif
        ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_OFFSET(0), 0);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_FORMAT(0), NV046_SET_LUT_FORMAT_NOTIFY_WRITE_ONLY);
        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);
        while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    }
    return;

#else
    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
	{
        ulHead = ppdev->ulDeviceDisplay[i];
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
    } //for each head
    return;
        
#endif // _WIN32_WINNT >= 0x0500

    }


/******************************Public*Routine******************************\
* ULONG nvSetGammaRamp
\**************************************************************************/
BOOL nvSetHeadGammaRamp(PDEV *ppdev, ULONG ulHead, PUSHORT fpRampVals)
{
    ULONG i, ulOffset;
    ULONG ulNumEntries = 256;

    DECLARE_DMA_FIFO;

    if (!ppdev) return(FALSE);

    if(ppdev->iBitmapFormat != BMF_8BPP)
    {
        ulOffset = ulHead*256;

        for (i = 0; i < ulNumEntries; i++) 
        {
            ppdev->ajClutData[i + ulOffset].Blue  = (UCHAR) (fpRampVals[512 + i] >> 8); /* blue value */
            ppdev->ajClutData[i + ulOffset].Green = (UCHAR) (fpRampVals[256 + i] >> 8); /* green value */
            ppdev->ajClutData[i + ulOffset].Red   = (UCHAR) (fpRampVals[i] >> 8); /* red value */
        }

        INIT_LOCAL_DMA_FIFO;

        ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_OFFSET(0), 0);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_LUT_FORMAT(0), NV046_SET_LUT_FORMAT_NOTIFY_WRITE_ONLY);
        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);
        while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_LUT(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);

        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}



/******************************Public*Routine******************************\
* ULONG nvSetGammaRamp
\**************************************************************************/
BOOL nvGetHeadGammaRamp(PDEV *ppdev, ULONG ulHead, PUSHORT fpRampVals)
{
    ULONG i, ulOffset;
    ULONG ulNumEntries = 256;

    DECLARE_DMA_FIFO;

    if (!ppdev) return(FALSE);

    if(ppdev->iBitmapFormat != BMF_8BPP)
    {
        ulOffset = ulHead*256;

        for (i = 0; i < ulNumEntries; i++) 
        {
            fpRampVals[512 + i] = ((USHORT)ppdev->ajClutData[i + ulOffset].Blue << 8); // blue value
            fpRampVals[256 + i] = ((USHORT)ppdev->ajClutData[i + ulOffset].Green << 8);// green value
            fpRampVals[i]       = ((USHORT)ppdev->ajClutData[i + ulOffset].Red << 8);  // red value
        }
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }

}
