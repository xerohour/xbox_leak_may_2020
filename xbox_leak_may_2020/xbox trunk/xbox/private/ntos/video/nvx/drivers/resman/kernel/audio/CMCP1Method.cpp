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
* Module: CMCP1Method.cpp                                                     *
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
#include <nv_uap.h>
#include <nv_papu.h>
#include <AudioHw.h>
#include <nv32.h>
};
#include <aos.h>
#include <ahal.h>

#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalMCP1.h"
#include "CHalGlobalProc.h"
#include "CHalVoiceProc.h"
#include <CLinkList.h>
#include "CContextDma.h"
#include "CMCP1Method.h"

RM_STATUS
CMCP1Method::Execute(U032 uOffset, U032 uData)
{
    RM_STATUS rmStatus = RM_ERROR;  // not implemented

    do
    {
        if (CheckRange(uOffset, NV1BA0_PIO_SET_CONTEXT_DMA_NOTIFY))
        {
            rmStatus = SetContextDMANotify(uData);
            break;
        }

        if (CheckRange(uOffset, NV1BA0_PIO_SET_CURRENT_SSL_CONTEXT_DMA))
        {
            rmStatus = SetCurrentSSLContextDMA(uData);
            break;
        }

        if (CheckRange(uOffset, NV1BA0_PIO_SET_CURRENT_INBUF_SGE_CONTEXT_DMA))
        {
            rmStatus = SetCurrentInBufSGEContextDMA(uData);
            break;
        }

        if (CheckRange(uOffset, NV1BA0_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA))
        {
            rmStatus = SetCurrentOutBufSGEContextDMA(uData);
            break;
        }
        
        if (CheckRange(uOffset, 0x4000))        // to be moved to the .ref file.. todo vik
        {
            rmStatus = SetGPState(uData);
            break;
        }

        if (CheckRange(uOffset, 0x4004))        // debug...
        {
            rmStatus = SetCurrentOutBufSGEOffset(uData);
            break;
        }
        
        if (CheckRange(uOffset, 0x4008))        // debug...
        {
            rmStatus = SetCurrentInBufSGEOffset(uData);
            break;
        }
        
        U032 uCnt = 0;

        for (uCnt = 0; uCnt < 4; uCnt++)
        {
            if (CheckRange(uOffset, (0x420C + (uCnt*8))) )
            {
                rmStatus = SetOutBufBA(uData, uCnt);
                break;
            }
        }
        
        if (uCnt != 4)
            break;

        for (uCnt = 0; uCnt < 4; uCnt++)
        {
            if (CheckRange(uOffset, (0x4210 + (uCnt*8))) )
            {
                rmStatus = SetOutBufLen(uData, uCnt);
                break;
            }
        }

        if (uCnt != 4)
            break;

        // find out if NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET got triggered
        for (uCnt = 0; uCnt < 64; uCnt++)
        {
            if (CheckRange(uOffset, (0x400C + (uCnt*8))) )
            {
                rmStatus = SetSSLSegmentOffset(uData);
                break;
            }
        }
        
        if (uCnt != 64)
            break;

        for (uCnt = 0; uCnt < 64; uCnt++)
        {
            if (CheckRange(uOffset, (0x4010 + (uCnt*8))) )
            {
                rmStatus = SetSSLSegmentOffsetLength(uData, uCnt);
                break;
            }
        }

        if (uCnt != 64)
            break;
        
        
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "NVARM: CMCP1Method::Execute - SW method not yet implemented!\n");

#if defined WDM
        KdPrint(("Method offset = 0x%x Method Data = 0x%x\n", uOffset, uData));
#endif

    } while (FALSE);
    
    return rmStatus;
}

// software methods
RM_STATUS
CMCP1Method::SetContextDMANotify(U032 uData)
{
    CContextDma *pDma = (CContextDma *)uData;

    if ((U032)(pDma->GetAddress()) & 0xFF)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: SetContextDMANotify - Address not aligned on 256bytes boundary\n");
        return RM_ERROR;
    }

    m_pHal->RegWrite(NV_PAPU_FENADDR, (U032)pDma->GetAddress());
    return RM_OK;
}

