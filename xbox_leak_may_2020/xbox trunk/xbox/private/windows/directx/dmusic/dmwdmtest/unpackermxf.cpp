/*  Base implementation of MIDI event unpacker

    05/19/98    Jim Geist           Created this file

*/

#include "stdafx.h"
#include <assert.h>
#include "AllocatorMXF.h"
#include "UnpackerMXF.h"
#include "Ks.h"
#include "KsMedia.h"

// Alignment macros
//
#define DWORD_ALIGN(x) (((x) + 3) & ~3)
#define QWORD_ALIGN(x) (((x) + 7) & ~7)

// MIDI parsing helpers
//
#define IS_DATA_BYTE(x)     (((x) & 0x80) == 0)
#define IS_STATUS_BYTE(x)   ((x) & 0x80)
#define IS_CHANNEL_MSG(x)   (IS_STATUS_BYTE(x) && (((x) & 0xF0) != 0xF0))
#define IS_REALTIME_MSG(x)  (((x) & 0xF8) == 0xF8)
#define IS_SYSTEM_COMMON(x) (((x) & 0xF8) == 0xF0)
#define IS_SYSEX(x)         ((x) == 0xF0)
#define IS_SYSEX_END(x)     ((x) == 0xF7)

#define SYSEX_END           0xF7

static ULONG cbChannelMsgData[] =
{
    2,  /* 0x8x Note off */
    2,  /* 0x9x Note on */
    2,  /* 0xAx Polyphonic key pressure/aftertouch */
    2,  /* 0xBx Control change */
    1,  /* 0xCx Program change */
    1,  /* 0xDx Channel pressure/aftertouch */
    2,  /* 0xEx Pitch change */
    0,  /* 0xFx System message - use cbSystemMsgData */
};

static ULONG cbSystemMsgData[] =
{
    0,  /* 0xF0 SysEx (variable until F7 seen) */
         
        /* System common messages */
    1,  /* 0xF1 MTC quarter frame */
    2,  /* 0xF2 Song position pointer */
    1,  /* 0xF3 Song select */
    0,  /* 0xF4 Undefined */
    0,  /* 0xF5 Undefined */
    0,  /* 0xF6 Tune request */
    0,  /* 0xF7 End of sysex flag */

        /* System realtime messages */
    0,  /* 0xF8 Timing clock */
    0,  /* 0xF9 Undefined */
    0,  /* 0xFA Start */
    0,  /* 0xFB Continue */
    0,  /* 0xFC Stop */
    0,  /* 0xFD Undefined */
    0,  /* 0xFE Active sensing */
    0,  /* 0xFF System reset */
};

#define STATUS_MSG_DATA_BYTES(x)    \
    (IS_CHANNEL_MSG(x) ? cbChannelMsgData[((x) >> 4) & 0x07] : cbSystemMsgData[(x) & 0x0F])

///////////////////////////////////////////////////////////////////////
//
// CUnpackerMXF
//
// Code which is common to all unpackers
//

// CUnpackerMXF::CUnpackerMXF
//
// Get the system page size, which unpackers use as the maximum buffer size.
//
CUnpackerMXF::CUnpackerMXF(CAllocatorMXF *allocatorMXF)
 : CMXF(allocatorMXF)
{
    m_sinkMXF = NULL;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    m_pageSize = si.dwPageSize;

    HRESULT hr = m_allocatorMXF->GetMessage(&m_nextSysExEvent);
    if (FAILED(hr))
    {
        m_nextSysExEvent = NULL;
    }

    m_bRunningStatus = 0;
    m_state = stateNone;
}

// CUnpackerMXF::~CUnpackerMXF
//
CUnpackerMXF::~CUnpackerMXF()
{
    DisconnectOutput(m_sinkMXF);    
}

