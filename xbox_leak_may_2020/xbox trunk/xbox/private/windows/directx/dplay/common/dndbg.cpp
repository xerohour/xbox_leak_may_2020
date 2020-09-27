/*==========================================================================
 *
 *  Copyright (C) 1999 - 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dndbg.c
 *  Content:	debug support for DirectPlay8
 *				
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  05-20-99	aarono	Created
 *  07-16-99	johnkan	Fixed include of OSInd.h, defined WSPRINTF macro
 *  07-19-99	vanceo	Explicitly declared OutStr as returning void for NT
 *						Build environment.
 *	07-22-99	a-evsch	Check for multiple Inits,  and release CritSec when DebugPrintf
 *						returns early.
 *	08-02-99	a-evsch	Added LOGPF support. LW entries only go into shared-file log
 *	08-31-99	johnkan	Removed include of <OSIND.H>
 *  02-17-00  	rodtoll	Added Memory / String validation routines
 *  05-23-00    RichGr  IA64: Changed some DWORDs to DWORD_PTRs to make va_arg work OK.
 *  07-16-00    jchauvin IA64:  Added %p parsing to change back to %x for Win9x machines in DebugPrintf, DebugPrintfNoLock, LogPrintf
 *  07-24-00    RichGr  IA64: As there's no separate build for Win9x, added code to detect Win9x for the %p parse-and-replace.
 *	07-29-00	masonb	Rewrite to add logging by subcomponent, perf improvements, process ID
 *	08/28/2000	masonb	Voice Merge: Modified asm in DebugPrintf to preserve registers that may have affected Voice
 *	
 *  Notes:
 *	
 *  Use /Oi compiler option for strlen()
 *
 ***************************************************************************/

#include "commoni.h"

#if defined(DEBUG)

// The constructor of this will be called prior to DllMain and the destructor
// after DllMain, so we can be assured of having the logging code properly
// initialized and deinitialized for the life of the module.
struct _InitDbg
{
	_InitDbg() { DebugPrintfInit(); }
	~_InitDbg() { DebugPrintfFini(); }
} DbgInited;

extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

//===============
// Debug  support
//===============

/*******************************************************************************
	This file contains support for the following types of logging:
		1. Logging to a VXD (Win9x only)
		2. Logging to a shared memory region
		3. Logging to the Debug Output
		4. FUTURE: Logging to a file

	General:
	========

	Debug Logging and playback is designed to operate on both Win9x and
	Windows NT (Windows 2000).  A shared file is used to capture information
	and can be played back using dp8log.exe.

	Under NT you can use the 'dt' command of NTSD to dump structures.  For
	example:

		dt DIRECTPLAYOBJECT <some memory address>

	will show all of the members of the DIRECTPLAYOBJECT structure at the
	specified address.  Some features are available only in post-Win2k
	versions of NTSD which can be obtained at http://dbg.

	Logging:
	========

	Debug Logging is controlled by settings in the WIN.INI file, under
	the section heading [DirectPlay8].  There are several settings:

	debug=9

	controls the default debug level.  All messages, at or below that debug level
	are printed.  You can control logging by each component specified in the
	g_rgszSubCompName member by adding its name to the end of the 'debug' setting:

	debug.addr=9

	sets the logging level for the addressing subcomponent to 9, leaving all
	others at either their specified level or the level specified by 'debug'
	if there is no specific level specified.

	The second setting controls where the log is seen.  If not specified, all
	debug logs are sent through the standard DebugPrint and will appear in a
	debugger if it is attached.

	log=0 {no debug output}
	log=1 {spew to console only}
	log=2 {spew to shared memory log only}
	log=3 {spew to console and shared memory log}

	This setting can also be divided by subcomponent, so:

	log=3
	log.protocol=2

	sends logs for the 'protocol' subcomponent to the shared memory log only, and
	all other logs to both locations.

	example win.ini...

	[DirectPlay8]
	Debug=7		; lots of spew
	log=2		; don't spew to debug window

	[DirectPlay8]
	Debug=0		; only fatal errors spewed to debug window

	Asserts:
	========
	Asserts are used to validate assumptions in the code.  For example
	if you know that the variable jojo should be > 700 and are depending
	on it in subsequent code, you SHOULD put an assert before the code
	that acts on that assumption.  The assert would look like:

	DNASSERT(jojo>700);

	Asserts generally will produce 3 lines of debug spew to highlight the
	breaking of the assumption.  You can add text to your asserts by ANDing:
	
	  DNASSERT(jojo>700 && "Jojo was too low");
	
	Will show the specified text when the assert occurs. For testing, you might
	want to set the system to break in on asserts.  This is done in the
	[DirectPlay8] section of WIN.INI by setting BreakOnAssert=TRUE:

	[DirectPlay8]
	Debug=0
	BreakOnAssert=TRUE
	Verbose=1

	The Verbose setting enables logging of file, function, and line information.

	Debug Breaks:
	=============
	When something really severe happens and you want the system to break in
	so that you can debug it later, you should put a debug break in the code
	path.  Some people use the philosophy that all code paths must be
	verified by hand tracing each one in the debugger.  If you abide by this
	you should place a DEBUG_BREAK() in every code path and remove them
	from the source as you trace each.  When you have good coverage but
	some unhit paths (error conditions) you should force those paths in
	the debugger.

	Debug Logging to Shared Memory Region:
	======================================

	All processes will share the same memory region, and will log the specified amount
	of activity.  The log can be viewed with the DPLOG.EXE utility.

	Debug Logging to Debug Output:
	==============================
	This option uses OutputDebugString to log the specified amount of activity.

==============================================================================*/

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DNSUBCOMP_COMMON


