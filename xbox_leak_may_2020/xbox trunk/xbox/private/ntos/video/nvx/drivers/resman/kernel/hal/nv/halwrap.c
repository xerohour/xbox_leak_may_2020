 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************* HAL Interface Wrappers **************************\
*                                                                           *
* Module: halwrap.c                                                         *
*                                                                           *
* This source module contains wrappers around the HAL interfaces that       *
* are useful for packaging up arguments into the proper revision-specific   *
* data structure.                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nvrm.h>

#undef  nvAddr
#undef  ChipBugs
#define ChipBugs pHalHwInfo->pMcHalInfo->ChipBugs

//-------------------------------------------------------------------
// MC interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalMcControl(PHWINFO pDev, U032 cmd)
{
    MCCONTROLARG_000 mcControlArg;

    mcControlArg.id = MC_CONTROL_000;
    mcControlArg.cmd = cmd;
    mcControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalMcControl)((VOID *)&mcControlArg));
}

RM_STATUS
nvHalMcPower(PHWINFO pDev, U032 newLevel, U032 oldLevel)
{
    MCPOWERARG_000 mcPowerArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcPower oldLevel ", oldLevel);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalMcPower newLevel ", newLevel);

    mcPowerArg.id = MC_POWER_000;
    mcPowerArg.newLevel = newLevel;
    mcPowerArg.oldLevel = oldLevel;
    mcPowerArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalMcPower)((VOID *)&mcPowerArg));
}

//-------------------------------------------------------------------
// Fifo interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalFifoControl(PHWINFO pDev, U032 cmd)
{
    FIFOCONTROLARG_000 fifoControlArg;

    fifoControlArg.id = FIFO_CONTROL_000;
    fifoControlArg.cmd = cmd;
    fifoControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoControl)((VOID *)&fifoControlArg));
}

RM_STATUS
nvHalFifoAllocPio(PHWINFO pDev, U032 chid, U032 instance)
{
    FIFOALLOCPIOARG_000 fifoAllocArg;

    fifoAllocArg.id = FIFO_ALLOC_PIO_000;
    fifoAllocArg.chid = chid;
    fifoAllocArg.instance = instance;
    fifoAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoAllocPio)((VOID *)&fifoAllocArg));
}

RM_STATUS
nvHalFifoAllocDma(PHWINFO pDev, U032 chid, U032 instance, U032 dmaInstance, U032 fetchTrigger, U032 fetchSize, U032 fetchRequests)
{
    FIFOALLOCDMAARG_000 fifoAllocArg;

    fifoAllocArg.id = FIFO_ALLOC_DMA_000;
    fifoAllocArg.chid = chid;
    fifoAllocArg.instance = instance;
    fifoAllocArg.dmaInstance = dmaInstance;
    fifoAllocArg.fetchTrigger = fetchTrigger;
    fifoAllocArg.fetchSize = fetchSize;
    fifoAllocArg.fetchRequests = fetchRequests;
    fifoAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoAllocDma)((VOID *)&fifoAllocArg));
}

RM_STATUS
nvHalFifoFree(PHWINFO pDev, U032 chid, U032 instance)
{
    FIFOFREEARG_000 fifoFreeArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "nvHalFifoFree: chid ", chid);

    fifoFreeArg.id = FIFO_FREE_000;
    fifoFreeArg.chid = chid;
    fifoFreeArg.instance = instance;
    fifoFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoFree)((VOID *)&fifoFreeArg));
}

RM_STATUS
nvHalFifoGetExceptionData(PHWINFO pDev, PFIFOEXCEPTIONDATA pFifoExceptionData)
{
    FIFOGETEXCEPTIONDATAARG_000 fifoGetExceptionDataArg;

    fifoGetExceptionDataArg.id = FIFO_GET_EXCEPTION_DATA_000;
    fifoGetExceptionDataArg.pExceptionData = pFifoExceptionData;
    fifoGetExceptionDataArg.pHalHwInfo = &pDev->halHwInfo;
    
    return (HALFUNC_000(_nvHalFifoGetExceptionData)((VOID *)&fifoGetExceptionDataArg));
}

