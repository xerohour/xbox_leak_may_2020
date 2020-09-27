/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpbuf.cpp
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
 *  CMcpxBuffer
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CMcpxAPU * [in]: MCP-X APU object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::CMcpxBuffer"

CMcpxBuffer::CMcpxBuffer
(
    CMcpxAPU *                      pMcpxApu,
    CDirectSoundBufferSettings *    pSettings
)
:   CMcpxVoiceClient(pMcpxApu, pSettings)
{
    DWORD                   i;
    
    DPF_ENTER();

    m_pSettings = ADDREF(pSettings);

    //
    // Initialize deferred command structures
    //

    for(i = 0; i < NUMELMS(m_aDeferredCommands); i++)
    {
        InitializeListHead(&m_aDeferredCommands[i].leListEntry);
        
        m_aDeferredCommands[i].pVoice = this;
        m_aDeferredCommands[i].dwCommand = i;
    }

    m_aDeferredCommands[MCPX_DEFERREDCMD_BUFFER_CHECKSTUCK].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;
    m_aDeferredCommands[MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;
    m_aDeferredCommands[MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA].dwFlags |= MCPX_DEFERREDCMDF_LOWPRIORITY | MCPX_DEFERREDCMDF_PERSIST;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CMcpxBuffer
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
#define DPF_FNAME "CMcpxBuffer::~CMcpxBuffer"

CMcpxBuffer::~CMcpxBuffer
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Free buffer resources
    //

    ReleaseBufferResources();

    //
    // Make sure all deferred commands are dequeued
    //

    for(i = 0; i < NUMELMS(m_aDeferredCommands); i++)
    {
        RemoveDeferredCommand(i);
    }

    //
    // Release the settings object
    //
    
    RELEASE(m_pSettings);

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
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::Initialize"

HRESULT
CMcpxBuffer::Initialize
(
    void
)
{
    HRESULT                         hr;

    DPF_ENTER();

    //
    // Initialize the base class
    //

    hr = CMcpxVoiceClient::Initialize(FALSE);

    //
    // If the buffer was created without LOCDEFER and we have valid buffer 
    // data, allocate resources now.
    //

    if(SUCCEEDED(hr) && !(m_pSettings->m_dwFlags & DSBCAPS_LOCDEFER))
    {
        hr = AllocateBufferResources();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetBufferData
 *
 *  Description:
 *      Sets the data buffer address and size.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::SetBufferData"

HRESULT 
CMcpxBuffer::SetBufferData
(
    void
)
{
    HRESULT                 hr  = DS_OK;

    DPF_ENTER();

    ASSERT(!(m_dwStatus & ~MCPX_VOICESTATUS_ALLOCATED));
    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));
    ASSERT(m_pSettings->m_pvBufferData);
    ASSERT(m_pSettings->m_dwBufferSize);

    //
    // Reset the cached play cursor position
    //

    m_dwCachedPlayCursor = 0;

    //
    // If we're LOCDEFER, don't do anything.  If we're not, make sure
    // resources are allocated.
    //

    if(!(m_pSettings->m_dwFlags & DSBCAPS_LOCDEFER))
    {
        hr = AllocateBufferResources();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReleaseBufferData
 *
 *  Description:
 *      Called before SetBufferData, giving the object a chance to clean
 *      up before the data pointer/size pair changes.
 *
 *  Arguments:
 *      BOOL [in]: TRUE if more data is on the way; FALSE if the data is
 *                 being released.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::ReleaseBufferData"

HRESULT 
CMcpxBuffer::ReleaseBufferData
(
    BOOL                    fChanging
)
{
    DPF_ENTER();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));

    //
    // If we're LOCDEFER, go ahead and free voice resources.  If not, just
    // deactivate the voice and unmap the data buffer.
    //

    if(!fChanging && (m_pSettings->m_dwFlags & DSBCAPS_LOCDEFER))
    {
        ReleaseBufferResources();
    }
    else
    {
        UnmapBuffer();
    }

    //
    // We're not allowing the current position to persist after changing
    // the buffer data.
    //

    m_dwCachedPlayCursor = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  AllocateBufferResources
 *
 *  Description:
 *      Allocates hardware resources for the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::AllocateBufferResources"

HRESULT 
CMcpxBuffer::AllocateBufferResources
(
    void
)
{
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Allocate voice resources
    //
    
    hr = AllocateVoiceResources();

    //
    // Map the data buffer
    //

    if(SUCCEEDED(hr) && !(m_pSettings->m_dwFlags & DSBCAPS_MIXIN))
    {
        if(m_pSettings->m_dwFlags & DSBCAPS_FXIN)
        {
            MapEffectsBuffer();
        }
        else if(m_pSettings->m_pvBufferData && m_pSettings->m_dwBufferSize)
        {
            hr = MapInputBuffer();
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReleaseBufferResources
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
#define DPF_FNAME "CMcpxBuffer::ReleaseBufferResources"

void
CMcpxBuffer::ReleaseBufferResources
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    //
    // Make sure the voice is stopped
    //

    Stop();
    WaitForVoiceOff();

    //
    // Unschedule any pending low-priority tasks (including RELEASERESOURCES)
    //

    for(i = 0; i < MCPX_DEFERREDCMD_BUFFER_COUNT; i++)
    {
        if(m_aDeferredCommands[i].dwFlags & MCPX_DEFERREDCMDF_LOWPRIORITY)
        {
            RemoveDeferredCommand(i);
        }
    }

    //
    // Unmap the buffer.
    //

    UnmapBuffer();

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
 *  MapInputBuffer
 *
 *  Description:
 *      Maps the data buffer into SGEs used by the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::MapInputBuffer"

HRESULT
CMcpxBuffer::MapInputBuffer
(
    void
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));
    ASSERT(m_pSettings->m_pvBufferData);
    ASSERT(m_pSettings->m_dwBufferSize);

    if(!(m_dwStatus & MCPX_VOICESTATUS_BUFFERMAPPED))
    {
        ASSERT(m_dwStatus == MCPX_VOICESTATUS_ALLOCATED);
        ASSERT(!m_pSgeHeapEntry);

        //
        // Set the status bit that shows that the buffer was mapped
        //

        or(&m_dwStatus, MCPX_VOICESTATUS_BUFFERMAPPED);

        //
        // Allocate a run of SGEs and map the buffer into the global SGE table
        //

        if(!(m_pSgeHeapEntry = m_pMcpxApu->m_SgeHeap.Alloc(m_pSettings->m_pvBufferData, m_pSettings->m_dwBufferSize)))
        {
            DPF_RESOURCE("Out of scatter/gather entries.  Either your buffer is too big, or too many buffers have been mapped at one time.");
            hr = DSERR_OUTOFMEMORY;
        }

        //
        // Complete the mapping operation
        //

        if(SUCCEEDED(hr))
        {
            MapBuffer();
        }

        //
        // If anything went wrong, unmap the buffer
        //

        if(FAILED(hr))
        {
            UnmapBuffer();
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  MapEffectsBuffer
 *
 *  Description:
 *      Maps the data buffer into SGEs used by the voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::MapEffectsBuffer"

void
CMcpxBuffer::MapEffectsBuffer
(
    void
)
{
    DWORD                   dwVoiceIndex;
    LPVOID                  pvBufferData;
    DWORD                   dwBufferSize;
    
    DPF_ENTER();

    ASSERT((m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK) == DSBCAPS_FXIN);
    ASSERT(!m_pSgeHeapEntry);

    if(!(m_dwStatus & MCPX_VOICESTATUS_BUFFERMAPPED))
    {
        ASSERT(m_dwStatus == MCPX_VOICESTATUS_ALLOCATED);
        ASSERT(!m_pSettings->m_pvBufferData);
        ASSERT(!m_pSettings->m_dwBufferSize);

        //
        // Set the status bit that shows that the buffer was mapped
        //

        or(&m_dwStatus, MCPX_VOICESTATUS_BUFFERMAPPED);

        //
        // The 2047th SGE is reserved for global multipass (i.e. FXIN) buffers.
        // It was already set up for us, so all we have to do here is point the
        // voice at it.
        //

        dwVoiceIndex = m_pSettings->m_dwInputMixBin - DSMIXBIN_FXSEND_FIRST;

        dwBufferSize = SamplesToBytes(MCPX_HW_FRAME_SIZE_SAMPLES);
        pvBufferData = (LPBYTE)m_pMcpxApu->m_ctxMemory[MCPX_MEM_GPMULTIPASS].VirtualAddress + (dwVoiceIndex * dwBufferSize);

        m_pSettings->SetBufferData(pvBufferData, dwBufferSize);

        DPF_INFO("Voice %x (%x) using mixbin %lu (address %x) as it's input", this, m_ahVoices[0], m_pSettings->m_dwInputMixBin, pvBufferData);

        //
        // Complete the mapping operation
        //

        MapBuffer();
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  MapBuffer
 *
 *  Description:
 *      Maps the data buffer into SGEs used by the voice.
 *
 *  Arguments:
 *      DWORD [in]: byte offset from the base buffer address to map.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::MapBuffer"

void
CMcpxBuffer::MapBuffer
(
    DWORD                   dwOffset
)
{

#ifdef DEBUG

    DWORD                   dwAlignment;

#endif // DEBUG

    DWORD                   dwSgeIndex;
    DWORD                   i;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_MIXIN));
    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);

#ifdef DEBUG

    //
    // Validate that the buffer base address and offset are properly aligned
    //
    
    if(WAVE_FORMAT_XBOX_ADPCM == m_pSettings->m_fmt.wFormatTag)
    {
        dwAlignment = 4;
    }
    else if(8 == m_pSettings->m_fmt.wBitsPerSample)
    {
        dwAlignment = 1;
    }
    else if(16 == m_pSettings->m_fmt.wBitsPerSample)
    {
        dwAlignment = 2;
    }
    else if((24 == m_pSettings->m_fmt.wBitsPerSample) || (32 == m_pSettings->m_fmt.wBitsPerSample))
    {
        dwAlignment = 4;
    }

    ASSERT(!(((DWORD)m_pSettings->m_pvBufferData + dwOffset) % dwAlignment));

#endif // DEBUG

    //
    // Calculate the proper base SGE index.  For FXIN buffers, it's the 
    // reserved 2047th one that's already set up for us.  For standard
    // buffers, calculate the proper index based on the offset from the
    // start of the base page.
    //
    
    ASSERT(m_pSettings->m_pvBufferData);
    
    if(m_pSettings->m_dwFlags & DSBCAPS_FXIN)
    {
        ASSERT(!m_pSgeHeapEntry);
        ASSERT(!dwOffset);
        
        dwOffset = BYTE_OFFSET(m_pSettings->m_pvBufferData);
        dwSgeIndex = MCPX_HW_MAX_BUFFER_PRDS - 1;
    }
    else
    {
        ASSERT(m_pSgeHeapEntry);

        dwSgeIndex = m_pSgeHeapEntry->nElement;
        
        dwOffset += BYTE_OFFSET(m_pSettings->m_pvBufferData);
        dwSgeIndex += dwOffset >> PAGE_SHIFT;

        ASSERT(dwSgeIndex < (DWORD)m_pSgeHeapEntry->nElement + (DWORD)m_pSgeHeapEntry->nLength);
        
        dwOffset &= PAGE_SIZE - 1;
    }

    //
    // Calculate the buffer address register value.  The low word represents
    // the page offset, while the high word represents the entry into the 
    // SGE table.
    //

    ASSERT(dwSgeIndex < MCPX_HW_MAX_BUFFER_PRDS);
    ASSERT(dwOffset < PAGE_SIZE);
    
    m_dwBufferBase = (dwSgeIndex << PAGE_SHIFT) | dwOffset;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  UnmapBuffer
 *
 *  Description:
 *      Releases the data buffer mapped into this voice.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::UnmapBuffer"

void
CMcpxBuffer::UnmapBuffer
(
    void
)
{
    DPF_ENTER();

    //
    // Make sure the voice is stopped
    //

    Stop();
    WaitForVoiceOff();

    //
    // Unmap the buffer
    //

    if(m_dwStatus & MCPX_VOICESTATUS_BUFFERMAPPED)
    {
        //
        // Clear the status bit
        //

        and(&m_dwStatus, ~MCPX_VOICESTATUS_BUFFERMAPPED);
        
        //
        // Release the SGE
        //

        if(m_pSgeHeapEntry)
        {
            m_pMcpxApu->m_SgeHeap.Free(m_pSgeHeapEntry);
            m_pSgeHeapEntry = NULL;
        }
    }
    else
    {
        ASSERT(!(m_dwStatus & ~MCPX_VOICESTATUS_ALLOCATED));
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Play
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: time to start playing.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::Play"

HRESULT
CMcpxBuffer::Play
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
        fDeferred = ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_PLAY, rtTimeStamp, dwFlags);
    }

    if(!fDeferred)
    {
        hr = Play(dwFlags);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Play
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::Play"

HRESULT
CMcpxBuffer::Play
(
    DWORD                   dwFlags
)
{
    DWORD                   dwCurrentBufferOffset;
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Make sure resources are allocated
    //

    hr = AllocateBufferResources();

    //
    // Handle the different modes of playing the buffer:  normal, 
    // extra-crispy... no, wait; normal, non-zero position, mixin.
    //

    if(SUCCEEDED(hr))
    {
        if(m_pSettings->m_dwFlags & DSBCAPS_MIXIN)
        {
            //
            // Just activate the voice.  The hardware takes care of the rest
            //
            
            ActivateVoice();
        }
        else 
        {
            //
            // Determine where to start playback from
            //
            
            if(!(dwFlags & DSBPLAY_FROMSTART) && !(m_dwStatus & MCPX_VOICESTATUS_ACTIVE))
            {
                dwCurrentBufferOffset = m_dwCachedPlayCursor;
            }
            else
            {
                dwCurrentBufferOffset = 0;
            }

            m_dwCachedPlayCursor = 0;

            //
            // Hand off to the proper play handler
            //
            
            if(dwCurrentBufferOffset)
            {
                PlayFromPosition(dwCurrentBufferOffset, dwFlags);
            }
            else
            {
                PlayFromCurrent(dwFlags);
            }

            //
            // Register for position notification callbacks
            //

            if(m_pSettings->m_dwNotifyCount)
            {
                ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA, 0, 0);
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  PlayFromCurrent
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to loop the buffer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::PlayFromCurrent"

void
CMcpxBuffer::PlayFromCurrent
(
    DWORD                   dwFlags
)
{
    DWORD                   dwLoopBackOffset;
    DWORD                   dwEndBufferOffset;
    DWORD                   dwIncrement;
    CIrql                   irql;
    HRESULT                 hr;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);
    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_MIXIN));

    //
    // Raise IRQL for synchronization
    //

    irql.Raise();

    //
    // Calculate the proper starting and ending buffer offsets as well
    // as the proper loop region
    //

    if(dwFlags & DSBPLAY_LOOPING)
    {
        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP);

        dwLoopBackOffset = BytesToSamples(m_pSettings->m_dwLoopStart);
        dwEndBufferOffset = BytesToSamples(m_pSettings->m_dwLoopStart + m_pSettings->m_dwLoopLength);

        or(&m_dwStatus, MCPX_VOICESTATUS_LOOPING);
    }
    else
    {
        m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_OFF, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP);

        dwLoopBackOffset = 0;
        dwEndBufferOffset = BytesToSamples(m_pSettings->m_dwPlayLength);

        and(&m_dwStatus, ~MCPX_VOICESTATUS_LOOPING);
    }

    ASSERT(dwEndBufferOffset > 1);
    dwEndBufferOffset--;

    dwIncrement = m_pSettings->m_fmt.wBitsPerSample * 2 / 8;

    MCPX_CHECK_VOICE_FIFO(8 * m_bVoiceCount);

    for(i = 0; i < m_bVoiceCount; i++)
    {
        MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
        MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
        MCPX_VOICE_WRITE(SetVoiceCfgBufBase, m_dwBufferBase + (dwIncrement * i));
        MCPX_VOICE_WRITE(SetVoiceCfgBufEBO, dwEndBufferOffset);
        MCPX_VOICE_WRITE(SetVoiceCfgBufLBO, dwLoopBackOffset);

        if(dwFlags & DSBPLAY_FROMSTART)
        {
            MCPX_VOICE_WRITE(SetVoiceBufCBO, 0);
        }

        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
    }

    // 
    // Lower IRQL
    //

    irql.Lower();

    //
    // Activate the voice
    //

    ActivateVoice();

    //
    // By the time we get to this point, any pending SSL or VoiceOff interrupts
    // should have happened.  Unschedule any pending RELEASERESOURCES tasks 
    // that may have been scheduled.
    //
    // BUGBUG: unless we're at raised IRQL through this whole function, such
    // as when the dmusic sequencer calls us or a deferred Play command happens.
    //

#pragma TODO("What impact does this have?")

    RemoveDeferredCommand(MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  PlayFromPosition
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      DWORD [in]: starting offset, in bytes.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::PlayFromPosition"

void
CMcpxBuffer::PlayFromPosition
(
    DWORD                   dwPosition,
    DWORD                   dwFlags
)
{
    MCPX_VOICE_VOLUME       Volume;
    DWORD                   dwState;
    CIrql                   irql;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ALLOCATED);
    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_MIXIN));
    ASSERT(!(dwFlags & DSBPLAY_FROMSTART));
    ASSERT(dwPosition);

    //
    // The hardware won't let us set the CBO until it's processed the first
    // frame.  Set the buffer up so it plays the first 32 samples in a loop
    // until the NEW_VOICE bit is cleared.
    //

    memset(&Volume, 0xFF, sizeof(Volume));

    irql.Raise();

    m_RegCache.CfgFMT = MCPX_SET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP);

    MCPX_CHECK_VOICE_FIFO(10 * m_bVoiceCount);

    for(i = 0; i < m_bVoiceCount; i++)
    {
        MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
        MCPX_VOICE_WRITE(SetVoiceCfgFMT, GetVoiceCfgFMT(i));
        MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
        MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
        MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
        MCPX_VOICE_WRITE(SetVoiceCfgBufBase, m_dwBufferBase);
        MCPX_VOICE_WRITE(SetVoiceCfgBufEBO, MCPX_HW_FRAME_SIZE_SAMPLES);
        MCPX_VOICE_WRITE(SetVoiceCfgBufLBO, 0);
        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
    }

    irql.Lower();

    ActivateVoice();

    //
    // Wait for the NEW_VOICE bit to clear so we know the first frame of audio
    // has been processed
    //

    do
    {
        MCPX_VOICE_STRUCT_READ(m_ahVoices[m_bVoiceCount - 1], NV_PAVS_VOICE_PAR_STATE, &dwState);
    }
    while(MCPX_GET_REG_VALUE(dwState, NV_PAVS_VOICE_PAR_STATE_NEW_VOICE));

    //
    // Play the buffer properly
    //

    PlayFromCurrent(dwFlags);

    //
    // Set the cursor position and restore the volume
    //

    irql.Raise();

    ConvertVolumeValues(&Volume);

    dwPosition = BytesToSamples(dwPosition);

    MCPX_CHECK_VOICE_FIFO(7 * m_bVoiceCount);
    
    for(i = 0; i < m_bVoiceCount; i++)
    {
        MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
        MCPX_VOICE_WRITE(SetVoiceTarVOLA, Volume.TarVOLA[i]);
        MCPX_VOICE_WRITE(SetVoiceTarVOLB, Volume.TarVOLB[i]);
        MCPX_VOICE_WRITE(SetVoiceTarVOLC, Volume.TarVOLC[i]);
        MCPX_VOICE_WRITE(SetVoiceBufCBO, dwPosition);
        MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
    }

    irql.Lower();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetPlayRegion
 *
 *  Description:
 *      Sets the buffer's play region.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::SetPlayRegion"

HRESULT
CMcpxBuffer::SetPlayRegion
(
    void
)
{
    HRESULT                 hr      = DS_OK;
    DWORD                   dwFlags;
 
    DPF_ENTER();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));

    //
    // Reset the cached play cursor
    //

    m_dwCachedPlayCursor = 0;

    //
    // Recalculate the buffer base address
    //

    if(m_dwStatus & MCPX_VOICESTATUS_BUFFERMAPPED)
    {
        MapBuffer();
    }

    //
    // If we're playing, call Play again.  That will reset the regions
    // based on what was just set.
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        dwFlags = DSBPLAY_FROMSTART;

        if(m_dwStatus & MCPX_VOICESTATUS_LOOPING)
        {
            dwFlags |= DSBPLAY_LOOPING;
        }
        
        hr = Play(dwFlags);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetLoopRegion
 *
 *  Description:
 *      Sets the buffer's loop region.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::SetLoopRegion"

HRESULT
CMcpxBuffer::SetLoopRegion
(
    void
)
{
    HRESULT                 hr  = DS_OK;
 
    DPF_ENTER();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));

    //
    // If we're playing, call Play again.  That will reset the regions
    // based on what was just set.
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        hr = Play((m_dwStatus & MCPX_VOICESTATUS_LOOPING) ? DSBPLAY_LOOPING : 0);
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
#define DPF_FNAME "CMcpxBuffer::Stop"

HRESULT
CMcpxBuffer::Stop
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
        fDeferred = ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_STOP, rtTimeStamp, dwFlags);
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
 *      Pauses a playing buffer.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::Stop"

HRESULT
CMcpxBuffer::Stop
(
    DWORD                   dwFlags
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();

    //
    // Make sure resources are allocated and we're actually running
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        if(dwFlags & DSBSTOPEX_ENVELOPE)
        {
            //
            // If we're supposed to break out of the loop, just call Play again
            // without the loop flag.
            //

            if(dwFlags & DSBSTOPEX_RELEASEWAVEFORM)
            {
                if((m_dwStatus & MCPX_VOICESTATUS_LOOPINGMASK) == MCPX_VOICESTATUS_LOOPINGMASK)
                {
                    hr = Play(0);
                }
            }

            //
            // Enter the release segment
            //

            if(SUCCEEDED(hr))
            {
                ReleaseVoice();
            }
        }
        else
        {
            //
            // Cache the current play cursor position
            //

            if(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK))
            {
                GetCurrentPosition(&m_dwCachedPlayCursor, NULL);
            }

            //
            // Stop the voice
            //

            DeactivateVoice();

            //
            // Signal positions up to this point
            //

            OnPositionDelta();

            //
            // If we're LOCDEFER, schedule a deferred command to release voice
            // resources
            //

            if(m_pSettings->m_dwFlags & DSBCAPS_LOCDEFER)
            {
                ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES, 0, 0);
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets the current play state of the buffer.
 *
 *  Arguments:
 *      LPDWORD [out]: status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::GetStatus"

HRESULT
CMcpxBuffer::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    DPF_ENTER();
    AutoIrql();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));

    //
    // Convert the voice state to buffer status.  We're going to look at
    // the VOICEOFF bit so IDirectSoundBuffer::GetStatus is as accurate as
    // possible.  This will allow a client to spin on GetStatus until the
    // voice is really inactive, preventing us from busy-waiting later.
    //

    *pdwStatus = 0;
    
    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        if(m_dwStatus & MCPX_VOICESTATUS_ACTIVEORVOICEOFF)
        {
            *pdwStatus |= DSBSTATUS_PLAYING;

            if(m_dwStatus & MCPX_VOICESTATUS_LOOPING)
            {
                *pdwStatus |= DSBSTATUS_LOOPING;
            }
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  GetCurrentPosition
 *
 *  Description:
 *      Gets the current play and write cursor positions.
 *
 *  Arguments:
 *      LPDWORD [out]: play cursor position, in bytes.
 *      LPDWORD [out]: write cursor position, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::GetCurrentPosition"

HRESULT
CMcpxBuffer::GetCurrentPosition
(
    LPDWORD                 pdwPlayCursor,
    LPDWORD                 pdwWriteCursor
)
{
    DWORD                   dwPlayCursor;
    DWORD                   dwFrameSize;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));

    //
    // Get the current SSL position if we're playing or used the cached one
    // if we're not.
    //
    // When we're playing, the write cursor is 32 samples ahead of the play 
    // cursor.
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        dwPlayCursor = GetSslPosition();

        if(pdwPlayCursor)
        {
            *pdwPlayCursor = dwPlayCursor;
        }

        if(pdwWriteCursor)
        {
            dwFrameSize = SamplesToBytes(MCPX_HW_FRAME_SIZE_SAMPLES);

            if(dwFrameSize < m_pSettings->m_fmt.nBlockAlign)
            {
                dwFrameSize = m_pSettings->m_fmt.nBlockAlign;
            }
            
            *pdwWriteCursor = dwPlayCursor + dwFrameSize;
    
            if((m_dwStatus & MCPX_VOICESTATUS_LOOPING) && (dwPlayCursor >= m_pSettings->m_dwLoopStart) && (dwPlayCursor < m_pSettings->m_dwLoopStart + m_pSettings->m_dwLoopLength))
            {
                *pdwWriteCursor %= m_pSettings->m_dwLoopLength;
                *pdwWriteCursor += m_pSettings->m_dwLoopStart;
            }
            else
            {
                *pdwWriteCursor %= m_pSettings->m_dwBufferSize;
            }
        }
    }
    else
    {
        if(pdwPlayCursor)
        {
            *pdwPlayCursor = m_dwCachedPlayCursor;
        }

        if(pdwWriteCursor)
        {
            *pdwWriteCursor = m_dwCachedPlayCursor;
        }
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetCurrentPosition
 *
 *  Description:
 *      Sets the current play cursor position.
 *
 *  Arguments:
 *      DWORD [in]: play cursor position, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::SetCurrentPosition"

HRESULT
CMcpxBuffer::SetCurrentPosition
(
    DWORD                   dwPlayCursor
)
{
    HRESULT                 hr      = DS_OK;
    DWORD                   dwState;
    CIrql                   irql;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));
    ASSERT(dwPlayCursor <= m_pSettings->m_dwPlayLength);

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        //
        // Wait for the NEW_VOICE bit to clear so we can set the CBO.  The
        // hardware will ignore the CBO until it's processed at least one
        // frame.
        //

        do
        {
            MCPX_VOICE_STRUCT_READ(m_ahVoices[m_bVoiceCount - 1], NV_PAVS_VOICE_PAR_STATE, &dwState);
        }
        while(MCPX_GET_REG_VALUE(dwState, NV_PAVS_VOICE_PAR_STATE_NEW_VOICE));

        //
        // If the cursor position is outside the loop region, toggle
        // the loop flag.
        //

        if((m_dwStatus & MCPX_VOICESTATUS_LOOPINGMASK) == MCPX_VOICESTATUS_LOOPINGMASK)
        {
            if(dwPlayCursor >= m_pSettings->m_dwLoopStart + m_pSettings->m_dwLoopLength)
            {
                hr = Play(0);
            }
        }

        //
        // Set voice position
        //
        
        if(SUCCEEDED(hr))
        {
            dwPlayCursor = BytesToSamples(dwPlayCursor);

            DPF_INFO("Voice %x (%x) setting current buffer offset to %lu", this, m_ahVoices[0], dwPlayCursor);

            irql.Raise();
        
            MCPX_CHECK_VOICE_FIFO(2 * m_bVoiceCount);
            
            for(i = 0; i < m_bVoiceCount; i++)
            {
                MCPX_VOICE_WRITE(SetCurrentVoice, m_ahVoices[i]);
                MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON);
                MCPX_VOICE_WRITE(SetVoiceBufCBO, dwPlayCursor);
                MCPX_VOICE_WRITE(VoiceLock, NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF);
            }

            irql.Lower();
        }
    }
    else
    {
        //
        // Cache the play cursor for the next time we call Play
        //
        
        m_dwCachedPlayCursor = dwPlayCursor;
    }

    //
    // Signal position events
    //

    OnPositionDelta();

    DPF_LEAVE_HRESULT(hr);

    return hr;
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
#define DPF_FNAME "CMcpxBuffer::ServiceVoiceInterrupt"

