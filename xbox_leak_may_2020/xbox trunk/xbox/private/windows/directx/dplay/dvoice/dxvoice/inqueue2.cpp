/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       inqueue2.cpp
 *  Content:
 *      
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 * 07/16/99     pnewson Created
 * 02/08/2000   georgioc redone from scratch to make queueing work with our dowork model/simplify it
 *
 ***************************************************************************/

#include "dvntos.h"
#include "in_core.h"
#include "inqueue2.h"

#include <math.h>

#define MODULE_ID   INPUTQUEUE2

#undef DPF_MODNAME
#define DPF_MODNAME "CInputQueue2::CInputQueue2"
CInputQueue2::CInputQueue2( )
        : m_dwSignature(VSIG_INPUTQUEUE2)
        , m_bCurMsgNum(0)
        , m_wQueueId(0)
        , m_dwTotalFrames(0)
        , m_dwTotalMessages(0)
        , m_dwTotalBadMessages(0)
        , m_dwDiscardedFrames(0)
        , m_dwDuplicateFrames(0)
        , m_dwLostFrames(0)
        , m_dwLateFrames(0)
        , m_dwOverflowFrames(0)
        , m_wMSPerFrame(0)
        , m_pFramePool(NULL)
{
    InitializeCriticalSection(&m_CS);
}

HRESULT CInputQueue2::Initialize( PQUEUE_PARAMS pParams )
{
    m_wQueueId = (USHORT)pParams->wQueueId;
    m_dwTotalFrames = 0;
    m_dwTotalMessages = 0;
    m_dwTotalBadMessages = 0;
    m_dwDiscardedFrames = 0;
    m_dwDuplicateFrames = 0;
    m_dwLostFrames = 0;
    m_dwLateFrames = 0;
    m_dwOverflowFrames = 0;
    m_wMSPerFrame = pParams->wMSPerFrame;
    m_pFramePool = pParams->pFramePool;

    InitializeListHead (&m_FrameQueue);

    DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** CInputQueue2::CInputQueue2() pFramePool: %p"), m_wQueueId, m_pFramePool);

    // set the queue to an empty state
    Reset();

    return DV_OK;
}

void CInputQueue2::GetStatistics( PQUEUE_STATISTICS pQueueStats )
{
    pQueueStats->dwTotalFrames = GetTotalFrames();
    pQueueStats->dwTotalMessages = GetTotalMessages();
    pQueueStats->dwTotalBadMessages = GetTotalBadMessages();
    pQueueStats->dwDiscardedFrames = GetDiscardedFrames();
    pQueueStats->dwDuplicateFrames = GetDuplicateFrames();
    pQueueStats->dwLostFrames = GetLostFrames();
    pQueueStats->dwLateFrames = GetLateFrames();
    pQueueStats->dwOverflowFrames = GetOverflowFrames();
}

void CInputQueue2::DeInitialize()
{
    PLIST_ENTRY pEntry;
    CFrame *pFrame;

    DV_AUTO_LOCK(&m_CS);

    pEntry = m_FrameQueue.Flink;
    while (pEntry != &m_FrameQueue) {
        
        pFrame = CONTAINING_RECORD(pEntry, CFrame, m_FramePoolEntry);
        RemoveEntryList(&pFrame->m_FramePoolEntry);
        pFrame->Return();
        pEntry = m_FrameQueue.Flink;
        
    }

}

// The destructor. Release all the resources we acquired in the
// constructor
#undef DPF_MODNAME
#define DPF_MODNAME "CInputQueue2::~CInputQueue2"
CInputQueue2::~CInputQueue2()
{
    DeInitialize();
    m_dwSignature = VSIG_INPUTQUEUE2_FREE;
}

