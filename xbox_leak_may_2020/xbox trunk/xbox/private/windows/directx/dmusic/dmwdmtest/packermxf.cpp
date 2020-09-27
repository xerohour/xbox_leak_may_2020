/*  Base implementation of MIDI event packer

    05/22/98    Jim Geist           Created this file

*/

#include "stdafx.h"
#include <assert.h>
#include "PackerMXF.h"
#include "Ks.h"
#include "KsMedia.h"

// Alignment macros
//
#define DWORD_ALIGN(x) (((x) + 3) & ~3)     // Pad to next DWORD
#define DWORD_TRUNC(x) ((x) & ~3)           // Trunc to DWORD's that will fit
#define QWORD_ALIGN(x) (((x) + 7) & ~7)
#define QWORD_TRUNC(x) ((x) & ~7)


///////////////////////////////////////////////////////////////////////
//
// CPackerMXF
//
// Code which is common to all packers
//

// CPackerMXF::CPackerMXF
//
// XXX Might want one m_evtNodePool per PortClass, not per graph instance.
//
CPackerMXF::CPackerMXF(CAllocatorMXF *allocatorMXF) 
    : CMXF(allocatorMXF)

{
    m_irpHead       = NULL;
    m_irpTail       = NULL;
    m_evtHead       = NULL;
    m_evtTail       = NULL;
    m_ullLastTime   = 0;
}

// CPackerMXF::~CPackerMXF
//
CPackerMXF::~CPackerMXF()
{
}

// CPackerMXF::ConnectOutput
//
// Fail; this filter is an MXF sink only
//
HRESULT CPackerMXF::ConnectOutput(CMXF *)
{
    return E_FAIL;
}

// CPackerMXF::DisconnectOutput
//
// Fail; this filter is an MXF sink only
//
HRESULT CPackerMXF::DisconnectOutput(CMXF *)
{
    return E_FAIL;
}

// CPackerMXF::SubmitIrp
//
void CPackerMXF::SubmitIrp(CIrp *irp)
{
    // Reject out of hand IRP's that would be too small. This avoids
    // having to wade through them during queue processing.
    //
    if (GetEventByteCount(irp->m_cbBuffer) < sizeof(DWORD))
    {
        irp->Complete(E_OUTOFMEMORY);
        return;
    }

    // Queue the IRP at the tail so they stay queued for a minimum of time
    // 
    irp->m_cbLeft = irp->m_cbBuffer;
    irp->m_ullPresTime100Ns = 0;
    irp->m_pNext = NULL;

    if (m_irpHead)
    {
        assert(m_irpTail);
        m_irpTail->m_pNext  = irp;
    }
    else
    {
        m_irpHead = irp;
    }
    m_irpTail = irp;

    if (m_evtHead)
    {
        // Waiting to send up some pending events
        //
        ProcessQueues();
    }
}

// CPackerMXF::PutMessage
//
// Call the appropriate translate function with a buffer to pack
//
HRESULT CPackerMXF::PutMessage(PDMUS_KERNEL_EVENT pevtHead)
{
    PDMUS_KERNEL_EVENT pevtTail;

    // If the sysex chain bit is set, then the next pointer points
    // to a hypothetical next buffer which hasn't been filled in yet.
    // In this case, just link this node at the tail of the queue.
    // Otherwise the next pointers could form a chain of events
    // to queue.
    //
    pevtTail = pevtHead;
    if (COMPLETE_EVT(pevtHead))
    {
        while (pevtTail->pNextEvt)
        {
            pevtTail = pevtTail->pNextEvt;
        }
    }

    pevtTail->pNextEvt = NULL;
    
    if (m_evtHead)
    {
        // Event queue not empty
        //
        assert(m_evtTail);

        m_evtTail->pNextEvt = pevtHead;
        m_evtTail = pevtHead;

        // Already waiting on an IRP to fill or queue would be empty, so 
        // don't bother trying to process
        //
    }
    else
    {
        // Event queue empty
        //
        m_evtHead   = pevtHead;
        m_evtTail   = pevtHead;
        m_evtOffset = 0;            

        ProcessQueues();
    }

    return S_OK;
}

// CPackerMXF::ProcessQueues
//
void CPackerMXF::ProcessQueues()
{
    ULONG cbEventBytes;
    ULONG cbToCopy;
    LPBYTE pbSource;
    LPBYTE pbDest;

    while (m_irpHead && m_evtHead)
    {
        CIrp *pirp = m_irpHead;

        cbEventBytes = GetEventByteCount(pirp->m_cbLeft);
        assert(cbEventBytes >= sizeof(DWORD));
        assert(m_evtOffset < m_evtHead->cbEvent);

        cbToCopy = m_evtHead->cbEvent - m_evtOffset;
        if (cbToCopy > cbEventBytes)
        {
            cbToCopy = cbEventBytes;
        }

        pbSource = &m_evtHead->uData.abData[0];
        if (m_evtHead->cbEvent > sizeof(PBYTE))
        {
            pbSource = m_evtHead->uData.pbData;
        }

        if (pirp->m_cbLeft == pirp->m_cbBuffer)
        {
            // Irp is empty, start a new buffer
            //
            StartBuffer(m_evtHead->ullPresTime100Ns);
            pirp->m_ullPresTime100Ns = m_evtHead->ullPresTime100Ns;
        }
        else if (m_evtHead->ullPresTime100Ns < m_ullLastTime)
        {
            // Event before we started this irp. This is illegal for KSMUSICFORMAT
            //
            // Flush it and start with the next one
            //
            CompleteHeadIrp(S_OK);
            continue;
        }

        m_ullLastTime = m_evtHead->ullPresTime100Ns;

        ULONG cbTotalEvent;
        pbDest = FillHeader(pirp->m_pbBuffer + pirp->m_cbBuffer - pirp->m_cbLeft, 
                            m_evtHead->ullPresTime100Ns, 
                            m_evtHead->ulChannelGroup, 
                            cbToCopy,
                            &cbTotalEvent);

        /*Rtl*/CopyMemory(pbDest, pbSource + m_evtOffset, cbToCopy);

        assert(cbTotalEvent <= pirp->m_cbLeft);
        pirp->m_cbLeft -= cbTotalEvent;

        if (GetEventByteCount(pirp->m_cbLeft) < sizeof(DWORD))
        {
            CompleteHeadIrp(S_OK);
        }
    
        m_evtOffset += cbToCopy;
        assert(m_evtOffset <= m_evtHead->cbEvent);

        if (m_evtOffset == m_evtHead->cbEvent)
        {
            PDMUS_KERNEL_EVENT pEvent = m_evtHead;
            m_evtHead = pEvent->pNextEvt;

            m_allocatorMXF->PutMessage(pEvent);
        }
    }
}

