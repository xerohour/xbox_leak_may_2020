//******************************************************************************
//
// Module Name:
//
//     NVHW.C
//
// Abstract:
//
//     Implements chip generic hardware routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"

//#include "oldnv332.h"
#include "driver.h"

#include "nv3a_ref.h"

#include "nvsubch.h"
#include "nvalpha.h"
#include "nvapi.h"
#include "nvcm.h"

#undef DEBUG_MSG_CHANNEL_WAIT

#define OFFSET_NV_PFIFO_BASE                        (0x2000)
#define OFFSET_NV_PFIFO_CACHE1_PUSH0_REG            (0x3200-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA0_REG             (0x3220-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_INTR_0                      (0x2100-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHES                      (0x2500-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_PULL0                (0x3240-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_PUSH1                (0x3204-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_STATUS               (0x3214-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CONFIG_0                    (0x2200-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_TLB_PT_BASE      (0x3238-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_TLB_TAG          (0x3230-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_TLB_PTE          (0x3234-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA3                 (0x322C-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_STATUS           (0x3218-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA1                 (0x3224-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA2                 (0x3228-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA0                 (0x3220-OFFSET_NV_PFIFO_BASE)

#define OFFSET_PRMCIO_INP0_COLOR_REG                0x3da

#define OFFSET_PRMCIO_CRX__COLOR_REG                0x3d4
#define OFFSET_PRMCIO_CR__COLOR_REG                 0x3d5

#define OFFSET_PRMVIO_SRX_REG                       0x3c4
#define OFFSET_PRMVIO_SR_LOCK_REG                   0x3c5
#define OFFSET_PRMVIO_MISC_READ_REG                 0x3cc
#define OFFSET_PRAMDAC_CU_START_POS_REG             0x0

#define NV_SR_UNLOCK_VALUE                          0x00000057
#define NV_SR_LOCK_VALUE                            0x00000099

//******************************Public*Routine**********************************
//
// Function: NV_GetScanLineData
//
// Routine Description:
//
//             Return current display scanline.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     Current Scanline.
//
//******************************************************************************
ULONG NV_GetScanLineData(PDEV* ppdev) {
#ifdef NV3
    ULONG   Value, Value32;
    UCHAR   lock;

    //******************************************************************************
    // Make sure the extended CRTC registers are enabled
    //******************************************************************************

    (BYTE) PRMVIO_Base[OFFSET_PRMVIO_SRX_REG] = NV_PRMVIO_SR_LOCK_INDEX;
    lock = (BYTE) PRMVIO_Base[OFFSET_PRMVIO_SR_LOCK_REG];
    (BYTE) PRMVIO_Base[OFFSET_PRMVIO_SR_LOCK_REG] =  NV_SR_UNLOCK_VALUE;

    //******************************************************************************
    // Get the scanline value.
    //******************************************************************************
    (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CRX__COLOR_REG] = NV_CIO_CRE_RL1__INDEX;
    Value = (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CR__COLOR_REG];
    Value <<= 8;
    (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CRX__COLOR_REG] = NV_CIO_CRE_RL0__INDEX;
    Value32 = (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CR__COLOR_REG] & 0xFF;
    Value |= Value32;
    Value &= 0x7FF;
    if (Value == 0)
    {
        //******************************************************************************
        // Double check to fix hw bug.
        //******************************************************************************
        (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CRX__COLOR_REG] = NV_CIO_CRE_RL1__INDEX;
        Value = (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CR__COLOR_REG];
        Value <<= 8;
        (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CRX__COLOR_REG] = NV_CIO_CRE_RL0__INDEX;
        Value32 = (BYTE) PRMCIO_Base[OFFSET_PRMCIO_CR__COLOR_REG] & 0xFF;
        Value |= Value32;
        Value &= 0x7FF;
    }

    //******************************************************************************
    // Restore the lock
    //******************************************************************************
    (BYTE) PRMVIO_Base[OFFSET_PRMVIO_SRX_REG] = NV_PRMVIO_SR_LOCK_INDEX;
    if(lock != 0x01)    // unlock read-back value
    {
        (BYTE) PRMVIO_Base[OFFSET_PRMVIO_SR_LOCK_REG] = NV_SR_LOCK_VALUE;
    }

    return(Value);
#else
    return (((REG_RD32(NV_PCRTC_RASTER+(ppdev->ulDeviceDisplay[0]*0x2000)))>>
            DRF_SHIFT(NV_PCRTC_RASTER_POSITION))&DRF_MASK(NV_PCRTC_RASTER_POSITION));
#endif // NV#
}


