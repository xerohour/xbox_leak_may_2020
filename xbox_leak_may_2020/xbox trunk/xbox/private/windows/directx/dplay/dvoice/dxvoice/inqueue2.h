/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		inqueue2.h
 *  Content:	Definition of the CInputQueue2 class
 *		
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/16/99		pnewson	Created
 * 02/05/2001   georgioc rewrote it , replacing queuing scheme with very simple version
 *
 ***************************************************************************/

#ifndef _INPUTQUEUE2_H_
#define _INPUTQUEUE2_H_

#include "Frame.h"

typedef struct _QUEUE_PARAMS
{
    WORD wFrameSize;
	BYTE bInnerQueueSize;
	DWORD wQueueId;
    CFramePool* pFramePool;
	WORD wMSPerFrame;
    	
} QUEUE_PARAMS, *PQUEUE_PARAMS;

typedef struct _QUEUE_STATISTICS
{
    DWORD dwTotalFrames;
    DWORD dwTotalMessages;
    DWORD dwTotalBadMessages;
    DWORD dwDiscardedFrames;
    DWORD dwDuplicateFrames;
    DWORD dwLostFrames;
    DWORD dwLateFrames;
    DWORD dwOverflowFrames;
} QUEUE_STATISTICS, *PQUEUE_STATISTICS;

// This class manages a queue of frames. It is designed
// to allow a client class to remove frames from the queue
// at regular intervals, and to hide any out of order
// frame reception, or dropped frames from the caller.
// If for whatever reason there is no frame available
// to give a client, this class will still provide a
// frame marked as silent.  This allows the client to
// simply call the dequeue function once per period, and
// consume the data at the agreed rate.  So for example,
// the client to this class could be a thread which
// is consuming input data and passing it to DirectSound
// for playback. It can simply get a frame every 1/10 of
// a second (or however long a frame is), and play it.
//
// This is the second generation of input queue. It 
// manages a set of inner queues, each of which is used
// for a "message". The stream of speech is divided into
// a series of messages, using silence as the divider.
// This class will not function well if the audio stream
// is not divided into separate messages.
//
#define VSIG_INPUTQUEUE2		'QNIV'
#define VSIG_INPUTQUEUE2_FREE	'QNI_'
//
volatile class CInputQueue2
{
private:

    CRITICAL_SECTION m_CS;
	DWORD m_dwSignature; // Debug signature

	// The message number currently at the head of the queue
	BYTE m_bCurMsgNum;
	WORD m_wMSPerFrame;

	// Some statistics to track.
	DWORD m_dwTotalFrames;
	DWORD m_dwTotalMessages;
	DWORD m_dwTotalBadMessages;
	DWORD m_dwDiscardedFrames;
	DWORD m_dwDuplicateFrames;
	DWORD m_dwLostFrames;
	DWORD m_dwLateFrames;
	DWORD m_dwOverflowFrames;
	DWORD m_dwQueueErrors;

	// An abritrary queue ID, provided to the constructor, 
	// used to identify which queue an instrumentation message
	// is coming from. It serves no other purpose, and can be
	// ignored except for debug purposes.
	WORD m_wQueueId;

	// the frame pool to manage the frames so we don't have to
	// allocate a huge number of them when only a few are 
	// actually in use.
	CFramePool* m_pFramePool;

    //
    // our doubly linked list representing the frame FIFO
    //

    LIST_ENTRY m_FrameQueue;

public:

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'lcvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

	// The constructor. 
	CInputQueue2();
    
    HRESULT Initialize( PQUEUE_PARAMS pQueueParams );
    void DeInitialize();

    void GetStatistics( PQUEUE_STATISTICS pStats );

	// The destructor. Release all the resources we acquired in the
	// constructor
	~CInputQueue2();

	// This function clears all buffers and resets the other class 
	// information to an initial state. DO NOT CALL THIS FUNCTION
	// IF THE QUEUE IS IN USE! i.e. do not call it if you have
	// not called Return() on every frame that you have
	// taken from this queue.
	void Reset();

	// Call this function to add a frame to the queue.  I 
	// considered returning a reference to a frame which 
	// the caller could then stuff, but because the frames
	// will not always arrive in order, that would mean I would have
	// to copy the frame sometimes anyway.  So, for simplicity, the
	// caller has allocated a frame, which it passes a reference
	// to, and this function will copy that frame into the
	// appropriate place in the queue, according to its
	// message number and sequence number.
	BOOLEAN Enqueue(const CFrame& fr);

	// This function retrieves the next frame from the head of
	// the queue. For speed, it does not copy the data out of the
	// buffer, but instead returns a pointer to the actual
	// frame from the queue. Of course, there is the danger
	// that the CInputQueue2 object which returns a reference to the
	// frame may try to reuse that frame before the caller is 
	// finished with it. The CFrame's lock and unlock member functions
	// are used to ensure this does not happen.  When the caller
	// is finished with the CFrame object, it should call vUnlock()
	// on it. If the caller doesn't unlock the frame, bad things
	// will happen when the input queue tries lock it again when 
	// it wants to reuse that frame. In any case, the caller
	// should always unlock the returned frame before it attempts
	// to dequeue another frame.
	CFrame* Dequeue();

	// get stats
	DWORD GetDiscardedFrames() { return m_dwDiscardedFrames; }
	DWORD GetDuplicateFrames() { return m_dwDuplicateFrames; }
	DWORD GetLateFrames() { return m_dwLateFrames; }
	DWORD GetLostFrames() { return m_dwLostFrames; }
	DWORD GetOverflowFrames() { return m_dwOverflowFrames; }
	DWORD GetQueueErrors() { return m_dwQueueErrors; }
	DWORD GetTotalBadMessages() { return m_dwTotalBadMessages; }
	DWORD GetTotalFrames() { return m_dwTotalFrames; }
	DWORD GetTotalMessages() { return m_dwTotalMessages; }

};

#endif
