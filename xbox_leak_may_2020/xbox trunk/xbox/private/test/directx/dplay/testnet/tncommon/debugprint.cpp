//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <mmsystem.h>

#ifdef _XBOX
#include <stdio.h>
#include <stdlib.h>
#endif

#include "main.h"
#include "cppobjhelp.h"
#include "linklist.h"
#include "linkedstr.h"
#include "sprintf.h"
#include "strutils.h"
#include "fileutils.h"

#include "debugprint.h"


//==================================================================================
// Debugging help
//==================================================================================
#ifdef DEBUG
char	g_szDebugHelp[] =	"Debug help -"
							"  If the pointer at TNCOMMON!g_pszDebugLogHelp is NULL, the memory log is not enabled."
							"  Otherwise, dump the string it points to and follow the instructions to view the debug log.";
char*	g_pszDebugLogHelp = NULL;


#else // ! DEBUG
char	g_szDebugHelp[] =	"Not compiled with debugging support!";
#endif // ! DEBUG






// This file will not contain anything else if DEBUG is not defined
#ifdef DEBUG





//==================================================================================
// Defines
//==================================================================================
#define DEBUGPRINT_SHAREAPI_VERSION			3


#define MAX_MEMORYLOG_STRING_SIZE			512

#define DEBUGLOG_SHARED_MUTEX_PREFIX		"TNCommon Debug Log Shared Mutex"
#define DEBUGLOG_SHARED_MMFILE_PREFIX		"TNCommon Debug Log Shared MM File"

#define DEBUG_SHAREDLOGLOCATION_TNCOMMON	1
#define DEBUG_SHAREDLOGLOCATION_DPLAY		2






//==================================================================================
// Local structures
//==================================================================================
typedef struct tagTNMEMORYLOGENTRY
{
	DWORD	dwLocation; // who generated this entry (see above locations)
	int		iLevel; // spew level of the debug log entry
	DWORD	dwProcessID; // ID of process that submitted the entry
	DWORD	dwThreadID; // ID of thread that submitted the entry
	DWORD	tEntryTime; // time this entry was submitted
	char	szString[MAX_MEMORYLOG_STRING_SIZE]; // actual log entry
} TNMEMORYLOGENTRY, * PTNMEMORYLOGENTRY;

typedef struct tagTNTHREADINFOENTRY
{
	DWORD	dwThreadID; // ID of thread whose info is being maintained
	TCHAR	tszModule[256]; // module the thread was last known to be in
	TCHAR	tszSection[256]; // section the thread was last known to be in
	TCHAR	tszFile[256]; // file the thread was last known to be in
	int		iLineNum; // line number the thread was last known to be in
	TCHAR	tszInfoString[256]; // notes about the block of code the thread was last known to be in
	DWORD	tEntryTime; // time this entry was submitted
} TNTHREADINFOENTRY, * PTNTHREADINFOENTRY;



typedef struct tagDEBUGLOGMMFILEHEADER
{
	DWORD	dwVersion; // debugprint.cpp version that created this MM file
	DWORD	dwRefCount; // number of processes using this MM file
	DWORD	dwGlobalsOffset; // how far into the file the globals start
	DWORD	dwEntriesOffset; // how far into the file the entries start
} DEBUGLOGMMFILEHEADER, * PDEBUGLOGMMFILEHEADER;


typedef struct tagDEBUGLOGMEMORYGLOBALS
{
	DWORD	dwMaxMemoryLogEntries; // total number of entries in (i.e. size of) memory buffer
	int		iMemoryLogLevelMin; // minimum spew level to log to memory
	int		iMemoryLogLevelMax; // minimum spew level to log to memory
	DWORD	dwCurrentMemoryLogIndex; // current index into the memory array
	DWORD	dwEntrySize; // size of each entry; not used, for debugging purposes only
	DWORD	dwEntryNonStringSize; // size of each entry up to the string; not used, for debugging purposes only
	DWORD	dwCurrentEntryOffset; // offset of current entry; not used, for debugging purposes only
} DEBUGLOGMEMORYGLOBALS, * PDEBUGLOGMEMORYGLOBALS;



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// From DPlay, with DPLAY prepended on items not starting with DPLAY or DP

#ifndef _DPLAY_SHARED_MEMLOG_
#define _DPLAY_SHARED_MEMLOG_

#define DPLAYBASE_LOG_FILENAME  "DPLAYLOG-0"
#define DPLAYBASE_LOG_MUTEXNAME "DPLAYLOGMUTEX-0"

#define DPLOG_NUMENTRIES	5000
#define DPLOG_ENTRYSIZE		120
#define DPLOG_SIZE (sizeof(DPLAYSHARED_LOG_FILE)+((sizeof(DPLAYLOG_ENTRY)+DPLOG_ENTRYSIZE)*DPLOG_NUMENTRIES))

//
// Globals for shared memory based logging
//
typedef struct _DPLAYSHARED_LOG_FILE{
	CHAR	szAppName[16];
	DWORD   nEntries;
	DWORD	cbLine;
	DWORD 	iWrite;
	DWORD	cInUse;
	// followed by an array of LOGENTRIES.
}DPLAYSHARED_LOG_FILE, *PDPLAYSHARED_LOG_FILE;

typedef struct _DPLAYLOG_ENTRY {
	DWORD	hThread;
	DWORD	tLogged;
	DWORD	DebugLevel;
	//CHAR	str[0]; // commented out because it's not used directly and compiler barfs anyway
} DPLAYLOG_ENTRY, *PDPLAYLOG_ENTRY;

#endif // _DPLAY_SHARED_MEMLOG_
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -





//==================================================================================
// Instance-specific globals
//==================================================================================
DWORD				g_dwTNDebugRefcount = 0;
HANDLE				g_hLocalMutex = NULL;
TCHAR				g_tszModule[256];
TCHAR				g_tszSection[256];
TNTHREADINFOENTRY	g_aThreadInfos[128];
DWORD				g_dwNumThreadInfos = 0;

BOOL				g_fLogToDebugger = TRUE; // should we log to the debugger/terminal?
int					g_iDebuggerLogLevelMin = 0; // minimum spew level to log to the debugger/terminal
int					g_iDebuggerLogLevelMax = 0; // maximum spew level to log to the debugger/terminal

HANDLE				g_hFile = INVALID_HANDLE_VALUE; // handle to file to log to
TCHAR*				g_ptszCurrentFilepath = NULL; // name of current file path logging to
TCHAR*				g_ptszHeader = NULL; // header string for file
BOOL				g_fAppend = FALSE; // whether to append to files or not
DWORD				g_dwMaxFileSize = 0; // maximum size a file is allowed to grow to
int					g_iFileLogLevelMin = 0; // minimum spew level to log to the file
int					g_iFileLogLevelMax = 9; // maximum spew level to log to the file
DWORD				g_tTimeFileLastEntry = 0; // time the last entry was made

PTNMEMORYLOGENTRY	g_paLocalMemoryLog = NULL;

HANDLE				g_hSharedMutex = NULL;
HANDLE				g_hMemLogMMFile = NULL;
HANDLE				g_hMemLogView = NULL;
DWORD				g_dwLastMemLogIndex = 0;

HANDLE				g_hDPlayLogMutex = NULL;
HANDLE				g_hDPlayLogMMFile = NULL;
HANDLE				g_hDPlayLogView = NULL;
LPBYTE				g_lpDPlayLog = NULL;
DWORD				g_dwDPlayLastIndex = 0;


// These are the defaults to start up memory logging with.  This is used until
// TNDebugInitializeMemLogSharing (which is completely optional) is called.
DEBUGLOGMEMORYGLOBALS		g_defaultMemLogGlobals =
{
	0,															// dwMaxMemoryLogEntries
	0,															// iMemoryLogLevelMin
	9,															// iMemoryLogLevelMax
	0,															// dwCurrentMemoryLogIndex
	sizeof (TNMEMORYLOGENTRY),									// dwEntrySize
	(DWORD_PTR) (&(((PTNMEMORYLOGENTRY) (0x00000000))->szString)),	// dwEntryNonStringSize
	0															// dwCurrentEntryOffset
};

// These pointers may be local or shared, so don't try to free them, but they
// start off as local.
PDEBUGLOGMEMORYGLOBALS		g_pMemLogGlobals = &g_defaultMemLogGlobals;
PTNMEMORYLOGENTRY			g_paTNMemoryLog = g_paLocalMemoryLog;




//==================================================================================
// Local Prototypes
//==================================================================================
void BuildDebugLogHelpString(void);




//==================================================================================
// TNDebugInitialize
//----------------------------------------------------------------------------------
//
// Description: Initializes debug log functionality.  Must be called before any
//				other TNDebug functions are used.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugInitialize(void)
{
	DWORD	dwNewRefcount;


	//BUGBUG according to MSDN, this won't necessarily work on Win95, because "if
	//		 the result of the operation is greater than zero, the return value is
	//		 positive, but it is not necessarily equal to the result".
	dwNewRefcount = InterlockedIncrement((LPLONG) &g_dwTNDebugRefcount);

	// If this process has already initialized TNDebug at least once, we're done.
	if (dwNewRefcount > 1)
		return;

	// Otherwise, we're the first caller, set up the stuff we need.

	g_hLocalMutex = CreateMutex(NULL, TRUE, NULL);
	if (g_hLocalMutex == NULL)
	{
		OutputDebugString("Couldn't create/open debug log local mutex!\n");
		dwNewRefcount--; // should bring us back to 0
		//BUGBUG no way for caller to know this failed
		return;
	} // end if (couldn't create the mutex)


	//BUGBUG theoretically, a thread could enter this, increment to 1, get
	//		 preempted, while a separate thread sees that he doesn't need to init,
	//		 and tries to use the mutex before the first thread actually
	//		 finished creating it... 


	// Just ensure that it's initted, because we use it a lot.
	TNsprintfInitialize();

	ZeroMemory(g_tszModule, 256);
	ZeroMemory(g_tszSection, 256);
	ZeroMemory(g_aThreadInfos, 128 * sizeof (TNTHREADINFOENTRY));
	g_dwNumThreadInfos = 0;

	g_hMemLogMMFile = NULL;
	g_hMemLogView = NULL;

	// We took ownership of the mutex when we created it, so release it now
	ReleaseMutex(g_hLocalMutex);
} // TNDebugInitialize





