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

/************************GP class for the HAL******************************\
*                                                                           *
* Module: CHalGlobalProc.cpp                                                *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
extern "C"
{
#include <nvarm.h>
#include <AudioHw.h>
#include <nv_papu.h>
#include <nv_ugp.h>
#include <aos.h>
#include <nv32.h>
};

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalGlobalProc.h"
#include "CScratchManager.h"
#include "CGPScratchManager.h"
#include "CDspProgram.h"
#include "CGPDspProgram.h"
#include "CGPMethod.h"

CHalGlobalProc::CHalGlobalProc(CHalBase *pHal) : CObjBase(pHal)
{
    m_pNotifier = NULL;
    m_uNotifierSize = 0;
    m_pScratchManager = NULL;
    m_pDspProgram = NULL;
    m_pMethod = NULL;
}

RM_STATUS
CHalGlobalProc::Allocate(VOID *pParam)
{
	APU_AP_CREATE *pArg = (APU_AP_CREATE *)pParam;
	RM_STATUS rmStatus = RM_OK;
    
    do
    {
        // allocate all the objects required for GP

        m_pScratchManager = new CGPScratchManager(GetDevice());
        m_pDspProgram = new CGPDspProgram();
        m_pMethod = new CGPMethod(this, m_pScratchManager, m_pDspProgram, GetDevice());

        if (!m_pDspProgram || !m_pDspProgram || !m_pMethod)
            break;

        m_pDspProgram->Initialize();

        rmStatus = m_pScratchManager->Initialize( m_pDspProgram->GetLoader(),
                                                  m_pDspProgram->GetLoaderSize() );
        if (rmStatus != RM_OK)
            break;

        rmStatus = m_pMethod->Initialize();
        if (rmStatus != RM_OK)
            break;

        // get the device capabilities
        if (pArg->uSGEOutCount)
        {
	        VOLU32 vTemp = pArg->uSGEOutCount - 1;
	        RegWrite(NV_PAPU_FEMAXGPSGE, LOW16(vTemp));
	        RegRead(NV_PAPU_FEMAXGPSGE, &vTemp);
	        pArg->uSGEOutCount = vTemp + 1;
        }

        m_SgeOut.Size = pArg->uSGEOutCount * NV_PSGE_SIZE;

	    rmStatus =  aosAllocateMem(m_SgeOut.Size,
						                    ALIGN_16K,
						                    &m_SgeOut.pLinAddr,
						                    &m_SgeOut.pPhysAddr);
        if (rmStatus == RM_OK)
        {
            // clear any pending interrupts! - todo cleanup
            RegWrite(NV_PAPU_GPISTS, (U032)0xFFFFFFFF);
            // output buffer PRDs
            RegWrite(NV_PAPU_GPFADDR, (U032)m_SgeOut.pPhysAddr);
            // update the max fifo PRDs req
            RegWrite(NV_PAPU_GPFMAXSGE, (U016)(m_SgeOut.Size / NV_PSGE_SIZE) );

            // return the linear address of the command Q to the client
            pArg->pGPCommandQ = (VOID_PTR)m_pMethod->GetCommandQAddress();
        }

    } while (FALSE);

	return rmStatus;
}

RM_STATUS
CHalGlobalProc::Free(VOID *pParam)
{
    if (m_pMethod)
        delete m_pMethod;

    if (m_pDspProgram)
        delete m_pDspProgram;

    if (m_pScratchManager)
        delete m_pScratchManager;

	aosFreeMem(m_SgeOut.pLinAddr);
    m_SgeOut.pLinAddr = NULL;

    m_pMethod = NULL;
    m_pDspProgram = NULL;
    m_pScratchManager = NULL;

    return RM_OK;
}



VOID
CHalGlobalProc::WriteCommand(U032 uOffset, DSP_CONTROL *pDspWord, WORD4 *pW4, WORD5 *pW5, WORD6 *pW6)
{
    RegWrite(NV_PAPU_GPXMEM(uOffset++), pDspWord->w0.uValue);
    RegWrite(NV_PAPU_GPXMEM(uOffset++), pDspWord->w1.uValue);
    RegWrite(NV_PAPU_GPXMEM(uOffset++), pDspWord->w2.uValue);
    RegWrite(NV_PAPU_GPXMEM(uOffset++), pDspWord->w3.uValue);
    
    if (pW4)
        RegWrite(NV_PAPU_GPXMEM(uOffset++), pW4->uValue);

    if (pW5)
        RegWrite(NV_PAPU_GPXMEM(uOffset++), pW5->uValue);

    if (pW6)
        RegWrite(NV_PAPU_GPXMEM(uOffset++), pW6->uValue);
}

VOID
CHalGlobalProc::WriteNotifier(U032 uMethod, U032 uData, U008 uStatus)
{
    if (!m_pNotifier)
    {
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CHalGlobalProc::WriteNotifier - Memory not set\n");
        return;
    }
    
    // calculate the notifier memory address for the method
    U032 uOffset = sizeof(NVAUDGPNOTIFICATION) * uMethod;

    // check for boundary
    if (uOffset < m_uNotifierSize)
    {
        // great.. now write the notifier
        PNVAUDGPNOTIFICATION pNotify = (PNVAUDGPNOTIFICATION)(m_pNotifier + uOffset);
    
        pNotify->GSCNT = 0;     // todo vik
        pNotify->Method = uMethod;
        pNotify->Data = uData;
        pNotify->ReturnCode = uStatus;
        pNotify->Status = NV_AUDGP_NOTIFICATION_STATUS_DONE_SUCCESS;
    }
    else
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CHalGlobalProc::WriteNotifier - boundary check fails!\n");
}

VOID
CHalGlobalProc::ExecuteMethod() 
{ 
    if (m_pMethod)
        m_pMethod->Execute();
    else
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CHalGlobalProc::ExecuteMethod - no method class!\n");
}

RM_STATUS 
CHalGlobalProc::AC3SetOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages)
{
    RM_STATUS rmStatus = RM_ERROR;

    if (m_pScratchManager)
        rmStatus = m_pScratchManager->AC3SetOutputBuffer(pMem, uNumOfPages);

    return rmStatus;
}

VOID 
CHalGlobalProc::AC3RemoveOutputBuffer(PMEM_DESC pMem, U032 uNumOfPages)
{
    if (m_pScratchManager)
       m_pScratchManager->AC3RemoveOutputBuffer(pMem, uNumOfPages);
}

U032
CHalGlobalProc::AC3GetBufferOffset()
{
    if (m_pScratchManager)
       return m_pScratchManager->AC3GetBufferOffset();

    return 0;

}