RM_STATUS
nvHalFifoService(PHWINFO pDev, U032 intr, PFIFOEXCEPTIONDATA pFifoExceptionData)
{
    FIFOSERVICEARG_000 fifoServiceArg;

    fifoServiceArg.id = FIFO_SERVICE_000;
    fifoServiceArg.intr = intr;
    fifoServiceArg.pExceptionData = pFifoExceptionData;
    fifoServiceArg.pHalHwInfo = &pDev->halHwInfo;
    
    return (HALFUNC_000(_nvHalFifoService)((VOID *)&fifoServiceArg));
}

RM_STATUS
nvHalFifoAccess(PHWINFO pDev, U032 cmd, PFIFOACCESSINFO pFifoAccessInfo)
{
    FIFOACCESSARG_000 fifoAccessArg;

    fifoAccessArg.id = FIFO_ACCESS_000;
    fifoAccessArg.cmd = cmd;
    fifoAccessArg.pAccessInfo = pFifoAccessInfo;
    fifoAccessArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoAccess)((VOID *)&fifoAccessArg));
}

RM_STATUS
nvHalFifoHashAdd(PHWINFO pDev, U032 entry, U032 handle, U032 chid, U032 instance, U032 engine)
{
    FIFOHASHADDARG_000 fifoHashAddArg;

    fifoHashAddArg.id = FIFO_HASH_ADD_000;
    fifoHashAddArg.entry = entry;
    fifoHashAddArg.handle = handle;
    fifoHashAddArg.chid = chid;
    fifoHashAddArg.instance = instance;
    fifoHashAddArg.engine = engine;
    fifoHashAddArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoHashAdd)((VOID *)&fifoHashAddArg));
}

RM_STATUS
nvHalFifoHashDelete(PHWINFO pDev, U032 entry)
{
    FIFOHASHDELETEARG_000 fifoHashDeleteArg;

    fifoHashDeleteArg.id = FIFO_HASH_DELETE_000;
    fifoHashDeleteArg.entry = entry;
    fifoHashDeleteArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFifoHashDelete)((VOID *)&fifoHashDeleteArg));
}

RM_STATUS
nvHalFifoHashFunc(PHWINFO pDev, U032 handle, U032 chid, U032 *result)
{
    FIFOHASHFUNCARG_000 fifoHashFuncArg;
    RM_STATUS status;

    fifoHashFuncArg.id = FIFO_HASH_FUNC_000;
    fifoHashFuncArg.handle = handle;
    fifoHashFuncArg.chid = chid;
    fifoHashFuncArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalFifoHashFunc)((VOID *)&fifoHashFuncArg);
    *result = fifoHashFuncArg.result;

    return (status);
}

RM_STATUS
nvHalFifoHashSearch(PHWINFO pDev, U032 handle, U032 chid, U032 *result)
{
    FIFOHASHSEARCHARG_000 fifoHashSearchArg;
    RM_STATUS status;

    fifoHashSearchArg.id = FIFO_HASH_SEARCH_000;
    fifoHashSearchArg.handle = handle;
    fifoHashSearchArg.chid = chid;
    fifoHashSearchArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalFifoHashSearch)((VOID *)&fifoHashSearchArg);
    *result = fifoHashSearchArg.result;

    return (status);
}

//-------------------------------------------------------------------
// Fb interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalFbControl(PHWINFO pDev, U032 cmd)
{
    FBCONTROLARG_000 fbControlArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalFbControl cmd ", cmd);

    fbControlArg.id = FB_CONTROL_000;
    fbControlArg.cmd = cmd;
    fbControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFbControl)((VOID *)&fbControlArg));
}

