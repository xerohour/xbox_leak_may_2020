//******************************************************************************
//
// Module Name:
//
//     NV3DDRW.C
//
// Abstract:
//
//     Implements all NV3 specific routines
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
#ifdef NV3

#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"
#include "nv3a_ref.h"

#include "nvsubch.h"
#include "nvalpha.h"
#include "nvapi.h"

#undef DEBUG_MSG_CHANNEL_WAIT

//******************************************************************************
// Forward Declarations
//******************************************************************************
VOID NV3_FlipBuffer(PDEV *, ULONG);

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

//******************************************************************************
// Forward Declarations
//******************************************************************************

ULONG NV3_VBlankIsActive(PDEV *);
ULONG NV3_DisplayIsActive(PDEV *);
ULONG NV3_GraphicsEngineBusy(PDEV *);
VOID NV3_WaitWhileVBlankActive(PDEV * );
VOID NV3_WaitWhileDisplayActive(PDEV * );
VOID NV3_SetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV3_WaitForFifoAndEngineIdle(PDEV* ppdev);


//******************************************************************************
//
// Function: NV3_VBlankIsActive()
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


ULONG NV3_VBlankIsActive(PDEV*   ppdev)

    {
    return (!(ppdev->pfnGetScanline(ppdev)));
    }


//******************************************************************************
//
// Function: NV3_DisplayIsActive()
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


ULONG NV3_DisplayIsActive(PDEV*   ppdev)

    {
    return (ppdev->pfnGetScanline(ppdev));
    }


//******************************************************************************
//
// Function: NV3_WaitForFifoAndEngineIdle()
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


VOID NV3_WaitForFifoAndEngineIdle(PDEV* ppdev)

    {
    volatile ULONG *Intr0Reg;
    volatile ULONG *CachesReg;
    volatile ULONG *Cache1Pull0Reg;
    ULONG cache1Pull0Value, cachesValue, intr;
    volatile ULONG *GrStatusReg;
    volatile ULONG *Cache1Dma0Reg;
    volatile ULONG *Cache1StatusReg;

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "NV3_WaitForFifoAndEngineIdle: Entry"));
#endif
    //**************************************************************************
    // Wait for pending interrupts to be completed...
    //**************************************************************************
    Intr0Reg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_INTR_0);
    intr = READ_REGISTER_ULONG(Intr0Reg) & 0x10111;
    while (intr)
    {
        // just look at cache_error, runout_pending, overflow_pending, and DMA PTE pending
        intr = READ_REGISTER_ULONG(Intr0Reg) & 0x10111;
    }

    //**************************************************************************
    // Wait for the FIFO to be enabled by potential RM operations...
    //**************************************************************************
    CachesReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHES);
    cachesValue = READ_REGISTER_ULONG(CachesReg);
    while ((cachesValue & 0x1) == 0)
        {
        cachesValue = READ_REGISTER_ULONG(CachesReg);
        }

    Cache1Pull0Reg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PULL0);
    cache1Pull0Value = READ_REGISTER_ULONG(Cache1Pull0Reg);
    while ((cache1Pull0Value & 0x1) == 0)
        {
        cache1Pull0Value = READ_REGISTER_ULONG(Cache1Pull0Reg);
        }

    //**************************************************************************
    // Wait for DMA pusher to be complete...
    //**************************************************************************
    Cache1Dma0Reg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA0_REG);
    while (READ_REGISTER_ULONG(Cache1Dma0Reg) > 0); // wait for idle status

    //**************************************************************************
    // Make sure that the FIFO is really empty.
    //**************************************************************************
    Cache1StatusReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_STATUS);
    while ((READ_REGISTER_ULONG(Cache1StatusReg) & 0x10) != 0x10);

    //**************************************************************************
    // Wait for the graphics engine to complete the last command
    //**************************************************************************
    GrStatusReg = ppdev->GrStatusReg;
    while (READ_REGISTER_ULONG(GrStatusReg) & (NV_PGRAPH_STATUS_STATE_BUSY | 0x80000000));

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "NV3_WaitForFifoAndEngineIdle: Exit"));
#endif
    }

//******************************************************************************
//
// Function: NV3_WaitForChannelSwitch()
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