//==================================================================================
// TNDebugCleanup
//----------------------------------------------------------------------------------
//
// Description: Cleans up debug log functionality.  Must be called for each call
//				to TNDebugInitialize.  Last caller in the process shuts down debug
//				log sharing, file, and memory logs if they were running.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugCleanup(void)
{
	// BUGBUG if someone calls Initialize while we're inside the final cleanup code,
	//		  they will get hosed.


	// We take the lock to make sure two people don't try (or nobody tries) to
	// do the final cleanup code.  Theoretically, we don't need to hold the lock for
	// the entire function, (since if we got that far, we'd be the only thread left
	// using it, but oh well)
	WaitForSingleObject(g_hLocalMutex, INFINITE); // ignore return value

	// If this process still has more users of TNDebug functions, we're done.
	if (g_dwTNDebugRefcount > 1)
	{
		g_dwTNDebugRefcount--;
		ReleaseMutex(g_hLocalMutex);
		return;
	} // end if (we're not the last one to use the function)


	// Otherwise, shut down the stuff that was in use.

	// If file logging was on, end it
	if (g_hFile != INVALID_HANDLE_VALUE)
		TNDebugEndFileLog();

	// If memory logging was on, end it
	if (g_paTNMemoryLog != NULL)
		TNDebugEndMemoryLog();

	// Balance our initialize call.
	TNsprintfCleanup();


	g_dwTNDebugRefcount--; // should be 0 after this
	if (g_dwTNDebugRefcount != 0)
	{
		OutputDebugString("Debug log refcount is screwed!\n");
		DEBUGBREAK();
	} // end if (refcount is screwed)

	ReleaseMutex(g_hLocalMutex);

	CloseHandle(g_hLocalMutex);
	g_hLocalMutex = NULL;
} // TNDebugCleanup






//==================================================================================
// TNDebugTakeLock
//----------------------------------------------------------------------------------
//
// Description: Prevents other threads (and processes, if sharing the memory debug
//				log) from printing anything to the debug log until TNDebugDropLock
//				is called by this thread.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugTakeLock(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take both locks if we're sharing the log.
	if (g_hSharedMutex != NULL)
	{
		HANDLE	ahWaitObjects[3];


		//BUGBUG The mutexes could be closed between now and the WaitForMultipleObjects call.
		//		 Duplicate handles?
		ahWaitObjects[0] = g_hLocalMutex;
		ahWaitObjects[1] = g_hSharedMutex;
		ahWaitObjects[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this will cause it to rewalk the array

		WaitForMultipleObjects(2, ahWaitObjects, TRUE, INFINITE);
	} // end if (sharing the log)
	else
		WaitForSingleObject(g_hLocalMutex, INFINITE);
} // TNDebugTakeLock



//==================================================================================
// TNDebugDropLock
//----------------------------------------------------------------------------------
//
// Description: Releases other threads (and processes, if sharing the memory debug
//				log) to print to the debug log after a previous TNDebugTakeLock
//				call.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugDropLock(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Drop the sharing lock if it's there.
	if (g_hSharedMutex != NULL)
		ReleaseMutex(g_hSharedMutex);

	ReleaseMutex(g_hLocalMutex);
} // TNDebugDropLock




//==================================================================================
// TNDebugSetDebuggerLogLevel
//----------------------------------------------------------------------------------
//
// Description: Sets the minimum and maximum levels of spew that are output to the
//				debugger.
//
// Arguments:
//	int iLevelMin	Minimum spew level to print.
//	int iLevelMax	Maximum spew level to print.
//
// Returns: None.
//==================================================================================
void TNDebugSetDebuggerLogLevel(int iLevelMin, int iLevelMax)
{
	g_iDebuggerLogLevelMin = iLevelMin;
	g_iDebuggerLogLevelMax = iLevelMax;
} // TNDebugSetDebuggerLogLevel




#ifndef _XBOX // no file logging
//==================================================================================
// TNDebugStartFileLog
//----------------------------------------------------------------------------------
//
// Description: Starts logging to the file specified.  If specified the header is
//				written to the file.
//
// Arguments:
//	LPTSTR lptszFilepath		Path to file to log to.
//	LPTSTR lptszHeaderString	Pointer to optional header string to place above
//								file, or NULL for none.
//	BOOL fAppend				Whether to append to the file or overwrite if it
//								already exists.
//	DWORD dwStartNewFileSize	If the the file reaches this size (in bytes), then a
//								new file with a sequential number is started.  Note
//								that this is approximate, since a new file will only
//								be started after a line break.
//
// Returns: None.
//==================================================================================
void TNDebugStartFileLog(LPTSTR lptszFilepath, LPTSTR lptszHeaderString,
						BOOL fAppend, DWORD dwStartNewFileSize)
{
	HRESULT		hr;
	TCHAR		tszTemp[1024];
	DWORD		dwSize;
	int			iPos;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);


	if (g_hFile != INVALID_HANDLE_VALUE)
	{
		OutputDebugString("Previously logging debug strings to a file; closing that and opening new file.\n");
		TNDebugEndFileLog();
	} // end if (already started logging)



	// Ignore error, assume BUFFER_TOO_SMALL
	FileGetRealFilePath(lptszFilepath, FALSE, NULL, &dwSize);

	// If we could possibly split the log, make room for an order indicator.
	if (dwStartNewFileSize > 0)
		dwSize += 2 * sizeof (TCHAR); // + "_1"

	g_ptszCurrentFilepath = (TCHAR*) LocalAlloc(LPTR, dwSize);
	if (g_ptszCurrentFilepath == NULL)
	{
		ReleaseMutex(g_hLocalMutex);
		return;
	} // end if (couldn't allocate memory)

	hr = FileGetRealFilePath(lptszFilepath, FALSE, g_ptszCurrentFilepath, &dwSize);
	if (hr != S_OK)
	{
		wsprintf(tszTemp, "Couldn't get real file path of \"%s\"!  hr = 0x%08x\n",
				lptszFilepath, hr);
		OutputDebugString(tszTemp);
		LocalFree(g_ptszCurrentFilepath);
		g_ptszCurrentFilepath = NULL;
		ReleaseMutex(g_hLocalMutex);
		return;
	} // end if (couldn't get real file path)

	// If we could possibly split the log, tack on an order indicator.
	if (dwStartNewFileSize > 0)
	{
		// Move back to the end of the string
		iPos = strlen(g_ptszCurrentFilepath) - 1;

		// Loop backward until we find the start of the extension
		while (g_ptszCurrentFilepath[iPos] != '.')
		{
			iPos--;

			// If we hit the beginning of the file path or the file name, then
			// there is no extension.
			if ((iPos == 0) || (g_ptszCurrentFilepath[iPos] == '\\'))
				iPos = strlen(g_ptszCurrentFilepath);
		} // end while (haven't found extension)

		// Save the extension
		strcpy(tszTemp, g_ptszCurrentFilepath + iPos);

		// Set the string to end in "_1".
		g_ptszCurrentFilepath[iPos++] = '_';
		g_ptszCurrentFilepath[iPos++] = '1';
		g_ptszCurrentFilepath[iPos++] = '\0';

		// Restore the extension
		strcat(g_ptszCurrentFilepath, tszTemp);
	} // end if (we may split the file)



	if (lptszHeaderString != NULL)
	{
		g_ptszHeader = (TCHAR*) LocalAlloc(LPTR, (strlen(lptszHeaderString) + 1) * (sizeof (TCHAR)));
		if (g_ptszHeader == NULL)
		{
			LocalFree(g_ptszCurrentFilepath);
			g_ptszCurrentFilepath = NULL;
			ReleaseMutex(g_hLocalMutex);
			return;
		} // end if (couldn't allocate memory)
		strcpy(g_ptszHeader, lptszHeaderString);
	} // end if (there's a header string)


	g_dwMaxFileSize = dwStartNewFileSize;
	g_fAppend = fAppend;


	hr = FileCreateAndOpenFile(g_ptszCurrentFilepath, FALSE, g_fAppend, FALSE, &g_hFile);
	if (hr != S_OK)
	{
		wsprintf(tszTemp, "Couldn't create debug log file \"%s\"!  hr = 0x%08x\n",
				g_ptszCurrentFilepath, hr);
		OutputDebugString(tszTemp);
		LocalFree(g_ptszCurrentFilepath);
		g_ptszCurrentFilepath = NULL;
		ReleaseMutex(g_hLocalMutex);
		return;
	} // end if (couldn't create file)


	if (g_ptszHeader != NULL)
	{
		FileWriteLine(g_hFile, "//==================================================================================");
		FileSprintfWriteLine(g_hFile, "// %s", 1, g_ptszHeader);
		FileWriteLine(g_hFile, "//");

		FileWriteLine(g_hFile, "// Debug log file part 1");

		StringGetCurrentDateStr(tszTemp);
		StringPrepend(tszTemp, "// Begins ");
		FileWriteLine(g_hFile, tszTemp);

		FileSprintfWriteLine(g_hFile, "// Current process is %x, current thread is %x",
							2, GetCurrentProcessId(), GetCurrentThreadId());
		FileWriteLine(g_hFile, "//==================================================================================");
	} // end if (have header string to display)


#ifndef _XBOX // timeGetTime not supported
	g_tTimeFileLastEntry = timeGetTime();
#else // ! XBOX
	g_tTimeFileLastEntry = GetTickCount();
#endif // XBOX

	ReleaseMutex(g_hLocalMutex);
} // TNDebugStartFileLog
#endif // ! XBOX



//==================================================================================
// TNDebugGetCurrentFileLogPath
//----------------------------------------------------------------------------------
//
// Description: Retrieves the current path the debug log output is being written to,
//				or ERROR_NOT_READY if output is not currently going to a file.
//				If lptszFilepath is NULL, then the size of the buffer required
//				(including NULL termination) is stored in pdwFilepathSize and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	LPTSTR lptszFilepath	Buffer to store file path, or NULL to retrieve size
//							required.
//	DWORD* pdwFilepathSize	Pointer to size of previous buffer, or place to store
//							size required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNDebugGetCurrentFileLogPath(LPTSTR lptszFilepath, DWORD* pdwFilepathSize)
{
	HRESULT		hr;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return (ERROR_NOT_READY);
	} // end if (not initialized)


	// Take the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);

	if (g_hFile == INVALID_HANDLE_VALUE)
	{
		hr = ERROR_NOT_READY;
		goto DONE;
	} // end if (already started logging)

	if (lptszFilepath == NULL)
	{
		(*pdwFilepathSize) = strlen(g_ptszCurrentFilepath) + 1;
		hr = ERROR_BUFFER_TOO_SMALL;
	} // end if (retrieving size)
	else
	{
		strcpy(lptszFilepath, g_ptszCurrentFilepath);
		hr = S_OK;
	} // end else (getting actual path)