#define ASSERT_BUFFER_SIZE   8192
#define ASSERT_BANNER_STRING "************************************************************"
#define ASSERT_MESSAGE_LEVEL 0

#define WSPRINTF		wsprintfA
#define WVSPRINTF		wvsprintfA
#define PROF_SECT		"DirectPlay8"

//
// Globals for shared memory based logging
//

LPSTR g_rgszSubCompName[] =
{
	"UNK",		// DN_SUBCOMP_GLOBAL	0
	"CORE",		// DN_SUBCOMP_CORE		1
	"ADDR",		// DN_SUBCOMP_ADDR		2
	"LOBBY",	// DN_SUBCOMP_LOBBY		3
	"PROTOCOL", // DN_SUBCOMP_PROTOCOL	4
	"VOICE",	// DN_SUBCOMP_VOICE		5
	"DPNSVR",	// DN_SUBCOMP_DPNSVR	6
	"WSOCK",	// DN_SUBCOMP_WSOCK		7
	"MODEM",	// DN_SUBCOMP_MODEM		8
	"COMMON",	// DN_SUBCOMP_COMMON    9
	"MAX",		// DN_SUBCOMP_MAX		10 // NOTE: this should never get used, but
										  // is needed due to the way DebugPrintfInit
										  // is written, since it reads one past the end.
};

#define MAX_SUBCOMPS (sizeof(g_rgszSubCompName)/sizeof(g_rgszSubCompName[0]) - 1)
UINT g_rgLevel[MAX_SUBCOMPS] = {0};
UINT g_rgDestination[MAX_SUBCOMPS] = {1};

DWORD g_fBreakOnAssert = FALSE; // if TRUE, causes DEBUG_BREAK on false asserts.

// Informational variables set for DebugPrintf before actual call
// to do printing/logging.  These variables are locked by csDPF
// when set.  csDPF is dropped when the data is finally printed
// which means there must be a call to DebugSetLineInfo followed
// immediately by a call to DebugPrintf.  This is hidden by the
// DPF macro.
static CRITICAL_SECTION  csDPF;
static char g_szLineInfo [ ASSERT_BUFFER_SIZE ] = {0};
DWORD g_dwCurrentSubComp = DN_SUBCOMP_GLOBAL;

// DebugPrintfInit() - initialize DPF support.
void DebugPrintfInit()
{
	CHAR szLevel[32] = {0};
	strcpy(szLevel, "debug");

	CHAR szDest[32] = {0};
	strcpy(szDest, "log");

	// Loop through all the subcomps, and get the level and destination for each
	for (int iSubComp = 0; iSubComp < sizeof(g_rgszSubCompName)/sizeof(g_rgszSubCompName[0]) - 1; iSubComp++)
	{
		// Set up for the next subcomp
		strcpy(szLevel + 5, "."); // 5 is strlen of "debug", we are building debug.addr, etc.
		strcpy(szLevel + 6, g_rgszSubCompName[iSubComp + 1]);

		strcpy(szDest + 3, "."); // 3 is strlen of "log", we are building log.addr, etc.
		strcpy(szDest + 4, g_rgszSubCompName[iSubComp + 1]);
	}

    InitializeCriticalSection(&csDPF);
}

// DebugPrintfFini() - release resources used by DPF support.
void DebugPrintfFini()
{
	DeleteCriticalSection(&csDPF);
}


// DebugSetLineInfo - store information about where the DPF is from.
//
// Called before a call to DebugPrintf in order to specify the file
// line and function from which the DPF is being called.  This allows
// logging of these values.  In order to support this though, the
// values are stored in globals over the duration of the call, so a
// lock is acquired in this call and released in the DebugPrintf call.
// This means these functions MUST be called one after the other.

// On x86 we can avoid using the critsec by not adjusting the stack after this call,
// and then fixing it in DebugPrintf.  That way the values we need are locals.
void DebugSetLineInfo(LPCSTR szFile, DWORD dwLine, LPCSTR szModName, DWORD dwSubComp)
{
	// Validate the subcomp value.  If it is invalid, log everything under unknown
	if (dwSubComp >= MAX_SUBCOMPS)
	{
		dwSubComp = DN_SUBCOMP_GLOBAL;
	}

	EnterCriticalSection(&csDPF);
/*
	// Save some time if Verbose is OFF
	if (g_fLogFileAndLine)
	{
		LPCSTR c;

		int i = strlen(szFile);
		if (i < 25)
		{
			c = szFile;
		}
		else
		{
			c = szFile + i - 25;
		}

		WSPRINTF(g_szLineInfo,"(%s)%s(L%d)",c , szModName, dwLine);
	}
*/
	g_dwCurrentSubComp = dwSubComp;
}

