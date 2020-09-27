/*==========================================================================
 *
 *  Copyright (C) 1999-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       OSInd.cpp
 *  Content:	OS indirection functions to abstract OS specific items.
 *
 *  History:
 *   Date		By	Reason
 *   ====		==	======
 *	07/12/99	jtk	Created
 *	09/21/99	rodtoll	Fixed for retail builds
 *	09/22/99	jtk	Added callstacks to memory allocations
 *	08/28/2000	masonb	Voice Merge: Allow new and delete with size of 0
 ***************************************************************************/

#include "commoni.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

#define PROF_SECT		"DirectPlay8"

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//
// Macro to compute the offset of an element inside of a larger structure.
// Copied from MSDEV's STDLIB.H and modified to return INT_PTR
//
#ifdef OFFSETOF
#undef OFFSETOF
#endif
#define OFFSETOF(s,m)	( ( INT_PTR ) &( ( (s*) 0 )->m ) )

//
// macro for length of array
//
#define	LENGTHOF( arg )		( sizeof( arg ) / sizeof( arg[ 0 ] ) )

//
// ASSERT macro
//

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef	_DEBUG

#define	ASSERT( arg )	if ( arg == FALSE ) { _asm { int 3 }; }

#else	// _DEBUG

#define	ASSERT( arg )

#endif	//_DEBUG

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//
// time variables
//
static	DNCRITICAL_SECTION	g_TimeLock;
static	DWORD				g_dwLastTimeCall = 0;

DWORD g_dwCurrentMem = 0;
DWORD g_dwMaxMem = 0;

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

//**********************************************************************
// ------------------------------
// DNOSIndirectionInit - initialize the OS indirection layer
//
// Entry:		Nothing
//
// Exit:		Boolean indicating success
//				TRUE = initialization successful
//				FALSE = initialization unsuccessful
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNOSIndirectionInit"

