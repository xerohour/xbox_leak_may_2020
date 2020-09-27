/*==========================================================================
 *
 *  Copyright (C) 1999-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       OSInd.h
 *  Content:	OS indirection functions to abstract OS specific items.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/12/99	jtk		Created
 * 	09/21/99	rodtoll		Fixed for retail build
 *   	08/28/2000	masonb		Voice Merge: Fix for code that only defines one of DEBUG, DBG, _DEBUG
 *   	08/28/2000	masonb		Voice Merge: Added IsUnicodePlatform macro
 *
 ***************************************************************************/

#ifndef	__OSIND_H__
#define	__OSIND_H__


//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//
// critical section
//
#define DNCRITICAL_SECTION	CRITICAL_SECTION

//
// DirectNet time variable.  Currently 64-bit, but can be made larger
//
typedef	union
{
	UINT_PTR	Time64;

	struct
	{
		DWORD	TimeLow;
		DWORD	TimeHigh;
	} Time32;
} DN_TIME;

//**********************************************************************
// Variable definitions
//**********************************************************************
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

extern DWORD g_dwCurrentMem;
extern DWORD g_dwMaxMem;

//**********************************************************************
// Function prototypes
//**********************************************************************

//
// initialization functions
//
BOOL	DNOSIndirectionInit( void );
void	DNOSIndirectionDeinit( void );

//
// time functions
//
void		DNTimeGet( DN_TIME *const pTimeDestination );
INT_PTR		DNTimeCompare( const DN_TIME *const pTime1, const DN_TIME *const pTime2 );
void		DNTimeAdd( const DN_TIME *const pTime1, const DN_TIME *const pTime2, DN_TIME *const pTimeResult );
void		DNTimeSubtract( const DN_TIME *const pTime1, const DN_TIME *const pTime2, DN_TIME *const pTimeResult );

//
// CriticalSection functions
//
#define	AssertCriticalSectionIsTakenByThisThread( pCS, Flag )
#define	DebugSetCriticalSectionRecursionCount( pCS, Count )
#define	DNEnterCriticalSection( arg )	EnterCriticalSection( arg )
#define	DNLeaveCriticalSection( arg )	LeaveCriticalSection( arg )

BOOL	DNInitializeCriticalSection( DNCRITICAL_SECTION *const pCriticalSection );
void	DNDeleteCriticalSection( DNCRITICAL_SECTION *const pCriticalSection );


//
// Memory functions
//
#define STDNEWDELETE \
    public: __inline void *__cdecl operator new(size_t size) { return DNMalloc( size ); } \
    public: __inline void *__cdecl operator new[](size_t size) { return DNMalloc( size ); } \
    public: __inline void __cdecl operator delete(void *pData) { DNFree( pData ); } \
    public: __inline void __cdecl operator delete[](void *pData) { DNFree( pData ); }

void * DNMalloc( size_t size );
void DNFree( void *pData );
void * DNRealloc( void *pData, size_t newsize );


#endif	// __OSIND_H__
