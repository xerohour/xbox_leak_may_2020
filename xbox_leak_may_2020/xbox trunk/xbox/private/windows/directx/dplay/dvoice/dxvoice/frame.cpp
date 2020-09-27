/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		wirecd.cpp
 *  Content:
 *		
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/16/99		pnewson	Created
 * 08/03/99		pnewson General clean up, updated target to DVID
 * 01/14/2000	rodtoll	Updated to support multiple targets.  Frame will 
 *						automatically allocate memory as needed for targets.
 *				rodtoll	Added SetEqual function to making copying of frame
 *						in Queue easier. 
 *				rodtoll	Added support for "user controlled memory" frames.
 *						When the default constructor is used with the UserOwn_XXXX
 *						functions the frames use user specified buffers.  
 *						(Removes a buffer copy when queueing data). 
 *  01/31/2000	pnewson replace SAssert with DNASSERT
 *  02/17/2000	rodtoll	Updated so sequence/msg numbers are copied when you SetEqual
 *  07/09/2000	rodtoll	Added signature bytes 
 *
 ***************************************************************************/

#include "dvntos.h"
#include "in_core.h"

#include "frame.h"


#undef DPF_MODNAME
#define DPF_MODNAME "CFrame"

// SetEqual        
//
// This function sets the current frame to match the data in frSourceFrame
//
HRESULT CFrame::SetEqual( const CFrame &frSourceFrame )
{
	HRESULT hr;
	
	SetClientId( frSourceFrame.GetClientId());
	SetSeqNum(frSourceFrame.GetSeqNum());
	SetMsgNum(frSourceFrame.GetMsgNum());
	CopyData(frSourceFrame);
	SetIsSilence(frSourceFrame.GetIsSilence());

	hr = SetTargets( frSourceFrame.GetTargetList(), frSourceFrame.GetNumTargets() );

	if( FAILED( hr ) )
	{
		DPVF( DPVF_ERRORLEVEL, "Error copying frame for queue" );
	}

	return hr;
}

// GetTargets
//
// This program gets the targets for this frame
HRESULT CFrame::GetTargets( PDVID pdvidTargets, PDWORD pdwNumTargets ) const
{
	ASSERT( pdwNumTargets != NULL );
		
	if( pdwNumTargets != NULL )
	{
		DPVF( DPVF_ERRORLEVEL, "Invalid param" );
		return DVERR_INVALIDPARAM;
	}

	if( *pdwNumTargets < m_dwNumTargets || pdvidTargets == NULL )
	{
		*pdwNumTargets = m_dwNumTargets;
		return DVERR_BUFFERTOOSMALL;
	}

	*pdwNumTargets = m_dwNumTargets;

	memcpy( pdvidTargets, m_pdvidTargets, sizeof(DVID)*m_dwNumTargets );

	return DV_OK;
}

// SetTargets
//
// This program sets the targets for this frame.  It will expand the 
// target list (if required) or use a subset of the current buffer.
//
HRESULT CFrame::SetTargets( PDVID pdvidTargets, DWORD dwNumTargets )
{
	ASSERT( m_fOwned );
	
	if( dwNumTargets > m_dwMaxTargets )
	{
        //
        // we should never be here. The session must never exceed
        // the pre-allocated max target size in dvprot.h
        //
        ASSERT(FALSE);
		if( m_pdvidTargets != NULL )
		{
			delete [] m_pdvidTargets;
		}

		m_pdvidTargets = (DVID *)DV_POOL_ALLOC(sizeof(DVID)*dwNumTargets);

		if( m_pdvidTargets == NULL )
		{
			DPVF( DPVF_ERRORLEVEL, "Memory allocation failure" );
			return DVERR_OUTOFMEMORY;
		}
		
		m_dwMaxTargets = dwNumTargets;
	}

	m_dwNumTargets = dwNumTargets;

	memcpy( m_pdvidTargets, pdvidTargets, sizeof(DVID)*dwNumTargets );

	return DV_OK;
}

// This function is called to return a frame to the frame
// pool that is managing it. If a primary pointer was 
// provided, it will be set to NULL.
void CFrame::Return()
{
    KIRQL irql;

    RIRQL(irql);

	// this frame is supposed to be part of a frame pool if
	// this function is called
	ASSERT(m_pFramePool != NULL);

	// return the frame to the pool, and set the primary
	// frame pointer to null to signal to the caller that
	// this frame is now gone. Note that this pointer update
	// is done within the critical section passed to this
	// class, and so the caller should also use this 
	// critical section to check the pointer value. This
	// is true for CInputQueue, which uses the critical
	// section for Reset, Enqueue and Dequeue.
	m_pFramePool->Return(this);

    LIRQL(irql);
}