//******************************Public*Routine**********************************
//
// Function: NV_WaitForOneVerticalRefresh
//
// Routine Description:
//
//           Palette writes
//           (using video colormap) get queued up and don't
//           actually get written until the next vertical blank.
//           Normally, we would wait on the video colormap buffer notifier
//           to make sure the palette writes are done, before resetting
//           the device (calling int 10h).  However notifiers for the video
//           colormap are currrently *not* functional.  So for now, we'll
//           wait for at least one vertical refresh to occur , to make sure
//           all the palette writes have completed.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     Current Scanline.
//
//******************************************************************************

VOID NV_WaitForOneVerticalRefresh(PDEV* ppdev)

    {
    ULONG   TotalScanLines;
    ULONG   PreviousScanLine, NextScanLine;

    TotalScanLines   = 0;

    //**************************************************************************
    // Get a 'starting' scanline
    //**************************************************************************

    PreviousScanLine = NV_GetScanLineData(ppdev);

    //**************************************************************************
    // The largest resolution (at least for the foreseeable future)
    // will be 2k x 2k.  So wait for 2000 vertical scanlines worth to occur.
    //**************************************************************************

    do
        {
        ULONG   Count=0;

        //**********************************************************************
        // Wait for scanline to advance
        //**********************************************************************

        do
            {
            NextScanLine = NV_GetScanLineData(ppdev);
            Count++;

            //******************************************************************
            // Safety Abort just in case
            //******************************************************************

            if (Count > 10000) goto abort_scan_loop;

            } while (NextScanLine == PreviousScanLine);


        //**********************************************************************
        // If the scan has wrapped, then just add the value of NextScanLine.
        // This is *not* an accurate algorithm but it will get the job done.
        //**********************************************************************

        if (NextScanLine < PreviousScanLine)
            {
            TotalScanLines+=NextScanLine;
            PreviousScanLine = NextScanLine;
            }

        else
            {
            TotalScanLines+=(NextScanLine-PreviousScanLine);
            PreviousScanLine = NextScanLine;
            }


        } while (TotalScanLines < 2000);


abort_scan_loop:

    return;
}





//******************************Public*Routine**********************************
//
// Function: NvHwSpecific
//
// Routine Description:
//
//      This functioin is specific to a Nv chip set to workaround HW problem.
//
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     Current Scanline.
//
//******************************************************************************

VOID NvHwSpecific(PDEV* ppdev)
{
    ULONG ulRetVal;

    // Looking for NV11 Rev. B chip
    NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_CFG_FLAT_PANEL_DITHER_MODE, &ulRetVal );
    if(ulRetVal & NV_CFG_FLAT_PANEL_DITHER_MODE_CAPABLE)
    {
        NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,    
                NV_CFG_FLAT_PANEL_DITHER_MODE, &ulRetVal );             
        if(ulRetVal & NV_CFG_FLAT_PANEL_DITHER_MODE_ENABLED)            
            ppdev->ulWorkAroundHwFlag |=  NV_WORKAROUND_NV11RevB_DIRTHERING;
        else
            ppdev->ulWorkAroundHwFlag &=  ~NV_WORKAROUND_NV11RevB_DIRTHERING;
    }

    // Other chips ...
}