// CPackerMXF::CompleteHeadIrp 
//
HRESULT CPackerMXF::CompleteHeadIrp(HRESULT hr)
{
    CIrp *irp = m_irpHead;

    m_irpHead = irp->m_pNext;
    if (m_irpHead == NULL)
    {
        m_irpTail = NULL;
    }

    irp->Complete(hr);

    return S_OK;
}

///////////////////////////////////////////////////////////////////////
//
// CDMusPackerMXF
//
// Code to pack into DirectMusic buffer format
//

// CDMusPackerMXF::CDMusPackerMXF
//
CDMusPackerMXF::CDMusPackerMXF(CAllocatorMXF *allocatorMXF) 
    : CPackerMXF(allocatorMXF)
{
    m_ullBufferBaseTime = 0;
}

// CDMusPackerMXF::~CDMusPackerMXF
//
CDMusPackerMXF::~CDMusPackerMXF()
{
}

// CDMusPackerMXF::Translate
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


// CDMusPackerMXF::GetEventByteCount
//
ULONG CDMusPackerMXF::GetEventByteCount(ULONG ulBytesLeftInBuffer)
{
    ulBytesLeftInBuffer = QWORD_TRUNC(ulBytesLeftInBuffer);

    if (ulBytesLeftInBuffer < offsetof(DMEVENT, abEvent))
    {
        return 0;
    }

    return ulBytesLeftInBuffer - offsetof(DMEVENT, abEvent);
}

// CDMusPackerMXF::StartBuffer
//
void CDMusPackerMXF::StartBuffer(ULONGLONG ullPresentationTime)
{
    m_ullBufferBaseTime = ullPresentationTime;
}

// CDMusPackerMXF::FillHeader
//
LPBYTE CDMusPackerMXF::FillHeader(LPBYTE pbHeader, ULONGLONG ullPresentationTime, ULONG ulChannelGroup, ULONG cbEvent,
                                  PULONG pcbTotalEvent)
{
    DMEVENT *pEvent = (DMEVENT*)pbHeader;

    pEvent->cbEvent           = cbEvent;
    pEvent->dwChannelGroup    = ulChannelGroup;

    assert(ullPresentationTime >= m_ullBufferBaseTime);
    pEvent->rtDelta           = ullPresentationTime - m_ullBufferBaseTime;

    *pcbTotalEvent = offsetof(DMEVENT, abEvent) + QWORD_ALIGN(cbEvent);       

    return (LPBYTE)&pEvent->abEvent[0];
}

///////////////////////////////////////////////////////////////////////
//
// CKsPackerMXF
//
// Code to pack into KSMUSICFORMAT
//

// CKsPackerMXF::CKsPackerMXF
//
CKsPackerMXF::CKsPackerMXF(CAllocatorMXF *allocatorMXF) 
    : CPackerMXF(allocatorMXF)
{
    m_ullLastTime = 0;
}

// CKsPackerMXF::~CKsPackerMXF
//
CKsPackerMXF::~CKsPackerMXF()
{
}

// CKsPackerMXF::GetEventByteCount
//
ULONG CKsPackerMXF::GetEventByteCount(ULONG ulBytesLeftInBuffer)
{
    ulBytesLeftInBuffer = DWORD_TRUNC(ulBytesLeftInBuffer);

    if (ulBytesLeftInBuffer < sizeof(KSMUSICFORMAT))
    {
        return 0;
    }

    return ulBytesLeftInBuffer - sizeof(KSMUSICFORMAT);
}

// CKsPackerMXF::StartBuffer
//
void CKsPackerMXF::StartBuffer(ULONGLONG ullPresentationTime)
{
    m_ullLastTime = ullPresentationTime;
}

// CKsPackerMXF::FillHeader
//
LPBYTE CKsPackerMXF::FillHeader(LPBYTE pbHeader, ULONGLONG ullPresentationTime, ULONG ulChannelGroup, ULONG cbEvent,
                                PULONG pcbTotalEvent)
{
    assert(ulChannelGroup <= 1);

    KSMUSICFORMAT *pEvent = (KSMUSICFORMAT*)pbHeader;

    assert(ullPresentationTime >= m_ullLastTime);

    pEvent->TimeDeltaMs         = (DWORD)((ullPresentationTime - m_ullLastTime) / 10000);
    m_ullLastTime = ullPresentationTime;
    pEvent->ByteCount           = cbEvent;

    *pcbTotalEvent = sizeof(KSMUSICFORMAT) + DWORD_ALIGN(cbEvent);

    return (LPBYTE)(pEvent + 1);
}