DONE:

	ReleaseMutex(g_hLocalMutex);

	return (hr);
} // TNDebugGetCurrentFileLogPath




//==================================================================================
// TNDebugEndFileLog
//----------------------------------------------------------------------------------
//
// Description: Stops logging to a file.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugEndFileLog(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);
	
	if (g_hFile != INVALID_HANDLE_VALUE)
	{
		if (! CloseHandle(g_hFile))
		{
			OutputDebugString("Couldn't close debug log file!");
		} // end if (couldn't close file)

		g_hFile = INVALID_HANDLE_VALUE;
	} // end if (already started logging)

	if (g_ptszCurrentFilepath != NULL)
	{
		LocalFree(g_ptszCurrentFilepath);
		g_ptszCurrentFilepath = NULL;
	} // end if (there's a filepath)

	if (g_ptszHeader != NULL)
	{
		LocalFree(g_ptszHeader);
		g_ptszHeader = NULL;
	} // end if (there's a filepath)

	g_dwMaxFileSize = 0;

	ReleaseMutex(g_hLocalMutex);
} // TNDebugEndFileLog




//==================================================================================
// TNDebugSetFileLogLevel
//----------------------------------------------------------------------------------
//
// Description: Sets the minimum and maximum levels of spew that are output to the
//				file.
//
// Arguments:
//	int iLevelMin	Minimum spew level to print.
//	int iLevelMax	Maximum spew level to print.
//
// Returns: None.
//==================================================================================
void TNDebugSetFileLogLevel(int iLevelMin, int iLevelMax)
{
	g_iFileLogLevelMin = iLevelMin;
	g_iFileLogLevelMax = iLevelMax;
} // TNDebugSetFileLogLevel





//==================================================================================
// TNDebugStartMemoryLog
//----------------------------------------------------------------------------------
//
// Description: Starts logging to the file specified.  If it already was logging,
//				the log is recreated from scratch.
//
// Arguments:
//	DWORD dwNumEntries		Number of entries to save.
//
// Returns: None.
//==================================================================================
void TNDebugStartMemoryLog(DWORD dwNumEntries)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)

	// Take the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);
	
	if (g_paLocalMemoryLog != NULL)
	{
		OutputDebugString("Previously logging debug strings to memory; freeing that and starting new memory log.\n");
		TNDebugEndMemoryLog();
	} // end if (already started logging)

	g_pMemLogGlobals->dwMaxMemoryLogEntries = dwNumEntries;
	g_pMemLogGlobals->dwCurrentMemoryLogIndex = 0;
	g_pMemLogGlobals->dwEntrySize = sizeof (TNMEMORYLOGENTRY);
	g_pMemLogGlobals->dwCurrentEntryOffset = 0;

	g_paLocalMemoryLog = (PTNMEMORYLOGENTRY) LocalAlloc(LPTR, dwNumEntries * sizeof (TNMEMORYLOGENTRY));
	g_paTNMemoryLog = g_paLocalMemoryLog;

	// If we can't allocate memory, we won't warn but we also won't do anything
	// further.
	if (g_paLocalMemoryLog != NULL)
	{
		BuildDebugLogHelpString();
	} // end if (allocated memory)


	ReleaseMutex(g_hLocalMutex);
} // TNDebugStartMemoryLog



//==================================================================================
// TNDebugEndMemoryLog
//----------------------------------------------------------------------------------
//
// Description: Stops logging to memory.  If the memory log is being shared, this
//				is terminated as well.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugEndMemoryLog(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// If we were connected to the DPlay log, disconnect.
	if (g_hMemLogView != NULL)
	{
		TNDebugDisconnectDPlayLogging();
	} // end if (sharing the DPlay log)


#ifndef _XBOX // no IPC supported, no need to share memory
	// If we were sharing the memory log, stop that.
	if (g_hMemLogView != NULL)
	{
		TNDebugCleanupMemLogSharing();
	} // end if (sharing the memory log)
#endif // ! XBOX

	// Take the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);


	if (g_paLocalMemoryLog != NULL)
	{
		LocalFree(g_paLocalMemoryLog);
		g_paLocalMemoryLog = NULL;
	} // end if (started logging)

	g_paTNMemoryLog = NULL;

	// Destroy the debug log help string, if we had it.
	if (g_pszDebugLogHelp != NULL)
	{
		LocalFree(g_pszDebugLogHelp);
		g_pszDebugLogHelp = NULL;
	} // end if (already have string)

	ReleaseMutex(g_hLocalMutex);
} // TNDebugEndMemoryLog



//==================================================================================
// TNDebugSetMemoryLogLevel
//----------------------------------------------------------------------------------
//
// Description: Sets the minimum and maximum levels of spew that are output to
//				memory.
//
// Arguments:
//	int iLevelMin	Minimum spew level to print.
//	int iLevelMax	Maximum spew level to print.
//
// Returns: None.
//==================================================================================
void TNDebugSetMemoryLogLevel(int iLevelMin, int iLevelMax)
{
	g_pMemLogGlobals->iMemoryLogLevelMin = iLevelMin;
	g_pMemLogGlobals->iMemoryLogLevelMax = iLevelMax;
} // TNDebugSetMemoryLogLevel





#ifndef _XBOX // no dumping files
//==================================================================================
// TNDebugDumpMemoryLogToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the current contents of the memory log (local or shared) to
//				the file specified.  If specified, the header is written to the
//				file.
//
// Arguments:
//	LPTSTR lptszFilepath		Path to dump current contents to.
//	LPTSTR lptszHeaderString	Pointer to optional header string to place above
//								file, or NULL for none.
//	BOOL fAppend				Whether to overwrite the file if it exists already.
//
// Returns: None.
//==================================================================================
void TNDebugDumpMemoryLogToFile(LPTSTR lptszFilepath, LPTSTR lptszHeaderString,
								BOOL fAppend)
{
	HRESULT		hr;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	DWORD		dwTemp;
	BOOL		fOneLine = FALSE;
	DWORD		tPreviousEntryTime = 0;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take the locks
	TNDebugTakeLock();

	
	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Not currently logging debug strings to memory, ignoring call to DumpMemoryLogToFile.\n");
		goto DONE;
	} // end if (didn't start logging)


	hr = FileCreateAndOpenFile(lptszFilepath, FALSE, fAppend, FALSE, &hFile);
	if (hr != S_OK)
	{
		OutputDebugString("Failed creating/opening file to dump memory log!\n");
		goto DONE;
	} // end if (couldn't create file)


	if (lptszHeaderString != NULL)
	{
		FileWriteLine(hFile, "//==================================================================================");
		FileSprintfWriteLine(hFile, "// %s", 1, lptszHeaderString);
		FileSprintfWriteLine(hFile, "// Current process is %x, current thread is %x",
							2, GetCurrentProcessId(), GetCurrentThreadId());
		FileWriteLine(hFile, "//==================================================================================");
	} // end if (have header string to display)


	// Pull in any new entries from the DPlay log, if we're doing that.
	if (g_hDPlayLogView != NULL)
	{
		TNDebugGrabNewDPlayEntries();
	} // end if (sharing the DPlay log)
	

	dwTemp = g_pMemLogGlobals->dwCurrentMemoryLogIndex;

	do
	{
		if (strcmp(g_paTNMemoryLog[dwTemp].szString, "") != 0)
		{
			// Ignore errors.  Time printed as an integer to handle negative numbers.
			FileSprintfWriteString(hFile, "%u\t%x\t%x\t%u\t%i\t",
								5, dwTemp,
								g_paTNMemoryLog[dwTemp].dwProcessID,
								g_paTNMemoryLog[dwTemp].dwThreadID,
								g_paTNMemoryLog[dwTemp].tEntryTime,
								(g_paTNMemoryLog[dwTemp].tEntryTime - tPreviousEntryTime));

			tPreviousEntryTime = g_paTNMemoryLog[dwTemp].tEntryTime;

			if (g_paTNMemoryLog[dwTemp].dwLocation == DEBUG_SHAREDLOGLOCATION_DPLAY)
			{
				FileWriteString(hFile, "DP: ");
			} // end if (this log entry came from DPlay)

			FileWriteLine(hFile, g_paTNMemoryLog[dwTemp].szString);

			fOneLine = TRUE;
		} // end if (theres a valid string to dump)

		dwTemp++;

		if (dwTemp >= g_pMemLogGlobals->dwMaxMemoryLogEntries)
			dwTemp = 0;
	} // end do (while we haven't wrapped back to where we started)
	while (dwTemp != g_pMemLogGlobals->dwCurrentMemoryLogIndex);

	if (! fOneLine)
		FileWriteLine(hFile, "// No log entries.");


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (we had a file open)

	TNDebugDropLock();
} // TNDebugDumpMemoryLogToFile
#endif ! XBOX