RM_STATUS
nvHalFbAlloc(PHWINFO pDev, PFBALLOCINFO pFbAllocInfo)
{
    FBALLOCARG_000 fbAllocArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalFbAlloc type ", pFbAllocInfo->type);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                   height ", pFbAllocInfo->height);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                   pitch ", pFbAllocInfo->pitch);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                   size ", pFbAllocInfo->size);

    fbAllocArg.id = FB_ALLOC_000;
    if (pDev->Framebuffer.NoAdjustedPitch) {
        if (pFbAllocInfo->height <= 600)
            fbAllocArg.flags = FB_ALLOC_NO_PITCH_ADJUST;
        else
            fbAllocArg.flags = 0;
    } else
        fbAllocArg.flags = 0;
    fbAllocArg.pAllocInfo = pFbAllocInfo;
    fbAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFbAlloc)((VOID *)&fbAllocArg));
}

RM_STATUS
nvHalFbFree(PHWINFO pDev, U032 hwResId)
{
    FBFREEARG_000 fbFreeArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalFbFree hwresid ", hwResId);

    fbFreeArg.id = FB_FREE_000;
    fbFreeArg.hwResId = hwResId;
    fbFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFbFree)((VOID *)&fbFreeArg));
}

RM_STATUS
nvHalFbSetAllocParameters(PHWINFO pDev, PFBALLOCINFO pFbAllocInfo)
{
    FBSETALLOCPARAMSARG_000 fbSetAllocParamsArg;

    fbSetAllocParamsArg.id = FB_SET_ALLOC_PARAMS_000;
    fbSetAllocParamsArg.pAllocInfo = pFbAllocInfo;
    fbSetAllocParamsArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFbSetAllocParameters)((VOID *)&fbSetAllocParamsArg));
}

RM_STATUS
nvHalFbGetSurfacePitch(PHWINFO pDev, U032 width, U032 bpp, U032 *pitch)
{
    FBGETSURFACEPITCHARG_000 fbGetSurfacePitchArg;
    RM_STATUS status;

    fbGetSurfacePitchArg.id = FB_GET_SURFACE_PITCH_000;
    if (pDev->Framebuffer.NoAdjustedPitch) {
        if (width <= 800)
            fbGetSurfacePitchArg.flags = FB_GET_SURFACE_PITCH_NO_ADJUST;
        else
            fbGetSurfacePitchArg.flags = 0;
    } else
        fbGetSurfacePitchArg.flags = 0;
    fbGetSurfacePitchArg.width = width;
    fbGetSurfacePitchArg.bpp = bpp;
    fbGetSurfacePitchArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalFbGetSurfacePitch)((VOID *)&fbGetSurfacePitchArg);
    *pitch = fbGetSurfacePitchArg.pitch;

    return (status);
}

RM_STATUS
nvHalFbGetSurfaceDimensions(PHWINFO pDev, U032 width, U032 height, U032 bpp, U032 *pitch, U032 *size)
{
    FBGETSURFACEDIMENSIONSARG_000 fbGetSurfaceDimensionsArg;
    RM_STATUS status;

    fbGetSurfaceDimensionsArg.id = FB_GET_SURFACE_DIMENSIONS_000;
    if (pDev->Framebuffer.NoAdjustedPitch) {
        if (width <= 800)
            fbGetSurfaceDimensionsArg.flags = FB_GET_SURFACE_PITCH_NO_ADJUST;
        else
            fbGetSurfaceDimensionsArg.flags = 0;
    } else
        fbGetSurfaceDimensionsArg.flags = 0;
    fbGetSurfaceDimensionsArg.width = width;
    fbGetSurfaceDimensionsArg.height = height;
    fbGetSurfaceDimensionsArg.bpp = bpp;
    fbGetSurfaceDimensionsArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalFbGetSurfaceDimensions)((VOID *)&fbGetSurfaceDimensionsArg);
    *pitch = fbGetSurfaceDimensionsArg.pitch;
    *size  = fbGetSurfaceDimensionsArg.size;

    return (status);
}

