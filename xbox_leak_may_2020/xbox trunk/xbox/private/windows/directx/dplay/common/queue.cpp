/*==========================================================================
 *
 *  Copyright (C) 1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       cq.cpp
 *  Content:	Implementation of a queue
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/19/98	jwo		Created it.
 *	04/16/99	jtk		Removed reliance on STL
***************************************************************************/

#include "commoni.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable prototypes
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_New - creates new queue
//
// Entry:		Pointer to pointer to queue to be created
//				Handle of event signalled when queue is empty
//
// Exit:		Error Code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_New"

HRESULT	Queue_New( QUEUE **ppQueue, const HANDLE hIsEmptyEvent )
{
	HRESULT	hr;


	DNASSERT( ppQueue != NULL );

	// initialize
	hr = DPN_OK;
	*ppQueue = static_cast<QUEUE*>( DNMalloc( sizeof( QUEUE ) ) );
	if ( *ppQueue == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		goto Exit;
	}

	memset( *ppQueue, 0x00, sizeof( **ppQueue ) );

	if ( Queue_Initialize( *ppQueue, hIsEmptyEvent ) == FALSE )
	{
		DPF( 0, "Failed to initialize queue!" );
		DNFree( *ppQueue );
		*ppQueue = NULL;
		hr = DPNERR_OUTOFMEMORY;
		goto Exit;
	}

Exit:
	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_Delete - deletes a queue
//
// Entry:		Pointer to the queue to initialize
//				Pointer to initialization data
//
// Exit:		Error Code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_Delete"

HRESULT	Queue_Delete( QUEUE *const pQueue )
{
	HRESULT	hr;


	DNASSERT( pQueue != NULL );
	DNASSERT( Queue_IsEmpty( pQueue ) != FALSE );

	// initialize
	hr = DPN_OK;

	Queue_Deinitialize( pQueue );
	DNFree( pQueue );

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_Initialize - intialize a queue
//
// Entry:		Pointer to queue
//				Handle of event signalled when the queue is empty
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failure
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_Initialize"

BOOL	Queue_Initialize( QUEUE *const pQueue, const HANDLE hIsEmptyEvent )
{
	BOOL	fReturn;
	BOOL	fCriticalSectionInitialized;


	DNASSERT( pQueue != NULL );
	fReturn = TRUE;
	fCriticalSectionInitialized = FALSE;

	//
	// set internals
	//
	if ( DNInitializeCriticalSection( &pQueue->Lock ) == FALSE )
	{
		fReturn = FALSE;
		goto Failure;
	}
	DebugSetCriticalSectionRecursionCount( &pQueue->Lock, 0 );

	pQueue->hIsEmptyEvent = hIsEmptyEvent;
	pQueue->pHead = NULL;
	pQueue->pTail = NULL;

	if ( FPM_Initialize( &pQueue->Pool,					// pointer to pool
						 sizeof( *(pQueue->pHead) ),	// size of item in pool
						 NULL,							// function called on item alloc (none)
						 NULL,							// function called on get (none)
						 NULL,							// function called on release (none)
						 NULL							// function called on item free (none)
						 ) == FALSE )
	{
		fReturn = FALSE;
		goto Failure;
	}

Exit:
	return	fReturn;

Failure:
	if ( fCriticalSectionInitialized != FALSE )
	{
		DNDeleteCriticalSection( &pQueue->Lock );
		fCriticalSectionInitialized = FALSE;
	}

	goto Exit;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_Deinitialize - deinitialize a queue
//
// Entry:		Pointer to queue
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_Deinitialize"

void	Queue_Deinitialize( QUEUE *const pQueue )
{
	DNASSERT( pQueue != NULL );
	DNASSERT( pQueue->pHead == NULL );
	DNASSERT( pQueue->pTail == NULL );

	FPM_Deinitialize( &pQueue->Pool );
	DNDeleteCriticalSection( &pQueue->Lock );
	DEBUG_ONLY( memset( pQueue, 0x00, sizeof( *pQueue ) ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_Lock - lock queue
//
// Entry:		Pointer to queue
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_Lock"

void	Queue_Lock( QUEUE *const pQueue )
{
	DNASSERT( pQueue != NULL );
	DNEnterCriticalSection( &pQueue->Lock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_Unlock - unlock queue
//
// Entry:		Pointer to queue
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_Unlock"

void	Queue_Unlock( QUEUE *const pQueue )
{
	DNASSERT( pQueue != NULL );
	DNLeaveCriticalSection( &pQueue->Lock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_EnQ - add element to queue
//
// Entry:		Pointer to queue
//				Pointer to data to add
//
// Exit:		Error Code
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_EnQ"

HRESULT	Queue_EnQ( QUEUE *const pQueue, void *const pItem )
{
	HRESULT	hr;
	QUEUE_ENTRY	*pTempElement;


	DNASSERT( pQueue != NULL );

	// NULL is reserved as a return value for a dequeue from and empty queue
	DNASSERT( pItem != NULL );

	// initialize
	hr = DPN_OK;

	Queue_Lock( pQueue );

	pTempElement = static_cast<QUEUE_ENTRY*>( pQueue->Pool.Get( &pQueue->Pool ) );
	if ( pTempElement == NULL )
	{
		hr = DPNERR_OUTOFMEMORY;
		DPF( 0, "Out of memory when attempting to add to queue!" );
		goto Exit;
	}

	pTempElement->pData = pItem;
	pTempElement->pNext = NULL;
	if ( pQueue->pHead == NULL )
	{
		DNASSERT( pQueue->pTail == NULL );
		pQueue->pHead = pTempElement;
	}
	else
	{
		DNASSERT( pQueue->pTail != NULL );
		pQueue->pTail->pNext = pTempElement;
	}
	pQueue->pTail = pTempElement;

Exit:
	Queue_Unlock( pQueue );

	return	hr;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_DeQ - remove element from queue
//
// Entry:		Pointer to queue
//
// Exit:		Pointer to removed element
//				NULL implies that the queue is empty
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_DeQ"

void	*Queue_DeQ( QUEUE *const pQueue )
{
	void	*pReturn;
	QUEUE_ENTRY	*pTempEntry;


	DNASSERT( pQueue != NULL );

	// initialize
	pReturn = NULL;
	Queue_Lock( pQueue );

	// note the entry being removed
	pTempEntry = pQueue->pHead;
	if ( pTempEntry != NULL )
	{
		pReturn = pTempEntry->pData;
		DNASSERT( pReturn != NULL );

		pQueue->pHead = pQueue->pHead->pNext;
		if ( pQueue->pHead == NULL )
		{
			DNASSERT( pQueue->pTail != NULL );
			pQueue->pTail = NULL;

			if ( pQueue->hIsEmptyEvent != NULL )
			{
				if ( SetEvent( pQueue->hIsEmptyEvent ) == FALSE )
				{
					DPF( 0, "Problem setting empty event for Queue!" );
					DisplayErrorCode( 0, GetLastError() );
				}
			}
		}

		// return Queue item to the available pool
		pQueue->Pool.Release( &pQueue->Pool, pTempEntry );
	}

	Queue_Unlock( pQueue );

	return	pReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// Queue_IsEmpty - checks for empty queue
//
// Entry:		Pointer to queue
//
// Exit:		Boolean indicating that the queue is empty
//
// Note:	This function doesn't lock the queue!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "Queue_IsEmpty"

BOOL	Queue_IsEmpty( const QUEUE *const pQueue )
{
	BOOL	fReturn;


	DNASSERT( pQueue != NULL );

	// initialize
	fReturn = FALSE;

	if ( pQueue->pHead == NULL )
	{
		DNASSERT( pQueue->pTail == NULL );
		fReturn = TRUE;
	}

	return	fReturn;
}
//**********************************************************************