BOOL	DNOSIndirectionInit( void )
{
	//
	// get initial time for timebase
	//
	g_dwLastTimeCall = timeGetTime();
	if ( DNInitializeCriticalSection( &g_TimeLock ) == FALSE )
	{
		goto Failure;
	}

	goto Exit;

Exit:
	return	TRUE;

Failure:
	DNOSIndirectionDeinit();

	return FALSE;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNOSIndirectionDeinit - deinitialize OS indirection layer
//
// Entry:		Nothing
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNOSIndirectionDeinit"

void	DNOSIndirectionDeinit( void )
{
	//
	// clean up time management resources
	//
	DNDeleteCriticalSection( &g_TimeLock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNTimeGet - get time in milliseconds
//
// Entry:		Pointer to destination time
//
// Exit:		Nothing
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNTimeGet"

void	DNTimeGet( DN_TIME *const pTimeDestination )
{
static	DN_TIME	Time = { 0 };
	DN_TIME		DeltaT;
	DWORD		dwCurrentTime;


	DNASSERT( pTimeDestination != NULL );

#pragma	BUGBUG( johnkan, "Fix this!  We shouldn't call timeGetTime() for time each function call!" )
#pragma	TODO( johnkan, "Should this be inlined?" )

	DNEnterCriticalSection( &g_TimeLock );

	//
	// we'll assume that we're getting called more than once every 40 days
	// so time wraps can be easily accounted for
	//
	dwCurrentTime = timeGetTime();
	DeltaT.Time32.TimeHigh = 0;
	DeltaT.Time32.TimeLow = dwCurrentTime - g_dwLastTimeCall;
	if ( DeltaT.Time32.TimeLow > 0x7FFFFFFFF )
	{
		DNASSERT( FALSE );
		DeltaT.Time32.TimeLow = -static_cast<INT>( DeltaT.Time32.TimeLow );
	}

	g_dwLastTimeCall = dwCurrentTime;
	DNTimeAdd( &Time, &DeltaT, &Time );

	DBG_CASSERT( sizeof( *pTimeDestination ) == sizeof( Time ) );
	memcpy( pTimeDestination, &Time, sizeof( *pTimeDestination ) );

	DNLeaveCriticalSection( &g_TimeLock );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNTimeCompare - compare two times
//
// Entry:		Pointer to time1
//				Pointer to time2
//
// Exit:		Value indicating relative magnitude
//				-1 = *pTime1 < *pTime2
//				0 = *pTime1 == *pTime2
//				1 = *pTime1 > *pTime2
//
// Notes:	This function comes in 32-bit and 64-bit flavors.  This function
//			will result in a compile error if compiled on an unsupported platform.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNTimeCompare"

INT_PTR	DNTimeCompare( const DN_TIME *const pTime1, const DN_TIME *const pTime2 )
{
	UINT_PTR	iReturnValue;


	DNASSERT( pTime1 != NULL );
	DNASSERT( pTime2 != NULL );

#pragma	TODO( johnkan, "Should this be inlined?" )

//#ifdef	_WIN32
	if ( pTime1->Time32.TimeHigh < pTime2->Time32.TimeHigh )
	{
		iReturnValue = -1;
	}
	else
	{
		if ( pTime1->Time32.TimeHigh > pTime2->Time32.TimeHigh )
		{
			iReturnValue = 1;
		}
		else
		{
			if ( pTime1->Time32.TimeLow < pTime2->Time32.TimeLow )
			{
				iReturnValue = -1;
			}
			else
			{
				if ( pTime1->Time32.TimeLow == pTime2->Time32.TimeLow )
				{
					iReturnValue = 0;
				}
				else
				{
					iReturnValue = 1;
				}
			}
		}
	}
//#endif	// _WIN32


//#ifdef	_WIN64
//	// debug me!
//	DNASSERT( FALSE );
//
//	if ( pTime1->Time < pTime2->Time )
//	{
//		iReturnValue = -1;
//	}
//	else
//	{
//		if ( pTime1->Time == pTime2->Time )
//		{
//			iReturnValue = 0;
//		}
//		else
//		{
//			iReturnValue = 1;
//		}
//	}
//#endif	// _WIN64

	return	iReturnValue;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNTimeAdd - add two times
//
// Entry:		Pointer to time1
//				Pointer to time2
//				Pointer to time result
//
// Exit:		Nothing
//
// Note:	This function assumes that the time calculation won't wrap!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNTimeAdd"

void	DNTimeAdd( const DN_TIME *const pTime1, const DN_TIME *const pTime2, DN_TIME *const pTimeResult )
{
	DNASSERT( pTime1 != NULL );
	DNASSERT( pTime2 != NULL );
	DNASSERT( pTimeResult != NULL );

#pragma	TODO( johnkan, "Should this be inlined?" )

	_asm { mov ecx, pTime1
		   mov eax, ( DN_TIME [ ecx ] ).Time32.TimeLow;
		   mov edx, ( DN_TIME [ ecx ] ).Time32.TimeHigh

		   mov ecx, pTime2
		   add eax, ( DN_TIME [ ecx ] ).Time32.TimeLow
		   adc edx, ( DN_TIME [ ecx ] ).Time32.TimeHigh

		   mov ecx, pTimeResult
		   mov ( DN_TIME [ ecx ] ).Time32.TimeLow, eax
		   mov ( DN_TIME [ ecx ] ).Time32.TimeHigh, edx
		   };
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNTimeSubtract - subtract two times
//
// Entry:		Pointer to time1
//				Pointer to time2
//				Pointer to time result
//
// Exit:		Nothing
//
// Notes:	This function assumes no underflow!
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNTimeSubtract"

void	DNTimeSubtract( const DN_TIME *const pTime1, const DN_TIME *const pTime2, DN_TIME *const pTimeResult )
{
	DNASSERT( pTime1 != NULL );
	DNASSERT( pTime2 != NULL );
	DNASSERT( pTimeResult != NULL );


#pragma	TODO( johnkan, "Should this be inlined?" )


	_asm { mov ecx, pTime1
		   mov eax, ( DN_TIME [ ecx ] ).Time32.TimeLow
		   mov edx, ( DN_TIME [ ecx ] ).Time32.TimeHigh

		   mov ecx, pTime2
		   sub eax, ( DN_TIME [ ecx ] ).Time32.TimeLow
		   sbb edx, ( DN_TIME [ ecx ] ).Time32.TimeHigh

		   mov ecx, pTimeResult
		   mov ( DN_TIME [ ecx ] ).Time32.TimeLow, eax
		   mov ( DN_TIME [ ecx ] ).Time32.TimeHigh, edx
		   };
}
//**********************************************************************


void* DNMalloc( size_t size )
{
	void *pBuffer = NULL;

	if (size+g_dwCurrentMem <= g_dwMaxMem)
	{
		pBuffer = (void*) LocalAlloc(LMEM_ZEROINIT, size);
		if (pBuffer != NULL)
		{
			g_dwCurrentMem += size;
		}
	}

	return pBuffer;
}

void DNFree( void *pData )
{
	if (pData != NULL)
	{
		g_dwCurrentMem -= LocalSize((HLOCAL) pData);
		LocalFree( (HLOCAL) pData );
	}
}

void * DNRealloc( void *pData, size_t newsize )
{
	void *pBuffer = NULL;

	pBuffer = (void*) LocalReAlloc( (HLOCAL) pData, newsize, 0 );

	if (pBuffer != NULL)
	{
		g_dwCurrentMem -= LocalSize((HLOCAL)pData);
		g_dwCurrentMem += newsize;
	}

	return pBuffer;

/*
// OLD REALLOC
	VOID *pNewData = NULL;
	UINT oldsize = 0;

	oldsize = LocalSize((HLOCAL)pData);

	if (newsize+g_dwCurrentMem <= g_dwMaxMem)
	{
		pNewData = (void*) LocalAlloc(LMEM_ZEROINIT, newsize);
	
		if (pNewData != NULL)
		{
			memcpy(pNewData, pData, (oldsize < newsize) ? oldsize : newsize );
			DNFree(pData);

			if (newsize > oldsize)
			{
				g_dwCurrentMem += (newsize - oldsize);
			}
			else
			{
				g_dwCurrentMem -= (oldsize - newsize);
			}
		}
	}

	return pNewData;
*/
}

//**********************************************************************
// ------------------------------
// DNInitializeCriticalSection - initialize a critical section
//
// Entry:		Pointer to critical section
//
// Exit:		Boolean indicating success
//				TRUE = success
//				FALSE = failue
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNInitializeCriticalSection"

BOOL	DNInitializeCriticalSection( DNCRITICAL_SECTION *const pCriticalSection )
{
	BOOL	fReturn;


	DNASSERT( pCriticalSection != NULL );
	fReturn = TRUE;

	memset( pCriticalSection, 0x00, sizeof( *pCriticalSection ) );

	InitializeCriticalSection( pCriticalSection );

	//
	// attempt to enter the critical section once
	//
	_try
	{
		EnterCriticalSection( pCriticalSection );
	}
	_except( EXCEPTION_EXECUTE_HANDLER )
	{
		fReturn = FALSE;
	}

	//
	// if we didn't fail on entering the critical section, make sure
	// we release it
	//
	if ( fReturn != FALSE )
	{
		LeaveCriticalSection( pCriticalSection );
	}

	return	fReturn;
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNDeleteCriticalSection - delete a critical section
//
// Entry:		Pointer to critical section
//
// Exit:		Nothing
//
// Notes:	This function wrapping is overkill, but we're closing down so
//			the overhead is negligible.
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNDeleteCriticalSection"

void	DNDeleteCriticalSection( DNCRITICAL_SECTION *const pCriticalSection )
{
	DNASSERT( pCriticalSection != NULL );

	DeleteCriticalSection( pCriticalSection );
	memset( pCriticalSection, 0x00, sizeof( *pCriticalSection ) );
}
//**********************************************************************

/*
//**********************************************************************
// ------------------------------
// operator new - allocate memory for a C++ class
//
// Entry:		Size of memory to allocate
//
// Exit:		Pointer to memory
//				NULL = no memory available
//
// Notes:	This function is for classes only and will ASSERT on zero sized
//			allocations!  This function also doesn't do the whole proper class
//			thing of checking for replacement 'new handlers' and will not throw
//			an exception if allocation fails.
// ------------------------------
inline	void*	__cdecl operator new( size_t size )
{
	DPF(0,"New called");
	
	return DNMalloc( size );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// operator delete - deallocate memory for a C++ class
//
// Entry:		Pointer to memory
//
// Exit:		Nothing
//
// Notes:	This function is for classes only and will ASSERT on NULL frees!
// ------------------------------
inline	void	__cdecl operator delete( void *pData )
{
	if( pData == NULL )
		return;

	DNFree( pData );
}
//**********************************************************************
*/


#ifdef	_DEBUG
//**********************************************************************
//**
//** THIS IS THE DEBUG-ONLY SECTION.  DON'T ADD FUNCTIONS HERE THAT ARE
//** REQUIRED FOR THE RELEASE BUILD!!
//**
//**********************************************************************

//**********************************************************************
//**
//** THIS IS THE DEBUG-ONLY SECTION.  DON'T ADD FUNCTIONS HERE THAT ARE
//** REQUIRED FOR THE RELEASE BUILD!!
//**
//**********************************************************************
#endif	// _DEBUG
