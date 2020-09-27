/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpbuf.h
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  03/16/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __MCPBUF_H__
#define __MCPBUF_H__

//
// Deferred buffer commands
//

#define MCPX_DEFERREDCMD_BUFFER_CHECKSTUCK MCPX_DEFERREDCMD_VOICE_CHECKSTUCK

BEGIN_DEFINE_ENUM()
    MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES = MCPX_DEFERREDCMD_VOICE_COUNT,
    MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA,
    MCPX_DEFERREDCMD_BUFFER_PLAY,
    MCPX_DEFERREDCMD_BUFFER_STOP,
    MCPX_DEFERREDCMD_BUFFER_COUNT
END_DEFINE_ENUM_();

#ifdef __cplusplus

//
// Audio buffer implementation
//

namespace DirectSound
{
    class CMcpxBuffer
        : public CMcpxVoiceClient
    {
        friend class CMcpxAPU;
        friend class CMcpxVoiceClient;

    protected:
        CDirectSoundBufferSettings *    m_pSettings;                                        // Shared buffer settings
        LPSGEHEAPRUNMARKER              m_pSgeHeapEntry;                                    // SGE heap allocation
        MCPX_DEFERRED_COMMAND           m_aDeferredCommands[MCPX_DEFERREDCMD_BUFFER_COUNT]; // Deferred command data    
        DWORD                           m_dwCachedPlayCursor;                               // Cached for LOCDEFER
        DWORD                           m_dwLastNotifyPosition;                             // Last signaled notification position
        DWORD                           m_dwNextNotifyIndex;                                // Next notification index to signal
        DWORD                           m_dwBufferBase;                                     // Buffer base address register

    public:
        CMcpxBuffer(CMcpxAPU *pAPU, CDirectSoundBufferSettings *pSettings);
        virtual ~CMcpxBuffer(void);

    public:
        // Initialization
        HRESULT Initialize(void);

        // Buffer state
        HRESULT Play(DWORD dwFlags);
        HRESULT Play(REFERENCE_TIME rtTimeStamp, DWORD dwFlags);
        HRESULT Stop(DWORD dwFlags = 0);
        HRESULT Stop(REFERENCE_TIME rtTimeStamp, DWORD dwFlags = 0);
        HRESULT GetStatus(LPDWORD pdwStatus);

        // Buffer data
        HRESULT SetBufferData(void);
        HRESULT ReleaseBufferData(BOOL fChanging = FALSE);

        // Buffer play and loop regions
        HRESULT SetPlayRegion(void);
        HRESULT SetLoopRegion(void);

        // Buffer position
        HRESULT GetCurrentPosition(LPDWORD pdwPlayCursor, LPDWORD pdwWriteCursor);
        HRESULT SetCurrentPosition(DWORD dwPlayCursor);

        // Notifications
        HRESULT SetNotificationPositions(void);

    protected:
        // Hardware voice resources
        HRESULT AllocateBufferResources(void);
        void ReleaseBufferResources(void);

        // Buffer state
        void PlayFromCurrent(DWORD dwFlags);
        void PlayFromPosition(DWORD dwPosition, DWORD dwFlags);
    
        // Buffer data
        HRESULT MapInputBuffer(void);
        void MapEffectsBuffer(void);
        void MapBuffer(void);
        void MapBuffer(DWORD dwOffset);
        void UnmapBuffer(void);

        // Deferred commands
        BOOL ScheduleDeferredCommand(DWORD dwCommand, REFERENCE_TIME rtTimeStamp, DWORD dwContext);
        void RemoveDeferredCommand(DWORD dwCommand);
        void ServiceDeferredCommand(DWORD dwCommand, DWORD dwContext);

        // Buffer events
        void OnPositionDelta(void);
        void NotifyToPosition(DWORD dwPlayCursor, BOOL fSignal = TRUE);
        void NotifyStop(void);

        // Interrupt handler
        BOOL ServiceVoiceInterrupt(void);
        void OnDeferredTerminate(void);
    };

    __inline void CMcpxBuffer::MapBuffer(void)
    {
        MapBuffer(m_pSettings->m_dwPlayStart);
    }
}

#endif // __cplusplus

#endif // __MCPBUF_H__