VOID NV3_WaitForChannelSwitch(PDEV*   ppdev)

    {
    Nv3ChannelPio  *nv  = (Nv3ChannelPio*) ppdev->pjMmBase;
    USHORT         FreeCount;
    NvNotification *pSyncNotifier = (NvNotification *)ppdev->Notifiers->Sync;

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "WaitForChannelSwitch: Entry"));
#endif

    if (!nv) return;

    NV3_WaitForFifoAndEngineIdle(ppdev);

    //**************************************************************************
    // Here, we make sure the Resource Manager has completed the switch from
    // Channel 1 (DMA Pusher/OpenGL) to Channel 0 (PIO)
    //**************************************************************************

    //**************************************************************************
    // Send dummy methods down the FIFO
    // If Channel 1 was previously active (OpenGL/ICD), writing this
    // method will generate a channel exception in the resource manager,
    // causing it to switch to channel 0.
    //**************************************************************************

    if (nv)
        {
#ifdef DEBUG_MSG_CHANNEL_WAIT
        DISPDBG((2, "WaitForChannelSwitch: Punch PIO channel"));
#endif

        FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.Free ;
        while (FreeCount < NV_GUARANTEED_FIFO_SIZE)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.Free ;

        //***********************************************************************
        // Ensure graphics engine has completed all processing by waiting for
        // method notification.
        //***********************************************************************
        pSyncNotifier->status = NV_IN_PROGRESS;
        nv->subchannel[DD_ROP_RECT_AND_TEXT].nvRenderGdi0RectangleAndText.SetNotify = 0;
        nv->subchannel[DD_ROP_RECT_AND_TEXT].nvRenderGdi0RectangleAndText.Color1A = 0;
        while ((volatile)pSyncNotifier->status == NV_IN_PROGRESS); // wait for notification
        ppdev->NVFreeCount = NvGetFreeCount(nv, DD_ROP_RECT_AND_TEXT);

#ifdef DEBUG_MSG_CHANNEL_WAIT
        DISPDBG((2, "WaitForChannelSwitch: Punch PIO channel complete"));
#endif

        }

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "WaitForChannelSwitch: Exit"));
#endif
    }



//******************************************************************************
//
// Function: NV3_GraphicsEngineBusy()
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


ULONG NV3_GraphicsEngineBusy(PDEV*   ppdev)

    {
    Nv3ChannelPio *nv  = (Nv3ChannelPio*) ppdev->pjMmBase;
    USHORT FreeCount;
    BOOL   bReleaseOpenGLSemaphore = FALSE;

    if (!nv) return(FALSE);

    //**************************************************************************
    // If OpenGL is enabled, then wait for access to PIO FIFO and Semaphore lock
    // out any other processes.
    //**************************************************************************

    if (OglIsEnabled(ppdev))
        {
        ppdev->pfnAcquireOglMutex(ppdev);
        bReleaseOpenGLSemaphore = TRUE;

        ppdev->NVFreeCount = 0;

        //**********************************************************************
        // We need to make absolutely sure that the Channel switch
        // from the DMA pusher to the PIO channel has been completed
        //**********************************************************************

        ppdev->pfnWaitForChannelSwitch(ppdev);
        }

    //**************************************************************************
    // Init the cached free count
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Here, we use a notifier to make sure the engine is no longer busy.
    // We'll just send a NO-OP (using the RECT_AND_TEXT object) and
    // wait for the notifier to complete.
    //**************************************************************************

    while (FreeCount < 2*4)
         FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
    FreeCount -= 2*4;

    ((NvNotification *) (ppdev->Notifiers->Sync))->status = NV_STATUS_IN_PROGRESS;

    //**************************************************************************
    // Send the NO-OP on thru
    //**************************************************************************

    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.SetNotify = 0;
    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.Color1A = 0;

    //**************************************************************************
    // Wait for completion...
    //**************************************************************************
    while ((volatile)((NvNotification *) ppdev->Notifiers->Sync)->status == NV_IN_PROGRESS); // wait for notification

    //**************************************************************************
    // Make sure to update the free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    if (bReleaseOpenGLSemaphore == TRUE)
        {
        ppdev->pfnReleaseOglMutex(ppdev);

        bReleaseOpenGLSemaphore = FALSE;
        }

    return(FALSE);
    }


//******************************************************************************
//
// Function: NV3_WaitWhileVBlankActive()
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


