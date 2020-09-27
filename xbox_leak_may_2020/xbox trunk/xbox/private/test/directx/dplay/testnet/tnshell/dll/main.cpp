//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include <objbase.h>
#include <cguid.h>
#include <windowsx.h>
#include <initguid.h>


#include <math.h>
#include <time.h>
#include <stdio.h>

#include <winsockx.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\cmdline.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\fileutils.h"
#include "..\tncommon\sprintf.h"
#ifndef _XBOX
#include "..\tncommon\text.h"
#endif // ! XBOX
#include "..\tncommon\symbols.h"
#include "..\tncommon\errors.h"

#include "..\tncontrl\tncontrl.h"

#include "version.h"
#include "resource.h"

#include "tnshell.h"
#include "main.h"
#include "prefs.h"
#include "select.h"

#include "xlog.h"

#ifdef _XBOX
HANDLE	g_hLog = NULL;			// Handle for using Xbox logging subsystem
#endif

//==================================================================================
// External Globals
//==================================================================================
// For now, make StartTest thread safe by using a critical section and serializing everything
CRITICAL_SECTION	g_csGlobal;
char*				g_pszTestNetRootPath = NULL;
PMODULEDATA			g_pModuleData = NULL;
DWORD				g_dwMode = TNMODE_API;
char*				g_pszSessionFilter = NULL;
char*				g_pszMastersAddress = NULL;
char*				g_pszMetaMastersAddress = NULL;
BOOL				g_fUseReps = FALSE;
DWORD				g_dwReps = 1;
BOOL				g_fUseTimelimit = FALSE;
DWORD				g_dwTimelimit = 0;



//==================================================================================
// Defines
//==================================================================================
#define SESSION_DIVIDER_STRING		"--------------------------------------------------------"

//==================================================================================
// Structures
//==================================================================================
typedef struct tagWORKERTHREADDATA
{
	HANDLE					hThread; // handle to the thread
	HANDLE					hKillThreadEvent; // set when it's time to die
	HANDLE					hCloseControlSessionEvent; // set when the worker thread should close the control session on behalf of the main thread
	HANDLE					hControlLayerEvent; // set when the control layer completes an operation or the stats should be updated.
} WORKERTHREADDATA, * PWORKERTHREADDATA;


//==================================================================================
// Prototypes
//==================================================================================
HRESULT AnalyzeCommandLine(PCOMMANDLINE pCmdline);
HRESULT GetShellBinariesInfo(void);
HRESULT InitializeModule(PCOMMANDLINE pCmdline);
HRESULT Cleanup(void);

HRESULT DoStartSearchForSession(void);

HRESULT StartWorkerThread(void);
HRESULT KillWorkerThread(void);

DWORD WINAPI WorkerThreadProc(LPVOID lpvParameter);


//==================================================================================
// Globals
//==================================================================================
PTNSLAVE				g_pTNSlave = NULL;
BOOL					g_fControlInitialized = FALSE;
HANDLE					g_hUserCancelEvent = NULL;
WORKERTHREADDATA		g_wtd;
char*					g_pszModuleSavedBinsPath = NULL;
DWORD					g_dwModCtrlLayerFlags = 0;
DWORD					g_dwSessionID = 0;

//==================================================================================
// Module sub directories
//==================================================================================
#define MODSUBDIR_BINSAVE		"binsave"
#define MODSUBDIR_PREFS			"prefs"
#define MODSUBDIR_REPORTS		"reports"
#define MODSUBDIR_SCRIPTS		"scripts"
#define MODSUBDIR_ERRTOSRC		"errtosrc"
#define MODSUBDIR_DOCS			"docs"

char*	c_aszSubDirs[] =
{
	MODSUBDIR_BINSAVE,
		MODSUBDIR_PREFS,
		MODSUBDIR_REPORTS,
		MODSUBDIR_SCRIPTS,
		MODSUBDIR_ERRTOSRC,
		MODSUBDIR_DOCS
};



//==================================================================================
// Command line parameters
//==================================================================================
char*				g_pszModuleName = NULL;

// Sucks, but we have to pretend to be Winsock 1.
DWORD				g_dwUseControlMethodID = TN_CTRLMETHOD_TCPIP_WINSOCK1;
#pragma TODO(tristanj, "Do we need to do anything about the lack of Winsock 2 event functions?")

CMDLINEIDITEM		c_controlMethodIDTable[] = 
{
	"winsockoptimal",	TN_CTRLMETHOD_TCPIP_OPTIMAL,
		"ws",				TN_CTRLMETHOD_TCPIP_OPTIMAL,
		"winsock",			TN_CTRLMETHOD_TCPIP_OPTIMAL,
		"ws2",				TN_CTRLMETHOD_TCPIP_OPTIMAL,
		"winsock2",			TN_CTRLMETHOD_TCPIP_OPTIMAL,
		
		"winsock1",			TN_CTRLMETHOD_TCPIP_WINSOCK1,
		"ws1",				TN_CTRLMETHOD_TCPIP_WINSOCK1,
		
		"winsock2onetoone",	TN_CTRLMETHOD_TCPIP_WINSOCK2_ONETOONE,
		"ws2onetoone",		TN_CTRLMETHOD_TCPIP_WINSOCK2_ONETOONE
};

