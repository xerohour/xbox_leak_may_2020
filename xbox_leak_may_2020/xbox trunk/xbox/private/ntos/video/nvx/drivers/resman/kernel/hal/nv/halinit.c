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

/********************** HAL Initialization/Destruction *********************\
*                                                                           *
* Module: halinit.c                                                         *
*                                                                           *
* This source module contains the HAL initialize and tear-down code.        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <fb.h>
#include <nvhal.h>
#include "nvhw.h"

// statics
static RM_STATUS allocHalEngineMem(PHWINFO);
RM_STATUS allocHalEngineInstMem(PHWINFO);
RM_STATUS freeHalEngineInstMem(PHWINFO);

//
// initHal
//
// It all starts here.  Id the chip represented by pDev and
// wire up the correct hal data/code. Generate the class database
// and allocate any engine-specific resources (e.g. instance memory).
//
RM_STATUS
initHal(PHWINFO pDev)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    U032 pmcBoot0;
    RM_STATUS status;
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;

    //
    // Hook up chip-dependent data.
    //
    pmcBoot0 = REG_RD32(NV_PMC_BOOT_0);

#ifdef DEBUG
    //
    // This check will catch the case where we haven't yet mapped the regs.
    //
    if (pmcBoot0 == 0xffffffff)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: registers not mapped in initHal\n\r!");
        DBG_BREAKPOINT();
        return RM_ERROR;
    }
#endif

    //
    // Check for nv4 and family using the old-style PMC_BOOT_0 format.
    // Check for nv10 and beyond using new-style format.
    //
    if (DRF_VAL(_PMC, _BOOT_0, _ARCHITECTURE, pmcBoot0) == NV_PMC_BOOT_0_ARCHITECTURE_NV4)
    {
        U032 impl = REG_RD_DRF(_PMC, _BOOT_0, _MASK_REVISION);  // implementation

        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: hooking up NV4 HAL\n");
        pDev->pHalFuncs = (PHALFUNCS)&NV04_HalFuncs_000;
        pDev->engineDB.pEngineDescriptors = NV04_EngineDescriptors;
        pDev->engineDB.numEngineDescriptors = NV04_NumEngineDescs;
        if (impl == MC_IMPLEMENTATION_NV04) {
            pDev->classDB.pClassDescriptors = NV04_ClassDescriptors;
            pDev->classDB.numClassDescriptors = NV04_NumClassDescs;
        } else {
            pDev->classDB.pClassDescriptors = NV05_ClassDescriptors;
            pDev->classDB.numClassDescriptors = NV05_NumClassDescs;
        }
    }
    else if (MC_GET_ARCHITECTURE(pmcBoot0) == MC_ARCHITECTURE_NV10)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: hooking up NV10 HAL\n");
        pDev->pHalFuncs = (PHALFUNCS)&NV10_HalFuncs_000;
        pDev->engineDB.pEngineDescriptors = NV10_EngineDescriptors;
        pDev->engineDB.numEngineDescriptors = NV10_NumEngineDescs;
        if (MC_GET_IMPLEMENTATION(pmcBoot0) == MC_IMPLEMENTATION_NV10) {
            pDev->classDB.pClassDescriptors =   NV10_ClassDescriptors;
            pDev->classDB.numClassDescriptors = NV10_NumClassDescs;
        } else if (MC_GET_IMPLEMENTATION(pmcBoot0) == MC_IMPLEMENTATION_NV15) {
            pDev->classDB.pClassDescriptors =   NV15_ClassDescriptors;
            pDev->classDB.numClassDescriptors = NV15_NumClassDescs;
        } else if (MC_GET_IMPLEMENTATION(pmcBoot0) == MC_IMPLEMENTATION_NV11) {
            pDev->classDB.pClassDescriptors =   NV11_ClassDescriptors;
            pDev->classDB.numClassDescriptors = NV11_NumClassDescs;
        }
    }
    else if (MC_GET_ARCHITECTURE(pmcBoot0) == MC_ARCHITECTURE_NV20)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: hooking up NV20 HAL\n");
        pDev->pHalFuncs = (PHALFUNCS)&NV20_HalFuncs_000;
        pDev->engineDB.pEngineDescriptors = NV20_EngineDescriptors;
        pDev->engineDB.numEngineDescriptors = NV20_NumEngineDescs;
        pDev->classDB.pClassDescriptors = NV20_ClassDescriptors;
        pDev->classDB.numClassDescriptors = NV20_NumClassDescs;
    }
    else
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: unknown architecture in pmcboot0", pmcBoot0);
        DBG_BREAKPOINT();
        return RM_ERROR;
    }

    //
    // Allocate space for HAL private areas (ENGMALLOC_TYPE_PRIVATE buffers).
    //
    status = allocHalEngineMem(pDev);

    //
    // Initialize mapped device/fb addresses.  Note that on some
    // platforms (i.e. nt), we'll be asked to setup the HAL before
    // the framebuffer is mapped.  The initHalMappings call can be
    // used to handle such cases (see below).
    //
    pHalHwInfo->nvBaseAddr = pDev->DBnvAddr;
    pHalHwInfo->fbBaseAddr = pDev->DBfbAddr;

    //
    // Setup up RM services table pointer.
    //
    pHalHwInfo->pHalRmFuncs = (PHALRMFUNCS)&HalRmFuncs_000;

    //
    // Save off (opaque) device identifier.
    //
    pHalHwInfo->pDeviceId = (VOID_PTR)pDev;

    //
    // Initialize HAL info pointers.
    //
    pHalHwInfo->pProcessorHalInfo = &pRmInfo->Processor.HalInfo;
    pHalHwInfo->pMcHalInfo = &pDev->Chip.HalInfo;
    pHalHwInfo->pPramHalInfo = &pDev->Pram.HalInfo;
    pHalHwInfo->pFifoHalInfo = &pDev->Fifo.HalInfo;
    pHalHwInfo->pFbHalInfo = &pDev->Framebuffer.HalInfo;
    pHalHwInfo->pGrHalInfo = &pDev->Graphics.HalInfo;
    pHalHwInfo->pDacHalInfo = &pDev->Dac.HalInfo;
    pHalHwInfo->pDmaHalInfo = &pDev->Dma.HalInfo;
    pHalHwInfo->pVideoHalInfo = &pDev->Video.HalInfo;
    pHalHwInfo->pMpHalInfo = &pDev->MediaPort.HalInfo;

    //
    // Now create class database.
    //
    status = classBuildDB(&pDev->engineDB, &pDev->classDB);
    if (status != RM_OK)
    {   
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: classBuildDB failure ", status);
        osFreeMem((VOID *)pDev->pHalEngineMem);
        pDev->pHalEngineMem = 0;
    }
    return (status);
}

RM_STATUS
destroyHal(PHWINFO pDev)
{
    classDestroyDB(&pDev->classDB);
    freeHalEngineInstMem(pDev);
    osFreeMem((VOID *)pDev->pHalEngineMem);

    return (RM_OK);
}

//
// initHalMappings
//
// This routine moves the mapped register/framebuffer addresses into
// into the HwInfo struct (pDev) into the HalHwInfo struct.
//
RM_STATUS
initHalMappings(PHWINFO pDev)
{
    //
    // Initialize mapped device/fb addresses.
    //
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: initHal: nvAddr ", pDev->DBnvAddr);
    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: initHal: fbAddr ", pDev->DBfbAddr);

    pDev->halHwInfo.nvBaseAddr = pDev->DBnvAddr;
    pDev->halHwInfo.fbBaseAddr = pDev->DBfbAddr;
    
    return (RM_OK);
}

//
// allocHalEngineMem
//
// Setup HAL engine private data areas using info from
// engine descriptor table.
//
static RM_STATUS
allocHalEngineMem(PHWINFO pDev)
{
    U008 *pHalData;
    RM_STATUS status;
    U032 i, tag, size;
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;
    PENGINEDB pEngineDB  = &pDev->engineDB;

    //
    // Start by scanning engine descriptor table for number of ENGMALLOC
    // instructions.
    //
    size = 0; pHalData = NULL;
    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if ((ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) != ENGMALLOC_INSTR) ||
            (ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TYPE) != ENGMALLOC_TYPE_PRIVATE))
            continue;

        // Value is in dwords, so adjust to get bytes
        // In addition, align to a pointer boundary to avoid unaligned
        // reference faults on the IA64.

        size += NV_ALIGN_UP((ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE) * 4), sizeof(void *));
    }
    if (size == 0)
        return (RM_OK);

    status = osAllocMem((VOID *)&pHalData, size);
    if (status)
        return (status);

    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: HAL private area base ", pHalData);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: total HAL private area size ", size);

    //
    // Clear this memory out.
    //
    for (i = 0; i < size; i++)
    {
        pHalData[i] = 0;
    }

    //
    // Wire up engine-specific private info pointers.
    //
    pDev->pHalEngineMem = (VOID_PTR)pHalData;
    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if ((ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) != ENGMALLOC_INSTR) ||
            (ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TYPE) != ENGMALLOC_TYPE_PRIVATE))
            continue;

        tag = ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TAG);
        size = NV_ALIGN_UP((ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE) * 4), sizeof(void *));

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: engine tag ", tag);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: engine pvt size ", size);
        DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO, "NVRM: engine pvt base ", pHalData);

        switch (tag)
        {
            case MC_ENGINE_TAG:
                pHalHwInfo->pMcHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case FB_ENGINE_TAG:
                pHalHwInfo->pFbHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case DAC_ENGINE_TAG:
                pHalHwInfo->pDacHalPvtInfo = (VOID_PTR)pHalData;
                break;
#if 0
            case TMR_ENGINE_TAG:
                pHalHwInfo->pTmrHalPvtInfo = (VOID_PTR)pHalData;
                break;
#endif
            case DMA_ENGINE_TAG:
                pHalHwInfo->pDmaHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case GR_ENGINE_TAG:
                pHalHwInfo->pGrHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case FIFO_ENGINE_TAG:
                pHalHwInfo->pFifoHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case MP_ENGINE_TAG:
                pHalHwInfo->pMpHalPvtInfo = (VOID_PTR)pHalData;
                break;
            case VIDEO_ENGINE_TAG:
                pHalHwInfo->pVideoHalPvtInfo = (VOID_PTR)pHalData;
                break;
            default:
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: unknown engine tag ", tag);
                break;
        }
        pHalData += size;
    }

    return (RM_OK);
}

//
// allocHalEngineInstMem
//
// Setup HAL engine instance memory areas using info from
// engine descriptor table.
//
RM_STATUS
allocHalEngineInstMem(PHWINFO pDev)
{
    RM_STATUS status;
    U032 i, tag, size, align;
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;
    PENGINEDB pEngineDB = &pDev->engineDB;
    U032 instance = 0;

    //
    // Start by scanning engine descriptor table for number of ENGINSTMALLOC
    // instructions.
    //
    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if (ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) != ENGINSTMALLOC_INSTR)
            continue;

        //
        // Get size/alignment (specified in paragraphs).
        //
        tag = ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TAG);
        size = ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE);
        align = ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _ALIGN);
        if (align == 0)
            align = 1;
#ifdef DEBUG
        if (size == 0)
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: ENGINSTMALLOC has zero size!", tag);
            DBG_BREAKPOINT();
            continue;
        }
#endif

        if (align == 1)
            status = fbAllocInstMem(pDev, &instance, size);
        else
            status = fbAllocInstMemAlign(pDev, &instance, size, align);

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: engine tag ", tag);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: instance size ", size);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: instance align ", align);

        //
        // Save instance offset in corresponding engine state.
        // 
        switch (tag)
        {
            case MC_ENGINE_TAG:
                pHalHwInfo->mcInstMem = instance;
                break;
            case FB_ENGINE_TAG:
                pHalHwInfo->fbInstMem = instance;
                break;
            case DAC_ENGINE_TAG:
                pHalHwInfo->dacInstMem = instance;
                break;
#if 0
            case TMR_ENGINE_TAG:
                pHalHwInfo->tmrInstMem = instance;
                break;
#endif
            case DMA_ENGINE_TAG:
                pHalHwInfo->dmaInstMem = instance;
                break;
            case GR_ENGINE_TAG:
                pHalHwInfo->grInstMem = instance;
                break;
            case FIFO_ENGINE_TAG:
                pHalHwInfo->fifoInstMem = instance;
                break;
            case MP_ENGINE_TAG:
                pHalHwInfo->mpInstMem = instance;
                break;
            case VIDEO_ENGINE_TAG:
                pHalHwInfo->videoInstMem = instance;
                break;
            default:
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: unknown engine tag ", tag);
                break;
        }
    }

    return (RM_OK);
}

//
// freeHalEngineInstMem
//
// Release HAL engine instance memory areas using info from
// engine descriptor table.  This routine really isn't needed
// because the only time it's called is when the device is
// being "destroyed".
//
RM_STATUS
freeHalEngineInstMem(PHWINFO pDev)
{
    U032 i, tag, size;
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;
    PENGINEDB pEngineDB = &pDev->engineDB;

    //
    // Start by scanning engine descriptor table for number of ENGINSTMALLOC
    // instructions.
    //
    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if (ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) != ENGINSTMALLOC_INSTR)
            continue;

        //
        // Get size/alignment (specified in paragraphs).
        //
        tag = ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TAG);
        size = ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE);
        
        //
        // Save instance offset in corresponding engine state.
        // 
        switch (tag)
        {
            case MC_ENGINE_TAG:
                if (pHalHwInfo->mcInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->mcInstMem, size);
                break;
            case FB_ENGINE_TAG:
                if (pHalHwInfo->fbInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->fbInstMem, size);
                break;
            case DAC_ENGINE_TAG:
                if (pHalHwInfo->dacInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->dacInstMem, size);
                break;
#if 0
            case TMR_ENGINE_TAG:
                if (pHalHwInfo->tmrInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->tmrInstMem, size);
                break;
#endif
            case DMA_ENGINE_TAG:
                if (pHalHwInfo->dmaInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->dmaInstMem, size);
                break;
            case GR_ENGINE_TAG:
                if (pHalHwInfo->grInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->grInstMem, size);
                break;
            case FIFO_ENGINE_TAG:
                if (pHalHwInfo->fifoInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->fifoInstMem, size);
                break;
            case MP_ENGINE_TAG:
                if (pHalHwInfo->mpInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->mpInstMem, size);
                break;
            case VIDEO_ENGINE_TAG:
                if (pHalHwInfo->videoInstMem)
                    fbFreeInstMem(pDev, pHalHwInfo->videoInstMem, size);
                break;
            default:
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: unknown engine tag ", tag);
                break;
        }
    }

    return (RM_OK);
}
