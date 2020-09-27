//******************************************************************************
//
// Module Name:
//
//     NV4HW.C
//
// Abstract:
//
//     Implements NV4 specific routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//     This driver was adapted from the S3 Display driver
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
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "ddmini.h"
#endif

#include "nv4_ref.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free

//******************************************************************************
// We'll add the 565 bitmap format functions here
// (We'll use the NV3 functions for 555 format)
//******************************************************************************

//******************************************************************************
// Forward declarations
//******************************************************************************

VOID NV4_DmaPushSend(PDEV *);
VOID NV_DmaPush_Wrap(PDEV *, ULONG );
VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG);

VOID MagicDelay (VOID);

#define LOOP_WAIT   450*8

#ifdef _WIN64
VOID MagicDelay (VOID)
{
    volatile int i;

    for (i = 0; i < LOOP_WAIT; i++);
}
#else
VOID MagicDelay (VOID)
{
    _asm              mov     ecx, LOOP_WAIT
    _asm WaitLoop:    loop    WaitLoop
}
#endif


//******************************************************************************
//
// Function: NV4_DmaPushSetSourceBase()
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


VOID NV4_DmaPushSetSourceBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    DECLARE_DMA_FIFO;

    if ((Offset == ppdev->CurrentSourceOffset) &&
        (Stride == (LONG)ppdev->CurrentSourcePitch))

        return;                 // current settings are correct - just return

    //**************************************************************************
    // Get push buffer information
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;


    //**************************************************************************
    // NOTE: BitBlt/Text/Lines currently assume that there are 2 surface objects
    //       (Surface0 and Surface1).  However, in NV4, instead of 2 surface objects,
    //       there is now JUST 1 surface object (NV4_CONTEXT_SURFACES_2D).
    //**************************************************************************

    //**************************************************************************
    // Make sure that pitch is at least 32 (NV4 requirement for CONTEXT_SURFACES_2D)
    // even when NO source is being used during the blit operation
    // (A newly created ppdev ZERO's out these values)
    //**************************************************************************

    if (ppdev->CurrentDestPitch <= ppdev->ulSurfaceAlign)       // Destination Pitch
        ppdev->CurrentDestPitch = ppdev->ulSurfaceAlign+1;

    if ((ULONG)Stride <= ppdev->ulSurfaceAlign)                        // Source Pitch
        Stride = ppdev->ulSurfaceAlign+1;

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(6)));

    //**************************************************************************
    // Setup DSTIMAGE_IN_MEMORY object.
    //**************************************************************************

    if (ppdev->dDrawSpareSubchannelObject != DD_PRIMARY_IMAGE_IN_MEMORY)
        {
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), DD_PRIMARY_IMAGE_IN_MEMORY);
        ppdev->dDrawSpareSubchannelObject     = DD_PRIMARY_IMAGE_IN_MEMORY;
        }

    //**************************************************************************
    // Send methods to DMA push buffer
    //**************************************************************************

    NV_DMAPUSH_START(3, DD_SPARE , NV042_SET_PITCH )
    NV_DMA_FIFO = ((ppdev->CurrentDestPitch <<16) | (Stride));
    NV_DMA_FIFO = Offset;
    NV_DMA_FIFO = ppdev->CurrentDestOffset;

    //**************************************************************************
    // Save the updated offset and pitch, update global freecount.
    //**************************************************************************
    ppdev->CurrentSourceOffset = Offset;
    ppdev->CurrentSourcePitch  = Stride;

    //**************************************************************************
    // Make sure to update the DMA count before we exit!!
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    }


//******************************************************************************
//
// Function: NV4_DmaPushSetDestBase()
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


