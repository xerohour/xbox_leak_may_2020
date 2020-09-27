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

namespace DirectSound
{
    class CMcpxVoiceClient;
    class CMcpxBuffer;
    class CMcpxStream;
}

//
// Deferred commands
//

#define MCPX_DEFERREDCMDF_LOWPRIORITY   0x00000001
#define MCPX_DEFERREDCMDF_SCHEDULED     0x00000002
#define MCPX_DEFERREDCMDF_PERSIST       0x00000004

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY                      leListEntry;
    DWORD                           dwFlags;
    DirectSound::CMcpxVoiceClient * pVoice;
    DWORD                           dwCommand;
    DWORD                           dwContext;
    REFERENCE_TIME                  rtTimestamp;
END_DEFINE_STRUCT(MCPX_DEFERRED_COMMAND);

//
// Miscelaneous APU state flags
//

#define MCPX_APUSTATE_GPIMAGE           0x00000001

//
// Audio device implementation
//

namespace DirectSound
{
    class CMcpxAPU
        : public CRefCount, public CMcpxCore

#ifndef MCPX_BOOT_LIB

        , protected CHrtfListener, protected CI3dl2Listener

#endif // MCPX_BOOT_LIB

    {
        friend class CMcpxVoiceClient;

    public:
        DWORD &                     m_dwFree2dVoiceCount;                       // Count of free 2D voices
        DWORD &                     m_dwFree3dVoiceCount;                       // Count of free 3D voices
        LIST_ENTRY                  m_lst3dVoices;                              // List of 3D voices
        CMcpxBufferSgeHeap          m_SgeHeap;                                  // Buffer scatter-gather heap object
        DWORD                       m_dwState;                                  // APU state flags
                                                                            
    protected:                                                                  
        volatile DWORD              m_dwVoiceMapLock;                           // Voice map/list lock count
        CMcpxVoiceClient *          m_apVoiceMap[MCPX_HW_MAX_VOICES];           // Voice usage map
        LIST_ENTRY                  m_alstActiveVoices[MCPX_VOICELIST_COUNT];   // Active voice list
        LIST_ENTRY                  m_lstPendingInactiveVoices;                 // Pending inactive voice list
        LIST_ENTRY                  m_lstDeferredCommandsHigh;                  // Voice deferred command list (high-priority)
        LIST_ENTRY                  m_lstDeferredCommandsLow;                   // Voice deferred command list (low-priority)
        R_INTR                      m_arInterruptStatus[2];                     // Status bits from last interrupt received
        KDPC                        m_dpcInterrupt;                             // Interrupt handler DPC object
        static KINTERRUPT           m_Interrupt;                                // Interrupt object
        KTIMER                      m_tmrDeferredCommandsHigh;                  // Voice deferred command timer
        KDPC                        m_dpcDeferredCommandsHigh;                  // Voice deferred command DPC
        HAL_SHUTDOWN_REGISTRATION   m_HalShutdownData;                          // HAL shutdown registration data

    private:
        static DWORD                m_dwDeltaPanicCount;                        // Delta panic counter
        static DWORD                m_dwDeltaWarningCount;                      // Delta warning counter

#ifdef DEBUG

        static BOOL                 m_fSimulateDeltaPanic;                      // Delta panic simulator
        static BOOL                 m_fSimulateDeltaWarning;                    // Delta warning simulator

#endif // DEBUG

    public:
        CMcpxAPU(CDirectSoundSettings *pSettings);
        virtual ~CMcpxAPU(void);

    public:
        // Initialization
        HRESULT Initialize(void);

        // 3D listener properties
        DWORD Commit3dSettings(void);

        // GP DSP
        HRESULT DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPDSEFFECTIMAGEDESC *ppImageDesc);
        HRESULT SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags);
        HRESULT GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize);
        HRESULT CommitEffectData(void);

        // Device properties
        HRESULT SetHrtfHeadroom(DWORD dwHeadroom);
        HRESULT SetMixBinHeadroom(DWORD dwMixBin);
        HRESULT SetSpeakerConfig(void);

        // Voice allocation
        HRESULT AllocateVoices(CMcpxVoiceClient *pVoice);
        void FreeVoices(CMcpxVoiceClient *pVoice);

        // Synchronization
        void BlockIdleHandler(void);
        void UnblockIdleHandler(void);

        // Deferred voice commands
        BOOL ScheduleDeferredCommand(LPMCPX_DEFERRED_COMMAND pCmd);
        void RemoveDeferredCommand(LPMCPX_DEFERRED_COMMAND pCmd);

        // Work, work, work
        void DoWork(void);

    protected:
        // Voice allocation
        static DWORD AllocateVoice(LPDWORD adwVoiceMap, DWORD dwElementCount);
        static void FreeVoice(DWORD dwVoice, LPDWORD adwVoiceMap, DWORD dwElementCount);

        // Shutdown
        void Terminate(void);
    
        // Interrupt handlers
        BOOL ServiceApuInterrupt(void);
        void ServiceApuInterruptDpc(void);
        void ServiceVoiceInterrupt(void);
        void WaitForMagicWrite(void);
        void HandleFETrap(void);

