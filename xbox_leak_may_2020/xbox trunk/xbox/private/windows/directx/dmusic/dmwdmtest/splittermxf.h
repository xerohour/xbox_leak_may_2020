/*  Base definition of MIDI Transform Filter object

    05/10/98    Martin Puryear      Created this file

*/

#ifndef __SplitterMXF_H__
#define __SplitterMXF_H__

#include "MXF.h"
#include "AllocatorMXF.h"


#define kNumSinkMXFs    32

class CSplitterMXF : public CMXF
{
public:
    CSplitterMXF(CAllocatorMXF *allocatorMXF);
    ~CSplitterMXF(void);

    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);
    
private:
    PDMUS_KERNEL_EVENT  MakeDMKEvtCopy(PDMUS_KERNEL_EVENT pDMKEvt);

    CMXF           *m_sinkMXF[kNumSinkMXFs];
    DWORD           m_sinkMXFBitMap;
};

#endif  //  __SplitterMXF_H__