BOOL
CMcpxBuffer::ServiceVoiceInterrupt
(
    void
)
{
    BOOL                    fHandled;

    DPF_ENTER();

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);
    ASSERT(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED);
    
    if(fHandled = m_Notifier.GetStatus(MCPX_NOTIFIER_VOICE_OFF))
    {
        //
        // Reset the notifier
        //
        
        m_Notifier.SetStatus(MCPX_NOTIFIER_VOICE_OFF, FALSE);

        //
        // Handle the stop
        //

        DeactivateVoice();

        //
        // If we're LOCDEFER, free voice resources
        //

        if(m_pSettings->m_dwFlags & DSBCAPS_LOCDEFER)
        {
            ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES, 0, 0);
        }
    }

    DPF_LEAVE(fHandled);

    return fHandled;
}


/****************************************************************************
 *
 *  SetNotificationPositions
 *
 *  Description:
 *      Sets notification positions.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::SetNotificationPositions"

HRESULT
CMcpxBuffer::SetNotificationPositions
(
    void
)
{
    DPF_ENTER();
    AutoIrql();

    //
    // If we're playing, make sure the position delta event is set up
    //

    if((m_dwStatus & MCPX_VOICESTATUS_ACTIVEMASK) == MCPX_VOICESTATUS_ACTIVEMASK)
    {
        if(m_pSettings->m_dwNotifyCount)
        {
            ScheduleDeferredCommand(MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA, 0, 0);
        }
    }

    //
    // Reset the notification index
    //

    m_dwLastNotifyPosition = -1;
    m_dwNextNotifyIndex = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  OnPositionDelta
 *
 *  Description:
 *      Signals position notifications.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::OnPositionDelta"

void
CMcpxBuffer::OnPositionDelta
(
    void
)
{
    DWORD                   dwPlayCursor;
    DWORD                   dwStartPosition;
    DWORD                   dwEndPosition;
    HRESULT                 hr;
    
    DPF_ENTER();

    if(m_pSettings->m_dwNotifyCount)
    {
        ASSERT(m_pSettings->m_paNotifies);

        //
        // Compare the current position to the last and signal the events in
        // between
        //

        if(NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON == MCPX_GET_REG_VALUE(m_RegCache.CfgFMT, NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP))
        {
            dwStartPosition = m_pSettings->m_dwLoopStart;
            dwEndPosition = m_pSettings->m_dwLoopStart + m_pSettings->m_dwLoopLength;
        }
        else
        {
            dwStartPosition = 0;
            dwEndPosition = m_pSettings->m_dwPlayLength;
        }

        hr = GetCurrentPosition(&dwPlayCursor, NULL);
        ASSERT(SUCCEEDED(hr));

        if((dwPlayCursor > m_dwLastNotifyPosition) || (m_dwLastNotifyPosition > dwEndPosition))
        {
            //
            // The play cursor has moved forward.  Signal the events in
            // between where we were and where we are.
            //

            NotifyToPosition(dwPlayCursor);
        }
        else if(dwPlayCursor < m_dwLastNotifyPosition)
        {
            //
            // The play cursor has moved backwards.  We'll assume it's because 
            // we looped.  Signal to the end of the buffer, reset the 
            // notification index to the beginning of the play region, then 
            // signal up to the current position.
            //

            NotifyToPosition(dwEndPosition);

            m_dwNextNotifyIndex = 0;

            NotifyToPosition(dwStartPosition, FALSE);
            NotifyToPosition(dwPlayCursor);
        }

        //
        // If the buffer isn't playing anymore, signal the stop event and
        // unregister for position delta callbacks.
        //

        if(!(m_dwStatus & MCPX_VOICESTATUS_ACTIVEORVOICEOFF))
        {
            RemoveDeferredCommand(MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA);
            NotifyStop();
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  NotifyToPosition
 *
 *  Description:
 *      Signals position notifications.
 *
 *  Arguments:
 *      DWORD [in]: play cursor position.
 *      BOOL [in]: TRUE to signal notifications; FALSE to just move the
 *                 indices.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::NotifyToPosition"

void
CMcpxBuffer::NotifyToPosition
(
    DWORD                   dwPlayCursor,
    BOOL                    fSignal
)
{
    DPF_ENTER();

    ASSERT(m_pSettings->m_dwNotifyCount);
    ASSERT(m_pSettings->m_paNotifies);

    while(TRUE)
    {
        if(m_dwNextNotifyIndex >= m_pSettings->m_dwNotifyCount)
        {
            break;
        }
        else if(DSBPN_OFFSETSTOP == m_pSettings->m_paNotifies[m_dwNextNotifyIndex].dwOffset)
        {
            break;
        }
        else if(m_pSettings->m_paNotifies[m_dwNextNotifyIndex].dwOffset >= dwPlayCursor)
        {
            break;
        }

        if(fSignal)
        {
            NtSetEvent(m_pSettings->m_paNotifies[m_dwNextNotifyIndex].hEventNotify, NULL);
        }

        m_dwNextNotifyIndex++;
    }

    m_dwLastNotifyPosition = dwPlayCursor;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  NotifyStop
 *
 *  Description:
 *      Signals position notifications.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::NotifyStop"

void
CMcpxBuffer::NotifyStop
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();

    ASSERT(m_pSettings->m_dwNotifyCount);
    ASSERT(m_pSettings->m_paNotifies);

    for(i = m_pSettings->m_dwNotifyCount; i > 0; i--)
    {
        if(DSBPN_OFFSETSTOP != m_pSettings->m_paNotifies[i - 1].dwOffset)
        {
            break;
        }

        NtSetEvent(m_pSettings->m_paNotifies[i - 1].hEventNotify, NULL);
    }

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
#define DPF_FNAME "CMcpxBuffer::ScheduleDeferredCommand"

BOOL 
CMcpxBuffer::ScheduleDeferredCommand
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
#define DPF_FNAME "CMcpxBuffer::RemoveDeferredCommand"

void 
CMcpxBuffer::RemoveDeferredCommand
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
#define DPF_FNAME "CMcpxBuffer::ServiceDeferredCommand"

void 
CMcpxBuffer::ServiceDeferredCommand
(
    DWORD                   dwCommand, 
    DWORD                   dwContext
)
{
    DPF_ENTER();
    
    switch(dwCommand)
    {
        case MCPX_DEFERREDCMD_BUFFER_PLAY:
            Play(dwContext);
            break;

        case MCPX_DEFERREDCMD_BUFFER_STOP:
            Stop(dwContext);
            break;

        case MCPX_DEFERREDCMD_BUFFER_POSITIONDELTA:
            OnPositionDelta();
            break;

        case MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES:
            OnDeferredTerminate();
            break;

        case MCPX_DEFERREDCMD_BUFFER_CHECKSTUCK:
            CheckStuckVoice();
            break;

        default:
            ASSERTMSG("Unexpected deferred command identifier");
            break;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  OnDeferredTerminate
 *
 *  Description:
 *      Checks for a stopped voice and frees voice resources.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CMcpxBuffer::OnDeferredTerminate"

void 
CMcpxBuffer::OnDeferredTerminate
(
    void
)
{
    DPF_ENTER();

    if(m_dwStatus & MCPX_VOICESTATUS_ALLOCATED)
    {
        if(!(m_dwStatus & MCPX_VOICESTATUS_ACTIVEORVOICEOFF))
        {
            ReleaseBufferResources();
        }
    }
    else
    {
        RemoveDeferredCommand(MCPX_DEFERREDCMD_BUFFER_RELEASERESOURCES);
    }

    DPF_LEAVE_VOID();
}