// CUnpackerMXF::ConnectOutput
//
// It is an error to connect to nothing (use DisconnectOutput) or to connect
// an unpacker to more than one sink (use a splitter).
//
HRESULT CUnpackerMXF::ConnectOutput(CMXF *sinkMXF)
{
    if (m_sinkMXF || !sinkMXF) 
    {
        return S_FALSE;
    }

    m_sinkMXF = sinkMXF;
    return S_OK;
}

// CUnpackerMXF::DisconnectOutput
//
// Validate that the unpacker is connected and the disconnection applies
// to the correct filter.
//
HRESULT CUnpackerMXF::DisconnectOutput(CMXF *sinkMXF)
{
    if ((m_sinkMXF == sinkMXF) || (sinkMXF == NULL))
    {
        m_sinkMXF = NULL;
        return S_OK;
    }
    return S_FALSE;
}

// CUnpackerMXF::PutMessage
//
// An unpacker's upper edge is type dependent, but it is not by definition an MXF interface.
// Therefore this method should never be called.
//
HRESULT CUnpackerMXF::PutMessage(PDMUS_KERNEL_EVENT)
{       
    return E_FAIL;
}

// CUnpackerMXF::QueueShortEvent
//
// Create and put an MXF event with a short message (anything other than system exclusive data).
// By definition this data must fit in 4 bytes or less.
//
HRESULT CUnpackerMXF::QueueShortEvent(
    LPBYTE pbData, 
    USHORT cbData, 
    ULONGLONG ullPresTime, 
    ULONG dwChannelGroup)
{
    HRESULT hr;

    PDMUS_KERNEL_EVENT  pMXFEvent;

    hr = m_allocatorMXF->GetMessage(&pMXFEvent);
    if (FAILED(hr)) 
    {
        return hr;
    }   

    pMXFEvent->cbEvent          = cbData; 
    pMXFEvent->usFlags          = 0;
    pMXFEvent->ulChannelGroup   = dwChannelGroup;
    pMXFEvent->ullPresTime100Ns = ullPresTime;
    pMXFEvent->pNextEvt         = NULL;
 
    // Short event by definition is < sizeof(PBYTE)
    //
    assert(cbData <= sizeof(PBYTE));
    /*RTL*/CopyMemory(&pMXFEvent->uData.abData[0], pbData, cbData);

    if (FAILED(m_sinkMXF->PutMessage(pMXFEvent)))
    {
        m_allocatorMXF->PutMessage(pMXFEvent);
        return S_FALSE;
    }
    
    return S_OK;
}

// CUnpackerMXF::QueueSysEx
//
// Create and put an MXF event which contains system exclusive data. This data must already
// be truncated into page-sized buffers.
//
HRESULT CUnpackerMXF::QueueSysEx(
    LPBYTE pbData,
    USHORT cbData,
    ULONGLONG ullPresTime,
    ULONG dwChannelGroup,
    BOOL fIsContinued)
{
    assert(cbData <= PageSize());

    // Get the preallocated next sysex event and allocate a new next. This
    // ensures linkage in the chain of sysex we pass along to the
    // graph.
    //
    if (!m_nextSysExEvent)
    {
        return E_OUTOFMEMORY;
    }

    PDMUS_KERNEL_EVENT pMXFEvent = m_nextSysExEvent;
    HRESULT hr = m_allocatorMXF->GetMessage(&m_nextSysExEvent);
    if (FAILED(hr))
    {
        m_allocatorMXF->PutMessage(pMXFEvent);
        m_nextSysExEvent = NULL;

        return E_OUTOFMEMORY;
    }

    // Build the event.
    //
    pMXFEvent->cbEvent          = cbData; 
    pMXFEvent->usFlags          = (USHORT)(fIsContinued ? DMUS_KEF_EVENT_INCOMPLETE : 0);
    pMXFEvent->ulChannelGroup   = dwChannelGroup;
    pMXFEvent->ullPresTime100Ns = ullPresTime;
    pMXFEvent->pNextEvt         = fIsContinued ? m_nextSysExEvent : NULL;
 
    if (cbData <= sizeof(PBYTE)) 
    {
        /*RTL*/CopyMemory(&pMXFEvent->uData.abData[0], pbData, cbData);
    }
    else
    {
        // Event data won't fit in uData, so allocate some memory to
        // hold it.
        //
        pMXFEvent->uData.pbData = /*ExAllocatePool*/new BYTE[cbData];
        if (pMXFEvent->uData.pbData ==  NULL)
        {
            // XXX This is going to cause problems in the stack - the MXF chain
            // will forever be waiting for this event which will never come
            //
            m_allocatorMXF->PutMessage(pMXFEvent);

            return E_OUTOFMEMORY;
        }

        /*RTL*/CopyMemory(pMXFEvent->uData.pbData, pbData, cbData);
    }
    
    if (FAILED(m_sinkMXF->PutMessage(pMXFEvent)))
    {
        m_allocatorMXF->PutMessage(pMXFEvent);
        return S_FALSE;
    }
    
    return S_OK;
}