VOID NV4_DmaPushSetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {

    DECLARE_DMA_FIFO;

    if ((Offset == ppdev->CurrentDestOffset) &&
        (Stride == (LONG)ppdev->CurrentDestPitch))

        return;                 // current settings are correct - just return

    //**************************************************************************
    // Get push buffer information
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;


    //**************************************************************************
    // NOTE: BitBlt/Text/Lines currently assume that there are 2 surface objects
    //       (Surface0 and Surface1).  However, in NV4, instead of 2 surface objects,
    //       there is now JUST 1 surface object (NV4_CONTEXT_SURFACES_2D).
    //**************************************************************************

    //**************************************************************************
    // Make sure that pitch is at least 32 (NV4 requirement for CONTEXT_SURFACES_2D)
    // even when NO source is being used during the blit operation
    // (A newly created ppdev ZERO's out these values)
    //**************************************************************************

    if (ppdev->CurrentSourcePitch <= ppdev->ulSurfaceAlign)     // Source Pitch
        ppdev->CurrentSourcePitch = ppdev->ulSurfaceAlign+1;

    if ((ULONG)Stride <= ppdev->ulSurfaceAlign)                        // Destination Pitch
        Stride = ppdev->ulSurfaceAlign+1;

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(6)));

    //**************************************************************************
    // Setup DSTIMAGE_IN_MEMORY object.
    //**************************************************************************

    if (ppdev->dDrawSpareSubchannelObject != DD_PRIMARY_IMAGE_IN_MEMORY)
        {
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), DD_PRIMARY_IMAGE_IN_MEMORY);
        ppdev->dDrawSpareSubchannelObject     = DD_PRIMARY_IMAGE_IN_MEMORY;
        }

    //**************************************************************************
    // Send methods to DMA push buffer
    //**************************************************************************

    NV_DMAPUSH_START(3, DD_SPARE , NV042_SET_PITCH )
    NV_DMA_FIFO = ((Stride <<16) | (ppdev->CurrentSourcePitch));
    NV_DMA_FIFO = ppdev->CurrentSourceOffset;
    NV_DMA_FIFO = Offset;

    //**************************************************************************
    // Save the updated offset and pitch, update global freecount.
    //**************************************************************************
    ppdev->CurrentDestOffset = Offset;
    ppdev->CurrentDestPitch  = Stride;

    //**************************************************************************
    // Make sure to update the DMA count before we exit!!
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    }

//******************************************************************************
//
// Function: NV4_DmaPushSend()
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


VOID NV4_DmaPushSend(PDEV *ppdev)

    {
    Nv04ControlDma      *nvDmaChannel;
    BOOL                 bReleaseOpenGLMutex = FALSE;

    nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

    //**********************************************************************
    // Check if the ALI 1541 chipset is present.
    // If so, then we need to workaround a cache issue by doing an out to port
    //**********************************************************************

    if (ppdev->AliFixupIoNeeded)
        {
// Assembly is not feasible for IA64
#ifndef _WIN64
        outp(ppdev->AliFixupIoBase, 0);      // This will flush the cache
#endif
        }
        
    //**************************************************************************
    // Some OGL CAD apps need GDI & OGL to be sync'ed up.
    //**************************************************************************
    if (ppdev->oglSyncGdi)
        {
        ppdev->pfnAcquireOglMutex(ppdev);
        ppdev->pfnWaitForChannelSwitch(ppdev);
        ppdev->NVFreeCount = 0;
        bReleaseOpenGLMutex = TRUE;
        }

#ifdef NVD3D    // DX6
    //**********************************************************************
    // If there is another DMA push channel active, wait til it's finished.
    //
    // It hurts performance to check this with every kickoff, so we only do
    // this check when:
    // 1) This is a DX6 build. We can only successfully sync with other DMA
    //    Push Channels and not PIO channels (DX3 is PIO only).
    // 2) DirectDraw is enabled and ddraw objects have been created
    //   (determined by checking if surfaces have been created).
    //
    //**********************************************************************

    if ((ppdev->DDrawEnabledFlag) &&
        (ppdev->pDriverData->DDrawVideoSurfaceCount))
        {
        if (ppdev->pfnWaitForChannelSwitch)
            ppdev->pfnWaitForChannelSwitch(ppdev);
        }
#else
    //**********************************************************************
    // It's possible the DX3 PIO channel is still active. We need to wait
    // til it's finished. It hurts performance to check this with every
    // kickoff, so we only do this check when DirectDraw is enabled.
    //**********************************************************************
    if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
        {
        NV4_DdPioSync(ppdev);
        ppdev->bDDChannelActive = FALSE;
        }
#endif // #ifndef NVD3D

    //**************************************************************************
    // Convert the PUT ptr to a byte address
    //**************************************************************************

    ppdev->nvDmaCachedPut = ppdev->nvDmaCount << 2;

    //**************************************************************************
    // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
    // Then 'OR' it near the end of the push buffer
    //**************************************************************************

    if (ppdev->nvDmaCount >0)
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
    else
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

    *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;

    //**************************************************************************
    // Tell the hardware to start processing the push buffer
    //**************************************************************************

    nvDmaChannel->Put = ppdev->nvDmaCachedPut;

    ppdev->oglLastChannel = GDI_CHANNEL_ID;

    if (bReleaseOpenGLMutex)
        {
        ppdev->pfnReleaseOglMutex(ppdev);
        }

    }

