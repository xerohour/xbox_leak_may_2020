/******************************Module*Header*******************************\
* Module Name: oglkick.c
*
* This module contains the functions to support the OpenGL kick off.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

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
#include "oglDD.h"

#define NV_JUMP(addr)  (0x20000000 | (addr))

#define DBG_KICKOFF_MESSAGES 1
#undef DBG_KICKOFF_MESSAGES

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

// ripped off from nv4ddrw.c
#define OFFSET_NV_PFIFO_BASE            (0x2000)
#define OFFSET_NV_PFIFO_DMA_REG         (0x2508-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_PUSH1    (0x3204-OFFSET_NV_PFIFO_BASE)
#define OFFSET_NV_PFIFO_CACHE1_DMA_PUSH (0x3220-OFFSET_NV_PFIFO_BASE)
#define CACHE1_PUSH1_MODE_DMA           0x100
#define CACHE1_DMA_PUSH_BUFFER_EMPTY    0x100

extern VOID MagicDelay(void);

//******************************************************************************
//
//  Function: OglChannelWait
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************
void OglChannelWait(
    PDEV  *ppdev,
    ULONG ourChannelId,
    ULONG maskOtherChannels
    )
{
    /*
    volatile ULONG  *pfifoDmaReg;
    volatile ULONG  *pfifoCache1Push1;
    volatile ULONG  *pfifoCache1DmaPush;
    volatile ULONG  regValue;

    pfifoDmaReg        = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_DMA_REG);
    pfifoCache1Push1   = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_PUSH1);
    pfifoCache1DmaPush = (ULONG *)((BYTE *)ppdev->PFIFORegs + OFFSET_NV_PFIFO_CACHE1_DMA_PUSH);

    if (ppdev->oglLastChannel != ourChannelId)
        {
        regValue = READ_REGISTER_ULONG(pfifoDmaReg);
        // wait for all other channels to go to 0 (not active)
        while ((regValue & maskOtherChannels) != 0) 
            {
            (VOID)MagicDelay();
            regValue = READ_REGISTER_ULONG(pfifoDmaReg);
            }

        regValue = READ_REGISTER_ULONG(pfifoCache1Push1);
        if (((regValue & CACHE1_PUSH1_MODE_DMA) != 0) &&
            ((regValue & (0x1F)) != ourChannelId)) 
            {
            regValue = READ_REGISTER_ULONG(pfifoCache1DmaPush);
            while ((regValue & CACHE1_DMA_PUSH_BUFFER_EMPTY) == 0) 
                {
                (VOID)MagicDelay();
                regValue = READ_REGISTER_ULONG(pfifoCache1DmaPush);
                }
            }
        }
    */
    

    if (ppdev->oglLastChannel != ourChannelId)
    {
        ppdev->oglLastChannel = -1;             // FORCE IT TO SYNC...
        ppdev->pfnWaitForChannelSwitch(ppdev);
        ppdev->oglLastChannel = ourChannelId;
    }
    
}
