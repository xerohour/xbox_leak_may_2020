/*  Base implementation of MIDI Transform Filter object 

    05/10/98    Martin Puryear      Created this file

*/

#include "stdafx.h"
#include "SplitterMXF.h"

CSplitterMXF::CSplitterMXF(CAllocatorMXF *allocatorMXF)
: CMXF(allocatorMXF)
{
    short count;

    m_sinkMXFBitMap = 0;
    for (count = 0;count < kNumSinkMXFs;count++)
    {
        m_sinkMXF[count] = NULL;
    }
}

/*  Artfully remove this filter from the chain  */
CSplitterMXF::~CSplitterMXF(void)
{
}

HRESULT CSplitterMXF::ConnectOutput(CMXF *sinkMXF)
{
    DWORD   bitmap;
    short   count;

    if (m_sinkMXFBitMap == 0xFFFFFFFF)
        return S_FALSE;
    
    bitmap = m_sinkMXFBitMap;
    count = 0;
    
    for (count = 0;count < kNumSinkMXFs;count++)
    {
        if ((bitmap & 1) == 0)
        {
            break;
        }
        bitmap >>= 1;
    }    
    m_sinkMXF[count] = sinkMXF;
    m_sinkMXFBitMap |= (1 << count);
    
    return S_OK;
}

HRESULT CSplitterMXF::DisconnectOutput(CMXF *sinkMXF)
{
    DWORD bitmap;
    
    if (m_sinkMXFBitMap == 0)
    {
        return S_FALSE;
    }
    
    bitmap = m_sinkMXFBitMap;
    for (short count = 0;count < kNumSinkMXFs;count++)
    {
        if ((bitmap & 1) && (m_sinkMXF[count] == sinkMXF))
        {
            m_sinkMXF[count] = NULL;
            m_sinkMXFBitMap &= ~(1 << count);
        }
        bitmap >>= 1;
    }
    return S_OK;
}

HRESULT CSplitterMXF::PutMessage(PDMUS_KERNEL_EVENT pDMKEvt)
{
    DWORD               bitmap;
    PDMUS_KERNEL_EVENT  pDMKEvt2;

    if (m_sinkMXFBitMap)
    {
        bitmap = m_sinkMXFBitMap;
        for (short count = 0;count < kNumSinkMXFs;count++)
        {
            if (bitmap & 1)
            {
                pDMKEvt2 = MakeDMKEvtCopy(pDMKEvt);
                if (pDMKEvt2 != NULL)
                {
                    m_sinkMXF[count]->PutMessage(pDMKEvt2);
                }
            }
            bitmap >>= 1;
        }
    }
    m_allocatorMXF->PutMessage(pDMKEvt);

    return S_OK;
}

PDMUS_KERNEL_EVENT CSplitterMXF::MakeDMKEvtCopy(PDMUS_KERNEL_EVENT pDMKEvt)
{
    PDMUS_KERNEL_EVENT  pDMKEvt2;

    if (m_allocatorMXF != NULL)
    {
        m_allocatorMXF->GetMessage(&pDMKEvt2);
        if (pDMKEvt2 != NULL)
        {
            (void) memcpy(pDMKEvt2,pDMKEvt, sizeof(DMUS_KERNEL_EVENT));
            if (pDMKEvt->cbEvent > sizeof(PBYTE))
            {
//
//BUGBUG!!  This is a no-no at DPC time.  Do it with a pre-allocated pool.
//
//              pDMKEvt = (PDMUS_KERNEL_EVENT)ExAllocatePoolWithTag(
//                                    NonPagedPool,
//                                    pDMKEvt->cbEvent,
//                                    ' FXM');    //  'MXF '
                pDMKEvt2->uData.pbData = (unsigned char *)malloc(pDMKEvt->cbEvent);

                if (pDMKEvt2->uData.pbData)
                {
                    (void) memcpy(  pDMKEvt2->uData.pbData,
                                    pDMKEvt->uData.pbData,
                                    pDMKEvt->cbEvent);
                }
                else
                {
                    m_allocatorMXF->PutMessage(pDMKEvt2);
                    pDMKEvt2 = NULL;
                }
            }
            if (pDMKEvt->pNextEvt != NULL)
            {
                pDMKEvt2->pNextEvt = MakeDMKEvtCopy(pDMKEvt->pNextEvt);
            }
        }
    }
    else
    {
        pDMKEvt2 = NULL;
    }
    return pDMKEvt2;
}