//******************************************************************************
//
// Function: NV_DmaPush_Wrap()
//
// Routine Description:
//
//         This function causes a WRAP around to occur
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

VOID NV_DmaPush_Wrap(PDEV *ppdev, ULONG Dwords_Needed)

    {
    ULONG               totalBytesNeeded;
    Nv04ControlDma      *nvDmaChannel;

    nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

    ASSERTDD(Dwords_Needed < ( (ppdev->DmaPushBufTotalSize>>2) - DMAPUSH_PADDING), "ERROR:  PushBuffer Size TOO small !!!");

#ifdef NVD3D    // DX6
    //**********************************************************************
    // If there is another DMA push channel active, wait til it's finished.
    //
    // It hurts performance to check this with every kickoff, so we only do
    // this check when:
    // 1) This is a DX6 build. We can only successfully sync with other DMA
    //    Push Channels and not PIO channels (DX3 is PIO only).
    // 2) DirectDraw is enabled and ddraw objects have been created
    //   (determined by checking if surfaces have been created).
    //
    //**********************************************************************

    if ((ppdev->DDrawEnabledFlag) &&
        (ppdev->pDriverData->DDrawVideoSurfaceCount))
        {
        if (ppdev->pfnWaitForChannelSwitch)
            ppdev->pfnWaitForChannelSwitch(ppdev);
        }
#else
    //**********************************************************************
    // It's possible the DX3 PIO channel is still active. We need to wait
    // til it's finished. It hurts performance to check this with every
    // kickoff, so we only do this check when DirectDraw is enabled.
    //**********************************************************************
    if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
        {
        NV4_DdPioSync(ppdev);
        ppdev->bDDChannelActive = FALSE;
        }
#endif // #ifndef NVD3D

    //**************************************************************************
    //
    // Write a jump to prepare for wrap.
    //
    //  WRAP_FLAG = TRUE
    //
    //         ---------  0
    //   ---> |         |
    //  |     |         |
    //  |     |         |  <-- Get Ptr
    //  |     |         |
    //  |     |  Push   |
    //  |     | Buffer  |  <-- Put Ptr
    //  |     |         |
    //  |     |         |
    //  |     |         |
    //   ---- |JUMP CMD |  <-- DMA Count
    //         ---------
    //
    //**************************************************************************

    ppdev->nvDmaFifo[ppdev->nvDmaCount] = NV4_JUMP(0);

    //**************************************************************************
    // Check if get pointer is way behind. If it is, wait until it starts to
    // catch up. This way we can distinguish between a pointer that is way behind
    // from a pointer that has wrapped.
    //**************************************************************************

    ppdev->nvDmaCachedGet = nvDmaChannel->Get;

    while (ppdev->nvDmaCachedGet == 0)
        {
        MagicDelay ();
        ppdev->nvDmaCachedGet = nvDmaChannel->Get;
        }

    //**************************************************************************
    // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
    // Then 'OR' it near the end of the push buffer
    //**************************************************************************

    if (ppdev->nvDmaCount >0)
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
    else
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

    *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;


    //******************************************************************
    // Force a kickoff. This will cause the engine to process the current
    // push buffer data and then wraparound to zero.
    //******************************************************************

    nvDmaChannel->Put = 0;
    ppdev->nvDmaCount = 0;

    //**********************************************************************
    //
    //  WRAP_FLAG = TRUE
    //
    //  Wait until Get >= Dwords_Needed
    //
    //         ---------  0
    //        |         |  <-- Put Ptr <------ DMA Count
    //        |         |                         ^
    //        |         |                         |
    //        |         |                         |--- Dwords Needed
    //        |  Push   |                         |
    //        | Buffer  |                         v
    //        |         |  <-- Get Ptr     <-------
    //        |         |
    //        |         |
    //        |JUMP CMD |
    //         ---------
    //
    //**********************************************************************

    totalBytesNeeded = (Dwords_Needed + (DMAPUSH_PADDING)) << 2;

    ppdev->nvDmaCachedGet = nvDmaChannel->Get;

    while (ppdev->nvDmaCachedGet > 0)
        {
        if (totalBytesNeeded < ppdev->nvDmaCachedGet)
            {
            ppdev->nvDmaWrapFlag = TRUE;
            return;
            }

        MagicDelay ();
        ppdev->nvDmaCachedGet = nvDmaChannel->Get;

        }


    //**********************************************************************
    //
    // WRAP_FLAG = FALSE
    //
    // We are no longer wrapped, Get == 0, wrap has occurred. We should have
    // enough room now.
    //
    //         ---------   <- Put Ptr  <- DMA Count <- Get Ptr
    //        |         |
    //        |         |
    //        |         |
    //        |         |
    //        |  PUSH   |
    //        | BUFFER  |
    //        |         |
    //        |         |
    //        |         |
    //        |         |
    //         ---------
    //
    //**********************************************************************

    ppdev->nvDmaWrapFlag = FALSE;
    return;

    }

