//******************************************************************************
//
// Module Name:
//
//     NV4DDRW.C
//
// Abstract:
//
//     Implements all NV4 specific routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"
#include "nv32.h"
#include "driver.h"

#include "nv4_ref.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// We'll add the 565 bitmap format functions here
// (We'll use the NV3 functions for 555 format)
//******************************************************************************

#define MASK_OUT_OTHER_CHANNELS         0xFFFFFFFE
#define OFFSET_NV_PFIFO_BASE            (0x2000)
#define OFFSET_NV_PFIFO_DMA_REG         (0x2508-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_PUSH1    (0x3204-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_STATUS   (0x3214-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_PUSH (0x3220-OFFSET_NV_PFIFO_BASE)
#define CACHE1_DMA_PUSH_BUFFER_EMPTY    0x100
#define CACHE1_PUSH1_MODE_DMA           0x100
#define CACHE1_STATUS_LOW_MARK_EMPTY    0x010

#define OFFSET_NV_PGRAPH_DEBUG_3                     (0x0040008C - 0x00400000)
#define POSTDITHER_2D_ENABLE                          0x00001000
#define PREDITHER_2D_ENABLE                           0x00004000

//
//#define OFFSET_PRMCIO_INP0_COLOR_REG                0x3da
//
//#define OFFSET_PRMCIO_CRX__COLOR_REG                0x3d4
//#define OFFSET_PRMCIO_CR__COLOR_REG                 0x3d5
//
//#define OFFSET_PRMVIO_SRX_REG                       0x3c4
//#define OFFSET_PRMVIO_SR_LOCK_REG                   0x3c5
//#define OFFSET_PRMVIO_MISC_READ_REG                 0x3cc
//#define OFFSET_PRAMDAC_CU_START_POS_REG             0x0
//
//#define NV_SR_UNLOCK_VALUE                          0x00000057
//#define NV_SR_LOCK_VALUE                            0x00000099
//


//******************************************************************************
// Forward Declarations
//******************************************************************************

ULONG NV4_VBlankIsActive(PDEV *);
ULONG NV4_DisplayIsActive(PDEV *);
ULONG NV4_GraphicsEngineBusy(PDEV *);
VOID NV4_WaitWhileVBlankActive(PDEV * );
VOID NV4_WaitWhileDisplayActive(PDEV * );
VOID NV4_SetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV4_DmaPushSend(PDEV *);
VOID MagicDelay ();

//******************************************************************************
// External Declarations
//******************************************************************************

extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );

//******************************************************************************
//
// Function: NV4_VBlankIsActive()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


ULONG NV4_VBlankIsActive(PDEV*   ppdev)

    {
    //**************************************************************************
    // Get VBLANK status
    //**************************************************************************
    if (REG_RD_DRF(_PCRTC, _RASTER, _VERT_BLANK))
        return(TRUE);
    return(FALSE);

    }


//******************************************************************************
//
// Function: NV4_DisplayIsActive()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


ULONG NV4_DisplayIsActive(PDEV*   ppdev)

    {
    //**************************************************************************
    // Get DISPLAY status
    //**************************************************************************
    if (REG_RD_DRF(_PCRTC, _RASTER, _VERT_BLANK))
        return(FALSE);
    return(TRUE);
    }

#pragma optimize("",off)    // Need this so we only do ULONG reads from the hw

//******************************************************************************
//
// Function: NV4_ChannelIsGdi()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

ULONG NV4_ChannelIsGdi(PDEV *ppdev)

    {
    volatile ULONG  *pfifoDmaReg;
    volatile ULONG  regValue;

    pfifoDmaReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_DMA_REG);

    regValue = READ_REGISTER_ULONG(pfifoDmaReg);
    if ((regValue & MASK_OUT_OTHER_CHANNELS) == 0) // if its GDI return TRUE
        {
        return(1); // GDI channel
        }

    return(0); // not GDI channel
    }

