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
* Module: CGPScratchManager.cpp                                              *
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
#include <aos.h>
#include <AudioHw.h>
#include <nv_papu.h>
};

#include "CHalBase.h"
#include "CScratchDma.h"
#include "CScratchManager.h"
#include "CGPScratchManager.h"

#define MAX_SIZE_SCRATCH_GP     0x200000            //2 Meg

CGPScratchManager::CGPScratchManager(CHalBase *pHal)
{
    m_pHal = pHal;
}

CGPScratchManager::~CGPScratchManager()
{
    CScratchManager::Shutdown();
    
    CScratchDma *pScratchDma = GetScratchDma();
    
    if (pScratchDma)
    {
        VOID *pScratchTable = pScratchDma->GetScratchTableLinAddr();
        aosFreeMem(pScratchTable);
        delete pScratchDma;
    }
    
    SetScratchDma(NULL);
}


RM_STATUS   
CGPScratchManager::Initialize(VOID_PTR pLoader, U032 uLoaderSize)
{
    // allocate the scratch Dma
    CScratchDma *pScratchDma = new CScratchDma(m_pHal, NV_PAPU_GPSMAXSGE);
    if (!pScratchDma)
        return RM_ERROR;

    // 	configure the HW
    VOLU32 vTemp = (MAX_SIZE_SCRATCH_GP/PAGE_SIZE);
	m_pHal->RegWrite(NV_PAPU_GPSMAXSGE, LOW16(vTemp));
	m_pHal->RegRead(NV_PAPU_GPSMAXSGE, &vTemp);

    MEM_DESC ScratchTable;

    ScratchTable.Size = (vTemp + 1)*NV_PSGE_SIZE;

    // allocate the scratch table - PRDs for the max size
    RM_STATUS rmStatus = aosAllocateMem(ScratchTable.Size,
						                ALIGN_16K,
						                &ScratchTable.pLinAddr,
						                &ScratchTable.pPhysAddr);

    if (rmStatus != RM_OK)
        return rmStatus;

    m_pHal->RegWrite(NV_PAPU_GPSADDR, (U032) ScratchTable.pPhysAddr);
    
    rmStatus = pScratchDma->Initialize(&ScratchTable);
    if (rmStatus != RM_OK)
        return rmStatus;

    // if everything is good till now.. initialize the base object
    SetScratchDma(pScratchDma);
    //rmStatus = CScratchManager::Initialize(pLoader, uLoaderSize);

    //HACK allocate a bunch of Scratch for effects until we can allocate correctly
    if (rmStatus != RM_OK)
        return rmStatus;

    U032 Offset;
    rmStatus = pScratchDma->Allocate(0x100000,&Offset);

    return rmStatus;
}

VOID
CGPScratchManager::AC3RemoveOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages)
{
    // stop the GP from pumping data... TODO
    /*
    AC3_SET_BUFFER rSetBuffer;

    rSetBuffer.data1.uVal = 0;
    rSetBuffer.data1.fields.oper = NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_DISABLE;
    rSetBuffer.data1.fields.size = uNumOfPages * PAGE_SIZE;
    rSetBuffer.offset = m_uAC3BufferOffset;

    // write to the output Q
    CScratchQ *pQ = GetOutputQ();
    pQ->Start();
    pQ->Write(NV_OGP_SET_AC3_OUTPUT_BUFFER, rSetBuffer.data1.uVal, rSetBuffer.offset);
    pQ->Stop();
    */

    CScratchDma *pDma = GetScratchDma();
    pDma->RemovePage(pMem, uNumOfPages);
}

RM_STATUS
CGPScratchManager::AC3SetOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages)
{
    // AC3_SET_BUFFER rSetBuffer;

    CScratchDma *pDma = GetScratchDma();

    // we need to add these pages to the GP scratch
    RM_STATUS rmStatus = pDma->AddPage(pMem, uNumOfPages, &m_uAC3BufferOffset);
    if (rmStatus == RM_OK)
    {
        // write a method to the DSP telling him the offset of the AC3 output buffer - TODO!
        /*
        rSetBuffer.data1.uVal = 0;
        rSetBuffer.data1.fields.oper = NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_ENABLE;
        rSetBuffer.data1.fields.size = uNumOfPages * PAGE_SIZE;
        rSetBuffer.offset = m_uAC3BufferOffset;

        // write to the output Q
        CScratchQ *pQ = GetOutputQ();
        pQ->Start();
        if (FALSE == pQ->Write(NV_OGP_SET_AC3_OUTPUT_BUFFER, rSetBuffer.data1.uVal, rSetBuffer.offset))
            rmStatus = RM_ERROR;
        pQ->Stop();
        */
    }

    return rmStatus;
}