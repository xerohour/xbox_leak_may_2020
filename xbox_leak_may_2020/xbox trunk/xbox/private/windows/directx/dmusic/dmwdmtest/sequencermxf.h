/*  Base definition of MIDI Transform Filter object 

    05/06/98    Martin Puryear      Created this file

*/

#ifndef __SequencerMXF_H__
#define __SequencerMXF_H__

#include "MXF.h"
#include "AllocatorMXF.h"


class CSequencerMXF : public CMXF
{
public:
    CSequencerMXF(CAllocatorMXF *allocatorMXF);    //  must provide a default sink/source
    ~CSequencerMXF(void);

    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);

protected:
    HRESULT InsertEvtIntoQueue(PDMUS_KERNEL_EVENT pDMKEvt);
    HRESULT ScheduleQueueEvents(void);

    CMXF                *m_sinkMXF;
    PDMUS_KERNEL_EVENT  m_evtQueue;
};

#endif  //  __SequencerMXF_H__
