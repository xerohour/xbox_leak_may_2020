#ifdef  NV4
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4RENDR.C                                                        *
*   HW Specivic D3D Rendereing routines.                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman               01/31/98 - NV4 development.             *
*                                                                           *
\***************************************************************************/
#include <math.h>
#include <windows.h>
#include <conio.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"

#define PUSH_THRESHOLD (DWORD)(1024)

/*
 * DMA kickoff routines.
 */
#define sizeDmaBufferPad 0

DWORD   dbgLastGetAddr;

DWORD __stdcall nvGetDmaFreeCount
(
    long sizeRequired
)
{
    Nv4ControlDma *dmaChannel         = (Nv4ControlDma *)pDriverData->NvDevFlatDma;
    DWORD          getOffset;
    DWORD          putOffset;
    long           freeCount;
    BOOL           bWait;

    /*
     * aa capture override - allocate more space
     */
    if ((pCurrentContext)
     && (pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER))
    {
        PNVAACONTEXT pCtx = pCurrentContext->pAAContext;
        DWORD        dif  = ((sizeRequired << 2) + 16383) & ~16383;
        pCtx->dwCapIndex  = nvFifo - pCtx->dwCapAddr;
        pCtx->dwCapSize  += dif;
        pCtx->dwCapAddr   = (DWORD)ReallocIPM((void*)pCtx->dwCapAddr,pCtx->dwCapSize);
        nvFifo            = pCtx->dwCapAddr + pCtx->dwCapIndex;
        return (pCtx->dwCapSize - pCtx->dwCapIndex) >> 2;
    }

#ifdef NV_PROFILE
    /*
     * FreeCount Wait Log
     */
    NVP_START(NVP_T_FREECOUNT);
#endif

    bWait = FALSE;
    for (;;) {
        getOffset   = (DWORD)(volatile)dmaChannel->Get;
#ifdef DEBUG
        if (getOffset > nvDmaPushOffsetMax)
        {
            DPF_LEVEL(NVDBG_LEVEL_ERROR, "Invalid Get Offset!!");
            dbgD3DError();
        }
        dbgLastGetAddr = getOffset;
#endif
        putOffset = (DWORD)nvFifo - (DWORD)nvDmaPushBase;

        if (putOffset >= getOffset)
        /*
         * |       G......P     |
         */
        {
            freeCount = (long)(nvDmaPushOffsetMax - putOffset) >> 2;
        // deal with wrapping around
            if ((freeCount <= sizeRequired) && getOffset)
            {
                *((volatile DWORD *)nvFifo) = (0x20000000 | (0));
#if 1
                /*
                 * Don't want this, but it's currently the only known way to
                 * guarantee a write combine flush.
                 */
                _outp (0x3d0,0);
#endif

#ifdef NV_NULL_HW_DRIVER
                if (nvDmaLastPutAddr) ((DWORD*)nvDmaLastPutAddr)[0] = 0x20000000 | 0;
#endif //NV_NULL_HW_DRIVER

                *((volatile DWORD *)&pDriverData->NvDmaPusherBufferEnd[0]) = 0;

                /*
                 * Multiple push buffer sync up.
                 */
                WAIT_FOR_VSYNC(pCurrentContext);
                NV_DD_DMA_PUSHER_SYNC();

                /*
                 * push buffer disassembler
                 */
#ifdef NV_DISASM
                {
                    extern HANDLE nvPusherFile;
                    extern BOOL   nvPusherDisassemblerEnable;
                    if (nvPusherFile && nvPusherDisassemblerEnable)
                    {
                        DWORD dw;
                        WriteFile (nvPusherFile,(void*)nvDmaLastPutAddr,(DWORD)nvFifo-(DWORD)nvDmaLastPutAddr+4,&dw,0);
                        FlushFileBuffers (nvPusherFile);
                    }
                }
#endif
                (volatile)dmaChannel->Put = putOffset = 0;
                nvDmaLastPutAddr = nvFifo = (DWORD)nvDmaPushBase;
                DPF_LEVEL(NVDBG_LEVEL_INFO2, "Last Put = 0x%08lx",nvDmaLastPutAddr);

                bWait = FALSE;
            }
        }
        else
        /*
         * |.......P      G.....|
         */
        {
            freeCount = ((getOffset - putOffset) >> 2) - 1;
        }

        /*
         * Maximize freeCount
         */
        if (((DWORD)nvFifo - (DWORD)nvDmaLastPutAddr) >= PUSH_THRESHOLD)
        {
            *((volatile DWORD *)&pDriverData->NvDmaPusherBufferEnd[0]) = putOffset;
#if 1
                /*
                 * Don't want this, but it's currently the only known way to
                 * guarantee a write combine flush.
                 */
                _outp (0x3d0,0);
#endif

            /*
             * Multiple push buffer sync up.
             */
            WAIT_FOR_VSYNC(pCurrentContext);
            NV_DD_DMA_PUSHER_SYNC();

#ifdef NV_NULL_HW_DRIVER
            if (nvDmaLastPutAddr) ((DWORD*)nvDmaLastPutAddr)[0] = 0x20000000 | putOffset;
#endif //NV_NULL_HW_DRIVER

            /*
             * push buffer disassembler
             */
#ifdef NV_DISASM
            {
                extern HANDLE nvPusherFile;
                extern BOOL   nvPusherDisassemblerEnable;
                if (nvPusherFile && nvPusherDisassemblerEnable)
                {
                    DWORD dw;
                    WriteFile (nvPusherFile,(void*)nvDmaLastPutAddr,(DWORD)nvFifo-(DWORD)nvDmaLastPutAddr,&dw,0);
                    FlushFileBuffers (nvPusherFile);
                }
            }
#endif
            (volatile)dmaChannel->Put = putOffset;
            nvDmaLastPutAddr = nvFifo;
            DPF_LEVEL(NVDBG_LEVEL_INFO2, "Last Put = 0x%08lx",nvDmaLastPutAddr);
        }

        if (freeCount > sizeRequired) break;

        if (bWait)
        {
            NV_DELAY;
        }
        bWait = TRUE;
    }

#ifdef NV_PROFILE
    /*
     * FreeCount Wait Log
     */
    NVP_STOP(NVP_T_FREECOUNT);
    nvpLogTime (NVP_T_FREECOUNT,nvpTime[NVP_T_FREECOUNT]);
#endif

    return freeCount;
}