//==================================================================================
// TNDebugDumpMemoryLogToString
//----------------------------------------------------------------------------------
//
// Description: Copies the current contents of the memory log (local or shared) to
//				the string passed in.  If the buffer is too small (dwStringSize is
//				reached), only the most recent entries that can completely fit in
//				the buffer are stored.
//				Entries are separated by newlines ('\n').
//
// Arguments:
//	char* szString			Pointer to buffer to place memory log into.
//	DWORD dwStringSize		How large the buffer is (including room for NULL
//							termination).
//
// Returns: None.
//==================================================================================
void TNDebugDumpMemoryLogToString(char* szString, DWORD dwStringSize)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Here's a little macro for us
	#define CHKANDDUMPSTR(string, size)		if (size > dwStringSize)\
												break;\
											\
											pszCurrent -= size;\
											CopyMemory(pszCurrent, string, size);\
											dwStringSize -= size;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	DWORD	dwTemp;
	TCHAR	tszNumber[32];
	char*	pszCurrent;
	char*	pszLastFullLine;
	DWORD	tPreviousEntryTime;



	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take the locks
	TNDebugTakeLock();

	
	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Not currently logging debug strings to memory, ignoring call to DumpMemoryLogToString.\n");
		goto DONE;
	} // end if (didn't start logging)


	// Pull in any new entries from the DPlay log, if we're doing that
	if (g_hDPlayLogView != NULL)
	{
		TNDebugGrabNewDPlayEntries();
	} // end if (sharing the DPlay log)


	dwTemp = g_pMemLogGlobals->dwCurrentMemoryLogIndex - 1;

	// Start at the end of the buffer (with enough room for the NULL termination).
	dwStringSize--;
	pszCurrent = szString + dwStringSize;

	do
	{
		// Wherever we are right now, it must have fit into the buffer (or else we
		// wouldn't be here, duh).  So this is the last known valid spot to start
		// the string.
		pszLastFullLine = pszCurrent;


		// We dump everything backwards so when we hit our head on the buffer
		// limit, we can just slide the last complete line down to the start
		// of the string buffer.

		CHKANDDUMPSTR("\n", 1);

		CHKANDDUMPSTR(g_paTNMemoryLog[dwTemp].szString,
					strlen(g_paTNMemoryLog[dwTemp].szString));

		if (g_paTNMemoryLog[dwTemp].dwLocation == DEBUG_SHAREDLOGLOCATION_DPLAY)
		{
			CHKANDDUMPSTR("DP: ", 4);
		} // end if (this log entry came from DPlay)

		if (dwTemp > 0)
			tPreviousEntryTime = g_paTNMemoryLog[dwTemp - 1].tEntryTime;
		else
			tPreviousEntryTime = g_paTNMemoryLog[g_pMemLogGlobals->dwMaxMemoryLogEntries].tEntryTime;

		// Time printed as an integer to handle negative numbers.
		wsprintf(tszNumber, "%i\t", (g_paTNMemoryLog[dwTemp].tEntryTime - tPreviousEntryTime));
		CHKANDDUMPSTR(tszNumber, strlen(tszNumber));

		wsprintf(tszNumber, "%u\t", g_paTNMemoryLog[dwTemp].tEntryTime);
		CHKANDDUMPSTR(tszNumber, strlen(tszNumber));

		wsprintf(tszNumber, "0x%08x\t", g_paTNMemoryLog[dwTemp].dwThreadID);
		CHKANDDUMPSTR(tszNumber, strlen(tszNumber));

		wsprintf(tszNumber, "0x%08x\t", g_paTNMemoryLog[dwTemp].dwProcessID);
		CHKANDDUMPSTR(tszNumber, strlen(tszNumber));

		wsprintf(tszNumber, "%i\t", dwTemp);
		CHKANDDUMPSTR(tszNumber, strlen(tszNumber));

		if (dwTemp == 0)
			dwTemp = g_pMemLogGlobals->dwMaxMemoryLogEntries;
		else
			dwTemp--;

	} // end do (while we haven't wrapped back to where we started)
	while (dwTemp != (g_pMemLogGlobals->dwCurrentMemoryLogIndex - 1));

	// Shuffle everything down to the front of the string if it's not there already
	if (pszLastFullLine > szString)
		strcpy(szString, pszLastFullLine);


DONE:

	TNDebugDropLock();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#undef CHKANDDUMPSTR
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
} // TNDebugDumpMemoryLogToString




//==================================================================================
// TNDebugDisplayNewMemLogEntries
//----------------------------------------------------------------------------------
//
// Description: Displays any items in the shared memory debug log that have been
//				entered since the last time this function was called (via another
//				process that's sharing the log, or DPlay connection, etc).  This
//				does not include any items that were printed to the memory log
//				locally.
//				The items are printed to the other debug log modes (if they are
//				running) according to the booleans passed in.
//
// Arguments:
//	BOOL fToDebuggerIfOn	The items will be printed to the debugger assuming the
//							level is correct and debugger logging is on.
//	BOOL fToFileIfOn		The items will be printed to the file assuming the
//							level is correct and file logging is on.
//
// Returns: None.
//==================================================================================
void TNDebugDisplayNewMemLogEntries(BOOL fToDebuggerIfOn, BOOL fToFileIfOn)
{
	DWORD	dwTemp;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)

	//BUGBUG these (and all other calls like them) should be inside the lock grab.
	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Memory debug log not initialized.\n");
		return;
	} // end if (mem log not initialized)


	// Grab our locks
	TNDebugTakeLock();


	//BUGBUG it's possible to wrap around to the exact same spot, and we'd miss
	//		 every single one of those entries
	if (g_dwLastMemLogIndex != g_pMemLogGlobals->dwCurrentMemoryLogIndex)
	{
		dwTemp = g_dwLastMemLogIndex;
		do
		{
			if (strcmp(g_paTNMemoryLog[dwTemp].szString, "") != 0)
			{
				// If debugger logging is on, and we should print to the debugger,
				// and the level is unknown or within the debugger logging
				// boundaries, print it.
				if ((g_fLogToDebugger) && (fToDebuggerIfOn) &&
					((g_paTNMemoryLog[dwTemp].iLevel == -1) ||
						((g_paTNMemoryLog[dwTemp].iLevel >= g_iDebuggerLogLevelMin) &&
						(g_paTNMemoryLog[dwTemp].iLevel <= g_iDebuggerLogLevelMax))))
				{
					// From experience it seems we may need to allow a little time
					// so that we don't flood the debugger.
					//Sleep(10);

					OutputDebugString("$$\t");

					if (g_paTNMemoryLog[dwTemp].dwLocation == DEBUG_SHAREDLOGLOCATION_DPLAY)
						OutputDebugString("DP: ");

					OutputDebugString(g_paTNMemoryLog[dwTemp].szString);
					OutputDebugString("\n");
				} // end if (should and can print to terminal)

				// If file logging is on, and we should print to the file, and the
				// level is unknown or within the file logging boundaries, print it.
				if ((g_hFile != INVALID_HANDLE_VALUE) && (fToFileIfOn) &&
					((g_paTNMemoryLog[dwTemp].iLevel == -1) ||
						((g_paTNMemoryLog[dwTemp].iLevel >= g_iFileLogLevelMin) &&
						(g_paTNMemoryLog[dwTemp].iLevel <= g_iFileLogLevelMax))))
				{
					// Ignore errors.  Time printed as an integer to handle negative numbers.
					FileSprintfWriteString(g_hFile, "%x\t%x\t%u\t%i\t",
										4, g_paTNMemoryLog[dwTemp].dwProcessID,
										g_paTNMemoryLog[dwTemp].dwThreadID,
										g_paTNMemoryLog[dwTemp].tEntryTime,
										(g_paTNMemoryLog[dwTemp].tEntryTime - g_tTimeFileLastEntry));

					g_tTimeFileLastEntry = g_paTNMemoryLog[dwTemp].tEntryTime;

					if (g_paTNMemoryLog[dwTemp].dwLocation == DEBUG_SHAREDLOGLOCATION_DPLAY)
					{
						FileWriteString(g_hFile, "DP: ");
					} // end if (this log entry came from DPlay)

					FileWriteLine(g_hFile, g_paTNMemoryLog[dwTemp].szString);
				} // end if (should and can print to file)
			} // end if (theres a valid string to dump)

			dwTemp++;

			if (dwTemp >= g_pMemLogGlobals->dwMaxMemoryLogEntries)
				dwTemp = 0;
		} // end do (while we haven't wrapped back to where we started)
		while (dwTemp != g_pMemLogGlobals->dwCurrentMemoryLogIndex);

		// Okay, we're up to date.
		g_dwLastMemLogIndex = g_pMemLogGlobals->dwCurrentMemoryLogIndex;
	} // end if (the DPlay log has been updated)
	/*
	else
	{
		OutputDebugString("\tNo updates to DPlay log.\n");
	} // end else (no updates to DPlay log)
	*/

	// Drop our locks
	TNDebugDropLock();
} // TNDebugDisplayNewMemLogEntries