//
// This function clears all the input buffers and 
// resets the other class information to an initial
// state. The queue should not be in use when this 
// function is called. i.e. there should not be any
// locked frames.
//
#undef DPF_MODNAME
#define DPF_MODNAME "CInputQueue2::Reset"
void CInputQueue2::Reset()
{
    PLIST_ENTRY pEntry;
    CFrame *pFrame;

    // make sure no one is using the queue right now
    DV_AUTO_LOCK(&m_CS);

    #if defined(DPVOICE_QUEUE_DEBUG)
    DPVF( DPVF_INFOLEVEL, ("** QUEUE ** %i ** CInputQueue2::Reset()"), m_wQueueId);
    #endif

    //
    // return all queued frames to the frame pool
    //

    pEntry = m_FrameQueue.Flink;
    while (pEntry != &m_FrameQueue) {
        
        pFrame = CONTAINING_RECORD(pEntry, CFrame, m_FramePoolEntry);
        RemoveEntryList(&pFrame->m_FramePoolEntry);
        pFrame->Return();
        pEntry = m_FrameQueue.Flink;
        
    }

    InitializeListHead(&m_FrameQueue);

    // we don't yet know the first message number, so just use zero
    m_bCurMsgNum = 0;


    // reset all the other stats too
    m_dwDiscardedFrames = 0;
    m_dwDuplicateFrames = 0;
    m_dwLateFrames = 0;
    m_dwLostFrames = 0;
    m_dwOverflowFrames = 0;
    m_dwQueueErrors = 0;
    m_dwTotalBadMessages = 0;
    m_dwTotalFrames = 0;
    m_dwTotalMessages = 0;

}

#undef DPF_MODNAME
#define DPF_MODNAME "CInputQueue2::Enqueue"
BOOLEAN CInputQueue2::Enqueue(const CFrame& fr)
{
    PLIST_ENTRY pEntry;
    CFrame *pFrame = NULL;

    DV_AUTO_LOCK(&m_CS);

    DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** ******************************************"), m_wQueueId);
    DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** CInputQueue2::Enqueue() MsgNum[%i] SeqNum[%i]"), m_wQueueId, fr.GetMsgNum(), fr.GetSeqNum());

    //
    // first get our tail frame.
    // if the frame we are trying to enqueue is out of order (older than whats in our tail),
    // drop new one on the floor
    //


    if (!IsListEmpty(&m_FrameQueue)) {

        pEntry = m_FrameQueue.Blink;
        pFrame = CONTAINING_RECORD(pEntry, CFrame, m_FramePoolEntry);
    
        if ((pFrame->GetMsgNum() > fr.GetMsgNum()) ||
            (pFrame->GetSeqNum() > fr.GetSeqNum()) ) {

            DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** CInputQueue2::Enqueue() MsgNum[%i] SeqNum[%i], droping out of order frame"), m_wQueueId, fr.GetMsgNum(), fr.GetSeqNum());

            m_dwLateFrames++;

            return FALSE;

        }

    } 

    //
    // Get a frame from the frame pool.
    // Then enqueue the frame at the tail of our queue
    //

    if ((pFrame = m_pFramePool->Get())) {

        pFrame->SetEqual(fr);

        InsertTailList(&m_FrameQueue,
                       &pFrame->m_FramePoolEntry);

        m_bCurMsgNum = fr.GetMsgNum();
        m_dwTotalFrames++;

    } else {

        DPVF( DPVF_ERRORLEVEL, ("** QUEUE ** %i ** CInputQueue2::Enqueue() MsgNum[%i] SeqNum[%i],failed to get frame"), m_wQueueId, fr.GetMsgNum(), fr.GetSeqNum());
        return FALSE;

    }

    return TRUE;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CInputQueue2::Dequeue"
CFrame* CInputQueue2::Dequeue()
{
    DV_AUTO_LOCK(&m_CS);
    CFrame* pfrReturn = 0;
    PLIST_ENTRY pEntry;

    DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** ******************************************"), m_wQueueId);
    DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** CInputQueue2::Dequeue()"), m_wQueueId);

    if (IsListEmpty(&m_FrameQueue)) {

        DPVF( DPVF_QUEUELEVEL, ("** QUEUE ** %i ** CInputQueue2::Dequeue() nothing available in inner queues, returning empty frame"), m_wQueueId);
    
        pfrReturn = m_pFramePool->Get();

        if (pfrReturn) {

            pfrReturn->SetIsSilence(TRUE);
            pfrReturn->SetIsLost(false);

        }

        return pfrReturn;

    }

    //
    // we got frames queued.. dequeue at the head so we get oldest frames out first (FIFO)
    //

    pEntry = RemoveHeadList(&m_FrameQueue);
    pfrReturn = CONTAINING_RECORD(pEntry, CFrame, m_FramePoolEntry);

    return pfrReturn;

}
        