// CUnpackerMXF::UnpackEventBytes
//
// This is basically a MIDI parser with state. It assumes nothing about alignment of
// events to the buffers that are passed in - a message may cross calls.
//
// QueueEvent's return code is not checked; we don't want to lose state if we 
// can't queue a message.
//
//
HRESULT CUnpackerMXF::UnpackEventBytes(ULONGLONG ullCurrentTime, ULONG ulChannelGroup, LPBYTE pbDataStart, ULONG cbData)
{
    LPBYTE      pbData = pbDataStart;
    LPBYTE      pbSysExStart;
    BYTE        bData;

    if (m_state == stateInSysEx) 
    {
        pbSysExStart = pbData;
    }

    while (cbData)
    {
        bData = *pbData++;
        cbData--;

        // Realtime messages have priority over anything else. They can appear anywhere and 
        // don't change the state of the stream.
        //
        if (IS_REALTIME_MSG(bData))
        {
            QueueShortEvent(&bData, sizeof(bData), ullCurrentTime, ulChannelGroup);

            // Did this interrupt a SysEx? Spit out the contiguous buffer so far
            // and reset the start pointer. State is still in SysEx.
            //
            // Other messages are copied as they are parsed so no need to change their
            // parsing state.
            //
            if (m_state == stateInSysEx)
            {
                USHORT cbSysEx = (USHORT)((pbData - 1) - pbSysExStart);

                if (cbSysEx)
                {
                    QueueSysEx(pbSysExStart, cbSysEx, m_ullEventTime, ulChannelGroup, TRUE);
                }

                pbSysExStart = pbData;
            }

            continue;
        }

        // If we're parsing a SysEx, just pass over data bytes - they will be dealt with
        // when we reach a terminating condition (end of buffer or a status byte).
        //
        if (m_state == stateInSysEx)
        {
            if (!IS_STATUS_BYTE(bData))
            {
                // Don't allow a single buffer to grow to more than the system page size
                //
                USHORT cbSysEx = (USHORT)(pbData - pbSysExStart);
                if (cbSysEx >= PageSize())
                {
                    QueueSysEx(pbSysExStart, cbSysEx, m_ullEventTime, ulChannelGroup, TRUE);
                    pbSysExStart = pbData;
                }

                continue;
            }
       
            // Trickery: We have the end of the SysEx. We always want to plant an F7 in the end of 
            // the sysex so anyone watching the buffers above us will know when it ends, even if
            // it was truncated. XXX Indication of truncation?
            //
            pbData[-1] = SYSEX_END;

            // Unlike the above case, we are guaranteed at least one byte to pack here
            //
            QueueSysEx(pbSysExStart, (USHORT)(pbData - pbSysExStart), m_ullEventTime, ulChannelGroup, FALSE);

            // Restore original data. If this was a real end of sysex, then eat the byte and
            // continue.
            //
            pbData[-1] = bData;

            m_state = stateNone;
            if (IS_SYSEX_END(bData)) 
            {
                continue;
            }
        }                                       

        // If we're starting a SysEx, tag it.
        //
        if (IS_SYSEX(bData))
        {
#ifdef DEBUG
            if (m_state != m_stateNone)
            {
                //Trace("SysEx interrupted another msg. Oh well.");
            }
#endif
            // Note that we've already advanced over the start byte.
            //
            m_ullEventTime = ullCurrentTime;
            pbSysExStart = pbData - 1;
            m_state = stateInSysEx;

            continue;
        }

        if (IS_STATUS_BYTE(bData))
        {
            // We have a status byte. Even if we're already in the middle of a short
            // message, we have to start a new one
            //
#ifdef DEBUG     
            if (state == m_stateInShortMsg)
            {
                //Trace("Short message interrupted by another short msg");
            }
#endif                                

            m_abShortMsg[0]     = bData;
            m_pbShortMsg        = &m_abShortMsg[1];
            m_cbShortMsgLeft    = STATUS_MSG_DATA_BYTES(bData);
            m_ullEventTime      = ullCurrentTime;
            m_state             = stateInShortMsg;

            // Update running status
            // System common -> clear running status
            // Channel message -> change running status
            //
            m_bRunningStatus = 0;
            if (IS_CHANNEL_MSG(bData))
            {
                m_bRunningStatus = bData;
            }
        }
        else
        {
            // Not a status byte. If we're not in a short message,
            // start one with running status. 
            // 
            if (m_state != stateInShortMsg)  
            {
#ifdef DEBUG     
                if (m_state == stateInShortMsg)
                {
                    //Trace("Short message interrupted by another short msg");
                }
#endif                                
                if (m_bRunningStatus == 0) 
                {
                    //Trace("Attempt to use running status with no pending status byte");
                    continue;
                }

                m_abShortMsg[0]     = m_bRunningStatus;
                m_pbShortMsg        = &m_abShortMsg[1];
                m_cbShortMsgLeft    = STATUS_MSG_DATA_BYTES(m_bRunningStatus);
                m_ullEventTime      = ullCurrentTime;
                m_state             = stateInShortMsg;
            }

            // Now we are guaranteed to be in a short message, and can safely add this
            // byte. Note that since running status is only allowed on channel messages,
            // we are also guaranteed at least one byte of expected data, so no need
            // to check for that.
            //
            *m_pbShortMsg++ = bData;
            m_cbShortMsgLeft--;
        }

        // See if we've finished a short message, and if so, queue it.
        //
        if (m_state == stateInShortMsg && m_cbShortMsgLeft == 0)
        {
            QueueShortEvent(m_abShortMsg, (USHORT)(m_pbShortMsg - m_abShortMsg), m_ullEventTime, ulChannelGroup);
            m_state = stateNone;
        }
    }    

    // If we got part of a SysEx but ran out of buffer, queue that part
    // without leaving the SysEx
    //
    if (m_state == stateInSysEx)
    {
        QueueSysEx(pbSysExStart, (USHORT)(pbData - pbSysExStart), m_ullEventTime, ulChannelGroup, TRUE);
    }

    return S_OK;
}
 
