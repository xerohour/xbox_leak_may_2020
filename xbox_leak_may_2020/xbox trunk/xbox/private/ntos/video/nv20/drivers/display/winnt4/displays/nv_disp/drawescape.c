//*****************************Module*Header******************************
//
// Module Name: drawescape.c
//
// Supports the API DrvDrawEscape entry
//
// FNicklisch 14.09.00: New: added from ELSA code
//   Added this to implement the roller tool support. I use this entry because 
//   the code and the tool were already available and it was the fastest
//   way to get it running. 
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

// import header files
#include "precomp.h"
#include "driver.h"
#include "nv32.h"

#include "dmamacro.h"
#include "nv32.h"
#include "nvsubch.h"

#ifdef IA64
    //
    // Disable the scroll interface for IA64 as we don't have the tool and
    // I don't know how dwParam in the DCICOMMAND is defined
    //
    #undef ROLLER_TOOL_SUPPORT
#else
    #include <Dciddi.h>
#endif


// export header file
#include "drawescape.h"

//*****************************************************************************
// Externs
//*****************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );

//
// 
//
static BOOL ulRollerToolEscape(PPDEV ppdev, VOID* pv);


//
// Driver Entry point
//
ULONG APIENTRY DrvDrawEscape (
    SURFOBJ *pso,
    ULONG    iEsc,
    CLIPOBJ *pco,
    RECTL   *prcl,
    ULONG    cjIn,
    PVOID    pvIn)
{
    ULONG ulRet = 0; // unsuported escape
    PPDEV ppdev = (PPDEV)pso->dhpdev;

    prcl = prcl; // unreferenced formal parameter
    pco  = pco;  // unreferenced formal parameter

    switch(iEsc)
    {
        #ifdef ROLLER_TOOL_SUPPORT
        case DCICOMMAND:
        {
            DCICMD *pDciCmd = (DCICMD*)pvIn;

            if (   (cjIn < sizeof(DCICMD)) 
                || (pDciCmd->dwVersion != DCI_VERSION) )
            {
                ulRet = (ULONG)-1; // error
                break;
            }

            switch (pDciCmd->dwCommand)
            {
            case 0xEEEE0000: // EDDESCAPE==ELSA defined service escape
                ulRet = ulRollerToolEscape(ppdev, (PVOID)pDciCmd->dwParam1);
                break;
            }
            break;
        }
        #endif // ROLLER_TOOL_SUPPORT
    }

    return ulRet;
}


#ifdef ROLLER_TOOL_SUPPORT
//
// Downstripped version of the ELSA DrawEscape support function. 
// The remaining code supports the roller tool.
//

//
// ELSA specific command group
//
#define EDDESC_SCROLL               (0xEDD00000 | 0x00000012)

//
// scrolling into offscreen (debug purposes)
//
typedef struct _EDDESC_SCROLL_DATA
{
  DWORD dwSize;             // = sizeof(struct tagSCROLL_DATA)
  DWORD dwCommand;          // = EDDESC_SCROLL
  LONG  cyLines;            // # of lines vertical panning
} EDDESC_SCROLL_DATA;


//
// ulRollerToolEscape was spawned from a more complex
// escape interface, but only the EDDESC_SCROLL is supported
// in the nVidia code.
//
static BOOL ulRollerToolEscape(PPDEV ppdev, VOID* pv)
{
    BOOL                bRet    = FALSE; // unsupported escape==default error exit
    EDDESC_SCROLL_DATA *pScroll = pv;

    if (   (NULL != pScroll)
        && (NULL != ppdev)
        && (EDDESC_SCROLL==pScroll->dwCommand)
        && (pScroll->dwSize == sizeof(EDDESC_SCROLL_DATA)) )
    {
        static LONG ActualLine = 0; // remember this as last address!

        // CSchalle 12/3/98 2:43:28 PM: want to see instance memory, too
        ULONG cbTotalRAM = (ppdev->cbFrameBuf + (4 *1024*1024 - 1)) & ~(4 *1024 *1024 - 1);
        LONG  lLastLine  = cbTotalRAM / ppdev->lDelta;
        ULONG i=0;
        ULONG ulMaxDacs;

        DECLARE_DMA_FIFO;

        INIT_LOCAL_DMA_FIFO;


        ActualLine += pScroll->cyLines;

        // stop scrolling up at top
        if (ActualLine < 0)  
            ActualLine = 0;

        // stop scrolling down at bottom - screem height
        if (ActualLine + ppdev->cyScreen > lLastLine) 
            ActualLine = lLastLine - ppdev->cyScreen;

        if (ppdev->ulDesktopMode & NV_CLONE_DESKTOP)
        {
            // In clone mode only do it for first monitor by now as
            // it seems to be more convenient for debugging

            ulMaxDacs = 1;
        }
        else
        {
            // In a multimon environment scroll all dacs
            ulMaxDacs = ppdev->ulNumberDacsActive;
        }

        //doesn't work for multiple dacs...    for (i = 0; i < ulMaxDacs; i++)
        {
            ULONG ulHead;
            ulHead = ppdev->ulDeviceDisplay[i];

            // ELSA method of setting the display start address 
            // replaced by code taken from OglRestoreDacs.

            NV_DMAPUSH_CHECKFREE( ((ULONG)(10)));
            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,
                NV_VIDEO_LUT_CURSOR_DAC + ulHead);

            ASSERT(0==i); // notification fails for multiple dacs, don't know why!
            while (((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)* sizeof(NvNotification)])))->status == NV_IN_PROGRESS)
            {
                ;
            }

            ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*
                                   sizeof(NvNotification)])))->status = NV_IN_PROGRESS;

            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_OFFSET(0), 
                ActualLine * ppdev->lDelta);
            NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_FORMAT(0), 
                ppdev->lDelta | NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31);
        }
        // Restore to 1st DAC
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0), NV_VIDEO_LUT_CURSOR_DAC);

        UPDATE_PDEV_DMA_COUNT;

        //******************************************************************
        // Send data on thru to the DMA push buffer
        //******************************************************************

        NV4_DmaPushSend(ppdev);

        bRet = TRUE;
    }

    return bRet;
}

// End of drawescape.c
#endif //ROLLER_TOOL_SUPPORT