// DebugPrintf - print a debug string
//
// You must call DebugSetLineInfo before making this call.
//
//#pragma warning(disable:4731)

void DebugPrintf(DWORD dwLevel, LPCSTR pszFormat, ...)
{
	CHAR                    szString[0x400];
	va_list                 va;
	DWORD dwCurrentSubComp;
	
	dwCurrentSubComp = g_dwCurrentSubComp;
	LeaveCriticalSection(&csDPF);

	if(g_rgLevel[dwCurrentSubComp] < dwLevel)
	{
		goto Exit;
	}

        va_start(va, pszFormat);
        vsprintf(szString, pszFormat, va);
        va_end(va);

        DebugPrint("%s:%1d:%04x:%s\n",g_rgszSubCompName[dwCurrentSubComp], dwLevel, GetCurrentThreadId(), szString);
                
Exit:
	return;
/*
	CHAR  cMsg[ ASSERT_BUFFER_SIZE ];
	CHAR  cTemp[ ASSERT_BUFFER_SIZE ];
	LPSTR szFormat;
    char  *psz = NULL;
	va_list argptr;
	LPSTR pszCursor = cMsg;
	DWORD dwCurrentSubComp;
	
	dwCurrentSubComp = g_dwCurrentSubComp;

	va_start(argptr, dwDetail);
	szFormat = (LPSTR) va_arg(argptr, DWORD_PTR);

	cMsg[0] = 0;

	// Prints out / logs as:
	// 1. Verbose
	// subcomp:dwDetail:ProcessId:ThreadId:File:Fn:Line:DebugString
	// e.g.
	// ADDR:2:0450:0378:(c:\somefile.cpp)BuildURLA(L25)Can you believe it?
	//
	// 2. Regular
	// subcomp:dwDetail:ProcessId:ThreadId:DebugString

	strcpy(pszCursor, g_rgszSubCompName[dwCurrentSubComp]);
	pszCursor += strlen(pszCursor);

	WSPRINTF(pszCursor,":%1d:",dwDetail);
	pszCursor += strlen(pszCursor);

	WSPRINTF(pszCursor,"%04x:",GetCurrentThreadId());
	pszCursor += strlen(pszCursor);

	WVSPRINTF(pszCursor, szFormat, argptr);
	pszCursor += strlen(pszCursor);

	strcpy(pszCursor, "\n");
	pszCursor += strlen(pszCursor);

		// log to debugger output
		OutputDebugStringA(cMsg);

	va_end(argptr);
*/
}

//
// NOTE: I don't want to get into error checking for buffer overflows when
// trying to issue an assertion failure message. So instead I just allocate
// a buffer that is "bug enough" (I know, I know...)
//

void _DNAssert( LPCSTR szFile, int nLine, LPCSTR szFnName, DWORD dwSubComp, LPCSTR szCondition )
{
    char buffer[ASSERT_BUFFER_SIZE];

	// Build the debug stream message
    WSPRINTF( buffer, "ASSERTION FAILED! File: %s Line: %d: %s", szFile, nLine, szCondition);

    // Actually issue the message. These messages are considered error level
    // so they all go out at error level priority.

	// Must call DebugSetLineInfo before calling DebugPrintf
	// This enters and leaves the CS 3 times, but it will only happen on an assert,
	// so perf hit should be minimal.  The previous solution was an identical function
	// to DebugPrintf that didn't keep locks.  That was a maintenance hassle keeping
	// the two identical.
	DebugSetLineInfo(szFile, nLine, szFnName, dwSubComp);
	DebugPrintf(ASSERT_MESSAGE_LEVEL, ASSERT_BANNER_STRING );
	DebugSetLineInfo(szFile, nLine, szFnName, dwSubComp);
	DebugPrintf(ASSERT_MESSAGE_LEVEL, buffer );
	DebugSetLineInfo(szFile, nLine, szFnName, dwSubComp);
	DebugPrintf(ASSERT_MESSAGE_LEVEL, ASSERT_BANNER_STRING );

    // Should we drop into the debugger?
    if(g_fBreakOnAssert)
    {
		// Into the debugger we go...
		DEBUG_BREAK();
    }
}

#endif //defined debug

BOOL IsValidStringA( const CHAR * const szString )
{
	return (!IsBadStringPtrA( szString, 0xFFFFFFFF ) );
}

BOOL IsValidStringW( const WCHAR * const  swzString )
{
	const wchar_t *szTmpLoc = swzString;
	
	if( swzString == NULL )
	{
		return FALSE;
	}
	
	_try
	{
		for( ; *szTmpLoc ; szTmpLoc++ );
	}
	_except( EXCEPTION_EXECUTE_HANDLER )
	{
		return FALSE;
	}

	return TRUE;
}


