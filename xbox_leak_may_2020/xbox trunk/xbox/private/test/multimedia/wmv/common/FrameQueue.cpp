/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	FrameQueue.cpp

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 04-Sep-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	04-Sep-2001 jeffsul
		Initial Version

--*/

#include "FrameQueue.h"
#include <xdbg.h>
#include <macros.h>


CFrameQueue::CFrameQueue()
{ 
	m_pHead = m_pTail = NULL; 
}

BOOL 
CFrameQueue::IsEmpty()
{ 
	return m_pHead == NULL; 
}

//-----------------------------------------------------------------------------
// Name: FrameQueue::Enqueue
// Desc: Adds a frame to tail of queue
//-----------------------------------------------------------------------------
VOID
CFrameQueue::Enqueue( VIDEOFRAME * pFrame )
{ 
    pFrame->pNext = NULL;

    if( m_pTail ) 
    {
        m_pTail->pNext = pFrame;
        m_pTail = pFrame;
    }
    else
        m_pHead = m_pTail = pFrame;
}



//-----------------------------------------------------------------------------
// Name: FrameQueue::Dequeue
// Desc: Returns the head of the queue
//-----------------------------------------------------------------------------
VIDEOFRAME *
CFrameQueue::Dequeue()
{
    ASSERT( m_pHead );
    VIDEOFRAME * pPacket = m_pHead;

    m_pHead = m_pHead->pNext;
    if( !m_pHead )
        m_pTail = NULL;

    pPacket->pNext = NULL;
    return pPacket;
}

/*
VOID
CFrameQueue::Destroy()
{
	ASSERT( m_pHead );
	VIDEOFRAME* pPacket = m_pHead;
	
	while ( pPacket )
	{
		VIDEOFRAME* pNext = pPacket->pNext;
		SAFEDELETE( pPacket );
		pPacket = pNext;
    }

	m_pHead = NULL;
}*/