RM_STATUS
nvHalFbLoadOverride(PHWINFO pDev, U032 cmd, U032 value)
{
    FBLOADOVERRIDEARG_000 fbLoadOverrideArg;

    fbLoadOverrideArg.id = FB_LOAD_OVERRIDE_000;
    fbLoadOverrideArg.cmd = cmd;
    fbLoadOverrideArg.value = value;
    fbLoadOverrideArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalFbLoadOverride)((VOID *)&fbLoadOverrideArg));
}

//-------------------------------------------------------------------
// Gr interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalGrControl(PHWINFO pDev, U032 cmd)
{
    GRCONTROLARG_000 grControlArg;

    grControlArg.id = GR_CONTROL_000;
    grControlArg.cmd = cmd;
    grControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrControl)((VOID *)&grControlArg));
}

RM_STATUS
nvHalGrAlloc(PHWINFO pDev, U032 chid, U032 instance, U032 classNum)
{
    GRALLOCARG_000 grAllocArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrAlloc chid ", chid);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:              instance ", instance);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:              classNum ", classNum);

    grAllocArg.id = GR_ALLOC_000;
    grAllocArg.classNum = classNum;
    grAllocArg.instance = instance;
    grAllocArg.chid = chid; 
    grAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrAlloc)((VOID *)&grAllocArg));
}

RM_STATUS
nvHalGrFree(PHWINFO pDev, U032 chid, U032 instance)
{
    GRFREEARG_000 grFreeArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrFree instance ", instance);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalGrFree chid ", chid);

    grFreeArg.id = GR_FREE_000;
    grFreeArg.chid = chid;
    grFreeArg.instance = instance;
    grFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrFree)((VOID *)&grFreeArg));
}

RM_STATUS
nvHalGrGetExceptionData(PHWINFO pDev, PGREXCEPTIONDATA pGrExceptionData)
{
    GRGETEXCEPTIONDATAARG_000 grGetExceptionDataArg;

    grGetExceptionDataArg.id = GR_GET_EXCEPTION_DATA_000;
    grGetExceptionDataArg.pExceptionData = pGrExceptionData;
    grGetExceptionDataArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrGetExceptionData)((VOID *)&grGetExceptionDataArg));
}

RM_STATUS
nvHalGrService(PHWINFO pDev, U032 intr, PGREXCEPTIONDATA pGrExceptionData)
{
    GRSERVICEARG_000 grServiceArg;

    grServiceArg.id = GR_SERVICE_000;
    grServiceArg.intr = intr;
    grServiceArg.pExceptionData = pGrExceptionData;
    grServiceArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrService)((VOID *)&grServiceArg));
}

RM_STATUS
nvHalGrGetNotifyData(PHWINFO pDev, PGRNOTIFYDATA pGrNotifyData)
{
    GRGETNOTIFYDATAARG_000 grGetNotifyDataArg;

    grGetNotifyDataArg.id = GR_GET_NOTIFY_DATA_000;
    grGetNotifyDataArg.pGrNotifyData = pGrNotifyData;
    grGetNotifyDataArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrGetNotifyData)((VOID *)&grGetNotifyDataArg));
}

RM_STATUS
nvHalGrSetObjectContext(PHWINFO pDev, U032 andMask, U032 orMask, U032 chid, U032 instance)
{
    GRSETOBJECTCONTEXTARG_000 grSetObjectContextArg;

    grSetObjectContextArg.id = GR_SET_OBJECT_CONTEXT_000;
    grSetObjectContextArg.andMask = andMask;
    grSetObjectContextArg.orMask = orMask;
    grSetObjectContextArg.instance = instance;
    grSetObjectContextArg.chid = chid;
    grSetObjectContextArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrSetObjectContext)((VOID *)&grSetObjectContextArg));
}

