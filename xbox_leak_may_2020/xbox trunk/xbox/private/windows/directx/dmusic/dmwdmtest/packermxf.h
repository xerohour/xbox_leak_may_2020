/*  Base definition of MIDI event packer

    05/22/98    Jim Geist           Created this file

*/

#ifndef __PackerMXF_H__
#define __PackerMXF_H__

#include "MXF.h"
#include "AllocatorMXF.h"


class CIrp
{
public:
    LPBYTE              m_pbBuffer;
    DWORD               m_cbBuffer;
    DWORD               m_cbLeft;
    ULONGLONG           m_ullPresTime100Ns;
    CIrp               *m_pNext;

    virtual void Notify(void) = 0;
    virtual void Complete(HRESULT hr) = 0;
};

class CPackerMXF : public CMXF
{
public:
    CPackerMXF(CAllocatorMXF *allocatorMXF);
    virtual ~CPackerMXF();

    // CMXF interface
    //
    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);

    // Upper edge interface
    //
    void SubmitIrp(CIrp *irp);

    // Subclass interface
    //
    virtual ULONG GetEventByteCount(ULONG ulBytesLeftInBuffer) = 0;
    virtual void  StartBuffer(ULONGLONG ullPresentationTime) = 0;
    virtual LPBYTE FillHeader(LPBYTE pbHeader, ULONGLONG ullPresentationTime, ULONG ulChannelGroup, ULONG cbEvent,
                              PULONG pcbTotalEvent) = 0;

private:
    CIrp                *m_irpHead;
    CIrp                *m_irpTail;
    PDMUS_KERNEL_EVENT  m_evtHead;
    PDMUS_KERNEL_EVENT  m_evtTail;
    ULONG               m_evtOffset;
    ULONGLONG           m_ullLastTime;

private:
    void ProcessQueues();
    HRESULT CompleteHeadIrp(HRESULT hr);
};

class CDMusPackerMXF : public CPackerMXF
{
public:
    CDMusPackerMXF(CAllocatorMXF *allocatorMXF);
    ~CDMusPackerMXF();

    ULONG GetEventByteCount(ULONG ulBytesLeftInBuffer);
    void  StartBuffer(ULONGLONG ullPresentationTime);
    LPBYTE FillHeader(LPBYTE pbHeader, ULONGLONG ullPresentationTime, ULONG ulChannelGroup, ULONG cbEvent, 
                      PULONG pcbTotalEvent);

private:
    ULONGLONG m_ullBufferBaseTime;
};

class CKsPackerMXF : public CPackerMXF
{
public:
    CKsPackerMXF(CAllocatorMXF *allocatorMXF);
    ~CKsPackerMXF();

    ULONG GetEventByteCount(ULONG ulBytesLeftInBuffer);
    void  StartBuffer(ULONGLONG ullPresentationTime);
    LPBYTE FillHeader(LPBYTE pbHeader, ULONGLONG ullPresentationTime, ULONG ulChannelGroup, ULONG cbEvent,
                      PULONG pcbTotalEvent);

private:
    ULONGLONG m_ullLastTime;
};

#endif // __PackerMXF_H__