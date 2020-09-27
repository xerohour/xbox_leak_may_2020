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
#include <aos.h>
#include <nv32.h>
};

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalGlobalProc.h"

CHalGlobalProc::CHalGlobalProc(CHalBase *pHal) : CObjBase(pHal)
{
    m_CommandQ.Size = 4096;
}

RM_STATUS
CHalGlobalProc::Allocate(VOID *pParam)
{
	APU_AP_CREATE *pArg = (APU_AP_CREATE *)pParam;
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
CHalGlobalProc::Free(VOID *pParam)
{
    FreeMemory();
    return RM_OK;
}

RM_STATUS
CHalGlobalProc::GetCaps(APU_AP_CREATE *pArg)
{
    VOLU32 vTemp;

    // output from GP
    if (pArg->uSGEOutCount)
    {
	    vTemp = pArg->uSGEOutCount - 1;
	    RegWrite(NV_PAPU_FEMAXGPSGE, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXGPSGE, &vTemp);
	    pArg->uSGEOutCount = vTemp + 1;
    }
    
    if (pArg->uGPScratch)
    {
	    vTemp = pArg->uGPScratch - 1;
	    RegWrite(NV_PAPU_GPSMAXSGE, LOW16(vTemp));
	    RegRead(NV_PAPU_GPSMAXSGE, &vTemp);
	    pArg->uGPScratch = vTemp + 1;
    }

    return RM_OK;
}

RM_STATUS
CHalGlobalProc::AllocateMemory(APU_AP_CREATE *pArg)
{
    m_SgeOut.Size = pArg->uSGEOutCount * NV_PSGE_SIZE;
    m_Scratch.Size = pArg->uGPScratch * NV_PSGE_SIZE;

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
CHalGlobalProc::FreeMemory()
{
	aosFreeMem(m_SgeOut.pLinAddr);
	aosFreeMem(m_CommandQ.pLinAddr);
	aosFreeMem(m_Scratch.pLinAddr);
}

VOID
CHalGlobalProc::InitializeHardware()
{
    // clear any pending interrupts! - todo cleanup
    RegWrite(NV_PAPU_GPISTS, (U032)0xFFFFFFFF);
    
    /*
    // set up the Q ptrs .. JLT
    RegWrite(NV_PAPU_GPOFBASE0, (U032)0);
    RegWrite(NV_PAPU_GPOFEND0, m_SgeOut.Size);
    RegWrite(NV_PAPU_GPOFCUR0, (U032)0);

    RegWrite(NV_PAPU_GPOFBASE1, (U032)0);
    RegWrite(NV_PAPU_GPOFEND1, m_SgeOut.Size);
    RegWrite(NV_PAPU_GPOFCUR1, (U032)0);

    RegWrite(NV_PAPU_GPOFBASE2, (U032)0);
    RegWrite(NV_PAPU_GPOFEND2, m_SgeOut.Size);
    RegWrite(NV_PAPU_GPOFCUR2, (U032)0);

    RegWrite(NV_PAPU_GPOFBASE3, (U032)0);
    RegWrite(NV_PAPU_GPOFEND3, m_SgeOut.Size);
    RegWrite(NV_PAPU_GPOFCUR3, (U032)0);
    */

    // output buffer PRDs
    RegWrite(NV_PAPU_GPFADDR, (U032)m_SgeOut.pPhysAddr);

    // scratch area for the GP
    RegWrite(NV_PAPU_GPSADDR, (U032)m_Scratch.pPhysAddr);

    // PRDs for the FIFO
    RegWrite(NV_PAPU_GPCADDR, (U032)m_CommandQ.pPhysAddr);
}

VOID
CHalGlobalProc::WriteCommand(DSP_CONTROL *pDspWord, WORD4 *pW4, WORD5 *pW5, WORD6 *pW6)
{
    RegWrite(NV_PAPU_GPXMEM(0), pDspWord->w0.uValue);
    RegWrite(NV_PAPU_GPXMEM(1), pDspWord->w1.uValue);
    RegWrite(NV_PAPU_GPXMEM(2), pDspWord->w2.uValue);
    RegWrite(NV_PAPU_GPXMEM(3), pDspWord->w3.uValue);
    
    if (pW4)
        RegWrite(NV_PAPU_GPXMEM(4), pW4->uValue);

    if (pW5)
        RegWrite(NV_PAPU_GPXMEM(5), pW5->uValue);

    if (pW6)
        RegWrite(NV_PAPU_GPXMEM(6), pW6->uValue);
}
