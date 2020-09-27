/*  Base implementation of MIDI Transform Filter object

    05/06/98    Martin Puryear      Created this file

*/
#include "stdafx.h"
#include "SequencerMXF.h"

//  Must furnish an allocator.
//  Perhaps must furnish a clock as well?
CSequencerMXF::CSequencerMXF(CAllocatorMXF *allocatorMXF
                         //, CClockObject *clockObj
                             )
: CMXF(allocatorMXF)
{
    m_sinkMXF = NULL;
    m_evtQueue = NULL;
    //m_clock = clockObj;
}

//  Artfully remove this filter from the chain
CSequencerMXF::~CSequencerMXF(void)
{
    (void) DisconnectOutput(m_sinkMXF);

    if (m_evtQueue != NULL)
    {
        m_allocatorMXF->PutMessage(m_evtQueue);
    }
}

//  Furnish a sink.  The allocator is default (events are destroyed).
HRESULT CSequencerMXF::ConnectOutput(CMXF *sinkMXF)
{
    if ((m_sinkMXF != NULL) || (sinkMXF == NULL))
    {
        return S_FALSE;
    }

    m_sinkMXF = sinkMXF;
    return S_OK;
}

//  Pull the plug.  The sequencer should now put into the allocator.
HRESULT CSequencerMXF::DisconnectOutput(CMXF *sinkMXF)
{
    if ((m_sinkMXF == sinkMXF) || (sinkMXF == NULL))
    {
        m_sinkMXF = NULL;
        return S_OK;
    }
    return S_FALSE;
}

//  Receive event from above.  Insert in queue and check timer.
HRESULT CSequencerMXF::PutMessage(PDMUS_KERNEL_EVENT pDMKEvt)
{
    (void) InsertEvtIntoQueue(pDMKEvt);
    (void) ScheduleQueueEvents();
    
    return S_OK;
}

//  Maintain the sorted list, using only the forward 
//  links already in the DMUS)KERNEL_EVENT struct.
//
//  MGP-TODO
//  BUGBUG I haven't yet dealt with a) multi-part SysEx,
//  and b) passing in a chain of unsorted events.
HRESULT CSequencerMXF::InsertEvtIntoQueue(PDMUS_KERNEL_EVENT pDMKEvt)
{
    PDMUS_KERNEL_EVENT pEvt;
    
/*
    if (  (pDMKEvt->pNextEvt) 
       && (0 == (pDMKEvt->usFlags & DMUS_KEF_SYSEX_CONTINUED)))
    {
        pEvt = pDMKEvt->NextEvt;
        pDMKEvt->NextEvt = NULL;                        //  disconnect the first
        (void) InsertEvtIntoQueue(pDMKEvt);             //  queue it
        (void) InsertEvtIntoQueue(pDMKEvt->pNextEvt);   //  queue the rest
        return S_OK;
    }
*/
    if (!m_evtQueue)
    {
        m_evtQueue = pDMKEvt;
        return S_OK;
    }
    if (m_evtQueue->ullPresTime100Ns > pDMKEvt->ullPresTime100Ns)
    {
        pDMKEvt->pNextEvt = m_evtQueue;
        m_evtQueue = pDMKEvt;
        return S_OK;
    }

    //  go through each message in the queue looking at timestamps
    pEvt = m_evtQueue;
    while (pEvt->pNextEvt)
    {
        if (pEvt->pNextEvt->ullPresTime100Ns <= pDMKEvt->ullPresTime100Ns)
        {
            pEvt = pEvt->pNextEvt;
        }
        else
        {
            pDMKEvt->pNextEvt = pEvt->pNextEvt;
            pEvt->pNextEvt = pDMKEvt;
            return S_OK;
        }
    }
    pEvt->pNextEvt = pDMKEvt;
    return S_OK;
}

//  Get the current time and emit those messages whose time have come.
//  Make only one PutMessage with a sorted chain.
HRESULT CSequencerMXF::ScheduleQueueEvents(void)
{
    ULONGLONG   ullCurrentPresTime100ns;

//  MGP-TODO Get clock information
    //    ullCurrentPresTime100ns = GetCurrentTime100Ns();
    ullCurrentPresTime100ns = 0;

    //  if queue is not empty
    if (m_evtQueue)
    {
        if (m_evtQueue->ullPresTime100Ns <= ullCurrentPresTime100ns)
        //  if we are going to send down at least one event....
        {
            PDMUS_KERNEL_EVENT pEvt, pNewQueueHeadEvt;
            pEvt = m_evtQueue;
            //  figure out how many events are good to go
            while (pEvt->pNextEvt) 
            {
                if (pEvt->pNextEvt->ullPresTime100Ns <= ullCurrentPresTime100ns)
                {
                    pEvt = pEvt->pNextEvt;
                }
                else    
                    break;
            }
            //  send m_evtQueue, and set the new m_evtQueue to pEvt->pNextEvt
            //  so if the queue only had one item, m_evtQueue is set to NULL
            pNewQueueHeadEvt = pEvt->pNextEvt;
            pEvt->pNextEvt = NULL;
        
            if (m_sinkMXF)
            {
                m_sinkMXF->PutMessage(m_evtQueue);
            }
            else
            {
                m_allocatorMXF->PutMessage(m_evtQueue);
            }
            m_evtQueue = pNewQueueHeadEvt;
        }   //  if we sent at least one event
        
        //  MGP-TODO: if there's anything left in the queue, set up a timer DPC

    }   //  if queue is not empty
    return S_OK;
}