#ifndef _XBOX // no IPC supported, no need to share memory
//==================================================================================
// TNDebugInitializeMemLogSharing
//----------------------------------------------------------------------------------
//
// Description: Sets up memory for inter-process sharing of the memory debug log.
//				TNDebugStartMemoryLog must have been called previously.
//				If a log with the same sharing key was already being shared by
//				another process, then it is used and whatever settings it has
//				override the current mem log settings.
//
// Arguments:
//	LPTSTR tszSharingKey	Key to use for sharing.
//
// Returns: None.
//==================================================================================
void TNDebugInitializeMemLogSharing(LPTSTR tszSharingKey)
{
	TCHAR					tszTemp[1024];
	PDEBUGLOGMMFILEHEADER	pHeader = NULL;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)

	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Memory debug log not initialized.\n");
		return;
	} // end if (not mem log initialized)

	if ((g_hMemLogMMFile != NULL) || (g_hMemLogView != NULL))
	{
		OutputDebugString("Debug sharing already initialized, restarting.\n");

		TNDebugCleanupMemLogSharing();
	} // end if (already initialized)

	if (tszSharingKey == NULL)
	{
		OutputDebugString("Can't pass in NULL string!\n");
		return;
	} // end if (not mem log initialized)


	// Build the shared mutex name
	wsprintf(tszTemp, "%s - %s", DEBUGLOG_SHARED_MUTEX_PREFIX, tszSharingKey);

	// Get the local lock, ignoring return value
	WaitForSingleObject(g_hLocalMutex, INFINITE);


	// Create the shared mutex, or open it if it already exists
	g_hSharedMutex = CreateMutex(NULL, FALSE, tszTemp);
	if (g_hSharedMutex == NULL)
	{
		ReleaseMutex(g_hLocalMutex);
		OutputDebugString("Couldn't create/open debug log shared mutex!\n");
		return;
	} // end if (couldn't create/open the mutex)
	

	// Build the shared memory mapping name
	wsprintf(tszTemp, "%s - %s", DEBUGLOG_SHARED_MMFILE_PREFIX, tszSharingKey);

	//BUGBUG load globals then resize mapping to hold all, or init if 0

	// Create the file mapping object, or open it if it already exists
	g_hMemLogMMFile = CreateFileMapping(INVALID_HANDLE_VALUE,
										NULL,
										PAGE_READWRITE,
										0,
										(sizeof (DEBUGLOGMMFILEHEADER) + sizeof (DEBUGLOGMEMORYGLOBALS) + (g_pMemLogGlobals->dwMaxMemoryLogEntries * sizeof (TNMEMORYLOGENTRY))),
										tszTemp);
	if (g_hMemLogMMFile == NULL)
	{
		CloseHandle(g_hSharedMutex);
		g_hSharedMutex = NULL;
		ReleaseMutex(g_hLocalMutex);
		OutputDebugString("Couldn't create debug log globals memory mapped file!\n");
		return;
	} // end if (couldn't create new file mapping)


	// Get the shared lock. We couldn't pass TRUE for CreateMutex to automatically
	// take the lock, since if the mutex already existed, it would be ignored, so
	// we have to do it explicitly here.
	WaitForSingleObject(g_hSharedMutex, INFINITE); // ignoring return value


	// Get a usable pointer to the shared memory
	g_hMemLogView = MapViewOfFile(g_hMemLogMMFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (g_hMemLogView == NULL)
	{
		OutputDebugString("Couldn't create view of file mapping!\n");
		goto ERROR_EXIT;
	} // end if (couldn't create view of file)


	pHeader = (PDEBUGLOGMMFILEHEADER) g_hMemLogView;

	// If we're the first users, initialize the shared memory to our current setup
	if (pHeader->dwVersion == 0)
	{
		pHeader->dwVersion = DEBUGPRINT_SHAREAPI_VERSION;
		pHeader->dwRefCount = 1; // us

		// Store the current memory log globals and offset
		pHeader->dwGlobalsOffset = sizeof (DEBUGLOGMMFILEHEADER);
		g_pMemLogGlobals = (PDEBUGLOGMEMORYGLOBALS) (((LPBYTE) pHeader) + pHeader->dwGlobalsOffset);
		CopyMemory(g_pMemLogGlobals, &g_defaultMemLogGlobals, sizeof (DEBUGLOGMEMORYGLOBALS));

		// Store the current memory log and offset
		pHeader->dwEntriesOffset = pHeader->dwGlobalsOffset + sizeof (DEBUGLOGMEMORYGLOBALS);
		g_paTNMemoryLog = (PTNMEMORYLOGENTRY) (((LPBYTE) pHeader) + pHeader->dwEntriesOffset);
		CopyMemory(g_paTNMemoryLog, g_paLocalMemoryLog,
					(g_pMemLogGlobals->dwMaxMemoryLogEntries * sizeof (TNMEMORYLOGENTRY)));
	} // end if (couldn't open file mapping)
	else
	{
		// Check to make sure we can use it
		if (pHeader->dwVersion != DEBUGPRINT_SHAREAPI_VERSION)
		{
			OutputDebugString("Can't use existing file mapping, incorrect version!\n");
			goto ERROR_EXIT;
		} // end if (can't use the file)


		// Increment the file's refcount
		pHeader->dwRefCount++;

		// Get pointers to the shared items
		g_pMemLogGlobals = (PDEBUGLOGMEMORYGLOBALS) (((LPBYTE) pHeader) + pHeader->dwGlobalsOffset);
		g_paTNMemoryLog = (PTNMEMORYLOGENTRY) (((LPBYTE) pHeader) + pHeader->dwEntriesOffset);

		g_dwLastMemLogIndex = g_pMemLogGlobals->dwCurrentMemoryLogIndex;
	} // end else (opened file mapping)


	// We won't need the local log anymore, so screw it.
	if (g_paLocalMemoryLog != NULL)
	{
		LocalFree(g_paLocalMemoryLog);
		g_paLocalMemoryLog = NULL;
	} // end if (local log exists)


	// We need to rebuild the help string.
	BuildDebugLogHelpString();



	// Drop the locks we had
	ReleaseMutex(g_hSharedMutex);
	ReleaseMutex(g_hLocalMutex);

	return;

ERROR_EXIT:

	if (g_hMemLogView != NULL)
	{
		UnmapViewOfFile(g_hMemLogView);
		g_hMemLogView = NULL;
	} // end if (globals view existed)

	if (g_hMemLogMMFile != NULL)
	{
		CloseHandle(g_hMemLogMMFile);
		g_hMemLogMMFile = NULL;
	} // end if (memory mapped file existed)

	if (g_hSharedMutex != NULL)
	{
		ReleaseMutex(g_hSharedMutex);
		CloseHandle(g_hSharedMutex);
		g_hSharedMutex = NULL;
	} // end if (shared mutex existed)

	ReleaseMutex(g_hLocalMutex);
} // TNDebugInitializeMemLogSharing
#endif // ! XBOX



#ifndef _XBOX // no IPC supported, no need to share memory
//==================================================================================
// TNDebugCleanupMemLogSharing
//----------------------------------------------------------------------------------
//
// Description: Releases any items that may have been allocated for sharing the
//				memory debug log.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugCleanupMemLogSharing(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	// Take the locks.
	TNDebugTakeLock();
	

	// Unmap the view if it existed
	if (g_hMemLogView != NULL)
	{
		PDEBUGLOGMMFILEHEADER	pHeader;


		// Update our local version to reflect what is currently in the shared log
		// before we fork/split.

		CopyMemory(&g_defaultMemLogGlobals, g_pMemLogGlobals, sizeof (DEBUGLOGMEMORYGLOBALS));
		g_pMemLogGlobals = &g_defaultMemLogGlobals;


		// If somehow we had a local log already, then we want to free the old one.
		if (g_paLocalMemoryLog != NULL)
		{
			OutputDebugString("WARNING: Freeing previous memory log!\n");
			LocalFree(g_paLocalMemoryLog);
			g_paLocalMemoryLog = NULL;
		} // end if (had memory log already)

		g_paLocalMemoryLog = (PTNMEMORYLOGENTRY) LocalAlloc(LPTR, (g_pMemLogGlobals->dwMaxMemoryLogEntries  * sizeof (TNMEMORYLOGENTRY)));
		if (g_paLocalMemoryLog == NULL)
		{
			TNDebugDropLock();
			return;
		} // end if (couldn't reallocate log)

		CopyMemory(g_paLocalMemoryLog, g_paTNMemoryLog,
					(g_pMemLogGlobals->dwMaxMemoryLogEntries * sizeof (TNMEMORYLOGENTRY)));
		g_paTNMemoryLog = g_paLocalMemoryLog;

		

		pHeader = (PDEBUGLOGMMFILEHEADER) g_hMemLogView;

		// Decrement the file's refcount, marking it for deletion if we're the
		// last users of it.
		pHeader->dwRefCount--;
		if (pHeader->dwRefCount == 0xFFFFFFFF)
		{
			OutputDebugString("WARNING: Shared log refcount is bogus!\n");
		} // end if (we're the last users)

		UnmapViewOfFile(g_hMemLogView);
		g_hMemLogView = NULL;
	} // end if (there was a view of the memory mapped file)

	// Close the memory mapped file
	if (g_hMemLogMMFile != NULL)
	{
		CloseHandle(g_hMemLogMMFile);
		g_hMemLogMMFile = NULL;
	} // end if (the memory mapped file exists)


	// We need to rebuild the help string.
	BuildDebugLogHelpString();


	// Close the shared mutex
	if (g_hSharedMutex != NULL)
	{
		ReleaseMutex(g_hSharedMutex);
		CloseHandle(g_hSharedMutex);
		g_hSharedMutex = NULL;
	} // end if (we took the shared lock)

	// Drop the remaining local lock
	ReleaseMutex(g_hLocalMutex);
} // TNDebugCleanupMemLogSharing
#endif // ! XBOX




#ifndef _XBOX // no IPC supported, so no need to share log
//==================================================================================
// TNDebugConnectDPlayLogging
//----------------------------------------------------------------------------------
//
// Description: Sets up resources used for sharing DPlay's debug log.
//				TNDebugStartMemoryLog must have been called previously.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugConnectDPlayLogging(void)
{
	PDPLAYSHARED_LOG_FILE	lpHeader = NULL;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)

	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Memory debug log not initialized.\n");
		return;
	} // end if (not mem log initialized)

	if ((g_hDPlayLogMMFile != NULL) || (g_hDPlayLogView != NULL))
	{
		OutputDebugString("DPlay log sharing already initialized, restarting.\n");

		TNDebugDisconnectDPlayLogging();
	} // end if (already initialized)

	TNDebugTakeLock();


	// Create the shared mutex.
	g_hDPlayLogMutex = CreateMutex(NULL, FALSE, DPLAYBASE_LOG_MUTEXNAME);
	if (g_hDPlayLogMutex == NULL)
	{
		OutputDebugString("Couldn't create DPlay log shared mutex!\n");
		goto ERROR_EXIT;
	} // end if (couldn't create the mutex)

	// Take the lock
	WaitForSingleObject(g_hDPlayLogMutex, INFINITE);


	g_hDPlayLogMMFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
										0, DPLOG_SIZE, DPLAYBASE_LOG_FILENAME);
	if (g_hDPlayLogMMFile == NULL)
	{
		OutputDebugString("Couldn't create DPlay log memory mapped file!\n");
		goto ERROR_EXIT;
	} // end if (couldn't create new file mapping)

	// Get a usable pointer to the shared memory
	g_hDPlayLogView = MapViewOfFile(g_hDPlayLogMMFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (g_hDPlayLogView == NULL)
	{
		OutputDebugString("Couldn't create view of file mapping!\n");
		goto ERROR_EXIT;
	} // end if (couldn't create view of file)


	// Initialize the shared memory to our current setup

	lpHeader = (PDPLAYSHARED_LOG_FILE) g_hDPlayLogView;

	g_lpDPlayLog = (((LPBYTE) lpHeader) + sizeof (DPLAYSHARED_LOG_FILE));

	g_dwDPlayLastIndex = lpHeader->iWrite;


	// Drop the locks we had
	ReleaseMutex(g_hDPlayLogMutex);

	TNDebugDropLock();

	return;

ERROR_EXIT:

	if (g_hDPlayLogView != NULL)
	{
		UnmapViewOfFile(g_hDPlayLogView);
		g_hDPlayLogView = NULL;
	} // end if (globals view existed)

	if (g_hDPlayLogMMFile != NULL)
	{
		CloseHandle(g_hDPlayLogMMFile);
		g_hDPlayLogMMFile = NULL;
	} // end if (memory mapped file existed)

	if (g_hDPlayLogMutex != NULL)
	{
		ReleaseMutex(g_hDPlayLogMutex);
		CloseHandle(g_hDPlayLogMutex);
		g_hDPlayLogMutex = NULL;
	} // end if (shared mutex existed)

	TNDebugDropLock();
} // TNDebugConnectDPlayLogging
#endif // ! XBOX