#ifndef MCPX_BOOT_LIB

        void HandleDeltaWarning(void);
        void HandleDeltaPanic(void);

#endif // MCPX_BOOT_LIB

        void HandleSoftwareMethod(DWORD dwMethod, DWORD dwParam);
        void HandleIdleVoice(DWORD dwIdleVoice);

        // Deferred voice commands
        BOOL ScheduleDeferredCommandHigh(LPMCPX_DEFERRED_COMMAND pCmd);
        BOOL ScheduleDeferredCommandLow(LPMCPX_DEFERRED_COMMAND pCmd);
        void ServiceDeferredCommandsHigh(void);
        void ServiceDeferredCommandsLow(void);
        void RemoveDeferredCommandHigh(LPMCPX_DEFERRED_COMMAND pCmd);
        void RemoveDeferredCommandLow(LPMCPX_DEFERRED_COMMAND pCmd);

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

    __inline HRESULT CMcpxAPU::DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPDSEFFECTIMAGEDESC *ppImageDesc)
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

    __inline HRESULT CMcpxAPU::SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags)
    {
        ASSERT(m_pGpDspManager);
        ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

        return m_pGpDspManager->SetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize, dwFlags);
    }
    
    __inline HRESULT CMcpxAPU::GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize)
    {
        ASSERT(m_pGpDspManager);
        ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

        return m_pGpDspManager->GetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize);
    }
    
    __inline HRESULT CMcpxAPU::CommitEffectData(void)
    {
        ASSERT(m_pGpDspManager);
        ASSERT(m_dwState & MCPX_APUSTATE_GPIMAGE);

        m_pGpDspManager->CommitChanges(0, 0);

        return DS_OK;
    }
    
    __inline BOOLEAN CMcpxAPU::ApuInterruptServiceRoutine(PKINTERRUPT pInterrupt, LPVOID pvContext)
    {
        return (BOOLEAN)((CMcpxAPU *)pvContext)->ServiceApuInterrupt();
    }

    __inline void CMcpxAPU::ApuInterruptDpcRoutine(PKDPC pDpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2)
    {
        ((CMcpxAPU *)pvDeferredContext)->ServiceApuInterruptDpc();
    }

    __inline void CMcpxAPU::DeferredCommandDpcRoutine(PKDPC pdpc, LPVOID pvDeferredContext, LPVOID pvSystemContext1, LPVOID pvSystemContext2)
    {
        ((CMcpxAPU *)pvDeferredContext)->ServiceDeferredCommandsHigh();
    }

    __inline void CMcpxAPU::ApuShutdownNotifier(PHAL_SHUTDOWN_REGISTRATION pHalShutdownData)
    {
        g_fDirectSoundInFinalRelease = TRUE;
    
        CONTAINING_RECORD(pHalShutdownData, CMcpxAPU, m_HalShutdownData)->Terminate();
    }

    __inline void CMcpxAPU::DoWork(void)
    {
        ServiceDeferredCommandsLow();
    }

    __inline void CMcpxAPU::BlockIdleHandler(void)
    {
        ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);
        ASSERT(m_dwVoiceMapLock < ~0UL);
        m_dwVoiceMapLock++;
    }

    __inline void CMcpxAPU::UnblockIdleHandler(void)
    {
        ASSERT(KeGetCurrentIrql() >= DISPATCH_LEVEL);
        ASSERT(m_dwVoiceMapLock >= 1);
        m_dwVoiceMapLock--;
    }

    __inline void CMcpxAPU::ScheduleApuInterruptDpc(void)
    {
        KeInsertQueueDpc(&m_dpcInterrupt, NULL, NULL);
    }
}

#endif // __cplusplus

#endif // __MCPAPU_H__