RM_STATUS
CMCP1Method::SetCurrentSSLContextDMA(U032 uData)
{
    CContextDma *pDma = (CContextDma *)uData;

    if (((U032)(pDma->GetAddress()) & 0xFFF) ||
        (pDma->GetSize() & 0xFFF))
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: SetCurrentSSLContextDMA - Address not aligned on 4K boundary\n");
        return RM_ERROR;
    }

    m_pHal->RegWrite(NV_PAPU_FESESSLCTXPA, (U032)pDma->GetAddress());
    m_pHal->RegWrite(NV_PAPU_FESESSLMAXOFF, (U032)pDma->GetSize());

    return RM_OK;
}

RM_STATUS
CMCP1Method::SetCurrentInBufSGEContextDMA(U032 uData)
{
    CContextDma *pDma = (CContextDma *)uData;

    if (((U032)(pDma->GetAddress()) & 0xFFF) ||
        (pDma->GetSize() & 0xFFF))
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: SetCurrentInBufSGEContextDMA - Address not aligned on 4K boundary\n");
        return RM_ERROR;
    }

    m_pHal->RegWrite(NV_PAPU_FESESGECTXPA, (U032)pDma->GetAddress());
    m_pHal->RegWrite(NV_PAPU_FESESGEMAXOFF, (U032)pDma->GetSize());

    return RM_OK;
}

RM_STATUS
CMCP1Method::SetCurrentOutBufSGEContextDMA(U032 uData)
{
    CContextDma *pDma = (CContextDma *)uData;

    if (((U032)(pDma->GetAddress()) & 0xFFF) ||
        (pDma->GetSize() & 0xFFF))
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: SetCurrentOutBufSGEContextDMA - Address not aligned on 4K boundary\n");
        return RM_ERROR;
    }

    m_pHal->RegWrite(NV_PAPU_FEGPSGECTXPA, (U032)pDma->GetAddress());
    m_pHal->RegWrite(NV_PAPU_FEGPSGEMAXOFF, (U032)pDma->GetSize());

    return RM_OK;
}

RM_STATUS
CMCP1Method::SetGPState(U032 uData)
{
    R_GP_CONTROL rControl;
    rControl.uValue = 0;

    if (uData)
    {
        // take it out of reset...
        R_GP_RESET rReset;
        rReset.uValue = 0;
        rReset.Global = NV_PAPU_GPRST_GPRST_DISABLED;
        rReset.DSP = NV_PAPU_GPRST_GPDSPRST_ENABLED;
        rReset.NMI = NV_PAPU_GPRST_GPNMI_DISABLED;
        rReset.Abort = NV_PAPU_GPRST_GPABORT_DISABLED;

        m_pHal->RegWrite(NV_PAPU_GPRST, rReset.uValue);
        
        // write the command to make the GP work in pass thru
        DSP_CONTROL dspCtl;

        dspCtl.w0.uValue = 0;
        dspCtl.w0.field.EOL = 1;    // this is the last command

        dspCtl.w1.uValue = 0;
        dspCtl.w1.field.interleave = 1;
        dspCtl.w1.field.dspToSys = 1;
        dspCtl.w1.field.smDataFormat = 1;
        dspCtl.w1.field.increment = 32;

        dspCtl.w2.uValue = 0;
        dspCtl.w2.field.count0 = 1;   // 2 samples per block
        dspCtl.w2.field.count1 = 32;

        dspCtl.w3.uValue = 0;
        dspCtl.w3.field.dspStart = 0x1400;

        m_pHal->GetGlobalProc()->WriteCommand(&dspCtl);

        // auto start and auto idle
        R_GPDMA_CONFIG rConfig;
        rConfig.uValue = 0;
        rConfig.Start = NV_PAPU_GPDCFG_GPDASTRT_ENABLED;
        rConfig.Ready = NV_PAPU_GPDCFG_GPDARDY_ENABLED;
        m_pHal->RegWrite(NV_PAPU_GPDCFG, rConfig.uValue);
    
        // DMA start block pointer
        m_pHal->RegWrite(NV_PAPU_GPDSPT, (U032) 0);

        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CMCP1Method::SetGPState - the GP might just start...");
        // enable the GP
        rControl.Idle = NV_PAPU_GPIDRDY_GPSETIDLE_SET;
        rControl.IntrNotify = NV_PAPU_GPIDRDY_GPSETNINT_NOT_SET;
    }
    else
    {
        // stop the GP
        /*
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "NVARM: CMCP1Method::SetGPState - the GP might just stop...");
        rControl.EnableStop = NV_PAPU_GPIDRDY_GPSTOP_ENABLED;
        rControl.Stopped = NV_PAPU_GPIDRDY_GPSTOPPED_SET;
        */
    }

    m_pHal->RegWrite(NV_PAPU_GPIDRDY, rControl.uValue);

    aosDbgPrintString(DEBUGLEVEL_ERRORS, "done.  Congratulations!\n");
    return RM_OK;
}

// NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET
RM_STATUS
CMCP1Method::SetCurrentOutBufSGEOffset(U032 uData)
{
    MCP1_PRD *pLin = (MCP1_PRD *)(m_pHal->GetGlobalProc()->GetSgeOutLinAddr());

    pLin->uAddr = uData;
    pLin->Control.uValue = 0;
    pLin->Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;      // owned by HW

    return RM_OK;
}

// NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET
RM_STATUS
CMCP1Method::SetCurrentInBufSGEOffset(U032 uData)
{
    VOLU32 uTemp;

    // find out the max offset
    m_pHal->RegRead(NV_PAPU_FESESGEMAXOFF, &uTemp);
    if (uTemp < uData)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CMCP1Method::SetCurrentInBufSGEOffset - max offset exceeded!\n");
        return RM_ERROR;
    }

    R_FE_CONTROL rFeCtl;

    m_pHal->RegRead(NV_PAPU_FECTL, &rFeCtl.uValue);
    if (rFeCtl.ValidSESGE == NV_PAPU_FECTL_FESESGECTXPA_VALID_NO)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CMCP1Method::SetCurrentInBufSGEOffset - SGE context dma valid bit not set\n");
        return RM_ERROR;
    }

    // find the right PRD to write to
    MCP1_PRD *pPrd = (MCP1_PRD *)(m_pHal->GetVoiceProc()->GetSgeInLinAddr());
    U032 uNumOfPrds = (m_pHal->GetVoiceProc()->GetSgeInSize())/NV_PSGE_SIZE;

    // read the current index to write to...
    m_pHal->RegRead(NV_PAPU_FECSESGE, &uTemp);

    // a little error checking of my own..
    if (uNumOfPrds < uTemp)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CMCP1Method::SetCurrentInBufSGEOffset - current index greater than total PRDs allocated\n");
        return RM_ERROR;
    }

    VOLU32 uBase;
    m_pHal->RegRead(NV_PAPU_FESESGECTXPA, &uBase);

    // make the PRD (not 0 based.. i think)
    pPrd[uTemp-1].uAddr = uBase + uData;
    pPrd[uTemp-1].Control.uValue = 0;
    pPrd[uTemp-1].Control.Owner = NV_PSSL_PRD_CONTROL_OWN_HW;

    return RM_OK;
}

// NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET
RM_STATUS
CMCP1Method::SetSSLSegmentOffset(U032 uData)
{
    // error checking .. to hell with it! (for now)

    if (uData & 0xFFF)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CMCP1Method::SetSSLSegmentOffset - alignment check fails!\n");
        return RM_ERROR;
    }
    
    // save it as pending
    uData = uData | NV_PAPU_FESESSLPENDOFF_VALID_YES;

    m_pHal->RegWrite(NV_PAPU_FESESSLPENDOFF, uData);

    return RM_OK;
}

// NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH
RM_STATUS
CMCP1Method::SetSSLSegmentOffsetLength(U032 uData, U032 uIndex)
{
    /*
    Check that the offset has already been saved by the previous method.
    Check that container_size != 2. 
    Check that (offset & container_size) == 0.
    Check that samples_per_block > stereo.
    
    Then, check the offset and length as follows:
    
           offset+(length*container_size*samples_per_block) <= maxoff
    */

    MCP1_PRD *pPrd = (MCP1_PRD *)(m_pHal->GetVoiceProc()->GetSegInLinAddr());
    
    // calculate the index of the SSL
    VOLU16 uCurrSSL = 0;
    m_pHal->RegRead(NV_PAPU_FECSESSL, &uCurrSSL);
    
    uIndex += (uCurrSSL * 64);
    
    // read the offset previosuly set
    VOLU32 uPendOffset = 0;
    m_pHal->RegRead(NV_PAPU_FESESSLPENDOFF, &uPendOffset);
    
    // just for fun confirm that it's valid
    if ((uPendOffset & 0x1) != NV_PAPU_FESESSLPENDOFF_VALID_YES)
    {
        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CMCP1Method::SetSSLSegmentOffsetLength - u can only set length after setting the offset!\n");
        return RM_ERROR;
    }
    
    VOLU32 uBase;
    m_pHal->RegRead(NV_PAPU_FESESSLCTXPA, &uBase);
    
    // these don't look like being '0' based
    pPrd[uIndex-1].uAddr = uBase + (uPendOffset & 0xFFFFF000);
    pPrd[uIndex-1].Control.uValue = uData;

    return RM_OK;
}

// NV1BA0_PIO_SET_OUTBUF_BA(i)
RM_STATUS
CMCP1Method::SetOutBufBA(U032 uData, U032 uIndex)
{
    U032 uOffBase;
    U032 uOffEnd;

    switch(uIndex)
    {
    case 0:
        uOffBase = NV_PAPU_GPOFBASE0;
        uOffEnd = NV_PAPU_GPOFEND0;
        break;
    case 1:
        uOffBase = NV_PAPU_GPOFBASE1;
        uOffEnd = NV_PAPU_GPOFEND1;
        break;
    case 2:
        uOffBase = NV_PAPU_GPOFBASE2;
        uOffEnd = NV_PAPU_GPOFEND2;
        break;
    case 3:
        uOffBase = NV_PAPU_GPOFBASE3;
        uOffEnd = NV_PAPU_GPOFEND3;
        break;
    }

    m_pHal->RegWrite(uOffBase, uData);
    m_pHal->RegWrite(uOffEnd, uData);

    return RM_OK;
}

// NV1BA0_PIO_SET_OUTBUF_LEN(i)
RM_STATUS
CMCP1Method::SetOutBufLen(U032 uData, U032 uIndex)
{
    U032 uOffBase;
    U032 uOffEnd;

    switch(uIndex)
    {
    case 0:
        uOffBase = NV_PAPU_GPOFBASE0;
        uOffEnd = NV_PAPU_GPOFEND0;
        break;
    case 1:
        uOffBase = NV_PAPU_GPOFBASE1;
        uOffEnd = NV_PAPU_GPOFEND1;
        break;
    case 2:
        uOffBase = NV_PAPU_GPOFBASE2;
        uOffEnd = NV_PAPU_GPOFEND2;
        break;
    case 3:
        uOffBase = NV_PAPU_GPOFBASE3;
        uOffEnd = NV_PAPU_GPOFEND3;
        break;
    }
    
    U032 uBaseAddr;
    m_pHal->RegRead(uOffBase, &uBaseAddr);

    m_pHal->RegWrite(uOffEnd, uBaseAddr + uData);
    
    return RM_OK;
}