//******************************************************************************
//
// Function: NV_DmaPush_CheckWrapped()
//
// Routine Description:
//
//          Make sure that the DmaCount PUT ptr does NOT go past the
//          GET ptr (this occurs when DmaCount is wrapped around and
//          catching up to the GET ptr).
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

VOID NV_DmaPush_CheckWrapped(PDEV *ppdev, ULONG Dwords_Needed)

    {
    ULONG               totalBytesNeeded;
    Nv04ControlDma      *nvDmaChannel;
    nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

    //**************************************************************************
    //
    //         WRAP_FLAG = TRUE
    //
    //         ---------  0
    //        |         |  <---- Put Ptr
    //        |         |
    //        |         |
    //        |  Push   |  <----- DMA Count (Where CPU currently writes data)
    //        | Buffer  |    |
    //        |         |    |
    //        |         |    v
    //        |         |  <----- Get Ptr
    //        |         |    |
    //        |         |    v
    //        |JUMP CMD |
    //         ---------
    //
    //**************************************************************************

    ASSERTDD(Dwords_Needed < ((ppdev->DmaPushBufTotalSize>>2) - DMAPUSH_PADDING), "ERROR:  PushBuffer Size TOO small !!!");

#ifdef NVD3D    // DX6
    //**********************************************************************
    // If there is another DMA push channel active, wait til it's finished.
    //
    // It hurts performance to check this with every kickoff, so we only do
    // this check when:
    // 1) This is a DX6 build. We can only successfully sync with other DMA
    //    Push Channels and not PIO channels (DX3 is PIO only).
    // 2) DirectDraw is enabled and ddraw objects have been created
    //   (determined by checking if surfaces have been created).
    //
    //**********************************************************************

    if ((ppdev->DDrawEnabledFlag) &&
        (ppdev->pDriverData->DDrawVideoSurfaceCount))
        {
        if (ppdev->pfnWaitForChannelSwitch)
            ppdev->pfnWaitForChannelSwitch(ppdev);
        }
#else
    //**********************************************************************
    // It's possible the DX3 PIO channel is still active. We need to wait
    // til it's finished. It hurts performance to check this with every
    // kickoff, so we only do this check when DirectDraw is enabled.
    //**********************************************************************
    if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
        {
        NV4_DdPioSync(ppdev);
        ppdev->bDDChannelActive = FALSE;
        }
#endif // #ifndef NVD3D

    //**************************************************************************
    // Calculate the number of bytes needed in the push buffer
    //**************************************************************************

    totalBytesNeeded = (ppdev->nvDmaCount + Dwords_Needed + DMAPUSH_PADDING) << 2;

    //**************************************************************************
    // Get the current GET ptr from hardware
    //**************************************************************************

    ppdev->nvDmaCachedGet = nvDmaChannel->Get;

    //**************************************************************************
    // Check if the GET ptr is still ahead of the DMA count ptr
    //**************************************************************************

    if (ppdev->nvDmaCount < (ppdev->nvDmaCachedGet>>2))
        {
        //******************************************************************
        //
        //  CASE 1:
        //
        //  WRAP_FLAG = TRUE
        //
        //  CachedGet >= Dwords_Needed, just return immediately
        //
        //         ---------  0
        //        |         |
        //        |         |  <------------------ DMA Count
        //        |         |                         ^
        //        |  Push   |                         |
        //        | Buffer  |                         |--- Dwords Needed
        //        |         |                         |
        //        |         |                  <------
        //        |         |  <-- Get Ptr
        //        |         |    |
        //        |         |    |
        //        |         |    v
        //        |JUMP CMD |
        //         ---------
        //
        //******************************************************************

        if (totalBytesNeeded < ppdev->nvDmaCachedGet)
            return;

        //******************************************************************
        //
        // CASE 2:
        //
        //      WRAP_FLAG = TRUE
        //
        //      Dwords Needed < remaining space available
        //      Then need to wait for get pointer to advance
        //
        //         ---------  0
        //        |         |
        //        |         |
        //        |  Push   |  <----- DMA Count (Where CPU currently writes data)
        //        | Buffer  |    |                  |
        //        |         |    |                  |
        //        |         |    v                  |
        //        |         |  <----- Get Ptr       |-- Dwords Needed < remaining space
        //        |         |    |                  |
        //        |         |    v                  |
        //        |JUMP CMD |  <--------------------
        //         ---------
        //
        //******************************************************************

        else if (totalBytesNeeded < (ppdev->DmaPushBufTotalSize))
            {
            while (ppdev->nvDmaCount < (ppdev->nvDmaCachedGet>>2))
                {
                if (totalBytesNeeded < ppdev->nvDmaCachedGet)
                    return;

                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }

            //*************************************************************
            //
            //      WRAP_FLAG = FALSE
            //
            // We are no longer wrapped
            //
            //         ---------   <- Get Ptr
            //        |         |
            //        |         |
            //        |         |
            //        |  Push   |  <----- DMA Count (Where CPU currently writes data)
            //        | Buffer  |                       |
            //        |         |                       |
            //        |         |                       |
            //        |         |                       |-- Dwords Needed < remaining space
            //        |         |                       |
            //        |         |                       |
            //        |         |  <--------------------
            //         ---------
            //
            //******************************************************************

            ppdev->nvDmaWrapFlag = FALSE;
            return;

            }

        //******************************************************************
        //
        // CASE 3:
        //
        //      WRAP_FLAG = TRUE
        //
        //      Dwords Needed > remaining space available
        //
        //      We are way behind here, so we need to wait and catch up or else
        //      we will overwrite data.
        //
        //         ---------  0
        //        |         |
        //        |         |
        //        |  Push   |
        //        | Buffer  |
        //        |         |
        //        |         |
        //        |         |  <----- DMA Count (Where CPU currently writes data)
        //        |         |    |                  |
        //        |         |    v                  |
        //        |         |  <----- Get Ptr       |-- Dwords Needed > remaining space
        //        |         |    |                  |
        //        |JUMP CMD |    v                  |
        //         ---------                        |
        //                                          |
        //                     <--------------------
        //
        //******************************************************************

        else    // (totalBytesNeeded > DMAPUSH_BUFFER_SIZE)
            {

            //******************************************************************
            //
            // Wait for wraparound to occur
            //
            //         ---------   <- Get Ptr
            //        |         |
            //        |         |
            //        |  Push   |
            //        | Buffer  |
            //        |         |
            //        |         |
            //        |         |
            //        |         |  <----- DMA Count (Where CPU currently writes data)
            //        |         |                       |
            //        |         |                       |
            //        |         |                       |-- Dwords Needed > remaining space
            //        |         |                       |
            //         ---------                        |
            //                                          |
            //                     <--------------------
            //
            //******************************************************************

            while (ppdev->nvDmaCount < (ppdev->nvDmaCachedGet>>2))
                {
                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }

            //******************************************************************
            // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
            // Then 'OR' it near the end of the push buffer
            //******************************************************************

            if (ppdev->nvDmaCount >0)
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
            else
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

            *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;


            //******************************************************************
            //
            // Kick off so pointers catch up to DMA count
            //
            //         ---------
            //        |         |
            //        |         | <-- Get Ptr
            //        |  Push   |
            //        | Buffer  |
            //        |         |
            //        |         |
            //        |         |
            //        |JUMP CMD |  <----- DMA Count <- Put Ptr
            //        |         |                       |
            //        |         |                       |
            //        |         |                       |-- Dwords Needed > remaining space
            //        |         |                       |
            //         ---------                        |
            //                                          |
            //                     <--------------------
            //
            //******************************************************************

            ppdev->nvDmaCachedPut = ppdev->nvDmaCount << 2;

            nvDmaChannel->Put = ppdev->nvDmaCachedPut;

            //******************************************************************
            // Make sure Get ptr is not zero, otherwise we can't distinguish between
            // a pointer that is way behind from a pointer that has wrapped.
            //******************************************************************

            ppdev->nvDmaCachedGet = nvDmaChannel->Get;

            while (ppdev->nvDmaCachedGet == 0)
                {
                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }

            //******************************************************************
            //
            // Force a kickoff. This will cause the engine to process the current
            // push buffer data and then wraparound to zero.
            //
            //         ---------   <- Put Ptr  <- DMA Count
            //        |         |
            //        |         |
            //        |  Push   |  <-- Get Ptr
            //        | Buffer  |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |JUMP CMD |
            //        |         |
            //        |         |
            //         ---------
            //
            //******************************************************************

            ppdev->nvDmaFifo[ppdev->nvDmaCount] = NV4_JUMP(0);

            //******************************************************************
            // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
            // Then 'OR' it near the end of the push buffer
            //******************************************************************

            if (ppdev->nvDmaCount >0)
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
            else
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

            *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;


            ppdev->nvDmaCount = 0;
            nvDmaChannel->Put = 0;

            totalBytesNeeded = (Dwords_Needed + DMAPUSH_PADDING) << 2;

            ppdev->nvDmaCachedGet = nvDmaChannel->Get;

            while (ppdev->nvDmaCachedGet > 0)
                {
                if (totalBytesNeeded < ppdev->nvDmaCachedGet)
                    {
                    ppdev->nvDmaWrapFlag = TRUE;
                    return;
                    }
                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }


            //******************************************************************
            //
            //      WRAP_FLAG = FALSE
            //
            // Get pointer has wrapped.
            //
            //         ---------   <- Put Ptr  <- DMA Count <-- Get Ptr
            //        |         |
            //        |         |
            //        |  Push   |
            //        | Buffer  |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //         ---------
            //
            //******************************************************************

            ppdev->nvDmaWrapFlag = FALSE;
            return;

            }
        }
    else        // (ppdev->nvDmaCount > (ppdev->nvDmaCachedGet>>2))
        {

        //******************************************************************
        //
        //  CASE 4:
        //
        //      WRAP_FLAG = FALSE
        //
        //         ---------  0
        //        |         |
        //        |         |
        //        |         |  <-- Get Ptr
        //        |         |
        //        |         |  <-------------- DMA Count
        //        |         |                         |
        //        |         |                         |--- Dwords Needed
        //        |  Push   |                         |
        //        | Buffer  |                         |
        //        |         |                 <-------
        //        |         |
        //         ---------
        //
        //         Total Size of Push Buffer
        //
        //******************************************************************

        if (totalBytesNeeded <= (ppdev->DmaPushBufTotalSize))
            {
            ppdev->nvDmaWrapFlag = FALSE;       // no longer wrapped
            return;
            }

        //******************************************************************
        //
        //  CASE 5:
        //
        //      WRAP_FLAG = FALSE
        //
        //         ---------  0
        //        |         |
        //        |         |
        //        |         |  <-- Get Ptr
        //        |         |
        //        |         |
        //        |         |  <-------------- DMA Count
        //        |  Push   |                         |
        //        | Buffer  |                         |
        //        |         |                         |
        //        |         |                         |--- Dwords Needed
        //        |         |                         |
        //         ---------                          |
        //                                    <-------
        //******************************************************************

        else        // totalBytesNeeded > DMAPUSH_BUFFER_SIZE
            {

            //******************************************************************
            // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
            // Then 'OR' it near the end of the push buffer
            //******************************************************************

            if (ppdev->nvDmaCount >0)
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
            else
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

            *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;


            //******************************************************************
            //
            // WRAP_FLAG = TRUE
            //
            // Force a kickoff. This will cause the engine to process the current
            // push buffer data and then wraparound to zero.
            //
            //         ---------   <- Put Ptr  <- DMA Count
            //        |         |
            //        |         |
            //        |  Push   |
            //        | Buffer  |
            //        |         |  <- Get Ptr
            //        |         |
            //        |         |
            //        |         |
            //        |JUMP CMD |
            //        |         |
            //        |         |
            //        |         |
            //         ---------
            //
            //******************************************************************

            ppdev->nvDmaCachedPut = ppdev->nvDmaCount << 2;

            nvDmaChannel->Put = ppdev->nvDmaCachedPut;

            //******************************************************************
            // Make sure Get ptr is not zero, otherwise we can't distinguish between
            // a pointer that is way behind from a pointer that has wrapped.
            //******************************************************************

            ppdev->nvDmaCachedGet = nvDmaChannel->Get;

            while (ppdev->nvDmaCachedGet == 0)
                {
                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }

            ppdev->nvDmaFifo[ppdev->nvDmaCount] = NV4_JUMP(0);

            //******************************************************************
            // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
            // Then 'OR' it near the end of the push buffer
            //******************************************************************

            if (ppdev->nvDmaCount >0)
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
            else
                ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

            *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;


            ppdev->nvDmaCount = 0;
            nvDmaChannel->Put = 0;

            totalBytesNeeded = (Dwords_Needed + DMAPUSH_PADDING) << 2;

            ppdev->nvDmaCachedGet = nvDmaChannel->Get;

            while (ppdev->nvDmaCachedGet > 0)
                {
                if (totalBytesNeeded < ppdev->nvDmaCachedGet)
                    {
                    ppdev->nvDmaWrapFlag = TRUE;
                    return;
                    }
                MagicDelay ();
                ppdev->nvDmaCachedGet = nvDmaChannel->Get;
                }

            //******************************************************************
            //
            //      WRAP_FLAG = FALSE
            //
            // We are no longer wrapped, Get == 0, wrap has occurred
            //
            //         ---------   <- Put Ptr  <- DMA Count <- Get Ptr
            //        |         |
            //        |         |
            //        |  Push   |
            //        | Buffer  |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //        |         |
            //         ---------
            //
            //******************************************************************

            ppdev->nvDmaWrapFlag = FALSE;
            return;

            }
        }
    }