CMDLINEIDITEM		c_modeIDTable[] = 
{
	"api",				TNMODE_API,
		"stress",			TNMODE_STRESS,
		"poke",				TNMODE_POKE,
		"picky",			TNMODE_PICKY,
};


CMDLINEHANDLEDITEM	c_aHandledItems[] =
{
	
	{CLPT_ID,			"controlmethod",
		"Specifies a means of communicating with other TestNet applications."
		"  Default is \"winsockoptimal\".",
		0,
		(PVOID*) (&g_dwUseControlMethodID),
		c_controlMethodIDTable,
		sizeof (c_controlMethodIDTable)},
								
	{CLPT_ID,			"mode",
		"Specifies a mode to run in."
		"  Default is \"api\".",
		0,
		(PVOID*) (&g_dwMode),
		c_modeIDTable,
		sizeof (c_modeIDTable)},
	
	{CLPT_STRING,		"session",
		"Specifies a session filter key, to allow/prevent joining incorrect sessions."
		"  Can be any string, but must be same for all machines intended to be connected.",
		0,
		(PVOID*) (&g_pszSessionFilter),
		NULL,
		0},
	
	{CLPT_STRING,		"joinmaster",
		"Specifies a particular master machine to connect to, instead of broadcasting."
		"  Control method specific.",
		0,
		(PVOID*) (&g_pszMastersAddress),
		NULL,
		0},
	
	{CLPT_STRING,		"usemetamaster",
		"Specifies a meta-master machine to connect to."
		"  Control method specific.",
		0,
		(PVOID*) (&g_pszMetaMastersAddress),
		NULL,
		0},
	
	{CLPT_DWORD,		"reps",
		"Causes the list of tests to be repeated a specified number of times in API mode."
		"  Default is 1."
		"  0 means infinite repetitions.",
		0,
		(PVOID*) (&g_dwReps),
		NULL,
		0},
	
	{CLPT_DWORD,		"timelimit",
		"Specifies a limit (in minutes) for how long a slave will run tests."
		"  Default is 0 which means no limit.",
		0,
		(PVOID*) (&g_dwTimelimit),
		NULL,
		0},
	
	{CLPT_STRING,		"module",
		"Specifies a module to use."
		"  If this is not specified, a window will be displayed allowing this and other options to be selected",
		0,
		(PVOID*) (&g_pszModuleName),
		NULL,
		0},
	
};




