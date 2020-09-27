/*  Base implementation of MIDI Transform Filter object

    05/06/98    Martin Puryear      Created this file

*/
#include "stdafx.h"
#include "BasicMXF.h"

CBasicMXF::CBasicMXF(CAllocatorMXF *allocatorMXF)
: CMXF(allocatorMXF)
{
    m_sinkMXF = NULL;
}

/*  Artfully remove this filter from the chain  */
CBasicMXF::~CBasicMXF(void)
{
    (void) DisconnectOutput(m_sinkMXF);
}

HRESULT CBasicMXF::ConnectOutput(CMXF *sinkMXF)
{
    if ((m_sinkMXF != NULL) || (sinkMXF == NULL))
    {
        return S_FALSE;
    }

    m_sinkMXF = sinkMXF;
    return S_OK;
}

HRESULT CBasicMXF::DisconnectOutput(CMXF *sinkMXF)
{
    if ((m_sinkMXF == sinkMXF) || (sinkMXF == NULL))
    {
        m_sinkMXF = NULL;
        return S_OK;
    }
    return S_FALSE;
}

//  Process and forward this message to the next filter in the chain.
HRESULT CBasicMXF::PutMessage(PDMUS_KERNEL_EVENT pDMKEvt)
{
    if (m_sinkMXF)
    {
        (void) UnrollAndProcess(pDMKEvt);
        m_sinkMXF->PutMessage(pDMKEvt);
    }
    else
    {
        m_allocatorMXF->PutMessage(pDMKEvt);
    }
    return S_OK;
}

//  example --  transposes up by one semitone
HRESULT CBasicMXF::UnrollAndProcess(PDMUS_KERNEL_EVENT pDMKEvt)
{
    if (COMPLETE_EVT(pDMKEvt))
    {
        if (pDMKEvt->cbEvent <= sizeof(PBYTE))  //  short message
        {
            (void) DoProcessing(pDMKEvt);
        }
        else if (PACKAGE_EVT(pDMKEvt))          //  deal with packages
        {
            (void) UnrollAndProcess(pDMKEvt->uData.pPackageEvt);
        }
    }
    if (pDMKEvt->pNextEvt)                      //  deal with successors
    {
        (void) UnrollAndProcess(pDMKEvt->pNextEvt);
    }
    return S_OK;
}

HRESULT CBasicMXF::DoProcessing(PDMUS_KERNEL_EVENT pDMKEvt)
{
    if (  (pDMKEvt->uData.abData[0] & 0xE0 == 0x80)     //  if NoteOn/NoteOff
       || (pDMKEvt->uData.abData[0] & 0xF0 == 0xA0))    //  if After-Pressure
    {
        (pDMKEvt->uData.abData[1])++;                   //  increment the noteNum
        (pDMKEvt->uData.abData[1]) &= 0x7F;             //  rollover to 0 if too high
    }
    return S_OK;
}