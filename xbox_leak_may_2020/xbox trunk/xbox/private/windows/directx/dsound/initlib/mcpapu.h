/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpapu.h
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created based on NVidia/georgioc code.
 *
 ****************************************************************************/

#ifndef __MCPAPU_H__
#define __MCPAPU_H__

//
// Voice indeces
//

#define MCPX_IS_3D_VOICE(hVoice) \
    ((DWORD)hVoice < MCPX_HW_MAX_3D_VOICES)

#ifdef __cplusplus

//
// Forward declarations
//

class CMcpxVoiceClient;
class CMcpxBuffer;
class CMcpxStream;

//
// Deferred commands.  Command indeces must be numbered from 0 up, and must
// have low-priority commands in the lowest numbers.
//

#define MCPX_DEFERREDCMDF_LOWPRIORITY   0x00000001
#define MCPX_DEFERREDCMDF_SCHEDULED     0x00000002
#define MCPX_DEFERREDCMDF_PERSIST       0x00000004

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY              leListEntry;
    DWORD                   dwFlags;
    CMcpxVoiceClient *      pVoice;
    DWORD                   dwCommand;
    DWORD                   dwContext;
    REFERENCE_TIME          rtTimestamp;
END_DEFINE_STRUCT(MCPX_DEFERRED_COMMAND);

BEGIN_DEFINE_ENUM()
    MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES = 0,
    MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA,
    MCPX_DEFERREDCMD_BUFFER_PLAY,
    MCPX_DEFERREDCMD_BUFFER_STOP,
    MCPX_DEFERREDCMD_BUFFER_COUNT
END_DEFINE_ENUM_();

#define MCPX_DEFERREDCMD_BUFFER_LOWPRIOCOUNT    (MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA + 1)

BEGIN_DEFINE_ENUM()
    MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES = 0,
    MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS,
    MCPX_DEFERREDCMD_STREAM_FLUSH,
    MCPX_DEFERREDCMD_STREAM_STOP,
    MCPX_DEFERREDCMD_STREAM_COUNT
END_DEFINE_ENUM_();

#define MCPX_DEFERREDCMD_STREAM_LOWPRIOCOUNT    (MCPX_DEFERREDCMD_STREAM_FLUSH + 1)

//
// Miscelaneous APU state flags
//

#define MCPX_APUSTATE_GPIMAGE           0x00000001

//
// Audio device implementation
//

class CMcpxAPUTest
    : public CRefCountTest, public CMcpxCoreTest

{
    friend class CMcpxVoiceClient;

public:
    WORD                        m_wFree2dVoiceCount;                                    // Count of free 2D voices
    WORD                        m_wFree3dVoiceCount;                                    // Count of free 3D voices
    LIST_ENTRY                  m_lst3dVoices;                                          // List of 3D voices
    CMcpxBufferSgeHeapTest          m_SgeHeap;                                              // Buffer scatter-gather heap object
    DWORD                       m_dwState;                                              // APU state flags
                                                                                        
protected:                                                                              
    volatile DWORD              m_dwVoiceMapLock;                                       // Voice map/list lock count
    LIST_ENTRY                  m_alstActiveVoices[MCPX_VOICELIST_COUNT];               // Active voice list
    LIST_ENTRY                  m_lstPendingInactiveVoices;                             // Pending inactive voice list
    LIST_ENTRY                  m_lstDeferredCommandsHigh;                              // Voice deferred command list (high-priority)
    LIST_ENTRY                  m_lstDeferredCommandsLow;                               // Voice deferred command list (low-priority)
    R_INTR                      m_arInterruptStatus[2];                                 // Status bits from last interrupt received
    KDPC                        m_dpcInterrupt;                                         // Interrupt handler DPC object
    static KINTERRUPT           m_Interrupt;                                            // Interrupt object
    KTIMER                      m_tmrDeferredCommandsHigh;                              // Voice deferred command timer
    KDPC                        m_dpcDeferredCommandsHigh;                              // Voice deferred command DPC
    HAL_SHUTDOWN_REGISTRATION   m_HalShutdownData;                                      // HAL shutdown registration data

private:
    static DWORD                m_dwDeltaPanicCount;                                    // Delta panic counter

public:
    CMcpxAPUTest(CDirectSoundTestSettings *pSettings);
    virtual ~CMcpxAPUTest(void);

public:
    // Initialization
    HRESULT Initialize(void);

    // GP DSP
    HRESULT DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPDSEFFECTIMAGEDESC *ppImageDesc);
    HRESULT SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags);
    HRESULT GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize);
    HRESULT CommitEffectData(void);


    // Synchronization
    void BlockIdleHandler(void);
    void UnblockIdleHandler(void);

    // Deferred voice commands
    BOOL ScheduleDeferredCommand(LPMCPX_DEFERRED_COMMAND pCmd);
    void RemoveDeferredCommand(LPMCPX_DEFERRED_COMMAND pCmd);

    // Work, work, work
    void DoWork(void);