//******************************************************************************
//
// Function: NV4_DmaPushWaitOnSync
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

VOID NV4_DmaPushWaitOnSync(PDEV *ppdev)

    {
    Nv04ControlDma      *nvDmaChannel;

    nvDmaChannel = (Nv04ControlDma *)(ppdev->nvDmaChannel);

#ifdef NVD3D    // DX6
    //**********************************************************************
    // If there is another DMA push channel active, wait til it's finished.
    //
    // It hurts performance to check this with every kickoff, so we only do
    // this check when:
    // 1) This is a DX6 build. We can only successfully sync with other DMA
    //    Push Channels and not PIO channels (DX3 is PIO only).
    // 2) DirectDraw is enabled and ddraw objects have been created
    //   (determined by checking if surfaces have been created).
    //
    //**********************************************************************

    if ((ppdev->DDrawEnabledFlag) &&
        (ppdev->pDriverData->DDrawVideoSurfaceCount))
        {
        if (ppdev->pfnWaitForChannelSwitch)
            ppdev->pfnWaitForChannelSwitch(ppdev);
        }
#else
    //**********************************************************************
    // It's possible the DX3 PIO channel is still active. We need to wait
    // til it's finished. It hurts performance to check this with every
    // kickoff, so we only do this check when DirectDraw is enabled.
    //**********************************************************************
    if (ppdev->DDrawEnabledFlag && ppdev->bDDChannelActive)
        {
        NV4_DdPioSync(ppdev);
        ppdev->bDDChannelActive = FALSE;
        }
#endif // #ifndef NVD3D

    //**************************************************************************
    // Wait for all commands to be processed
    //**************************************************************************

    ppdev->nvDmaCachedGet = nvDmaChannel->Get;
    while (ppdev->nvDmaCachedGet != ppdev->nvDmaCachedPut)
        {
        MagicDelay ();
        ppdev->nvDmaCachedGet = nvDmaChannel->Get;
        }

    //**************************************************************************
    // VIA / Flush Write Combine Fix - Read the last DWORD that was output.
    // Then 'OR' it near the end of the push buffer
    //**************************************************************************

    if (ppdev->nvDmaCount >0)
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[((ppdev->nvDmaCount)-1)];
    else
        ppdev->nvDmaFlushFixDummyValue = ppdev->nvDmaFifo[0];

    *(ppdev->nvDmaFlushFixDwordPtr) |= ppdev->nvDmaFlushFixDummyValue;

    //**************************************************************************
    // Kickoff again
    //**************************************************************************

    nvDmaChannel->Put = 0;

    //**************************************************************************
    // Wait for jump wraparound
    //**************************************************************************

    ppdev->nvDmaCachedGet = nvDmaChannel->Get;
    while (ppdev->nvDmaCachedGet != 0)
        {
        MagicDelay ();
        ppdev->nvDmaCachedGet = nvDmaChannel->Get;
        }
    }

