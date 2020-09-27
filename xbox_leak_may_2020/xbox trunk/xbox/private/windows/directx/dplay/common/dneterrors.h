/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DNetErrors.h
 *  Content:    Function for expanding DNet errors to debug output
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *   12/04/98  johnkan	Created
 *   08/28/2000	masonb	Voice Merge: Fix for code that only defines one of DEBUG, DBG, _DEBUG
 *@@END_MSINTERNAL
 *
 ***************************************************************************/


#ifndef	__DNET_ERRORS_H__
#define	__DNET_ERRORS_H__

//**********************************************************************
// Constant definitions
//**********************************************************************

//
// enumerated values for determining output destination
//
typedef	enum
{
	DPNERR_OUT_CONSOLE,
	DPNERR_OUT_DEBUGGER
} DN_OUT_TYPE;

//
// enumerated values to determine error class
typedef	enum
{
	EC_DPLAY8,
	EC_INET,
	EC_TAPI,
	EC_WIN32,
	EC_WINSOCK

	// no entry for TAPI message output

} EC_TYPE;

//**********************************************************************
// Macro definitions
//**********************************************************************

#ifdef	_DEBUG

	// where is the output going?
	#ifdef	DN_PRINTF
		#define	OUT_TYPE	DPNERR_OUT_CONSOLE
	#else
		#define	OUT_TYPE	DPNERR_OUT_DEBUGGER
	#endif

	// ErrorLevel = DPF level for outputting errors
	// DNErrpr = DirectNet error code

	#define	DisplayString( ErrorLevel, String )			LclDisplayString( OUT_TYPE, ErrorLevel, String )
	#define	DisplayErrorCode( ErrorLevel, Win32Error )	LclDisplayError( EC_WIN32, OUT_TYPE, ErrorLevel, Win32Error )
	#define	DisplayDNError( ErrorLevel, DNError )		LclDisplayError( EC_DPLAY8, OUT_TYPE, ErrorLevel, DNError )
	#define	DisplayInetError( ErrorLevel, InetError )	LclDisplayError( EC_INET, OUT_TYPE, ErrorLevel, InetError )
	#define	DisplayWinsockError( ErrorLevel, WinsockError )	LclDisplayError( EC_WINSOCK, OUT_TYPE, ErrorLevel, WinsockError )

#else	// _DEBUG

	#define	DisplayString( ErrorLevel, String )
	#define	DisplayErrorCode( ErrorLevel, Win32Error )
	#define	DisplayDNError( ErrorLevel, DNError )
	#define	DisplayInetError( ErrorLevel, InetError )
	#define	DisplayWinsockError( ErrorLevel, WinsockError )

#endif	// _DEBUG

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef struct linemessage_tag	LINEMESSAGE;

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

#ifdef	__cplusplus
extern	"C"	{
#endif	// __cplusplus

#ifdef	_DEBUG

// don't call this function directly, use the 'DisplayDNError' macro
void	LclDisplayError( EC_TYPE ErrorType, DN_OUT_TYPE OutputType, DWORD ErrorLevel, HRESULT ErrorCode );
void	LclDisplayString( DN_OUT_TYPE OutputType, DWORD ErrorLevel, char *pString );

#endif

#ifdef	__cplusplus
}
#endif	// __cplusplus

#endif	// __DNET_ERRORS_H__