VOID NV3_WaitWhileVBlankActive(PDEV*   ppdev)

    {
    while (NV3_VBlankIsActive(ppdev))
        ;
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


VOID NV3_WaitWhileDisplayActive(PDEV*   ppdev)

    {
    while (NV3_DisplayIsActive(ppdev))
        ;
    }


//******************************************************************************
//
// Function: NV3_WaitWhileGraphicsEngineBusy()
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


VOID NV3_WaitWhileGraphicsEngineBusy(PDEV*   ppdev)

    {
    while (NV3_GraphicsEngineBusy(ppdev))
        ;
    }

//******************************************************************************
//
// Function: NV3_FlipBuffer()
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


VOID NV3_FlipBuffer(PDEV*   ppdev, ULONG MemoryOffset)

    {
    volatile UCHAR *PRMCIO_CRX_COLOR_Reg;
    volatile UCHAR *PRMCIO_CR_COLOR_Reg;
    volatile UCHAR *PRMVIO_SRX_Reg;
    volatile UCHAR *PRMVIO_SR_LOCK_Reg;
    volatile UCHAR *PRMVIO_MISC_READ_Reg;
    UCHAR i;
    UCHAR lock;

    //**************************************************************************
    // Get pointers to CRTC registers
    //
    //  ???? Should be BYTE writes or DWORDS writes ????
    //  ???? Should be BYTE? but CRTC_WR macro outputs in DWORDS???
    //
    //  #define CRTC_WR(i,d)    {PRMCIO_REG_WR32(PRMCIO_Base,NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
    //                        PRMCIO_REG_WR32(PRMCIO_Base,NV_PRMCIO_CR__COLOR, (d));}
    //  #define CRTC_RD(i,d)    {PRMCIO_REG_WR32(PRMCIO_Base,NV_PRMCIO_CRX__COLOR, (i) & 0x3F);\
    //                        (d) = PRMCIO_REG_RD32(PRMCIO_Base,NV_PRMCIO_CR__COLOR);}
    //**************************************************************************

    PRMCIO_CRX_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
    PRMCIO_CRX_COLOR_Reg += OFFSET_PRMCIO_CRX__COLOR_REG;

    PRMCIO_CR_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
    PRMCIO_CR_COLOR_Reg += OFFSET_PRMCIO_CR__COLOR_REG;

    //**************************************************************************
    // Init access to extended registers
    //**************************************************************************

    PRMVIO_SRX_Reg = (volatile UCHAR *)ppdev->PRMVIORegs;
    PRMVIO_SRX_Reg += OFFSET_PRMVIO_SRX_REG;

    PRMVIO_SR_LOCK_Reg = (volatile UCHAR *)ppdev->PRMVIORegs;
    PRMVIO_SR_LOCK_Reg += OFFSET_PRMVIO_SR_LOCK_REG;

    PRMVIO_MISC_READ_Reg = (volatile UCHAR *)ppdev->PRMVIORegs;
    PRMVIO_MISC_READ_Reg += OFFSET_PRMVIO_MISC_READ_REG;

    //**************************************************************************
    // Set new frame buffer address (21-bit address)
    //
    //      NV_CIO_CR_SA_HI_INDEX
    //
    //          Bits [7:0]  -> Bits [15:8] of 21-bit display buffer address
    //
    //      NV_CIO_CR_SA_LO_INDEX
    //
    //          Bits [7:0]  -> Bits [7:0] of 21-bit display buffer address
    //
    //      NV_CIO_CRE_RPC0_INDEX
    //
    //          Bits[4:0]   -> Bits [20:16] of 21-bit display buffer address
    //          Bits[7:5]   -> Bits [10:8] of the CRTC offset register
    //
    // ?? Why bother with 0x3f ??
    //**************************************************************************

    //**************************************************************************
    // Need to shift down memory offset by 2 to account for granularity
    //**************************************************************************

    MemoryOffset>>=2;

    *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CR_SA_LO_INDEX & 0x3f);
    *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)(MemoryOffset & 0xff);

    *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CR_SA_HI_INDEX & 0x3f);
    *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)((MemoryOffset >> 8) & 0xff);

    //**************************************************************************
    // Unlock CRTC extended regs
    //
    // NV_PRMVIO_SRX            = Sequencer Index Register (3c4)
    // NV_PRMVIO_SR_LOCK        = Data is written/ readm from register (3c5)
    // NV_PRMVIO_SR_LOCK_INDEX  = This indexed register (5) locks/unlocks
    //                            all extended registers.  When written with
    //                            value of 57, all extended registers are UNLOCKED.
    //                            Otherwise, all extended registers are LOCKED.
    //
    //                            When value = 0, extended register are in a locked state
    //                            When value = 1, extended registers are in an unlocked state
    //
    //**************************************************************************

    *PRMVIO_SRX_Reg = NV_PRMVIO_SR_LOCK_INDEX;
    lock = *PRMVIO_SR_LOCK_Reg;
    *PRMVIO_SR_LOCK_Reg = NV_SR_UNLOCK_VALUE;

    //**************************************************************************
    // Fix high address bit in extended CRTC reg
    //
    //      NV_CIO_CRE_RPC0_INDEX
    //
    //          Bits[4:0]   -> Bits [20:16] of 21-bit display buffer address
    //
    //          We preserve bits [7:5]   (i & 0xe0) and
    //          just modify the botom 5 bits (0x1f)
    //
    //**************************************************************************

    *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_RPC0_INDEX & 0x3f);
    i = *PRMCIO_CR_COLOR_Reg;       // Get current value

    *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_RPC0_INDEX & 0x3f);
    *PRMCIO_CR_COLOR_Reg = (i & 0xe0) | (volatile UCHAR)((MemoryOffset >> 16) & 0x1f) ;

    //**************************************************************************
    // Lockup CRTC extended regs
    //**************************************************************************

    *PRMVIO_SRX_Reg = NV_PRMVIO_SR_LOCK_INDEX;
    if (lock == 0)
        *PRMVIO_SR_LOCK_Reg = NV_SR_LOCK_VALUE;

    }

