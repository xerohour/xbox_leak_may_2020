//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     nv4subch.h
//
// Abstract:
//
//     This module contains the object subchannel definitions
//
// Environment:
//
//     Kernel mode
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
// ddraw SubChannel assignments for objects
//******************************************************************************
#define NV_DD_ROP                      0
#define NV_DD_BLIT                     1
#define NV_DD_RECT_AND_TEXT            2
#define NV_DD_PRIMARY                  3
#define NV_DD_IMAGE                    4
#define NV_DD_PATTERN                  5
#define NV_DD_CLIP                     6
#define NV_DD_STRETCH                  7
#define NV_DD_SPARE                    7

/* Default cached objects in Windows 95 Direct Draw patch */
#define dDrawRop                       subchannel[NV_DD_ROP].nv3ContextRop
#define dDrawImageInMemory             subchannel[NV_DD_SPARE].nvImageInMemory
#define dDrawRopRectAndText            subchannel[NV_DD_RECT_AND_TEXT].nvRenderGdi0RectangleAndText
#define dDrawBlit                      subchannel[NV_DD_BLIT].nv4ImageBlit
#define dDrawImageFromCpu              subchannel[NV_DD_IMAGE].nv4ImageFromCpu
#define dDrawStretch                   subchannel[NV_DD_STRETCH]
#define dDrawTransColor                subchannel[NV_DD_SPARE].nv4ContextColorKey
#define dDrawSpare                     subchannel[NV_DD_SPARE]

#define DD_CHANNEL      1                   // DDRAW always in channel 1
#define GDI_CHANNEL     0                   // GDI always in channel 0
#define DD_CHANNEL_MASK (1 << DD_CHANNEL)   // Mask out ddraw channel

//********************************************************************
// #defines for NV4 fifo sync registers.
//********************************************************************
#define OFFSET_NV_PFIFO_BASE            0x2000
#define NV4_PFIFO_DMA                   0x2508-OFFSET_NV_PFIFO_BASE
#define NV4_PFIFO_CACHE1_PUSH1          0x3204-OFFSET_NV_PFIFO_BASE
#define NV4_PFIFO_CACHE1_DMA_PUSH       0x3220-OFFSET_NV_PFIFO_BASE
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT  0x00000100
#define CACHE1_DMA_PUSH_BUFFER_EMPTY    0x100L

//********************************************************************
// This macro is used to ensure that all DMA channel processing has
// completed prior to performing PIO writes.
//********************************************************************
#define NV_DD_DMA_PUSHER_SYNC()                                 \
{                                                               \
if (!ppdev->bDDChannelActive)                                   \
    {                                                           \
    volatile DWORD *npNvBase = (DWORD *)ppdev->PFIFORegs;       \
    volatile DWORD  regValue;                                   \
    DWORD chid;                                                 \
                                                                \
    while ((npNvBase[(NV4_PFIFO_DMA >> 2)] &                    \
            (~DD_CHANNEL_MASK)) != 0);                          \
    if ((npNvBase[NV4_PFIFO_CACHE1_PUSH1 >> 2] & NV_PFIFO_CACHE1_PUSH1_MODE_DMA_BIT) &&         \
        ((chid = (npNvBase[NV4_PFIFO_CACHE1_PUSH1 >> 2] & 0x1F)) !=      \
            (WORD)DD_CHANNEL))                                  \
        {                                                       \
            regValue = npNvBase[(NV4_PFIFO_CACHE1_DMA_PUSH >> 2)]; \
            while ((regValue & CACHE1_DMA_PUSH_BUFFER_EMPTY) == 0) { \
                regValue = npNvBase[(NV4_PFIFO_CACHE1_DMA_PUSH >> 2)]; \
            }                                                   \
            if (chid == GDI_CHANNEL)                            \
                ppdev->pfnWaitEngineBusy(ppdev);                \
        }                                                       \
        ppdev->bDDChannelActive = TRUE;                         \
        ppdev->oglLastChannel = DD_CHANNEL;                     \
    }                                                           \
}