RM_STATUS
nvHalGrLoadOverride(PHWINFO pDev, U032 cmd, U032 value)
{
    GRLOADOVERRIDEARG_000 grLoadOverrideArg;

    grLoadOverrideArg.id = GR_LOAD_OVERRIDE_000;
    grLoadOverrideArg.cmd = cmd;
    grLoadOverrideArg.value = value;
    grLoadOverrideArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalGrLoadOverride)((VOID *)&grLoadOverrideArg));
}

//-------------------------------------------------------------------
// Dma interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalDmaControl(PHWINFO pDev, U032 cmd)
{
    DMACONTROLARG_000 dmaControlArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalDmaControl cmd ", cmd);

    dmaControlArg.id = DMA_CONTROL_000;
    dmaControlArg.cmd = cmd;
    dmaControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDmaControl)((VOID *)&dmaControlArg));

}

RM_STATUS
nvHalDmaAlloc(PHWINFO pDev, PDMAHALOBJINFO pDmaHalObjInfo)
{
    DMAALLOCARG_000 dmaAllocArg;
    
    dmaAllocArg.id = DMA_ALLOC_000;
    dmaAllocArg.pHalObjInfo = pDmaHalObjInfo;
    dmaAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDmaAlloc)((VOID *)&dmaAllocArg));
}

RM_STATUS
nvHalDmaFree(PHWINFO pDev, PDMAHALOBJINFO pDmaHalObjInfo)
{
    DMAFREEARG_000 dmaFreeArg;
    
    dmaFreeArg.id = DMA_FREE_000;
    dmaFreeArg.pHalObjInfo = pDmaHalObjInfo;
    dmaFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDmaFree)((VOID *)&dmaFreeArg));
}

RM_STATUS
nvHalDmaGetInstSize(PHWINFO pDev, PDMAHALOBJINFO pDmaHalObjInfo)
{
    DMAGETINSTSIZEARG_000 dmaGetInstSizeArg;
    
    dmaGetInstSizeArg.id = DMA_GET_INSTSIZE_000;
    dmaGetInstSizeArg.pHalObjInfo = pDmaHalObjInfo;
    dmaGetInstSizeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDmaGetInstSize)((VOID *)&dmaGetInstSizeArg));
}

//-------------------------------------------------------------------
// Dac interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalDacControl(PHWINFO pDev, U032 cmd)
{
    DACCONTROLARG_000 dacControlArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalDacControl cmd ", cmd);

    dacControlArg.id = DAC_CONTROL_000;
    dacControlArg.cmd = cmd;
    dacControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacControl)((VOID *)&dacControlArg));
}

RM_STATUS
nvHalDacAlloc(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    DACALLOCARG_000 dacAllocArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "nvHalDacAlloc: classNum ", classNum);

    dacAllocArg.id = DAC_ALLOC_000;
    dacAllocArg.chid = chid;
    dacAllocArg.instance = instance;
    dacAllocArg.classNum = classNum;
    dacAllocArg.pHalObjInfo = pHalObj;
    dacAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacAlloc)((VOID *)&dacAllocArg));
}

RM_STATUS
nvHalDacFree(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    DACFREEARG_000 dacFreeArg;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "nvHalDacFree: classNum ", classNum);

    dacFreeArg.id = DAC_FREE_000;
    dacFreeArg.chid = chid;
    dacFreeArg.instance = instance;
    dacFreeArg.classNum = classNum;
    dacFreeArg.pHalObjInfo = pHalObj;
    dacFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacFree)((VOID *)&dacFreeArg));
}

RM_STATUS
nvHalDacSetStartAddr(PHWINFO pDev, U032 startAddr, VOID *pHalObj)
{
    DACSETSTARTADDRARG_000 dacSetStartAddrArg;

    dacSetStartAddrArg.id = DAC_SET_START_ADDR_000;
    dacSetStartAddrArg.startAddr = startAddr;
    dacSetStartAddrArg.pHalObjInfo = pHalObj;
    dacSetStartAddrArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacSetStartAddr)((VOID *)&dacSetStartAddrArg));
}