#undef DEBUG_SECTION
#define DEBUG_SECTION	"AnalyzeCommandLine()"
//==================================================================================
// AnalyzeCommandLine
//----------------------------------------------------------------------------------
//
// Description: Parses the command line and sets the appropriate globals based on
//				what it finds.  If this function fails during set up, it is assumed
//				that clean up will still be called and any partially allocated
//				stuff will be freed then.
//
// Arguments:
//	PCOMMANDLINE pCmdline	Pointer to command line object with the command line
//							already loaded.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT AnalyzeCommandLine(PCOMMANDLINE pCmdline)
{
	HRESULT		hr = S_OK;
	DWORD		dwSize = 0;
	char*		pszItem;
	char*		pszTemp = NULL;
	
	
	
	//BUGBUG how much overriding vs. blending do we want when both the master and the
	//		 slave specify things?
	
	
#ifndef _XBOX // Not supporting _splitpath
	// Ignore error, assume BUFFER_TOO_SMALL
	pCmdline->GetPathToAppDir(NULL, &dwSize);
	
	g_pszTestNetRootPath = (char*) LocalAlloc(LPTR, dwSize);
	if (g_pszTestNetRootPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
	hr = pCmdline->GetPathToAppDir(g_pszTestNetRootPath, &dwSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get application directory path!", 0);
		goto DONE;
	} // end if (couldn't get path to application)
	
	if (StringStartsWith(g_pszTestNetRootPath, "\\\\", TRUE))
	{
		DPL(0, "Appear to be running from the network share \"%s\"!",
			1, g_pszTestNetRootPath);
		
		hr = ERROR_NOT_SAME_DEVICE;
		goto DONE;
		
	} // end if (appear to be running from a share)
#else // ! XBOX
#pragma BUGBUG(tristanj, "Hacking g_pszTestNetRootPath to be X:\\")
	g_pszTestNetRootPath = (char*) LocalAlloc(LPTR, 4);
	strcpy(g_pszTestNetRootPath, "X:\\");
#endif

	if (pCmdline->WasSpecified((PVOID*) (&g_pszSessionFilter)))
	{
		DPL(1, "Will only be searching for \"%s\" sessions.",
			1, g_pszSessionFilter);
	} // end if (we were given a session filter)
	
	
	if(pCmdline->WasSpecified((PVOID*) (&g_pszMastersAddress)))
	{
		DPL(1, "Will be joining master at \"%s\".",
			1, g_pszMastersAddress);
	} // end if (we're a slave and we were given a master address to join)
	
	if (pCmdline->WasSpecified((PVOID*) (&g_dwTimelimit)))
	{
		g_fUseTimelimit = TRUE;
		DPL(1, "Time limit of %u minutes specified.",
			1, g_dwTimelimit);
	} // end if (we were given a time limit)
	
	
	if (pCmdline->WasSpecified((PVOID*) (&g_pszModuleName)))
	{
		DPL(1, "Using module \"%s\".", 1, g_pszModuleName);
		
		g_pModuleData = (PMODULEDATA) LocalAlloc(LPTR, sizeof (MODULEDATA));
		if (g_pModuleData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		
		g_pModuleData->pszModuleName = (char*) LocalAlloc(LPTR, strlen(g_pszModuleName) + 1);
		if (g_pModuleData->pszModuleName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		
		strcpy(g_pModuleData->pszModuleName, g_pszModuleName);
	} // end if (we were given a module to use)
	
DONE:
	
	return (hr);
} // AnalyzeCommandLine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"GetShellBinariesInfo()"
//==================================================================================
// GetShellBinariesInfo
//----------------------------------------------------------------------------------
//
// Description: Retrieves location and version information for binaries that the
//				shell uses.  Also loads symbols for them.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT GetShellBinariesInfo(void)
{
#ifdef _XBOX // no versioning supported
#pragma TODO(tristanj, "Look into tracking versioning information for used binaries")
#endif // XBOX
	
	return (S_OK);
} // GetShellBinariesInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"InitializeModule()"
//==================================================================================
// InitializeModule
//----------------------------------------------------------------------------------
//
// Description: Initializes the module specified by g_pszModuleFilename or pops a
//				dialog to allow selection.
//
// Arguments:
//	PCOMMANDLINE pCmdline		Pointer to command line object with the command
//								line already loaded, used to pass to the module.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT InitializeModule(PCOMMANDLINE pCmdline)
{
	HRESULT				hr;
	DWORD				dwSize = 0;
	char*				pszTemp = NULL;
	TNINITMODULEDATA	initdata;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	
	
	ZeroMemory(&initdata, sizeof (TNINITMODULEDATA));
	
	if (g_pModuleData == NULL)
	{
		DPL(0, "No module specified!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (we still don't have a module name)
	
	
#ifdef _XBOX // We may not create new directories in the XBOX version
#pragma BUGBUG(tristanj, "What should be done about module directories?")
#endif // XBOX
	
	
	// Build the module's saved bin dir path
	
	dwSize = strlen(g_pszTestNetRootPath)			// TestNet path
		+ strlen(g_pModuleData->pszModuleName)	// module directory
		+ strlen("\\" MODSUBDIR_BINSAVE "\\")	// backslash + binsave subdirectory
		+ 1;									// NULL terminator
	g_pszModuleSavedBinsPath = (char*) LocalAlloc(LPTR, (dwSize * sizeof (char)));
	if (g_pszModuleSavedBinsPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
	strcpy(g_pszModuleSavedBinsPath, g_pszTestNetRootPath);
	strcat(g_pszModuleSavedBinsPath, g_pModuleData->pszModuleName);
	strcat(g_pszModuleSavedBinsPath, "\\" MODSUBDIR_BINSAVE "\\");
	
	hr = SelectLoadModuleAndInfo(g_pszTestNetRootPath, g_pModuleData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't load module and info!", 0);
		goto DONE;
	} // end if (couldn't load module and info)
	
	
	// Prepare the initialization data for use in a little bit
	
	initdata.dwSize = sizeof (TNINITMODULEDATA);
	initdata.fMaster = FALSE;
	initdata.dwMode = g_dwMode;
	initdata.pCmdline = pCmdline;
	
	//BUGBUG the control API version is not necessarily what the DLL has,
	//		 it's actually what we (the shell) think it is.
	initdata.shellinfo.dwSize = sizeof (TNSHELLINFO);
	initdata.shellinfo.dwShellAPIVersion = CURRENT_TNSHELL_API_VERSION;
	initdata.shellinfo.dwControlAPIVersion = CURRENT_TNCONTROL_API_VERSION;
	
	
	// Build the module's directory so we can tell it where it is
	
#ifndef _XBOX
	dwSize = strlen(g_pszTestNetRootPath)			// TestNet path
		+ strlen(g_pModuleData->pszModuleName)		// module directory
		+ 2;										// backslash and NULL terminator
#else // ! XBOX
	dwSize = 1;										// Just the NULL terminator, no path
#endif // XBOX
	
	initdata.pszModuleDirPath = (char*) LocalAlloc(LPTR, (dwSize * sizeof (char)));
	if (initdata.pszModuleDirPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
#ifndef _XBOX
	strcpy(initdata.pszModuleDirPath, g_pszTestNetRootPath);			// TestNet path
	strcat(initdata.pszModuleDirPath, g_pModuleData->pszModuleName);	// module directory
	strcat(initdata.pszModuleDirPath, "\\");							// backslash
#else // ! XBOX
	initdata.pszModuleDirPath[0] = 0;				// Just the NULL terminator, no path
#endif // XBOX
	
	
	
	// Initialize the main module
	
	hr = g_pModuleData->pfnInitModule(&initdata);
	if (hr != S_OK)
	{
		DPL(0, "Module's InitModule function failed!", 0);
		goto DONE;
	} // end if (module's InitModule function failed)
	
	g_pModuleData->fInitialized = TRUE;
	
	CopyMemory(&(g_pModuleData->funcs), &(initdata.TNModuleFuncs),
		sizeof (TNMODULEFUNCS));
	g_dwModCtrlLayerFlags = initdata.dwControlLayerFlags;
	
	
	// Make sure the main module implements a LoadTestTable function.
	if (g_pModuleData->funcs.pfnLoadTestTable == NULL)
	{
		DPL(0, "Module doesn't implement required LoadTestTable function!", 0);
		
		hr = E_FAIL;
		goto DONE;
	} // end if (the module doesn't implement required functions)
	
#ifndef _XBOX // no versioning supported
	
	// Add module binary to our list and load symbols for it
	
	dwSize = strlen(g_pModuleData->pszModuleName) + strlen(".dll") + 1;
	pszTemp = (char*) LocalAlloc(LPTR, (dwSize * sizeof (char)));
	if (pszTemp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
	strcpy(pszTemp, g_pModuleData->pszModuleName);
	strcat(pszTemp, ".dll");
	
	hr = g_shellbins.LocateAndAddBinary(pszTemp);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't locate and add module binary (\"%s\")!", 1, pszTemp);
		goto DONE;
	} // end if (module's Initialize function failed)
	
	LocalFree(pszTemp);
	pszTemp = NULL;
	
	hr = g_shellbins.LoadSymbolsForFirstBinaries();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't load symbols for first instance of module binary!", 0);
		goto DONE;
	} // end if (loading binary symbols failed)
	
#else // ! XBOX
#pragma TODO(tristanj, "Look into tracking versioning information for used binaries")
#endif // XBOX
	
DONE:
	
	if (initdata.pszModuleDirPath != NULL)
	{
		LocalFree(initdata.pszModuleDirPath);
		initdata.pszModuleDirPath = NULL;
	} // end if (allocated a module dir path)
	
	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated a string)
	
	return (hr);
} // InitializeModule
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"Cleanup()"
//==================================================================================
// Cleanup
//----------------------------------------------------------------------------------
//
// Description: Cleans up any memory or resources we may have allocated along the
//				way.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT Cleanup(void)
{
	HRESULT		hr = S_OK;
	
	
	if (g_fControlInitialized)
	{
		hr = g_pTNSlave->LeaveControlSession();
		if (hr != S_OK)
		{
			DPL(0, "Closing control object failed!  %e", 1, hr);
		} // end if (closing control object failed)
		
		g_fControlInitialized = FALSE;
	} // end if (we started using the control object)
	
	if (g_pTNSlave != NULL)
	{
		delete (g_pTNSlave);
		g_pTNSlave = NULL;
	} // end if (there's a slave object)
	
	if (g_pszModuleSavedBinsPath != NULL)
	{
		LocalFree(g_pszModuleSavedBinsPath);
		g_pszModuleSavedBinsPath = NULL;
	} // end if (there's a saved bin path)
	
	// Shut down the worker thread
	hr = KillWorkerThread();
	if (hr != S_OK)
	{
		DPL(0, "Killing worker thread failed!  %e", 1, hr);
	} // end if (killing worker thread failed)
	
	if (g_hUserCancelEvent != NULL)
	{
		CloseHandle(g_hUserCancelEvent);
		g_hUserCancelEvent = NULL;
	} // end if (there's a user cancel event)
	
	if (g_pModuleData != NULL)
	{
		if (g_pModuleData->info.pszName != NULL)
		{
			LocalFree(g_pModuleData->info.pszName);
			g_pModuleData->info.pszName = NULL;
		} // end if (there's a module name)
		
		if (g_pModuleData->info.pszDescription != NULL)
		{
			LocalFree(g_pModuleData->info.pszDescription);
			g_pModuleData->info.pszDescription = NULL;
		} // end if (there's a module description)
		
		if (g_pModuleData->info.pszAuthors != NULL)
		{
			LocalFree(g_pModuleData->info.pszAuthors);
			g_pModuleData->info.pszAuthors = NULL;
		} // end if (there's a module description)
		
		if (g_pModuleData->hModule != NULL)
		{
			if (g_pModuleData->fInitialized)
			{
				hr = g_pModuleData->pfnRelease();
				if (hr != S_OK)
				{
					DPL(0, "Test module's Release() function failed!  %e", 1, hr);
				} // end if (test module's Release() function failed)
				
				g_pModuleData->fInitialized = FALSE;
			} // end if (we initialized the module we loaded)
			
			if (! FreeLibrary(g_pModuleData->hModule))
			{
				hr = GetLastError();
				DPL(0, "Failed freeing test module library!  %e", 1, hr);
			} // end if (we failed freeing the library)
		} // end if (there's a library to free)
		
		if (g_pModuleData->pszModuleName != NULL)
		{
			LocalFree(g_pModuleData->pszModuleName);
			g_pModuleData->pszModuleName = NULL;
		} // end if (a module name was allocated)
		
		LocalFree(g_pModuleData);
		g_pModuleData = NULL;
	} // end if (have module data)
	
	if (g_pszTestNetRootPath != NULL)
	{
		LocalFree(g_pszTestNetRootPath);
		g_pszTestNetRootPath = NULL;
	} // end if (allocated string)
	
	return (hr);
} // Cleanup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#undef DEBUG_SECTION
#define DEBUG_SECTION	"DoStartSearchForSession()"
//==================================================================================
// DoStartSearchForSession
//----------------------------------------------------------------------------------
//
// Description: Starts searching for a control layer session.  If we find one, it
//				will be joined automatically.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT DoStartSearchForSession(void)
{
	HRESULT						hr;
	TNJOINCONTROLSESSIONDATA	tnjcsdata;
	char*						pszErrToSrcPathSearch = NULL;
	DWORD						dwNumFields;
	char*						pszErrToSrcPathReplace = NULL;
	
	g_pTNSlave = new (CTNSlave);
	if (g_pTNSlave == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)
	
	ZeroMemory(&tnjcsdata, sizeof(TNJOINCONTROLSESSIONDATA));
	tnjcsdata.dwSize = sizeof (TNJOINCONTROLSESSIONDATA);
	tnjcsdata.dwAPIVersion = CURRENT_TNCONTROL_API_VERSION;
	tnjcsdata.dwMode = g_dwMode;
	tnjcsdata.pModuleID = &(g_pModuleData->info.moduleid);
	tnjcsdata.pszTestNetRootPath = g_pszTestNetRootPath;
	tnjcsdata.pszSessionFilter = g_pszSessionFilter;
	tnjcsdata.dwControlMethodID = g_dwUseControlMethodID;
	switch (g_dwUseControlMethodID)
	{
	case TN_CTRLMETHOD_TCPIP_OPTIMAL:
	case TN_CTRLMETHOD_TCPIP_WINSOCK1:
		tnjcsdata.dwMethodFlags = 0;
		tnjcsdata.pvMethodData = NULL;
		tnjcsdata.dwMethodDataSize = 0;
		break;
		
	default:
		DPL(0, "WARNING: Unknown control method (%i)!", 1, g_dwUseControlMethodID);
		
		tnjcsdata.dwMethodFlags = 0;
		tnjcsdata.pvMethodData = NULL;
		tnjcsdata.dwMethodDataSize = 0;
		break;
	} // end switch (on control method)
	
	if (g_fUseTimelimit)
		tnjcsdata.dwTimelimit = g_dwTimelimit;
	else
	{
		// Default timelimit for all modes is 0
		tnjcsdata.dwTimelimit = 0;
	} // end else (not using timelimit)
	
	tnjcsdata.pszMetaMasterAddress = g_pszMetaMastersAddress;
	tnjcsdata.pszMasterAddress = g_pszMastersAddress;
	tnjcsdata.hUserCancelEvent = g_hUserCancelEvent; //BUGBUG duplicate handle?
	tnjcsdata.apfnLoadTestTable = &(g_pModuleData->funcs.pfnLoadTestTable);
	tnjcsdata.dwNumLoadTestTables = 1;
	tnjcsdata.pfnLogString = NULL;
	tnjcsdata.pfnDoStartup = g_pModuleData->funcs.pfnDoStartup;
	tnjcsdata.pfnInitializeTesting = g_pModuleData->funcs.pfnInitializeTesting;
	tnjcsdata.pfnCleanupTesting = g_pModuleData->funcs.pfnCleanupTesting;
	tnjcsdata.pfnGetTestingWindow = NULL;
	tnjcsdata.pfnAddImportantBinaries = g_pModuleData->funcs.pfnAddImportantBinaries;
	tnjcsdata.pfnChangeBinarySet = g_pModuleData->funcs.pfnChangeBinarySet;
	tnjcsdata.hCompletionOrUpdateEvent = g_wtd.hControlLayerEvent;
	tnjcsdata.pszSavedBinsDirPath = g_pszModuleSavedBinsPath;
	tnjcsdata.pFailureBreakRules = NULL;
	tnjcsdata.pszFailureMemDumpDirPath = NULL;
	tnjcsdata.fLogToDB = FALSE;
	//tnjcsdata.fInitializeCOMInTestThread = FALSE;
	if (g_dwModCtrlLayerFlags & TNCLR_INITIALIZE_COM_FOR_TESTTHREAD)
		tnjcsdata.fInitializeCOMInTestThread = TRUE;
	tnjcsdata.fPromptTestThreadDieTimeout = FALSE;
	tnjcsdata.pszErrToSrcFilepath = NULL;
	tnjcsdata.pszErrToSrcPathSearch = pszErrToSrcPathSearch;
	tnjcsdata.pszErrToSrcPathReplace = pszErrToSrcPathReplace;
	tnjcsdata.pszDocFilepath = NULL;
	tnjcsdata.fGetNetStatInfoOnFailure = FALSE;
	
	// This should always return E_PENDING if successful
	hr = g_pTNSlave->StartJoiningControlSession(&tnjcsdata);
	if (hr != E_PENDING)
	{
		DPL(0, "StartJoiningControlSession failed!", 0);
		
		if (hr == S_OK)
			hr = E_FAIL;
		
		goto DONE;
	} // end if (join control session failed)
	
	hr = S_OK; // pending is what we want
	g_fControlInitialized = TRUE;
	
	
DONE:
	
	if (pszErrToSrcPathSearch != NULL)
	{
		LocalFree(pszErrToSrcPathSearch);
		pszErrToSrcPathSearch = NULL;
	} // end if (have search string)
	
	return (hr);
} // DoStartSearchForSession
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"StartWorkerThread()"
//==================================================================================
// StartWorkerThread
//----------------------------------------------------------------------------------
//
// Description: Creates the events and thread used for statistics updating and
//				other miscellaneous chores we don't want to do in the main
//				window thread.
//
// Arguments:
//	LPPROPSHEETPAGE lpPropPages		Pointer to property sheet pages for thread to
//									use.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT StartWorkerThread(void)
{
	DWORD	dwThreadID;
	
	
	//BUGBUG all functions like this one will leak events on failure. fix
	
	ZeroMemory(&g_wtd, sizeof (WORKERTHREADDATA));
	
	// Create kill thread manual reset event
	g_wtd.hKillThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_wtd.hKillThreadEvent == NULL)
		return (GetLastError());
	
	// Create close control session manual reset event
	g_wtd.hCloseControlSessionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_wtd.hCloseControlSessionEvent == NULL)
		return (GetLastError());
	
	// Create control layer auto reset event
	g_wtd.hControlLayerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_wtd.hControlLayerEvent == NULL)
		return (GetLastError());
	
	g_wtd.hThread = CreateThread(NULL, 0, WorkerThreadProc,
		&g_wtd, 0, &dwThreadID);
	if (g_wtd.hThread == NULL)
		return (GetLastError());
	
	return (S_OK);
} // StartWorkerThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"KillWorkerThread()"
//==================================================================================
// KillWorkerThread
//----------------------------------------------------------------------------------
//
// Description: Shuts down the worker thread and releases the events it used.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT KillWorkerThread(void)
{
	// Close all items associated with the thread
	if (g_wtd.hThread != NULL)
	{
		if (g_wtd.hKillThreadEvent == NULL) // ack, we won't be able to tell it to die
			return (E_FAIL);
		
		if (! SetEvent(g_wtd.hKillThreadEvent))
			return (E_FAIL);
		
		switch (WaitForSingleObject(g_wtd.hThread, TIMEOUT_DIE_WORKERTHREAD))
		{
		case WAIT_OBJECT_0:
			// what we want
			break;
		case WAIT_TIMEOUT:
			DPL(0, "Waited %i ms for worker thread to die but it didn't (control layer may be taking a long time to close)!",
				1, TIMEOUT_DIE_WORKERTHREAD);
			
			
#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
			DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT
			
			break;
		default:
			DPL(0, "Got unexpected return code from WaitForSingleObject on the worker thread!", 0);
			break;
		} // end switch (on result of waiting for thread to die)
		
		CloseHandle(g_wtd.hThread);
		g_wtd.hThread = NULL;
	} // end if (the send thread exists)
	
	if (g_wtd.hKillThreadEvent != NULL)
	{
		CloseHandle(g_wtd.hKillThreadEvent);
		g_wtd.hKillThreadEvent = NULL;
	} // end if (have event)
	
	if (g_wtd.hCloseControlSessionEvent != NULL)
	{
		CloseHandle(g_wtd.hCloseControlSessionEvent);
		g_wtd.hCloseControlSessionEvent = NULL;
	} // end if (have event)
	
	if (g_wtd.hControlLayerEvent != NULL)
	{
		CloseHandle(g_wtd.hControlLayerEvent);
		g_wtd.hControlLayerEvent = NULL;
	} // end if (have event)
	
	return (S_OK);
} // KillWorkerThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#undef DEBUG_SECTION
#define DEBUG_SECTION	"WorkerThreadProc()"
//==================================================================================
// WorkerThreadProc
//----------------------------------------------------------------------------------
//
// Description: Updates the stats when kicked (by setting the update stats event),
//				monitors the control session and shuts it down if requested, etc.
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to an WORKERTHREADDATA pointer.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI WorkerThreadProc(LPVOID lpvParameter)
{
	HRESULT					hr = S_OK;
	PWORKERTHREADDATA		pThreadData = (PWORKERTHREADDATA) lpvParameter;
	HANDLE					waitarray[4];
	//HWND					hCurrentPropPageWnd = NULL;
	DWORD					dwNumFirings = 0;
	PTNMACHINEINFO			pInfo = NULL;
	PTNTESTSTATS			pNewStats = NULL;
	char					szTemp[MAX_STRING];
	int						iNumCases = 0;
	int						i;
	char*					pszID = NULL;
	char*					pszName = NULL;
	HWND					hCurrentPropPageWnd = NULL;
	
	
	DPL(1, "Starting up.", 0);
	
	if (lpvParameter == NULL)
	{
		DPL(0, "Got passed invalid startup data!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed invalid startup data)
	
	if (pThreadData->hKillThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (event doesn't exist)
	
	if (pThreadData->hCloseControlSessionEvent == NULL)
	{
		DPL(0, "Close control session event doesn't exist!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (event doesn't exist)
	
	if (pThreadData->hControlLayerEvent == NULL)
	{
		DPL(0, "Control layer event doesn't exist!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (event doesn't exist)
	
	waitarray[0] = pThreadData->hKillThreadEvent;
	waitarray[1] = pThreadData->hCloseControlSessionEvent;
	waitarray[2] = pThreadData->hControlLayerEvent;
	waitarray[3] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array
	
	do
	{
		switch (WaitForMultipleObjects(3, waitarray, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			// Time to die.
			hr = S_OK;
			goto DONE;
			break;
			
		case WAIT_OBJECT_0 + 1:
			// The main thread wants us to close the control session for him.
			// The main thread can't do it himself because he still needs to
			// process windows messages.
			
			// BUGBUG crappy race checking, we have to do this better
			if (g_fControlInitialized)
			{
				g_fControlInitialized = FALSE; // do it now, to prevent a race
				
				xLog(g_hLog, XLL_INFO, "Leaving control session.");
				
				hr = g_pTNSlave->LeaveControlSession();
				
				delete (g_pTNSlave);
				g_pTNSlave = NULL;
				if (hr != S_OK)
				{
					DPL(0, "Closing control object failed!", 0);
					goto DONE;
				} // end if (closing control object failed)
			} // end if (control initialized)
			else
			{
				DPL(0, "WARNING: Told to shutdown control session but it was already gone!", 0);
			} // end else (control not initialized)
			
			break;
			
		case WAIT_OBJECT_0 + 2:
			// The control layer did something significant.  If this is the first
			// time the event is fired, it means the session has been successfully
			// created/joined/whatever.
			
			dwNumFirings++;
			
			if (dwNumFirings == 1)
			{
				// Joining the session was an asynchronous operation, so it's
				// possible it failed.  Check to see if we made it into a
				// session.
				// If we're in documentation mode, though, we don't actually
				// join a session, so assume success.
				
				if (g_pTNSlave->IsInSession())
				{
					pInfo = g_pTNSlave->GetMachineInfo();
					if (pInfo == NULL)
					{
						DPL(0, "Couldn't get machine info!", 0);
						hr = E_FAIL;
						goto DONE;
					} // end if (couldn't get info)
					
					g_dwSessionID = g_pTNSlave->GetSessionID();
					
					xLog(g_hLog, XLL_INFO, "Successfully joined control session ID %010u.", g_dwSessionID);
					
					hr = g_pTNSlave->GetModuleString(szTemp);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't get module string!", 0);
						goto DONE;
					} // end if (couldn't get module string)
					
					// Load symbols for the binaries the module requested
					hr = pInfo->m_binaries.LoadSymbolsForFirstBinaries();
					if (hr != S_OK)
					{
						DPL(0, "Couldn't load symbols for first instance of module's important binaries!", 0);
						goto DONE;
					} // end if (loading binary symbols failed)
					
				} // end if (we successfully joined a session)
				else
				{
					xLog(g_hLog, XLL_FAIL, "Failed to join session!");
					break; // get out of the switch statement and go to sleep again
				} // end else (we failed to join a session)
				
				xLog(g_hLog, XLL_INFO, SESSION_DIVIDER_STRING);

				pNewStats = g_pTNSlave->GetTotalStats();
				if (pNewStats == NULL)
				{
					DPL(0, "Couldn't get the slave's total stats!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (getting the stats failed)
				
				pNewStats->Lock();
	
			} // end if (this is the first control layer event firing)
			else
			{
				pNewStats = g_pTNSlave->GetTotalStats();
				if (pNewStats == NULL)
				{
					DPL(0, "Couldn't get the slave's total stats!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (getting the stats failed)
				
				pNewStats->Lock();
			} // end else (this is a stats update)
			
#ifdef _XBOX // no window logging supported
#pragma TODO(tristanj, "Need non-GDI way to show total complete")
#endif // XBOX
			
#ifdef _XBOX // no window logging supported
#pragma TODO(tristanj, "Need non-GDI way to show successes")
#endif // XBOX
			
#ifdef _XBOX // no window logging supported
#else // ! XBOX
#pragma TODO(tristanj, "Need non-GDI way to show failures")
#endif // XBOX
			
#ifdef _XBOX // no window logging supported
#pragma TODO(tristanj, "Need non-GDI way to show warnings")
#endif // XBOX
			
			pNewStats->Unlock();
			
			// Check if we're done
			if (g_pTNSlave->IsTestingComplete())
			{
				xLog(g_hLog, XLL_INFO, SESSION_DIVIDER_STRING);
			} // end if (testing is done)
			break;
			
		default:
			DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
			hr = E_FAIL;
			break;
		} // end switch (on wait return)
	} // end do
	while (hr == S_OK);
	
	
DONE:
	
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "WorkerThreadProc failed with %u", hr);
	} // end if (quitting with a failure)
	
	
	DPL(1, "Exiting.", 0);
	
	return (hr);
} // WorkerThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


//==================================================================================
// GenerateSlaveCommandLine
//----------------------------------------------------------------------------------
//
// Description: Generates a fake command-line to pass to the TestNet functions
//
// Arguments:
//	int           *argc				Address of int to hold num of params
//	char const    *szModuleRoot		String containing the module to use
//	char const 	  *szScriptRoot		String containing the script to use
// Returns: Array of strings holding individual parts of generated command-line
//==================================================================================
char ** WINAPI GenerateSlaveCommandLine(int *pargc, char const * szModuleRoot, char const * szScriptRoot)
{
	int nParamCount = 1, nCurrentParam = 0;
	char **argv = NULL;

	*szModuleRoot ? ++nParamCount : 0;
	*szScriptRoot ? ++nParamCount : 0;

	argv = new LPSTR[nParamCount];

	argv[nCurrentParam] = new CHAR[strlen("tnshell") + 1];
	strcpy(argv[nCurrentParam], "tnshell");
	++nCurrentParam;

	if(*szModuleRoot)
	{
		argv[nCurrentParam] = new CHAR[strlen("/module:") + strlen(szModuleRoot) + 1];
		strcpy(argv[nCurrentParam], "/module:");
		strcat(argv[nCurrentParam], szModuleRoot);
		++nCurrentParam;
	}
	
	if(*szScriptRoot)
	{
		argv[nCurrentParam] = new CHAR[strlen("/script:") + strlen(szScriptRoot) + 1];
		strcpy(argv[nCurrentParam], "/script:");
		strcat(argv[nCurrentParam], szScriptRoot);
		++nCurrentParam;
	}

	*pargc = nParamCount;

	return (argv);
}

//==================================================================================
// ReleaseGeneratedArguments
//----------------------------------------------------------------------------------
//
// Description: Deallocates converted arguments
//
// Arguments:
//	char   **argv		Array of char ptrs to be deallocated
//	int    argc			Number of arguments to deallocate
// Returns: void
//==================================================================================
void WINAPI ReleaseGeneratedArguments(char **argv, int argc)
{
	int i;

	for(i = 0; i < argc; i++)
		delete argv[i];

	delete argv;
}

//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry
//
// Arguments:
//	HINSTANCE	hInstance		Handle to module
//	DWORD		dwReason		Indicates the reason for calling the function
//	LPVOID		lpContext		reserved
// Returns:
//	TRUE on success
//==================================================================================
BOOL WINAPI DllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
	// We'll initialize/delete the global critical section here
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		XnetInitialize(NULL, TRUE);
		InitializeCriticalSection(&g_csGlobal);
		break;
	case DLL_PROCESS_DETACH:
		XnetCleanup();
		DeleteCriticalSection(&g_csGlobal);
		break;
	default:
		break;
	}

    return TRUE;
}

//==================================================================================
// StartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//	HANDLE		hLog			Handle to logging subsystem
// Returns:
//	none
//==================================================================================
VOID WINAPI StartTest(IN HANDLE  hLog)
{
	CCommandLine	cmdline;
	HRESULT			hr = S_OK;
	CHAR			**argv = NULL;
	INT				argc = 0;
	DWORD			dwError = 0;

	EnterCriticalSection(&g_csGlobal);

	g_hLog = hLog;

    // Set the component and subcomponent
    xSetComponent(hLog, "XAPI(S)", "DirectPlay(S-x)");

    // Create the stop event object
	g_hUserCancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hUserCancelEvent == NULL)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't create user cancel event!");
		hr = GetLastError();
		goto DONE;
	} // end if (couldn't create event)


	hr = cmdline.Initialize(CMDLINE_FLAG_DONTHANDLEDEBUGSWITCH,
		c_aHandledItems,
		((sizeof (c_aHandledItems)) / (sizeof (CMDLINEHANDLEDITEM))));
	if (hr != CMDLINE_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Failed to initialize command line object!");
		goto DONE;
	} // end if (failed to parse command line)
	
	argv = GenerateSlaveCommandLine(&argc, "1sttest", "t_main");
#pragma TODO(tristanj, "Need to parse the INI files for module and script")

	hr = cmdline.Parse(argc, argv);
	if (hr != CMDLINE_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Failed to parse command line!");
		goto DONE;
	} // end if (failed to parse command line)
	
	hr = AnalyzeCommandLine(&cmdline);
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Analyzing command line failed!");
		goto DONE;
	} // end if (couldn't analyze command line)
	OutputDebugString(L"Analyzed command line\r\n");
	
	// We don't really support this yet... this is really a place holder
	hr = GetShellBinariesInfo();
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Getting info for important shell binaries failed!");
		goto DONE;
	} // end if (couldn't get shell binaries info)
	
	// Initialize the module we'll be testing with
	hr = InitializeModule(&cmdline);
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Couldn't initialize module!");
		goto DONE;
	} // end if (couldn't initialize module)
	
	// Start up worker thread
	hr = StartWorkerThread();
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Starting worker thread failed!");
		goto DONE;
	} // end if (starting worker thread failed)
	
	xLog(g_hLog, XLL_INFO, "Worker thread started");
	
	// Start looking for a master session
	hr = DoStartSearchForSession();
	if (hr != S_OK)
	{
		xLog(g_hLog, XLL_FAIL, "Session search failed");
		goto DONE;
	} // end if (creating or joining session failed)
	
	xLog(g_hLog, XLL_INFO, "Session search succeeded");

DONE:

	// If we created the fake command-line arguments, release them
	if(argv)
	{
		ReleaseGeneratedArguments(argv, argc);
		argv = NULL;
	}

	// Clean up everything
	Cleanup();

	// Let other threads in
	LeaveCriticalSection(&g_csGlobal);
}


//==================================================================================
// EndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//	none
// Returns:
//	none
//==================================================================================
VOID WINAPI EndTest()
{
	// We already cleaned up everything at the end of StartTest
}