//******************************************************************************
//
// Function: NV4_WaitForChannelSwitch()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4_WaitForChannelSwitch(PDEV*   ppdev)

    {
    volatile ULONG  *pfifoDmaReg;
    volatile ULONG  *pfifoCache1Push1;
    volatile ULONG  *pfifoCache1Status;
    volatile ULONG  *pfifoCache1DmaPush;
    volatile ULONG  *GrStatusReg;
    volatile ULONG  regValue;

    if (!ppdev->PFIFORegs)
        {
        ppdev->oglLastChannel = GDI_CHANNEL_ID;
        return;
        }

    if (ppdev->oglLastChannel == GDI_CHANNEL_ID)
        {
        return;
        }
    
    pfifoDmaReg        = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_DMA_REG);
    pfifoCache1Push1   = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PUSH1);
    pfifoCache1Status  = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_STATUS);
    pfifoCache1DmaPush = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_PUSH);

    //
    // Wait for all DMA push channels to be not busy.
    // TODO: wait on DirectDraw PIO channel...
    // TODO: in these loops we should not just hammer the bus, we should probably do a sleep...
    //
    regValue = READ_REGISTER_ULONG(pfifoDmaReg);
    // Assume that GDI is channel 0 and wait for all other channels to go to 0 (not active)
    while (regValue)
        {
        MagicDelay ();
        regValue = READ_REGISTER_ULONG(pfifoDmaReg);
        }

    //
    // If its not the GDI channel then wait for DMA push to complete in FIFO
    //
    regValue = READ_REGISTER_ULONG(pfifoCache1Push1);
    if ((regValue & CACHE1_PUSH1_MODE_DMA) && ((regValue & 0x1F) != GDI_CHANNEL_ID))
        {
        regValue = READ_REGISTER_ULONG(pfifoCache1DmaPush);
        while ((regValue & CACHE1_DMA_PUSH_BUFFER_EMPTY) == 0)
            {
            MagicDelay ();
            regValue = READ_REGISTER_ULONG(pfifoCache1DmaPush);
            }
        regValue = READ_REGISTER_ULONG(pfifoCache1Status);
        while ((regValue & CACHE1_STATUS_LOW_MARK_EMPTY) == 0)
            {
            MagicDelay ();
            regValue = READ_REGISTER_ULONG(pfifoCache1Status);
            }
        }

    GrStatusReg = ppdev->GrStatusReg;

    while (*GrStatusReg)
        {
        MagicDelay ();
        }

    // On NV15 there is a one cycle bubble between the fifo and the gfx
    // engine where the status register may indicate idle.  Reading it
    // twice ensures that we won't hit the bubble.
    while (*GrStatusReg)
        {
        MagicDelay ();
        }

    return;
    }
#pragma optimize("",on)


//******************************************************************************
//
// Function: NV4_GraphicsEngineBusy()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


ULONG NV4_GraphicsEngineBusy(PDEV*   ppdev)

    {
    volatile ULONG  *pfifoDmaReg;
    ULONG           intr;

    if (ppdev == NULL)
        return FALSE;

    pfifoDmaReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_DMA_REG);

    //
    // Return DMA push register status
    // FIXME: TODO check DirectDraw PIO channel...
    //

    intr = READ_REGISTER_ULONG(pfifoDmaReg);
    if  (intr)
        return(TRUE);
    else
        return(FALSE);
    }


//******************************************************************************
//
// Function: NV4_WaitWhileVBlankActive()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4_WaitWhileVBlankActive(PDEV*   ppdev)

    {
    while (NV4_VBlankIsActive(ppdev))
        MagicDelay();
    }

//******************************************************************************
//
// Function: NV3_WaitWhileDisplayActive()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4_WaitWhileDisplayActive(PDEV*   ppdev)

    {
    while (NV4_DisplayIsActive(ppdev))
        MagicDelay();
    }


//******************************************************************************
//
// Function: NV4_WaitWhileGraphicsEngineBusy()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4_WaitWhileGraphicsEngineBusy(PDEV*   ppdev)

    {
    if (ppdev == NULL)
        return;

    NV4_WaitForChannelSwitch(ppdev);

    ppdev->NVFreeCount = 0;

    }


//******************************************************************************
//
// Function: NV4_DmaPushWaitForChannelSwitch()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4_DmaPushWaitForChannelSwitch(PDEV*   ppdev)

    {

    if (ppdev == NULL)
        return;

    NV4_WaitForChannelSwitch(ppdev);

    return;
    }

//******************************************************************************
//
// Function: NV4_DmaPushGraphicsEngineBusy()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


ULONG NV4_DmaPushGraphicsEngineBusy(PDEV*   ppdev)

    {
    volatile ULONG *GrStatusReg;

// TODO: Update this code for NV4 !!

    //**************************************************************************
    // It's possible that the device may have been disabled, and this function
    // still gets called (when pjmmbase == NULL). So make sure we don't
    // try to access the user channel if it doesn't exist!
    //**************************************************************************

    //**************************************************************************
    // Get pointer to graphics status register
    //**************************************************************************

    GrStatusReg = ppdev->GrStatusReg;

    //**************************************************************************
    // Wait for graphics engine to to finish
    //
    // WARNING: The compiler will generate code that reads the
    //          bottom BYTE of the CONFIG0 REG. We need to be
    //          aware of this in case we need to use registers
    //          which REQUIRE entire DWORD reads. To make sure we read
    //          an ENTIRE Dword, we can set a bit in the highest
    //          byte. This will force a DWORD read.  Also, we
    //          know that Bit 31 will ALWAYS be 0 for PFB_CONFIG_0
    //          register. So, no harm done. As always, check the LISTING !!
    //**************************************************************************

    if  (*GrStatusReg)  //& (NV_PGRAPH_STATUS_STATE_BUSY | 0x80000000))
        return(TRUE);
    else
        return(FALSE);
    }

 
