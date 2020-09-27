/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpxstrm.cpp
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
 *  CMcpxStream
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CMcpxAPU * [in]: pointer back to the APU object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CMcpxStream"

CMcpxStream::CMcpxStream
(
    CMcpxAPU *                      pMcpxApu,
    CDirectSoundStreamSettings *    pSettings
)
:   CMcpxVoiceClient(pMcpxApu, pSettings)
{
    DWORD                   i;
    
    DPF_ENTER();

    m_pSettings = ADDREF(pSettings);

    InitializeListHead(&m_lstAvailable);
    InitializeListHead(&m_lstPending);
    InitializeListHead(&m_lstCompleted);

    for(i = 0; i < NUMELMS(m_aSslDesc); i++)
    {
        InitializeListHead(&m_aSslDesc[i].lstPackets);
    }

    //
    // Initialize deferred command structures
    //

    for(i = 0; i < NUMELMS(m_aDeferredCommands); i++)
    {
        InitializeListHead(&m_aDeferredCommands[i].leListEntry);
        
        m_aDeferredCommands[i].pVoice = this;
        m_aDeferredCommands[i].dwCommand = i;
    }

    m_aDeferredCommands[MCPX_DEFERREDCMD_STREAM_CHECKSTUCK].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;
    m_aDeferredCommands[MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY;
    m_aDeferredCommands[MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;
    m_aDeferredCommands[MCPX_DEFERREDCMD_STREAM_FLUSH].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;

    //
    // Set the PERSIST bit in the voice status so we'll only be removed from
    // the voice list when we explicitly stop.  This helps us recover from
    // starvation.
    //

    or(&m_dwStatus, MCPX_VOICESTATUS_PERSIST);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxStream
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
#define DPF_FNAME "CMcpxStream::~CMcpxStream"

CMcpxStream::~CMcpxStream
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Make sure all deferred commands are dequeued
    //

    for(i = 0; i < NUMELMS(m_aDeferredCommands); i++)
    {
        RemoveDeferredCommand(i);
    }

    //
    // Flush the stream
    //

    Flush();

    //
    // Release hardware resources
    //

    ReleaseStreamResources();

    //
    // Release the settings object
    //
    
    RELEASE(m_pSettings);

    //
    // Free memory
    //

    MEMFREE(m_paPacketContexts);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      CDirectSoundStreamSettings * [in]: shared stream settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Initialize"

HRESULT
CMcpxStream::Initialize
(
    void
)
{
    HRESULT                         hr;
    DWORD                           i;
    
    DPF_ENTER();

    //
    // Call the base class
    //

    hr = CMcpxVoiceClient::Initialize(TRUE);

    //
    // Allocate a fixed amount of context packets
    //

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_paPacketContexts = MEMALLOC(MCPX_PACKET_CONTEXT, m_pSettings->m_dwMaxAttachedPackets));
    }

    //
    // Initialize context lists used to track submitted XMEDIABUFFERS
    //

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < m_pSettings->m_dwMaxAttachedPackets; i++) 
        {
            InsertTailListUninit(&m_lstAvailable, &m_paPacketContexts[i].leListEntry);
        }
    }

    //
    // If the stream was created without LOCDEFER, allocate voice resources now
    //

    if(SUCCEEDED(hr) && !(m_pSettings->m_dwFlags & DSSTREAMCAPS_LOCDEFER))
    {
        hr = AllocateStreamResources();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetFormat
 *
 *  Description:
 *      Sets the voice format.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::SetFormat"

HRESULT 
CMcpxStream::SetFormat
(
    void
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Hand off to the base class
    //

    hr = CMcpxVoiceClient::SetFormat();

    //
    // Setup PRD control
    //

    if(SUCCEEDED(hr))
    {
        switch(m_pSettings->m_fmt.wFormatTag)
        {
            case WAVE_FORMAT_PCM:
                switch(m_pSettings->m_fmt.wBitsPerSample)
                {
                    case 8:
                        m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B8, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE);
                        break;

                    case 16:
                        m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B16, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE);
                        break;

                    case 32: 
                        m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B32, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE);
                        break;

                    default:
                        ASSERTMSG("Unexpected sample container size");
                        break;
                }

                break;

            case WAVE_FORMAT_XBOX_ADPCM:
                m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE);
                break;

            default:
                ASSERTMSG("Unexpected format tag");
                break;
        }

        m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, m_pSettings->m_fmt.nChannels - 1, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK);

        if(m_pSettings->m_fmt.nChannels > 1)
        {
            m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_OK, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO);
        }
        else
        {
            m_dwPrdControl = MCPX_SET_REG_VALUE(m_dwPrdControl, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_NOTOK, NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SubmitPacket
 *
 *  Description:
 *      Submits a packet to the stream.
 *
 *  Arguments:
 *      REFXMEDIAPACKET [in]: packet description.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::SubmitPacket"

HRESULT
CMcpxStream::SubmitPacket
(
    REFXMEDIAPACKET         Packet
)
{

#ifdef DEBUG

    DWORD                   dwContiguousLength;

#endif // DEBUG

    PMCPX_PACKET_CONTEXT    pPacketContext;
    PLIST_ENTRY             pleEntry;
    CIrql                   irql;
    HRESULT                 hr;

    DPF_ENTER();

    //
    // Set the packet's initial state
    //
    
    XMOAcceptPacket(&Packet);

    //
    // If a discontinuity was signaled, but another packet was attached
    // before resources were freed, undo the discontinuity
    //

    and(&m_dwStatus, ~MCPX_VOICESTATUS_DISCONTINUITY);

    RemoveDeferredCommand(MCPX_DEFERREDCMD_STREAM_FLUSH);
    RemoveDeferredCommand(MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES);

    //
    // Make sure resources are allocated
    //
    
    hr = AllocateStreamResources();

    //
    // Get a packet context to track this stream packet
    //

    if(SUCCEEDED(hr))
    {
        irql.Raise();
        
        ASSERT(!IsListEmpty(&m_lstAvailable));
        
        pleEntry = RemoveHeadList(&m_lstAvailable);
        pPacketContext = CONTAINING_RECORD(pleEntry, MCPX_PACKET_CONTEXT, leListEntry);

        irql.Lower();

        pPacketContext->xmpPacket = Packet;
        pPacketContext->dwCompletedSize = 0;
        pPacketContext->dwStatus = XMEDIAPACKET_STATUS_PENDING;
        
        DPF_BLAB("Received packet %x, buffer %x, size %lu", pPacketContext, pPacketContext->xmpPacket.pvBuffer, pPacketContext->xmpPacket.dwMaxSize);
    }

    //
    // Lock the packet memory
    //

    if(SUCCEEDED(hr))
    {
        MmLockUnlockBufferPages(pPacketContext->xmpPacket.pvBuffer, pPacketContext->xmpPacket.dwMaxSize, FALSE);
    }

#ifdef DEBUG

    //
    // ADPCM streams require phsycally contiguous memory
    //

    if(SUCCEEDED(hr) && (WAVE_FORMAT_XBOX_ADPCM == m_pSettings->m_fmt.wFormatTag))
    {
        m_pMcpxApu->GetPhysicalMemoryProperties(pPacketContext->xmpPacket.pvBuffer, pPacketContext->xmpPacket.dwMaxSize, NULL, &dwContiguousLength);

        if(dwContiguousLength != pPacketContext->xmpPacket.dwMaxSize)
        {
            ASSERT(dwContiguousLength < pPacketContext->xmpPacket.dwMaxSize);
            DPF_ERROR("ADPCM stream packet data must be allocated using XPhysicalAlloc.  Using any other allocator will cause unexpected results.");
        }
    }

#endif // DEBUG

    //
    // Add the packet to the pending queue
    //

    if(SUCCEEDED(hr))
    {
        irql.Raise();

        InsertTailList(&m_lstPending, &pPacketContext->leListEntry);

        irql.Lower();
    }

    //
    // Queue up as many packets as we can
    //

    if(SUCCEEDED(hr))
    {
        Process();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Discontinuity
 *
 *  Description:
 *      Signals that no more audio data is available.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Discontinuity"

HRESULT 
CMcpxStream::Discontinuity
(
    void
)
{
    DPF_ENTER();

    //
    // The caller has no more data to supply
    //

    if(HasPendingData())
    {
        or(&m_dwStatus, MCPX_VOICESTATUS_DISCONTINUITY);
    }
    else
    {
        Flush();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Pause
 *
 *  Description:
 *      Pauses or resumes a stream.
 *
 *  Arguments:
 *      DWORD [in]: pause state.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Pause"

HRESULT
CMcpxStream::Pause
(
    DWORD                   dwPause
)
{
    DPF_ENTER();
    AutoIrql();

    if(DSSTREAMPAUSE_PAUSE == dwPause)
    {
        PauseVoice(m_dwStatus | MCPX_VOICESTATUS_PAUSED);
    }
    else if(DSSTREAMPAUSE_RESUME == dwPause)
    {
        PauseVoice(m_dwStatus & ~MCPX_VOICESTATUS_PAUSED);
    }
    else
    {
        ASSERTMSG("Invalid pause state");
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Pause
 *
 *  Description:
 *      Pauses or resumes a stream.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: timestamp.
 *      DWORD [in]: pause state.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Pause"

HRESULT
CMcpxStream::Pause
(
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwPause
)
{
    BOOL                    fDeferred   = FALSE;
    HRESULT                 hr          = DS_OK;

    DPF_ENTER();

    if(rtTimeStamp)
    {
        fDeferred = ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_PAUSE, rtTimeStamp, dwPause);
    }

    if(!fDeferred)
    {
        hr = Pause(dwPause);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Stop
 *
 *  Description:
 *      Pauses a playing buffer.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: time to stop.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Stop"

HRESULT
CMcpxStream::Stop
(
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags
)
{
    BOOL                    fDeferred   = FALSE;
    HRESULT                 hr          = DS_OK;

    DPF_ENTER();

    if(rtTimeStamp)
    {
        fDeferred = ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_STOP, rtTimeStamp, dwFlags);
    }

    if(!fDeferred)
    {
        hr = Stop(dwFlags);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Stop
 *
 *  Description:
 *      Stops the stream and readies it to be flushed.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Stop"

HRESULT 
CMcpxStream::Stop
(
    DWORD                   dwFlags
)
{
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(dwFlags & DSSTREAMFLUSHEX_ASYNC);

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        //
        // Cancel all outstanding deferred commands
        //

        for(i = 0; i < MCPX_DEFERREDCMD_STREAM_COUNT; i++)
        {
            RemoveDeferredCommand(i);
        }

        //
        // Deactivate or release the voice
        //

        if(dwFlags & DSSTREAMFLUSHEX_ENVELOPE)
        {
            ReleaseVoice();
        }
        else
        {
            DeactivateVoice();
        }

        //
        // Schedule a deferred flush for when the voice is actually stopped
        //

        ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_FLUSH, 0, 0);
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Revokes all submitted packets.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Flush"

HRESULT 
CMcpxStream::Flush
(
    void
)
{
    const KIRQL             CurrentIrql = KeGetCurrentIrql();
    CIrql                   irql;
    DWORD                   i;
    
    DPF_ENTER();

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        //
        // Raise IRQL for synchronization with the voice interrupt handler
        //
    
        irql.Raise();
    
        //
        // Cancel any outstanding deferred commands
        //

        for(i = 0; i < MCPX_DEFERREDCMD_STREAM_COUNT; i++)
        {
            RemoveDeferredCommand(i);
        }

        //
        // Deactivate the voice
        //

        DeactivateVoice();

        //
        // Lower IRQL to allow the interrupt handler to run
        //

        irql.Lower();

        //
        // Busy-wait until the voice is really idle
        //

        WaitForVoiceOff();

        //
        // Clear the SSLs
        //

        for(i = 0; i < NUMELMS(m_aSslDesc); i++)
        {
            CompleteSsl(i, XMEDIAPACKET_STATUS_FLUSHED);
        }

        ASSERT(!m_dwMappedSslCount);
        m_dwFirstMappedSslIndex = 0;

        //
        // Complete any pending packets
        //

        CompletePackets(&m_lstPending, XMEDIAPACKET_STATUS_FLUSHED);

        //
        // If we're LOCDEFER, free stream resources
        //

        if(m_pSettings->m_dwFlags & DSSTREAMCAPS_LOCDEFER)
        {
            if(PASSIVE_LEVEL == CurrentIrql)
            {
                ReleaseStreamResources();
            }
            else
            {
                ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES, 0, 0);
            }
        }

        //
        // Clear the deferred completion list
        //
        
        if(PASSIVE_LEVEL == CurrentIrql)
        {
            CompleteDeferredPackets();
        }
        else
        {
            ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS, 0, 0);
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets the status of the stream.
 *
 *  Arguments:
 *      LPDWORD [out]: stream status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::GetStatus"

HRESULT
CMcpxStream::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    DPF_ENTER();
    AutoIrql();

    *pdwStatus = 0;

    if(!IsListEmpty(&m_lstAvailable))
    {
        *pdwStatus |= DSSTREAMSTATUS_READY;
    }

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        if(m_dwStatus & MCPX_VOICESTATUS_ALLPAUSEDMASK)
        {
            *pdwStatus |= DSSTREAMSTATUS_PAUSED;

            if(m_dwStatus & MCPX_VOICESTATUS_STARVED)
            {
                *pdwStatus |= DSSTREAMSTATUS_STARVED;
            }
        }
        else
        {
            *pdwStatus |= DSSTREAMSTATUS_PLAYING;
        }
    }
    else if((m_dwStatus & MCPX_VOICESTATUS_VOICEOFFMASK) == MCPX_VOICESTATUS_VOICEOFFMASK)
    {
        *pdwStatus |= DSSTREAMSTATUS_PLAYING;
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  OnEndOfStream
 *
 *  Description:
 *      Handles the voice deactivation notifier.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::OnEndOfStream"

void
CMcpxStream::OnEndOfStream
(
    void
)
{

#ifdef DEBUG

    DWORD                   i;

#endif // DEBUG

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK);

    //
    // Stop the voice
    //

    DeactivateVoice();

#ifdef DEBUG

    //
    // Assert that all packets have been completed
    //

    for(i = 0; i < NUMELMS(m_aSslDesc); i++)
    {
        ASSERT(IsListEmpty(&m_aSslDesc[i].lstPackets));
    }

    ASSERT(IsListEmpty(&m_lstPending));

#endif // DEBUG

    //
    // Reset the SSL indeces
    //

    ASSERT(!m_dwMappedSslCount);
    m_dwFirstMappedSslIndex = 0;

    //
    // If we're LOCDEFER, schedule a deferred command to release voice
    // resources
    //

    if(m_pSettings->m_dwFlags & DSSTREAMCAPS_LOCDEFER)
    {
        ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES, 0, 0);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  OnDeferredFlush
 *
 *  Description:
 *      Handles the deferred flush command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::OnDeferredFlush"

void
CMcpxStream::OnDeferredFlush
(
    void
)
{
    DPF_ENTER();

    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);

    //
    // Check the voice status.  If we're still waiting for the VoiceOff, don't
    // do the flush yet.
    //

    if(m_dwStatus & MCPX_VOICESTATUS_ACTIVEORVOICEOFF)
    {
        DPF_BLAB("Voice %x (%x) not stopped yet", this, m_ahVoices[0]);
    }
    else
    {
        Flush();
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Updates the stream by completing and remapping SSLs.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: MCPX_STREAM_PROCESS code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::Process"

DWORD
CMcpxStream::Process
(
    void
)
{
    DWORD                   dwStatus        = 0;
    BOOL                    fContinue       = TRUE;
    DWORD                   dwSslIndex;
    DWORD                   dwSslCount;

    DPF_ENTER();
    AutoIrql();

    while(fContinue)
    {
        //
        // Check for completed SSLs, starting with the oldest one first
        //

        dwSslIndex = m_dwFirstMappedSslIndex;
    
        for(dwSslCount = 0; dwSslCount < NUMELMS(m_aSslDesc); dwSslCount++)
        {
            if(m_Notifier.GetStatus(dwSslIndex))
            {
                CompleteSsl(dwSslIndex, XMEDIAPACKET_STATUS_SUCCESS);

                dwStatus |= MCPX_STREAM_PROCESS_COMPLETEDSSL;
            }
            else
            {
                break;
            }

            dwSslIndex = (dwSslIndex + 1) % NUMELMS(m_aSslDesc);
        }

        //
        // Fill SSLs, starting with the first empty one
        //

        dwSslIndex = (m_dwFirstMappedSslIndex + m_dwMappedSslCount) % NUMELMS(m_aSslDesc);
    
        for(dwSslCount = 0; dwSslCount < NUMELMS(m_aSslDesc); dwSslCount++)
        {
            if(MapPackets(dwSslIndex))
            {
                dwStatus |= MCPX_STREAM_PROCESS_MAPPEDDATA;
            }
            else
            {
                fContinue = FALSE;
                break;
            }

            dwSslIndex = (dwSslIndex + 1) % NUMELMS(m_aSslDesc);
        }
    }

    //
    // If we're providing accurate callbacks, flush the deferred completion 
    // list
    //

    if(m_pSettings->m_dwFlags & DSSTREAMCAPS_ACCURATENOTIFY)
    {
        CompleteDeferredPackets();
    }

    DPF_LEAVE(dwStatus);

    return dwStatus;
}


/****************************************************************************
 *
 *  MapPackets
 *
 *  Description:
 *      Maps packets into the given SSL.
 *
 *  Arguments:
 *      DWORD [in]: SSL index.
 *
 *  Returns:  
 *      BOOL: TRUE if any packets were mapped.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::MapPackets"

BOOL
CMcpxStream::MapPackets
(
    DWORD                   dwSslIndex
)
{
    BOOL                    fMapped         = FALSE;
    PMCPX_PACKET_CONTEXT    pPacketContext;
    DWORD                   dwBytesMapped;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Map as many packets as we can into this SSL.  If the user asked
    // for accurate packet completions, we'll only map one packet per
    // SSL.  Mapping any more would cause multiple packets to complete
    // at the same time.
    //

    if(!m_aSslDesc[dwSslIndex].dwPrdCount)
    {
        while(m_aSslDesc[dwSslIndex].dwPrdCount < MCPX_HW_MAX_PRD_ENTRIES_PER_SSL)
        {
            if(IsListEmpty(&m_lstPending))
            {
                break;
            }

            //
            // Get the next packet from the list
            //

            pPacketContext = CONTAINING_RECORD(m_lstPending.Flink, MCPX_PACKET_CONTEXT, leListEntry);

            //
            // Map the packet into the SSL
            //

            if(dwBytesMapped = MapPacket(dwSslIndex, pPacketContext))
            {
                DPF_BLAB("Mapped %lu bytes into SSL%c (%lu total)", dwBytesMapped, 'A' + dwSslIndex, m_aSslDesc[dwSslIndex].dwBytesMapped);
                fMapped = TRUE;
            }
            else
            {
                break;
            }

            //
            // Are we allowed to map more packets?
            //

            if(m_pSettings->m_dwFlags & DSSTREAMCAPS_ACCURATENOTIFY)
            {
                break;
            }
        }

        //
        // Commit the SSL to the hardware.  If we didn't map any packets into 
        // this SSL, we can stop processing.
        //

        if(fMapped)
        {
            CommitSsl(dwSslIndex);
        }
    }

    DPF_LEAVE(fMapped);

    return fMapped;
}


/****************************************************************************
 *
 *  MapPacket
 *
 *  Description:
 *      Maps a packet into an SSL.
 *
 *  Arguments:
 *      DWORD [in]: SSL index.
 *      PMCPX_PACKET_CONTEXT [in]: packet data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::MapPacket"

DWORD
CMcpxStream::MapPacket
(
    DWORD                   dwSslIndex,
    PMCPX_PACKET_CONTEXT    pPacketContext
)
{
    DWORD                   dwBytesMapped       = 0;
    LPBYTE                  pbPacketData;
    DWORD                   dwPacketSize;
    PMCPX_SSL_DESC          pSsl;
    DWORD                   dwMaxPrdSize;
    DWORD                   dwContiguousLength;
    DWORD                   dwSampleCount;
    DWORD                   dwAddress;
    DWORD                   dwBase;
    DWORD                   dwBasePage;
    DWORD                   dwIndex;
    DWORD                   dwLengthAndControl;
    DWORD                   dwIncrement;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(dwSslIndex < NUMELMS(m_aSslDesc));

    pSsl = &m_aSslDesc[dwSslIndex];

    //
    // Save a local pointer to the packet data.  If we've already mapped part
    // of this packet into another SSL, the dwCompletedSize member will be 
    // non-zero.
    //
    
    pbPacketData = (LPBYTE)pPacketContext->xmpPacket.pvBuffer;
    dwPacketSize = pPacketContext->xmpPacket.dwMaxSize;

    if(pPacketContext->dwCompletedSize)
    {
        ASSERT(pPacketContext->dwCompletedSize < dwPacketSize);

        pbPacketData += pPacketContext->dwCompletedSize;
        dwPacketSize -= pPacketContext->dwCompletedSize;

        DPF_BLAB("Working packet offset %lu", pPacketContext->dwCompletedSize);
    }
    else
    {
        DPF_BLAB("Mapping new packet");
    }

    //
    // PRDs can only contain 16-bit addresses
    //

    dwMaxPrdSize = SamplesToBytes(0xFFFF);

    //
    // Map as much of the packet as we can into the SSL
    //

    while((dwBytesMapped < dwPacketSize) && (pSsl->dwPrdCount < MCPX_HW_MAX_PRD_ENTRIES_PER_SSL))
    {
        //
        // Get physical memory properties of the packet data
        //
        
        dwAddress = m_pMcpxApu->GetPhysicalMemoryProperties(pbPacketData + dwBytesMapped, min(dwPacketSize - dwBytesMapped, dwMaxPrdSize), NULL, &dwContiguousLength);

        //
        // Convert packet length to samples
        //

        dwSampleCount = BytesToSamples(dwContiguousLength);
        ASSERT(!(dwSampleCount & ~0xFFFF));

        //
        // ... and back to bytes
        //

        dwContiguousLength = SamplesToBytes(dwSampleCount);

        //
        // If there's not at least one full block to write, we can't do
        // any more.
        //

        if(!dwSampleCount)
        {
            DPF_ERROR("Unable to map the next packet because it doesn't contain a full block of contiguous data");
            break;
        }

        //
        // If we ever support multiple mono voices (i.e. an n-channel voice uses
        // n mono hardware voices instead of n/2 stereo), the increment value 
        // should be halved.
        //

        dwIncrement = m_pSettings->m_fmt.wBitsPerSample * 2 / 8;

        //
        // Fill in PRD values
        //

        MCPX_CHECK_VOICE_FIFO(3 * m_bVoiceCount);
        
        for(i = 0; i < m_bVoiceCount; i++)
        {
            dwBase = GetSslBase(i, dwSslIndex) + pSsl->dwPrdCount;
            dwBasePage = dwBase / NUMELMS(((LPMCPXVPREGSET)NULL)->SetSSLSegment);
            dwIndex = dwBase % NUMELMS(((LPMCPXVPREGSET)NULL)->SetSSLSegment);
            dwLengthAndControl = (m_dwPrdControl & 0xFFFF0000) | dwSampleCount;

            MCPX_VOICE_WRITE(SetCurrentSSL, MCPX_MAKE_REG_VALUE(dwBasePage, NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE));
            MCPX_VOICE_WRITE(SetSSLSegment[dwIndex].Offset, dwAddress);
            MCPX_VOICE_WRITE(SetSSLSegment[dwIndex].Length, dwLengthAndControl);

            dwAddress += dwIncrement;
        }

        //
        // Increment the count of bytes we've mapped
        //
        
        dwBytesMapped += dwContiguousLength;

        pSsl->dwBytesMapped += dwContiguousLength;

        //
        // Increment the count of PRDs used in this SSL
        //
        
        pSsl->dwPrdCount++;
    }

    //
    // If we managed to map the whole packet into the SSL, remove it from
    // the pending list and add it to the SSL's list.  If not, leave it
    // in the pending list, but set the completed size so we'll know where
    // to start working next time.
    //

    if(dwBytesMapped >= dwPacketSize)
    {
        MoveEntryTailList(&pSsl->lstPackets, &pPacketContext->leListEntry);
    }
    else
    {
        pPacketContext->dwCompletedSize += dwBytesMapped;
    }

    DPF_LEAVE(dwBytesMapped);

    return dwBytesMapped;
}


/****************************************************************************
 *
 *  CommitSsl
 *
 *  Description:
 *      Commits an SSL to the hardware.
 *
 *  Arguments:
 *      DWORD [in]: SSL index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CommitSsl"

void
CMcpxStream::CommitSsl
(
    DWORD                   dwSslIndex
)
{
    DWORD                   dwSslBase;
    DWORD                   dwSslBaseAndCount;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(dwSslIndex < NUMELMS(m_aSslDesc));

    DPF_BLAB("Commiting SSL%c", 'A' + dwSslIndex);

    //
    // Assign the SSL to the voice
    //

    MCPX_CHECK_VOICE_FIFO(2 * m_bVoiceCount);
    
    for(i = 0; i < m_bVoiceCount; i++)
    {
        dwSslBase = GetSslBase(i, dwSslIndex);
        
        dwSslBaseAndCount = MCPX_MAKE_REG_VALUE(dwSslBase, NV1BA0_PIO_SET_VOICE_SSL_A_BASE);
        dwSslBaseAndCount |= MCPX_MAKE_REG_VALUE(m_aSslDesc[dwSslIndex].dwPrdCount, NV1BA0_PIO_SET_VOICE_SSL_A_COUNT);
        
        MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
        
        if(!dwSslIndex)
        {
            MCPX_VOICE_WRITE(SetVoiceSSLA, dwSslBaseAndCount);
        }
        else
        {
            MCPX_VOICE_WRITE(SetVoiceSSLB, dwSslBaseAndCount);
        }
    }

    //
    // Reset the notifier
    //

    m_Notifier.SetStatus(dwSslIndex, FALSE);

    //
    // Increment the count of mapped SSLs
    //

    ASSERT(m_dwMappedSslCount < NUMELMS(m_aSslDesc));

    if(!m_dwMappedSslCount++)
    {
        m_dwFirstMappedSslIndex = dwSslIndex;
    }

    //
    // If we're not providing high-accuracy packet completions, register for
    // deferred completion callbacks.
    //

    if(!(m_pSettings->m_dwFlags & DSSTREAMCAPS_ACCURATENOTIFY))
    {
        ScheduleDeferredCommand(MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS, 0, 0);
    }

    //
    // Activate the voice
    //

    ActivateVoice();

    //
    // If the voice is starved, unstarve it
    //

    PauseVoice(m_dwStatus & ~MCPX_VOICESTATUS_STARVED);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceVoiceInterrupt
 *
 *  Description:
 *      DPC callback function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::ServiceVoiceInterrupt"

BOOL
CMcpxStream::ServiceVoiceInterrupt
(
    void
)
{
    DWORD                   dwStatus;
    DWORD                   dwSslIndex;
    DWORD                   dwSslCount;
    DWORD                   i;

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);
    
    //
    // The first thing to check is whether the voice is still active or 
    // not.  If we reached the end of the release envelope, the voice will
    // automatically be removed from the voice list.  This should only
    // happen when NoteOff has been called, but may occur either because
    // the envelope ended or because the stream was starved after the 
    // NoteOff was received.  Because we remove the PERSIST bit from the
    // voice format when NoteOff is called, the hardware is free to idle
    // the voice whenever it wants to.  We have no way of knowing why the
    // voice idled, so we just don't worry about it.
    //

    if(m_Notifier.GetStatus(MCPX_NOTIFIER_VOICE_OFF))
    {
        ASSERT(MCPX_VOICELIST_INVALID == m_bVoiceList);
        
        DPF_INFO("Stream completed naturally");

        //
        // Reset the notifier
        //

        m_Notifier.SetStatus(MCPX_NOTIFIER_VOICE_OFF, FALSE);

        //
        // Flush the stream
        //

        Flush();
    }
    else if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        ASSERT(m_bVoiceList < MCPX_VOICELIST_COUNT);

        //
        // An interrupt triggered for voice end or notifier written.  If either
        // of our SSL completion notifiers are set, map some more packets into
        // them.  Note that we're not resetting the notifier status in this
        // function.  We're doing it in Process instead.
        //

        dwStatus = Process();

        //
        // If we handled the interrupt, but no more packets were available
        // to be mapped into an SSL, the stream has been starved by the 
        // user.  If DISCONTINUITY is set in the status, we can flush the
        // stream now.  If not, the voice should be stopped until more 
        // packets arrive.  If we allow the voice to remain active, the 
        // hardware generates an SSL completion interrupt every frame.
        //

        if(((dwStatus & MCPX_STREAM_PROCESS_MASK) == MCPX_STREAM_PROCESS_COMPLETEDSSL) && !m_dwMappedSslCount)
        {
            if(m_dwStatus & MCPX_VOICESTATUS_DISCONTINUITY)
            {
                DPF_INFO("End-of-stream reached");
                OnEndOfStream();
            }
            else if(!(m_dwStatus & MCPX_VOICESTATUS_STARVED))
            {
                DPF_WARNING("The stream is starving");
                PauseVoice(MCPX_VOICESTATUS_STARVED);
            }
        }
    }

    DPF_LEAVE(TRUE);

    return TRUE;
}


/****************************************************************************
 *
 *  CompleteSsl
 *
 *  Description:
 *      Called to handle an SSL completion.
 *
 *  Arguments:
 *      DWORD [in]: SSL index.
 *      DWORD [in]: packet status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CompleteSsl"

void
CMcpxStream::CompleteSsl
(
    DWORD                   dwSslIndex,
    DWORD                   dwStatus
)
{
    DWORD                   dwSslBase;
    DWORD                   dwSslBaseAndCount;
    DWORD                   i;

    DPF_ENTER();
    AutoIrql();

    ASSERT(dwSslIndex < NUMELMS(m_aSslDesc));

    if(m_aSslDesc[dwSslIndex].dwPrdCount)
    {
        DPF_BLAB("Completing SSL%c", 'A' + dwSslIndex);

        //
        // Reset hardware SSL
        //

        MCPX_CHECK_VOICE_FIFO(2 * m_bVoiceCount);
    
        for(i = 0; i < m_bVoiceCount; i++)
        {
            dwSslBase = GetSslBase(i, dwSslIndex);
        
            dwSslBaseAndCount = MCPX_MAKE_REG_VALUE(dwSslBase, NV1BA0_PIO_SET_VOICE_SSL_A_BASE);
        
            MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);

            if(!dwSslIndex)
            {
                MCPX_VOICE_WRITE(SetVoiceSSLA, dwSslBaseAndCount);
            }
            else
            {
                MCPX_VOICE_WRITE(SetVoiceSSLB, dwSslBaseAndCount);
            }
        }

        //
        // Complete all the packets mapped into the SSL
        //

        CompletePackets(&m_aSslDesc[dwSslIndex].lstPackets, dwStatus);

        //
        // Reset the SSL data
        //

        m_aSslDesc[dwSslIndex].dwPrdCount = 0;
        m_aSslDesc[dwSslIndex].dwBytesMapped = 0;

        //
        // Decrement the count of mapped SSLs
        //

        ASSERT(m_dwMappedSslCount);
        m_dwMappedSslCount--;

        m_dwFirstMappedSslIndex = (m_dwFirstMappedSslIndex + 1) % NUMELMS(m_aSslDesc);
    }
    else
    {
        ASSERT(IsListEmpty(&m_aSslDesc[dwSslIndex].lstPackets));
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CompletePackets
 *
 *  Description:
 *      Releases packets.
 *
 *  Arguments:
 *      PLIST_ENTRY [in]: packet list entry.
 *      DWORD [in]: packet completion status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CompletePackets"

void
CMcpxStream::CompletePackets
(
    PLIST_ENTRY             pListHead,
    DWORD                   dwStatus
)
{
    PLIST_ENTRY             pleEntry;
    CIrql                   irql;

    DPF_ENTER();

    //
    // Only complete the packets if it's the deferred list.  For everything
    // else, add the packets to the deferred completion list.
    //

    if(&m_lstCompleted == pListHead)
    {
        irql.Raise();
        
        while((pleEntry = MoveHeadTailList(&m_lstAvailable, pListHead)) != pListHead)
        {
            irql.Lower();

            CompletePacket(CONTAINING_RECORD(pleEntry, MCPX_PACKET_CONTEXT, leListEntry), dwStatus);

            irql.Raise();
        }

        irql.Lower();
    }
    else
    {
        irql.Raise();
        
        while((pleEntry = MoveHeadTailList(&m_lstCompleted, pListHead)) != pListHead)
        {
            CONTAINING_RECORD(pleEntry, MCPX_PACKET_CONTEXT, leListEntry)->dwStatus = dwStatus;

            DPF_BLAB("Packet %x scheduled for deferred completion with status %lu", pleEntry, dwStatus);
        }

        irql.Lower();
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CompletePacket
 *
 *  Description:
 *      Releases a packet.
 *
 *  Arguments:
 *      PMCPX_PACKET_CONTEXT [in]: packet context.
 *      DWORD [in]: packet completion status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CompletePacket"

void
CMcpxStream::CompletePacket
(
    PMCPX_PACKET_CONTEXT    pPacketContext,
    DWORD                   dwStatus
)
{
    DPF_ENTER();

    //
    // If the packet has a status value other than PENDING stored in it's
    // context, we'll use that instead of the status value passed in.
    //

    if(XMEDIAPACKET_STATUS_PENDING != pPacketContext->dwStatus)
    {
        dwStatus = pPacketContext->dwStatus;
    }

    ASSERT(XMEDIAPACKET_STATUS_PENDING != dwStatus);

    DPF_BLAB("Completing packet %x with status %lu", pPacketContext, dwStatus);

    //
    // Unlock buffer pages
    //

    MmLockUnlockBufferPages(pPacketContext->xmpPacket.pvBuffer, pPacketContext->xmpPacket.dwMaxSize, TRUE);

    //
    // Now call them back so there is at least one available context
    // if they turn around and call process in their callback
    //

    XMOCompletePacket(&pPacketContext->xmpPacket, pPacketContext->xmpPacket.dwMaxSize, m_pSettings->m_pfnCallback, m_pSettings->m_pvContext, dwStatus);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CompleteDeferredPackets
 *
 *  Description:
 *      Releases packets in the deferred completion list.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::CompleteDeferredPackets"

void
CMcpxStream::CompleteDeferredPackets
(
    void
)
{
    DPF_ENTER();

    //
    // If we're not running any more, we can unregister for completion 
    // callbacks
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) != MCPX_VOICESTATUS_ACTIVEMASK)
    {
        RemoveDeferredCommand(MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS);
    }        

    //
    // Complete all the packets in the deferred completion list
    //
    
    CompletePackets(&m_lstCompleted, XMEDIAPACKET_STATUS_PENDING);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ReleaseStreamResources
 *
 *  Description:
 *      Releases hardware resources for the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::ReleaseStreamResources"

void
CMcpxStream::ReleaseStreamResources
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Make sure the voice is stopped
    //

    DeactivateVoice(TRUE);

    //
    // Unschedule any pending low-priority tasks (including RELEASERESOURCES)
    //

    for(i = 0; i < MCPX_DEFERREDCMD_STREAM_COUNT; i++)
    {
        if(m_aDeferredCommands[i].dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY)
        {
            RemoveDeferredCommand(i);
        }
    }

    //
    // Clear any left-over status bits
    //

    and(&m_dwStatus, MCPX_VOICESTATUS_DEACTIVATEMASK);

    //
    // Release voice resources
    //

    ReleaseVoiceResources();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ScheduleDeferredCommand
 *
 *  Description:
 *      Adds a deferred voice command to the queue.
 *
 *  Arguments:
 *      DWORD [in]: command index.
 *      REFERENCE_TIME [in]: timestamp.
 *      DWORD [in]: context.
 *
 *  Returns:  
 *      BOOL: TRUE if the command was scheduled.  If the timestamp for the
 *            command has already elapsed, it won't be scheduled and should
 *            be processed immediately.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::ScheduleDeferredCommand"

BOOL 
CMcpxStream::ScheduleDeferredCommand
(
    DWORD                   dwCommand, 
    REFERENCE_TIME          rtTimeStamp, 
    DWORD                   dwContext
)
{
    LPMCPX_DEFERRED_COMMAND pCmd        = &m_aDeferredCommands[dwCommand];
    BOOL                    fScheduled;

    DPF_ENTER();
    
    ASSERT(dwCommand < NUMELMS(m_aDeferredCommands));

    pCmd->rtTimestamp = rtTimeStamp;
    pCmd->dwContext = dwContext;

    if(fScheduled = m_pMcpxApu->ScheduleDeferredCommand(pCmd))
    {
        DPF_BLAB("Voice %x (%x) scheduled command %lu", this, m_ahVoices[0], dwCommand);
    }

    DPF_LEAVE(fScheduled);

    return fScheduled;
}


/****************************************************************************
 *
 *  RemoveDeferredCommand
 *
 *  Description:
 *      Removes a deferred voice command from the queue.
 *
 *  Arguments:
 *      DWORD [in]: command index.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::RemoveDeferredCommand"

void 
CMcpxStream::RemoveDeferredCommand
(
    DWORD                   dwCommand
)
{
    DPF_ENTER();

    ASSERT(dwCommand < NUMELMS(m_aDeferredCommands));

    if(m_aDeferredCommands[dwCommand].dwFlags & MCPX_DEFERREDCMDF_SCHEDULED)
    {
        DPF_BLAB("Voice %x (%x) unscheduling command %lu", this, m_ahVoices[0], dwCommand);
    }
    
    m_pMcpxApu->RemoveDeferredCommand(&m_aDeferredCommands[dwCommand]);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceDeferredCommand
 *
 *  Description:
 *      Deferred command dispatcher.
 *
 *  Arguments:
 *      DWORD [in]: command identifier.
 *      DWORD [in]: command context.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxStream::ServiceDeferredCommand"

void 
CMcpxStream::ServiceDeferredCommand
(
    DWORD                   dwCommand, 
    DWORD                   dwContext
)
{
    DPF_ENTER();
    
    switch(dwCommand)
    {
        case MCPX_DEFERREDCMD_STREAM_COMPLETEPACKETS:
            CompleteDeferredPackets();
            break;

        case MCPX_DEFERREDCMD_STREAM_RELEASERESOURCES:
            ReleaseStreamResources();
            break;

        case MCPX_DEFERREDCMD_STREAM_FLUSH:
            OnDeferredFlush();
            break;

        case MCPX_DEFERREDCMD_STREAM_STOP:
            Stop(dwContext);
            break;

        case MCPX_DEFERREDCMD_STREAM_PAUSE:
            Pause(dwContext);
            break;

        case MCPX_DEFERREDCMD_STREAM_CHECKSTUCK:
            CheckStuckVoice();
            break;

        default:
            ASSERTMSG("Unexpected deferred command identifier");
            break;
    }

    DPF_LEAVE_VOID();
}