// CFrame Constructor
//
// This is the primary constructor which is used for creating frames
// that are used by the frame pool.
//
// If you want to create a non-pooled frame then use the default constructor
//
CFrame::CFrame(WORD wFrameSize, 
	WORD wClientNum,
	BYTE wSeqNum,
    BYTE bMsgNum,
	BYTE bIsSilence,
	CFramePool* pFramePool)
	: m_dwSignature(VSIG_FRAME),
	m_wFrameSize(wFrameSize),
	m_wClientId(wClientNum),
	m_wSeqNum(wSeqNum),
	m_bMsgNum(bMsgNum),
	m_bIsSilence(bIsSilence),
    m_wFrameLength(wFrameSize),
	m_pFramePool(pFramePool),
	m_fIsLost(false),
	m_pdvidTargets(NULL),
	m_dwNumTargets(0),
	m_dwMaxTargets(0),
	m_fOwned(true)
{
	m_pbData = (PBYTE)DV_POOL_ALLOC(m_wFrameSize);
}

// CFrame Constructor
//
// This is the constructor to use when creating a standalone frame.  This 
// type of frame can take an external buffer to eliminate a buffer copy.
//
// The frame doesn't "own" the buffer memory so it doesn't attempt to 
// free it.
//
// To set the data for the frame use the UserOwn_SetData member.
//
// Target information can be handled the same way by using UserOwn_SetTargets 
//
CFrame::CFrame(
	): 	m_dwSignature(VSIG_FRAME),
		m_wFrameSize(0),
		m_wClientId(0),
		m_wSeqNum(0),
		m_bMsgNum(0),
		m_bIsSilence(true),
	    m_wFrameLength(0),
		m_pFramePool(NULL),
		m_fIsLost(false),
		m_pdvidTargets(NULL),
		m_dwNumTargets(0),
		m_dwMaxTargets(0),
		m_fOwned(false)
{
}

CFrame::~CFrame() 
{	
	if( m_fOwned )
	{
        if (m_pbData) {

            DV_POOL_FREE(m_pbData); 

        }
		
        m_pbData = NULL;

		if( m_pdvidTargets != NULL )
		{
			DV_POOL_FREE(m_pdvidTargets);
		}

        m_pdvidTargets = NULL;
	}


	m_dwSignature = VSIG_FRAME_FREE;
}

void CFrame::CopyData(const BYTE* pbData, WORD wFrameLength)
{
	ASSERT(pbData != 0);
	memcpy(m_pbData, pbData, wFrameLength);
    m_wFrameLength = wFrameLength;
}

CFramePool::CFramePool(WORD wFrameSize)
	: m_wFrameSize(wFrameSize)
{
    ULONG i;
    CFrame * pFrame;

	#if defined(DEBUG) || defined(DBG)
	m_lCount = 0;
	#endif

    InitializeListHead(&m_FramePool);

    //
	// Push a few frames into the pool to start with
    // this must tuned to be enough for the lifetime of the session
    //

    for (i=0;i<FRAME_POOL_MAX_FRAMES;i++) {

        pFrame = new CFrame(m_wFrameSize);
        InsertHeadList(&m_FramePool,&pFrame->m_FramePoolEntry);
    }
	

	return;
}

CFramePool::~CFramePool()
{
    CFrame *pFrame;
    PLIST_ENTRY pEntry;

    pEntry = m_FramePool.Flink;
    while (pEntry != &m_FramePool) {

        pFrame = CONTAINING_RECORD(pEntry, CFrame, m_FramePoolEntry);
        pEntry = pFrame->m_FramePoolEntry.Flink;

        delete pFrame;

    }

}

CFrame* CFramePool::Get()
{

    KIRQL irql;
    RIRQL(irql);

    ASSERT( m_lCount < 300 );

	CFrame* pfr;
	if (IsListEmpty(&m_FramePool) )
	{
        ASSERT(FALSE);
        LIRQL(irql);
		return NULL;
		
	}
	else
	{
        PLIST_ENTRY pEntry = RemoveTailList(&m_FramePool);
        pfr = CONTAINING_RECORD(pEntry,CFrame,m_FramePoolEntry);
	}

	pfr->SetFramePool(this);

	// clear up the rest of the flags, but don't bother messing
	// with the data.
	pfr->SetIsLost(false);
	pfr->SetMsgNum(0);
	pfr->SetSeqNum(0);
	pfr->SetIsSilence(FALSE);

	#ifdef DEBUG
    m_lCount++;
    #endif

    LIRQL(irql);
	return pfr;
}

void CFramePool::Return(CFrame* pFrame)
{

    KIRQL irql;
    RIRQL(irql);
    ASSERT( m_lCount < 300 );

	// drop this frame on the back for reuse
	InsertHeadList(&m_FramePool,&pFrame->m_FramePoolEntry);

	#ifdef DEBUG
    m_lCount--;
    #endif
    LIRQL(irql);
}


