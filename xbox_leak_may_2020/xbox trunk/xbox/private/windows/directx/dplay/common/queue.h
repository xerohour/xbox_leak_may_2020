/*==========================================================================
 *
 *  Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Queue.h
 *  Content:	Queue manipulation functions.  This queue will not accept NULL
 *				as an argument because it is used to indicate an empty queue on
 *				Queue_DeQ.
 *
 *	Note:	This queue implementation requires the Fixed Pool manager found
 *			in FPM.h and FPM.c
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/15/98	jwo		Created it.
 *	04/16/99	jtk		Removed reliance on STL
 ***************************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef	struct	_QUEUE_ENTRY
{
	void			*pData;
	_QUEUE_ENTRY	*pNext;

} QUEUE_ENTRY;

typedef struct _QUEUE
{
	DNCRITICAL_SECTION	Lock;			// critical section
	HANDLE				hIsEmptyEvent;	// event signalled when queue is empty

	QUEUE_ENTRY		*pHead;		// pointer to head
	QUEUE_ENTRY		*pTail;		// pointer to tail

	FPOOL			Pool;		// pool of queue entries
} QUEUE;


//**********************************************************************
// Variable prototypes
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

HRESULT	Queue_New( QUEUE **ppQueue, const HANDLE hIsEmptyEvent );
HRESULT	Queue_Delete( QUEUE *const pQueue );

BOOL	Queue_Initialize( QUEUE *const pQueue, const HANDLE hIsEmptyEvent );
void	Queue_Deinitialize( QUEUE *const pQueue );

HRESULT	Queue_EnQ( QUEUE *const pQueue, void *const lpvItem );
void	*Queue_DeQ( QUEUE *const pQueue );
BOOL	Queue_IsEmpty( const QUEUE *const pQueue );

void	Queue_Lock( QUEUE *const pQueue );
void	Queue_Unlock( QUEUE *const pQueue );

#ifdef __cplusplus
}
#endif


#endif	// __QUEUE_H__