//******************************************************************************
//
// Function: NV3_DmaPushGo()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     chID - Pointer to channel ID for resource manager
//     tlbPtBase - Base of context DMA
//     busAddressSpace - PCI, AGP or other memory base for context DMA
//     getOffset - Offset from start of context DMA where push data starts
//     putOffset - Offset from start of context DMA where push data ends
//
// Return Value:
//
//     TRUE means it kicked off FALSE means it did not.
//
//******************************************************************************

#if 0
#undef WRITE_REGISTER_ULONG
#define WRITE_REGISTER_ULONG NV3_WRITE_REGISTER_ULONG
NV3_WRITE_REGISTER_ULONG(ULONG *addr, ULONG val)
{
ULONG tmp;
tmp = *addr;
*addr = val;
tmp = *addr;
_asm lock or tmp,0x5f5f5f5f
}
#endif


ULONG NV3_DmaPushGo(PDEV* ppdev, ULONG chID, ULONG tlbPtBase, ULONG busAddressSpace, ULONG getOffset, ULONG putOffset, ULONG hClient, ULONG hDevice)

    {
    volatile ULONG *CachesReg;
    volatile ULONG *Cache1Pull0Reg;
    volatile ULONG *Cache1Dma0Reg;
    volatile ULONG *Cache1Push0Reg;
    volatile ULONG *Cache1Push1Reg;
    volatile ULONG *Config0Reg;
    volatile ULONG *cache1DmaTlbPtBaseReg;
    volatile ULONG *cache1DmaTlbTagReg;
    volatile ULONG *cache1DmaTlbPteReg;
    volatile ULONG *cache1Dma3Reg;
    volatile ULONG *cache1Dma2Reg;
    volatile ULONG *cache1Dma1Reg;
    volatile ULONG *cache1Dma0Reg;
    volatile ULONG *cache1DmaStatusReg;
    ULONG status, numDmaBytes;
    extern  ULONG RmFifoFlushContext(PDEV *ppdev, ULONG chID, ULONG hDevice, ULONG hClient);

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "DmaPushGo: Entry"));
#endif

    if (!ppdev)
    {
#ifdef DEBUG_MSG_CHANNEL_WAIT
        DISPDBG((2, "DmaPushGo: NULL pdev Exit"));
#endif
        return (ULONG)FALSE;
    }

    if (!(ppdev->pjMmBase))
    {
#ifdef DEBUG_MSG_CHANNEL_WAIT
        DISPDBG((2, "DmaPushGo: NULL pdev->pjMmBase Exit"));
#endif
        return (ULONG)FALSE;
    }

    if (ppdev->bEnabled == FALSE)
    {
#ifdef DEBUG_MSG_CHANNEL_WAIT
        DISPDBG((2, "DmaPushGo: pdev NOT ENABLED Exit"));
#endif
        return (ULONG)FALSE;
    }

    numDmaBytes = (long)putOffset - (long)getOffset;
    if (numDmaBytes > 0)
        {
        ppdev->pfnAcquireOglMutex(ppdev);

        NV3_WaitForFifoAndEngineIdle(ppdev);

        CachesReg           = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHES);
        Cache1Pull0Reg      = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PULL0);
        Cache1Dma0Reg       = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA0_REG);
        Cache1Push0Reg      = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PUSH0_REG);
        Config0Reg          = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CONFIG_0);
        Cache1Push1Reg      = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PUSH1);
        cache1DmaTlbTagReg  = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_TLB_TAG);
        cache1DmaTlbPteReg  = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_TLB_PTE);
        cache1DmaStatusReg  = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_STATUS);
        cache1Dma3Reg       = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA3);
        cache1Dma2Reg       = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA2);
        cache1Dma1Reg       = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA1);
        cache1Dma0Reg       = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA0);

        WRITE_REGISTER_ULONG(CachesReg, 0);
        WRITE_REGISTER_ULONG(Cache1Pull0Reg, 0);
        WRITE_REGISTER_ULONG(Cache1Push0Reg, 0);
        WRITE_REGISTER_ULONG(Cache1Dma0Reg, 0);

        if (READ_REGISTER_ULONG(Cache1Push1Reg) != chID)
            {
            status = RmFifoFlushContext(ppdev, chID, hClient, hDevice);
            if (status)
                {
                WRITE_REGISTER_ULONG(Cache1Pull0Reg, 1);
                WRITE_REGISTER_ULONG(Cache1Push0Reg, 1);
                WRITE_REGISTER_ULONG(CachesReg, 1);

                ppdev->pfnReleaseOglMutex(ppdev);

#ifdef DEBUG_MSG_CHANNEL_WAIT
                DISPDBG((2, "DmaPushGo: RmFifoFlushContext ERROR: Exit"));
#endif
                return (ULONG)FALSE;
                }
            }

        WRITE_REGISTER_ULONG(Cache1Push1Reg, chID);

        WRITE_REGISTER_ULONG(Config0Reg, (3 << 8) | (0x1F << 18));

        cache1DmaTlbPtBaseReg = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_TLB_PT_BASE);
        WRITE_REGISTER_ULONG(cache1DmaTlbPtBaseReg, tlbPtBase);

        WRITE_REGISTER_ULONG(cache1DmaTlbTagReg, 0xFFFFFFFF);

        WRITE_REGISTER_ULONG(cache1DmaTlbPteReg, 1);

        WRITE_REGISTER_ULONG(cache1Dma3Reg, busAddressSpace);

        WRITE_REGISTER_ULONG(cache1DmaStatusReg, 0);

        WRITE_REGISTER_ULONG(cache1Dma2Reg, getOffset);

        WRITE_REGISTER_ULONG(cache1Dma1Reg, numDmaBytes);

        WRITE_REGISTER_ULONG(Cache1Pull0Reg, 1);
        WRITE_REGISTER_ULONG(Cache1Push0Reg, 1);
        WRITE_REGISTER_ULONG(CachesReg, 1);

        WRITE_REGISTER_ULONG(Cache1Dma0Reg, 1);

        ppdev->pfnReleaseOglMutex(ppdev);
        }

