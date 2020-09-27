/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpxstrm.h
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created based on NVidia/georgioc code.
 *
 ****************************************************************************/

#ifndef __MCPSTRM_H__
#define __MCPSTRM_H__

//
// Stream data
//

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY          leListEntry;        // List entry
    XMEDIAPACKET        xmpPacket;          // Packet data
    DWORD               dwCompletedSize;    // Amount of the packet that's been mapped into an SSL
    DWORD               dwStatus;           // Packet status (used by the deferred completion list)
    LONGLONG            rtTimestamp;        // Packet timestamp
END_DEFINE_STRUCT(MCPX_PACKET_CONTEXT);

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY          lstPackets;         // List of packets mapped into the SSL
    DWORD               dwPrdCount;         // Count of PRDs filled
    DWORD               dwBytesMapped;      // Count of bytes mapped into the SSL
END_DEFINE_STRUCT(MCPX_SSL_DESC);

//
// Return codes from CMcpxStream::Process
//

#define MCPX_STREAM_PROCESS_COMPLETEDSSL    0x00000001
#define MCPX_STREAM_PROCESS_MAPPEDDATA      0x00000002
#define MCPX_STREAM_PROCESS_MASK            0x00000003

//
// Deferred stream commands
//

#define MCPX_DEFERREDCMD_STREAM_CHECKSTUCK MCPX_DEFERREDCMD_VOICE_CHECKSTUCK

BEGIN_DEFINE_ENUM()
    MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES = MCPX_DEFERREDCMD_VOICE_COUNT,
    MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS,
    MCPX_DEFERREDCMD_STREAM_FLUSH,
    MCPX_DEFERREDCMD_STREAM_STOP,
    MCPX_DEFERREDCMD_STREAM_PAUSE,
    MCPX_DEFERREDCMD_STREAM_COUNT
END_DEFINE_ENUM_();

#ifdef __cplusplus

//
// Audio stream implementation
//

namespace DirectSound
{
    class CMcpxStream
        : public CMcpxVoiceClient
    {
    protected:
        CDirectSoundStreamSettings *    m_pSettings;                                        // Shared stream settings
        DWORD                           m_dwPrdControl;                                     // PRD control value
        MCPX_SSL_DESC                   m_aSslDesc[MCPX_HW_SSLS_PER_VOICE];                 // SSL descriptors
        LIST_ENTRY                      m_lstPending;                                       // Pending packet list
        LIST_ENTRY                      m_lstAvailable;                                     // Available packet list
        LIST_ENTRY                      m_lstCompleted;                                     // Completed packet list
        PMCPX_PACKET_CONTEXT            m_paPacketContexts;                                 // Packet contexts
        PMCPX_PACKET_CONTEXT            m_pWorkingPacket;                                   // Current packet (only used when packet size is too large for SSL)
        DWORD                           m_dwWorkingPacketOffset;                            // Current read offset of working packet
        MCPX_DEFERRED_COMMAND           m_aDeferredCommands[MCPX_DEFERREDCMD_STREAM_COUNT]; // Deferred command data    
        DWORD                           m_dwFirstMappedSslIndex;                            // Index of the first mapped SSL
        DWORD                           m_dwMappedSslCount;                                 // Count of Ssls currently mapped

    public:
        CMcpxStream(CMcpxAPU *pAPU, CDirectSoundStreamSettings *pSettings);
        virtual ~CMcpxStream(void);

    public:
        // Initialization
        HRESULT Initialize(void);

        // Stream state
        HRESULT Pause(DWORD dwPause);
        HRESULT Pause(REFERENCE_TIME rtTimeStamp, DWORD dwPause);
        HRESULT Stop(DWORD dwFlags = 0);
        HRESULT Stop(REFERENCE_TIME rtTimeStamp, DWORD dwFlags = 0);
    
        // Stream status
        HRESULT GetStatus(LPDWORD pdwStatus);
        BOOL HasPendingData(void);

        // Stream properties
        HRESULT SetFormat(void);
        DWORD GetLowWatermark(void);

        // Stream data
        HRESULT SubmitPacket(REFXMEDIAPACKET Packet);
        HRESULT Discontinuity(void);
        HRESULT Flush(void);

    protected:
        // Hardware voice resources
        HRESULT AllocateStreamResources(void);
        void ReleaseStreamResources(void);

        // SSL management
        DWORD Process(void);
        BOOL MapPackets(DWORD dwSslIndex);
        DWORD MapPacket(DWORD dwSslIndex, PMCPX_PACKET_CONTEXT pPacketContext);
        void CommitSsl(DWORD dwSslIndex);
        void CompleteSsl(DWORD dwSslIndex, DWORD dwStatus);
        DWORD GetSslBase(DWORD dwVoiceIndex, DWORD dwSslIndex);

        // Packet list
        void CompletePackets(PLIST_ENTRY pListHead, DWORD dwStatus);
        void CompleteDeferredPackets(void);
        void CompletePacket(PMCPX_PACKET_CONTEXT pPacketContext, DWORD dwStatus);
        void OnDeferredFlush(void);

        // Interrupt handler
        BOOL ServiceVoiceInterrupt(void);

        // Deferred commands
        BOOL ScheduleDeferredCommand(DWORD dwCommand, REFERENCE_TIME rtTimeStamp, DWORD dwContext);
        void RemoveDeferredCommand(DWORD dwCommand);
        void ServiceDeferredCommand(DWORD dwCommand, DWORD dwContext);

        // Voice state
        void OnEndOfStream(void);
    };

    __inline DWORD CMcpxStream::GetLowWatermark(void)
    {
        return max((m_pSettings->m_fmt.nChannels * (DWORD)m_pSettings->m_fmt.wBitsPerSample / 8) * 32, m_pSettings->m_fmt.nBlockAlign) * 2;
    }

    __inline DWORD CMcpxStream::GetSslBase(DWORD dwVoiceIndex, DWORD dwSslIndex)
    {
        return ((DWORD)m_ahVoices[dwVoiceIndex] * MCPX_HW_MAX_PRD_ENTRIES_PER_VOICE) + (dwSslIndex * MCPX_HW_MAX_PRD_ENTRIES_PER_SSL);
    }

    __inline HRESULT CMcpxStream::AllocateStreamResources(void)
    {
        return CMcpxVoiceClient::AllocateVoiceResources();
    }

    __inline BOOL CMcpxStream::HasPendingData(void)
    {
        return !(IsListEmpty(&m_lstPending) && !m_dwMappedSslCount);
    }
}

#endif // __cplusplus

#endif // __MCPSTRM_H__