///////////////////////////////////////////////////////////////////////
//
// CDMusUnpackerMXF
//
// Unpacker which understands DirectMusic buffer format.
//

// DirectMusic event as packed by IDirectMusic
//
struct DMEVENT 
{
    DWORD               cbEvent;                // Unrounded number of event bytes 
    DWORD               dwChannelGroup;         // This field determines which channel group 
                                                // (set of 16 MIDI channels) receives the event. 
    ULONGLONG           rtDelta;                // Offset from buffer header in 100 ns units 
    BYTE                abEvent[4];             // Actual event data, rounded up to be an even number 
                                                // of QWORD's (8 bytes) 
};

#define DMEVENT_SIZE(cbData)    (QWORD_ALIGN(sizeof(DMEVENT) + (cbData) - 4))

// CDMusUnpackerMXF::CDMusUnpackerMXF
//
CDMusUnpackerMXF::CDMusUnpackerMXF(CAllocatorMXF *allocatorMXF) :
    CUnpackerMXF(allocatorMXF)
{                                  
}
 
// CDMusUnpackerMXF::~CDMusUnpackerMXF
//
CDMusUnpackerMXF::~CDMusUnpackerMXF()
{
}   

// CDMusUnpackerMXF::SinkIRP
//
// This does all the work. Use the MIDI parser to split up DirectMusic events.
// Using the parser is a bit of overkill, but it handles saving state across 
// buffers for SysEx, which we have to support.
//
HRESULT CDMusUnpackerMXF::SinkIRP(LPBYTE pbData, ULONG cbData, ULONGLONG ullBaseTime)
{
    while (cbData)
    {
        if (cbData < sizeof(DMEVENT)) 
        {
            // Trace("Buffer too small for event header");
            return E_INVALIDARG;
        }

        DMEVENT *pEvent = (DMEVENT*)pbData;
        DWORD cbFullEvent = sizeof(DMEVENT) + QWORD_ALIGN(pEvent->cbEvent);

        if (cbFullEvent > cbData) 
        {
            // Trace("Event past end of buffer");
            return E_INVALIDARG;
        }

        pbData += cbFullEvent;
        cbData -= cbFullEvent;

        // Event is intact, let's build an MXF event for it
        //
        LPBYTE pbThisEvent = &pEvent->abEvent[0];
        ULONG  cbThisEvent = pEvent->cbEvent;

        UnpackEventBytes(ullBaseTime + pEvent->rtDelta, pEvent->dwChannelGroup, pbThisEvent, cbThisEvent);
    }        

    return S_OK;
}