void nvStartDmaBuffer
(
    BOOL bOptional
)
{
    DWORD           dwCurrentOffset;
    Nv4ControlDma  *nvDmaControl;

    /*
     * aa capture override - ignore kickoff
     */
    if (pCurrentContext)
        if (pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER)
            return;

    /*
     * Is it worth sending stuff now?
     */
    if (bOptional)
    {
        if (((DWORD)nvFifo-(DWORD)nvDmaLastPutAddr) < PUSH_THRESHOLD) return;
    }
//    if (nvDmaLastPutAddr == nvFifo)
//        return;

    /*
     * Update the ending dma offset.
     */
    if ((!pDriverData->NvDevFlatDma)
     || (pDriverData->fFullScreenDosOccurred))
        return;

    nvDmaControl = (Nv4ControlDma *)pDriverData->NvDevFlatDma;

    dwCurrentOffset = nvFifo - nvDmaPushBase;

#ifdef NV_NULL_HW_DRIVER
    if (nvDmaLastPutAddr) ((DWORD*)nvDmaLastPutAddr)[0] = 0x20000000 | dwCurrentOffset;
#endif //NV_NULL_HW_DRIVER

    dwCurrentOffset = nvFifo - nvDmaPushBase;
    *((volatile DWORD *)&pDriverData->NvDmaPusherBufferEnd[0]) = dwCurrentOffset;
#if 1
                /*
                 * Don't want this, but it's currently the only known way to
                 * guarantee a write combine flush.
                 */
                _outp (0x3d0,0);
#endif

    /*
     * Multiple push buffer sync up.
     */
    WAIT_FOR_VSYNC(pCurrentContext);
    NV_DD_DMA_PUSHER_SYNC();

#ifdef NV_PROFILE
    nvpLogEvent (NVP_E_PUT);
    {
        DWORD put = nvDmaLastPutAddr - nvDmaPushBase;
        DWORD get = (volatile)nvDmaControl->Get;

        if (put >= get)
        {
            nvpLogCount (NVP_C_PENDING,put-get);
        }
        else
        {
            get = (nvDmaPushOffsetMax - get) + put;
            nvpLogCount (NVP_C_PENDING,get);
        }
    }
#endif

    /*
     * push buffer disassembler
     */
#ifdef NV_DISASM
    {
        extern HANDLE nvPusherFile;
        extern BOOL   nvPusherDisassemblerEnable;
        if (nvPusherFile && nvPusherDisassemblerEnable)
        {
            DWORD dw;
            WriteFile (nvPusherFile,(void*)nvDmaLastPutAddr,(DWORD)nvFifo-(DWORD)nvDmaLastPutAddr,&dw,0);
            FlushFileBuffers (nvPusherFile);
        }
    }
#endif

    (volatile)nvDmaControl->Put = dwCurrentOffset;

    nvDmaLastPutAddr = nvFifo; // important - used by TexMan
    DPF_LEVEL(NVDBG_LEVEL_INFO2, "Last Put = 0x%08lx",nvDmaLastPutAddr);
    return;
}

void nvFlushDmaBuffers
(
    void
)
{
    DWORD           dwDmaLastPutOffset;
    Nv4ControlDma  *nvDmaControl;

    if ((!pDriverData->NvDevFlatDma)
     || (pDriverData->fFullScreenDosOccurred))
        return;

    /*
     * aa capture override - ignore kickoff
     */
    if (pCurrentContext)
        if (pCurrentContext->dwAntiAliasFlags & AA_CAPTURE_PUSHBUFFER)
            return;

    nvDmaControl = (Nv4ControlDma *)pDriverData->NvDevFlatDma;

    /*
     * Setup frequently accessed globals.
     */
    NV_D3D_GLOBAL_SETUP();

    DPF_LEVEL(NVDBG_LEVEL_INFO2, "nvFlushDmaBuffers - Kick off outstanding data.");
    nvStartDmaBuffer (FALSE);

    /*
     * Wait for the buffer to finish.
     * This is done by waiting for the (Get == LastPut)
     */
    dwDmaLastPutOffset = nvFifo - nvDmaPushBase;
    while (nvDmaControl->Get != dwDmaLastPutOffset)
    {
        NV_DELAY;
    }

#if 1   // TEMPORARY_HACK
    DPF_LEVEL(NVDBG_LEVEL_INFO2, "nvFlushDmaBuffers - Wait for Graphics Engine Idle!!!");
    while (*(volatile DWORD *)(pDriverData->NvBaseFlat + 0x400700));
#endif

    NV_D3D_GLOBAL_SAVE();
    return;
}
#endif  // NV4