//==================================================================================
// TNDebugDisconnectDPlayLogging
//----------------------------------------------------------------------------------
//
// Description: Releases any items that may have been allocated for sharing the
//				DPlay debug log.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugDisconnectDPlayLogging(void)
{
	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	TNDebugTakeLock();

	if (g_hDPlayLogMutex == NULL)
	{
		ReleaseMutex(g_hLocalMutex);
		return;
	} // end if (don't have valid DPlay log mutex)

	// Get the DPlay log lock, ignoring return value
	WaitForSingleObject(g_hDPlayLogMutex, INFINITE);
	

	// Unmap the view if it existed
	if (g_hDPlayLogView != NULL)
	{
		UnmapViewOfFile(g_hDPlayLogView);
		g_hDPlayLogView = NULL;
	} // end if (there was a view of the memory mapped file)

	// Close the memory mapped file
	if (g_hDPlayLogMMFile != NULL)
	{
		CloseHandle(g_hDPlayLogMMFile);
		g_hDPlayLogMMFile = NULL;
	} // end if (the memory mapped file exists)


	// Close the shared DPlay mutex
	ReleaseMutex(g_hDPlayLogMutex);
	CloseHandle(g_hDPlayLogMutex);
	g_hDPlayLogMutex = NULL;

	TNDebugDropLock();
} // TNDebugDisconnectDPlayLogging




//==================================================================================
// TNDebugGrabNewDPlayEntries
//----------------------------------------------------------------------------------
//
// Description: Copies any items in DPlay debug log that have been entered since
//				the last time this function was called and places them in the
//				memory debug log.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNDebugGrabNewDPlayEntries(void)
{
	PDPLAYSHARED_LOG_FILE	lpHeader = NULL;
	PDPLAYLOG_ENTRY			lpEntry = NULL;
	DWORD					dwTemp;
	DWORD					dwSrcChar;
	DWORD					dwDestChar;
	char*					pszEntry;
	char					cTruncatedChar;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)

	//BUGBUG these (and all other calls like them) should be inside the lock grab.
	if (g_paTNMemoryLog == NULL)
	{
		OutputDebugString("Memory debug log not initialized.\n");
		return;
	} // end if (mem log not initialized)

	if (g_hDPlayLogView == NULL)
	{
		OutputDebugString("DPlay log not initialized.\n");
		return;
	} // end if (DPlay log not initialized)


	// Grab our locks
	TNDebugTakeLock();


	// Get the DPlay log lock, ignoring return value
	WaitForSingleObject(g_hDPlayLogMutex, INFINITE);


	lpHeader = (PDPLAYSHARED_LOG_FILE) g_hDPlayLogView;


	//BUGBUG it's possible to wrap around to the exact same spot, and we'd miss
	//		 every single one of those entries
	if (g_dwDPlayLastIndex != lpHeader->iWrite)
	{
		dwTemp = g_dwDPlayLastIndex;
		do
		{
			lpEntry = (PDPLAYLOG_ENTRY) (g_lpDPlayLog + (dwTemp * (sizeof (DPLAYLOG_ENTRY) + lpHeader->cbLine)));
			pszEntry = (char*) (lpEntry + 1);
			

			// Due to a bug in the DPlay logging, lines that exceed the length of
			// the buffer aren't terminated, which gives us garbage at the end
			// of the strings.  We're going to fix that up in here, as well as
			// put an ellipsis in our copy of the DPlay spew to indicate that it
			// got truncated.
			cTruncatedChar = '\0';

			// Check to see if we need to truncate the line.
			if (pszEntry[lpHeader->cbLine - 1] != '\0')
			{
				// Save the character at the end of the string and then terminate
				// it.
				cTruncatedChar = pszEntry[lpHeader->cbLine - 1];
				pszEntry[lpHeader->cbLine - 1] = '\0';

				// If our forced termination didn't actually affect the length of
				// the line, then don't bother saving the character we thought was
				// truncated.
				if (strlen(pszEntry) != (lpHeader->cbLine - 1))
					cTruncatedChar = '\0';
			} // end if (may need to truncate line)

			// Don't copy lines that are empty.
			if (pszEntry[0] != '\0')
			{
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwLocation = DEBUG_SHAREDLOGLOCATION_DPLAY;
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].iLevel = lpEntry->DebugLevel;
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwProcessID = 0xFFFFFFFF;
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwThreadID = lpEntry->hThread;
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].tEntryTime = lpEntry->tLogged;

				dwSrcChar = 0;
				dwDestChar = 0;
				do
				{
					// Skip newline characters.
					if (pszEntry[dwSrcChar] == '\r')
					{
						dwSrcChar++;
					} // end if (new line character)
					else
					{
						g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar++] = pszEntry[dwSrcChar];
						dwSrcChar++;

						// If we've reached our buffer limit, stop.
						if (dwDestChar >= MAX_MEMORYLOG_STRING_SIZE)
							break;
					} // end else (not the new line character)
				} // end do (while not at end of mem log string)
				while (pszEntry[dwSrcChar] != '\0');

				// If we truncated the string, try to add it to our current string
				if ((cTruncatedChar != '\0') && (dwDestChar < MAX_MEMORYLOG_STRING_SIZE))
				{
					// Copy the truncated character.
					g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar++] = cTruncatedChar;

					// If there's room, try to add an ellipsis to our string to help
					// the user notice that the line got truncated.
					if (dwDestChar < (MAX_MEMORYLOG_STRING_SIZE - 3))
					{
						g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar++] = '.';
						g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar++] = '.';
						g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar++] = '.';
					} // end if (more room in string)
				} // end if (truncated string, but more room in our buffer)

				// Make sure the string is NULL terminated.
				g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString[dwDestChar] = '\0';


				g_pMemLogGlobals->dwCurrentEntryOffset += sizeof (TNMEMORYLOGENTRY);
				g_pMemLogGlobals->dwCurrentMemoryLogIndex++;
				if (g_pMemLogGlobals->dwCurrentMemoryLogIndex >= g_pMemLogGlobals->dwMaxMemoryLogEntries)
				{
					g_pMemLogGlobals->dwCurrentEntryOffset = 0;
					g_pMemLogGlobals->dwCurrentMemoryLogIndex = 0;
				} // end if (should wrap around)
			} // end if (there's a valid entry here)

			dwTemp++;

			if (dwTemp >= lpHeader->nEntries)
				dwTemp = 0;
		} // end do (while we haven't wrapped back to where we started)
		while (dwTemp != lpHeader->iWrite);

		// Okay, we're up to date.
		g_dwDPlayLastIndex = lpHeader->iWrite;
	} // end if (the DPlay log has been updated)

	// Drop the DPlay lock
	ReleaseMutex(g_hDPlayLogMutex);

	// Drop our locks
	TNDebugDropLock();
} // TNDebugGrabNewDPlayEntries




//==================================================================================
// TNDebugGetThreadBlockNote
//----------------------------------------------------------------------------------
//
// Description: Used to retrieve the last known block info for the given thread.
//				If the thread ID has some information, it is copied into the buffers
//				provided and TRUE is returned.  If not (or an error occcured),
//				FALSE is returned.
//
// Arguments:
//	DWORD dwThreadID		Thread ID to look up
//	LPTSTR lptszModule		String to store last known module for thread.
//	LPTSTR lptszString		String to store last known section for thread.
//	LPTSTR lptszFile		String to store last known file for thread.
//	int* lpiLineNum			Pointer to int to store last known line number for thread.
//	LPTSTR lptszInfoString	String to store last known extra info for thread.
//	DWORD* lpdwNoteAge		Place to store how long ago (in ms) this note was
//							entered.
//
// Returns: TRUE if thread's info was found and successfully copied, FALSE
//			otherwise.
//==================================================================================
BOOL TNDebugGetThreadBlockNote(DWORD dwThreadID, LPTSTR lptszModule,
								LPTSTR lptszSection, LPTSTR lptszFile,
								int* lpiLineNum, LPTSTR lptszInfoString,
								DWORD* lpdwNoteAge)
{
	DWORD	dwIndex;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return (FALSE);
	} // end if (not initialized)

	for(dwIndex = 0; dwIndex < g_dwNumThreadInfos; dwIndex++)
	{
		if (g_aThreadInfos[dwIndex].dwThreadID == dwThreadID)
		{
			lstrcpy(lptszModule, g_aThreadInfos[dwIndex].tszModule);
			lstrcpy(lptszSection, g_aThreadInfos[dwIndex].tszSection);
			lstrcpy(lptszFile, g_aThreadInfos[dwIndex].tszFile);
			(*lpiLineNum) = g_aThreadInfos[dwIndex].iLineNum;
			lstrcpy(lptszInfoString, g_aThreadInfos[dwIndex].tszInfoString);
#ifndef _XBOX // timeGetTime not supported
			(*lpdwNoteAge) = timeGetTime() - g_aThreadInfos[dwIndex].tEntryTime;
#else // ! XBOX
			(*lpdwNoteAge) = GetTickCount() - g_aThreadInfos[dwIndex].tEntryTime;
#endif // XBOX

			return (TRUE);
		} // end if (we found the thread)
	} // end for (each entry)

	// If we get here, we didn't find the thread.
	return (FALSE);
} // TNDebugGetThreadBlockNote




//==================================================================================
// TNInternalDebugPrepLog
//----------------------------------------------------------------------------------
//
// Description: Part of the DPL macro.  Prepares the log entry by setting the
//				module and section to the ones passed in.
//				NOTE: This enters the critical section, and it is up to the caller
//				to leave it.  Please use the macro instead of directly calling this
//				this function.
//
// Arguments:
//	LPTSTR lptszModule	Name of module.
//	LPTSTR lptszString	Name of section.
//
// Returns: None.
//==================================================================================
void TNInternalDebugPrepLog(LPTSTR lptszModule, LPTSTR lptszSection)
{
	if (g_dwTNDebugRefcount < 1)
	{
		//OutputDebugString("Debug log not initialized.\n");

		// Note we're going to copy the info anyway.
		lstrcpy(g_tszModule, lptszModule);
		lstrcpy(g_tszSection, lptszSection);

		return; // bail
	} // end if (not initialized)

	TNDebugTakeLock();


	// Siphon in the DPlay items, if we're doing that
	if (g_hDPlayLogView != NULL)
	{
		TNDebugGrabNewDPlayEntries();
	} // end if (sharing the DPlay log)

	// If memory logging is turned on, and some form of sharing is enabled, display
	// any new items.
	if ((g_paTNMemoryLog != NULL) &&
		((g_hMemLogView != NULL) || (g_hDPlayLogView != NULL)))
	{
		//BUGBUG make customizable
		TNDebugDisplayNewMemLogEntries(FALSE, TRUE);
		//TNDebugDisplayNewMemLogEntries(TRUE, TRUE);
	} // end if (some form of sharing is on)

	lstrcpy(g_tszModule, lptszModule);
	lstrcpy(g_tszSection, lptszSection);
} // TNInternalDebugPrepLog




