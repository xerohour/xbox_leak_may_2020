/*  Base definition of MIDI Transform Filter object 

    05/08/98    Martin Puryear      Created this file

*/

#ifndef __AllocatorMXF_H__
#define __AllocatorMXF_H__

#include "MXF.h"


const int kNumMsgsInFreePool = 50;

class CAllocatorMXF : public CMXF
{
public:
    CAllocatorMXF(void);
    ~CAllocatorMXF(void);

    HRESULT GetMessage(PDMUS_KERNEL_EVENT *pDMKEvt);
    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);
    
    virtual HRESULT ConnectOutput(CMXF *)
    {   return S_FALSE;    };
    virtual HRESULT DisconnectOutput(CMXF *) 
    {   return S_FALSE;    };
    
private:
    short               m_numFreeMessages;
    PDMUS_KERNEL_EVENT  m_pFreeList;
};


#endif  //  __AllocatorMXF_H__