RM_STATUS
nvHalDacProgramMClk(PHWINFO pDev)
{
    DACPROGRAMMCLKARG_000   dacProgramMClkArg;

    dacProgramMClkArg.id = DAC_PROGRAMMCLK_000;
    dacProgramMClkArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacProgramMClk)((VOID *)&dacProgramMClkArg));
}

RM_STATUS
nvHalDacProgramNVClk(PHWINFO pDev)
{
    DACPROGRAMNVCLKARG_000  dacProgramNVClkArg;

    dacProgramNVClkArg.id = DAC_PROGRAMNVCLK_000;
    dacProgramNVClkArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalDacProgramNVClk)((VOID *)&dacProgramNVClkArg));
}

RM_STATUS
nvHalDacProgramPClk(PHWINFO pDev, U032 PixelClock, VOID *pHalObj)
{
    DACPROGRAMPCLKARG_000   dacProgramPClkArg;

    dacProgramPClkArg.id = DAC_PROGRAMPCLK_000;
    dacProgramPClkArg.pHalHwInfo = &pDev->halHwInfo;
    dacProgramPClkArg.pHalObjInfo = pHalObj;
    dacProgramPClkArg.PixelClock = PixelClock;

    return (HALFUNC_000(_nvHalDacProgramPClk)((VOID *)&dacProgramPClkArg));
}

RM_STATUS
nvHalDacProgramCursorImage(PHWINFO pDev, U032 startAddr, U032 asi,
                           U032 width, U032 height, U032 colorFormat, VOID *pHalObj)
{
    DACPROGRAMCURSORARG_000     dacProgramCursorImgArg;
    
    dacProgramCursorImgArg.id = DAC_PROGRAMCURSOR_000;
    dacProgramCursorImgArg.pHalHwInfo = &pDev->halHwInfo;
    dacProgramCursorImgArg.pHalObjInfo = pHalObj;
    dacProgramCursorImgArg.startAddr = startAddr;
    dacProgramCursorImgArg.asi = asi;
    dacProgramCursorImgArg.width = width;
    dacProgramCursorImgArg.height = height;
    dacProgramCursorImgArg.colorFormat = colorFormat;

    return (HALFUNC_000(_nvHalDacProgramCursorImage)((VOID *)&dacProgramCursorImgArg));
}

RM_STATUS
nvHalDacGetRasterPosition(PHWINFO pDev, U016 *result, VOID *pHalObj)
{
    DACRASTERPOSARG_000    dacRasterPosArg;
    RM_STATUS status;

    dacRasterPosArg.id = DAC_RASTERPOS_000;
    dacRasterPosArg.pHalHwInfo = &pDev->halHwInfo;
    dacRasterPosArg.pHalObjInfo = pHalObj;

    // fills in dacRasterPosArg.result
    status = HALFUNC_000(_nvHalDacGetRasterPosition)((VOID *)&dacRasterPosArg);

    *result = dacRasterPosArg.result;

    return status;
}

RM_STATUS
nvHalDacValidateArbSettings(PHWINFO pDev, U032 bpp, U032 video_enabled, U032 mport_enabled, U032 vclk, U032 *result)
{
    DACVALIDATEARBSETTINGSARG_000   dacValidateArbSettingsArg;
    RM_STATUS status;

    dacValidateArbSettingsArg.id = DAC_VALIDATEARBSETTINGS_000;
    dacValidateArbSettingsArg.pHalHwInfo = &pDev->halHwInfo;
    dacValidateArbSettingsArg.bpp = bpp;
    dacValidateArbSettingsArg.video_enabled = video_enabled;
    dacValidateArbSettingsArg.mport_enabled = mport_enabled;
    dacValidateArbSettingsArg.vclk = vclk;

    // fills in dacValidateArbSettingsArg.result
    status = HALFUNC_000(_nvHalDacValidateArbSettings)((VOID *)&dacValidateArbSettingsArg);

    *result = dacValidateArbSettingsArg.result;

    return status;
}