//==================================================================================
// TNInternalDebugPrintLine
//----------------------------------------------------------------------------------
//
// Description: Part of the DPL macro.  Prints a string line to the debug terminal
//				log file, and/or memory at a specific logging level.
//
//				See the TNsprintf source for available tokens.				
//
//
//				For example:
//					#define DEBUG_MODULE	"SOMEAPP"
//					#define DEBUG_SECTION	"MyFunc()"
//					DebugPrintLine(2, "The %s is %i.", "result", 5);
//				Would print:
//					"SOMEAPP: MyFunc(): The result is 5.\n"
//
//				NOTE: This leaves the critical section, so the caller must have
//				previously entered it.  Please use the macro for this.
//
// Arguments:
//	int iLevel					Level at which to log the string.
//	LPTSTR lptszFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms			Number of items in the following variable parameter
//								list.
//	...							Parameters to be inserted in the string.
//
// Returns: None.
//==================================================================================

// Internal version, see header actual macro to use
void TNInternalDebugPrintLine(int iLevel, LPTSTR lptszFormatString, DWORD dwNumParms,
							...)
{
#ifndef _XBOX // no TN printing supported
	TCHAR		tszCurrentModule[256];
	TCHAR		tszCurrentSection[256];
	BOOL		fDebuggerLog = FALSE;
	BOOL		fFileLog = FALSE;
	BOOL		fMemoryLog = FALSE;
	TCHAR*		ptszActualString = NULL;
	DWORD		dwActualStringSize = 0;
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		tCurrentTime;
	DWORD		dwTemp = 0;
	char*		pszOutput = NULL;


	// Save the location now, because it may get stomped by a subroutine.
	lstrcpy(tszCurrentModule, g_tszModule);
	lstrcpy(tszCurrentSection, g_tszSection);


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");

		// Note: continuing
	} // end if (not initialized)


	fDebuggerLog = ((g_fLogToDebugger) &&
					(iLevel >= g_iDebuggerLogLevelMin) &&
					(iLevel <= g_iDebuggerLogLevelMax)) ? TRUE : FALSE;

	fFileLog = ((g_hFile != INVALID_HANDLE_VALUE) &&
				(iLevel >= g_iFileLogLevelMin) &&
				(iLevel <= g_iFileLogLevelMax)) ? TRUE : FALSE;

	fMemoryLog = ((g_paTNMemoryLog != NULL) &&
				(iLevel >= g_pMemLogGlobals->iMemoryLogLevelMin) &&
				(iLevel <= g_pMemLogGlobals->iMemoryLogLevelMax)) ? TRUE : FALSE;

	// If none of the logs will print this, might as well skip the processing.
	if ((! (fDebuggerLog || fFileLog || fMemoryLog)) &&
		(g_dwTNDebugRefcount >= 1))
	{
		goto DONE;
	} // end if (no logs will print this item)

	if (dwNumParms > 0)
	{
		papvParms = (PVOID*) LocalAlloc(LPTR, dwNumParms * sizeof (PVOID));
		if (papvParms == NULL)
			goto DONE;

		va_start(currentparam, dwNumParms);

		while (dwTemp < dwNumParms)
		{
			papvParms[dwTemp] = va_arg(currentparam, PVOID);
			dwTemp++;
		} // end for (each parameter)

		va_end(currentparam);
	} // end if (there are parameters and/or module + section to print)


#ifndef _XBOX // no timeGetTime supported
	tCurrentTime = timeGetTime();
#else // ! XBOX
	tCurrentTime = GetTickCount();
#endif // XBOX

	// Actually parse the items into a string.
	TNsprintf_array(&pszOutput, lptszFormatString, dwNumParms, papvParms);

	// If we should print to the debugger and the level is appropriate (see above)
	// or we're not initialized and trying to be nice.
	if ((fDebuggerLog) || (g_dwTNDebugRefcount < 1))
	{
		dwActualStringSize = strlen(pszOutput) + 2; // + 2 = newline + NULL termination

		if (strcmp(tszCurrentModule, "") != 0)
			dwActualStringSize += strlen(tszCurrentModule) + 2; // + 2 = ": "
		if (strcmp(tszCurrentSection, "") != 0)
			dwActualStringSize += strlen(tszCurrentSection) + 2; // + 2 = ": "

		ptszActualString = (LPTSTR) LocalAlloc(LPTR, dwActualStringSize);
		if (ptszActualString == NULL)
			goto DONE;

		if (strcmp(tszCurrentModule, "") != 0)
		{
			strcat(ptszActualString, tszCurrentModule);
			strcat(ptszActualString, ": ");
		} // end if (there's a module to print)

		if (strcmp(tszCurrentSection, "") != 0)
		{
			strcat(ptszActualString, tszCurrentSection);
			strcat(ptszActualString, ": ");
		} // end if (there's a section to print)

		strcat(ptszActualString, pszOutput);
		strcat(ptszActualString, "\n");

		OutputDebugString(ptszActualString);

		LocalFree(ptszActualString);
		ptszActualString = NULL;
	} // end if (we should print to the debugger)


	// We're printing to the debugger just to be nice, but since we're not
	// initialized, there's no way we can do the advanced logging, so skip it.
	if (g_dwTNDebugRefcount < 1)
	{
		goto DONE;
	} // end if (not initialized)


	// If we should print to a file and the level is appropriate (see above)
	if (fFileLog)
	{
		TCHAR	tszProcessID[32];
		TCHAR	tszThreadID[32];
		TCHAR	tszTime[32];
		TCHAR	tszTimeSinceLast[32];


		dwActualStringSize = strlen(pszOutput) + 1; // + NULL termination

		wsprintf(tszProcessID, "0x%08x\t", GetCurrentProcessId());
		dwActualStringSize += strlen(tszProcessID);

		wsprintf(tszThreadID, "0x%08x\t", GetCurrentThreadId());
		dwActualStringSize += strlen(tszThreadID);

		wsprintf(tszTime, "%u\t", tCurrentTime);
		dwActualStringSize += strlen(tszTime);

		// Time printed as an integer to handle negative numbers.
		wsprintf(tszTimeSinceLast, "%i\t", tCurrentTime - g_tTimeFileLastEntry);
		dwActualStringSize += strlen(tszTimeSinceLast);

		// Update the last entry time
		g_tTimeFileLastEntry = tCurrentTime;


		if (strcmp(tszCurrentModule, "") != 0)
			dwActualStringSize += strlen(tszCurrentModule) + 2; // + 2 = ": "
		if (strcmp(tszCurrentSection, "") != 0)
			dwActualStringSize += strlen(tszCurrentSection) + 2; // + 2 = ": "

		ptszActualString = (LPTSTR) LocalAlloc(LPTR, dwActualStringSize);
		if (ptszActualString == NULL)
			goto DONE;

		strcat(ptszActualString, tszProcessID);
		strcat(ptszActualString, tszThreadID);
		strcat(ptszActualString, tszTime);
		strcat(ptszActualString, tszTimeSinceLast);

		if (strcmp(tszCurrentModule, "") != 0)
		{
			strcat(ptszActualString, tszCurrentModule);
			strcat(ptszActualString, ": ");
		} // end if (there's a module to print)

		if (strcmp(tszCurrentSection, "") != 0)
		{
			strcat(ptszActualString, tszCurrentSection);
			strcat(ptszActualString, ": ");
		} // end if (there's a section to print)

		strcat(ptszActualString, pszOutput);

		// Ignore error
		FileWriteLine(g_hFile, ptszActualString);

		LocalFree(ptszActualString);
		ptszActualString = NULL;


		// If we're capping file size, and that line put us over the limit, it's time
		// to start a new file.
		if ((g_dwMaxFileSize > 0) && (g_dwMaxFileSize <= GetFileSize(g_hFile, NULL)))
		{
			HRESULT		hr;
			TCHAR		tszTemp[1024];
			int			iPos;
			TCHAR*		ptszTemp;
			DWORD		dwLogNum;


			if (g_ptszCurrentFilepath == NULL)
			{
				// Uh oh, what can we do?
				goto DONE;
			} // end else (there's not filepath)

			FileGetExtension(g_ptszCurrentFilepath, tszTemp, TRUE);

			iPos = strlen(g_ptszCurrentFilepath) - strlen(tszTemp);
			g_ptszCurrentFilepath[iPos] = '\0'; // truncate the string

			iPos--; // move before the NULL terminator

			// Keep moving back until we find the underscore character.
			while (g_ptszCurrentFilepath[iPos] != '_')
			{
				iPos--;

				// If we hit the start of the name without encountering the
				// underscore character, something's messed up.
				if (iPos == 0)
					goto DONE;
			} // end while (haven't found non numeric character)

			iPos++; // move after underscore
			

			// Convert those last numbers into a DWORD and increment it.
			dwLogNum = StringToDWord(g_ptszCurrentFilepath + iPos);
			dwLogNum++;

			// + 1 for NULL termination
			ptszTemp = (TCHAR*) LocalAlloc(LPTR, (iPos + StringNumDWordDigits(dwLogNum) + strlen(tszTemp) + 1) * (sizeof (TCHAR)));
			if (ptszTemp == NULL)
				goto DONE;

			CopyMemory(ptszTemp, g_ptszCurrentFilepath, iPos * sizeof (TCHAR));
			wsprintf(ptszTemp + iPos, "%u", dwLogNum);
			strcat(ptszTemp, tszTemp);

			// Move the new filename into the global after freeing the old one.
			LocalFree(g_ptszCurrentFilepath);
			g_ptszCurrentFilepath = ptszTemp;
			ptszTemp = NULL;

			// Close the old file
			CloseHandle(g_hFile);
			g_hFile = INVALID_HANDLE_VALUE;


			// Open the new one
			hr = FileCreateAndOpenFile(g_ptszCurrentFilepath, FALSE, g_fAppend, FALSE, &g_hFile);
			if (hr != S_OK)
			{
				/*
				wsprintf(tszTemp, "Couldn't create debug log file \"%s\"!  hr = 0x%08x\n",
						g_ptszCurrentFilepath, hr);
				OutputDebugString(tszTemp);
				*/
				goto DONE;
			} // end if (couldn't create file)

			if (g_ptszHeader != NULL)
			{
				FileWriteLine(g_hFile, "//==================================================================================");
				FileSprintfWriteLine(g_hFile, "// %s", 1, g_ptszHeader);
				FileWriteLine(g_hFile, "//");

				FileSprintfWriteLine(g_hFile, "// Debug log file part %u", 1, dwLogNum);

				StringGetCurrentDateStr(tszTemp);
				StringPrepend(tszTemp, "// Begins ");
				FileWriteLine(g_hFile, tszTemp);

				FileWriteLine(g_hFile, "//==================================================================================");
			} // end if (have header string to display)
		} // end if (there's a max file size cap)
	} // end if (we should print to the debugger)

	// If we should print to memory and the level is appropriate (see above)
	if (fMemoryLog)
	{
		dwActualStringSize = strlen(pszOutput) + 1; // + 1 = NULL termination

		if (strcmp(tszCurrentModule, "") != 0)
			dwActualStringSize += strlen(tszCurrentModule) + 2; // + 2 = ": "
		if (strcmp(tszCurrentSection, "") != 0)
			dwActualStringSize += strlen(tszCurrentSection) + 2; // + 2 = ": "

		ptszActualString = (LPTSTR) LocalAlloc(LPTR, dwActualStringSize);
		if (ptszActualString == NULL)
			goto DONE;

		if (strcmp(tszCurrentModule, "") != 0)
		{
			strcat(ptszActualString, tszCurrentModule);
			strcat(ptszActualString, ": ");
		} // end if (there's a module to print)

		if (strcmp(tszCurrentSection, "") != 0)
		{
			strcat(ptszActualString, tszCurrentSection);
			strcat(ptszActualString, ": ");
		} // end if (there's a section to print)

		strcat(ptszActualString, pszOutput);

		g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwLocation = DEBUG_SHAREDLOGLOCATION_TNCOMMON;
		g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].iLevel = iLevel;
		g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwProcessID = GetCurrentProcessId();
		g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].dwThreadID = GetCurrentThreadId();
		g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].tEntryTime = tCurrentTime;


		if (strlen(ptszActualString) > MAX_MEMORYLOG_STRING_SIZE)
		{
			// Artificially terminate the string at the max length
			ptszActualString[MAX_MEMORYLOG_STRING_SIZE - 1] = 0;
		} // end if (we need to truncate the entry)

		strcpy(g_paTNMemoryLog[g_pMemLogGlobals->dwCurrentMemoryLogIndex].szString,
				ptszActualString);

		LocalFree(ptszActualString);
		ptszActualString = NULL;

		g_pMemLogGlobals->dwCurrentEntryOffset += sizeof (TNMEMORYLOGENTRY);
		g_pMemLogGlobals->dwCurrentMemoryLogIndex++;
		if (g_pMemLogGlobals->dwCurrentMemoryLogIndex >= g_pMemLogGlobals->dwMaxMemoryLogEntries)
		{
			g_pMemLogGlobals->dwCurrentEntryOffset = 0;
			g_pMemLogGlobals->dwCurrentMemoryLogIndex = 0;
		} // end if (should wrap around)

		g_dwLastMemLogIndex = g_pMemLogGlobals->dwCurrentMemoryLogIndex;
	} // end if (we should print to the debugger)


