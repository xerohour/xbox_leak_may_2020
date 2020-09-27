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


/****************************************************************************
 *
 *  CMcpxAPU
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
#define DPF_FNAME "CMcpxAPU::CMcpxAPU"

KINTERRUPT CMcpxAPU::m_Interrupt;

DWORD CMcpxAPU::m_dwDeltaWarningCount = 0;
DWORD CMcpxAPU::m_dwDeltaPanicCount = 0;

#ifdef DEBUG

BOOL CMcpxAPU::m_fSimulateDeltaPanic = FALSE;
BOOL CMcpxAPU::m_fSimulateDeltaWarning = FALSE;

#endif // DEBUG

CMcpxAPU::CMcpxAPU
(
    CDirectSoundSettings *  pSettings
)
:   CMcpxCore(pSettings),

#ifndef MCPX_BOOT_LIB

    CHrtfListener(pSettings->m_3dParams.HrtfParams),
    CI3dl2Listener(pSettings->m_3dParams.I3dl2Params),

#endif // MCPX_BOOT_LIB

    m_dwFree2dVoiceCount(g_dwDirectSoundFree2dVoices),
    m_dwFree3dVoiceCount(g_dwDirectSoundFree3dVoices)
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

    m_dwFree2dVoiceCount = MCPX_HW_MAX_2D_VOICES;
    m_dwFree3dVoiceCount = MCPX_HW_MAX_3D_VOICES;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxAPU
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
#define DPF_FNAME "CMcpxAPU::~CMcpxAPU"

CMcpxAPU::~CMcpxAPU
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

    m_dwFree2dVoiceCount = 0;
    m_dwFree3dVoiceCount = 0;

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
 *      CDirectSoundSettings * [in]: shared settings object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::Initialize"

HRESULT 
CMcpxAPU::Initialize
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

    hr = CMcpxCore::Initialize();

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

    //
    // Set default headroom values
    //

    if(SUCCEEDED(hr))
    {
        hr = SetHrtfHeadroom(0);
    }

    for(i = 0; (i < NUMELMS(m_pSettings->m_abMixBinHeadroom)) && SUCCEEDED(hr); i++)
    {
        hr = SetMixBinHeadroom(i);
    }

#ifndef MCPX_BOOT_LIB

    //
    // Set up the HRTF listener
    //

    if(SUCCEEDED(hr))
    {
        if(DSSPEAKER_IS_HEADPHONES(m_pSettings->m_dwSpeakerConfig))
        {
            m_fSurround = FALSE;
        }
        else
        {
            m_fSurround = DSSPEAKER_IS_SURROUND(m_pSettings->m_dwSpeakerConfig);
        }
    }

#endif // MCPX_BOOT_LIB

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
#define DPF_FNAME "CMcpxAPU::Terminate"

void
CMcpxAPU::Terminate
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CMcpxVoiceClient *      pVoice;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Block the idle handler
    //

    BlockIdleHandler();

    //
    // Deactivate all voices
    //

    for(i = 0; i < NUMELMS(m_alstActiveVoices); i++)
    {
        for(pleEntry = m_alstActiveVoices[i].Flink; pleEntry != &m_alstActiveVoices[i]; pleEntry = pleEntry->Flink)
        {
            pVoice = CONTAINING_RECORD(pleEntry, CMcpxVoiceClient, m_leActiveVoice);

            pVoice->DeactivateVoice();
        }
    }

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
 *  AllocateVoices
 *
 *  Description:
 *      Allocates hardware voices for a client.
 *
 *  Arguments:
 *      CMcpxVoiceClient * [in]: voice client.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::AllocateVoices"

HRESULT
CMcpxAPU::AllocateVoices
(
    CMcpxVoiceClient *      pVoice
)
{
    LPDWORD                 pdwFreeVoiceCount;
    MCPX_VOICE_HANDLE       nFirstVoice;
    MCPX_VOICE_HANDLE       nLastVoice;
    MCPX_VOICE_HANDLE       nVoice;
    BYTE                    bVoiceIndex;
    HRESULT                 hr;

    DPF_ENTER();
    AutoIrql();

#ifdef DEBUG

    ASSERT(pVoice->m_bVoiceCount);
    ASSERT(pVoice->m_bVoiceCount <= NUMELMS(pVoice->m_ahVoices));

    for(bVoiceIndex = 0; bVoiceIndex < NUMELMS(pVoice->m_ahVoices); bVoiceIndex++)
    {
        ASSERT(MCPX_VOICE_HANDLE_INVALID == pVoice->m_ahVoices[bVoiceIndex]);
    }

#endif // DEBUG

    //
    // Block the idle handler in order to lock the voice map
    //

    BlockIdleHandler();

    //
    // Allocate voices
    //
    
    if(pVoice->m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        pdwFreeVoiceCount = &m_dwFree3dVoiceCount;
        nFirstVoice = MCPX_HW_FIRST_3D_VOICE;
        nLastVoice = MCPX_HW_FIRST_3D_VOICE + MCPX_HW_MAX_3D_VOICES - 1;
    }
    else
    {
        pdwFreeVoiceCount = &m_dwFree2dVoiceCount;
        nFirstVoice = MCPX_HW_FIRST_2D_VOICE;
        nLastVoice = MCPX_HW_FIRST_2D_VOICE + MCPX_HW_MAX_2D_VOICES - 1;
    }

    if(pVoice->m_bVoiceCount <= *pdwFreeVoiceCount)
    {
        *pdwFreeVoiceCount -= pVoice->m_bVoiceCount;
        
        bVoiceIndex = 0;
        nVoice = nFirstVoice;

        while(bVoiceIndex < pVoice->m_bVoiceCount)
        {
            ASSERT(nVoice <= nLastVoice);

            if(!m_apVoiceMap[nVoice])
            {
                DPF_INFO("Voice client %x allocated hardware voice %x", pVoice, nVoice);

                pVoice->m_ahVoices[bVoiceIndex] = nVoice;
                m_apVoiceMap[nVoice] = pVoice;

                bVoiceIndex++;
            }

            nVoice++;
        }

        hr = DS_OK;
    }
    else
    {
        DPF_ERROR("Not enough free hardware voices");
        hr = DSERR_INVALIDCALL;
    }

    //
    // Unlock the voice map
    //

    UnblockIdleHandler();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  FreeVoices
 *
 *  Description:
 *      Allocates hardware voices for a client.
 *
 *  Arguments:
 *      CMcpxVoiceClient * [in]: voice client.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::FreeVoices"

void
CMcpxAPU::FreeVoices
(
    CMcpxVoiceClient *      pVoice
)
{
    DWORD                   dwVoiceIndex;
    WORD                    i;

    DPF_ENTER();
    AutoIrql();

    //
    // Block the idle handler in order to lock the voice map
    //

    BlockIdleHandler();

    //
    // Remove each voice from the map and the voice client's array
    //

    for(i = 0; i < pVoice->m_bVoiceCount; i++)
    {
        dwVoiceIndex = (DWORD)pVoice->m_ahVoices[i];
        pVoice->m_ahVoices[i] = MCPX_VOICE_HANDLE_INVALID;
        
        ASSERT(dwVoiceIndex < MCPX_HW_MAX_VOICES);
        ASSERT(m_apVoiceMap[dwVoiceIndex] == pVoice);

        m_apVoiceMap[dwVoiceIndex] = NULL;

        if(MCPX_IS_3D_VOICE(dwVoiceIndex))
        {
            m_dwFree3dVoiceCount++;
        }
        else
        {
            m_dwFree2dVoiceCount++;
        }
    }

#ifdef DEBUG

    for(i = 0; i < NUMELMS(pVoice->m_ahVoices); i++)
    {
        ASSERT(MCPX_VOICE_HANDLE_INVALID == pVoice->m_ahVoices[i]);
    }

#endif // DEBUG

    //
    // Unlock the voice map
    //

    UnblockIdleHandler();

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
#define DPF_FNAME "CMcpxAPU::ServiceApuInterrupt"

BOOL
CMcpxAPU::ServiceApuInterrupt
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

#ifdef DEBUG

        rInterruptStatus.DeltaPanic |= INTERLOCKED_EXCHANGE(m_fSimulateDeltaPanic, 0);
        rInterruptStatus.DeltaWarning |= INTERLOCKED_EXCHANGE(m_fSimulateDeltaWarning, 0);

#endif // DEBUG

        m_arInterruptStatus[0].uValue |= rInterruptStatus.uValue;

        //
        // Check for a trap caused by an error or an overridden hardware 
        // method
        //
    
        if(rInterruptStatus.FETrap)
        {
            HandleFETrap();
        }

#ifdef MCPX_HANDLE_DELTA_PANICS_FROM_ISR

#ifndef MCPX_BOOT_LIB

        //
        // If we got a delta panic, handle it here
        //

        if(rInterruptStatus.DeltaPanic)
        {
            HandleDeltaPanic();
        }

#ifdef MCPX_ENABLE_DELTA_WARNINGS

        else if(rInterruptStatus.DeltaWarning)
        {
            HandleDeltaWarning();
        }

#endif // MCPX_ENABLE_DELTA_WARNINGS

#endif // MCPX_BOOT_LIB

        //
        // If we got a voice interrupt, schedule a DPC to handle it
        //

        if(rInterruptStatus.FEVoice)
        {
            ScheduleApuInterruptDpc();
        }

#else // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

        //
        // If we got a voice interrupt or a delta panic, schedule a DPC to 
        // handle it
        //

        if(rInterruptStatus.DeltaPanic || rInterruptStatus.DeltaWarning || rInterruptStatus.FEVoice)
        {
            ScheduleApuInterruptDpc();
        }

#endif // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

        //
        // Read the interrupt status again to make sure everything's flushed
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
#define DPF_FNAME "CMcpxAPU::WaitForMagicWrite"

void
CMcpxAPU::WaitForMagicWrite
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
#define DPF_FNAME "CMcpxAPU::ServiceApuInterruptDpc"

void
CMcpxAPU::ServiceApuInterruptDpc
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

#ifndef MCPX_HANDLE_DELTA_PANICS_FROM_ISR

        if(m_arInterruptStatus[1].DeltaPanic)
        {
            HandleDeltaPanic();
        }

#ifdef MCPX_ENABLE_DELTA_WARNINGS

        else if(m_arInterruptStatus[1].DeltaWarning)
        {
            HandleDeltaWarning();
        }

#endif // MCPX_ENABLE_DELTA_WARNINGS

#endif // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

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
#define DPF_FNAME "CMcpxAPU::ServiceVoiceInterrupt"

void
CMcpxAPU::ServiceVoiceInterrupt
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CMcpxVoiceClient *      pClient;
    DWORD                   dwVoiceList;

#ifdef MCPX_ENABLE_ISR_DEBUGGING

    DWORD                   i;

#endif // MCPX_ENABLE_ISR_DEBUGGING

    //
    // Wait for the magic write to complete so that all notifiers are 
    // written
    //

    WaitForMagicWrite();

    //
    // Process all active voices.  Because the idle handler is allowed to run
    // while we're in this function, we need to protect ourselves against the
    // active voice list potentially being modified while we're walking it.
    // The best way to do this seems to be restarting at the head of the list
    // every time the list changes.  We can't guarantee that any node in the 
    // list will ever be valid, so we can't keep a back or next pointer.
    //

    for(dwVoiceList = 0; dwVoiceList < NUMELMS(m_alstActiveVoices); dwVoiceList++)
    {
        pleEntry = m_alstActiveVoices[dwVoiceList].Flink;

        while(pleEntry != &m_alstActiveVoices[dwVoiceList])
        {
            pClient = CONTAINING_RECORD(pleEntry, CMcpxVoiceClient, m_leActiveVoice);

#ifdef MCPX_ENABLE_ISR_DEBUGGING

            for(i = 0; i < MCPX_NOTIFIER_COUNT; i++)
            {
                if(pClient->m_Notifier.GetStatus(i))
                {
                    DPF_BLAB("Voice %x (%x) notifier %lu signaled", pClient, pClient->m_ahVoices[0], i);
                }
            }

#endif // MCPX_ENABLE_ISR_DEBUGGING

            pClient->ServiceVoiceInterrupt();

            BlockIdleHandler();
            
            if(IsEntryInList(pleEntry))
            {
                pleEntry = pleEntry->Flink;
            }
            else
            {
                pleEntry = m_alstActiveVoices[dwVoiceList].Flink;
            }

            UnblockIdleHandler();
        }
    }

    //
    // Process the pending inactive voice list, but this time just remove
    // every node from the list as we go.
    //

    while(TRUE)
    {
        BlockIdleHandler();
        
        pleEntry = RemoveHeadList(&m_lstPendingInactiveVoices);

        UnblockIdleHandler();

        if(&m_lstPendingInactiveVoices == pleEntry)
        {
            break;
        }
    
        pClient = CONTAINING_RECORD(pleEntry, CMcpxVoiceClient, m_lePendingInactiveVoice);

#ifdef MCPX_ENABLE_ISR_DEBUGGING

        if(!pClient->m_Notifier.GetStatus(MCPX_NOTIFIER_SSLA_DONE))
        {
            if(!pClient->m_Notifier.GetStatus(MCPX_NOTIFIER_SSLB_DONE))
            {
                if(!pClient->m_Notifier.GetStatus(MCPX_NOTIFIER_VOICE_OFF))
                {
                    ASSERTMSG("Voice has no signaled notifiers");
                }
            }
        }

#endif // MCPX_ENABLE_ISR_DEBUGGING

        pClient->ServiceVoiceInterrupt();
    }
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
#define DPF_FNAME "CMcpxAPU::GetInterruptStatusCallback"

BOOLEAN 
CMcpxAPU::GetInterruptStatusCallback
(
    LPVOID                  pvContext
)
{
    CMcpxAPU *              pThis   = (CMcpxAPU *)pvContext;

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
#define DPF_FNAME "CMcpxAPU::HandleFETrap"

VOID
CMcpxAPU::HandleFETrap
(
    void
)
{
    R_FE_CONTROL            rFeControl;
    DWORD                   dwParam;
    DWORD                   dwMethod;

    //
    // Whahoppen?
    //
    
    MCPX_REG_READ(NV_PAPU_FECTL, &rFeControl.uValue);

#ifdef MCPX_ENABLE_ISR_DEBUGGING

    ASSERT(NV_PAPU_FECTL_FEMETHMODE_TRAPPED == rFeControl.Mode);

#endif // MCPX_ENABLE_ISR_DEBUGGING

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
}


/****************************************************************************
 *
 *  HandleDeltaWarning
 *
 *  Description:
 *      Handles a delta warning (when the APU output read cursor gets too 
 *      close to the write cursor).
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
#define DPF_FNAME "CMcpxAPU::HandleDeltaWarning"

void
CMcpxAPU::HandleDeltaWarning
(
    void
)
{
    DWORD                   XGSCNT;
    DWORD                   DGSCNT;
    
    DPF_ENTER();

#if !defined(MCPX_HANDLE_DELTA_PANICS_FROM_ISR) || defined(MCPX_ENABLE_ISR_DEBUGGING)

    DPF_WARNING("Delta warning");

#endif // !defined(MCPX_HANDLE_DELTA_PANICS_FROM_ISR) || defined(MCPX_ENABLE_ISR_DEBUGGING)

    m_dwDeltaWarningCount++;

    MCPX_REG_READ(NV_PAPU_XGSCNT, &XGSCNT);
    MCPX_REG_READ(NV_PAPU_DGSCNT, &DGSCNT);
    
    MCPX_REG_WRITE(NV_PAPU_IGSCNT, XGSCNT + DGSCNT);
    
    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


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
#define DPF_FNAME "CMcpxAPU::HandleDeltaPanic"

void
CMcpxAPU::HandleDeltaPanic
(
    void
)
{
    BOOL                    fIdle;
    
    DPF_ENTER();

#if !defined(MCPX_HANDLE_DELTA_PANICS_FROM_ISR) || defined(MCPX_ENABLE_ISR_DEBUGGING)

    DPF_WARNING("Delta panic.  You'll hear a glitch while the APU is reset...");

#endif // !defined(MCPX_HANDLE_DELTA_PANICS_FROM_ISR) || defined(MCPX_ENABLE_ISR_DEBUGGING)
    
    m_dwDeltaPanicCount++;

#ifdef MCPX_HANDLE_DELTA_PANICS_FROM_ISR

    //
    // Shut down the setup engine
    //

    SetSetupEngineState(MCPX_SE_STATE_OFF);

#else // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

    //
    // Idle the voice processor
    //

    fIdle = IdleVoiceProcessor(TRUE);

#endif // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

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
    // Reset the EP
    //

    MCPX_REG_WRITE(NV_PAPU_EPRST, 1);

    SetupEncodeProcessor();

    //
    // Start the ACI running again
    //

    m_Ac97.m_apChannels[AC97_CHANNELTYPE_ANALOG]->Run();
    m_Ac97.m_apChannels[AC97_CHANNELTYPE_DIGITAL]->Run();

#ifdef MCPX_HANDLE_DELTA_PANICS_FROM_ISR

    //
    // Restart the setup engine
    //

    SetSetupEngineState(MCPX_SE_STATE_DEFAULT);

#else // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

    //
    // Restore the VP state
    //

    if(fIdle)
    {
        IdleVoiceProcessor(FALSE);
    }

#endif // MCPX_HANDLE_DELTA_PANICS_FROM_ISR

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
#define DPF_FNAME "CMcpxAPU::HandleSoftwareMethod"

void
CMcpxAPU::HandleSoftwareMethod
(
    DWORD                   dwMethod,
    DWORD                   dwParam
)
{
    switch(dwMethod)
    {
        case 0x8000:
            
            //
            // SE2FE_IDLE_VOICE is undocumented since it's an internal method
            //

            HandleIdleVoice(dwParam);

            break;

        default:
            
            //
            // Huh?
            //

            DPF_ERROR("Unexpected software method:  %x %x", dwMethod, dwParam);

            break;
    }
}


/****************************************************************************
 *
 *  HandleIdleVoice
 *
 *  Description:
 *      Handles the SE2FE_IDLE_VOICE hardware method by removing the voice
 *      from it's processing list.
 *
 *  Arguments:
 *      DWORD [in]: voice index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::HandleIdleVoice"

void
CMcpxAPU::HandleIdleVoice
(
    DWORD                   dwIdleVoice
)
{
    CMcpxVoiceClient *      pClient;
    DWORD                   dwVoiceFormat;
    
#ifdef MCPX_ENABLE_ISR_DEBUGGING

    DWORD                   dwVoiceState;

#endif // MCPX_ENABLE_ISR_DEBUGGING

    DEBUGLOG("HandleIdleVoice %x", dwIdleVoice);

    //
    // Check the voice index for validity.  The hardware sometimes likes
    // to have some fun with us.
    //

    if(dwIdleVoice < MCPX_HW_MAX_VOICES)
    {
        //
        // Make sure no-one's accessing the voice map.  If they are, we can
        // just ignore the idle message and it will get posted again on the
        // next frame.
        //

        if(!m_dwVoiceMapLock)
        {
            //
            // Don't remove PERSIST voices from the list.  When those voices
            // need to go away, they'll remove the PERSIST bit from their
            // format.
            //

#ifdef MCPX_ENABLE_ISR_DEBUGGING

            MCPX_VOICE_STRUCT_READ(dwIdleVoice, NV_PAVS_VOICE_PAR_STATE, &dwVoiceState);

#endif // MCPX_ENABLE_ISR_DEBUGGING

            MCPX_VOICE_STRUCT_READ(dwIdleVoice, NV_PAVS_VOICE_CFG_FMT, &dwVoiceFormat);

            if(!MCPX_GET_REG_VALUE(dwVoiceFormat, NV_PAVS_VOICE_CFG_FMT_PERSIST))
            {

#ifdef MCPX_ENABLE_ISR_DEBUGGING

                DPF_BLAB("Hardware reports voice %x is idle", dwIdleVoice);

                ASSERT(!MCPX_GET_REG_VALUE(dwVoiceState, NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE));

#endif // MCPX_ENABLE_ISR_DEBUGGING
        
                //
                // Get the voice client for the hardware voice
                //

                pClient = m_apVoiceMap[dwIdleVoice];

#ifdef MCPX_ENABLE_ISR_DEBUGGING

                ASSERT(pClient);

#endif // MCPX_ENABLE_ISR_DEBUGGING

#ifdef MCPX_DEBUG_STUCK_VOICES

                pClient->m_dwIgnoredTraps = 0;

#endif // MCPX_DEBUG_STUCK_VOICES
        
                //
                // We don't want to remove the voice from the processing list until the
                // last hardware voice is done processing.  Because the voices are always
                // placed into the processing list in order, the last voice in the array
                // should be the last one to finish.
                //

                if(dwIdleVoice == (DWORD)pClient->m_ahVoices[pClient->m_bVoiceCount - 1])
                {
                    if(IsEntryInList(&pClient->m_leActiveVoice))
                    {
                        pClient->RemoveIdleVoice();
                    }            
                    else
                    {
                        ASSERTMSG("Voice not in software active list");
                    }
                }
                else
                {

#ifdef MCPX_ENABLE_ISR_DEBUGGING

                    DPF_BLAB("Waiting for voice %x to idle...", pClient->m_ahVoices[pClient->m_bVoiceCount - 1]);

#endif // MCPX_ENABLE_ISR_DEBUGGING
        
                }
            }
            else
            {

#ifdef MCPX_ENABLE_ISR_DEBUGGING

                ASSERT(!MCPX_GET_REG_VALUE(dwVoiceState, NV_PAVS_VOICE_PAR_STATE_ACTIVE_VOICE));

#endif // MCPX_ENABLE_ISR_DEBUGGING

            }
        }

#ifdef MCPX_DEBUG_STUCK_VOICES

        else
        {
            pClient = m_apVoiceMap[dwIdleVoice];
            ASSERT(pClient);

            pClient->m_dwIgnoredTraps++;
        }

#endif // MCPX_DEBUG_STUCK_VOICES
        
    }

#if defined(MCPX_ENABLE_ISR_DEBUGGING) || defined(MCPX_DEBUG_STUCK_VOICES)

    else
    {
        DPF_ERROR("Internal hardware error -- invalid voice index in idle message (%x)", dwIdleVoice);
    }

#endif // defined(MCPX_ENABLE_ISR_DEBUGGING) || defined(MCPX_DEBUG_STUCK_VOICES)

}


/***************************************************************************
 *
 *  Commit3dSettings
 *
 *  Description:
 *      Commits deferred settings.
 *
 *  Arguments:
 *      DWORD [in]: parameter mask.
 *
 *  Returns:
 *      (void)
 *
 ***************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::Commit3dSettings"

DWORD
CMcpxAPU::Commit3dSettings
(
    void
)
{
    static const DWORD      dwStateOffset   = offsetof(DSFX_I3DL2REVERB_PARAMS, State.dwFlags);
    static const DWORD      dwParamOffset   = offsetof(DSFX_I3DL2REVERB_PARAMS, dwReflectionsInputDelay[0]);
    DWORD                   dwParameterMask;

    DPF_ENTER();

    //
    // Cache the parameter mask.  We're relying on the calling function to
    // clear it after all 3D voices have been recalculated.
    //
    
    dwParameterMask = m_pSettings->m_3dParams.dwParameterMask;

    //
    // Recalculate 3D
    //

    Calculate3d(dwParameterMask);

    //
    // Recalculate I3DL2
    //

    if((dwParameterMask & MCPX_3DCALC_I3DL2LISTENER) && (DSFX_IMAGELOC_UNUSED != m_pSettings->m_EffectLocations.dwI3DL2ReverbIndex))
    {
        //
        // Reverb effect data is made up of the following sections:
        //  - state block
        //  - delay line offsets/sizes
        //  - reverb parameters
        //
        // We're using this internal knowlege to tell us where to poke the
        // I3DL2 data.  If the reverb state block size ever changes, update
        // DSFX_I3DL2REVERB_PARAMS.
        //
        // Neither the state block nor the delay line data change between
        // calls to SetI3dl2Listener, so we'll start the changes after both
        // of those.
        //

        if(SUCCEEDED(GetEffectData(m_pSettings->m_EffectLocations.dwI3DL2ReverbIndex, 0, &m_I3dl2Data, dwParamOffset)))
        {
            //
            // In the Nov01 release, I accidentally broke I3DL2 by adding this
            // call to Initialize and failing to remove the Initialize call in
            // CI3dl2Listener::CalculateI3dl2.  If any games tweaked their
            // I3DL2 parameters based on this bug, we want them to be able to
            // get back to the broken behavior.
            //
            
            if(g_fDirectSoundI3DL2Overdelay)
            {
                CI3dl2Listener::Initialize();
            }

            CI3dl2Listener::CalculateI3dl2();

            m_I3dl2Data.State.dwFlags |= DSFX_STATE_UPDATE;

            SetEffectData(m_pSettings->m_EffectLocations.dwI3DL2ReverbIndex, dwStateOffset, &m_I3dl2Data.State.dwFlags, sizeof(m_I3dl2Data.State.dwFlags), DSFX_DEFERRED);
            SetEffectData(m_pSettings->m_EffectLocations.dwI3DL2ReverbIndex, dwParamOffset, (LPBYTE)&m_I3dl2Data + dwParamOffset, sizeof(DSFX_I3DL2REVERB_PARAMS) - dwParamOffset, DSFX_DEFERRED);
            
            CommitEffectData();
        }
    }

    DPF_LEAVE(dwParameterMask);

    return dwParameterMask;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  ScheduleDeferredCommand
 *
 *  Description:
 *      Adds a deferred voice command to the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      BOOL: TRUE if the command was scheduled.  If the timestamp for the
 *            command has already elapsed, it won't be scheduled and should
 *            be processed immediately.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::ScheduleDeferredCommand"

BOOL
CMcpxAPU::ScheduleDeferredCommand
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    BOOL                    fScheduled;
 
    DPF_ENTER();
    AutoIrql();

    if(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY)
    {
        fScheduled = ScheduleDeferredCommandLow(pCmd);
    }
    else
    {
        fScheduled = ScheduleDeferredCommandHigh(pCmd);
    }

    if(fScheduled)
    {
        pCmd->dwFlags |= MCPX_DEFERREDCMDF_SCHEDULED;
    }

    DPF_LEAVE(fScheduled);

    return fScheduled;
}


/****************************************************************************
 *
 *  ScheduleDeferredCommandHigh
 *
 *  Description:
 *      Adds a high-priority deferred voice command to the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      BOOL: TRUE if the command was scheduled.  If the timestamp for the
 *            command has already elapsed, it won't be scheduled and should
 *            be processed immediately.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::ScheduleDeferredCommandHigh"

BOOL
CMcpxAPU::ScheduleDeferredCommandHigh
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    REFERENCE_TIME          rtCurrentTime;
    PLIST_ENTRY             pleNextEntry;
    BOOL                    fScheduled;

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY));
    ASSERT(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_PERSIST));

    //
    // Check the current time.  If the timestamp has already elapsed,
    // we aren't going to schedule the command.
    //

    if(pCmd->rtTimestamp)
    {
        KeQuerySystemTime((PLARGE_INTEGER)&rtCurrentTime);

        if(pCmd->rtTimestamp <= 0)
        {
            pCmd->rtTimestamp = rtCurrentTime - pCmd->rtTimestamp;
        }

        if(fScheduled = (pCmd->rtTimestamp > rtCurrentTime))
        {
            //
            // If the command is already scheduled, remove it from the queue
            //

            if(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED)
            {
                RemoveDeferredCommand(pCmd);
            }

            //
            // Insert the command into the list in the order commands should be
            // processed.
            //

            for(pleNextEntry = m_lstDeferredCommandsHigh.Flink; pleNextEntry != &m_lstDeferredCommandsHigh; pleNextEntry = pleNextEntry->Flink)
            {
                AssertValidEntryList(pleNextEntry, ASSERT_IN_LIST);

                if(CONTAINING_RECORD(pleNextEntry, MCPX_DEFERRED_COMMAND, leListEntry)->rtTimestamp > pCmd->rtTimestamp)
                {
                    break;
                }
            }

            InsertTailList(pleNextEntry, &pCmd->leListEntry);

            //
            // If we just added the new item at the head of the list, set the timer
            //

            if(&pCmd->leListEntry == m_lstDeferredCommandsHigh.Flink)
            {
                KeSetTimer(&m_tmrDeferredCommandsHigh, *(PLARGE_INTEGER)&pCmd->rtTimestamp, &m_dpcDeferredCommandsHigh);
            }
        }
    }
    else
    {
        fScheduled = FALSE;
    }

    DPF_LEAVE(fScheduled);

    return fScheduled;
}


/****************************************************************************
 *
 *  ScheduleDeferredCommandLow
 *
 *  Description:
 *      Adds a low-priority deferred voice command to the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      BOOL: TRUE if the command was scheduled.  If the timestamp for the
 *            command has already elapsed, it won't be scheduled and should
 *            be processed immediately.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::ScheduleDeferredCommandLow"

BOOL
CMcpxAPU::ScheduleDeferredCommandLow
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    DWORD                   dwIndex;

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY);
    ASSERT(!pCmd->rtTimestamp);

    //
    // If the command is already in the queue, we don't need to do 
    // anything.  Low-priority commands don't use a timestamp, so the
    // list order is meaningless and there are no timers to reset.
    //

    if(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED))
    {
        InsertTailList(&m_lstDeferredCommandsLow, &pCmd->leListEntry);
    }

    DPF_LEAVE(TRUE);

    return TRUE;
}


/****************************************************************************
 *
 *  RemoveDeferredCommand
 *
 *  Description:
 *      Removes a deferred voice command from the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::RemoveDeferredCommand"

void
CMcpxAPU::RemoveDeferredCommand
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    DPF_ENTER();
    AutoIrql();

    if(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED)
    {
        if(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY)
        {
            RemoveDeferredCommandLow(pCmd);
        }
        else
        {
            RemoveDeferredCommandHigh(pCmd);
        }
    }

    pCmd->dwFlags &= ~MCPX_DEFERREDCMDF_SCHEDULED;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  RemoveDeferredCommandHigh
 *
 *  Description:
 *      Removes a deferred voice command from the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::RemoveDeferredCommandHigh"

void
CMcpxAPU::RemoveDeferredCommandHigh
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    BOOL                    fListHead;

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY));
    ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED);

    //
    // If this is the first command in the queue, we'll have some extra
    // processing to do.
    //

    fListHead = (&pCmd->leListEntry == m_lstDeferredCommandsHigh.Flink);

    //
    // Remove the command from the queue.  If we're in the middle of
    // processing the list, defer that to later.
    //

    RemoveEntryList(&pCmd->leListEntry);

    //
    // If that was the only item in the list, cancel the timer and remove
    // any pending DPCs.  If there's still items in the list, but we
    // removed the head, reset the timer.
    // 

    if(IsListEmpty(&m_lstDeferredCommandsHigh))
    {
        KeCancelTimer(&m_tmrDeferredCommandsHigh);
        KeRemoveQueueDpc(&m_dpcDeferredCommandsHigh);
    }
    else if(fListHead)
    {
        KeSetTimer(&m_tmrDeferredCommandsHigh, *(PLARGE_INTEGER)&CONTAINING_RECORD(m_lstDeferredCommandsHigh.Flink, MCPX_DEFERRED_COMMAND, leListEntry)->rtTimestamp, &m_dpcDeferredCommandsHigh);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  RemoveDeferredCommandLow
 *
 *  Description:
 *      Removes a deferred voice command from the queue.
 *
 *  Arguments:
 *      LPMCPX_DEFERRED_COMMAND [in/out]: command data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::RemoveDeferredCommandLow"

void
CMcpxAPU::RemoveDeferredCommandLow
(
    LPMCPX_DEFERRED_COMMAND pCmd
)
{
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY);
    ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED);

    //
    // Remove the command from the queue
    //

    RemoveEntryList(&pCmd->leListEntry);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceDeferredCommandsHigh
 *
 *  Description:
 *      Services high-priority deferred commands in the queue.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::ServiceDeferredCommandsHigh"

void
CMcpxAPU::ServiceDeferredCommandsHigh
(
    void
)
{
    LPMCPX_DEFERRED_COMMAND     pCmd;
    REFERENCE_TIME              rtCurrentTime;
    
    DPF_ENTER();
    AutoIrql();

    while(!IsListEmpty(&m_lstDeferredCommandsHigh))
    {
        pCmd = CONTAINING_RECORD(m_lstDeferredCommandsHigh.Flink, MCPX_DEFERRED_COMMAND, leListEntry);

        ASSERT(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY));
        ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED);
        ASSERT(!(pCmd->dwFlags & MCPX_DEFERREDCMDF_PERSIST));

        //
        // Get the current system time
        //

        KeQuerySystemTime((PLARGE_INTEGER)&rtCurrentTime);

        //
        // If the first command in the list's timestamp has expired, go ahead 
        // and trigger it and remove it from the queue.  If not, reset the
        // timer and bail out.
        //

        if(pCmd->rtTimestamp <= rtCurrentTime)
        {
            pCmd->dwFlags &= ~MCPX_DEFERREDCMDF_SCHEDULED;

            RemoveEntryList(&pCmd->leListEntry);

            pCmd->pVoice->ServiceDeferredCommand(pCmd->dwCommand, pCmd->dwContext);
        }
        else
        {
            KeSetTimer(&m_tmrDeferredCommandsHigh, *(PLARGE_INTEGER)&pCmd->rtTimestamp, &m_dpcDeferredCommandsHigh);
            break;
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceDeferredCommandsLow
 *
 *  Description:
 *      Services low-priority deferred commands in the queue.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::ServiceDeferredCommandsLow"

void
CMcpxAPU::ServiceDeferredCommandsLow
(
    void
)
{
    LIST_ENTRY              lstCommands;
    PLIST_ENTRY             pleEntry;
    LPMCPX_DEFERRED_COMMAND pCmd;
    CIrql                   irql;
    
    DPF_ENTER();

    //
    // Create a local copy of the command list.  We're doing this so we
    // can safely walk the list without worrying about nodes being added
    // or removed.
    //

    InitializeListHead(&lstCommands);

    irql.Raise();
    
    while(!IsListEmpty(&m_lstDeferredCommandsLow))
    {
        MoveEntryTailList(&lstCommands, m_lstDeferredCommandsLow.Flink);
    }

    //
    // Service all commands, removing them from the local list and adding
    // them back to the main list.
    //

    while((pleEntry = RemoveHeadList(&lstCommands)) != &lstCommands)
    {
        pCmd = CONTAINING_RECORD(pleEntry, MCPX_DEFERRED_COMMAND, leListEntry);

        ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY);
        ASSERT(pCmd->dwFlags & MCPX_DEFERREDCMDF_SCHEDULED);

        if(pCmd->dwFlags & MCPX_DEFERREDCMDF_PERSIST)
        {
            InsertTailList(&m_lstDeferredCommandsLow, pleEntry);
        }
        else
        {
            pCmd->dwFlags &= ~MCPX_DEFERREDCMDF_SCHEDULED;
        }

        irql.Lower();

        pCmd->pVoice->ServiceDeferredCommand(pCmd->dwCommand, pCmd->dwContext);

        irql.Raise();
    }

    ASSERT(IsListEmpty(&lstCommands));

    irql.Lower();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetHrtfHeadroom
 *
 *  Description:
 *      Sets the HRTF headroom.
 *
 *  Arguments:
 *      DWORD [in]: headroom amount.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::SetHrtfHeadroom"

HRESULT
CMcpxAPU::SetHrtfHeadroom
(
    DWORD                   dwHeadroom
)
{
    DPF_ENTER();
    AutoIrql();

    MCPX_CHECK_VOICE_FIFO(1);
    MCPX_VOICE_WRITE(SetHRTFHeadroom, MCPX_MAKE_REG_VALUE(dwHeadroom, NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT));

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetMixBinHeadroom
 *
 *  Description:
 *      Sets the headroom for one or more mixbins.
 *
 *  Arguments:
 *      DWORD [in]: mixbin.
 *      DWORD [in]: headroom amount.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::SetMixBinHeadroom"

HRESULT
CMcpxAPU::SetMixBinHeadroom
(
    DWORD                   dwMixBin
)
{
    DPF_ENTER();
    AutoIrql();

    MCPX_CHECK_VOICE_FIFO(1);
    MCPX_VOICE_WRITE(SetSubMixHeadroom[dwMixBin], MCPX_MAKE_REG_VALUE(m_pSettings->m_abMixBinHeadroom[dwMixBin], NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT));

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetSpeakerConfig
 *
 *  Description:
 *      Handles speaker configuration changes.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CMcpxAPU::SetSpeakerConfig"

HRESULT
CMcpxAPU::SetSpeakerConfig
(
    void
)
{
    HRESULT                 hr          = DS_OK;
    DSFX_CROSSTALK_STATE    Crosstalk;
    
    DPF_ENTER();

    //
    // If the headphone bit is set, disable the crosstalk effect
    //

    if(DSFX_IMAGELOC_UNUSED != m_pSettings->m_EffectLocations.dwCrosstalkIndex)
    {
        hr = GetEffectData(m_pSettings->m_EffectLocations.dwCrosstalkIndex, 0, &Crosstalk, sizeof(Crosstalk));

        if(SUCCEEDED(hr))
        {
            if(m_pSettings->m_dwSpeakerConfig & DSSPEAKER_ENABLE_HEADPHONES)
            {
                Crosstalk.dwFlags &= ~DSFX_STATE_GLOBAL;
            }
            else
            {
                Crosstalk.dwFlags |= DSFX_STATE_GLOBAL;
            }

            hr = SetEffectData(m_pSettings->m_EffectLocations.dwCrosstalkIndex, 0, &Crosstalk, sizeof(Crosstalk), DSFX_IMMEDIATE);
        }
    }

    //
    // Update the 3D listener's surround flag.  We're relying on te caller
    // to trigger the recalc of the listener and all 3D sources.
    //

    if(SUCCEEDED(hr))
    {
        if(DSSPEAKER_IS_HEADPHONES(m_pSettings->m_dwSpeakerConfig))
        {
            m_fSurround = FALSE;
        }
        else
        {
            m_fSurround = DSSPEAKER_IS_SURROUND(m_pSettings->m_dwSpeakerConfig);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}

#endif // MCPX_BOOT_LIB


