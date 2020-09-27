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

CHalExtendedProc::CHalExtendedProc(CHalBase *pHal) : CObjBase(pHal)
{
    // keep the internal count ahead of the external count by this much
    m_CommandQ.Size = 4096;
}

RM_STATUS
CHalExtendedProc::Allocate(VOID *pParam)
{
	APU_EP_CREATE *pArg = (APU_EP_CREATE *)pParam;
	RM_STATUS rmStatus = RM_OK;
    
    do
    {
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
    FreeMemory();
    return RM_OK;
}

RM_STATUS
CHalExtendedProc::GetCaps(APU_EP_CREATE *pArg)
{
    // have no idea what regs to check against

    return RM_OK;
}

RM_STATUS
CHalExtendedProc::AllocateMemory(APU_EP_CREATE *pArg)
{
    // what about notifier memory for both EP & GP ???
    // vik - todo
    m_SgeOut.Size = pArg->uSGEOutCount * NV_PSGE_SIZE;
    m_Scratch.Size = pArg->uScratch * NV_PSGE_SIZE;

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
    
    // allocate memory for 1PRD to point to the 4K command Q

    if (rmStatus == RM_OK)
    {
        // allocate memory scratch area
        rmStatus =  aosAllocateMem(m_Scratch.Size,
						        ALIGN_16K,
						        &m_Scratch.pLinAddr,
						        &m_Scratch.pPhysAddr);
    }

    return rmStatus;
}

VOID
CHalExtendedProc::FreeMemory()
{
	aosFreeMem(m_SgeOut.pLinAddr);
	aosFreeMem(m_CommandQ.pLinAddr);
	aosFreeMem(m_Scratch.pLinAddr);
}

VOID
CHalExtendedProc::InitializeHardware()
{
    // clear any pending interrupts! - todo cleanup
    RegWrite(NV_PAPU_EPISTS, (U032)0xFFFFFFFF);
    
    // output buffer PRDs
    RegWrite(NV_PAPU_EPFADDR, (U032)m_SgeOut.pPhysAddr);

    // scratch area for the eP
    RegWrite(NV_PAPU_EPSADDR, (U032)m_Scratch.pPhysAddr);

    // PRDs for the FIFO
    RegWrite(NV_PAPU_EPCADDR, (U032)m_CommandQ.pPhysAddr);
}