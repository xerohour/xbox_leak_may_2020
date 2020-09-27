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
#include "CHalExtendedProc.h"
#include <CLinkList.h>
#include "CContextDma.h"
#include "CScratchManager.h"
#include "CEPScratchManager.h"
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

        if (CheckRange(uOffset, NV1BAE_PIO_SET_AC3_FIFO))
        {
            rmStatus = SetAC3Fifo(uData);
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
    // todo vik - check against the max PRDs allocated on EP init

    // HW indexes are 0 based
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

    
    MCP1_PRD *pPrd = (MCP1_PRD *)(m_pProc->GetSgeOutLinAddr());

    pPrd[m_uCurrSge].uAddr = m_uBaseAddr + uData;
    pPrd[m_uCurrSge].Control.uValue = 0;
    pPrd[m_uCurrSge].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;

    return RM_OK;
}

RM_STATUS
CEPMethod::SetSgeBaseAddr(U032 uData, U032 uIndex)
{
    U032 uOffBase;
    U032 uOffCur;
    U032 uOffEnd;
    
    GetBaseCurEnd(uIndex, &uOffBase, &uOffCur, &uOffEnd);

    m_pHal->RegWrite(uOffBase, uData);
    m_pHal->RegWrite(uOffCur, uData);
    m_pHal->RegWrite(uOffEnd, uData+256);

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
    R_EP_RESET rReset;
    rReset.uValue = 0;
    R_GP_CONTROL rControl;
    rControl.uValue = 0;


    if (uData)
    {
        // take it out of reset...
        rReset.Global = NV_PAPU_EPRST_EPRST_DISABLED;
        rReset.DSP = NV_PAPU_EPRST_EPDSPRST_ENABLED;
        rReset.NMI = NV_PAPU_EPRST_EPNMI_DISABLED;
        rReset.Abort = NV_PAPU_EPRST_EPABORT_DISABLED;

        m_pHal->RegWrite(NV_PAPU_EPRST, rReset.uValue);


        // auto start and auto idle
        R_GPDMA_CONFIG rConfig;
        rConfig.uValue = 0;
        //rConfig.Start = NV_PAPU_EPDCFG_GPDASTRT_ENABLED;
        //rConfig.Ready = NV_PAPU_EPDCFG_GPDARDY_ENABLED;
        m_pHal->RegWrite(NV_PAPU_EPDCFG, rConfig.uValue);
        
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CEPMethod::SetEPState - the EP might just start...");
        // enable the EP

        rControl.Idle = NV_PAPU_EPIDRDY_EPSETIDLE_SET;
        rControl.IntrNotify = NV_PAPU_EPIDRDY_EPSETNINT_NOT_SET;
    }
    else
    {
        // stop the EP
        rReset.Global = NV_PAPU_EPRST_EPRST_DISABLED;
        rReset.DSP = NV_PAPU_EPRST_EPDSPRST_ENABLED;
        rReset.NMI = NV_PAPU_EPRST_EPNMI_DISABLED;
        rReset.Abort = NV_PAPU_EPRST_EPABORT_DISABLED;

        m_pHal->RegWrite(NV_PAPU_EPRST, rReset.uValue);
    }

    m_pHal->RegWrite(NV_PAPU_EPIDRDY, rControl.uValue);
    
    // clear the GPISTS bit
    m_pHal->RegWrite(NV_PAPU_EPISTS, (U032)0xff);

    // reset just the DSP core...
    /*
    R_GP_RESET rReset;
    rReset.uValue = 0;
    rReset.Global = NV_PAPU_EPRST_EPRST_DISABLED;
    rReset.DSP = NV_PAPU_EPRST_EPDSPRST_DISABLED;
    rReset.NMI = NV_PAPU_EPRST_EPNMI_DISABLED;
    rReset.Abort = NV_PAPU_EPRST_EPABORT_DISABLED;
    // u need to do this only when the program is in and all that
    m_pHal->RegWrite(NV_PAPU_EPRST, rReset.uValue);
    */

    m_pHal->RegWrite(NV_PAPU_EPRST, uData);


    aosDbgPrintString(DEBUGLEVEL_ERRORS, "done.  Congratulations!\n");
    return RM_OK;
}

RM_STATUS 
CEPMethod::SetAC3Fifo(U032 uData)
{
    RM_STATUS rmStatus = RM_ERROR;
    
    union
    {
        struct
        {
            U032 analog : 3;
            U032 digital : 3;
            U032 res : 26;
        } fields;

        U032 uVal;
    } Data;

    Data.uVal = uData;
    
    // some error checking
    if ( (Data.fields.analog > 3) || (Data.fields.digital > 3) )
    {
        // bad argumnet
        return RM_ERROR;
    }

    U032 uBaseAddr, uEndAddr;
    U032 uBaseVal, uEndVal;

    // get the analog fifo index
    GetBaseAndEnd(Data.fields.analog, &uBaseAddr, &uEndAddr);

    m_pHal->RegRead(uBaseAddr, &uBaseVal);
    m_pHal->RegRead(uEndAddr, &uEndVal);
    
    // update the loader table
    rmStatus = m_pProc->GetEPScratchManager()->AC3SetAnalogOutput(Data.fields.analog, (uEndVal - uBaseVal));
    //rmStatus = m_pProc->GetEPScratchManager()->AC3SetAnalogOutput(uBaseVal, (uEndVal - uBaseVal));
    if (rmStatus != RM_OK)
        return rmStatus;

    // get the digital fifo index
    GetBaseAndEnd(Data.fields.digital, &uBaseAddr, &uEndAddr);

    m_pHal->RegRead(uBaseAddr, &uBaseVal);
    m_pHal->RegRead(uEndAddr, &uEndVal);
    
    // update the loader table
    return m_pProc->GetEPScratchManager()->AC3SetDigitalOutput(Data.fields.digital, (uEndVal - uBaseVal));
    //return m_pProc->GetEPScratchManager()->AC3SetDigitalOutput(uBaseVal, (uEndVal - uBaseVal));
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


// utility functions
VOID
CEPMethod::GetBaseCurEnd(U032 uIndex, U032 *pBase, U032 *pCur, U032 *pEnd)
{
    switch(uIndex)
    {
    case 0:
        *pBase = NV_PAPU_EPOFBASE0;
        *pCur = NV_PAPU_EPOFCUR0;
        *pEnd = NV_PAPU_EPOFEND0;
        break;
    case 1:
        *pBase = NV_PAPU_EPOFBASE1;
        *pCur = NV_PAPU_EPOFCUR1;
        *pEnd = NV_PAPU_EPOFEND1;
        break;
    case 2:
        *pBase = NV_PAPU_EPOFBASE2;
        *pCur = NV_PAPU_EPOFCUR2;
        *pEnd = NV_PAPU_EPOFEND2;
        break;
    case 3:
        *pBase = NV_PAPU_EPOFBASE3;
        *pCur = NV_PAPU_EPOFCUR3;
        *pEnd = NV_PAPU_EPOFEND3;
        break;
    }
}