///////////////////////////////////////////////////////////////////////
//
// CKsUnpackerMXF
//
// Unpacker which understands KSMUSICFORMAT.
//

// CKsUnpackerMXF::CKsUnpackerMXF
//
CKsUnpackerMXF::CKsUnpackerMXF(CAllocatorMXF *allocatorMXF) :
    CUnpackerMXF(allocatorMXF)
{
}
 
// CKsUnpackerMXF::~CKsUnpackerMXF
//
CKsUnpackerMXF::~CKsUnpackerMXF()
{
}   

// CKsUnpackerMXF::~SinkIRP
//
// Parse the MIDI stream, assuming nothing about what might cross a packet or IRP boundary.
//
// An IRP buffer contains one or more KSMUSICFORMAT headers, each with data. Pull them apart
// and call UnpackEventBytes to turn them into messages.
//
HRESULT CKsUnpackerMXF::SinkIRP(LPBYTE pbData, ULONG cbData, ULONGLONG ullBaseTime)
{
    HRESULT hr;

    // This data can consist of multiple KSMUSICFORMAT headers, each w/ associated bytestream data
    //
    ULONGLONG ullCurrentTime = ullBaseTime;
    while (cbData)
    {
        if (cbData < sizeof(KSMUSICFORMAT)) 
        {
            //Trace("Buffer too small for header");
            return E_INVALIDARG;
        }

        PKSMUSICFORMAT pksmf = (PKSMUSICFORMAT)pbData;
        pbData += sizeof(KSMUSICFORMAT);
        cbData -= sizeof(KSMUSICFORMAT);

        ULONG cbPacket = pksmf->ByteCount;
        if (cbPacket > cbData) 
        {
            //Trace("Packet length longer than IRP buffer - truncated");
            cbPacket = cbData;
        }

        // XXX What is the base of this clock
        // XXX How do we relate this to the master clock? Is legacy time always KeQueryPerformanceCounter
        //
        ullCurrentTime += pksmf->TimeDeltaMs * 10000;

        hr = UnpackEventBytes(ullCurrentTime, /* ChannelGroup */ 1, pbData, cbPacket);
        if (FAILED(hr))
        {
            return hr;
        }

        cbPacket = DWORD_ALIGN(cbPacket);
        assert(cbPacket <= cbData);
        pbData += cbPacket;
        cbData -= cbPacket;
    }    

    return S_OK;
}


