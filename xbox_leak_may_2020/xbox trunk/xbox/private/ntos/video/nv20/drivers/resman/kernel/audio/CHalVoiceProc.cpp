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

/************************VP class for the HAL******************************\
*                                                                           *
* Module: CHalVoiceProc.cpp                                                 *
*   All functions common to diff HALs are implemented here			        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <AudioHw.h>
#include <nvarm.h>
#include <nv32.h>
#include <nv_papu.h>
#include <aos.h>
};

#include "CHalBase.h"
#include "CVPMethod.h"
#include "CObjBase.h"
#include "CHalVoiceProc.h"


RM_STATUS
CHalVoiceProc::Allocate(VOID *pParam)
{
	APU_AP_CREATE *pArg = (APU_AP_CREATE *)pParam;
	RM_STATUS rmStatus = RM_ERROR;
	
    do
    {
        // create the method class
        m_pMethod = new CVPMethod(this, m_pGlobalProc, GetDevice());
        if (!m_pMethod)
            break;
        
        rmStatus = GetCaps(pArg);
        if (rmStatus != RM_OK)
            break;

        rmStatus = AllocateMemory(pArg);
        if (rmStatus != RM_OK)
            break;

	    // this is for debug only!
        pArg->pVoiceList = (VOID_PTR)m_Voice.pLinAddr;
       
        InitializeHardware(pArg);

    } while (FALSE);

	return rmStatus;
}

RM_STATUS
CHalVoiceProc::Free(VOID *pParam)
{
	RM_STATUS rmStatus = RM_OK;
	
    if (m_pMethod)
        delete m_pMethod;

    m_pMethod = NULL;

	FreeMemory();

	return rmStatus;
}

VOID
CHalVoiceProc::ExecuteMethod(U032 uMethod, U032 uParam)
{
    if (m_pMethod)
        m_pMethod->Execute(uMethod, uParam);
}

VOID
CHalVoiceProc::FreeMemory()
{
	aosFreeMem(m_Voice.pLinAddr);
	aosFreeMem(m_SgeIn.pLinAddr);
	aosFreeMem(m_SegIn.pLinAddr);
	aosFreeMem(m_HRTFTarg.pLinAddr);
	aosFreeMem(m_HRTFCurr.pLinAddr);
}

RM_STATUS
CHalVoiceProc::AllocateMemory(APU_AP_CREATE *pArg)
{
	RM_STATUS rmStatus;

    U032 uCnt;

	m_Voice.Size = pArg->uVoiceCount * NV_PAVS_SIZE;
    m_SgeIn.Size = pArg->uSGEInCount * NV_PSGE_SIZE;
    // the driver will pass the total number of PRDs to be allocated for streams
    m_SegIn.Size = pArg->uSegInCount * NV_PSGE_SIZE;
    m_HRTFTarg.Size = pArg->uHRTFCount * NV_PAHRTFT_SIZE;
    m_HRTFCurr.Size = pArg->uVoice3dCount * NV_PAHRTFC_SIZE;
	
	rmStatus = aosAllocateMem(m_Voice.Size,
							ALIGN_32K,
							&m_Voice.pLinAddr,
							&m_Voice.pPhysAddr);
	if (rmStatus == RM_OK)
	{
        rmStatus = aosAllocateMem(m_SgeIn.Size,
								ALIGN_16K,
								&m_SgeIn.pLinAddr,
								&m_SgeIn.pPhysAddr);

	}
	

	if (rmStatus == RM_OK)
	{

		rmStatus = aosAllocateMem(m_SegIn.Size,
								ALIGN_16K,
								&m_SegIn.pLinAddr,
								&m_SegIn.pPhysAddr);

	}


	if (rmStatus == RM_OK)
	{

		rmStatus = aosAllocateMem(m_HRTFTarg.Size,
								ALIGN_16K,
								&m_HRTFTarg.pLinAddr,
								&m_HRTFTarg.pPhysAddr);

	}

	if (rmStatus == RM_OK)
	{
		rmStatus = aosAllocateMem(m_HRTFCurr.Size,
								ALIGN_16K,
								&m_HRTFCurr.pLinAddr,
								&m_HRTFCurr.pPhysAddr);

	}

    // initialize the voice structures
    if (rmStatus == RM_OK)
    {
        U032 uAddr;
        U016 uNxtVoice;

        for (uCnt = 0; uCnt < pArg->uVoiceCount; uCnt++) 
        {
            uAddr = U032(m_Voice.pLinAddr) + (uCnt * NV_PAVS_SIZE);

            aosZeroMemory((VOID *)uAddr, NV_PAVS_SIZE);

            uNxtVoice = (U016)REF_NUM(NV_PAVS_VOICE_TAR_PITCH_LINK_NEXT_VOICE_HANDLE, uCnt);

            MemWrite(uAddr + NV_PAVS_VOICE_TAR_PITCH_LINK, uNxtVoice);
        }
        
        aosZeroMemory(m_SgeIn.pLinAddr, m_SgeIn.Size);
        aosZeroMemory(m_SegIn.pLinAddr, m_SegIn.Size);
        
    }

	return rmStatus;
}


RM_STATUS
CHalVoiceProc::GetCaps(VOID *pParam)
{
	APU_AP_CREATE *pArg = (APU_AP_CREATE *)pParam;
	VOLU32 vTemp;

    if (pArg->uVoiceCount)
    {
	    vTemp = pArg->uVoiceCount - 1;
	    RegWrite(NV_PAPU_FEMAXV, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXV, &vTemp);
	    pArg->uVoiceCount = vTemp + 1;
    }

    if (pArg->uVoice3dCount)
    {
	    vTemp = pArg->uVoice3dCount - 1;
	    RegWrite(NV_PAPU_FEMAXTV, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXTV, &vTemp);
	    pArg->uVoice3dCount = vTemp + 1;
    }

	if (pArg->uHRTFCount)
    {
        vTemp = pArg->uHRTFCount - 1;
	    RegWrite(NV_PAPU_FEMAXHT, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXHT, &vTemp);
	    pArg->uHRTFCount = vTemp + 1;
    }


	if (pArg->uSegInCount)
    {
        vTemp = pArg->uSegInCount - 1;
	    RegWrite(NV_PAPU_FEMAXSESSL, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXSESSL, &vTemp);
	    pArg->uSegInCount = vTemp + 1;
    }

	if (pArg->uSGEInCount)
    {
        vTemp = pArg->uSGEInCount - 1;
	    RegWrite(NV_PAPU_FEMAXSESGE, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXSESGE, &vTemp);
	    pArg->uSGEInCount = vTemp + 1;
    }
    
    if (pArg->uSubmixCount)
    {
	    // submixes
        vTemp = pArg->uSubmixCount - 1;
	    RegWrite(NV_PAPU_FEMAXMB, LOW16(vTemp));
	    RegRead(NV_PAPU_FEMAXMB, &vTemp);
	    pArg->uSubmixCount = vTemp + 1;
    }
    
    pArg->limitFifo = NV_PAPU_FEUFIFOMETH__SIZE_1;

	return RM_OK;
}

VOID
CHalVoiceProc::InitializeHardware(APU_AP_CREATE *pArg)
{
    // program the instance memory
    RegWrite(NV_PAPU_VPVADDR, (U032)m_Voice.pPhysAddr);
	RegWrite(NV_PAPU_VPHTADDR, (U032)m_HRTFTarg.pPhysAddr);
	RegWrite(NV_PAPU_VPHCADDR, (U032)m_HRTFCurr.pPhysAddr);
	RegWrite(NV_PAPU_VPSGEADDR, (U032)m_SgeIn.pPhysAddr);
	RegWrite(NV_PAPU_VPSSLADDR, (U032)m_SegIn.pPhysAddr);
}

VOID
CHalVoiceProc::WriteNotifier(U032 uMethod, U008	uErrorCode, BOOL uIsCurrentVoice)		
{
    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: CHalVoiceProc::WriteNotifier - Writing to notification memory\n");

    if (m_pNotifier)
    {
        U032 uIndex = 0;

        // TRUE - pick up from FECV, else write to [0]
        if (uIsCurrentVoice)
        {
            // get index
            RegRead(NV_PAPU_FECV, &uIndex);
            uIndex = (2*uIndex) + 2;
        }
        
        m_pNotifier[uIndex]->GSCNT = 0xA1;             // just my signature
        m_pNotifier[uIndex]->CurrentOffset = uMethod; 
        m_pNotifier[uIndex]->Zero = 0;
        m_pNotifier[uIndex]->Res0 = 0;
        m_pNotifier[uIndex]->SamplesAvailable = 0;
        m_pNotifier[uIndex]->EnvelopeActive = 0;
        m_pNotifier[uIndex]->Status = uErrorCode;
    }
    else
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CHalMCP1::WriteError - Notification memory not available\n");
    }

    aosDbgPrintString(DEBUGLEVEL_TRACEINFO, "NVARM: CHalMCP1::WriteError - Done\n");
}

