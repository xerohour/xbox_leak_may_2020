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
* Module: CHalExtendedProc.cpp                                                *
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
#include <nv32.h>
#include <aos.h>
};

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalExtendedProc.h"
#include "CHalGlobalProc.h"
#include "CScratchManager.h"
#include "CEPScratchManager.h"
#include "CDspProgram.h"
#include "CEPDspProgram.h"
#include "CEPMethod.h"

CHalExtendedProc::CHalExtendedProc(CHalBase *pHal, CHalGlobalProc *pGlProc) : CObjBase(pHal)
{
    // keep the internal count ahead of the external count by this much
    m_CommandQ.Size = 4096;
    m_pDspProgram = NULL;
    m_pScratchManager = NULL;
    m_pGlobalProc = pGlProc;
    m_pMethod = NULL;
}

RM_STATUS
CHalExtendedProc::Allocate(VOID *pParam)
{
	APU_EP_CREATE *pArg = (APU_EP_CREATE *)pParam;
	RM_STATUS rmStatus = RM_ERROR;
    
    do
    {
        m_pDspProgram = new CEPDspProgram;
        m_pScratchManager = new CEPScratchManager(GetDevice());
        m_pMethod = new CEPMethod(GetDevice(), this);

        if (!m_pDspProgram || !m_pScratchManager || !m_pMethod)
            break;

        if (pArg->uUseForAC3)
        {
            rmStatus = m_pScratchManager->Initialize(m_pDspProgram, m_pGlobalProc);
            pArg->uAC3GpOutputOffset = m_pGlobalProc->AC3GetBufferOffset();
        }
        else
        {
            // tbd 
            rmStatus = RM_ERROR;
        }

        if (rmStatus != RM_OK)
            break;

        rmStatus = GetCaps(pArg);
        if (rmStatus != RM_OK)
            break;

        rmStatus = AllocateMemory(pArg);
        if (rmStatus != RM_OK)
            break;

        InitializeHardware();

    } while (FALSE);

	return rmStatus;
}


RM_STATUS
CHalExtendedProc::Free(VOID *pParam)
{
    if (m_pMethod)
        delete m_pMethod;

    if (m_pDspProgram)
        delete m_pDspProgram;

    if (m_pScratchManager)
        delete m_pScratchManager;

    FreeMemory();

    m_pMethod = NULL;
    m_pDspProgram = NULL;
    m_pScratchManager = NULL;

    return RM_OK;
}

RM_STATUS
CHalExtendedProc::GetCaps(APU_EP_CREATE *pArg)
{
    // have no idea what regs to check against

    return RM_OK;
}

VOID
CHalExtendedProc::ExecuteMethod(U032 uMethod, U032 uParam)
{
    if (m_pMethod)
        m_pMethod->Execute(uMethod, uParam);
    else
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CHalExtendedProc::ExecuteMethod - not method object!\n");
    }
}

RM_STATUS
CHalExtendedProc::AllocateMemory(APU_EP_CREATE *pArg)
{
    // vik - todo
    m_SgeOut.Size = pArg->uSGEOutCount * NV_PSGE_SIZE;

	RM_STATUS rmStatus =  aosAllocateMem(m_SgeOut.Size,
						                ALIGN_16K,
						                &m_SgeOut.pLinAddr,
						                &m_SgeOut.pPhysAddr);

    if (rmStatus == RM_OK)
    {
        // allocate memory for the command Q
        rmStatus =  aosAllocateMem(m_CommandQ.Size,
						        ALIGN_16K,
						        &m_CommandQ.pLinAddr,
						        &m_CommandQ.pPhysAddr);
    }

    return rmStatus;
}

VOID
CHalExtendedProc::FreeMemory()
{
	aosFreeMem(m_SgeOut.pLinAddr);
	aosFreeMem(m_CommandQ.pLinAddr);
}

VOID
CHalExtendedProc::InitializeHardware()
{
    // clear any pending interrupts! - todo cleanup
    RegWrite(NV_PAPU_EPISTS, (U032)0xFFFFFFFF);
    
    // output buffer PRDs
    RegWrite(NV_PAPU_EPFADDR, (U032)m_SgeOut.pPhysAddr);

    // PRDs for the FIFO
    RegWrite(NV_PAPU_EPCADDR, (U032)m_CommandQ.pPhysAddr);

    // update the max fifo PRDs req
    RegWrite(NV_PAPU_EPFMAXSGE, (U016)(m_SgeOut.Size / NV_PSGE_SIZE) );
}
