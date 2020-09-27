/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Locals.h
 *  Content:	Global information for the DNWSock service provider
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/25/98	jtk		Created
 ***************************************************************************/

#ifndef __LOCALS_H__
#define __LOCALS_H__

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//
// define for any port
//
#define	ANY_PORT	((WORD) 0)

#define	BITS_PER_BYTE	8

//
// Maximum size of a receved message (1500 byte ethernet frame - 28 byte UDP
// header).  The SP will have a bit reserved in the received data by the protocol
// so it knows that the data is a 'user datagram'.  If that bit is not set, the
// data is SP-specific (enum query, enum response, proxied enum query).
//
#define	MAX_MESSAGE_SIZE	1472

//
// maximum data size in bytes
//
#define	MAX_USER_PAYLOAD	( MAX_MESSAGE_SIZE )

#define	MAX_ACTIVE_WIN9X_ENDPOINTS	25

//
// enumerated constants for IO completion returns
//
typedef	enum
{
	IO_COMPLETION_KEY_UNKNONW = 0,		// invalid value
	IO_COMPLETION_KEY_SP_CLOSE,			// SP is closing, bail on completion threads
	IO_COMPLETION_KEY_IO_COMPLETE,		// IO operation complete
	IO_COMPLETION_KEY_NEW_JOB,			// new job notification
} IO_COMPLETION_KEY;


//
// maximum value of a 32-bit unsigned variable
//
#define	UINT32_MAX	((DWORD) 0xFFFFFFFF)
#define	WORD_MAX	((WORD) 0xFFFF)

//
// default enum retries for Winsock SP and retry time (milliseconds)
//
#define	DEFAULT_ENUM_RETRY_COUNT		5
#define	DEFAULT_ENUM_RETRY_INTERVAL		1500
#define	DEFAULT_ENUM_TIMEOUT			1500
#define	ENUM_RTT_ARRAY_SIZE				16

//**********************************************************************
// Macro definitions
//**********************************************************************

//
// macro for length of array
//
#define	LENGTHOF( arg )		( sizeof( arg ) / sizeof( arg[ 0 ] ) )

//
// macro to compute the offset of an element inside of a larger structure (copied from MSDEV's STDLIB.H)
//
#define OFFSETOF(s,m)	( ( INT_PTR ) ( ( PVOID ) &( ( (s*) 0 )->m ) ) )

//**********************************************************************
// Structure definitions
//**********************************************************************

//
// forward structure and class references
//
typedef	struct	IDP8ServiceProvider	IDP8ServiceProvider;

//**********************************************************************
// Variable definitions
//**********************************************************************

//
// count of outstanding COM interfaces
//
extern volatile	LONG	g_lOutstandingInterfaceCount;

//
// invalid adapter guid
//
extern const GUID	g_InvalidAdapterGuid;

//
// thread count
//
extern	UINT_PTR	g_uThreadCount;

//
// Winsock receive buffer size
//
extern	BOOL	g_fWinsockReceiveBufferSizeOverridden;
extern	INT		g_iWinsockReceiveBufferSize;

//
// Winsock receive buffer multiplier
//
extern	DWORD_PTR		g_dwWinsockReceiveBufferMultiplier;

extern	INT		g_iWinsockReceiveBufferSize;

//**********************************************************************
// Function prototypes
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNInterlockedIncrement - Interlocked increment
//
// Entry:		Pointer to value to increment
//
// Exit:		New value
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNInterlockedIncrement"

inline LONG	DNInterlockedIncrement( volatile LONG *const pValue )
{
	DNASSERT( pValue != NULL );
	DNASSERT( *pValue != -1 );
	return	InterlockedIncrement( const_cast<LONG*>( pValue ) );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// DNInterlockedDecrement - interlocked decrement wrapper
//
// Entry:		Pointer to value to decrement
//
// Exit:		New value
// ------------------------------
#undef DPF_MODNAME
#define DPF_MODNAME "DNInterlockedDecrement"

inline LONG	DNInterlockedDecrement( volatile LONG *const pValue )
{
	DNASSERT( pValue != NULL );
	DNASSERT( *pValue != 0 );
	return	InterlockedDecrement( const_cast<LONG*>( pValue ) );
}
//**********************************************************************

#undef DPF_MODNAME

#endif	// __LOCALS_H__