#ifdef DEBUG_MSG_CHANNEL_WAIT
    DISPDBG((2, "DmaPushGo: Exit"));
#endif

    return (ULONG)TRUE;
}

//******************************************************************************
//
// Function: NV3_RmFifoFlushContext
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     chID - Pointer to channel ID for resource manager
//
// Return Value:
//
//     None.
//
//******************************************************************************


ULONG RmFifoFlushContext(PDEV *ppdev, ULONG chID, ULONG hClient, ULONG hDevice)
{
    DWORD   rmInfo[3];
    DWORD   *rmInfoPtr;

    rmInfo[0] = chID;
    rmInfo[1] = hClient;
    rmInfo[2] = hDevice;
    rmInfoPtr = &rmInfo[0];

// IA64 - NvDmaFlowControl needs to be modified so that the put/get can be
//        64 bit pointers (if we really want NV3 to work under IA64)

#ifndef _WIN64
    if (NvDmaFlowControl(ppdev->hDriver, (ULONG)0xFFFFFFFF, (ULONG)0, (ULONG)&rmInfoPtr, (ULONG)&rmInfoPtr))
        {
        DISPDBG((2, "RmFifoFlushContext FAILED!"));
        return(TRUE);
        }
#endif

    return(FALSE);
}

//******************************************************************************
//
// Function: NV3_AcquireOglMutex
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

VOID NV3_AcquireOglMutex(PDEV *ppdev)
{
    if (0 == ppdev->bReleaseOglMutex)
        {
        EngAcquireSemaphore(ppdev->csFifo);
        }
    ppdev->bReleaseOglMutex++;
}

//******************************************************************************
//
// Function: NV3_ReleaseOglMutex
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

VOID NV3_ReleaseOglMutex(PDEV *ppdev)
{
    if (1 == ppdev->bReleaseOglMutex)
        {
        EngReleaseSemaphore(ppdev->csFifo);
        }
    ppdev->bReleaseOglMutex--;
}
#endif // NV3
