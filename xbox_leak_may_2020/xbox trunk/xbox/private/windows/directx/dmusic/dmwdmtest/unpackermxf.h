/*  Base definition of MIDI event unpacker

    05/19/98    Jim Geist           Created this file

*/

#ifndef __UnpackerMXF_H__
#define __UnpackerMXF_H__

#include "MXF.h"
#include "AllocatorMXF.h"


class CUnpackerMXF : public CMXF
{
public:
    CUnpackerMXF(CAllocatorMXF *allocatorMXF);
    virtual ~CUnpackerMXF();

    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);

    // NOTE: All of these things will eventually be pulled out of the IRP
    //
    virtual HRESULT SinkIRP(LPBYTE bufferData, ULONG bufferSize, ULONGLONG ullBaseTime) = 0;

    // Common code for allocating and queueing an event
    //
    HRESULT QueueShortEvent(LPBYTE pbData, USHORT cbData, ULONGLONG ullPresTime, ULONG dwChannelGroup);
    HRESULT QueueSysEx(LPBYTE pbData, USHORT cbData, ULONGLONG ullPresTime, ULONG dwChannelGroup, BOOL fIsContinued);

    inline DWORD PageSize() const { return m_pageSize; }

    HRESULT UnpackEventBytes(ULONGLONG ullCurrenTime, ULONG ulChannelGroup, LPBYTE pbData, ULONG cbData);

private:
    CMXF                   *m_sinkMXF;
    DWORD                   m_pageSize;
    PDMUS_KERNEL_EVENT      m_nextSysExEvent;

    ULONGLONG               m_ullEventTime;
    BYTE                    m_bRunningStatus;

    enum
    {
        stateNone,
        stateInShortMsg,
        stateInSysEx
    }                       m_state;
    ULONG                   m_cbShortMsgLeft;
    BYTE                    m_abShortMsg[4];
    LPBYTE                  m_pbShortMsg;
};

class CDMusUnpackerMXF : public CUnpackerMXF
{
public:
    CDMusUnpackerMXF(CAllocatorMXF *allocatorMXF);
    ~CDMusUnpackerMXF();

    // NOTE: All of these things will eventually be pulled out of the IRP
    //
    HRESULT SinkIRP(LPBYTE bufferData, ULONG bufferSize, ULONGLONG ullBaseTime);
};

class CKsUnpackerMXF : public CUnpackerMXF
{
public:
    CKsUnpackerMXF(CAllocatorMXF *allocatorMXF);
    ~CKsUnpackerMXF();

    // NOTE: All of these things will eventually be pulled out of the IRP
    //
    HRESULT SinkIRP(LPBYTE bufferData, ULONG bufferSize, ULONGLONG ullBaseTime);
};
#endif // __UnpackerMXF_H__