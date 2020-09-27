/*  Base definition of MIDI Transform Filter object 

    05/06/98    Martin Puryear      Created this file

*/

#ifndef __MXF_H__
#define __MXF_H__

//#include "wdm.h"

typedef unsigned __int64 ULONGLONG;
typedef struct _DMUS_KERNEL_EVENT
{
//    LIST_ENTRY  listEntry;
    USHORT      cbEvent;
    USHORT      usFlags;
    ULONG       ulChannelGroup;
    ULONGLONG   ullPresTime100Ns;
    _DMUS_KERNEL_EVENT *pNextEvt;
    union
    {
        BYTE    abData[sizeof(PBYTE)];
        PBYTE   pbData;
        _DMUS_KERNEL_EVENT *pPackageEvt;
    } uData;
} DMUS_KERNEL_EVENT, *PDMUS_KERNEL_EVENT;

//------------------------------------------

#define DMUS_KEF_PACKAGE_EVENT          0x0001  //  This event is a package. The uData.pPackageEvt
                                                //  field contains a pointer to a chain of events.
#define DMUS_KEF_EVENT_INCOMPLETE       0x0002  //  This event is an incomplete package or sysex. 
                                                //  Do not use this data.
#define INCOMPLETE_EVT(evt) ((evt)->usFlags & DMUS_KEF_EVENT_INCOMPLETE)
#define COMPLETE_EVT(evt)   ((evt)->usFlags & DMUS_KEF_EVENT_INCOMPLETE == 0)
#define PACKAGE_EVT(evt)    ((evt)->usFlags & DMUS_KEF_PACKAGE_EVENT)

#pragma warning (disable : 4100)

class CAllocatorMXF;

class CMXF
{
public:
    CMXF(CAllocatorMXF *allocatorMXF) { m_allocatorMXF = allocatorMXF;};
    virtual ~CMXF(void) {};

    virtual HRESULT ConnectOutput(CMXF *sinkMXF) = 0;
    virtual HRESULT DisconnectOutput(CMXF *sinkMXF) = 0;

    virtual HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt) = 0;
protected:
    CAllocatorMXF *m_allocatorMXF;
};

#pragma warning (default : 4100)
#endif  //  __MXF_H__
