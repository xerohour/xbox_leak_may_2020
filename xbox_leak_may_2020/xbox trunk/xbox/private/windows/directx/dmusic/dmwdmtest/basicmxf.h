/*  Base definition of MIDI Transform Filter object 

    05/06/98    Martin Puryear      Created this file

*/

#ifndef __BasicMXF_H__
#define __BasicMXF_H__

#include "MXF.h"
#include "AllocatorMXF.h"


class CBasicMXF : public CMXF
{
public:
    CBasicMXF(CAllocatorMXF *allocatorMXF);    //  must provide a default sink/source
    ~CBasicMXF(void);

    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);

protected:
    HRESULT UnrollAndProcess(PDMUS_KERNEL_EVENT pDMKEvt);
    HRESULT DoProcessing(PDMUS_KERNEL_EVENT pDMKEvt);

protected:
    CMXF          *m_sinkMXF;
};

#endif  //  __BasicMXF_H__
