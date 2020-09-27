/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpapu.cpp
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created based on NVidia/georgioc code.
 *
 ****************************************************************************/

#include "dsoundi.h"

#ifdef DEBUG

EXTERN_C BOOL g_fDirectSoundTestSimulateDeltaPanic;
BOOL g_fDirectSoundTestSimulateDeltaPanic = FALSE;

#endif // DEBUG


/****************************************************************************
 *
 *  CMcpxAPUTest
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::CMcpxAPUTest"

KINTERRUPT CMcpxAPUTest::m_Interrupt;
DWORD CMcpxAPUTest::m_dwDeltaPanicCount = 0;

CMcpxAPUTest::CMcpxAPUTest
(
    CDirectSoundTestSettings *  pSettings
)
:   CMcpxCoreTest(pSettings)
{
    DWORD                   i;
    
    DPF_ENTER();
    
    //
    // Initialize defaults
    //
    
    for(i = 0; i < NUMELMS(m_alstActiveVoices); i++)
    {
        InitializeListHead(&m_alstActiveVoices[i]);
    }

    InitializeListHead(&m_lstPendingInactiveVoices);
    InitializeListHead(&m_lstDeferredCommandsHigh);
    InitializeListHead(&m_lstDeferredCommandsLow);

    m_wFree2dVoiceCount = MCPX_HW_MAX_2D_VOICES;
    m_wFree3dVoiceCount = MCPX_HW_MAX_3D_VOICES;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxAPUTest
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::~CMcpxAPUTest"

CMcpxAPUTest::~CMcpxAPUTest
(
    void
)
{
    DPF_ENTER();

    DPF_INFO("APU going away...");

    //
    // Shut down the APU
    //

    Terminate();

    //
    // Disconnect the interrupt handler
    //

    if(m_Interrupt.ServiceRoutine)
    {
        KeDisconnectInterrupt(&m_Interrupt);
        m_Interrupt.ServiceRoutine = NULL;
    }

    //
    // Cancel timers
    //

    KeCancelTimer(&m_tmrDeferredCommandsHigh);

    //
    // Clear any pending DPCs
    //

    if(DpcObject == m_dpcInterrupt.Type)
    {
        KeRemoveQueueDpc(&m_dpcInterrupt);
        m_dpcInterrupt.Type = ~m_dpcInterrupt.Type;
    }

    if(DpcObject == m_dpcDeferredCommandsHigh.Type)
    {
        KeRemoveQueueDpc(&m_dpcDeferredCommandsHigh);
        m_dpcDeferredCommandsHigh.Type = ~m_dpcInterrupt.Type;
    }

    //
    // Unregister the shutdown handler
    //

    if(m_HalShutdownData.NotificationRoutine)
    {
        HalRegisterShutdownNotification(&m_HalShutdownData, FALSE);
        m_HalShutdownData.NotificationRoutine = NULL;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Intializes the object.
 *
 *  Arguments:
 *      CDirectSoundTestSettings * [in]: shared settings object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::Initialize"

HRESULT 
CMcpxAPUTest::Initialize
(
    void
)
{
    ULONG                   ulInterruptVector;
    KIRQL                   irql;
    HRESULT                 hr;
    DWORD                   i;

    //
    // Initialize the APU core
    //

    hr = CMcpxCoreTest::Initialize();

    //
    // Create the buffer SGE heap.  We're only managing 2047 markers because
    // the 2048th is reserved for FXIN buffers.
    //

    if(SUCCEEDED(hr))
    {
        hr = m_SgeHeap.Initialize(MCPX_HW_MAX_BUFFER_PRDS - 1);
    }

    //
    // Set up timers and DPCs
    //
    
	if(SUCCEEDED(hr))
    {
        KeInitializeDpc(&m_dpcInterrupt, ApuInterruptDpcRoutine, this);
        KeInitializeTimer(&m_tmrDeferredCommandsHigh);
        KeInitializeDpc(&m_dpcDeferredCommandsHigh, DeferredCommandDpcRoutine, this);
    }

	//
    // Hook and enable interrupts
    //

    if(SUCCEEDED(hr))
    {
        ulInterruptVector = HalGetInterruptVector(XPCICFG_APU_IRQ, &irql);

        KeInitializeInterrupt(&m_Interrupt, ApuInterruptServiceRoutine, this, ulInterruptVector, irql, LevelSensitive, TRUE);

        if(!KeConnectInterrupt(&m_Interrupt))
        {
            DPF_ERROR("KeConnectInterrupt failed");
            hr = DSERR_GENERIC;
        }
    }

    if(SUCCEEDED(hr))
    {
        MCPX_REG_WRITE(NV_PAPU_ISTS, ~0UL);
    }

    if(SUCCEEDED(hr))
    {
        SetInterruptState(TRUE);
    }

    //
    // Register for HAL shutdown notification
    //

    if(SUCCEEDED(hr))
    {
        m_HalShutdownData.NotificationRoutine = ApuShutdownNotifier;

        HalRegisterShutdownNotification(&m_HalShutdownData, TRUE);
    }

    DPF_LEAVE_HRESULT(hr);

	return hr;
}


/****************************************************************************
 *
 *  Terminate
 *
 *  Description:
 *      Shuts down the APU.  This function assumes that the system is 
 *      rebooting.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::Terminate"

void
CMcpxAPUTest::Terminate
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Block the idle handler
    //

    BlockIdleHandler();

    //
    // Unblock the idle handler
    //

    UnblockIdleHandler();

    //
    // Shut down the APU core
    //

    Reset();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceApuInterrupt
 *
 *  Description:
 *      Handles APU interrupts.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE if the interrupt was handled.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::ServiceApuInterrupt"

BOOL
CMcpxAPUTest::ServiceApuInterrupt
(
    void
)
{
    R_INTR                  rInterruptStatus;
    BOOL                    fServiced;

    //
    // Get the pending interrupt
    //
        
    MCPX_REG_READ(NV_PAPU_ISTS, &rInterruptStatus.uValue);

    if(fServiced = MAKEBOOL(rInterruptStatus.General))
    {
        //
        // Clear the interrupt(s)
        //

        MCPX_REG_WRITE(NV_PAPU_ISTS, rInterruptStatus.uValue);

        //
        // Save the interrupt bits
        //

        m_arInterruptStatus[0].uValue |= rInterruptStatus.uValue;

#ifdef DEBUG

        m_arInterruptStatus[0].DeltaPanic |= INTERLOCKED_EXCHANGE(g_fDirectSoundTestSimulateDeltaPanic, 0);

#endif // DEBUG

        //
        // Check for a trap caused by an error or an overridden hardware 
        // method
        //
        
        if(rInterruptStatus.FETrap)
        {
            HandleFETrap();
        }

        //
        // If we got a voice interrupt or a delta panic, schedule a DPC to 
        // handle it
        //

        if(rInterruptStatus.DeltaPanic || rInterruptStatus.FEVoice)
        {
            ScheduleApuInterruptDpc();
        }

        //
        // Read the interrupt mask one more time to make sure all registers are
        // flushed properly
        //

        MCPX_REG_READ(NV_PAPU_ISTS, &rInterruptStatus.uValue);
    }
    
    return fServiced;
}


/****************************************************************************
 *
 *  WaitForMagicWrite
 *
 *  Description:
 *      Waits for the "magic write" to complete.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::WaitForMagicWrite"

void
CMcpxAPUTest::WaitForMagicWrite
(
    void
)
{
    static DWORD            dwMagicWriteValue   = 0;
    DWORD                   dwTimeout           = 100;
    volatile DWORD *        pdwMagicWrite;
    DWORD                   i;

    dwMagicWriteValue++;

    pdwMagicWrite = (volatile DWORD *)m_ctxMemory[MCPX_MEM_MAGICWRITE].VirtualAddress;
    *pdwMagicWrite = ~dwMagicWriteValue;

    SetPrivLockState(TRUE);

    MCPX_REG_WRITE(NV_PAPU_FEMEMADDR, m_ctxMemory[MCPX_MEM_MAGICWRITE].PhysicalAddress);
    MCPX_REG_WRITE(NV_PAPU_FEMEMDATA, dwMagicWriteValue);

    SetPrivLockState(FALSE);

    while(*pdwMagicWrite != dwMagicWriteValue)
    {
        if(!dwTimeout--)
        {
            DPF_ERROR("Magic write not completing!");
            break;
        }

        KeStallExecutionProcessor(1);
    }
}


/****************************************************************************
 *
 *  ServiceApuInterruptDpc
 *
 *  Description:
 *      Handles APU interrupts from a deferred procedure callback.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::ServiceApuInterruptDpc"

void
CMcpxAPUTest::ServiceApuInterruptDpc
(
    void
)
{
    //
    // The DPC will parse the interrupt mask and do processing based on what
    // event was signaled. In ISR-safe fashion, it will read the current 
    // pending interrupt mask twice in the loop. This makes sure the dpc 
    // leaves only when no more ISRs are pending.
    //

    while(TRUE)
    {
        KeSynchronizeExecution(&m_Interrupt, GetInterruptStatusCallback, this);

        if(!m_arInterruptStatus[1].General)
        {
            break;
        }

#ifndef MCPX_BOOT_LIB

        if(m_arInterruptStatus[1].DeltaPanic)
        {
            HandleDeltaPanic();
        }

#endif // MCPX_BOOT_LIB

        if(m_arInterruptStatus[1].FEVoice)
        {
            ServiceVoiceInterrupt();
        }

        m_arInterruptStatus[1].uValue = 0;
    } 
}


/****************************************************************************
 *
 *  ServiceVoiceInterrupt
 *
 *  Description:
 *      Handles APU interrupts from a deferred procedure callback.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::ServiceVoiceInterrupt"

void
CMcpxAPUTest::ServiceVoiceInterrupt
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    DWORD                   dwVoiceList;

    //
    // Wait for the magic write to complete so that all notifiers are 
    // written
    //

    WaitForMagicWrite();

    //
    // Block the idle handler in order to prevent the voice list from
    // being modified.
    //

    BlockIdleHandler();

    //
    // Unblock the idle handler
    //

    UnblockIdleHandler();
}


/****************************************************************************
 *
 *  GetInterruptStatusCallback
 *
 *  Description:
 *      Interrupt synchonization function.
 *
 *  Arguments:
 *      LPVOID [in]: context.
 *
 *  Returns:  
 *      BOOLEAN: TRUE.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::GetInterruptStatusCallback"

BOOLEAN 
CMcpxAPUTest::GetInterruptStatusCallback
(
    LPVOID                  pvContext
)
{
    CMcpxAPUTest *              pThis   = (CMcpxAPUTest *)pvContext;

    pThis->m_arInterruptStatus[1].uValue |= INTERLOCKED_EXCHANGE(pThis->m_arInterruptStatus[0].uValue, 0);

    return TRUE;
}


/****************************************************************************
 *
 *  HandleFETrap
 *
 *  Description:
 *      Handles front-end method traps.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::HandleFETrap"

VOID
CMcpxAPUTest::HandleFETrap
(
    void
)
{
    R_FE_CONTROL            rFeControl;
    DWORD                   dwParam;
    DWORD                   dwMethod;

    DPF_ENTER();

    //
    // Whahoppen?
    //
    
    MCPX_REG_READ(NV_PAPU_FECTL, &rFeControl.uValue);
    ASSERT(NV_PAPU_FECTL_FEMETHMODE_TRAPPED == rFeControl.Mode);

    MCPX_REG_READ(NV_PAPU_FEDECMETH, &dwMethod);
    MCPX_REG_READ(NV_PAPU_FEDECPARAM, &dwParam);

    dwMethod = MCPX_GET_REG_VALUE(dwMethod, NV_PAPU_FEDECMETH_METH);
    dwParam = MCPX_GET_REG_VALUE(dwParam, NV_PAPU_FEDECPARAM_VALUE);

    switch(rFeControl.TrapReason)
    {
        case NV_PAPU_FECTL_FETRAPREASON_NONE:
            DPF_ERROR("NONE: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_NOT_IMPLEMENTED:
            DPF_ERROR("NOT_IMPLEMENTED: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_METHOD_UNKNOWN:
            DPF_ERROR("METHOD_UNKNOWN: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_BAD_ARGUMENT:
            DPF_ERROR("BAD_ARGUMENT: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_CURRENT_NOT_SET:
            DPF_ERROR("CURRENT_NOT_SET: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_ANTECEDENT_NOT_SET:
            DPF_ERROR("ANTECEDENT_NOT_SET: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_BAD_DATA_TYPE:
            DPF_ERROR("BAD_DATA_TYPE: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_BAD_LIST_POINTER:
            DPF_ERROR("BAD_LIST_POINTER: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_CURRENT_VOICE_NOT_3D:
            DPF_ERROR("CURRENT_VOICE_NOT_3d: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_CTXPA_INVALID:
            DPF_ERROR("CTXPA_INVALID: method %x, param %x", dwMethod, dwParam);
            break;

        case NV_PAPU_FECTL_FETRAPREASON_REQUESTED:
            HandleSoftwareMethod(dwMethod, dwParam);
            break;
        
        default:
            DPF_ERROR("(unknown) %x: method %x, param %x", (DWORD)rFeControl.TrapReason, dwMethod, dwParam);
            break;
    }

    //
    // Snap the FE out of trap mode
    //

    SetFrontEndState(MCPX_FE_STATE_HALTED);
    SetFrontEndState(MCPX_FE_STATE_FREE_RUNNING);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  HandleDeltaPanic
 *
 *  Description:
 *      Handles a delta panic (when the APU output read cursor passes the
 *      write cursor).
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::HandleDeltaPanic"

void
CMcpxAPUTest::HandleDeltaPanic
(
    void
)
{
    PHOST_TO_DSP_COMMANDBLOCK   pCmdBlock;
    BOOL                        fIdle;
    
    DPF_ENTER();

    DPF_WARNING("Delta panic.  You'll hear a glitch while the APU is reset...");
    
    m_dwDeltaPanicCount++;

    //
    // Idle the voice processor
    //

    fIdle = IdleVoiceProcessor(TRUE);

    //
    // Disable interrupts
    //

    SetInterruptState(FALSE);

    //
    // Reset the ACI DMA
    //

    m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->Reset();
    m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->Reset();

    //
    // Reset the sample counters
    //

    ResetGlobalCounters();

    //
    // Reset the GP command block
    //

    pCmdBlock = (PHOST_TO_DSP_COMMANDBLOCK)((LPBYTE)m_ctxMemory[MCPX_MEM_GPSCRATCH].VirtualAddress + DSP_COMMANDBLOCK_SCRATCHOFFSET);

    pCmdBlock->dwCommandFlags = 0;
    pCmdBlock->dwOffset = 0;

    //
    // Reset the DSPs
    //

    MCPX_REG_WRITE(NV_PAPU_GPRST, 0);
    MCPX_REG_WRITE(NV_PAPU_EPRST, 0);

    SetupDSPs();

    //
    // Tell the GP to re-download it's program
    //

    m_pGpDspManager->RestoreCommandBlock();

    pCmdBlock->dwCommandFlags = BIT_H2DCB_COMMAND_LOAD_XRAM_STATE | BIT_H2DCB_COMMAND_LOAD_CODE;

    //
    // Clear any remaining delta panic bits in the interrupt status
    //

    m_arInterruptStatus[0].DeltaPanic = FALSE;
    m_arInterruptStatus[1].DeltaPanic = FALSE;

    //
    // Enable interrupts
    //

    SetInterruptState(TRUE);

    //
    // Start the ACI running again
    //

    m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->Run();
    m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->Run();

    //
    // Restore the VP state
    //

    if(fIdle)
    {
        IdleVoiceProcessor(FALSE);
    }

    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  HandleSoftwareMethod
 *
 *  Description:
 *      Dispatches a software-overridden hardware method.
 *
 *  Arguments:
 *      DWORD [in]: method identifier.
 *      DWORD [in]: method parameter.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPUTest::HandleSoftwareMethod"

void
CMcpxAPUTest::HandleSoftwareMethod
(
    DWORD                   dwMethod,
    DWORD                   dwParam
)
{
    DPF_ENTER();

    switch(dwMethod)
    {
        case 0x8000:
            
            //
            // SE2FE_IDLE_VOICE is undocumented since it's an internal method
            //


            break;

        default:
            
            //
            // Huh?
            //

            DPF_ERROR("Unexpected software method:  %x %x", dwMethod, dwParam);

            break;
    }

    DPF_LEAVE_VOID();
}