//******************************************************************************
//
// Function: NV4_DdPioSync
//
// Routine Description:
// This routine is called from the GDI driver (as well as the the DD PIO
// code) to wait until the PIO channel has completed processing.
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

VOID NV4_DdPioSync(PDEV *ppdev)
{
    Nv3ChannelPio *nvDDChannel = (Nv03ChannelPio *)(ppdev->ddChannelPtr);
    USHORT freeCount = NvGetFreeCount(nvDDChannel, 2);
    NvNotification *pSyncNotifier = (NvNotification *)ppdev->Notifiers->Sync;

    //**************************************************************
    // Sync on method notification for NV_IMAGE_BLACK_RECTANGLE object.
    // This should ensure all processing of PIO channel has completed.
    //**************************************************************
    while (freeCount < 2 * 4)
        freeCount = NvGetFreeCount(nvDDChannel, 2);
    freeCount -= 2 * 4;

    while ((volatile) pSyncNotifier[NV019_NOTIFIERS_NOTIFY].status == NV019_NOTIFICATION_STATUS_IN_PROGRESS);
    pSyncNotifier[NV019_NOTIFIERS_NOTIFY].status = NV019_NOTIFICATION_STATUS_IN_PROGRESS;
    nvDDChannel->subchannel[6].nv1ImageBlackRectangle.Notify = NV019_NOTIFY_WRITE_ONLY;
    nvDDChannel->subchannel[6].nv1ImageBlackRectangle.NoOperation = 0;
    while ((volatile) pSyncNotifier[NV019_NOTIFIERS_NOTIFY].status == NV019_NOTIFICATION_STATUS_IN_PROGRESS);
    ppdev->NVFreeCount = freeCount;
}

//*****************************Public*Routine******************************\
// void NvSetDacImageOffset
//
// This routine set the new offset of Dac so that the display window can be
// on any part of frame buffer.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     ulDac - Which Dac is going to set, 0 - primary DAC
//     ulOffset - New offset in in the display desktop [ulDac] area.
//              - it is offset from the ppdev->ulPrimarySurfaceOffset
//
// Return Value:
//
//     None.
//**************************************************************************/
void __cdecl NvSetDacImageOffset(PDEV* ppdev, ULONG ulDac, ULONG ulOffset)
{
    // !!! To Do
    // ulDac is not current used; will be used for NV11 Dual View.
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    // Wait for previous image completed
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;

    NV_DMAPUSH_CHECKFREE( ((ULONG)(6)));

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulDac);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_OFFSET(0), ulOffset + ppdev->ulPrimarySurfaceOffset);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_FORMAT(0), ppdev->lDelta |
                    NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31);

    UPDATE_PDEV_DMA_COUNT;

    //******************************************************************
    // Send data on thru to the DMA push buffer
    //******************************************************************
    NV4_DmaPushSend(ppdev);

}