protected:

    // Shutdown
    void Terminate(void);
    
    // Interrupt handlers
    BOOL ServiceApuInterrupt(void);
    void ServiceApuInterruptDpc(void);
    void ServiceVoiceInterrupt(void);
    void WaitForMagicWrite(void);
    void HandleFETrap(void);

#ifndef MCPX_BOOT_LIB

    void HandleDeltaPanic(void);

#endif // MCPX_BOOT_LIB

    void HandleSoftwareMethod(DWORD dwMethod, DWORD dwParam);

    // Interrupt service
    void ScheduleApuInterruptDpc(void);
    void GetInterruptStatus(void);

private:
    static BOOLEAN ApuInterruptServiceRoutine(PKINTERRUPT pInterrupt, LPVOID pvContext);
    static void ApuInterruptDpcRoutine(PKDPC pDpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2);
    static BOOLEAN GetInterruptStatusCallback(LPVOID pvContext);
    static void DeferredCommandDpcRoutine(PKDPC pdpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2);
    static void ApuShutdownNotifier(PHAL_SHUTDOWN_REGISTRATION pHalShutdownData);
};


__inline HRESULT CMcpxAPUTest::DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPDSEFFECTIMAGEDESC *ppImageDesc)
{
    HRESULT                 hr;
    
    ASSERT(m_pGpDspManager);

    if(SUCCEEDED(hr = m_pGpDspManager->DownloadEffectsImage(pvImageBuffer, dwImageSize, ppImageDesc)))
    {
        m_dwState |= MCPX_APUSTATE_GPIMAGE;
    }
    else
    {
        m_dwState &= ~MCPX_APUSTATE_GPIMAGE;
    }

    return hr;
}

__inline HRESULT CMcpxAPUTest::SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags)
{
    ASSERT(m_pGpDspManager);
    ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

    return m_pGpDspManager->SetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize, dwFlags);
}
    
__inline HRESULT CMcpxAPUTest::GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize)
{
    ASSERT(m_pGpDspManager);
    ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

    return m_pGpDspManager->GetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize);
}
    
__inline HRESULT CMcpxAPUTest::CommitEffectData(void)
{
    ASSERT(m_pGpDspManager);
    ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

    m_pGpDspManager->CommitChanges(0, 0);

    return DS_OK;
}
    
__inline BOOLEAN CMcpxAPUTest::ApuInterruptServiceRoutine(PKINTERRUPT pInterrupt, LPVOID pvContext)
{
    return (BOOLEAN)((CMcpxAPUTest *)pvContext)->ServiceApuInterrupt();
}

__inline void CMcpxAPUTest::ApuInterruptDpcRoutine(PKDPC pDpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2)
{
    ((CMcpxAPUTest *)pvDeferredContext)->ServiceApuInterruptDpc();
}

__inline void CMcpxAPUTest::DeferredCommandDpcRoutine(PKDPC pdpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2)
{
}

__inline void CMcpxAPUTest::ApuShutdownNotifier(PHAL_SHUTDOWN_REGISTRATION pHalShutdownData)
{
    g_fDirectSoundTestInFinalRelease = TRUE;
    
    CONTAINING_RECORD(pHalShutdownData, CMcpxAPUTest, m_HalShutdownData)->Terminate();
}


__inline void CMcpxAPUTest::BlockIdleHandler(void)
{
    ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);
    ASSERT(m_dwVoiceMapLock < ~0UL);
    m_dwVoiceMapLock++;
}

__inline void CMcpxAPUTest::UnblockIdleHandler(void)
{
    ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);
    ASSERT(m_dwVoiceMapLock >= 1);
    m_dwVoiceMapLock--;
}

__inline void CMcpxAPUTest::ScheduleApuInterruptDpc(void)
{
    KeInsertQueueDpc(&m_dpcInterrupt, NULL, NULL);
}

#endif // __cplusplus

#endif // __MCPAPU_H__
