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

/***************************** Functions for MCP1***************************\
*                                                                           *
* Module: CGpMethod.cpp                                                       *
*     SW functions for the GP                                               *
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
#include <nv_ugp.h>
#include <nv32.h>
};

#include "CHalBase.h"
#include "CObjBase.h"
#include "CDspProgram.h"
#include "CGPDspProgram.h"
#include "CScratchManager.h"
#include "CScratchDma.h"
#include "CGpScratchManager.h"
#include "CHalGlobalProc.h"
#include <CLinkList.h>
#include "CCommandQ.h"
#include "CContextDma.h"
#include "CMixerBuffer.h"
#include "CGpMethod.h"
#include "AudioFx.h"

CGPMethod::CGPMethod(CHalGlobalProc *pProc, CGPScratchManager *pMgr, CGPDspProgram *pDsp, CHalBase *pHal)
{
    m_pScratchManager = pMgr;
    m_uActiveEffect = 0;
    m_pDspProgram = pDsp;
    m_pHal = pHal;
    m_pGlobalProc = pProc;
    m_pCommandQ = NULL;
    m_pAudioFx = NULL;
    m_pMixerBuffer = NULL;
    
    m_ExecutionList.uCount = 0;

    for (U032 uCnt = 0; uCnt < MAX_EFFECTS; uCnt++)
    {
        m_EffectInfo[uCnt].eState = FREE;
        m_EffectInfo[uCnt].pParamDma = NULL;

        // initialize the execution list
        m_ExecutionList.uEffects[uCnt] = kAudioFxModule_EmptyID;
    }

}


CGPMethod::~CGPMethod()
{
    if (m_pAudioFx)
        delete m_pAudioFx;

    if (m_pCommandQ)
        delete m_pCommandQ;

    if (m_pMixerBuffer)
        delete m_pMixerBuffer;

    m_pAudioFx = NULL;
    m_pCommandQ = NULL;
    m_pMixerBuffer = NULL;
}

RM_STATUS
CGPMethod::Initialize()
{
    RM_STATUS rmStatus = RM_ERROR;

    m_pCommandQ = new CCommandQ(m_pHal);
    if (m_pCommandQ)
        rmStatus = m_pCommandQ->Initialize(CCommandQ::Q_TYPE_GP);

    m_pAudioFx = new AUDIOFX(MAX_EFFECTS, m_pScratchManager->GetScratchDma());
    if (!m_pAudioFx)
        rmStatus = RM_ERROR;
    else
    {
        m_pMixerBuffer = new CMixerBuffer;
        if (!m_pMixerBuffer)
            rmStatus = RM_ERROR;
    }

    return rmStatus;
}

VOID *
CGPMethod::GetCommandQAddress() 
{ 
    return m_pCommandQ->GetLinAddr();
}

VOID
CGPMethod::Execute()
{
    RM_STATUS rmStatus;
    U032 uMethod;
    U032 uData;
    BOOL bWriteNotifier = FALSE;
    BOOL bQEmpty = FALSE;
    U032 uNotifierData;
    U032 uNotifierStatus;
    U032 uFifoNumber;
    U032 uSize;

    while (!bQEmpty)
    {
        m_pCommandQ->Read(&uMethod);

        bQEmpty = m_pCommandQ->Read(&uData);

        uNotifierStatus = 0;
        uFifoNumber = ~0;

        // execute the methods
        switch(uMethod)
        {
        case NV_AUDGP_REGISTER_EFFECT:
            uNotifierData = (U032)m_pAudioFx->RegisterModuleType((AUDIOFX_PLUGIN *)uData);
            bWriteNotifier = TRUE;
            break;

        case NV_AUDGP_ADD_EFFECT:
		// FIXXXX: this will temporarily get around the check-in.  Fix !!!!
            uNotifierData = (U032)m_pAudioFx->AddModule("IIR2");
            bWriteNotifier = TRUE;
            break;

        case NV_AUDGP_SET_CURRENT_EFFECT:
            rmStatus = SetCurrentEffect(uData);
            break;

        case NV_AUDGP_ROUTE_EFFECT:
            {
                STRUCT_ROUTE_EFFECT rRouteEffect;
                rRouteEffect.uValue = uData;
            
                U032 uBufID;
                if (rRouteEffect.connection == NV_AUDGP_ROUTE_EFFECT_CONNECTION_ENABLE)
                    uBufID = rRouteEffect.bufid;
                else
                {
                    // a null id means the connection from the pin is null... but can there be a case 
                    // of one pin pointing to multiple buffers???  probably not... it'll need to go through
                    // a mixer...
                    uBufID = kAudioFX_BufferID_Null; 
                }

            
                if (rRouteEffect.orientation == NV_AUDGP_ROUTE_EFFECT_ORIENTATION_SOURCE)
                    m_pAudioFx->SetModule_OutBuffer(rRouteEffect.effectID, rRouteEffect.pin, uBufID);
                else
                    m_pAudioFx->SetModule_InBuffer(rRouteEffect.effectID, rRouteEffect.pin, uBufID);
            }
            break;

        case NV_AUDGP_ROUTE_TO_FIFO:
            {
                STRUCT_ROUTE_TO_FIFO rRoutoFifo;
                rRoutoFifo.uValue = uData;
            
                U032 uBufferID;

                // find the buffer ID
                if (rRoutoFifo.fifoType == NV_AUDGP_ROUTE_TO_FIFO_TYPE_INPUT)
                {
                    if (rRoutoFifo.fifoNum == 0)
                        uBufferID = kAudioFX_BufferID_FIFO0_Input;
                    else
                        uBufferID = kAudioFX_BufferID_FIFO1_Input;

                }
                else
                {
                    switch(rRoutoFifo.fifoNum)
                    {
                    case 0:
                        uBufferID = kAudioFX_BufferID_FIFO0_Output;
                        break;
                    case 1:
                        uBufferID = kAudioFX_BufferID_FIFO1_Output;
                        break;
                    case 2:
                        uBufferID = kAudioFX_BufferID_FIFO2_Output;
                        break;
                    case 3:
                        uBufferID = kAudioFX_BufferID_FIFO3_Output;
                        break;
                    default:
                        aosDbgPrintString(DEBUGLEVEL_ERRORS, "CGPMethod::NV_AUDGP_ROUTE_TO_FIFO - only 4 output FIFOs!\n");
                        aosDbgBreakPoint();
                    }
                }

                uBufferID += (rRoutoFifo.fifoPin + 1);

                if (rRoutoFifo.connection == NV_AUDGP_ROUTE_TO_FIFO_CONNECTION_DISABLE)
                    uBufferID = kAudioFX_BufferID_Null;

                // so if it's an input fifo.. then does it feed an effect.. liek an input buffer?
                // it seems to be so...

                // now... i hope audiofx ignores inIndex (effect pin..), if it's a buffer, and not an effect
                if (rRoutoFifo.fifoType == NV_AUDGP_ROUTE_TO_FIFO_TYPE_INPUT)
                    m_pAudioFx->SetModule_InBuffer(rRoutoFifo.sourceID, rRoutoFifo.sourcePin, uBufferID);
                else
                    m_pAudioFx->SetModule_OutBuffer(rRoutoFifo.sourceID, rRoutoFifo.sourcePin, uBufferID);
            }
            break;

        case NV_AUDGP_QUERY_EFFECT:
            {
                STRUCT_QUERY_EFFECT rQueryEffect;
                rQueryEffect.uValue = uData;
                uNotifierData = (U032) m_pAudioFx->QueryModule(rQueryEffect.effectID, rQueryEffect.typeID);
                bWriteNotifier = TRUE;
            }
            break;

        case NV_AUDGP_DEREGISTER_EFFECT:
            m_pAudioFx->UnregisterModuleType(uData);
            break;

        case NV_AUDGP_GET_MIX_BUFFER:
            rmStatus = GetMixBuffer(uData, &uNotifierData);
            bWriteNotifier = TRUE;
            break;

        case NV_AUDGP_FREE_MIX_BUFFER:
            m_pMixerBuffer->Free((U008)uData);
            break;

        case NV_AUDGP_SET_CONTEXT_DMA_NOTIFIER:
            rmStatus = SetContextDmaNotifier(uData);
            break;
    
        case NV_AUDGP_SET_CONTEXT_DMA_PARAMETER:
            rmStatus = SetContextDmaParameter(uData);
            break;

        case NV_AUDGP_SET_CONTEXT_DMA_DSP_CODE:
            rmStatus = SetContextDmaDspCode(uData);
            break;

        case NV_AUDGP_UPDATE_EFFECT_PARAMETERS:
            {
                rmStatus = RM_ERROR;
                VOID *pParamData;
                U032 uEffectId = uData;

                // check if the context DMA for the effect params has been allocated
                if (m_EffectInfo[uEffectId].pParamDma)
                {
                    pParamData = m_EffectInfo[uEffectId].pParamDma->GetLinAddr();
                    m_pAudioFx->SetModule(uEffectId, pParamData);
                    rmStatus = RM_OK;
                }
            }
            break;

        case NV_AUDGP_ENABLE_EFFECT:
        case NV_AUDGP_DISABLE_EFFECT:
            UpdateExecutionList(uData, (uMethod==NV_AUDGP_ENABLE_EFFECT)?TRUE:FALSE);
            m_pAudioFx->SetModuleExecutionList((long *)&m_ExecutionList.uEffects[0], m_ExecutionList.uCount);
            break;
        
        case NV_AUDGP_DELETE_EFFECT:
            m_pAudioFx->DeleteModule(uData);
            break;


        case NV_AUDGP_SET_EXECUTION_LIST:
            U032 id;
            m_ExecutionList.uCount = uData;
            for(id =0;id < uData;id++)
                bQEmpty = m_pCommandQ->Read(&m_ExecutionList.uEffects[id]);
            m_pAudioFx->SetModuleExecutionList((long *)&m_ExecutionList.uEffects[0], m_ExecutionList.uCount);
            break;


        case NV_AUDGP_CONFIGURE_OUTPUT_FIFO(0):
        case NV_AUDGP_CONFIGURE_OUTPUT_FIFO(1):
        case NV_AUDGP_CONFIGURE_OUTPUT_FIFO(2):
        case NV_AUDGP_CONFIGURE_OUTPUT_FIFO(3):
        case NV_AUDGP_CONFIGURE_INPUT_FIFO(0):
        case NV_AUDGP_CONFIGURE_INPUT_FIFO(1):
            bQEmpty = m_pCommandQ->Read(&uData);
            ConfigureFifo(uMethod, uData);
            break;

        case NV_AUDGP_ALLOC_SCRATCH_MEMORY:
            m_pScratchManager->GetScratchDma()->Allocate(uData,&uNotifierData);
            bWriteNotifier = TRUE;
            break;

        case NV_AUDGP_FREE_SCRATCH_MEMORY:
            bQEmpty = m_pCommandQ->Read(&uSize);
            m_pScratchManager->GetScratchDma()->Free(uData,uSize);

        case NV_AUDGP_SET_SCRATCH_MEMORY:
            U032 uValue;
            m_pCommandQ->Read(&uSize);
            bQEmpty = m_pCommandQ->Read(&uValue);
            m_pScratchManager->GetScratchDma()->Put(uData, uSize, uValue);
            break;

        default:
            aosDbgPrintString(DEBUGLEVEL_ERRORS, "CGPMethod::Execute - unknown method\n");
            break;
        }

        // write the notifier...
        if (bWriteNotifier)
            m_pGlobalProc->WriteNotifier(uMethod, uNotifierData, (U008)rmStatus);

    }
}


// gp methods handled by this class
RM_STATUS
CGPMethod::SetCurrentEffect(U032 uData)
{
    if (uData < MAX_EFFECTS)
    {
        m_uActiveEffect = uData;
        return RM_OK;
    }
    
    return RM_ERROR;
}


RM_STATUS
CGPMethod::SetContextDmaNotifier(U032 uData)
{
    RM_STATUS rmStatus = RM_OK;
    CContextDma *pDma = (CContextDma *)uData;
    
    // in debug mode... do some checks... todo vik
    m_pHal->RegWrite( NV_PAPU_GPNADDR, (U032)(pDma->GetAddress()) );

    // set the notifier memory in the GP
    m_pGlobalProc->SetNotifierMemory(pDma->GetLinAddr(), pDma->GetSize());
    return RM_OK;
}

RM_STATUS
CGPMethod::SetContextDmaParameter(U032 uData)
{
    // i dunno if i wanna check if the effect is allocated
    m_EffectInfo[m_uActiveEffect].pParamDma = (CContextDma *)uData;
    return RM_OK;
}


RM_STATUS
CGPMethod::SetContextDmaDspCode(U032 uData)
{
    // i dunno if i wanna check if the effect is allocated
    m_EffectInfo[m_uActiveEffect].pDspCodeDma = (CContextDma *)uData;
    return RM_OK;
}


RM_STATUS
CGPMethod::GetMixBuffer(U032 uArg, U032 *pId)
{
    RM_STATUS rmStatus = RM_ERROR;

    *pId = 0;

    if ( (0x3 & uArg) != NV_AUDGP_GET_MIX_BUFFER_TYPE_SW )
        rmStatus = m_pMixerBuffer->Allocate(m_pMixerBuffer->MIXER_TYPE::HW, (U008 *)pId);

    if (rmStatus == RM_ERROR)
        rmStatus = m_pMixerBuffer->Allocate(m_pMixerBuffer->MIXER_TYPE::SW, (U008 *)pId);

    return rmStatus;
}

VOID
CGPMethod::UpdateExecutionList(U032 uEffectID, BOOL bOption)
{
    if (bOption == (BOOL)TRUE)
    {
        m_ExecutionList.uEffects[m_ExecutionList.uCount++] = uEffectID;
        return;
    }
    else
    {
        // remove it from the execution list
        for (U032 uCnt = 0; uCnt < m_ExecutionList.uCount; uCnt++)
        {
            if (m_ExecutionList.uEffects[uCnt] == uEffectID)
            {
                for (U032 uCnt1 = uCnt; uCnt1 < m_ExecutionList.uCount-1; uCnt1++)
                    m_ExecutionList.uEffects[uCnt1] = m_ExecutionList.uEffects[uCnt1+1];

                m_ExecutionList.uCount--;
                return;
            }
        }

        // some crap has happened... cant find effect!
        aosDbgPrintString(DEBUGLEVEL_WARNINGS, "CGPMethod::UpdateExecutionList - cannot find method!\n");
    }
}

VOID
CGPMethod::ConfigureFifo(U032 uMethod, U032 uData)
{
    R_GP_CTLOUT rCtlOut;
    R_GP_CTLIN rCtlIn;

    U032 uRegOffset;
    U032 uRegValue;

    STRUCT_CONFIGURE_FIFO sConfigFifo;

    m_pHal->RegRead(NV_PAPU_GPCTLOUT, &rCtlOut.uValue);
    sConfigFifo.uValue = uData;
    
    U032 uContainerSize;
    if (sConfigFifo.size == NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_8_8)
        uContainerSize = NV_PAPU_GPCTLOUT_GD0SIZ_8;
    else if (sConfigFifo.size == NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_32_32)
        uContainerSize = NV_PAPU_GPCTLOUT_GD0SIZ_32;
    else
        uContainerSize = NV_PAPU_GPCTLOUT_GD0SIZ_16;

    if (uMethod == NV_AUDGP_CONFIGURE_OUTPUT_FIFO(0))
    {
        rCtlOut.format0 = sConfigFifo.format;
        rCtlOut.type0 = sConfigFifo.type;
        rCtlOut.size0 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLOUT;
        uRegValue = rCtlOut.uValue;
        m_pAudioFx->SetOutputFIFOFormat(0, sConfigFifo.format, sConfigFifo.size);
    }
    else if (uMethod == NV_AUDGP_CONFIGURE_OUTPUT_FIFO(1))
    {
        rCtlOut.format1 = sConfigFifo.format;
        rCtlOut.type1 = sConfigFifo.type;
        rCtlOut.size1 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLOUT;
        uRegValue = rCtlOut.uValue;
        m_pAudioFx->SetOutputFIFOFormat(1, sConfigFifo.format, sConfigFifo.size);
    }
    else if (uMethod == NV_AUDGP_CONFIGURE_OUTPUT_FIFO(2))
    {
        rCtlOut.format2 = sConfigFifo.format;
        rCtlOut.type2 = sConfigFifo.type;
        rCtlOut.size2 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLOUT;
        uRegValue = rCtlOut.uValue;
        m_pAudioFx->SetOutputFIFOFormat(2, sConfigFifo.format, sConfigFifo.size);
    }
    else if (uMethod == NV_AUDGP_CONFIGURE_OUTPUT_FIFO(3))
    {
        rCtlOut.format3 = sConfigFifo.format;
        rCtlOut.type3 = sConfigFifo.type;
        rCtlOut.size3 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLOUT;
        uRegValue = rCtlOut.uValue;
        m_pAudioFx->SetOutputFIFOFormat(3, sConfigFifo.format, sConfigFifo.size);
    }
    else if (uMethod == NV_AUDGP_CONFIGURE_INPUT_FIFO(0))
    {
        rCtlIn.format0 = sConfigFifo.format;
        rCtlIn.type0 = sConfigFifo.type;
        rCtlIn.size0 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLIN;
        uRegValue = rCtlIn.uValue;
        m_pAudioFx->SetInputFIFOFormat(0, sConfigFifo.format, sConfigFifo.size);
    }
    else
    {
        rCtlIn.format1 = sConfigFifo.format;
        rCtlIn.type1 = sConfigFifo.type;
        rCtlIn.size1 = uContainerSize;
        uRegOffset = NV_PAPU_GPCTLIN;
        uRegValue = rCtlIn.uValue;
        m_pAudioFx->SetInputFIFOFormat(1, sConfigFifo.format, sConfigFifo.size);
    }
    
    m_pHal->RegWrite(uRegOffset, uRegValue);
}