RM_STATUS
nvHalDacUpdateArbSettings(PHWINFO pDev, VOID *pHalObj)
{
    DACUPDATEARBSETTINGSARG_000     dacUpdateArbSettingsArg;
    
    dacUpdateArbSettingsArg.id = DAC_UPDATEARBSETTINGS_000;
    dacUpdateArbSettingsArg.pHalHwInfo = &pDev->halHwInfo;
    dacUpdateArbSettingsArg.pHalObjInfo = pHalObj;

    return (HALFUNC_000(_nvHalDacUpdateArbSettings)((VOID *)&dacUpdateArbSettingsArg));
}

//-------------------------------------------------------------------
// Mediaport interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalMpControl(PHWINFO pDev, U032 cmd)
{
    MPCONTROLARG_000 mpControlArg;

    mpControlArg.id = MP_CONTROL_000;
    mpControlArg.cmd = cmd;
    mpControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalMpControl)((VOID *)&mpControlArg));
}

RM_STATUS
nvHalMpAlloc(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    MPALLOCARG_000 mpAllocArg;

    mpAllocArg.id = MP_ALLOC_000;
    mpAllocArg.chid = chid;
    mpAllocArg.instance = instance;
    mpAllocArg.classNum = classNum;
    mpAllocArg.pHalObjInfo = pHalObj;
    mpAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalMpAlloc)((VOID *)&mpAllocArg));
}

RM_STATUS
nvHalMpFree(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    MPFREEARG_000 mpFreeArg;

    mpFreeArg.id = MP_FREE_000;
    mpFreeArg.chid = chid;
    mpFreeArg.instance = instance;
    mpFreeArg.classNum = classNum;
    mpFreeArg.pHalObjInfo = pHalObj;
    mpFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalMpFree)((VOID *)&mpFreeArg));
}

RM_STATUS
nvHalMpMethod(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 offset, U032 data, U032 *mthdStatus)
{
    MPMETHODARG_000 mpMethodArg;
    RM_STATUS status;

    mpMethodArg.id = MP_METHOD_000;
    mpMethodArg.classNum = classNum;
    mpMethodArg.pHalObjInfo = pHalObj;
    mpMethodArg.offset = offset;
    mpMethodArg.data = data;
    mpMethodArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalMpMethod)((VOID *)&mpMethodArg);

    if (mthdStatus)
        *mthdStatus = mpMethodArg.mthdStatus;

    return status;
}

RM_STATUS
nvHalMpGetEventStatus(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 *pEventsPending, V032 *pIntrStatus)
{
    RM_STATUS status;
    MPGETEVENTSTATUSARG_000 mpGetEventStatusArg;

    mpGetEventStatusArg.id = MP_GET_EVENT_STATUS_000;
    mpGetEventStatusArg.classNum = classNum;
    mpGetEventStatusArg.pHalObjInfo = pHalObj;
    mpGetEventStatusArg.events = *pEventsPending;
    mpGetEventStatusArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalMpGetEventStatus)((VOID *)&mpGetEventStatusArg);
    *pEventsPending = mpGetEventStatusArg.events;
    *pIntrStatus = mpGetEventStatusArg.intrStatus;

    return status;
}

RM_STATUS
nvHalMpServiceEvent(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 eventPending, V032 *pIntrStatus)
{
    RM_STATUS status;
    MPSERVICEEVENTARG_000 mpServiceEventArg;

    mpServiceEventArg.id = MP_SERVICE_EVENT_000;
    mpServiceEventArg.classNum = classNum;
    mpServiceEventArg.pHalObjInfo = pHalObj;
    mpServiceEventArg.event = eventPending;
    mpServiceEventArg.intrStatus = *pIntrStatus;
    mpServiceEventArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalMpServiceEvent)((VOID *)&mpServiceEventArg);
    *pIntrStatus = mpServiceEventArg.intrStatus;

    return status;
}

