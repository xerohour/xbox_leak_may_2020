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

/***************************** Object Manager *****************************\
*                                                                           *
* Module: CEPMethod.cpp                                                     *
*     Object Classes are defined in this module.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

extern "C"
{
#include <nvarm.h>
#include <nv_uep.h>
#include <nv_papu.h>
#include <nv32.h>
#include <AudioHw.h>
};
#include <aos.h>
#include <ahal.h>

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalMCP1.h"
#include "CHalExtendedProc.h"
#include <CLinkList.h>
#include "CContextDma.h"
#include "CEPMethod.h"

RM_STATUS
CEPMethod::Execute(U032 uOffset, U032 uData)
{
    RM_STATUS rmStatus = RM_ERROR;

    do
    {
        if (CheckRange(uOffset, NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE))
        {
            rmStatus = SetCurrentSge(uData);
            break;
        }

        if (CheckRange(uOffset, NV1BAE_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA))
        {
            rmStatus = SetContextDMA(uData);
            break;
        }
        
        if (CheckRange(uOffset, NV1BAE_PIO_SET_STATE))
        {
            rmStatus = SetState(uData);
            break;
        }

        if (CheckRange(uOffset, NV1BAE_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET))
        {
            rmStatus = SetSgeOffset(uData);
            break;
        }
        
        U032 uCnt = 0;

        for (uCnt = 0; uCnt < 4; uCnt++)
        {
            if ( CheckRange(uOffset, (NV1BAE_PIO_SET_OUTBUF_BA(uCnt))) )
            {
                rmStatus = SetSgeBaseAddr(uData, uCnt);
                break;
            }
        }
        
        if (uCnt != 4)
            break;

        for (uCnt = 0; uCnt < 4; uCnt++)
        {
            if ( CheckRange(uOffset, (NV1BAE_PIO_SET_OUTBUF_LEN(uCnt))) )
            {
                rmStatus = SetSgeLength(uData, uCnt);
                break;
            }
        }

        if (uCnt != 4)
            break;

        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CEPMethod::Execute - SW method not yet implemented!\n");

#if defined WDM
        KdPrint(("Method offset = 0x%x Method Data = 0x%x\n", uOffset, uData));
#endif

    } while (FALSE);
    
    return rmStatus;
}

RM_STATUS
CEPMethod::SetCurrentSge(U032 uData)
{
    // HW indexes are not 0 based
    if (!uData)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CEPMethod::SetCurrentSge - SGE index cannot be 0\n");
        // bad argument
        return RM_ERROR;
    }
    
    m_uCurrSge = uData;

    return RM_OK;
}


RM_STATUS
CEPMethod::SetContextDMA(U032 uData)
{
    CContextDma *pDma = (CContextDma *)uData;

    if (((U032)(pDma->GetAddress()) & 0xFFF) ||
        (pDma->GetSize() & 0xFFF))
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CEPMethod::SetContextDMA - Address not aligned on 4K boundary\n");
        return RM_ERROR;
    }
    
    m_uBaseAddr = (U032)pDma->GetAddress();
    m_uMaxOffset = (U032)pDma->GetSize();

    return RM_OK;
}

RM_STATUS
CEPMethod::SetSgeOffset(U032 uData)
{
    if (!m_uMaxOffset)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CEPMethod::SetSgeOffset - context DMA is not valid\n");
        // context DMA not valid
        return RM_ERROR;
    }

    if (uData > m_uMaxOffset)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CEPMethod::SetSgeOffset - exceeds max offset\n");
        // bad argument
        return RM_ERROR;
    }

    
    MCP1_PRD *pPrd = (MCP1_PRD *)(m_pHal->GetExtendedProc()->GetSgeOutLinAddr());

    pPrd[m_uCurrSge-1].uAddr = m_uBaseAddr + uData;
    pPrd[m_uCurrSge-1].Control.uValue = 0;
    pPrd[m_uCurrSge-1].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;

    return RM_OK;
}

RM_STATUS
CEPMethod::SetSgeBaseAddr(U032 uData, U032 uIndex)
{
    U032 uOffBase;
    U032 uOffEnd;
    
    GetBaseAndEnd(uIndex, &uOffBase, &uOffEnd);

    m_pHal->RegWrite(uOffBase, uData);
    m_pHal->RegWrite(uOffEnd, uData);

    return RM_OK;
}

RM_STATUS
CEPMethod::SetSgeLength(U032 uData, U032 uIndex)
{
    U032 uOffBase;
    U032 uOffEnd;

    GetBaseAndEnd(uIndex, &uOffBase, &uOffEnd);
    
    U032 uBaseAddr;
    m_pHal->RegRead(uOffBase, &uBaseAddr);

    m_pHal->RegWrite(uOffEnd, uBaseAddr + uData);
    
    return RM_OK;
}

RM_STATUS 
CEPMethod::SetState(U032 uData)
{
    // vik - todo
    return RM_ERROR;
}

// utility functions
VOID
CEPMethod::GetBaseAndEnd(U032 uIndex, U032 *pBase, U032 *pEnd)
{
    switch(uIndex)
    {
    case 0:
        *pBase = NV_PAPU_EPOFBASE0;
        *pEnd = NV_PAPU_EPOFEND0;
        break;
    case 1:
        *pBase = NV_PAPU_EPOFBASE1;
        *pEnd = NV_PAPU_EPOFEND1;
        break;
    case 2:
        *pBase = NV_PAPU_EPOFBASE2;
        *pEnd = NV_PAPU_EPOFEND2;
        break;
    case 3:
        *pBase = NV_PAPU_EPOFBASE3;
        *pEnd = NV_PAPU_EPOFEND3;
        break;
    }
}