//******************************************************************************
//
// Function: NV4_DmaPushWaitWhileGraphicsEngineBusy()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID NV4_DmaPushWaitWhileGraphicsEngineBusy(PDEV*   ppdev)

    {
    DECLARE_DMA_FIFO;

    // HW is not intialized yet.
    if((ppdev == NULL) || (!ppdev->bEnabled) || (ppdev->NVSignature != NV_SIGNATURE))
        return;

    //**************************************************************************
    // MAKE SURE that the RM is done updating the palette registers
    // before we start freeing the contexts associated with the colormap object
    // (Make sure the 2nd notifier array entry (index 1) is zero)
    // See NV4DmaPushSetPalette for more information
    //**************************************************************************
    if (ppdev->iBitmapFormat == BMF_8BPP)
        while ( (volatile)(((NvNotification *) (&(ppdev->Notifiers->DmaToMem[NVFF9_NOTIFIERS_COLORMAP_DIRTY_NOTIFY*sizeof(NvNotification)])))->status) != NVFF9_NOTIFICATION_STATUS_DONE_SUCCESS );

    //*************************************************************************************************************************
    // Here is the sequence of opearations we do to ensure we syncronize correctly with other channels and the graphics engines.
    // what we will do.
    // 1.   Do a channelswitch to make sure GDI is the current channel.
    // 2.   Do a notifier to make sure the GDI channel's pushbuffer and hardware FIFO is empty and that the graphics engine
    //      is idle.
    // 3.   Read the Graphics status register to make sure the graphics engine is idle. This third step is not really
    //      necessary as the notifier should guarantee us that the graphics engine is idle but it should not hurt.
    // 4.   Finally, Set the CachedGet Pointer to be equal to the CachedPut pointer as the pushbuffer is guaranteed to be
    //      empty at this point.
    //*************************************************************************************************************************

    //**********************************
    // First do a channel switch to GDI
    //**********************************
    ppdev->pfnWaitForChannelSwitch(ppdev);

    //*********************************************************************************************************
    // Using the notifier method is the most guaranteed way of making sure our channel has finished rendering.
    // We use a notifier to make sure the
    // engine is no longer busy. We'll just send a NO-OP (using the
    // RECT_AND_TEXT object) and wait for the notifier to complete.
    //*********************************************************************************************************
    INIT_LOCAL_DMA_FIFO;
    if (ppdev->nvDmaFifo == NULL)
        return;
    ((NvNotification *) (ppdev->Notifiers->DmaToMem))->status = NV04A_NOTIFICATION_STATUS_IN_PROGRESS;

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************
    NV_DMAPUSH_CHECKFREE(((ULONG)(4)));

    //**************************************************************************
    // Send the NO-OP on thru
    //**************************************************************************
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL,  NV04A_NOTIFY       , NV04A_NOTIFY_WRITE_ONLY);
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL,  NV04A_NO_OPERATION , 0x00000000);

    //**************************************************************************
    // Kickoff the DMA push operation immediately
    // Make sure to update the DMA put pointer first
    //**************************************************************************
    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);

    //**************************************************************************
    // Wait for completion...
    //**************************************************************************
    while ( ((NvNotification *) (ppdev->Notifiers->DmaToMem))->status == NV04A_NOTIFICATION_STATUS_IN_PROGRESS);

    //*****************************************************************************************************************
    // Now read the graphics status register. This is strictly not necessary since we are using the notifier above, but
    // it should not hurt.
    //*****************************************************************************************************************
    while (NV4_DmaPushGraphicsEngineBusy(ppdev));

    //*************************************************************************************************
    // Now we know that our push buffer is empty. So set the Get pointer to be equal to the put pointer
    //*************************************************************************************************
    ppdev->nvDmaCachedGet = ppdev->nvDmaCachedPut;
}

 
//******************************************************************************
//
// Function: NV4_AcquireOglMutex
//
// Routine Description:
//
//     Acquire the mutex that keeps OpenGL off the hardware.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
VOID NV4_AcquireOglMutex(PDEV *ppdev)
{
}

//******************************************************************************
//
// Function: NV4_ReleaseOglMutex
//
// Routine Description:
//
//     Release the mutex that keeps OpenGL off the hardware.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID NV4_ReleaseOglMutex(PDEV *ppdev)
{
}


//******************************************************************************
//
// Function: nvDDrawSynWithNvGdiDrv 
//
// Routine Description:
//     This funciton is called by nvDDraw to syn. rendering
//     
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
void __cdecl nvDDrawSynWithNvGdiDrv(PDEV*   ppdev)
{
    ppdev->pfnWaitEngineBusy(ppdev);
}