//-------------------------------------------------------------------
// Video interface wrappers
//-------------------------------------------------------------------
RM_STATUS
nvHalVideoControl(PHWINFO pDev, U032 cmd)
{
    VIDEOCONTROLARG_000 videoControlArg;

    videoControlArg.id = VIDEO_CONTROL_000;
    videoControlArg.cmd = cmd;
    videoControlArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalVideoControl)((VOID *)&videoControlArg));
}

RM_STATUS
nvHalVideoAlloc(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    VIDEOALLOCARG_000 videoAllocArg;

    videoAllocArg.id = VIDEO_ALLOC_000;
    videoAllocArg.chid = chid;
    videoAllocArg.instance = instance;
    videoAllocArg.classNum = classNum;
    videoAllocArg.pHalObjInfo = pHalObj;
    videoAllocArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalVideoAlloc)((VOID *)&videoAllocArg));
}

RM_STATUS
nvHalVideoFree(PHWINFO pDev, U032 chid, U032 instance, U032 classNum, VOID *pHalObj)
{
    VIDEOFREEARG_000 videoFreeArg;

    videoFreeArg.id = VIDEO_FREE_000;
    videoFreeArg.chid = chid;
    videoFreeArg.instance = instance;
    videoFreeArg.classNum = classNum;
    videoFreeArg.pHalObjInfo = pHalObj;
    videoFreeArg.pHalHwInfo = &pDev->halHwInfo;

    return (HALFUNC_000(_nvHalVideoFree)((VOID *)&videoFreeArg));
}

RM_STATUS
nvHalVideoMethod(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 offset, U032 data, U032 *mthdStatus)
{
    VIDEOMETHODARG_000 videoMethodArg;
    RM_STATUS status;

    videoMethodArg.id = VIDEO_METHOD_000;
    videoMethodArg.classNum = classNum;
    videoMethodArg.pHalObjInfo = pHalObj;
    videoMethodArg.offset = offset;
    videoMethodArg.data = data;
    videoMethodArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalVideoMethod)((VOID *)&videoMethodArg);
    if (mthdStatus)
        *mthdStatus = videoMethodArg.mthdStatus;
    
    return (status);
}

RM_STATUS
nvHalVideoGetEventStatus(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 *pEvents, V032 *pIntrStatus)
{
    RM_STATUS status;
    VIDEOGETEVENTSTATUSARG_000 videoGetEventStatusArg;

    videoGetEventStatusArg.id = VIDEO_GET_EVENT_STATUS_000;
    videoGetEventStatusArg.classNum = classNum;
    videoGetEventStatusArg.pHalObjInfo = pHalObj;
    videoGetEventStatusArg.events = 0;
    videoGetEventStatusArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalVideoGetEventStatus)((VOID *)&videoGetEventStatusArg);
    *pEvents = videoGetEventStatusArg.events;
    *pIntrStatus = videoGetEventStatusArg.intrStatus;

    return status;
}

RM_STATUS
nvHalVideoServiceEvent(PHWINFO pDev, U032 classNum, VOID *pHalObj, U032 events, V032 *pIntrStatus)
{
    RM_STATUS status;
    VIDEOSERVICEEVENTARG_000 videoServiceEventArg;

    videoServiceEventArg.id = VIDEO_SERVICE_EVENT_000;
    videoServiceEventArg.classNum = classNum;
    videoServiceEventArg.pHalObjInfo = pHalObj;
    videoServiceEventArg.events = events;
    videoServiceEventArg.intrStatus = *pIntrStatus;
    videoServiceEventArg.pHalHwInfo = &pDev->halHwInfo;

    status = HALFUNC_000(_nvHalVideoServiceEvent)((VOID *)&videoServiceEventArg);
    *pIntrStatus = videoServiceEventArg.intrStatus;

    return status;
}