DONE:

	if (g_dwTNDebugRefcount >= 1)
	{
		TNDebugDropLock();
	} // end if (we we were initialized)

	TNsprintf_free(&pszOutput); // won't matter if it's NULL

	if (ptszActualString != NULL)
	{
		LocalFree(ptszActualString);
		ptszActualString = NULL;
	} // end if (allocated string)

	if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (allocated params array
#else // ! XBOX
#pragma TODO(tristanj, "Replace logging with Xbox log stuff")
#endif // XBOX
} // TNInternalDebugPrintLine



//==================================================================================
// TNInternalDebugNoteBlock
//----------------------------------------------------------------------------------
//
// Description: Called by the DNB macro.  Notes the 
//				Please use the macro instead of directly calling this
//				this function.
//
// Arguments:
//	LPTSTR lptszModule		Name of module.
//	LPTSTR lptszString		Name of section.
//	LPTSTR lptszFile		File containing caller.
//	int iLineNum			Line number of caller.
//	LPTSTR lptszInfoString	Extra info string describing the section.
//
// Returns: None.
//==================================================================================
void TNInternalDebugNoteBlock(LPTSTR lptszModule, LPTSTR lptszSection,
							LPTSTR lptszFile, int iLineNum, LPTSTR lptszInfoString)
{
	DWORD	dwCurrentThreadID;
	DWORD	dwIndex;


	if (g_dwTNDebugRefcount < 1)
	{
		OutputDebugString("Debug log not initialized.\n");
		return;
	} // end if (not initialized)


	dwCurrentThreadID = GetCurrentThreadId();

	for(dwIndex = 0; dwIndex < g_dwNumThreadInfos; dwIndex++)
	{
		if (g_aThreadInfos[dwIndex].dwThreadID == dwCurrentThreadID)
			break;
	} // end for (each entry)

	// If we didn't find an entry for this thread, make a new one if we can
	if (dwIndex >= g_dwNumThreadInfos)
	{
		if (g_dwNumThreadInfos >= 128)
		{
			OutputDebugString("Can't store any more thread info entries!\n");
			return;
		} // end if (we can't store any more thread info)

		dwIndex = g_dwNumThreadInfos;
		g_dwNumThreadInfos++;

		g_aThreadInfos[dwIndex].dwThreadID = dwCurrentThreadID;
	} // end if (we didn't find an existing entry)
	

	// Update the information regarding the thread.

	lstrcpy(g_aThreadInfos[dwIndex].tszModule, lptszModule);
	lstrcpy(g_aThreadInfos[dwIndex].tszSection, lptszSection);
	lstrcpy(g_aThreadInfos[dwIndex].tszFile, lptszFile);
	g_aThreadInfos[dwIndex].iLineNum = iLineNum;
	lstrcpy(g_aThreadInfos[dwIndex].tszInfoString, lptszInfoString);

#ifndef _XBOX // no timeGetTime supported
	g_aThreadInfos[dwIndex].tEntryTime = timeGetTime();
#else // ! XBOX
	g_aThreadInfos[dwIndex].tEntryTime = GetTickCount();
#endif // XBOX

} // TNInternalDebugNoteBlock





//==================================================================================
// BuildDebugLogHelpString
//----------------------------------------------------------------------------------
//
// Description: Builds the string that explains how to view the memory debug log
//				from inside a debugger.  The lock is assumed to be held.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void BuildDebugLogHelpString(void)
{
	DWORD	dwSize;
	char	szMemLogStart[32];
	char	szMemLogEnd[32];
	char	szCurrentEntryOffset[32];
	char	szEntrySize[32];
	char	szEntryNonStringSize[32];


	// Free the previous help string if there was one.
	if (g_pszDebugLogHelp != NULL)
	{
		LocalFree(g_pszDebugLogHelp);
		g_pszDebugLogHelp = NULL;
	} // end if (already have string)


	// Bail if we don't have anything to work with.
	if ((g_paTNMemoryLog == NULL) || (g_pMemLogGlobals == NULL))
		return;


	wsprintf(szMemLogStart, "0x" SPRINTF_PTR, g_paTNMemoryLog);
	wsprintf(szMemLogEnd, "0x" SPRINTF_PTR,
			((LPBYTE) g_paTNMemoryLog) + (g_pMemLogGlobals->dwMaxMemoryLogEntries * g_pMemLogGlobals->dwEntrySize));
	wsprintf(szCurrentEntryOffset, "0x" SPRINTF_PTR,
			((LPBYTE) g_pMemLogGlobals) + (DWORD_PTR) (&(((PDEBUGLOGMEMORYGLOBALS) 0x00000000)->dwCurrentEntryOffset)));
	wsprintf(szEntrySize, "0x%08x", g_pMemLogGlobals->dwEntrySize);
	wsprintf(szEntryNonStringSize, "0x%08x", g_pMemLogGlobals->dwEntryNonStringSize);


	dwSize = strlen("DebugLogHelp -  To see the log, take the address ")
			+ strlen(szMemLogStart)
			+ strlen(" (value at g_paTNMemoryLog).  Dump the DWORD at ")
			+ strlen(szCurrentEntryOffset)
			+ strlen(" (g_pMemLogGlobals->dwCurrentEntryOffset) and add it to the address.  Finally, add ")
			+ strlen(szEntryNonStringSize)
			+ strlen(" (g_pMemLogGlobals->dwEntryNonStringSize).  Dumping the string at that address will print the oldest log entry.  Dump that address minus ")
			+ strlen(szEntrySize)
			+ strlen(" (g_pMemLogGlobals->dwEntrySize) will print the most recent log entry.  Continuing to go back ")
			+ strlen(szEntrySize)
			+ strlen("  bytes will give you older and older entries.  Remember that the log wraps around at the addresses ")
			+ strlen(szMemLogStart)
			+ strlen(" and ")
			+ strlen(szMemLogEnd)
			+ strlen(".")
			+ 1;

	g_pszDebugLogHelp = (char*) LocalAlloc(LPTR, dwSize);
	if (g_pszDebugLogHelp != NULL)
	{
		strcpy(g_pszDebugLogHelp, "DebugLogHelp -  To see the log, take the address ");
		strcat(g_pszDebugLogHelp, szMemLogStart);
		strcat(g_pszDebugLogHelp, " (value at g_paTNMemoryLog).  Dump the DWORD at ");
		strcat(g_pszDebugLogHelp, szCurrentEntryOffset);
		strcat(g_pszDebugLogHelp, " (g_pMemLogGlobals->dwCurrentEntryOffset) and add it to the address.  Finally, add ");
		strcat(g_pszDebugLogHelp, szEntryNonStringSize);
		strcat(g_pszDebugLogHelp, " (g_pMemLogGlobals->dwEntryNonStringSize).  Dumping the string at that address will print the oldest log entry.  Dump that address minus ");
		strcat(g_pszDebugLogHelp, szEntrySize);
		strcat(g_pszDebugLogHelp, " (g_pMemLogGlobals->dwEntrySize) will print the most recent log entry.  Continuing to go back ");
		strcat(g_pszDebugLogHelp, szEntrySize);
		strcat(g_pszDebugLogHelp, "  bytes will give you older and older entries.  Remember that the log wraps around at the addresses ");
		strcat(g_pszDebugLogHelp, szMemLogStart);
		strcat(g_pszDebugLogHelp, " and ");
		strcat(g_pszDebugLogHelp, szMemLogEnd);
		strcat(g_pszDebugLogHelp, ".");
	} // end if (allocated memory)
} // BuildDebugLogHelpString

#endif // DEBUG

#ifdef _XBOX
void WINAPI DebugPrint (char *format, ...)
{
    va_list params;
	char szTemp [2048];

    va_start (params, format);
    vsprintf (szTemp, format, params);
    OutputDebugStringA(szTemp);
    va_end (params);
}
#endif
