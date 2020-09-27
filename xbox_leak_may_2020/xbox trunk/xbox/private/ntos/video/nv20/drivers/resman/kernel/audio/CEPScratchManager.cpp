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

/************************Object to manage scratch***************************\
*                                                                           *
* Module: CEPScratchManager.cpp                                              *
*   Object to handle all RM-GP conversations via the scracth memory         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <nvarm.h>
#include <nv32.h>
#include <aos.h>
#include <AudioHw.h>
#include <nv_papu.h>
};

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalGlobalProc.h"
#include "CScratchDma.h"
#include "CScratchManager.h"
#include "CEPScratchManager.h"
#include "CDspProgram.h"
#include "CEPDspProgram.h"

#define MAX_SIZE_SCRATCH_EP     65536

#define AC3_BUFFER_COUNT        2      // so we can ping-pong
#define AC3_FRAME_SIZE          256
#define AC3_CONTAINER_SIZE      4
#define AC3_NUM_CHANNELS        6
#define AC3_NUM_INPUT_PAGES     ((AC3_BUFFER_COUNT * AC3_FRAME_SIZE * AC3_CONTAINER_SIZE * AC3_NUM_CHANNELS) + PAGE_SIZE-1)  / PAGE_SIZE

CEPScratchManager::CEPScratchManager(CHalBase *pHal)
{
    m_pHal = pHal;
    m_pGlobalProc = NULL;
    m_pDspProgram = NULL;
}

CEPScratchManager::~CEPScratchManager()
{
    
    // if this was an AC-3 party, then stop it at the GP
    if (m_pGlobalProc)
        AC3StopGpInput();

    Shutdown();

    CScratchDma *pScratchDma = GetScratchDma();
    if (pScratchDma)
    {
        VOID *pScratchTable = pScratchDma->GetScratchTableLinAddr();
        aosFreeMem(pScratchTable);
        delete pScratchDma;
    }
}


RM_STATUS   
CEPScratchManager::Initialize(VOID_PTR pLoader, U032 uLoaderSize)
{
    // allocate the scratch Dma
    CScratchDma *pScratchDma = new CScratchDma(m_pHal, NV_PAPU_EPSMAXSGE);
    if (!pScratchDma)
        return RM_ERROR;

    // 	configure the HW
    VOLU32 vTemp = (MAX_SIZE_SCRATCH_EP/PAGE_SIZE);
	m_pHal->RegWrite(NV_PAPU_EPSMAXSGE, LOW16(vTemp));
	m_pHal->RegRead(NV_PAPU_EPSMAXSGE, &vTemp);

    MEM_DESC ScratchTable;

    ScratchTable.Size = (vTemp + 1)*NV_PSGE_SIZE;

    // allocate the scratch table - PRDs for the max size
    RM_STATUS rmStatus = aosAllocateMem(ScratchTable.Size,
						                ALIGN_16K,
						                &ScratchTable.pLinAddr,
						                &ScratchTable.pPhysAddr);

    if (rmStatus != RM_OK)
        return rmStatus;

    m_pHal->RegWrite(NV_PAPU_EPSADDR, (U032) ScratchTable.pPhysAddr);
    
    rmStatus = pScratchDma->Initialize(&ScratchTable);
    if (rmStatus != RM_OK)
        return rmStatus;
    
    // if everything is good till now.. initialize the base object
    CScratchManager::SetScratchDma(pScratchDma);
    //rmStatus = CScratchManager::Initialize(pLoader, uLoaderSize);

    return rmStatus;
}

// this initialize is for AC3 configuration
RM_STATUS   
CEPScratchManager::Initialize(CEPDspProgram *pProgram, CHalGlobalProc *pGlobalProc)
{
    // allocate the scratch Dma
    CScratchDma *pScratchDma = new CScratchDma(m_pHal, NV_PAPU_EPSMAXSGE);
    if (!pScratchDma)
        return RM_ERROR;
    
    m_pGlobalProc = pGlobalProc;
    m_pDspProgram = pProgram;

    // total pages required
    U032 uPagesRequired = (m_pDspProgram->AC3GetTotalScratchSize()+PAGE_SIZE-1) / PAGE_SIZE;

    // 256 frames 4 bytes per frame 6 channels = 1.5 pages so add 2 pages
    uPagesRequired += AC3_NUM_INPUT_PAGES;

    U032 uPagesAvail;

	// find out if HW supports the max indexes (index = pages - 1)
    m_pHal->RegWrite(NV_PAPU_EPSMAXSGE, LOW16(uPagesRequired - 1));
	m_pHal->RegRead(NV_PAPU_EPSMAXSGE, &uPagesAvail);

    if (uPagesAvail != LOW16(uPagesRequired - 1))
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CEPScratchManager::Initialize - PRDs not available for fixed effect\n");
        return RM_ERROR;
    }


    MEM_DESC ScratchTable;

    ScratchTable.Size = uPagesRequired * NV_PSGE_SIZE;

    // allocate the scratch table - PRDs for the max size
    RM_STATUS rmStatus = aosAllocateMem(ScratchTable.Size,
						                ALIGN_16K,
						                &ScratchTable.pLinAddr,
						                &ScratchTable.pPhysAddr);

    if (rmStatus != RM_OK)
        return rmStatus;

    m_pHal->RegWrite(NV_PAPU_EPSADDR, (U032) ScratchTable.pPhysAddr);
    
    rmStatus = pScratchDma->Initialize(&ScratchTable);
    if (rmStatus != RM_OK)
        return rmStatus;
    
    CScratchManager::SetScratchDma(pScratchDma);
    
    U032 uOffset;
    // allocate space for AC3...
    rmStatus = pScratchDma->Allocate(uPagesRequired * PAGE_SIZE, &uOffset);
    if (rmStatus != RM_OK)
        return rmStatus;
    
    // sanity check!
    if (uOffset)
        aosDbgBreakPoint();

    // now that we have the memory allocated
    VOID_PTR pCode;
    U032 uLength; // in words
    U032 uBase;   // in words
    
    // superexec
    m_pDspProgram->AC3GetSuperExec(&pCode, &uLength, &uBase);
    pScratchDma->Write(uBase*sizeof(U032), (VOID *)pCode, uLength*sizeof(U032));

    // loader
    m_pDspProgram->AC3GetLoader(&pCode, &uLength, &uBase);
    pScratchDma->Write(uBase*sizeof(U032), (VOID *)pCode, uLength*sizeof(U032));

    // loader table
    DOLBY_LOADER_TABLE LoaderTable;


    LoaderTable.tableSize = sizeof(DOLBY_LOADER_TABLE) / sizeof(U032);
    LoaderTable.maxProgs = m_pDspProgram->AC3GetMaxPrograms();

    // now copy the four Dolby encoder programs into system memory so the DSP can
    // DMA them when it needs them.  Keep track of the current offset into the
    // buffer, and update the loader table with the addresses as we go along.
    uBase = m_pDspProgram->AC3GetProgramBase();
    
    U032 uCnt;
    for (uCnt = 0; uCnt < m_pDspProgram->AC3GetMaxPrograms(); uCnt++)
    {
        // get program
        m_pDspProgram->AC3GetProgram(uCnt, &pCode, &uLength);

        // copy into scratch 
        pScratchDma->Write(uBase*sizeof(U032), (VOID *)pCode, uLength*sizeof(U032));

        // update the loader table
        LoaderTable.prog[uCnt].ptr  = uBase;
        LoaderTable.prog[uCnt].size = uLength;

        // update the current scratch index
        uBase += uLength;
    }

    // now the config table
    U032 *pConfig = (U032 *)m_pDspProgram->AC3GetConfigTable();
     // Note: the AC3 config table is actually a DOLBY_CONFIG_TABLE,
     // not a U032...  The size is contained in the first word of the
     // table, so we will treat it here as a U032 array.
    uLength = pConfig[0];
    LoaderTable.config_ptr  = uBase;
    LoaderTable.config_size = uLength;
    pScratchDma->Write(uBase*sizeof(U032), (VOID *)pConfig, uLength*sizeof(U032));
    uBase += uLength;
    
    //Init pingpong buffer offset
    LoaderTable.pingpong_offset = 0;
    LoaderTable.reserved1 = 0;
    LoaderTable.reserved2 = 0;
    LoaderTable.reserved3 = 0;
    
    // now for the heap
    uLength = m_pDspProgram->AC3GetHeapLength();
    LoaderTable.heap_ptr  = uBase;
    LoaderTable.heap_size = uLength;
    uBase += uLength;
    
    // copy the table to the offset
    pScratchDma->Write(sizeof(U032)*m_pDspProgram->AC3GetLoaderTableBase(), (VOID *)&LoaderTable, sizeof(DOLBY_LOADER_TABLE));

    return AC3StartGpInput((uPagesRequired - AC3_NUM_INPUT_PAGES) * PAGE_SIZE);
}



RM_STATUS
CEPScratchManager::AC3StartGpInput(U032 uLinOffset)
{
    if (!m_pGlobalProc)
        return RM_ERROR;

    // use the last n pages for ac-3 input from the GP
    MEM_DESC mem[AC3_NUM_INPUT_PAGES];
    
    U032 uOffset = m_pDspProgram->AC3GetLoaderTableBase() * sizeof(U032);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(pcm_ptr), uLinOffset/sizeof(U032));  
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(pcm_size), AC3_FRAME_SIZE * AC3_NUM_CHANNELS);

    // program the same pages in the GP
    for (U032 uCnt = 0; uCnt < AC3_NUM_INPUT_PAGES; uCnt++)
    {
        mem[uCnt].pLinAddr = GetScratchDma()->GetLinAddr(uLinOffset);
        mem[uCnt].pPhysAddr = GetScratchDma()->GetPhysAddr(uLinOffset);
        mem[uCnt].Size = PAGE_SIZE;
        uLinOffset += mem[uCnt].Size;
    }
    
    return m_pGlobalProc->AC3SetOutputBuffer(mem, AC3_NUM_INPUT_PAGES);
}

VOID
CEPScratchManager::AC3StopGpInput()
{
    if (!m_pGlobalProc)
        return;

    MEM_DESC mem[AC3_NUM_INPUT_PAGES];

    // update the loader table
    U032 uOffset = GetScratchDma()->Get( (m_pDspProgram->AC3GetLoaderTableBase()* sizeof(U032)) + DOLBY_LOADER_TABLE_OFFSET(pcm_ptr));

    // program the same pages in the GP
    for (U032 uCnt = 0; uCnt < AC3_NUM_INPUT_PAGES; uCnt++)
    {
        mem[uCnt].pLinAddr = GetScratchDma()->GetLinAddr(uOffset*sizeof(U032));
        mem[uCnt].pPhysAddr = GetScratchDma()->GetPhysAddr(uOffset*sizeof(U032));
        mem[uCnt].Size = PAGE_SIZE;
        uOffset += mem[uCnt].Size;
    }    
    m_pGlobalProc->AC3RemoveOutputBuffer(mem, AC3_NUM_INPUT_PAGES);
}

RM_STATUS
CEPScratchManager::AC3SetAnalogOutput(U032 uAnalogFIFO, U032 uSize)
{
    if (!m_pGlobalProc)
        return RM_ERROR;

    U032 uOffset = m_pDspProgram->AC3GetLoaderTableBase() * sizeof(U032);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ltrt_ptr), uAnalogFIFO);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ltrt_size), uSize/sizeof(U032));

    return RM_OK;
}

RM_STATUS
CEPScratchManager::AC3SetDigitalOutput(U032 uDigitalFIFO, U032 uSize)
{
    if (!m_pGlobalProc)
        return RM_ERROR;

    U032 uOffset = m_pDspProgram->AC3GetLoaderTableBase() * sizeof(U032);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_ptr), uDigitalFIFO);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_zero_fill), uDigitalFIFO);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_preamble), uDigitalFIFO);
    GetScratchDma()->Put(uOffset + DOLBY_LOADER_TABLE_OFFSET(ac3_size), uSize/sizeof(U032));

    return RM_OK;
}
