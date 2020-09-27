//==================================================================================
// Includes
//==================================================================================
#define _WIN32_DCOM // so we can use CoInitializeEx.  requires DCOM95 on Win95
#define INCL_WINSOCK_API_TYPEDEFS 1 // includes winsock2 fn proto's, for getprocaddress

#ifndef _XBOX
#include <winsock2.h>
#else // ! XBOX
#include <winsockx.h>		// Needed for XnetInitialize
#endif // XBOX

#include <windows.h>
#include <windowsx.h>

#include <mmsystem.h>

// We set the version to 4 so we don't go getting extra elements in our RASCONN
// structure (otherwise we might build with a larger size that earlier versions of
// RAS barf on)
#ifdef WINVER
#undef WINVER
#endif // WINVER
#define WINVER	0x0400
#include <ras.h>
#include <raserror.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"
#include "resource.h"

#include "script.h"
#include "faultsim.h"
#include "faultsimimtest.h"

#include "executor.h"





//==================================================================================
// Defines
//==================================================================================
//BUGBUG make customizable
#define TIMEOUT_DIE_TESTTHREAD			40000 // in milliseconds

#define MAX_WAITFOREVENTORCANCEL_TIME	600000 // 10 minutes


#define WM_MYCLOSEDIALOG				(WM_USER + 1)




//==================================================================================
// DCOM function type definitions
//==================================================================================
typedef HRESULT	(WINAPI *PCOINITIALIZEEXPROC)	(LPVOID, DWORD);




//==================================================================================
// Local Prototypes
//==================================================================================
INT_PTR CALLBACK PromptAllDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::CTNExecutorPriv()"
//==================================================================================
// CTNExecutorPriv constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNExecutorPriv object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNExecutorPriv::CTNExecutorPriv(void)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNExecutorPriv));


	// protected
	this->m_dwMode = 0;
	ZeroMemory(&(this->m_moduleID), sizeof (TNMODULEID));

	this->m_pOwningSlave = NULL;

#ifndef _XBOX // no IPC supported
	this->m_pOwningLeech = NULL;
#endif // ! XBOX

	ZeroMemory(&(this->m_id), sizeof (TNCTRLMACHINEID));
	this->m_hUserCancelEvent = NULL;

	this->m_dwTestThreadID = 0;
	this->m_fDeadlockCheck = TRUE;
	this->m_dwLastTestthreadPing = 0;


	// private
	this->m_hTestThread = NULL;
	this->m_hKillTestThreadEvent = NULL;
	this->m_hNewTestEvent = NULL;

	this->m_pTest = NULL;

	this->m_fInitCOMForTestThread = FALSE;
	this->m_pfnInitializeTesting = NULL;
	this->m_pfnCleanupTesting = NULL;
	this->m_pfnGetTestingWindow = NULL;

	this->m_hErrToSrcFile = INVALID_HANDLE_VALUE;
	this->m_hDocFile = INVALID_HANDLE_VALUE;

	this->m_fPromptTestThreadDieTimeout = FALSE;
} // CTNExecutorPriv::CTNExecutorPriv
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::~CTNExecutorPriv()"
//==================================================================================
// CTNExecutorPriv destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNExecutorPriv object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNExecutorPriv::~CTNExecutorPriv(void)
{
	HRESULT		hr;


	DPL(9, "this = %x", 1, this);

	hr = this->Cleanup();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't clean up!  %e", 1, hr);
	} // end if (couldn't clean up)
} // CTNExecutorPriv::~CTNExecutorPriv
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::InitializeBySlave()"
//==================================================================================
// CTNExecutorPriv::InitializeBySlave
//----------------------------------------------------------------------------------
//
// Description: Prepares this object for usage.  Retrieves machine information.
//				This is only called when initializing a slave's executor.  Leeches
//				currently initialize their executors manually.
//
// Arguments:
//	PTNEXECUTORINITBYSLAVEDATA pTNeibsd		Pointer to parameter block to use when
//											initializing this object.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::InitializeBySlave(PTNEXECUTORINITBYSLAVEDATA pTNeibsd)
{
	HRESULT		hr;
	DWORD		dwTemp;
	char		szTemp[1024];

	this->m_pOwningSlave = pTNeibsd->pOwningSlave;

	switch (pTNeibsd->dwMode)
	{
		case TNMODE_API:
			DPL(1, "Using API mode.", 0);
		break;

		case TNMODE_STRESS:
			DPL(1, "Using stress mode.", 0);
		  break;

		case TNMODE_POKE:
			DPL(1, "Using poke mode.", 0);
		  break;

		case TNMODE_PICKY:
			DPL(1, "Using picky mode.", 0);
		  break;

		case TNMODE_DOCUMENTATION:
			DPL(1, "Using documentation mode.", 0);
		  break;
	} // end switch (on mode)
	this->m_dwMode = pTNeibsd->dwMode;

	CopyMemory(&(this->m_moduleID), pTNeibsd->pModuleID, sizeof (TNMODULEID));

	hr = this->m_info.LoadLocalMachineInfo(pTNeibsd->pszTestNetRootPath,
											pTNeibsd->pfnAddImportantBinaries,
											pTNeibsd->iMultiInstanceNum);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't load local machine info!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't load local info)

	this->m_pfnInitializeTesting = pTNeibsd->pfnInitializeTesting;
	this->m_pfnCleanupTesting = pTNeibsd->pfnCleanupTesting;
	this->m_pfnGetTestingWindow = pTNeibsd->pfnGetTestingWindow;
	this->m_fInitCOMForTestThread = pTNeibsd->fInitializeCOMInTestThread;
	this->m_fPromptTestThreadDieTimeout = pTNeibsd->fPromptTestThreadDieTimeout;


	hr = ScriptLoadTestTable(&(this->m_testtable));
	if (hr != S_OK)
	{
		DPL(0, "Loading built-in script functions failed!", 0);
		goto ERROR_EXIT;
	} // end if (loading built-in script functions failed)


	for(dwTemp = 0; dwTemp < pTNeibsd->dwNumLoadTestTables; dwTemp++)
	{
		hr = pTNeibsd->apfnLoadTestTable[dwTemp](&(this->m_testtable));
		if (hr != S_OK)
		{
			DPL(0, "Module's LoadTestTable callback %u failed!", 1, dwTemp);
			goto ERROR_EXIT;
		} // end if (module's callback failed)
	} // end for (each LoadTestTable callback)


	// If there's a user cancel event, duplicate it.
	if (pTNeibsd->hUserCancelEvent != NULL)
	{
		if (! DuplicateHandle(GetCurrentProcess(), pTNeibsd->hUserCancelEvent,
								GetCurrentProcess(), &(this->m_hUserCancelEvent),
								0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hr = GetLastError();
			DPL(0, "Couldn't duplicate user cancel event handle (%x)!",
				2, pTNeibsd->hUserCancelEvent);
			goto ERROR_EXIT;
		} // end if (couldn't duplicate handle)
	} // end if (there's a user cancel event)


#ifndef _XBOX // ErrToSrc not supported
	// If there's a path to use for ErrToSrc data, use it.
	if (pTNeibsd->pszErrToSrcFilepath != NULL)
	{
		DPL(1, "Creating ErrToSrc file \"%s\".", 1, pTNeibsd->pszErrToSrcFilepath);

		if ((pTNeibsd->pszErrToSrcPathSearch != NULL) &&
			(pTNeibsd->pszErrToSrcPathReplace != NULL))
		{
			this->m_pszErrToSrcPathSearch = (char*) LocalAlloc(LPTR, strlen(pTNeibsd->pszErrToSrcPathSearch) + 1);
			if (this->m_pszErrToSrcPathSearch == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto ERROR_EXIT;
			} // end if (couldn't allocate memory)

			strcpy(this->m_pszErrToSrcPathSearch, pTNeibsd->pszErrToSrcPathSearch);


			this->m_pszErrToSrcPathReplace = (char*) LocalAlloc(LPTR, strlen(pTNeibsd->pszErrToSrcPathReplace) + 1);
			if (this->m_pszErrToSrcPathReplace == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto ERROR_EXIT;
			} // end if (couldn't allocate memory)

			strcpy(this->m_pszErrToSrcPathReplace, pTNeibsd->pszErrToSrcPathReplace);


			DPL(1, "Will replace ErrToSrc paths containing \"%s\" with \"%s\".",
				2, this->m_pszErrToSrcPathSearch, this->m_pszErrToSrcPathReplace);
		} // end if (search/replace ErrToSrc)

		hr = FileCreateAndOpenFile(pTNeibsd->pszErrToSrcFilepath, FALSE, FALSE,
									FALSE, &(this->m_hErrToSrcFile));
		if (hr != S_OK)
		{
			DPL(0, "Couldn't create ErrToSrc file \"%s\"!",
				1, pTNeibsd->pszErrToSrcFilepath);
			goto ERROR_EXIT;
		} // end if (couldn't create file)


		StringGetCurrentDateStr(szTemp);
		StringPrepend(szTemp, "// ErrToSrc starting at ");

		FileWriteLine(this->m_hErrToSrcFile,
					"//==================================================================================");
		FileWriteLine(this->m_hErrToSrcFile, szTemp);
		FileWriteLine(this->m_hErrToSrcFile,
					"//==================================================================================");
		FileWriteLine(this->m_hErrToSrcFile, ""); // double space
	} // end if (should write ErrToSrc data)
#endif // ! XBOX

#ifndef _XBOX // Documentation mode not supported
	// If there's a path to use for documentation data, use it.  Make sure
	// the mode is appropriate, regardless.
	if (pTNeibsd->dwMode == TNMODE_DOCUMENTATION)
	{
		if (pTNeibsd->pszDocFilepath == NULL)
		{
			DPL(0, "In documentation mode but no filepath was specified!", 0);
			hr = ERROR_INVALID_PARAMETER;
			goto ERROR_EXIT;
		} // end if (no documentation filepath)


#ifndef _XBOX // Removed unnecessary logging
		wsprintf(szTemp, "Creating documentation file \"%s\"",
				pTNeibsd->pszDocFilepath);

		DPL(1, szTemp, 0);
		this->LogInternal(TNLST_CONTROLLAYER_INFO, szTemp);
#endif // ! XBOX


		hr = FileCreateAndOpenFile(pTNeibsd->pszDocFilepath, FALSE, FALSE,
									FALSE, &(this->m_hDocFile));
		if (hr != S_OK)
		{
			DPL(0, "Couldn't create documentation file \"%s\"!",
				1, pTNeibsd->pszDocFilepath);
			goto ERROR_EXIT;
		} // end if (couldn't create file)


		StringGetCurrentDateStr(szTemp);
		StringPrepend(szTemp, "// Documentation starting at ");

		FileWriteLine(this->m_hDocFile,
					"//==================================================================================");
		FileWriteLine(this->m_hDocFile, szTemp);
		FileWriteLine(this->m_hDocFile,
					"//==================================================================================");
		FileWriteLine(this->m_hDocFile, ""); // double space


		// We need to kick off the test thread right away, because it's the
		// worker thread which generates the documentation.
		hr = this->StartTestThread();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't start test thread!", 0);
			goto ERROR_EXIT;
		} // end if (couldn't start test thread)
	} // end if (in documentation mode)
	else
	{
		if (pTNeibsd->pszDocFilepath != NULL)
		{
			DPL(0, "Not in documentation mode but a filepath (\"%s\") was specified!",
				1, pTNeibsd->pszDocFilepath);
			hr = ERROR_INVALID_PARAMETER;
			goto ERROR_EXIT;
		} // end if (documentation filepath specified)
	} // end else (not in documentation mode)
#endif // ! XBOX

	return (S_OK);


ERROR_EXIT:

	if (this->m_hUserCancelEvent != NULL)
	{
		CloseHandle(this->m_hUserCancelEvent);
		this->m_hUserCancelEvent = NULL;
	} // end if (duplicated handle)

	if (this->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hDocFile);
		this->m_hDocFile = INVALID_HANDLE_VALUE;
	} // end if (opened doc file)

	if (this->m_hErrToSrcFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hErrToSrcFile);
		this->m_hErrToSrcFile = INVALID_HANDLE_VALUE;
	} // end if (opened errtosrc file)

	if (this->m_pszErrToSrcPathSearch != NULL)
	{
		LocalFree(this->m_pszErrToSrcPathSearch);
		this->m_pszErrToSrcPathSearch = NULL;
	} // end if (allocated string)

	if (this->m_pszErrToSrcPathReplace != NULL)
	{
		LocalFree(this->m_pszErrToSrcPathReplace);
		this->m_pszErrToSrcPathReplace = NULL;
	} // end if (allocated string)

	// We may have started up the test thread, so ensure it's gone.  Ignore errors.
	this->KillTestThread(FALSE);

	return (hr);
} // CTNExecutorPriv::InitializeBySlave
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::Cleanup()"
//==================================================================================
// CTNExecutorPriv::Cleanup
//----------------------------------------------------------------------------------
//
// Description: Cleans up after this object.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::Cleanup(void)
{
	HRESULT			hr;
	int				i;
#ifndef _XBOX // no IPC supported
	PTNLEECH		pLeech;
#endif // ! XBOX
	PTNFAULTSIM		pFaultSim = NULL;


#pragma TODO(vanceo, "Remove/stop ongoing tests?")

	hr = this->KillTestThread(FALSE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't kill test thread!  %e", 1, hr);
	} // end if (couldn't kill test thread)


#ifndef _XBOX // no IPC supported
	i = this->m_leeches.Count();
	while (i > 0)
	{
		pLeech = (PTNLEECH) this->m_leeches.GetItem(0);
		if (pLeech == NULL)
		{
			DPL(0, "Couldn't get first leech item (there should be %i remaining)!",
				1, i);
			break;
		} // end if (couldn't get item)

		hr = this->m_leeches.RemoveLeech(pLeech);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove leech %x!  %e", 2, pLeech, hr);
			break;
		} // end if (couldn't remove leech)
		pLeech = NULL;

		i--;
	} // end while (there are still leeches)
#endif // ! XBOX

	i = this->m_faultsims.Count();
	while (i > 0)
	{
		pFaultSim = (PTNFAULTSIM) this->m_faultsims.PopFirstItem();
		if (pFaultSim == NULL)
		{
			DPL(0, "Couldn't pop first fault sim item (there should be %i remaining)!",
				1, i);
			break;
		} // end if (couldn't pop item)

		hr = pFaultSim->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release fault sim %x!  %e", 2, pFaultSim, hr);
			break;
		} // end if (couldn't release fault sim)

		pFaultSim->m_dwRefCount--;
		if (pFaultSim->m_dwRefCount == 0)
		{
			delete (pFaultSim);
		} // end if (can delete object)
		else
		{
			DPL(0, "WARNING: Can't delete fault simulator %x, its refcount is %u!",
				2, pFaultSim, pFaultSim->m_dwRefCount);
		} // end else (can't delete object)

		pFaultSim = NULL;

		i--;
	} // end while (there are still fault sims)


	// Remove all results.
	hr = this->m_results.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove all results!  %e", 1, hr);
	} // end if (couldn't remove all items)


	// Reset the members, just like in the constructor

	this->m_dwMode = 0;
	ZeroMemory(&(this->m_moduleID), sizeof (TNMODULEID));

	if (this->m_pTest != NULL)
	{
		this->m_pTest->m_dwRefCount--;
		if (this->m_pTest->m_dwRefCount == 0)
		{
			DPL(0, "Deleting current test %x.", 1, this->m_pTest);
			delete (this->m_pTest);
		} // end if (refcount hit 0)
		else
		{
			DPL(0, "Not deleting current test %x, its refcount is %u.",
				2, this->m_pTest, this->m_pTest->m_dwRefCount);
		} // end else (refcount hasn't hit 0)

		this->m_pTest = NULL;
	} // end if (still have current test)


	// Remove all queued tests.
	hr = this->m_queuedtests.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove all queued tests!  %e", 1, hr);
	} // end if (couldn't remove all items)

	// Remove all ongoing tests.
	hr = this->m_ongoingtests.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove all ongoing tests!  %e", 1, hr);
	} // end if (couldn't remove all items)


	if (this->m_hUserCancelEvent != NULL)
	{
		CloseHandle(this->m_hUserCancelEvent);
		this->m_hUserCancelEvent = NULL;
	} // end if (have cancel event)


	if (this->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hDocFile);
		this->m_hDocFile = INVALID_HANDLE_VALUE;
	} // end if (still have doc file)

	if (this->m_hErrToSrcFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->m_hErrToSrcFile);
		this->m_hErrToSrcFile = INVALID_HANDLE_VALUE;
	} // end if (still have errtosrc file)

	if (this->m_pszErrToSrcPathSearch != NULL)
	{
		LocalFree(this->m_pszErrToSrcPathSearch);
		this->m_pszErrToSrcPathSearch = NULL;
	} // end if (allocated string)

	if (this->m_pszErrToSrcPathReplace != NULL)
	{
		LocalFree(this->m_pszErrToSrcPathReplace);
		this->m_pszErrToSrcPathReplace = NULL;
	} // end if (allocated string)

	return (hr);
} // CTNExecutorPriv::Cleanup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::StartTestThread()"
//==================================================================================
// CTNExecutorPriv::StartTestThread
//----------------------------------------------------------------------------------
//
// Description: Creates the events and thread used for testing, if they don't
//				already exist.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::StartTestThread(void)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed bad pointer)


	// Create kill thread manual reset event if it doesn't exist
	if (this->m_hKillTestThreadEvent == NULL)
	{
		this->m_hKillTestThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (this->m_hKillTestThreadEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create kill thread event!", 0);
			return (hr);
		} // end if (couldn't create kill thread event)
	} // end if (kill event doesn't exist)


	// Create new test auto reset event if it doesn't exist
	if (this->m_hNewTestEvent == NULL)
	{
		this->m_hNewTestEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (this->m_hNewTestEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create new test event!", 0);
			return (hr);
		} // end if (couldn't create new test event)
	} // end if (new test event doesn't exist)


	// Create the actual thread if it doesn't exist
	if (this->m_hTestThread == NULL)
	{
		this->m_hTestThread = CreateThread(NULL, 0, ExecutorTestThreadProc,
											this, 0, &(this->m_dwTestThreadID));
		if (this->m_hTestThread == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create test thread!", 0);
			return (hr);
		} // end if (couldn't create thread)

		// The test thread is starting up, so initialize the time.
		this->m_dwLastTestthreadPing = GetTickCount();
	} // end if (kill event doesn't exist)

	return (S_OK);
} // CTNExecutorPriv::StartTestThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::KillTestThread()"
//==================================================================================
// CTNExecutorPriv::KillTestThread
//----------------------------------------------------------------------------------
//
// Description: Shuts down the test thread and releases the events it used.
//
// Arguments:
//	BOOL fKillingSelf	Only TRUE when the test thread itself is calling this
//						function.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::KillTestThread(BOOL fKillingSelf)
{
	HRESULT		hr = S_OK;
	DWORD		dwExitCode;
	DWORD		dwTimeout;
	int			iResult;


	DPL(9, "==> (%B)", 1, fKillingSelf);

	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (got passed bad pointer)


	// If the thread is initiating the shutdown, go right to the cleanup.
	if (fKillingSelf)
		goto DONE;

		
	// Close all items associated with the thread
	if (this->m_hTestThread != NULL)
	{
		// First check to see if it already died on its own
		if (! GetExitCodeThread(this->m_hTestThread, &dwExitCode))
		{
			hr = GetLastError();
			DPL(0, "Couldn't get thread exit code!", 0);
			goto DONE;
		} // end if (couldn't get the thread's exit code)

		if (dwExitCode != STILL_ACTIVE)
		{
			DPL(0, "Test thread killed itself!  %e", 1, dwExitCode);

			// Note: hr == S_OK
			goto DONE;
		} // end if (the thread must have killed itself)

		if (this->m_hKillTestThreadEvent == NULL) // ack, we won't be able to tell it to die
		{
			DPL(0, "Kill thread event doesn't exist, can't tell it to die!", 0);
			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (kill event doesn't exist)

		if (! SetEvent(this->m_hKillTestThreadEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set kill thread event!", 0);
			goto DONE;
		} // end if (couldn't set kill event)


		dwTimeout = TIMEOUT_DIE_TESTTHREAD;

RETRY:

		hr = WaitForSingleObject(this->m_hTestThread, dwTimeout);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// what we want
				hr = S_OK;
			  break;

			case WAIT_TIMEOUT:
				// Hmm, let's try getting the exit code again
				if (! GetExitCodeThread(this->m_hTestThread, &dwExitCode))
				{
					hr = GetLastError();
					DPL(0, "Couldn't get thread exit code!", 0);
					goto DONE;
				} // end if (couldn't get the thread's exit code)

				if (dwExitCode != STILL_ACTIVE)
				{
					DPL(0, "Test thread died in unusual way!  %e", 1, dwExitCode);

					// Note: hr == S_OK
					goto DONE;
				} // end if (the thread must have killed itself)


#ifndef _XBOX // removed unnecessary logging
				DPL(0, "Waited %i ms for test thread to die but it didn't (process ID = %x/%u, test thread ID %x/%u)!",
					5, dwTimeout,
					GetCurrentProcessId(), GetCurrentProcessId(),
					this->m_dwTestThreadID, this->m_dwTestThreadID);
#endif // ! XBOX


				if (this->m_fPromptTestThreadDieTimeout)
				{
					DPL(0, "Prompting user for action to take with this test thread that just won't die...", 0);

#ifndef _XBOX // no GDI supported
					iResult = MessageBox(NULL,
										"TestNet's test thread has not finished yet.  Press Abort to break into debugger, Retry to wait 10 more seconds, or Ignore to continue with your fingers crossed.\n"
										"Please see the debug spew for more information.",
										"TestNet test thread still hasn't finished!",
										MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_DEFBUTTON1);
#else // ! XBOX
#pragma TODO(tristanj, "Removed MessageBox, so hardcoded as if ABORT was returned.")
					iResult = IDABORT;
#endif // XBOX
					switch (iResult)
					{
						case IDABORT:
#ifndef _XBOX // removed unnecessary logging
							DPL(0, "User elected to DEBUGBREAK() because of test thread that hasn't finished (process ID = %x/%u, test thread ID %x/%u)!",
								4, GetCurrentProcessId(), GetCurrentProcessId(),
								this->m_dwTestThreadID, this->m_dwTestThreadID);
#endif // ! XBOX

							DEBUGBREAK();

							hr = E_FAIL;
						  break;

						case IDRETRY:
							DPL(0, "User elected to wait 10 more seconds for test thread to shut down.", 0);

							// Wait another 10 seconds.
							dwTimeout = 10000;
							goto RETRY;
						  break;
						  
						case IDIGNORE:
							DPL(0, "User elected to ignore problems shutting down test thread!  This app may hang!", 0);
							hr = S_OK;
						  break;
						  
						default:
							hr = GetLastError();
							DPL(0, "Unexpected return from MessageBox (%i)!  DEBUGBREAK()-ing.  %e",
								2, iResult, hr);

							DEBUGBREAK();

							if (hr == S_OK)
								hr = E_FAIL;
						  break;
					} // end switch (on button pressed)
				} // end if (should prompt user for action)
				else
				{
#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
					DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

					//hr = WAIT_TIMEOUT;
				} // end else (should not prompt user for action)
			  break;

			default:
				DPL(0, "Got unexpected return code (%i) from WaitForSingleObject on the test thread!",
					1, hr);
			  break;
		} // end switch (on result of waiting for thread to die)

	} // end if (the test thread exists)


DONE:

	if (this->m_hTestThread != NULL)
	{
		CloseHandle(this->m_hTestThread);
		this->m_hTestThread = NULL;
	} // end if (have handle)

	if (this->m_hKillTestThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillTestThreadEvent);
		this->m_hKillTestThreadEvent = NULL;
	} // end if (have event)

	if (this->m_hNewTestEvent != NULL)
	{
		CloseHandle(this->m_hNewTestEvent);
		this->m_hNewTestEvent = NULL;
	} // end if (have event)

	this->m_dwLastTestthreadPing = 0;

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutorPriv::KillTestThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::HandleNewTest()"
//==================================================================================
// CTNExecutorPriv::HandleNewTest
//----------------------------------------------------------------------------------
//
// Description: Handles a new test to run.
//
// Arguments:
//	DWORD dwUniqueTestID		Unique ID of test to run.
//	char* pszCaseID				Module defined case ID of the test.
//	char* pszInstanceID			User defined instance ID of the test.
//	int iNumMachines			Total number of testers for the instance
//	PTNCTRLMACHINEID aTesters	Array of tester IDs in tester number order.
//	PVOID pvInputData			Pointer to input data, if any.
//	DWORD dwInputDataSize		Pointer to input data size, if any.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::HandleNewTest(DWORD dwUniqueTestID,
									char* pszCaseID, char* pszInstanceID,
									int iNumMachines, PTNCTRLMACHINEID aTesters,
									PVOID pvInputData, DWORD dwInputDataSize)
{
	HRESULT				hr = S_OK;
	PTNTESTINSTANCES	pTest = NULL;
	PTNTESTTABLECASE	pCase = NULL;


	pCase = this->m_testtable.GetTest(pszCaseID);
	if (pCase == NULL)
	{
		DPL(0, "Couldn't find test case ID %s in table!", 1, pszCaseID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find that test in the table)

	//BUGBUG do we want to get the TNTIO_xxx options from the master?
	pTest = new (CTNTestInstanceS)(dwUniqueTestID,
									pCase,
									pszInstanceID,
									iNumMachines,
									aTesters,
									//0,
									NULL);
	if (pTest == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pTest->m_dwRefCount++; // we're using the object

	if (dwInputDataSize > 0)
	{
		hr = pTest->SetInputData(pvInputData, dwInputDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't set the input data for the test!", 0);
			goto DONE;
		} // end if (couldn't set the test's input data)
	} // end if (there's input data for the test)


	// This really shouldn't be happening, because the only time we're told to
	// run a new test is when we don't have anything else to be running.  But
	// for now, just print a warning and queue it up.
	if (this->m_queuedtests.Count() > 0)
	{
		DPL(0, "WARNING: Queueing test (case \"%s\", unique ID %u) behind %i other items!",
			3, pTest->m_pCase->m_pszID, pTest->m_dwUniqueID,
			this->m_queuedtests.Count());
	} // end if (there are tests queued already)


	// Add it to the queue to be run.
	hr = this->m_queuedtests.Add(pTest);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add test %x to queue!", 1, pTest);
		goto DONE;
	} // end if (couldn't add it to the list)

	// Trigger the event to wake up the testing thread.
	if (! SetEvent(this->m_hNewTestEvent))
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't set new test event (%x)!", 1, this->m_hNewTestEvent);
		goto DONE;
	} // end if (couldn't set event)



DONE:

	if (pTest != NULL)
	{
		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Not deleting test %x (ID %u).",
				2, pTest, pTest->m_dwUniqueID);
			delete (pTest);
			pTest = NULL;
		} // end if (can delete test)
		else
		{
			DPL(7, "Not deleting test %x (ID %u), its refcount = %u.",
				3, pTest, pTest->m_dwUniqueID, pTest->m_dwRefCount);
		} // end else (can't delete test)
	} // end if (we have the test)

	return (hr);
} // CTNExecutorPriv::HandleNewTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::HandleAnnounceResponse()"
//==================================================================================
// CTNExecutorPriv::HandleAnnounceResponse
//----------------------------------------------------------------------------------
//
// Description: Handles a test response resulting from a test announcement.
//
// Arguments:
//	DWORD dwResponseID		ID of request that asked for this test, or 0 for none.
//	DWORD dwUniqueTestID	Unique ID of test to run.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::HandleAnnounceResponse(DWORD dwResponseID, DWORD dwUniqueTestID)
{
	HRESULT				hr = S_OK;
	BOOL				fSetNewTestEvent = FALSE;
	PTNTESTINSTANCES	pTest = NULL;
	int					i;


	// Check the list of queued tests first.
	for(i = 0; i < this->m_queuedtests.Count(); i++)
	{
		pTest = (PTNTESTINSTANCES) this->m_queuedtests.GetItem(i);
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get top level ongoing test %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		pTest = pTest->GetTestWithRequestID(dwResponseID);
		if (pTest != NULL)
		{
			fSetNewTestEvent = TRUE;
			break;
		} // end if (we found it in the queue)
	} // end for (each ongoing test heirarchy)

	// If we haven't found the announcing parent test yet, check the current active
	// test hierarchy.
	if (pTest == NULL)
	{
		if (this->m_pTest != NULL)
		{
			pTest = this->m_pTest->GetTestWithRequestID(dwResponseID);
		} // end if (have a current active test)
	} // end if (haven't found test yet)

	// If we haven't found the announcing parent test yet, check the ongoing tests.
	if (pTest == NULL)
	{
		for(i = 0; i < this->m_ongoingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCES) this->m_ongoingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get top level ongoing test %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)

			pTest = pTest->GetTestWithRequestID(dwResponseID);
			if (pTest != NULL)
				break;
		} // end for (each ongoing test heirarchy)
	} // end if (have a current active test)

	// If we still haven't found the announcing parent test, something's wrong.
	if (pTest == NULL)
	{
		DPL(0, "Couldn't find test that announced using request ID %u!",
			1, dwResponseID);
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't find test)


	// If we're in Poke mode, the test with the request is the one which was
	// waiting for an ID.  Otherwise, it's the parent test of the subtest that
	// was waiting.
	if (this->m_dwMode == TNMODE_POKE)
	{
		// Make sure the test hasn't already been given an ID.
		if (pTest->m_dwUniqueID != 0xFFFFFFFF)
		{
			DPL(0, "Test to get ID %u received using request ID %u already has test ID %u!",
				3, dwUniqueTestID, dwResponseID, pTest->m_dwUniqueID);
			hr = E_FAIL;
			goto DONE;
		} // end if (didn't find test)

		// The test gets the ID.
		pTest->m_dwUniqueID = dwUniqueTestID;
	} // end if (in Poke mode)
	else
	{
		// Make sure the parent test has a valid subtest.
		if (pTest->m_pSubTest == NULL)
		{
			DPL(0, "Couldn't find subtest that should get ID (%u) received using request ID %u!",
				2, dwUniqueTestID, dwResponseID);
			hr = E_FAIL;
			goto DONE;
		} // end if (didn't find test)

		// Make sure the subtest hasn't already been given an ID.
		if (pTest->m_pSubTest->m_dwUniqueID != 0xFFFFFFFF)
		{
			DPL(0, "Subtest to get ID %u received using request ID %u already has test ID %u!",
				3, dwUniqueTestID, dwResponseID, pTest->m_pSubTest->m_dwUniqueID);
			hr = E_FAIL;
			goto DONE;
		} // end if (didn't find test)

		// The sub test gets the ID.
		pTest->m_pSubTest->m_dwUniqueID = dwUniqueTestID;
	} // end else (not in Poke mode)


	pTest->m_hrResponseResult = S_OK;

	if (fSetNewTestEvent)
	{
		pTest->m_dwRequestID = 0; // clear the request ID

		DPL(8, "Notifying test thread that test %u is now ready.",
			1, dwUniqueTestID);

		if (! SetEvent(this->m_hNewTestEvent))
		{
			hr = GetLastError();

			DPL(0, "WARNING: Couldn't set new test event (%x)!  %e",
				2, this->m_hNewTestEvent, hr);

			// Treat it as OK.  See notes below.
			hr = S_OK;
		} // end if (couldn't set event)
	} // end if (should notify test thread)
	else
	{
		DPL(8, "Notifying some waiting thread that test %u is now ready.",
			1, dwUniqueTestID);

		if (! SetEvent(pTest->m_hResponseEvent))
		{
			hr = GetLastError();

			DPL(0, "WARNING: Couldn't set announcement response event (%x)!  %e",
				2, pTest->m_hResponseEvent, hr);

			// Treat it as OK because I have seen an instance where this failed
			// for no apparent reason.  The waiting thread got triggered fine,
			// so I suspect what happened was that the waiting thead managed to
			// close the event handle before the SetEvent call officially
			// returned, and thus it ended up failing with ERROR_INVALID_HANDLE.
			hr = S_OK;
		} // end if (couldn't set event)
	} // end else (should release whomever was waiting on this)


DONE:

	return (hr);
} // CTNExecutorPriv::HandleAnnounceResponse
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::HandleSyncResponse()"
//==================================================================================
// CTNExecutorPriv::HandleSyncResponse
//----------------------------------------------------------------------------------
//
// Description: Handles a response resulting from a sync request.
//
// Arguments:
//	DWORD dwResponseID		ID of request that asked for this test, or 0 for none.
//	PVOID pvData			Pointer to incoming data from other testers.
//	DWORD dwDataSize		Size of incoming data buffer.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::HandleSyncResponse(DWORD dwResponseID, PVOID pvData,
											DWORD dwDataSize)
{
	HRESULT				hr = S_OK;
	PTNTESTINSTANCES	pTest = NULL;
	int					i;


	// Check the current active test hierarchy first
	if (this->m_pTest != NULL)
	{
		pTest = this->m_pTest->GetTestWithRequestID(dwResponseID);
	} // end if (have a current active test)

	// If we haven't found the syncing test yet, check the ongoing tests
	if (pTest == NULL)
	{
		for(i = 0; i < this->m_ongoingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCES) this->m_ongoingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get top level ongoing test %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)

			pTest = pTest->GetTestWithRequestID(dwResponseID);
			if (pTest != NULL)
				break;
		} // end for (each ongoing test heirarchy)
	} // end if (have a current active test)

	// If we still haven't found the syncing test, something's wrong
	if (pTest == NULL)
	{
		DPL(0, "Couldn't find test that synchronized using request ID %u!",
			1, dwResponseID);
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't find test)

	if (pvData != NULL)
	{
		if (pTest->m_pSyncDataList == NULL)
		{
			DPL(0, "Test %u's receive sync data list doesn't exist but data is incoming!",
				1, pTest->m_dwUniqueID);
			hr = E_FAIL;
			goto DONE;
		} // end if (the receive data list is gone)

		hr = pTest->m_pSyncDataList->UnpackFromBuffer(pvData, dwDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack sync data list!", 0);
			goto DONE;
		} // end if (couldn't add sync data from tester)
	} // end if (there's actually data associated with the response)


	pTest->m_hrResponseResult = S_OK;

	if (! SetEvent(pTest->m_hResponseEvent))
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't set sync response event (%x)!",
			1, pTest->m_hResponseEvent);
		goto DONE;
	} // end if (couldn't set event)


DONE:

	return (hr);
} // CTNExecutorPriv::HandleSyncResponse
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::HandleLostTester()"
//==================================================================================
// CTNExecutorPriv::HandleLostTester
//----------------------------------------------------------------------------------
//
// Description: Handles a lost tester.  If we're already done with that test, or
//				minding our own business, we ignore it.  If we're doing something
//				involving that tester right now, though, then we have to abort it.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test under which a tester was lost.
//	DWORD dwTestUniqueID		ID of test with problem.
//	int iLostTesterNum			Tester number who was lost.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::HandleLostTester(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iLostTesterNum)
{
	HRESULT				hr = S_OK;
	PTNTESTINSTANCES	pTest = NULL;
#ifndef _XBOX // no IPC supported
	int					i;
	PTNLEECH			pLeech;
#endif // ! XBOX



#pragma BUGBUG(vanceo, "There really should be a locking mechanism here")

	pTest = this->GetTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(1, "Couldn't find test %u:%u which lost tester %i, ignoring.",
			3, dwTopLevelUniqueID, dwTestUniqueID, iLostTesterNum);

		// See comments above.
		goto DONE;
	} // end if (couldn't find test)


	pTest->m_dwRefCount++; // we're using it

	pTest->m_paTesterSlots[iLostTesterNum].fGone = TRUE;


	// If the test is currently waiting for something, fire it and let it know
	// there's a problem.
	if (pTest->m_dwRequestID != 0)
	{
		DPL(0, "Lost tester %i in test %u:%u, alerting waiting operation %u.",
			4, iLostTesterNum, dwTopLevelUniqueID, dwTestUniqueID,
			pTest->m_dwRequestID);

		pTest->m_hrResponseResult = TNERR_LOSTTESTER;

		// Since we don't have a lock, it's possible to try setting the event
		// after the handle has been closed.  Just ignore the error.
		if (! SetEvent(pTest->m_hResponseEvent))
		{
			hr = GetLastError();

			DPL(0, "WARNING: Couldn't set response event (%x)!  %e",
				2, pTest->m_hResponseEvent, hr);

			hr = S_OK;

			goto DONE;
		} // end if (couldn't set event)
	} // end if (some request is outstanding)
	else
	{
		DPL(3, "Test %u:%u lost tester %i, alerting any leeches and noting for future reference.",
			3, dwTopLevelUniqueID, dwTestUniqueID, iLostTesterNum);

#ifndef _XBOX // no IPC supported
		// We want to pass the LostTester message down to any leeches we may have.
		this->m_leeches.EnterCritSection();
		for(i = 0; i < this->m_leeches.Count(); i++)
		{
			pLeech = (PTNLEECH) this->m_leeches.GetItem(i);
			if (pLeech == NULL)
			{
				DPL(0, "Couldn't get leech %i!", 1, i);
				this->m_leeches.LeaveCritSection();
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get leech)

			hr = pLeech->SendLostTesterMsgToOtherSide(dwTopLevelUniqueID, dwTestUniqueID,
													iLostTesterNum);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't send lost tester message (%u:%u, %i) to leech %i!",
					4, dwTopLevelUniqueID, dwTestUniqueID, iLostTesterNum, i);
				this->m_leeches.LeaveCritSection();
				goto DONE;
			} // end if (couldn't ping leech)
		} // end for (each leech)
		this->m_leeches.LeaveCritSection();
#endif // ! XBOX

	} // end else (no requests outstanding)


DONE:

	if (pTest != NULL)
	{
		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test %x (id %u).", 2, pTest, pTest->m_dwUniqueID);
			delete (pTest);
			pTest = NULL;
		} // end if (we can delete the test)
		else
		{
			DPL(7, "Not deleting test %x (id %u), its refcount = %u.",
				3, pTest, pTest->m_dwUniqueID, pTest->m_dwRefCount);
		} // end else (we can't delete the test)
	} // end if (have test)

	return (hr);
} // CTNExecutorPriv::HandleLostTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::HandleGetMachineInfoResponse()"
//==================================================================================
// CTNExecutorPriv::HandleGetMachineInfoResponse
//----------------------------------------------------------------------------------
//
// Description: Handles a response resulting from a GetMachineInfo request.
//
// Arguments:
//	DWORD dwResponseID		ID of request that asked for this test, or 0 for none.
//	PVOID pvData			Pointer to machine info data buffer.
//	DWORD dwDataSize		Size of buffer.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::HandleGetMachineInfoResponse(DWORD dwResponseID, PVOID pvData,
													DWORD dwDataSize)
{
	HRESULT				hr = S_OK;
	PTNTESTINSTANCES	pTest = NULL;
	int					i;


	// Check the current active test hierarchy first
	if (this->m_pTest != NULL)
	{
		pTest = this->m_pTest->GetTestWithRequestID(dwResponseID);
	} // end if (have a current active test)

	// If we haven't found the syncing test yet, check the ongoing tests
	if (pTest == NULL)
	{
		for(i = 0; i < this->m_ongoingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCES) this->m_ongoingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get top level ongoing test %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)

			pTest = pTest->GetTestWithRequestID(dwResponseID);
			if (pTest != NULL)
				break;
		} // end for (each ongoing test heirarchy)
	} // end if (have a current active test)

	// If we still haven't found the syncing test, something's wrong
	if (pTest == NULL)
	{
		DPL(0, "Couldn't find test that tried to GetMachineInfo using request ID %u!",
			1, dwResponseID);
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't find test)

	if ((pvData != NULL) && (pTest->m_pReturnedInfo != NULL))
	{
		hr = pTest->m_pReturnedInfo->UnpackFromBuffer(pvData, dwDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack info data!", 0);
			goto DONE;
		} // end if (couldn't unpack info)
	} // end if (there's data and a place to store it)
	else
	{
		DPL(0, "No data or no place to store it (%x, %x)!?  DEBUGBREAK()-ing.",
			2, pvData, pTest->m_pReturnedInfo);
		DEBUGBREAK();
	} // end else (no data or no place to store it)


	pTest->m_hrResponseResult = S_OK;

	if (! SetEvent(pTest->m_hResponseEvent))
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't set GetMachineInfo response event (%x)!",
			1, pTest->m_hResponseEvent);
		goto DONE;
	} // end if (couldn't set event)


DONE:

	return (hr);
} // CTNExecutorPriv::HandleGetMachineInfoResponse
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::DoTestThread()"
//==================================================================================
// CTNExecutorPriv::DoTestThread
//----------------------------------------------------------------------------------
//
// Description: Function that performs the real test thread behavior.  Waits for an
//				item to test, then executes it.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::DoTestThread(void)
{
	HRESULT					hr = S_OK;
	HMODULE					hOle32 = NULL;
	PCOINITIALIZEEXPROC		pfnCoInitializeEx = NULL;
	BOOL					fCOMInitted = FALSE;
	BOOL					fInitTestingCalled = FALSE;
	HANDLE					ahWaitArray[3];
	int						i;
	char					szTemp[1024];


	if (this->m_hKillTestThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	if (this->m_hNewTestEvent == NULL)
	{
		DPL(0, "New test event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	if (this->m_fInitCOMForTestThread)
	{
#ifndef _XBOX // CoInitialize not supported
#pragma BUGBUG(vanceo, "CoInitializeEx is crashing my Win9x machines when CoCreating DNWSOCK")
		//hOle32 = LoadLibrary("ole32.dll");
		if (hOle32 != NULL)
			pfnCoInitializeEx = (PCOINITIALIZEEXPROC) GetProcAddress(hOle32, "CoInitializeEx");

		// If we couldn't get it, assume it's because we're on a Win95 Gold machine
		if (pfnCoInitializeEx == NULL)
		{
			DPL(8, "Initializing COM with single thread apartment concurrency model.", 0);

			// Just use the regular CoInitialize.  S_FALSE is okay, it just means it
			// was already called.
			hr = CoInitialize(NULL);
			if ((hr != S_OK) && (hr != S_FALSE))
			{
				DPL(0, "Couldn't initialize COM!", 0);
				goto DONE;
			} // end if (couldn't initialize COM)
		}
		else
		{
			DPL(8, "Initializing COM with multithread apartment concurrency model.", 0);

			// Note: this function call requires DCOM95 to be installed on Win95
			// machines and it returns S_FALSE if it was already called (which is
			// documented as being okay, as long as we balance the calls).
			hr = pfnCoInitializeEx(NULL, COINIT_MULTITHREADED);
			if ((hr != S_OK) && (hr != S_FALSE))
			{
				DPL(0, "Couldn't initialize COM using Ex interface!", 0);
				goto DONE;
			} // end if (couldn't initialize COM ex)
		} // end if (got CoInitializeEx)
#endif // ! XBOX

		fCOMInitted = TRUE;

		if (hOle32 != NULL)
		{
			FreeLibrary(hOle32); // ignore error
			hOle32 = NULL;
		} // end if (have OLE library)
	} // end if (the test thread should initialize COM)


	// Let the user prep for testing if it wants too.
	if (this->m_pfnInitializeTesting != NULL)
	{
		DPL(9, "Calling module's InitializeTesting function %X",
			1, this->m_pfnInitializeTesting);

		hr = this->m_pfnInitializeTesting(&(this->m_info));
		if (hr != S_OK)
		{
			DPL(0, "Module's InitializeTesting callback failed!", 0);
			goto DONE;
		} // end if (app's callback failed)

		fInitTestingCalled = TRUE;
	} // end if (the module has an init testing callback)


	// If we're in documentation mode, we have to do things a little
	// differently.
	if (this->m_dwMode == TNMODE_DOCUMENTATION)
	{
		DPL(0, "Beginning documentation.", 0);
		this->LogInternal(TNLST_CONTROLLAYER_INFO, "Beginning documentation.");


		// Ping the UI to let it know.
		if (! SetEvent(this->m_pOwningSlave->m_hCompletionOrUpdateEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set event %x!",
				1, this->m_pOwningSlave->m_hCompletionOrUpdateEvent);
			goto DONE;
		} // end if (couldn't set event)


		hr = this->DoDocumentation(&(this->m_testtable));
		if (hr != S_OK)
		{
			DPL(0, "Doing documentation failed!", 0);
			goto DONE;
		} // end if (doing documentation)


		DPL(0, "Documentation done.", 0);
		this->LogInternal(TNLST_CONTROLLAYER_INFO, "Documentation done.");


		// Ping the UI to let it know.
		if (! SetEvent(this->m_pOwningSlave->m_hCompletionOrUpdateEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set event %x!",
				1, this->m_pOwningSlave->m_hCompletionOrUpdateEvent);
			goto DONE;
		} // end if (couldn't set event)


		// This thread's mission in life is now complete.
		goto DONE;
	} // end if (in documenation mode)


	ahWaitArray[0] = this->m_hKillTestThreadEvent;
	ahWaitArray[1] = this->m_hNewTestEvent;
	ahWaitArray[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

	do
	{
		// Update the control layer to let it know we're still alive
		this->m_dwLastTestthreadPing = GetTickCount();

		switch (WaitForMultipleObjects(2, ahWaitArray, FALSE, IDLE_INTERVAL))
		{
			case WAIT_OBJECT_0:
				// Time to die.
				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// We probably have a new test, so check our queue.

#ifdef DEBUG
				if (this->m_pTest != NULL)
				{
					DPL(0, "Current test is not NULL!", 0);
					DEBUGBREAK();
				} // end if (current test is not NULL)
#endif // DEBUG

				do
				{
					this->m_queuedtests.EnterCritSection();
					for(i = 0; i < this->m_queuedtests.Count(); i++)
					{
						this->m_pTest = (PTNTESTINSTANCES) this->m_queuedtests.GetItem(i);
						if (this->m_pTest == NULL)
						{
							DPL(0, "Couldn't get queued test %i!", 1, i);
							this->m_queuedtests.LeaveCritSection();
							hr = E_FAIL;
							goto DONE;
						} // end if (no test)

						if (this->m_pTest->m_dwRequestID != 0)
						{
							DPL(8, "Not running test %u, it's waiting for something (request ID %u).",
								1, this->m_pTest->m_dwRequestID);

							// Forget about the test.
							this->m_pTest = NULL;
						} // end if (this test is waiting for something)
						else
						{
							// Make sure it's known we're using the test.
							this->m_pTest->m_dwRefCount++;

							// Pull it from the queue.
							hr = this->m_queuedtests.RemoveFirstReference(this->m_pTest);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't remove test %x from queue!",
									1, this->m_pTest);
								goto DONE;
							} // end if (couldn't remove test)

							break;
						} // end else (this test isn't waiting)
					} // end for (each test in the queue)
					this->m_queuedtests.LeaveCritSection();


					// If we couldn't get a test it's because there aren't any left
					// to run.  Stop looping and go back to sleep.
					if (this->m_pTest == NULL)
						break;


					// Otherwise, run the test.

					hr = this->RunTest(this->m_pTest, this->m_pTest->m_pvInputData,
										this->m_pTest->m_dwInputDataSize);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't run top level test!", 0);
						goto DONE;
					} // end if (couldn't run test)


#pragma BUGBUG(vanceo, "Having ReportInternal clear this->m_pTest is kind of funky")
					// Note that ReportInternal is the function where this->m_pTest
					// is cleared, so it might be invalid at this point.


					// Update the control layer to let it know we're still alive
					this->m_dwLastTestthreadPing = GetTickCount();
				} // end do (while there are more tests to run)
				while (TRUE);
			  break;

			case WAIT_TIMEOUT:
				if (this->m_pOwningSlave == NULL)
				{
#pragma TODO(vanceo, "Implement leech idle")
				} // end if (not owned by slave)
				else
				{
					// Twiddle our thumbs while we're waiting.
					hr = this->m_pOwningSlave->DoIdle(CTRLIDLE_NOTHING);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't perform the idle activities!", 0);
						goto DONE;
					} // end if (failed idling)
				} // end else (owned by slave)
			  break;

			default:
				DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
				hr = E_FAIL;
			  break;
		} // end switch (on wait return)
	} // end do
	while (hr == S_OK);


DONE:

	// Let the user cleanup after testing if it wants too.
	if (fInitTestingCalled && (this->m_pfnCleanupTesting != NULL))
	{
		HRESULT		temphr;


		DPL(9, "Calling module's CleanupTesting function %X",
			1, this->m_pfnCleanupTesting);

		temphr = this->m_pfnCleanupTesting(&(this->m_info));
		if (temphr != S_OK)
		{
			DPL(0, "Module's CleanupTesting callback failed!", 0);
			
			// Only overwrite error if it was success previously.
			if (hr == S_OK)
				hr = temphr;
		} // end if (app's callback failed)

		fInitTestingCalled = FALSE;
	} // end if (the module has an init testing callback)

#ifndef _XBOX // no file logging supported
	if (this->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		FileWriteLine(this->m_hDocFile, ""); // double space
		FileWriteLine(this->m_hDocFile,
					"//==================================================================================");
		FileWriteLine(this->m_hDocFile, "// Documentation build listing complete");
		FileWriteLine(this->m_hDocFile,
					"//==================================================================================");

		CloseHandle(this->m_hDocFile);
		this->m_hDocFile = INVALID_HANDLE_VALUE;
	} // end if (have doc file)
#else // ! XBOX
#pragma TODO(tristanj, "What to do about documenation logging?")
#endif // XBOX

#ifndef _XBOX // no file logging supported
	if (this->m_hErrToSrcFile != INVALID_HANDLE_VALUE)
	{
#pragma BUGBUG(vanceo, "do this right")
		StringGetCurrentDateStr(szTemp);
		StringPrepend(szTemp, "// ErrToSrc complete at ");


		FileWriteLine(this->m_hErrToSrcFile, ""); // double space
		FileWriteLine(this->m_hErrToSrcFile,
					"//==================================================================================");
		FileWriteLine(this->m_hErrToSrcFile, szTemp);
		FileWriteLine(this->m_hErrToSrcFile,
					"//==================================================================================");

		CloseHandle(this->m_hErrToSrcFile);
		this->m_hErrToSrcFile = INVALID_HANDLE_VALUE;


		if (this->m_pszErrToSrcPathSearch != NULL)
		{
			LocalFree(this->m_pszErrToSrcPathSearch);
			this->m_pszErrToSrcPathSearch = NULL;
		} // end if (allocated string)

		if (this->m_pszErrToSrcPathReplace != NULL)
		{
			LocalFree(this->m_pszErrToSrcPathReplace);
			this->m_pszErrToSrcPathReplace = NULL;
		} // end if (allocated string)
	} // end if (have errtosrc file)
#else // ! XBOX
#pragma TODO(tristanj, "What to do about ErrToSrc logging?")
#endif // XBOX

	if (hr != S_OK)
	{
		DPL(0, "%s test thread aborting!  DEBUGBREAK()-ing.  %e",
			2, ((this->m_pOwningSlave != NULL) ? "Slave" : "Leech"), hr);

		// Ignoring error
		this->SprintfLogInternal(TNLST_CRITICAL, "%s test thread aborting!  DEBUGBREAK()-ing.  %e",
			2, ((this->m_pOwningSlave != NULL) ? "Slave" : "Leech"), hr);

		DEBUGBREAK();


#pragma BUGBUG(vanceo, "Disconnect from master?")

		this->KillTestThread(TRUE); // ignore result

#ifndef _XBOX // No dumping memory logs to files
#pragma TODO(vanceo, "Implement better (do for leech, too)")
		if ((this->m_pOwningSlave != NULL) &&
			(this->m_pOwningSlave->m_pszFailureMemDumpDirPath != NULL))
		{
			char*	pszFile;
			DWORD	dwSize;


			dwSize = strlen(this->m_pOwningSlave->m_pszFailureMemDumpDirPath) + strlen("testthread_abort.txt") + 1; // + NULL termination

			if (! StringEndsWith(this->m_pOwningSlave->m_pszFailureMemDumpDirPath, "\\", TRUE))
				dwSize++; // add backslash
			
			pszFile = (char*) LocalAlloc(LPTR, dwSize);
			if (pszFile != NULL)
			{
				strcpy(pszFile, this->m_pOwningSlave->m_pszFailureMemDumpDirPath);
				if (! StringEndsWith(this->m_pOwningSlave->m_pszFailureMemDumpDirPath, "\\", TRUE))
					strcat(pszFile, "\\"); // add backslash
				strcat(pszFile, "testthread_abort.txt");

				StringGetCurrentDateStr(szTemp);
				StringPrepend(szTemp, "Test thread aborted mem log dump ");

				TNDebugDumpMemoryLogToFile(pszFile, szTemp, TRUE);

				LocalFree(pszFile);
				pszFile = NULL;
			} // end if (couldn't allocate memory)
		} // end if (we should dump the log on failures)
#endif // ! XBOX

	} // end if (quitting with a failure)

	if (fCOMInitted)
	{

#ifndef _XBOX // CoUninitialize not supported
		if (hr == S_OK)
		{
			DPL(8, "Uninitializing COM.", 0);
			CoUninitialize();
			fCOMInitted = FALSE;
		} // end if (leaving normally)
		else
		{
			DPL(0, "WARNING: Not uninitializing COM because we're exiting with an error!", 0);
		} // end else (leaving with error)
#endif // ! XBOX

	} // end if (initialized COM)

#ifndef _XBOX // OLE32.DLL doesn't exist on Xbox
	if (hOle32 != NULL)
	{
		if (hr == S_OK)
		{
			FreeLibrary(hOle32); // ignore error
			hOle32 = NULL;
		} // end if (leaving normally)
		else
		{
			DPL(0, "WARNING: Not freeing OLE32 library because we're exiting with an error!", 0);
		} // end else (leaving with error)
	} // end if (have OLE library)
#endif // ! XBOX

	if (this->m_pTest != NULL)
	{
		this->m_pTest->m_dwRefCount--;
		if (this->m_pTest->m_dwRefCount == 0)
		{
			DPL(0, "Deleting current test %x.", 1, this->m_pTest);
			delete (this->m_pTest);
		} // end if (can delete test)
		else
		{
			DPL(0, "Not deleting current test %x, it's refcount is %u.",
				2, this->m_pTest, this->m_pTest->m_dwRefCount);
		} // end else (can't delete test)
		this->m_pTest = NULL;
	} // end if (have active test)

	return (hr);
} // CTNExecutorPriv::DoTestThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::LogInternal()"
//==================================================================================
// CTNExecutorPriv::LogInternal
//----------------------------------------------------------------------------------
//
// Description: Logs the string to the slave or sends it through the leech,
//				depending on which owns the executor.
//
// Arguments:
//	DWORD dwLogStringType	What type of string this is.
//	char* szString			String to log.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::LogInternal(DWORD dwLogStringType, char* szString)
{
	HRESULT		hr = S_OK;


	if (this->m_pOwningSlave == NULL)
	{

#ifndef _XBOX // no IPC supported
#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->LogToOwner(dwLogStringType, szString, 0);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't log string to slave through leech!", 0);
		} // end if (logging to slave failed)

		this->m_fDeadlockCheck = fOldCheckStatus;
#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
#ifndef _XBOX
		// Log the string
		hr = this->m_pOwningSlave->m_pfnLogString(dwLogStringType, szString);
		if (hr != S_OK)
		{
			DPL(0, "Shell's LogString function failed!", 0);
		} // end if (shell's log string function failed)
#else // ! XBOX
#pragma TODO(tristanj, "Map this logging to Xbox's logging subsystem")
#endif // XBOX
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::LogInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::SprintfLogInternal()"
//==================================================================================
// CTNExecutorPriv::SprintfLogInternal
//----------------------------------------------------------------------------------
//
// Description: Logs the string to the slave or sends it through the leech,
//				depending on which owns the executor.
//
// Arguments:
//	DWORD dwLogStringType	What type of string this is.
//	char* szFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms		How many parameters are in the following variable
//							parameter list.
//	...						Variable list of parameters to parse.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::SprintfLogInternal(DWORD dwLogStringType, char* szFormatString,
										DWORD dwNumParms, ...)
{
	HRESULT		hr = S_OK;
	char*		pszBuffer = NULL;
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp = 0;


#ifndef _XBOX // no TNsprintf functions
	if (dwNumParms > 0)
	{
		papvParms = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
		if (papvParms == NULL)
			return (E_OUTOFMEMORY);

		va_start(currentparam, dwNumParms);

		for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
		{
			papvParms[dwTemp] = va_arg(currentparam, PVOID);
		} // end for (each parameter)

		va_end(currentparam);
	} // end if (there are parameters to check)


	// Print the items.
	TNsprintf_array(&pszBuffer, szFormatString, dwNumParms, papvParms);

	LocalFree(papvParms);
	papvParms = NULL;
#else // ! XBOX
#pragma TODO(tristanj, "Need to map executor logging to xLog")
#endif // XBOX

	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->LogToOwner(dwLogStringType, pszBuffer, 0);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't log string to slave through leech!", 0);
		} // end if (logging to slave failed)

		this->m_fDeadlockCheck = fOldCheckStatus;
#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
#ifndef _XBOX
		// Log the string
		hr = this->m_pOwningSlave->m_pfnLogString(dwLogStringType, pszBuffer);
		if (hr != S_OK)
		{
			DPL(0, "Shell's LogString function failed!", 0);
		} // end if (shell's log string function failed)
#else // ! XBOX
#pragma TODO(tristanj, "Map this logging to Xbox's logging subsystem")
#endif // XBOX
	} // end else (owned by slave)

	TNsprintf_free(&pszBuffer);

	return (hr);
} // CTNExecutorPriv::SprintfLogInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::ReportInternal()"
//==================================================================================
// CTNExecutorPriv::ReportInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of test result reporting.
//
// Arguments:
//	PTNTESTINSTANCES pTest				Pointer to test instance this report
//										pertains to.
//	HRESULT hresult						Success or error code.
//	BOOL fTestComplete					Does this report mean that no more execution
//										will be performed on the test (either
//										because it completed or it encountered a
//										fatal error)?
//	BOOL fSuccess						Is this report a success (note this does not
//										necessarily mean hresult is 0)?
//	BOOL fExecCaseExitReport			Is this the report automatically generated
//										when the ExecCase function returns?
//	PVOID pvOutputData					Optional pointer to output data to send with
//										report.
//	DWORD dwOutputDataSize				Size of output data to send with report.
//	PTNOUTPUTVARSLIST pOutputVars		Optional pointer to list of output variables
//										associated with result.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::ReportInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
										BOOL fTestComplete, BOOL fSuccess,
										BOOL fExecCaseExitReport,
										PVOID pvOutputData, DWORD dwOutputDataSize,
										PTNOUTPUTVARSLIST pOutputVars)
{
	HRESULT		hr;
	PTNRESULT	pResult = NULL;
	PTNRESULT	pSubResult;
	BOOL		fHaveResultsLock = FALSE;


	DPL(9, "==>(%x, %x, %B, %B, %B, %x, %u, %x)",
		8, pTest, hresult, fTestComplete, fSuccess, fExecCaseExitReport,
		pvOutputData, dwOutputDataSize, pOutputVars);

	// This really should be done at a higher level, but since it's possibly not,
	// I'm making sure it's okay to use the test while we're in here.
	pTest->m_dwRefCount++;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();



	// If it's an exec case exit report, we have to remove the reference to this
	// test (i.e. hide it so we can run more when the master sends us something to
	// do in response to this report).  We do this here because the sending the
	// report to the master is asynchronous, and we could be processing the reply
	// before we even exit this function.
	// Note that it couldn't have been a top level ongoing test previously because
	// exec case exit reports only happen for non-ongoing/current tests, so we only
	// have to check the top level current test, otherwise it's a subtest.
	if (fExecCaseExitReport)
	{
		if (this->m_pTest == pTest)
		{
			this->m_pTest = NULL;
		} // end if (this is the top level current test)
		else
		{
			((PTNTESTINSTANCES) pTest->m_pParentTest)->m_pSubTest = NULL;
		} // end else (this is not a top level test)

		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(0, "Refcount for test %x hit 0!?  DEBUGBREAK()-ing.",
				1, pTest);
			DEBUGBREAK();
		} // end if (refcount hit 0)
		else
		{
			DPL(9, "Refcount for test %x is %u.",
				2, pTest, pTest->m_dwRefCount);
		} // end else (refcount not at 0)


		// If the ExecCase function returned but it's not complete, move it to the
		// ongoing tests list.
		if (! fTestComplete)
		{
#pragma BUGBUG(vanceo, "Their Executor interface will be going away though!")
			// Otherwise, add it to the ongoing tests list.
			hr = this->m_ongoingtests.Add(pTest);
			if (hr != S_OK)
			{
				DPL(0, "Adding test unique ID %u to ongoing test list failed!",
					1, pTest->m_dwUniqueID);
				goto DONE;
			} // end if (failed to run the test)
		} // end if (not complete on function exit)
	} // end if (exec case exited)


	if (fTestComplete)
	{
		// If there's a completion callback, notify it.
		if (pTest->m_pfnPokeTestCompleted != NULL)
		{
			TNPOKETESTCOMPLETEDDATA		tnptcd;


			ZeroMemory(&tnptcd, sizeof (TNPOKETESTCOMPLETEDDATA));
			tnptcd.dwSize = sizeof (TNPOKETESTCOMPLETEDDATA);
			tnptcd.pTest = pTest->m_pCase;
			tnptcd.fSuccess = fSuccess;
			tnptcd.hresult = hresult;
			tnptcd.pvData = pvOutputData;
			tnptcd.dwDataSize = dwOutputDataSize;
			tnptcd.pvUserContext = pTest->m_pvUserContext;

			hr = pTest->m_pfnPokeTestCompleted(&tnptcd);
			if (hr != S_OK)
			{
				DPL(0, "Module's PokeTestCompleted callback failed!", 0);
				goto DONE;
			} // end if (module's callback failed)
		} // end if (there's a PokeTestCompleted callback)
	} // end if (the test is done)



	// Retrieve the corresponding result for this test because we may need it
	// later.
#pragma BUGBUG(vanceo, "Grab lock because refcount isn't increased for us")
	this->m_results.EnterCritSection();

	pResult = this->m_results.GetResultForTest(pTest);
	if (pResult == NULL)
	{
		DPL(0, "Couldn't get result corresponding to test %x (ID %u)!",
			2, pTest, pTest->m_dwUniqueID);

		this->m_results.LeaveCritSection();

		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get result)

	// Grab a reference so it doesn't go away while we still need it.
	pResult->m_dwRefCount++;

	this->m_results.LeaveCritSection();



	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->ReportToOwner(pTest, hresult,
												fTestComplete, fSuccess,
												fExecCaseExitReport,
												pvOutputData, dwOutputDataSize,
												pOutputVars);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't send report to slave through leech!", 0);
			goto DONE;
		} // end if (send report failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->ReportInternal(pTest, hresult,
													fTestComplete, fSuccess,
													pvOutputData, dwOutputDataSize,
													pOutputVars);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't send report!", 0);
			goto DONE;
		} // end if (reporting failed)
	} // end else (owned by slave)


	// If the test is completed, we may need to throw out the results.  We do this
	// after the ReportToOwner/ReportInternal calls because the pOutputVars variable
	// we got passed in may actually belong to a result we're about to free.  We
	// crash if that's the case: bad.
	// When we get a repetition complete message, we also free results.  Because
	// there's a race condition where that message could clean out all the results
	// in the list before we get here, we added code there to ignore any tests which
	// which have the TNTCO_DONTSAVERESULTS flag or m_fUnderDontSaveTest set.
	if (fTestComplete)
	{
		// Prevent other threads from trying to do something similar while
		// we're doing it, cause that would mess us up.
		this->m_results.EnterCritSection();
		fHaveResultsLock = TRUE;

		// If we're not supposed to save results, remove the results from any
		// subtests we invoked.
		if (pTest->m_pResultsToFree != NULL)
		{
			while (pTest->m_pResultsToFree->Count() > 0)
			{
				pSubResult = (PTNRESULT) pTest->m_pResultsToFree->PopFirstItem();
				if (pSubResult == NULL)
				{
					DPL(0, "Couldn't pop first sub result to free from list!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't pop first item)


				DPL(3, "Freeing results for test ID %u (from ResultsToFree list).",
					1, pSubResult->m_pTest->m_dwUniqueID);


				// Pull it off the global list.  We're going to ignore errors,
				// because it may have been removed by some other means, like
				// the user explicitly calling a FreeResults function.
				hr = this->m_results.RemoveFirstReference(pSubResult);
				if (hr != S_OK)
				{
					DPL(4, "Couldn't remove result %x (under test %u) from results list, ignoring.  %e",
						3, pSubResult, pTest->m_dwUniqueID, hr);

					hr = S_OK;
				} // end if (remove first reference failed)


				// Free any output vars or data.
				pSubResult->DestroyOutputDataBufferAndVars();


				// Take off the ref given by PopFirstItem.
				pSubResult->m_dwRefCount--;

#ifdef DEBUG
				if (pSubResult->m_dwRefCount == 0xFFFFFFFF)
				{
					DPL(0, "Sub result %x (test %u) refcount has gone bad!  DEBUGBREAK()-ing.",
						2, pSubResult, pSubResult->m_pTest->m_dwUniqueID);
					DEBUGBREAK();
				} // end if (count got screwed)
#endif // DEBUG

				if (pSubResult->m_dwRefCount == 0)
				{
					DPL(7, "Deleting sub result %x.", 1, pSubResult);
					delete (pSubResult);
				} // end if (should delete result)
				else
				{
					DPL(7, "Not deleting sub result %x, its refcount is %u.",
						2, pSubResult, pSubResult->m_dwRefCount);
				} // end else (shouldn't delete result)
				pSubResult = NULL;
			} // end while (still more tests to free)


			// We don't need the list object any more.
			delete (pTest->m_pResultsToFree);
			pTest->m_pResultsToFree = NULL;
		} // end if (we shouldn't save results)



		// If we're not saving results from this, dump them, otherwise check
		// if parent tests should toss out our results.
		if (pTest->m_pCase->m_dwOptionFlags & TNTCO_DONTSAVERESULTS)
		{
			DPL(3, "Freeing results for test ID %u (because of TNTCO_DONTSAVERESULTS).",
				1, pTest->m_dwUniqueID);

			// Pull it off the global list.  We're going to ignore errors,
			// because it may have been removed by some other means, like
			// the user explicitly calling a FreeResults function.
			hr = this->m_results.RemoveFirstReference(pResult);
			if (hr != S_OK)
			{
				DPL(4, "Couldn't remove result %x (under test %u) from results list, ignoring.  %e",
					3, pResult, pTest->m_dwUniqueID, hr);

				hr = S_OK;
			} // end if (remove first reference failed)
			else
			{
				pResult->m_dwRefCount--;

#ifdef DEBUG
				if (pResult->m_dwRefCount == 0xFFFFFFFF)
				{
					DPL(0, "Result %x (test %u) refcount has gone bad!  DEBUGBREAK()-ing.",
						2, pResult, pTest->m_dwUniqueID);
					DEBUGBREAK();
				} // end if (count got screwed)
#endif // DEBUG

				if (pResult->m_dwRefCount == 0)
				{
					DPL(7, "Deleting result %x, its output data will die with it.",
						1, pResult);
					delete (pResult);
				} // end if (should delete test)
				else
				{
					DPL(7, "Not deleting result %x, its refcount is %u, but manually freeing output data.",
						2, pResult, pResult->m_dwRefCount);

					pResult->DestroyOutputDataBufferAndVars();
				} // end else (shouldn't delete test)

				// Forget about it, so we don't free it below.
				pResult = NULL;
			} // end if (remove first reference succeeded)
		} // end if (not saving results)
		else if (pTest->m_pParentTest != NULL)
		{
			PTNTESTINSTANCES	pParentTest;


			pParentTest = pTest->m_pParentTest;
			do
			{
				// This list will only exist if the test case has the
				// TNTCO_DONTSAVERESULTS flag.
				if (pParentTest->m_pResultsToFree != NULL)
				{
					// Add our results to that parent's list of results to free
					// when it completes.
					hr = pParentTest->m_pResultsToFree->Add(pResult);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add test %u's result to parent (%u)'s list of tests to free!",
							2, pTest->m_dwUniqueID, pParentTest->m_dwUniqueID);
						goto DONE;
					} // end if (couldn't add test)

					// Note that we've added it to a list of items to free so any
					// other automated operation that comes along won't try to free it
					// as well.
					pResult->m_fUnderDontSaveTest = TRUE;

					DPL(9, "Result %x (for test %x, ID %u) is under DONTSAVERESULTS test %x, ID %u.",
						5, pResult, pTest, pTest->m_dwUniqueID,
						pParentTest, pParentTest->m_dwUniqueID);


					// We found one, so we can stop searching.
					break;
				} // end if (parent test shouldn't save results)

				pParentTest = pParentTest->m_pParentTest;
			} // end do (while there's a parent test)
			while (pParentTest != NULL);
		} // end else if (a subtest)

		fHaveResultsLock = FALSE;
		this->m_results.LeaveCritSection();
	} // end if (test completed)


	// If it's an exec case exit report, we still have to do some more things.
	if (fExecCaseExitReport)
	{
		// Release the thread waiting for the ExecCase function to return, if any.
		if (pTest->m_hExecCaseExitEvent != NULL)
		{
			if (! SetEvent(pTest->m_hExecCaseExitEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set ExecCaseExit event (%x)!",
					1, pTest->m_hExecCaseExitEvent);
				goto DONE;
			} // end if (couldn't set event)
		} // end if (there's a thread waiting for this)


		/*
		// Note that this refcount would still be at least one if it got thrown on
		// the ongoing tests list.
		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test %x (id %u).", 2, pTest, pTest->m_dwUniqueID);
			delete (pTest);
		} // end if (we can delete the test)
		else
		{
			DPL(7, "Not deleting test %x (id %u), its refcount = %u.",
				3, pTest, pTest->m_dwUniqueID, pTest->m_dwRefCount);
		} // end else (we can't delete the test)
		pTest = NULL;
		*/
	} // end if (exec case exited)


DONE:


	// If we still have a reference to result object for this test, drop it.
	if (pResult != NULL)
	{
		pResult->m_dwRefCount--;

#ifdef DEBUG
		if (pResult->m_dwRefCount == 0xFFFFFFFF)
		{
			DPL(0, "Result %x (test %u) refcount has gone bad!  DEBUGBREAK()-ing.",
				2, pResult, pTest->m_dwUniqueID);
			DEBUGBREAK();
		} // end if (count got screwed)
#endif // DEBUG

		if (pResult->m_dwRefCount == 0)
		{
			DPL(7, "Deleting result %x, its output data will die with it.",
				1, pResult);
			delete (pResult);
		} // end if (should delete test)
		else
		{
			DPL(7, "Not deleting result %x, its refcount is %u.",
				2, pResult, pResult->m_dwRefCount);
		} // end else (shouldn't delete test)
		pResult = NULL;
	} // end if (still have result object)

	if (fHaveResultsLock)
	{
		fHaveResultsLock = FALSE;
		this->m_results.LeaveCritSection();
	} // end if (test completed)

	// Take off the refcount we added up above.
	pTest->m_dwRefCount--;
	if (pTest->m_dwRefCount == 0)
	{
		DPL(0, "Refcount for test %x hit 0 while leaving ReportInternal!  DEBUGBREAK()-ing.",
			1, pTest);
		DEBUGBREAK();
	} // end if (refcount hit 0)
	else
	{
		DPL(7, "Refcount for test %x is %u.",
			2, pTest, pTest->m_dwRefCount);
	} // end else (refcount did not hit 0)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutorPriv::ReportInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::WarnInternal()"
//==================================================================================
// CTNExecutorPriv::WarnInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of test result warning.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test instance this warning pertains to.
//	HRESULT hresult			Warning code.
//	PVOID pvUserData		Optional pointer to data to send with warning.
//	DWORD dwUserDataSize	Size of data to send with warning.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::WarnInternal(PTNTESTINSTANCES pTest, HRESULT hresult,
									PVOID pvUserData, DWORD dwUserDataSize)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->WarnOwner(pTest, hresult, pvUserData,
											dwUserDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't send warning to slave through leech!", 0);
		} // end if (send warning failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->WarnInternal(pTest, hresult, pvUserData,
												dwUserDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't send warning!", 0);
		} // end if (warning failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::WarnInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::SyncInternal()"
//==================================================================================
// CTNExecutorPriv::SyncInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of test syncing.
//
// Arguments:
//	PTNTESTINSTANCES pTest			Pointer to test to sync under.
//	char* szSyncName				User defined unique name for this sync
//									operation.  Must match on all machines involved.
//	PVOID pvSendData				Pointer to data to send to people to sync with.
//	DWORD dwSendDataSize			Size of data to send to people to sync with.
//	PTNSYNCDATALIST pReceiveData	Pointer to list that will hold the data received
//									from the other synchronizing testers.
//	int iNumMachines				How many testers we're syncing with (i.e. how
//									many integers there are in following array).
//	int* aiTesters					Array of integers indicating the testers to sync
//									with.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::SyncInternal(PTNTESTINSTANCES pTest,
									char* szSyncName,
									PVOID pvSendData,
									DWORD dwSendDataSize,
									PTNSYNCDATALIST pReceiveData,
									int iNumMachines,
									int* aiTesters)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{

#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerSync(pTest, szSyncName,
												pvSendData, dwSendDataSize,
												pReceiveData,
												iNumMachines, aiTesters);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have slave sync through leech!", 0);
		} // end if (sync failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX
	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->SyncInternal(pTest, szSyncName,
												pvSendData, dwSendDataSize,
												pReceiveData,
												iNumMachines, aiTesters);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't synchronize!", 0);
		} // end if (sync failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::SyncInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::IsTesterOnSameMachineInternal()"
//==================================================================================
// CTNExecutorPriv::IsTesterOnSameMachineInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of tester on same machine check.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to check under.
//	int iTesterNum			Tester number for which to check.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
BOOL CTNExecutorPriv::IsTesterOnSameMachineInternal(PTNTESTINSTANCES pTest,
													int iTesterNum)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (FALSE);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?")
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{

#ifndef _XBOX // no IPC supported
#pragma BUGBUG(vanceo, "Use hacking deadlock check turn off?")
		return (this->m_pOwningLeech->HaveOwnerCheckIfTesterOnMachine(pTest, iTesterNum));

#else // ! XBOX
		return E_FAIL; // On Xbox we aren't supporting leeches, so owner should always be a slave...
#endif // XBOX

	} // end if (not owned by slave)
	else
	{
		return (this->m_pOwningSlave->IsTesterOnSameMachineInternal(pTest, iTesterNum));
	} // end else (owned by slave)
} // CTNExecutorPriv::IsTesterOnSameMachineInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTesterIPInternal()"
//==================================================================================
// CTNExecutorPriv::GetTesterIPInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of tester IP retrieval.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to retrieve IP under.
//	int iTesterNum			Tester number for which to retrieve IP address.
//	WORD wPort				IP port to verify reachability.
//	char* szIPString		String to store result in (must be 16 chars).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::GetTesterIPInternal(PTNTESTINSTANCES pTest, int iTesterNum,
											WORD wPort, char* szIPString)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerGetTesterIP(pTest, iTesterNum, wPort,
														szIPString);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have slave get tester's IP through leech!", 0);
		} // end if (getting IP failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->GetTestersIPForPortInternal(pTest, iTesterNum,
																wPort, szIPString);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't get tester's IP!", 0);
		} // end if (getting IP failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::GetTesterIPInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTesterPhoneNumInternal()"
//==================================================================================
// CTNExecutorPriv::GetTesterPhoneNumInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of tester phone number retrieval.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to retrieve phone number under.
//	int iTesterNum			Tester number for which to retrieve phone number.
//	char* szIPString		String to store result in.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::GetTesterPhoneNumInternal(PTNTESTINSTANCES pTest,
												int iTesterNum, char* szPhoneNumber)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerGetTesterPhoneNum(pTest, iTesterNum,
																szPhoneNumber);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have slave get tester's phone number through leech!", 0);
		} // end if (getting phone number failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX

	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->GetTestersPhoneNumInternal(pTest, iTesterNum,
																szPhoneNumber);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't get tester's phone number!", 0);
		} // end if (getting phone number failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::GetTesterPhoneNumInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTesterCOMPortInternal()"
//==================================================================================
// CTNExecutorPriv::GetTesterCOMPortInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of COM port retrieval.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to retrieve COM port under.
//	int iTesterNum			Tester number for which to retrieve COM port.
//	DWORD* pdwCOMPort		Place to store result in.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::GetTesterCOMPortInternal(PTNTESTINSTANCES pTest, int iTesterNum,
												DWORD* pdwCOMPort)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerGetTesterCOMPort(pTest, iTesterNum,
															pdwCOMPort);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have slave get tester's COM port through leech!", 0);
		} // end if (getting COM port failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX
	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->GetTestersCOMPortInternal(pTest, iTesterNum,
															pdwCOMPort);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't get tester's COM port!", 0);
		} // end if (getting COM port failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::GetTesterCOMPortInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::CreateSubTest()"
//==================================================================================
// CTNExecutorPriv::CreateSubTest
//----------------------------------------------------------------------------------
//
// Description: Creates a new subtest under the given parent test, using the given
//				information.  This gets propogated up the object chain, and
//				eventually to the master, who responds with a unique ID to use.
//				This call will block until that happens.
//
// Arguments:
//	PTNTESTINSTANCES pParentTest	Pointer to parent test.
//	PTNTESTTABLECASE pCase			Pointer to case of subtest to create.
//	int iNumMachines				Number of integers in the following array.
//	PTNCTRLMACHINEID aTesters		Array of IDs indicating the testers in their
//									respective testing positions for the new test.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::CreateSubTest(PTNTESTINSTANCES pParentTest,
									PTNTESTTABLECASE pCase,
									int iNumMachines,
									PTNCTRLMACHINEID aTesters)
{
	HRESULT		hr;


	if (pParentTest->m_pSubTest != NULL)
	{
		DPL(0, "Parent test (ID %u) already has a subtest (ID %u)!",
			2, pParentTest->m_dwUniqueID, pParentTest->m_pSubTest->m_dwUniqueID);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (this item already has a subtest)

	pParentTest->m_pSubTest = new (CTNTestInstanceS)(0xFFFFFFFF,
													pCase,
													NULL,
													iNumMachines,
													aTesters,
													//0,
													pParentTest);
	if (pParentTest->m_pSubTest == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pParentTest->m_pSubTest->m_dwRefCount++; // we're using it


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerCreateSubTest((PTNTESTINSTANCES) pParentTest->m_pSubTest,
															iNumMachines,
															aTesters);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have leech's owner create sub test!", 0);
		} // end if (creating subtest failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX
	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->AnnounceSubTest((PTNTESTINSTANCES) pParentTest->m_pSubTest,
													iNumMachines,
													aTesters);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't announce subtest!", 0);
		} // end if (announcing test failed)
	} // end else (owned by slave)


DONE:

	return (hr);
} // CTNExecutorPriv::CreateSubTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTest()"
//==================================================================================
// CTNExecutorPriv::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the test with the given top level and personal
//				unique IDs.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test the specified test is under
//								(may be same as the actual ID).
//	DWORD dwTestUniqueID		ID of actual test to retrieve.
//
// Returns: Pointer to the given test if found, NULL otherwise.
//==================================================================================
PTNTESTINSTANCES CTNExecutorPriv::GetTest(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID)
{
	PTNTESTINSTANCES	pTopLevelTest = NULL;


	// If we have a current test, see if the top level specified is that.
	if (this->m_pTest != NULL)
	{
		if (this->m_pTest->m_dwUniqueID == dwTopLevelUniqueID)
			pTopLevelTest = this->m_pTest;
	} // end if (there's a current test)

	// If we didn't get a top level test above, check all the ongoing tests to see
	// if they're the specified top level test.
	if (pTopLevelTest == NULL)
	{
		pTopLevelTest = (PTNTESTINSTANCES) this->m_ongoingtests.GetTopLevelTest(dwTopLevelUniqueID);
	} // end if (haven't found top level test yet)


	// If we still don't have a top level test, we're screwed.
	if (pTopLevelTest == NULL)
		return (NULL);

	// Try to get the specified (sub)test.
	return (pTopLevelTest->GetTest(dwTestUniqueID));
} // CTNExecutorPriv::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTest()"
//==================================================================================
// CTNExecutorPriv::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the test with the given top level (from the
//				leech's perspective) and personal unique IDs.
//
// Arguments:
//	DWORD dwLeechTopLevelUniqueID	ID of leech's top level test the specified test
//									is under (may be same as the actual ID).
//	DWORD dwTestUniqueID			ID of actual test to retrieve.
//
// Returns: Pointer to the given test if found, NULL otherwise.
//==================================================================================
PTNTESTINSTANCES CTNExecutorPriv::GetLeechTest(DWORD dwLeechTopLevelUniqueID,
												DWORD dwTestUniqueID)
{
	PTNTESTINSTANCES	pLeechTopLevelTest = NULL;
	int					i;
	PTNTESTINSTANCES	pTest = NULL;


	// If we have a current test, see if the top level specified is that.  It
	// shouldn't be, because all of the leech top level tests should be under the
	// current test or on the ongoing tests list.
	if (this->m_pTest != NULL)
	{
#ifdef DEBUG
		if (this->m_pTest->m_dwUniqueID == dwLeechTopLevelUniqueID)
		{
			DPL(0, "Leech's top level test is our top most current test (ID %u)!?",
				1, dwLeechTopLevelUniqueID);
			return (NULL);
		} // end if (it's our top most current test)
#endif // DEBUG

		pLeechTopLevelTest = this->m_pTest->GetTest(dwLeechTopLevelUniqueID);
	} // end if (there's a current test)

	// If we didn't get a top level test above, check all the ongoing tests to see
	// if they have the specified top level test.
	if (pLeechTopLevelTest == NULL)
	{
		for(i = 0; i < this->m_ongoingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCES) this->m_ongoingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get ongoing test %i!", 1, i);
				return (NULL);
			} // end if (couldn't get item)

			pLeechTopLevelTest = pTest->GetTest(dwLeechTopLevelUniqueID);
			if (pLeechTopLevelTest != NULL)
				break;
		} // end for (each ongoing test)
	} // end if (haven't found top level test yet)


	// If we still don't have a top level test, we're screwed.
	if (pLeechTopLevelTest == NULL)
		return (NULL);

	// Try to get the specified (sub)test.
	return (pLeechTopLevelTest->GetTest(dwTestUniqueID));
} // CTNExecutorPriv::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::RunTest()"
//==================================================================================
// CTNExecutorPriv::RunTest
//----------------------------------------------------------------------------------
//
// Description: Executes the given test.  The test pointer is set to NULL upon
//				function exit.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to pointer to test to run.
//	PVOID pvInputData			Pointer to input data to use.
//	DWORD dwInputDataSize		Size of input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::RunTest(PTNTESTINSTANCES pTest, PVOID pvInputData,
								DWORD dwInputDataSize)
{
	HRESULT				hr;
	TNEXECCASEDATA		tnecd;
	int					i;


	ZeroMemory(&tnecd, sizeof (TNEXECCASEDATA));
	tnecd.dwSize = sizeof (TNEXECCASEDATA);
	tnecd.pExecutor = new (CTNExecutor)(this, pTest);
	if (tnecd.pExecutor == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	tnecd.iTesterNum = -1;
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		// If the ID of this tester is our ID, then we found our tester number.
		if (memcmp(&(pTest->m_paTesterSlots[i].id), &(this->m_id), sizeof (TNCTRLMACHINEID)) == 0)
		{
#ifdef DEBUG
			if (tnecd.iTesterNum >= 0)
			{
				DPL(0, "Duplicate tester number?!  DEBUGBREAK()-ing.", 0);

				DEBUGBREAK();

				hr = E_FAIL;
				goto DONE;
			} // end if (no tester number)
#endif // DEBUG

			tnecd.iTesterNum = i;
#ifndef DEBUG
			break; // get out of the loop
#endif // ! DEBUG
		} // end if (found our ID)
	} // end for (each tester)

#ifdef DEBUG
	if (tnecd.iTesterNum < 0)
	{
		DPL(0, "No tester number?!  DEBUGBREAK()-ing.", 0);

		DEBUGBREAK();

		hr = E_FAIL;
		goto DONE;
	} // end if (no tester number)
#endif // DEBUG

	tnecd.iNumMachines = pTest->m_iNumMachines;
	tnecd.pvInputData = pvInputData;
	tnecd.dwInputDataSize = dwInputDataSize;

	tnecd.pSubResults = &(this->m_results);
	tnecd.pFinalResult = tnecd.pSubResults->NewResult(pTest);
	if (tnecd.pFinalResult == NULL)
	{
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't create new result)
	tnecd.pMachineInfo = &(this->m_info);


	DPL(1, "Running test case \"%s-%s\", unique ID %u (\"%s\"), tester num %i (%i total testers), data = %x, %u bytes.",
			8, pTest->m_pCase->m_pszID,
			pTest->m_pszInstanceID,
			pTest->m_dwUniqueID,
			pTest->m_pCase->m_pszName,
			tnecd.iTesterNum,
			pTest->m_iNumMachines,
			pvInputData,
			dwInputDataSize);

#ifndef _XBOX

#pragma BUGBUG(vanceo, "What about ongoing tests?  They'll be top level")
	// Only log if it's a top level test and we're owned by the top level slave.
	if ((pTest->m_pParentTest == NULL) && (this->m_pOwningSlave != NULL))
	{
		hr = this->m_pOwningSlave->StartDBCase(pTest);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't notify database of test case start!", 0);
			goto DONE;
		} // end if (couldn't start case in DB)
	} // end if (it's a top level slave test)


#endif // ! XBOX

#ifndef _XBOX // no snapshots supported

#pragma TODO(vanceo, "Reimplement binary swapping")
	/*
	if (pTest->m_pCase->m_dwOptions & TNTCO_SWAPSBINARIES)
	{
		if (pTest->m_pCase->m_dwOptions & TNTCO_MULTITHREADABLE)
		{
			DPL(0, "Tests that swap binaries are not multithreadable!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (the case is multithreadable)

		hr = this->TakeSnapshot();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't take snapshot of binaries!", 0);
			goto DONE;
		} // end if (couldn't take snapshot)
	} // end if (the test may swap binaries)
	*/
#endif // ! XBOX

	hr = pTest->m_pCase->m_pfnExecCase(&tnecd);

	switch (hr)
	{
		case S_OK:
			// The test succeeded fine.
		  break;

		case TNSR_USERCANCEL:
		case TNWR_USERCANCEL:
		case TNCWR_USERCANCEL:
			// Convert user cancels from system failures to test result failures.

			DPL(0, "User cancelled test %s (unique ID %u), marking as failure.",
				2, pTest->m_pCase->m_pszID,
				pTest->m_dwUniqueID);

			//Ignoring error
			this->SprintfLogInternal(TNLST_CONTROLLAYER_TESTFAILURE,
									"User cancelled test \"%s\" (unique ID %u), marking as failure.",
									2, pTest->m_pCase->m_pszID,
									pTest->m_dwUniqueID);

			// Override anything the user may have set.
			tnecd.pFinalResult->SetResultCodeAndBools(hr, TRUE, FALSE);

#pragma BUGBUG(vanceo, "doesn't stop testing completely")
			hr = S_OK;
		  break;

		case TNWR_TIMEOUT:
		case TNCWR_TIMEOUT:
			// Convert timeouts from system failures to test result failures.

			DPL(0, "Timed out during %s (unique ID %u), marking as failure.",
				2, pTest->m_pCase->m_pszID,
				pTest->m_dwUniqueID);

			//Ignoring error
			this->SprintfLogInternal(TNLST_CONTROLLAYER_TESTFAILURE,
									"Timed out during \"%s\" (unique ID %u), marking as failure.",
									2, pTest->m_pCase->m_pszID,
									pTest->m_dwUniqueID);

			// Override anything the user may have set.
			tnecd.pFinalResult->SetResultCodeAndBools(hr, TRUE, FALSE);

			hr = S_OK;
		  break;

		case TNERR_LOSTTESTER:
		case TNSR_LOSTTESTER:
		case TNWR_LOSTTESTER:
		case TNCWR_LOSTTESTER:
			// Convert lost testers from system failures to test result failures.

			DPL(0, "Lost tester during %s (unique ID %u), marking as failure.",
				2, pTest->m_pCase->m_pszID,
				pTest->m_dwUniqueID);

			//Ignoring error
			this->SprintfLogInternal(TNLST_CONTROLLAYER_TESTFAILURE,
									"Lost tester during \"%s\" (unique ID %u), marking as failure.",
									2, pTest->m_pCase->m_pszID,
									pTest->m_dwUniqueID);

			// Override anything the user may have set.
			tnecd.pFinalResult->SetResultCodeAndBools(hr, TRUE, FALSE);

			hr = S_OK;
		  break;

		default:
			// All other failures will cause the test thread to abort.

			DPL(0, "Running test unique ID %u (\"%s\", case \"%s-%s\", tester %i, %i total testers), data = %x, %u bytes failed!",
				8, pTest->m_dwUniqueID,
				pTest->m_pCase->m_pszName,
				pTest->m_pCase->m_pszID,
				pTest->m_pszInstanceID,
				tnecd.iTesterNum,
				pTest->m_iNumMachines,
				pvInputData,
				dwInputDataSize);
			goto DONE;
		  break;

	} // end switch (on return code)

	DPL(1, "Done with test function (test %u, \"%s\", case \"%s-%s\", tester %i, %i total testers), data = %x, %u bytes.",
		8, pTest->m_dwUniqueID,
		pTest->m_pCase->m_pszName,
		pTest->m_pCase->m_pszID,
		pTest->m_pszInstanceID,
		tnecd.iTesterNum,
		pTest->m_iNumMachines,
		pvInputData,
		dwInputDataSize);

	// Note that the user has to save the info from the input data if he
	// wants to keep it around in an ongoing test.  If there was no data
	// stored there, it's harmless.
	pTest->FreeInputData();

#ifndef _XBOX // no swapping supported
	/*
	if (pTest->m_pCase->m_dwOptions & TNTCO_SWAPSBINARIES)
	{
		hr = this->RestoreSnapshot();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't restore snapshot of binaries!", 0);
			goto DONE;
		} // end if (couldn't restore snapshot)
	} // end if (the test may have swapped binaries)
	*/
#endif // ! XBOX

	// If the module didn't say that this was an ongoing test, but
	// the function did not say it was complete yet, that's goofy.
	if (! tnecd.pFinalResult->m_fComplete)
	{
		if (! (pTest->m_pCase->m_dwOptionFlags & TNTCO_ONGOING))
		{
			DPL(0, "Module does not appear to be finished with a non-ongoing test (ID %u, case \"%s\")!",
				2, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID);
			hr = E_FAIL;
			goto DONE;
		} // end if (not supposed to be an ongoing test)
	} // end if (the test is not done)

	if (! this->m_fDeadlockCheck)
	{
		if (pTest->m_pParentTest != NULL)
		{
			DPL(1, "NOTE: Leaving subtest (ID %u, case \"%s\") with deadlock checking still off.  May be okay if it was turned off in a parent test.",
				2, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID);
		} // end if (a sub test)
		else
		{
			DPL(0, "Leaving top level test (ID %u, case \"%s\") with deadlock checking still off!",
				2, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID);

			hr = ERROR_CAN_NOT_COMPLETE;
			goto DONE;
		} // end else (this is a top level test)
	} // end if (leaving test case with deadlock check still off)


	// We automatically generate a report for the module because it's something
	// he would have to do anyway and it's a side effect of our output data
	// mechanism.  This report function actually updates the test and moves it
	// as appropriate.
	hr = this->ReportInternal(pTest,
							tnecd.pFinalResult->m_hresult,
							tnecd.pFinalResult->m_fComplete,
							tnecd.pFinalResult->m_fSuccess,
							TRUE,
							((tnecd.pFinalResult->m_fSuccess) ? tnecd.pFinalResult->m_pvOutputData : NULL),
							((tnecd.pFinalResult->m_fSuccess) ? tnecd.pFinalResult->m_dwOutputDataSize : 0),
							((tnecd.pFinalResult->m_fSuccess) ? &(tnecd.pFinalResult->m_vars) : NULL));
	if (hr != S_OK)
	{
		DPL(0, "Reporting results for test unique ID %u failed!",
			1, pTest->m_dwUniqueID);
		goto DONE;
	} // end if (failed to report the results)


#pragma BUGBUG(vanceo, "This is ugly, don't have ReportInternal delete test")
	// Note that ReportInternal may delete pTest!



DONE:

	if (tnecd.pExecutor != NULL)
	{
		delete (tnecd.pExecutor);
		tnecd.pExecutor = NULL;
	} // end if (an executor exists)

	// If we had an error, check to see if the test wasn't already cleared by
	// ReportInternal.  If it wasn't, we'll do it ourselves.
	if (hr != S_OK)
	{
		if ((this->m_pTest != NULL) &&
			((pTest->m_pParentTest == NULL) ||
			((pTest->m_pParentTest != NULL) && (pTest->m_pParentTest->m_pSubTest != NULL))))
		{
			if (this->m_pTest == pTest)
			{
				this->m_pTest = NULL;
			} // end if (this is the top level current test)
			else if (pTest->m_pParentTest->m_pSubTest == pTest)
			{
				pTest->m_pParentTest->m_pSubTest = NULL;
			} // end else if (this is not a top level test)
			else
			{
				DPL(0, "WARNING: Test %x location not known!", 1, pTest);
			} // end else (other)

			pTest->m_dwRefCount--;
			if (pTest->m_dwRefCount == 0)
			{
				DPL(0, "Refcount for test %x hit 0, deleting.", 1, pTest);
				delete (pTest);
			} // end if (last reference to test)
			else
			{
				DPL(7, "Not deleting test %x, its refcount is %u.",
					2, pTest, pTest->m_dwRefCount);
			} // end else (not last reference to test)
			pTest = NULL;
		} // end if (test location wasn't already cleared)
		else
		{
			DPL(0, "Test %x already cleared, assuming it was freed.", 1, pTest);
		} // end else (test location was already cleared)
	} // end if (failed)

	return (hr);
} // CTNExecutorPriv::RunTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::FreeResultsInternal()"
//==================================================================================
// CTNExecutorPriv::FreeResultsInternal
//----------------------------------------------------------------------------------
//
// Description: Frees all results matching the given criteria, if specified.  If
//				both parameters are NULL, all results are freed.
//				Note that all output data and variables are destroyed as well.
//
// Arguments:
//	char* pszCaseID						Case ID to match, or NULL for all.
//	char* pszInstanceID					Instance ID to match, or NULL for all.
//	PTNTESTINSTANCES pUserCallingTest	Pointer to test where the user called this
//										explicitly, or NULL if automatic cleanup.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::FreeResultsInternal(char* pszCaseID, char* pszInstanceID,
											PTNTESTINSTANCES pUserCallingTest)
{
	HRESULT				hr = S_OK;
	PTNRESULT			pResult;
	PTNTESTINSTANCES	pParentTest;
	int					i;



	DPL(1, "Freeing results (%s, %s).", 2, pszCaseID, pszInstanceID);

	// Take the lock to prevent other threads from doing this, too.
	this->m_results.EnterCritSection();

	// Work backwards so our count doesn't get screwed up.
	for(i = this->m_results.Count() - 1; i >= 0; i--)
	{
		pResult = (PTNRESULT) this->m_results.GetItem(i);
		if (pResult == NULL)
		{
			DPL(0, "Couldn't get result %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)


		// If we're matching case ID, make sure it does.
		if (pszCaseID != NULL)
		{
			if (pResult->IsCase(pszCaseID))
				continue;
		} // end if (should check case ID)


		// If we're matching instance ID, make sure it does.
		if (pszInstanceID != NULL)
		{
			if ((pResult->m_pTest->m_pszInstanceID == NULL) ||
				(strcmp(pResult->m_pTest->m_pszInstanceID, pszInstanceID) != 0))
			{
				continue;
			} // end if (no instance ID, or doesn't match)
		} // end if (should check instance ID)



		// Since there are race conditions where a repetition complete message
		// comes in and we try to free all results before the ReportInternal
		// call that caused the repetition to complete tries to do the same thing,
		// we will skip results for tests marked TNTCO_DONTSAVERESULTS or for
		// subtests under one with that flag.  We assume the ReportInternal code
		// will free it when it gets the chance (should be very very soon).  This
		// will only occur and be handled in the non-user-called case.
		// However, we also don't want the user freeing results for the currently
		// executing test or any of its parents, so skip those as well.
		if (pUserCallingTest == NULL)
		{
			if (pResult->m_pTest->m_pCase->m_dwOptionFlags & TNTCO_DONTSAVERESULTS)
			{
				DPL(9, "Not auto-freeing result %x (for test %x, ID %u) because it's a DONTSAVERESULTS test and should be released shortly.",
					3, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID);
				continue;
			} // end if (results will not be saved)

			if (pResult->m_fUnderDontSaveTest)
			{
				DPL(9, "Not auto-freeing result %x (for test %x, ID %u) because it's a subtest of a DONTSAVERESULTS test.",
					3, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID);
				continue;
			} // end if (results will not be saved)
		} // end if (not user called)
		else
		{
			if (pResult->m_pTest == pUserCallingTest)
			{
				DPL(9, "Not user-freeing result %x because its the current test (%x, ID %u).",
					3, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID);
				continue;
			} // end if (results will not be saved)

			// Check parent tests, too.
			pParentTest = pUserCallingTest->m_pParentTest;
			while (pParentTest != NULL)
			{
				if (pResult->m_pTest == pParentTest)
				{
					DPL(9, "Not user-freeing result %x because its for a parent test (%x, ID %u) of the current test (%x, ID %u).",
						5, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID,
						pUserCallingTest, pUserCallingTest->m_dwUniqueID);
					// Stop this while loop, we'll detect that pParentTest is not NULL.
					break;
				} // end if (results will not be saved)

				pParentTest = pParentTest->m_pParentTest;
			} // end while (more parents)

			// Detect if we did find this result's test in the parent test chain.
			if (pParentTest != NULL)
				continue;
		} // end else (is user called)

#pragma BUGBUG(vanceo, "Are these rules also happening on server?  we don't want mismatch")

		// If this result is under a DONTSAVERESULTS test, we should take it off that
		// list to be polite so that code doesn't try to remove this result from the
		// main list (which we'll do ourselves here in a second).
		if (pResult->m_fUnderDontSaveTest)
		{
			pParentTest = pResult->m_pTest->m_pParentTest;
			while (pParentTest != NULL)
			{
				if (pParentTest->m_pResultsToFree != NULL)
				{
					hr = pParentTest->m_pResultsToFree->RemoveFirstReference(pResult);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't remove result %x from parent test %x's results to free list!",
							2, pResult, pParentTest);
						goto DONE;
					} // end if (couldn't remove result)

					pResult->m_fUnderDontSaveTest = FALSE;

					// Break out of the loop.
					break;
				} // end if (there's a results to free list)

				pParentTest = pParentTest->m_pParentTest;
			} // end while (more parents)

#ifdef DEBUG
			// Detect if we failed to find the parent test which will throw out this
			// result.
			if (pParentTest == NULL)
			{
				DPL(0, "Couldn't find parent test which will throw out result %x!",
					1, pResult);
				DEBUGBREAK();
				hr = E_FAIL;
				goto DONE;
			} // end if (didn't didn't find parent test)
#endif // DEBUG
		} // end if (under a dontsave test)


		// If we got here, it means we can remove this item.
		hr = this->m_results.RemoveFirstReference(pResult);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove reference to result %x!", 1, pResult);
			goto DONE;
		} // end if (couldn't remove ref)

		if (pResult->m_dwRefCount == 0)
		{
			DPL(7, "Deleting result %x (test = %x, ID %u).",
				3, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID);
			delete (pResult);
		} // end if (refcount hit 0)
		else
		{
			DPL(7, "Not deleting result %x (test = %x, ID %u), it's refcount is %u.",
				4, pResult, pResult->m_pTest, pResult->m_pTest->m_dwUniqueID,
				pResult->m_dwRefCount);
		} // end else (refcount didn't hit 0)
		pResult = NULL;
	} // end for (each result)


	// If the user called this, we need to propagate this up the chain,
	// all the way to the master.
	if (pUserCallingTest != NULL)
	{
		if (this->m_pOwningSlave != NULL)
		{
			hr = this->m_pOwningSlave->SendFreeOutputVars(pszCaseID, pszInstanceID,
															NULL, NULL);
			if (hr != S_OK)
			{
				DPL(0, "Failed sending FreeOutputVars message to master!", 0);
				goto DONE;
			} // end if (sending message to master failed)
		} // end if (owned by slave)
		else
		{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
			BOOL		fOldCheckStatus;

			fOldCheckStatus = this->m_fDeadlockCheck; 
			this->m_fDeadlockCheck = FALSE;

			hr = this->m_pOwningLeech->HaveOwnerFreeResults(pUserCallingTest,
															pszCaseID,
															pszInstanceID);

			this->m_fDeadlockCheck = fOldCheckStatus;

			if (hr != S_OK)
			{
				DPL(0, "Failed having leech's owner free results!", 0);
				goto DONE;
			} // end if (having leech do stuff)
#endif // ! XBOX
		} // end else (owned by leech)
	} // end if (the user called this function)


DONE:

	this->m_results.LeaveCritSection();

	return (hr);
} // CTNExecutorPriv::FreeResultsInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::FreeOutputVarsInternal()"
//==================================================================================
// CTNExecutorPriv::FreeOutputVarsInternal
//----------------------------------------------------------------------------------
//
// Description: Frees all output variables matching the given criteria.  If a
//				parameter is not NULL, then only vars from the corresponding test,
//				with a matching name, etc. are freed.  If all parameters are NULL,
//				all output vars are freed.
//
// Arguments:
//	char* pszCaseID						Case ID to match, or NULL for all.
//	char* pszInstanceID					Instance ID to match, or NULL for all.
//	char* pszName						Output variable name to match, or NULL for
//										all.
//	char* pszType						Output variable type to match, or NULL for
//										all.
//	BOOL fFreeData						Whether the data can be freed, too, if no
//										variables are left for a given result.
//	PTNTESTINSTANCES pUserCallingTest	Pointer to test where the user called this
//										explicitly, or NULL if automatic cleanup.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::FreeOutputVarsInternal(char* pszCaseID, char* pszInstanceID,
												char* pszName, char* pszType,
												BOOL fFreeData,
												PTNTESTINSTANCES pUserCallingTest)
{
	HRESULT			hr = S_OK;
	PTNRESULT		pResult;
	PTNOUTPUTVAR	pVar;
	int				i;
	int				j;


	DPL(9, "==>", 0);

	DPL(1, "Freeing output variables (%s, %s, %s, %s, %B, %x).",
		6, pszCaseID, pszInstanceID, pszName, pszType, fFreeData, pUserCallingTest);

	this->m_results.EnterCritSection();

	for(i = 0; i < this->m_results.Count(); i++)
	{
		pResult = (PTNRESULT) this->m_results.GetItem(i);
		if (pResult == NULL)
		{
			DPL(0, "Couldn't get result %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// If we're matching case ID, make sure it does.
		if (pszCaseID != NULL)
		{
			if (pResult->IsCase(pszCaseID))
				continue;
		} // end if (should check case ID)


		// If we're matching instance ID, make sure it does.
		if (pszInstanceID != NULL)
		{
			if ((pResult->m_pTest->m_pszInstanceID == NULL) ||
				(strcmp(pResult->m_pTest->m_pszInstanceID, pszInstanceID) != 0))
			{
				continue;
			} // end if (no instance ID, or doesn't match)
		} // end if (should check instance ID)


		// If we're matching individual variable names or types, check each one.
		if ((pszName != NULL) || (pszType != NULL))
		{
			// If there aren't any variables in this item, we don't need or want to
			// check if we can free vars and data.
			if (pResult->m_vars.Count() <= 0)
			{
				continue;
			} // end if (still variables left)


			// Loop through all the output variables (backwards so our
			// count doesn't get off).
			for(j = pResult->m_vars.Count() - 1; j >= 0; j--)
			{
				pVar = (PTNOUTPUTVAR) pResult->m_vars.GetItem(j);
				if (pVar == NULL)
				{
					DPL(0, "Couldn't get var %i!", 1, j);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// Check for matching names and types.
				if (((pszName != NULL) &&
						(StringCmpNoCase(pVar->GetString(), pszName))) ||
					((pszType != NULL) &&
						(strcmp(pVar->m_pszType, pszType) == 0)))
				{
					// Remove it.
					hr = pResult->m_vars.Remove(j);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't remove item %i from test %u's output vars list!",
							2, j, pResult->m_pTest->m_dwUniqueID);
						goto DONE;
					} // end if (couldn't remove item)
				} // end if (matching names and it does)
			} // end for (each output variable)

			// If we didn't remove all the variables, we can't remove
			// the whole list.
			if (pResult->m_vars.Count() > 0)
			{
				continue;
			} // end if (still variables left)
		} // end if (should check names or types)


		// If we got here, it means we can remove any output variables left.
		hr = pResult->m_vars.RemoveAll();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove any remaining output vars in test %u's output vars list!",
				1, pResult->m_pTest->m_dwUniqueID);
			goto DONE;
		} // end if (couldn't remove item)

		// If we can free the data, do that now, too.   This will try to
		// remove all the variables yet again, but oh well.
		if (fFreeData)
		{
			pResult->DestroyOutputDataBufferAndVars();
		} // end if (we can free the data)
	} // end for (each result)


	// If the user called this, we need to propagate this up the chain,
	// all the way to the master.
	if (pUserCallingTest != NULL)
	{
		if (this->m_pOwningSlave != NULL)
		{
			hr = this->m_pOwningSlave->SendFreeOutputVars(pszCaseID, pszInstanceID,
															pszName, pszType);
			if (hr != S_OK)
			{
				DPL(0, "Failed sending FreeOutputVars message to master!", 0);
				goto DONE;
			} // end if (sending message to master failed)
		} // end if (owned by slave)
		else
		{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
			BOOL		fOldCheckStatus;

			fOldCheckStatus = this->m_fDeadlockCheck; 
			this->m_fDeadlockCheck = FALSE;

			hr = this->m_pOwningLeech->HaveOwnerFreeOutputVars(pUserCallingTest,
																pszCaseID,
																pszInstanceID,
																pszName,
																pszType,
																fFreeData);

			this->m_fDeadlockCheck = fOldCheckStatus;

			if (hr != S_OK)
			{
				DPL(0, "Failed having leech's owner free output vars!", 0);
				goto DONE;
			} // end if (having leech do stuff)
#endif // ! XBOX
		} // end else (owned by leech)
	} // end if (the user called this function)


DONE:

	this->m_results.LeaveCritSection();

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutorPriv::FreeOutputVarsInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::GetTestersMachineInfoInternal()"
//==================================================================================
// CTNExecutorPriv::GetTestersMachineInfoInternal
//----------------------------------------------------------------------------------
//
// Description: Internal version of machine info retrieval.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to retrieve COM port under.
//	int iTesterNum			Tester number for which to retrieve COM port.
//	PTNMACHINEINFO* ppInfo	Place to store result in.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::GetTestersMachineInfoInternal(PTNTESTINSTANCES pTest,
														int iTesterNum,
														PTNMACHINEINFO* ppInfo)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


#pragma BUGBUG(vanceo, "does this belong here?"
	// Update the control layer to let it know we're still alive
	this->m_dwLastTestthreadPing = GetTickCount();


	if (this->m_pOwningSlave == NULL)
	{
#ifndef _XBOX // no IPC supported

#pragma BUGBUG(vanceo, "This is a hack, do this right")
		BOOL		fOldCheckStatus;

		fOldCheckStatus = this->m_fDeadlockCheck; 
		this->m_fDeadlockCheck = FALSE;

		hr = this->m_pOwningLeech->HaveOwnerGetTestersMachineInfo(pTest,
																iTesterNum,
																ppInfo);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have slave get tester's machine info through leech!", 0);
		} // end if (getting machine info failed)

		this->m_fDeadlockCheck = fOldCheckStatus;

#endif // ! XBOX
	} // end if (not owned by slave)
	else
	{
		hr = this->m_pOwningSlave->GetTestersMachineInfoInternal(pTest,
																iTesterNum,
																ppInfo);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't get tester's machine info!", 0);
		} // end if (getting machine info failed)
	} // end else (owned by slave)

	return (hr);
} // CTNExecutorPriv::GetTestersMachineInfoInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutorPriv::DoDocumentation()"
//==================================================================================
// CTNExecutorPriv::DoDocumentation
//----------------------------------------------------------------------------------
//
// Description: Performs the documentation retrieval.
//
// Arguments:
//	PTNTESTTABLEITEM pItem	Pointer to group or item to execute for documentation.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutorPriv::DoDocumentation(PTNTESTTABLEITEM pItem)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLEITEM	pSubItem;
	PTNTESTINSTANCES	pTest = NULL;
	TNEXECCASEDATA		tnecd;
	int					i;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	ZeroMemory(&tnecd, sizeof (TNEXECCASEDATA));


	if (pItem->m_fGroup)
	{
#pragma TODO(vanceo, "Document the group?")

		// Get documentation on all the subitems in the group.
		for(i = 0; i < ((PTNTESTTABLEGROUP) pItem)->m_items.Count(); i++)
		{
			pSubItem = (PTNTESTTABLEITEM) ((PTNTESTTABLEGROUP) pItem)->m_items.GetItem(i);
			if (pSubItem == NULL)
			{
				DPL(0, "Couldn't get subitem %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get sub item)

			hr = this->DoDocumentation(pSubItem);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't document subitem %i!", 1, i);
				goto DONE;
			} // end if (couldn't document)
		} // end for (each item)
	} // end if (group)
	else
	{
		// Skip the built-in test cases.
		if (! (((PTNTESTTABLECASE) pItem)->m_dwOptionFlags & TNTCO_BUILTIN))
		{
			pTest = new (CTNTestInstanceS)(0xFFFFFFFF,
											(PTNTESTTABLECASE) pItem,
											NULL,
											-666, // special value
											NULL,
											//0,
											NULL);
			if (pTest == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)
			pTest->m_dwRefCount++; // we're using it

			tnecd.dwSize = sizeof (TNEXECCASEDATA);
			tnecd.pExecutor = new (CTNExecutor)(this, pTest);
			if (tnecd.pExecutor == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			tnecd.iTesterNum = -1;
			//tnecd.pvInputData = NULL;
			//tnecd.dwInputDataSize = 0;
			//tnecd.pSubResults = NULL;
			//tnecd.pFinalResult = NULL;
			//tnecd.pMachineInfo = NULL;

#ifndef DONT_CATCH_DOC_EXCEPTIONS
			try
			{
#endif // ! DONT_CATCH_DOC_EXCEPTIONS
				DPL(5, "Trying to document case \"%s\".", 1, pTest->m_pCase->m_pszID);

				hr = pTest->m_pCase->m_pfnExecCase(&tnecd);
				if (hr != S_OK)
				{
					DPL(0, "WARNING: Case \"%s\" didn't return S_OK for documentation!  %e",
						2, pTest->m_pCase->m_pszID, hr);

					this->SprintfLogInternal(TNLST_MODULE_IMPORTANT,
										"WARNING: Case \"%s\" didn't return S_OK for documentation!  %e",
										2, pTest->m_pCase->m_pszID, hr);
					hr = S_OK;
				} // end if (case didn't return okay)
#ifndef DONT_CATCH_DOC_EXCEPTIONS
			} // end try
			catch (...)
			{
				DPL(0, "WARNING: Case \"%s\" caused an exception, ignoring!",
					1, pTest->m_pCase->m_pszID);

				this->SprintfLogInternal(TNLST_MODULE_IMPORTANT,
									"WARNING: Case \"%s\" caused an exception, ignoring!",
									1, pTest->m_pCase->m_pszID);
				hr = S_OK;
			} // end catch (generic)
#endif // ! DONT_CATCH_DOC_EXCEPTIONS
		} // end if (not a built-in case)
	} // end else (case)


DONE:

	if (tnecd.pExecutor != NULL)
	{
		delete (tnecd.pExecutor);
		tnecd.pExecutor = NULL;
	} // end if (have executor object)

	if (pTest != NULL)
	{
		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test %x.", 1, pTest);
			delete (pTest);
		} // end if (can delete test)
		else
		{
			DPL(0, "Not deleting test %x, it's refcount is %u!?  DEBUGBREAK()-ing.",
				2, pTest, pTest->m_dwRefCount);
			DEBUGBREAK();
		} // end else (can't delete test)
		pTest = NULL;
	} // end if (have test object)

	return (hr);
} // CTNExecutorPriv::DoDocumentation
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::CTNExecutor()"
//==================================================================================
// CTNExecutor constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNExecutor object.  Initializes the data structures.
//
// Arguments:
//	PTNEXECUTORPRIV pRealExecutor	Pointer to real executor object this interface
//									represents.
//	PTNTESTINSTANCES pTest			Pointer to current test.
//
// Returns: None (just the object).
//==================================================================================
CTNExecutor::CTNExecutor(PTNEXECUTORPRIV pRealExecutor, PTNTESTINSTANCES pTest):
	m_pRealExecutor(pRealExecutor),
	m_pTest(NULL),
	m_pszTestSectionName(NULL),
	m_pszTestSectionFilepath(NULL),
	m_iTestSectionLineNum(-1)
{
	DPL(2, "this = %x, sizeof (this) = %i, test = %x",
		3, this, sizeof (CTNExecutor), pTest);

	pTest->m_dwRefCount++;
	this->m_pTest = pTest;
} // CTNExecutor::CTNExecutor
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::~CTNExecutor()"
//==================================================================================
// CTNExecutor destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNExecutor object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNExecutor::~CTNExecutor(void)
{
	DPL(9, "this = %x, test = %x", 2, this, this->m_pTest);


	this->m_pTest->m_dwRefCount--;
	if (this->m_pTest->m_dwRefCount == 0)
	{
		DPL(2, "Deleting owned test %x.", 1, this->m_pTest);
		delete (this->m_pTest);
	} // end if (refcount hit 0)
	else
	{
		DPL(2, "Not deleting owned test %x, its refcount is %u.",
			2, this->m_pTest, this->m_pTest->m_dwRefCount);
	} // end else (refcount hasn't hit 0)

	this->m_pTest = NULL;


	if (this->m_pszTestSectionName != NULL)
	{
		LocalFree(this->m_pszTestSectionName);
		this->m_pszTestSectionName = NULL;
	} // end if (have string)

	if (this->m_pszTestSectionFilepath != NULL)
	{
		LocalFree(this->m_pszTestSectionFilepath);
		this->m_pszTestSectionFilepath = NULL;
	} // end if (have string)
} // CTNExecutor::~CTNExecutor
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::IsCase()"
//==================================================================================
// CTNExecutor::IsCase
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the currently executing test is based on the given
//				case ID, FALSE otherwise.
//
// Arguments:
//	char* szCaseID	Case ID to check.
//
// Returns: TRUE if test is from case, FALSE otherwise.
//==================================================================================
BOOL CTNExecutor::IsCase(char* szCaseID)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (FALSE);
	} // end if (bad object pointer)

	if (szCaseID == NULL)
	{
		DPL(0, "Case ID string is NULL!", 0);
		return (FALSE);
	} // end if (bad case ID)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	return ((strcmp(this->m_pTest->m_pCase->m_pszID, szCaseID) == 0) ? TRUE : FALSE);
} // CTNExecutor::IsCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetTestingWindow()"
//==================================================================================
// CTNExecutor::GetTestingWindow
//----------------------------------------------------------------------------------
//
// Description: Returns a handle to the window this is executing under.
//
// Arguments: None.
//
// Returns: Handle to window, or NULL if failed.
//==================================================================================
HWND CTNExecutor::GetTestingWindow(void)
{
	HRESULT		hr;
	HWND		hWnd = NULL;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (NULL);
	} // end if (bad object pointer)


	if (this->m_pRealExecutor->m_pfnGetTestingWindow == NULL)
	{
		DPL(0, "GetTestingWindow is not supported by shell!", 0);
		return (NULL);
	} // end if (callback doesn't exist)

#ifndef _XBOX // no windowing supported

	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->m_pfnGetTestingWindow(&hWnd);

	if (hr != S_OK)
	{
		DPL(0, "Couldn't get testing window!  %e", 1, hr);
		return (NULL);
	} // end if (failed getting window)

#endif // ! XBOX

	return (hWnd);
} // CTNExecutor::GetTestingWindow
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FirstBinIsVersion()"
//==================================================================================
// CTNExecutor::FirstBinIsVersion
//----------------------------------------------------------------------------------
//
// Description: Sets the passed in boolean to TRUE if the first occurrence of a
//				binary is the specified version, FALSE otherwise.
//
// Arguments:
//	char* szBinaryName		Binary name to examine.
//	DWORD dwHighVersion		High DWORD of file version to compare.
//	DWORD dwLowVersion		Low DWORD of file version to compare.
//	BOOL* lpfResult			Pointer to place to store result.
//
// Returns: S_OK if trying to compare was successful, error code otherwise.  Note
//			lpbResult holds the actual comparison result.
//==================================================================================
HRESULT CTNExecutor::FirstBinIsVersion(char* szBinaryName, DWORD dwHighVersion,
										DWORD dwLowVersion, BOOL* lpfResult)
{
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	return (this->m_pRealExecutor->m_info.m_binaries.FirstBinIsVersion(szBinaryName,
																		dwHighVersion,
																		dwLowVersion,
																		lpfResult));
} // CTNExecutor::FirstBinIsVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FirstBinIsAtLeastVersion()"
//==================================================================================
// CTNExecutor::FirstBinIsAtLeastVersion
//----------------------------------------------------------------------------------
//
// Description: Sets the passed in boolean to TRUE if the first occurrence of a
//				binary is the specified version or higher, FALSE otherwise.
//
// Arguments:
//	char* szBinaryName		Binary name to examine.
//	DWORD dwHighVersion		High DWORD of file version to compare.
//	DWORD dwLowVersion		Low DWORD of file version to compare.
//	BOOL* lpfResult			Pointer to place to store result.
//
// Returns: S_OK if trying to compare was successful, error code otherwise.  Note
//			lpbResult holds the actual comparison result.
//==================================================================================
HRESULT CTNExecutor::FirstBinIsAtLeastVersion(char* szBinaryName, DWORD dwHighVersion,
											  DWORD dwLowVersion, BOOL* lpfResult)
{
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	return (this->m_pRealExecutor->m_info.m_binaries.FirstBinIsAtLeastVersion(szBinaryName,
																				dwHighVersion,
																				dwLowVersion,
																				lpfResult));
} // CTNExecutor::FirstBinIsAtLeastVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::Log()"
//==================================================================================
// CTNExecutor::Log
//----------------------------------------------------------------------------------
//
// Description: Logs the passed in string to the shell.  The default level is
//				TNLST_MODULE_NORMAL, but if one of TNLF_CRITICAL, TNLF_IMPORTANT, or
//				TNLF_VERBOSE is specified, the level will be adjusted.
//				The string will be parsed for special tokens using the usual
//				routine, see tncommon\sprintf.cpp for possible arguments.
//				Depending on the style flags passed in, some prefixes may be
//				prepended to the string.
//
// Arguments:
//	DWORD dwFlags			Flags describing the string (TNLF_xxx).
//	char* szFormatString	String with optional special tokens to log.
//	DWORD dwNumParms		Number of parameters in the following variable parameter
//							list.
//	...						Variable parameter list; items are interpreted as the
//							special token replacements.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::Log(DWORD dwFlags, char* szFormatString, DWORD dwNumParms, ...)
{
	HRESULT		hr;
	DWORD		dwNumPrefixItems = 0;
	char		szPrefixFormat[256];
	char*		pszPrefixedFormatString = NULL;
	PVOID*		papvParams = NULL;
	va_list		currentparam;
	DWORD		dwCurrentItem = 0;
	char*		pszLogString = NULL;
	DWORD		dwLogStringType = TNLST_MODULE_NORMAL;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(this->m_pRealExecutor->m_pOwningSlave->m_pfnLogString == NULL))
	{
		// If we're on the owning slave, and he doesn't implement a log string
		// function, we're done.
		hr = S_OK;
		goto DONE;
	} // end if (not owned by slave)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	// Start with no prefix
	strcpy(szPrefixFormat, "");


	// Build prefix string

	if (dwFlags & TNLF_PREFIX_CASEANDINSTANCEID)
	{
		strcat(szPrefixFormat, "Case %s-%s: ");
		dwNumPrefixItems += 2;
	} // end if (should prefix case ID)

	if (dwFlags & TNLF_PREFIX_CASENAME)
	{
		strcat(szPrefixFormat, "\"%s\": ");
		dwNumPrefixItems++;
	} // end if (should prefix case name)

	if (dwFlags & TNLF_PREFIX_TESTUNIQUEID)
	{
		strcat(szPrefixFormat, "Test %u: ");
		dwNumPrefixItems++;
	} // end if (should prefix unique ID)


	pszPrefixedFormatString = (char*) LocalAlloc(LPTR, (strlen(szPrefixFormat) + strlen(szFormatString) + 1));
	if (pszPrefixedFormatString == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszPrefixedFormatString, szPrefixFormat);
	strcat(pszPrefixedFormatString, szFormatString);


	papvParams = (PVOID*) LocalAlloc(LPTR, ((dwNumPrefixItems + dwNumParms) * sizeof (PVOID)));
	if (papvParams == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)


	// Attach the prefix item(s) to the param array for passing to sprintf.

	if (dwFlags & TNLF_PREFIX_CASEANDINSTANCEID)
	{
		papvParams[dwCurrentItem] = this->m_pTest->m_pCase->m_pszID;
		dwCurrentItem++;
		papvParams[dwCurrentItem] = this->m_pTest->m_pszInstanceID;
		dwCurrentItem++;
	} // end if (should prefix case ID)

	if (dwFlags & TNLF_PREFIX_CASENAME)
	{
		papvParams[dwCurrentItem] = this->m_pTest->m_pCase->m_pszName;
		dwCurrentItem++;
	} // end if (should prefix case name)

	if (dwFlags & TNLF_PREFIX_TESTUNIQUEID)
	{
		papvParams[dwCurrentItem] = (PVOID) this->m_pTest->m_dwUniqueID;
		dwCurrentItem++;
	} // end if (should prefix unique ID)


#ifndef _XBOX // no TNsprintf functions
	// Loop through the variable arguments and put them into the array too.
	va_start(currentparam, dwNumParms);
	while(dwCurrentItem < (dwNumPrefixItems + dwNumParms))
	{
		papvParams[dwCurrentItem] = va_arg(currentparam, PVOID);
		dwCurrentItem++;
	} // end while (there are more variable parameter)
	va_end(currentparam);


	TNsprintf_array(&pszLogString, pszPrefixedFormatString,
					(dwNumPrefixItems + dwNumParms), papvParams);


	if (dwFlags & TNLF_CRITICAL)
		dwLogStringType = TNLST_CRITICAL;
	else if (dwFlags & TNLF_IMPORTANT)
		dwLogStringType = TNLST_MODULE_IMPORTANT;
	else if (dwFlags & TNLF_VERBOSE)
		dwLogStringType = TNLST_MODULE_VERBOSE;


	// Log the string
	hr = this->m_pRealExecutor->LogInternal(dwLogStringType, pszLogString);
	if (hr != S_OK)
	{
		DPL(0, "Internal logging failed!", 0);
		goto DONE;
	} // end if (log failed)
#else // ! XBOX
#pragma TODO(tristanj, "Need to map executor logging to xLog")
#endif // XBOX

DONE:

	if (pszLogString != NULL)
		TNsprintf_free(&pszLogString);

	if (papvParams != NULL)
	{
		LocalFree(papvParams);
		papvParams = NULL;
	} // end if (have item array)

	if (pszPrefixedFormatString != NULL)
	{
		LocalFree(pszPrefixedFormatString);
		pszPrefixedFormatString = NULL;
	} // end if (have prefixed format string)

	return (hr);
} // CTNExecutor::Log
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::Report()"
//==================================================================================
// CTNExecutor::Report
//----------------------------------------------------------------------------------
//
// Description: Reports checkpoint results for a test (a particular segment was a
//				success or failure, but testing isn't complete yet).
//
// Arguments:
//	HRESULT hresult		Success or error code.
//	BOOL fSuccess		Is this report a success (note this does not necessarily
//						mean hresult is 0)?
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::Report(HRESULT hresult, BOOL fSuccess)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not reporting because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->ReportInternal(this->m_pTest, hresult,
												FALSE, fSuccess, FALSE,
												NULL, 0, NULL);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't report!", 0);
	} // end if (reporting failed)

	return (hr);
} // CTNExecutor::Report
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::Warn()"
//==================================================================================
// CTNExecutor::Warn
//----------------------------------------------------------------------------------
//
// Description: Reports a non-fatal warning for a test.
//
// Arguments:
//	HRESULT hresult			Warning hresult code.
//	PVOID pvUserData		Pointer to optional user data to associate with warning.
//	DWORD dwUserDataSize	Size of user data for warning.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::Warn(HRESULT hresult, PVOID pvUserData, DWORD dwUserDataSize)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not warning because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->WarnInternal(this->m_pTest, hresult, pvUserData,
											dwUserDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't warn!", 0);
	} // end if (warning failed)

	return (hr);
} // CTNExecutor::Warn
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::WaitForEventOrCancel()"
//==================================================================================
// CTNExecutor::WaitForEventOrCancel
//----------------------------------------------------------------------------------
//
// Description: Waits for any of the passed in events to get signalled, the user to
//				cancel, or any of the passed in testers to be lost.
//				If an event got signalled, this function returns TNWR_GOTEVENT.  If
//				piItemNum is not NULL, then the integer it points to is set to the
//				zero-based index of the event that fired.
//				If pahEvents is NULL, then this function acts like a glorified
//				Sleep for the timeout given.
//				If a tester in the passed in array drops or leaves the test, this
//				function returns TNWR_LOSTTESTER.  If piItemNum is not NULL, then
//				the integer it points to is set to the tester number who was lost.
//				paiRelevantTesters can be NULL and iNumRelevantTesters be -1 to have
//				all other testers be relevant.  Specifying this slave's own tester
//				number is ignored.
//				This wait is IO Completion and TAPI friendly (it uses
//				WaitForMultipleObjectsEx and has a message pump).
//
// Arguments:
//	HANDLE* pahEvents			Array of events to wait for, or NULL to simulate a
//								Sleep.
//	int iNumEvents				Number of events in previous array.
//	int* paiRelevantTesters		Array of tester numbers that should cause this
//								function to fail if they drop, if any.
//	int iNumRelevantTesters		Number of testers in previous array, or -1 to
//								use all other testers..
//	DWORD dwTimeout				How long to wait before returning TNWR_TIMEOUT.
//	int* piItemNum				Optional place to store event signalled or tester
//								lost.
//
// Returns: TNWR_GOTEVENT if successful, or TNWR_xxx error code otherwise.
//==================================================================================
HRESULT CTNExecutor::WaitForEventOrCancel(HANDLE* pahEvents, int iNumEvents,
										int* paiRelevantTesters, int iNumRelevantTesters,
										DWORD dwTimeout, int* piItemNum)
{
	HRESULT		hr = S_OK;
	BOOL		fHaveTestMasterOpLock = FALSE;
	int			i = 0;
	int			iNumWaitObjects = 0;
	HANDLE*		pahWaitObjects = NULL;
	DWORD		dwCurrentTime = 0;
	DWORD		dwStartTime = 0;
	DWORD		dwInterval;
#ifndef _XBOX // no messaging supported
	MSG			msg;
#endif // ! XBOX

	if (this == NULL)
	{
		DPL(0, "Slave object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (slave object is invalid)

	if (iNumEvents < 0)
	{
		DPL(0, "Test ID %u called function with invalid number of handles (%i)!",
			2, this->m_pTest->m_dwUniqueID, iNumEvents);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if ((iNumRelevantTesters < -1) || (iNumRelevantTesters >= this->m_pTest->m_iNumMachines))
	{
		DPL(0, "Test ID %u called function with invalid number of relevant testers (%i is <-1 or >=%i)!",
			3, this->m_pTest->m_dwUniqueID, iNumRelevantTesters,
			this->m_pTest->m_iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if ((paiRelevantTesters != NULL) && (iNumRelevantTesters == -1))
	{
		DPL(0, "Test ID %u called function with array of relevant testers but the all-other-testers special number!",
			1, this->m_pTest->m_dwUniqueID);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if ((iNumRelevantTesters > 0) && (paiRelevantTesters == NULL))
	{
		DPL(0, "Test ID %u called function specifying %i relevant testers but no array!",
			2, this->m_pTest->m_dwUniqueID, iNumRelevantTesters);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	// Remap INFINITE and other really long timeouts to a maximum time.
	if (dwTimeout == INFINITE)
	{
		DPL(0, "WARNING: Converting WaitForEventOrCancel timeout from INFINITE to %u ms.",
			1, MAX_WAITFOREVENTORCANCEL_TIME);
		dwTimeout = MAX_WAITFOREVENTORCANCEL_TIME;
	} // end if (wait forever)
	else if (dwTimeout > MAX_WAITFOREVENTORCANCEL_TIME)
	{
		DPL(0, "WARNING: Capping specified WaitForEventOrCancel timeout (%u) at %u ms.",
			2, dwTimeout, MAX_WAITFOREVENTORCANCEL_TIME);
		dwTimeout = MAX_WAITFOREVENTORCANCEL_TIME;
	} // end if (wait forever)
#pragma TODO(vanceo, "Document this behavior.  Any situtations where we may need to wait longer?")



	EnterCriticalSection(&(this->m_pTest->m_csMasterOp));
	fHaveTestMasterOpLock = TRUE;

	// We need to set a request ID (even though we won't actually use it) so that the
	// HandleLostTester code will know to ping our event.
	this->m_pTest->m_dwRequestID = 0xFFFFFFFF;


	if (this->m_pTest->m_hResponseEvent != NULL)
	{
		DPL(0, "Test ID %u's response event already in use?!",
			1, this->m_pTest->m_dwUniqueID);
		hr = ERROR_ALREADY_EXISTS;
		goto DONE;
	} // end if (we're already syncing)

	this->m_pTest->m_hResponseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_pTest->m_hResponseEvent == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't create lost tester event for test ID %u!",
			1, this->m_pTest->m_dwUniqueID);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't create event)


	// If there's an array of relevant testers, validate it and make sure none
	// of those testers has already gone.
	if (iNumRelevantTesters > 0)
	{
		for(i = 0; i < iNumRelevantTesters; i++)
		{
			// Make sure it's in bounds.
			if ((paiRelevantTesters[i] < 0) ||
				(paiRelevantTesters[i] >= this->m_pTest->m_iNumMachines))
			{
				DPL(0, "Relevant tester at index %i is invalid (%i is <0 or >=%i)!",
					3, i, paiRelevantTesters[i], this->m_pTest->m_iNumMachines);
				hr = ERROR_INVALID_PARAMETER;
				goto DONE;
			} // end if (invalid number)

			// Make sure this tester hasn't already left.
			if (this->m_pTest->m_paTesterSlots[paiRelevantTesters[i]].fGone)
			{
				DPL(0, "Tester number %i (array index %i) is already gone.",
					2, paiRelevantTesters[i], i);

				if (piItemNum != NULL)
					(*piItemNum) = paiRelevantTesters[i];

				hr = TNWR_LOSTTESTER;
				goto DONE;
			} // end if (tester already gone)
		} // end for (each relevant tester)
	} // end if (there should be an array of relevant testers)
	else if (iNumRelevantTesters == -1)
	{
		// All testers are relevant, so make sure none are already gone.
		for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
		{
			// Make sure this tester hasn't already left.
			if (this->m_pTest->m_paTesterSlots[i].fGone)
			{
				DPL(0, "Tester number %i is already gone.",
					1, i);

				if (piItemNum != NULL)
					(*piItemNum) = i;

				hr = TNWR_LOSTTESTER;
				goto DONE;
			} // end if (tester already gone)
		} // end for (each tester)
	} // end else if (all testers are relevant)
	


	i = 0;

	iNumWaitObjects = iNumEvents + 1; // + 1 for lost tester event
	if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
		iNumWaitObjects++;

	pahWaitObjects = (HANDLE*) LocalAlloc(LPTR, ((iNumWaitObjects + 1) * (sizeof (HANDLE))));
	if (pahWaitObjects == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't set it)

	pahWaitObjects[i++] = this->m_pTest->m_hResponseEvent;

	if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
		pahWaitObjects[i++] = this->m_pRealExecutor->m_hUserCancelEvent;

	// If the user specified events, add them to the array, too.
	if (pahEvents > 0)
	{
		while (i < iNumWaitObjects)
		{
			pahWaitObjects[i] = pahEvents[i - 2];
			i++;
		} // end for (each tester in the param list)
	} // end if (there are event)

	pahWaitObjects[i] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this causes kernel to rewalk array



REWAIT:
	// Make sure the test thread still looks alive
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


#ifndef _XBOX // no message functions supported
	// Since TAPI requires a message pump, I've built a simple (hacky) one into
	// this Wait in case the user was expecting a message to arrive...
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		DPL(1, "Got Windows message.", 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end if (there's a Windows message)
#else // !XBOX
#pragma TODO(tristanj, "Removing message pump may affect things if TAPI isn't removed")
#endif // XBOX

	if (dwTimeout == INFINITE)
	{
		dwInterval = MESSAGE_PUMP_CHECK_INTERVAL;
	} // end if (wait forever)
	else
	{
#ifndef _XBOX // timeGetTime not supported
		dwCurrentTime = timeGetTime();
#else // ! XBOX
		dwCurrentTime = GetTickCount();
#endif // XBOX
		if (dwStartTime == 0)
			dwStartTime = dwCurrentTime;

		// If we reached or went over the timeout, we have to bail.
		if ((dwCurrentTime - dwStartTime) >= dwTimeout)
		{
			DPL(2, "Test ID %u's WaitForEventOrCancel timed out.",
				1, this->m_pTest->m_dwUniqueID);

			hr = TNWR_TIMEOUT;
			goto DONE;
		} // end if (exceeded timeout)


		// If we can't fit a whole interval in, just use the remainder of the time
		// allowed by the given timeout.
		if ((dwCurrentTime - dwStartTime) + MESSAGE_PUMP_CHECK_INTERVAL > dwTimeout)
			dwInterval = dwTimeout - (dwCurrentTime - dwStartTime);
		else
			dwInterval = MESSAGE_PUMP_CHECK_INTERVAL;
	} // end else (don't wait forever)


	hr = WaitForMultipleObjectsEx(iNumWaitObjects, pahWaitObjects, FALSE,
								dwInterval, TRUE);
	switch (hr)
	{
		case WAIT_IO_COMPLETION:
			// This is ugly, but it's how you have to do it.
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_TIMEOUT:
			goto REWAIT;
		  break;

		case WAIT_FAILED:
			hr = GetLastError();

			DPL(0, "Wait failed!  %e", 1, hr);

			if (hr == S_OK)
				hr = E_FAIL;

			goto DONE;
		  break;

		case WAIT_OBJECT_0:
			if (this->m_pTest->m_hrResponseResult != TNERR_LOSTTESTER)
			{
				hr = this->m_pTest->m_hrResponseResult;

				DPL(0, "Got unexpected LostTester event firing!  %e",
					1, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (not a lost tester error)


			// See which tester was lost (and if we care about him).
			if (iNumRelevantTesters == -1)
			{
				for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
				{
					if (this->m_pTest->m_paTesterSlots[i].fGone)
					{
						DPL(0, "Lost tester %i during WaitForEventOrCancel in test ID %u!",
							2, i, this->m_pTest->m_dwUniqueID);

						// Note the tester who was lost if the user wants it.
						if (piItemNum != NULL)
							(*piItemNum) = i;

						hr = TNWR_LOSTTESTER;
						goto DONE;
					} // end if (found a tester who's gone)
				} // end for (each tester)
			} // end if (we care about all of the other testers)
			else if (iNumRelevantTesters > 0)
			{
				for(i = 0; i < iNumRelevantTesters; i++)
				{
					if (this->m_pTest->m_paTesterSlots[paiRelevantTesters[i]].fGone)
					{
						DPL(0, "Lost tester %i (array entry %i) during WaitForEventOrCancel in test ID %u!",
							3, paiRelevantTesters[i], i,
							this->m_pTest->m_dwUniqueID);

						// Note the tester who was lost if the user wants it.
						if (piItemNum != NULL)
							(*piItemNum) = paiRelevantTesters[i];

						hr = TNWR_LOSTTESTER;
						goto DONE;
					} // end if (syncing with dead guy)
				} // end for (each relevant tester)
			} // end else if (we care about certain testers)

			// If we got here, it means doesn't care about that guy.
			goto REWAIT;
		  break;

		default:

			i = hr - (WAIT_OBJECT_0 + 1); // get the index of the item that caused this

			// Check to make sure it was a valid event
			if ((i < 0) || (i >= (iNumWaitObjects - 1)))
			{
				DPL(0, "Test ID %u got unexpected return from WaitForMultipleObjects!  %e",
					2, this->m_pTest->m_dwUniqueID, hr);
				hr = E_FAIL;
				goto DONE;
			} // end if (got wacky return)

			if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
			{
				if (i == 0)
				{
					DPL(3, "User cancelled test ID %u during WaitForEventOrCancel.",
						1, this->m_pTest->m_dwUniqueID);

					hr = TNWR_USERCANCEL;
					goto DONE;
				} // end if (it was the user cancel event)
				else
					i--; // adjust index as appropriate
			} // end if (there is a user cancel event)

			// Otherwise, note the object that signalled if the user wants it.
			if (piItemNum != NULL)
				(*piItemNum) = i;

			DPL(6, "Test ID %u's event #%i (%x) got signalled.",
				3, this->m_pTest->m_dwUniqueID, i, pahEvents[i]);

			hr = TNWR_GOTEVENT;
		  break;
	} // end switch (on wait result)



DONE:

	if (pahWaitObjects != NULL)
	{
		LocalFree(pahWaitObjects);
		pahWaitObjects = NULL;
	} // end if (allocated an array)

	if (fHaveTestMasterOpLock)
	{
		this->m_pTest->m_dwRequestID = 0;
		this->m_pTest->m_hrResponseResult = E_FAIL;

		if (this->m_pTest->m_hResponseEvent != NULL)
		{
			CloseHandle(this->m_pTest->m_hResponseEvent);
			this->m_pTest->m_hResponseEvent = NULL;
		} // end if (we have response event)

		fHaveTestMasterOpLock = FALSE;
		LeaveCriticalSection(&(this->m_pTest->m_csMasterOp));
	} // end if (have test master op lock)

	return (hr);
} // CTNExecutor::WaitForEventOrCancel
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ExecSubTestCase()"
//==================================================================================
// CTNExecutor::ExecSubTestCase
//----------------------------------------------------------------------------------
//
// Description: Executes the specifed case as a sub-test of the current test, using
//				the specified tester number & input data, and storing the results
//				on the front of test result chain.
//
// Arguments:
//	char* szCaseModuleID		Module specified test case ID to execute.
//	PVOID pvSubInputData		Pointer to buffer to use as the input data for the
//								sub test.
//	DWORD dwSubInputDataSize	Size of the input data buffer.
//	int iNumMachines			How many testers appear in the following variable
//								parameter list, or 0 to use all the current testers
//								in the same order.
//	...							The testers from the current test who should run the
//								subtest, listed in order of their new positions for
//								execute the subtest.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ExecSubTestCase(char* szCaseModuleID, PVOID pvSubInputData,
									DWORD dwSubInputDataSize, int iNumMachines, ...)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLECASE	pCase = NULL;
	int*				paiTesterArray = NULL;
	va_list				currentparam;
	int					i;



	// Any changes to this function should go in ExecLeechTestCase too.

	DPL(9, "(%x) ==>(\"%s\", %x, %u, %i, ...)", 5, this, szCaseModuleID,
		pvSubInputData, dwSubInputDataSize, iNumMachines);

	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)

	if (iNumMachines < 0)
	{
		DPL(0, "Module called function without even 1 tester (%i)!",
			1, iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)



	if (iNumMachines == 0)
	{
		iNumMachines = this->m_pTest->m_iNumMachines;
		paiTesterArray = (int*) LocalAlloc(LPTR, (iNumMachines * (sizeof (int))));
		if (paiTesterArray == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
		{
			paiTesterArray[i] = i;
		} // end for (each tester in the param list)
	} // end if (should build a list of all testers)
	else
	{
		paiTesterArray = (int*) LocalAlloc(LPTR, (iNumMachines * (sizeof (int))));
		if (paiTesterArray == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)


		va_start(currentparam, iNumMachines);
		for(i = 0; i < iNumMachines; i++)
		{
			paiTesterArray[i] = va_arg(currentparam, int);
		} // end for (each tester in the param list)
		va_end(currentparam);
	} // end else (got passed a specific list of testers)


	hr = this->ExecSubTestCaseArray(szCaseModuleID, pvSubInputData,
									dwSubInputDataSize, iNumMachines,
									paiTesterArray);
	if (hr != S_OK)
	{
		DPL(0, "Running sub test case ID %s failed!", 1, szCaseModuleID);
		goto DONE;
	} // end if (failed to run the test)


DONE:

	if (paiTesterArray != NULL)
	{
		LocalFree(paiTesterArray);
		paiTesterArray = NULL;
	} // end if (allocated a tester array)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::ExecSubTestCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ExecSubTestCaseArray()"
//==================================================================================
// CTNExecutor::ExecSubTestCaseArray
//----------------------------------------------------------------------------------
//
// Description: Executes the specifed case as a sub-test of the current test, using
//				the specified tester number & input data, and storing the results
//				on the front of test result chain.
//
// Arguments:
//	char* szCaseModuleID		Module specified test case ID to execute.
//	PVOID pvSubInputData		Pointer to buffer to use as the input data for the
//								sub test.
//	DWORD dwSubInputDataSize	Size of the input data buffer.
//	int iNumMachines			How many testers appear in the following variable
//								parameter list.
//	int* aiTesterArray			Array of testers from the current test who should
//								run the subtest, listed in order of their new
//								positions for execute the subtest.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ExecSubTestCaseArray(char* szCaseModuleID, PVOID pvSubInputData,
										DWORD dwSubInputDataSize, int iNumMachines,
										int* aiTesterArray)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLECASE	pCase = NULL;
	PTNCTRLMACHINEID	paTesters = NULL;
	int					i;



	// Any changes to this function should go in ExecLeechTestCaseArray too.

	DPL(9, "(%x) ==>(\"%s\", %x, %u, %i, %x)", 6, this, szCaseModuleID,
		pvSubInputData, dwSubInputDataSize, iNumMachines, aiTesterArray);

	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not executing sub test because not in a session!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we're not in a session right now)


	if (iNumMachines < 1)
	{
		DPL(0, "Module called function without even 1 tester (%i)!",
			1, iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if (iNumMachines > this->m_pTest->m_iNumMachines)
	{
		DPL(0, "Module called function with more testers (%i) than exist in the current test (%i)!",
			2, iNumMachines, this->m_pTest->m_iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if (!((this->m_pTest->m_pCase->m_dwOptionFlags & TNTCO_SCENARIO)))
	{
		DPL(0, "Current/parent test we're running did not indicate it would run subtests!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (invalid testing setup

	pCase = this->m_pRealExecutor->m_testtable.GetTest(szCaseModuleID);
	if (pCase == NULL)
	{
		DPL(0, "Couldn't find sub test ID %s in test table!",
			1, szCaseModuleID);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
												"Couldn't find sub test ID %s in test table!",
												1, szCaseModuleID);

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't get test)

	// If the test requires a fixed number of testers and we weren't given that,
	// that's an error.  Otherwise make sure it meets the minimum number required.
	if (pCase->m_iNumMachines > 0)
	{
		if (iNumMachines != pCase->m_iNumMachines)
		{
			DPL(0, "Trying to run sub test ID %s with %i testers (it requires exactly %i)!",
				3, szCaseModuleID, iNumMachines, pCase->m_iNumMachines);

			this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
													"Trying to run sub test ID %s with %i testers (it requires exactly %i)!",
													3, szCaseModuleID, iNumMachines,
													pCase->m_iNumMachines);

			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (incorrect number of machines)
	} // end if (requires exact number of machines)
#pragma BUGBUG(vanceo, "Remove if clause when everyone switches over to ADDDATA")
	else if (pCase->m_iNumMachines < 0)
	{
		if (iNumMachines < (-1 * pCase->m_iNumMachines))
		{
			DPL(0, "Trying to run sub test ID %s with %i testers (it requires at least %i)!",
				3, szCaseModuleID, iNumMachines, (-1 * pCase->m_iNumMachines));

			this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
													"Trying to run sub test ID %s with %i testers (it requires at least %i)!",
													3, szCaseModuleID, iNumMachines,
													(-1 * pCase->m_iNumMachines));

			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (incorrect number of machines)
	} // end else if (requires minimum number of machines)

	if (! (pCase->m_dwOptionFlags & TNTCO_SUBTEST))
	{
		DPL(0, "Sub test ID %s does not indicate it can be run as a subtest!",
			1, szCaseModuleID);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
												"Sub test ID %s does not indicate it can be run as a subtest!",
												1, szCaseModuleID);

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't get test)


	paTesters = (PTNCTRLMACHINEID) LocalAlloc(LPTR, (iNumMachines * sizeof (TNCTRLMACHINEID)));
	if (paTesters == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	for(i = 0; i < iNumMachines; i++)
	{
		if ((aiTesterArray[i] < 0) || (aiTesterArray[i] >= this->m_pTest->m_iNumMachines))
		{
			DPL(0, "Invalid tester number (%i <0 or >=%i) at index %i!",
				3, aiTesterArray[i], this->m_pTest->m_iNumMachines, i);

			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (invalid tester number)

		CopyMemory(&(paTesters[i]),
					&(this->m_pTest->m_paTesterSlots[aiTesterArray[i]].id),
					sizeof (TNCTRLMACHINEID));
	} // end for (each subtester)


	// Update the control layer to let it know we're still alive
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();



	hr = this->m_pRealExecutor->CreateSubTest(this->m_pTest,
											pCase,
											iNumMachines,
											paTesters);
	//BUGBUG what about user cancel, etc
	if (hr != S_OK)
	{
		DPL(0, "Failed to create sub test!", 0);
		goto DONE;
	} // end if (couldn't create subtest)

#ifdef DEBUG
	if (this->m_pTest->m_pSubTest == NULL)
	{
		DPL(0, "Test returned was NULL!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (sub test doesn't exist)

	if (this->m_pTest->m_pSubTest->m_dwUniqueID == 0xFFFFFFFF)
	{
		DPL(0, "Test ID returned was invalid!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (sub test is bogus)

	// Just double check to make sure we got the right tests
	if (this->m_pTest->m_pSubTest->m_pCase != pCase)
	{
		DPL(0, "Got unexpected test (%s) while waiting for %s!",
			2, this->m_pTest->m_pSubTest->m_pCase->m_pszID,
			szCaseModuleID);
		hr = E_FAIL;
		goto DONE;
	} // end if (something went wrong)

	// The user should be specifying input data, not the master
	if (((PTNTESTINSTANCES) (this->m_pTest->m_pSubTest))->m_pvInputData != NULL)
	{
		DPL(0, "Subtest response received from master had input data!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (something went wrong)
#endif // DEBUG


	// We seem to be okay, so run it.

	hr = this->m_pRealExecutor->RunTest((PTNTESTINSTANCES) (this->m_pTest->m_pSubTest),
										pvSubInputData, dwSubInputDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Running sub test case ID %s failed!", 1, szCaseModuleID);
		goto DONE;
	} // end if (failed to run the test)

	// Note this->m_pTest->m_pSubTest may be cleared.


DONE:

	if (paTesters != NULL)
	{
		LocalFree(paTesters);
		paTesters = NULL;
	} // end if (allocated array)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::ExecSubTestCaseArray
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SyncWithTesters()"
//==================================================================================
// CTNExecutor::SyncWithTesters
//----------------------------------------------------------------------------------
//
// Description:    Issues a request to the testmaster to synchronize with specific
//				other machines working on the test (designated in the variable
//				parameters section).  Data can also be sent to these machines at
//				this time.
//				   A slave is determined to be in-sync when the master has also
//				received sync requests from each of the other testers in the array
//				with the slave specified as a tester to sync with.
//				   This has a domino effect, so that each tester in a slave's sync
//				list must also have heard from all the tester's in its own sync list
//				before marking the first slave as ready.  At that point, the master
//				will fill the return data buffer with the data given by all parties
//				and return TNSR_INSYNC.
//				   Only one sync attempt is allowed to be outstanding at a time, per
//				unique test ID.  Syncing with your own tester number has no effect.
//				   If this function doesn't return TNSR_INSYNC, it is expected that
//				the test case will end as soon as possible.  Specifically, if
//				TNSR_LOSTTESTER is returned, the test case must not call any Sync
//				or Exec function that requires multiple machines.
//
// Arguments:
//	char* szSyncName				User defined unique name for this sync
//									operation.  Must match on all machines
//									involved.
//	PVOID pvSendData				Pointer to data to send to people to sync with.
//	DWORD dwSendDataSize			Size of data to send to people to sync with.
//	PTNSYNCDATALIST pReceiveData	Pointer to list that will hold the data received
//									from the other synchronizing testers.
//	int iNumMachines				How many testers we're syncing with (i.e. how
//									many additional integers there are in the
//									variable parameters list), or 0 to sync with all
//									other testers.
//	...								Integers indicating the testers to sync with.
//
// Returns: TNSR_INSYNC if successful, or TNSR_xxx error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SyncWithTesters(char* szSyncName,
									PVOID pvSendData,
									DWORD dwSendDataSize,
									PTNSYNCDATALIST pReceiveData,
									int iNumMachines, ...)
{
	HRESULT		hr;
	va_list		currentparam;
	int			i;
	int			j;
	int*		paiTesters = NULL;



	if (iNumMachines < 0)
	{
		DPL(0, "Passed invalid number of testers (%i)!", 1, iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (invalid parameters)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	// If they want to just sync with everybody else, build the array for them.
	if (iNumMachines == 0)
	{
		iNumMachines = this->m_pTest->m_iNumMachines - 1;
		paiTesters = (int*) LocalAlloc(LPTR, (iNumMachines * sizeof (int)));
		if (paiTesters == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		j = 0;
		for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
		{
			// Skip our tester number.
			if (memcmp(&(this->m_pTest->m_paTesterSlots[i].id), &(this->m_pRealExecutor->m_id), sizeof (TNCTRLMACHINEID)) == 0)
				continue;

			paiTesters[j] = i;
			j++;
		} // end for (each tester)
	} // end if (just syncing with everyone else)
	else
	{
		paiTesters = (int*) LocalAlloc(LPTR, (iNumMachines * sizeof (int)));
		if (paiTesters == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		// Retrieve all the variable parameter arguments
		va_start(currentparam, iNumMachines);
		for(i = 0; i < iNumMachines; i++)
		{
			paiTesters[i] = va_arg(currentparam, int);
		} // end for (each tester)
		va_end(currentparam);
	} // end else (syncing with a specific set of testers)


	hr = this->SyncWithTestersArray(szSyncName,
									pvSendData,
									dwSendDataSize,
									pReceiveData,
									iNumMachines,
									paiTesters);

DONE:

	if (paiTesters != NULL)
	{
		LocalFree(paiTesters);
		paiTesters = NULL;
	} // end if (allocated array)

	return (hr);
} // CTNExecutor::SyncWithTesters
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SyncWithTestersArray()"
//==================================================================================
// CTNExecutor::SyncWithTestersArray
//----------------------------------------------------------------------------------
//
// Description:    Issues a request to the testmaster to synchronize with specific
//				other machines working on the test (designated in the passed in
//				array).  Data can also be sent to these machines at this time.
//				   A slave is determined to be in-sync when the master has also
//				received sync requests from each of the other testers in the array
//				with the slave specified as a tester to sync with.
//				   This has a domino effect, so that each tester in a slave's sync
//				list must also have heard from all the tester's in its own sync
//				list before marking the first slave as ready.  At that point, the
//				master will fill the return data buffer with the data given by all
//				parties and return with TNSR_INSYNC.
//				   Only one sync attempt is allowed to be outstanding at a time, per
//				unique test ID.  Syncing with your own tester number has no effect.
//				   If this function doesn't return TNSR_INSYNC, it is expected that
//				the test case will end as soon as possible.  Specifically, if
//				TNSR_LOSTTESTER is returned, the test case must not call any Sync
//				or Exec function that requires multiple machines.
//
// Arguments:
//	char* szSyncName				User defined unique name for this sync
//									operation.  Must match on all machines
//									involved.
//	PVOID pvSendData				Pointer to data to send to people to sync with.
//	DWORD dwSendDataSize			Size of data to send to people to sync with.
//	PTNSYNCDATALIST pReceiveData	Pointer to list that will hold the data received
//									from the other synchronizing testers.
//	int iNumMachines				How many testers we're syncing with (i.e. how
//									many integers there are in following array).
//	int* aiTesters					Array of integers indicating the testers to sync
//									with.
//
// Returns: TNSR_INSYNC if successful, or TNSR_xxx error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SyncWithTestersArray(char* szSyncName,
										PVOID pvSendData,
										DWORD dwSendDataSize,
										PTNSYNCDATALIST pReceiveData,
										int iNumMachines,
										int* aiTesters)
{
	HRESULT		hr;
	int			iLocalTesterNum;
	int			iNumNonLocalTesters = 0;
	int*		paiNonLocalTesters = NULL;
	int			i;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)

	if ((szSyncName == NULL) || (iNumMachines < 1))
	{
		DPL(0, "Passed invalid test, sync name, or number of testers!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (invalid parameters)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not syncing because not in a session!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we're not in a session right now)



	iLocalTesterNum = this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id));
	if (iLocalTesterNum < 0)
	{
		DPL(0, "Couldn't get this machine's tester number!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get tester number)


	// Allocate and copy only tester numbers other than this machine's current
	// tester number.
	paiNonLocalTesters = (int*) LocalAlloc(LPTR, iNumMachines * sizeof (int));
	if (paiNonLocalTesters == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)


	// Look for references to the local tester number and for invalid testers.
	for(i = 0; i < iNumMachines; i++)
	{
		if ((aiTesters[i] < 0) || (aiTesters[i] >= this->m_pTest->m_iNumMachines))
		{
			DPL(0, "Tester number at index %i is invalid (%i < 0 or >= %i)!",
				3, i, aiTesters[i], this->m_pTest->m_iNumMachines);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (invalid tester number)

		if (aiTesters[i] != iLocalTesterNum)
			paiNonLocalTesters[iNumNonLocalTesters++] = aiTesters[i];
	} // end for (each item in the list)


	if (iNumNonLocalTesters <= 0)
	{
		DPL(0, "No non-local testers left!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no non-local testers people left)


#ifdef DEBUG
	if (iNumNonLocalTesters < iNumMachines)
	{
		DPL(2, "Test %u (case \"%s\") specified self in sync list %i time(s) in sync \"%s\".",
			4, this->m_pTest->m_dwUniqueID,
			this->m_pTest->m_pCase->m_pszID,
			(iNumMachines - iNumNonLocalTesters),
			szSyncName);
	} // end if (removed some items)

	{
		char	szNumber[32];
		char	szTemp[256];



		ZeroMemory(szTemp, 256);
		for(i = 0; i < iNumNonLocalTesters; i++)
		{
			wsprintf(szNumber, "%i", paiNonLocalTesters[i]);
			strcat(szTemp, szNumber);
			if (i < (iNumNonLocalTesters - 1))
				strcat(szTemp, ", ");
		} // end for (each item in the array)

		DPL(9, "Tester %i: Sync \"%s\" with {%s}, send %u bytes at %x, receive to %x.",
			6, iLocalTesterNum, szSyncName, szTemp, dwSendDataSize,
			pvSendData, pReceiveData);
	}
#endif // DEBUG

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->SyncInternal(this->m_pTest,
											szSyncName,
											pvSendData,
											dwSendDataSize,
											pReceiveData,
											iNumNonLocalTesters,
											paiNonLocalTesters);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't synchronize!", 0);
		//goto DONE;
	} // end if (sync failed)



DONE:

	SAFE_LOCALFREE(paiNonLocalTesters);

	return (hr);
} // CTNExecutor::SyncWithTestersArray
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::IsTesterOnSameMachine()"
//==================================================================================
// CTNExecutor::IsTesterOnSameMachine
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the given tester is located on this same machine,
//				FALSE if not.
//
// Arguments:
//	int iTesterNum		Tester number to check.
//
// Returns: TRUE if tester is on same machine, FALSE otherwise.
//==================================================================================
BOOL CTNExecutor::IsTesterOnSameMachine(int iTesterNum)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (FALSE);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (FALSE);
	} // end if (in documentation mode)


	if ((iTesterNum < 0) ||
		(iTesterNum >= this->m_pTest->m_iNumMachines) ||
		(iTesterNum == this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id))))
	{
		DPL(0, "Checking invalid tester number (%i must be >0, <%i and not %i)!",
			3, iTesterNum, this->m_pTest->m_iNumMachines,
			this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id)));
		return (FALSE);
	} // end if (invalid tester number)

#pragma BUGBUG(vanceo, "Only checks top level, fix in other functions too")
	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not checking if on same machine because not in a session!", 0);
		return (FALSE);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	return (this->m_pRealExecutor->IsTesterOnSameMachineInternal(this->m_pTest, iTesterNum));
} // CTNExecutor::IsTesterOnSameMachine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetTestersIPForPort()"
//==================================================================================
// CTNExecutor::GetTestersIPForPort
//----------------------------------------------------------------------------------
//
// Description: Retrieves the IP address of the specified tester and places it in
//				the passed in string buffer.  The buffer must be at least 15
//				characters + NULL termination long.
//				The reachability test must already have been run (and passed) for
//				the specified machine.  See the master object for more details.
//
// Arguments:
//	int iTesterNum		Tester number for which to retrieve IP address.
//	WORD wPort			IP port used to verify reachability.
//	char* szIPString	String to store results in (must be 16 chars).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetTestersIPForPort(int iTesterNum, WORD wPort,
										char* szIPString)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((iTesterNum < 0) ||
		(iTesterNum >= this->m_pTest->m_iNumMachines) ||
		(iTesterNum == this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id))))
	{
		DPL(0, "Requested tester IP for invalid tester number (%i must be >0, <%i and not %i)!",
			3, iTesterNum, this->m_pTest->m_iNumMachines,
			this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id)));
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid tester number)

	if ((wPort == 0) || (szIPString == NULL))
	{
		DPL(0, "Passed invalid port or IP string!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid parameters)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not getting tester's IP because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)


	// If the tester is on the same machine, then we never performed a reach
	// check.  Just return the first IP address we have.
	if (this->m_pRealExecutor->IsTesterOnSameMachineInternal(this->m_pTest, iTesterNum))
	{
		DPL(1, "Tester %i of test %u is on same machine, returning our first IP address.",
			2, iTesterNum, this->m_pTest->m_dwUniqueID);

#pragma BUGBUG(vanceo, "Won't work on leeches until they get info copies")
		hr = this->m_pRealExecutor->m_info.m_ipaddrs.GetIPString(0, szIPString);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't get our string for our first IP address!", 0);
			//goto DONE;
		} // end if (failed getting IP string)

		return (hr);
	} // end if (tester is on same machine)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->GetTesterIPInternal(this->m_pTest, iTesterNum,
													wPort, szIPString);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get tester %i's IP!", 1, iTesterNum);
	} // end if (getting IP failed)

	return (hr);
} // CTNExecutor::GetTestersIPForPort
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetFirstTAPIDeviceNameWithNumber()"
//==================================================================================
// CTNExecutor::GetFirstTAPIDeviceNameWithNumber
//----------------------------------------------------------------------------------
//
// Description: Sets the passed in pointer to the name of the first TAPI device
//				which has a phone number associated with it.
//				Note: this is not a copy of the string.
//
// Arguments:
//	char** lppszDeviceName		Pointer to set to point to device name string.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetFirstTAPIDeviceNameWithNumber(char** lppszDeviceName)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (lppszDeviceName == NULL)
	{
		DPL(0, "Passed invalid device name pointer!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid parameters)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not getting TAPI device name because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->m_info.m_TAPIdevices.GetFirstTAPIDeviceNameWithNumber(lppszDeviceName);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get first TAPI device name with number!", 0);
	} // end if (getting device failed)

	return (hr);
} // CTNExecutor::GetFirstTAPIDeviceNameWithNumber
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetPhoneNumberForTester()"
//==================================================================================
// CTNExecutor::GetPhoneNumberForTester
//----------------------------------------------------------------------------------
//
// Description: Returns the phone number of the specified tester number in the given
//				string buffer.  It is assumed to be large enough.
//				The reachability test must already have been run (and passed) for
//				the specified machine.  See the master object for more details.
//
// Arguments:
//	int iTesterNum			Tester number for which to retrieve phone number.
//	char* szPhoneNumber		String to store result in (must be large enough to hold
//							the entire phone number).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetPhoneNumberForTester(int iTesterNum, char* szPhoneNumber)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((iTesterNum < 0) ||
		(iTesterNum >= this->m_pTest->m_iNumMachines) ||
		(iTesterNum == this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id))))
	{
		DPL(0, "Requested tester phone number for invalid tester number (%i must be >0, <%i and not %i)!",
			3, iTesterNum, this->m_pTest->m_iNumMachines,
			this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id)));
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid tester number)

	if (szPhoneNumber == NULL)
	{
		DPL(0, "Passed invalid phone number string!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid parameters)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not getting phone number because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)


	// If the tester is on the same machine, then we never performed a reach check.
	// Even if there are multiple modems on this machine, we don't have enough
	// information in this function to determine which phone number we should
	// return, so if a user wants to do something with this case, they'll have to
	// do it manually.  It actually would make more sense to have it be a 1 machine
	// test, though.
	if (this->m_pRealExecutor->IsTesterOnSameMachineInternal(this->m_pTest, iTesterNum))
	{
		DPL(0, "Tester %i of test %u is on same machine!",
			2, iTesterNum, this->m_pTest->m_dwUniqueID);
		return (E_FAIL);
	} // end if (tester is on same machine)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->GetTesterPhoneNumInternal(this->m_pTest, iTesterNum,
															szPhoneNumber);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get tester %i's phone number!", 1, iTesterNum);
	} // end if (getting phone number failed)

	return (hr);
} // CTNExecutor::GetPhoneNumberForTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetCOMPortConnectedToTester()"
//==================================================================================
// CTNExecutor::GetCOMPortConnectedToTester
//----------------------------------------------------------------------------------
//
// Description: Returns the COM port connected to the specified tester in the DWORD
//				pointer given.
//				The reachability test must already have been run (and passed) for
//				the specified machine.  See the master object for more details.
//
// Arguments:
//	int iTesterNum		Tester number for which to retrieve COM port with
//						connection.
//	DWORD* lpdwCOMPort	Place to store COM port connected.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetCOMPortConnectedToTester(int iTesterNum, DWORD* lpdwCOMPort)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((iTesterNum < 0) ||
		(iTesterNum >= this->m_pTest->m_iNumMachines) ||
		(iTesterNum == this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id))))
	{
		DPL(0, "Requested tester IP for invalid tester number (%i must be >0, <%i and not %i)!",
			3, iTesterNum, this->m_pTest->m_iNumMachines,
			this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id)));
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid tester number)

	if (lpdwCOMPort == NULL)
	{
		DPL(0, "Passed invalid COM port destination pointer!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (invalid parameters)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not getting COM port because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)


	// If the tester is on the same machine, then we never performed a reach check.
	// Even if there are loopback serial connections on this machine, we don't have
	// enough information in this function to determine which one we should return,
	// so if a user wants to do something with this case, they'll have to do it
	// manually.  It actually would make more sense to have it be a 1 machine test,
	// though.
	if (this->m_pRealExecutor->IsTesterOnSameMachineInternal(this->m_pTest, iTesterNum))
	{
		DPL(0, "Tester %i of test %u is on same machine!",
			2, iTesterNum, this->m_pTest->m_dwUniqueID);
		return (E_FAIL);
	} // end if (tester is on same machine)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->GetTesterCOMPortInternal(this->m_pTest, iTesterNum,
														lpdwCOMPort);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get COM port connected to tester %i!", 1, iTesterNum);
	} // end if (getting COM port failed)

	return (hr);
} // CTNExecutor::GetCOMPortConnectedToTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::CreateNewLeechAttachment()"
//==================================================================================
// CTNExecutor::CreateNewLeechAttachment
//----------------------------------------------------------------------------------
//
// Description: Creates a new object and prepares it for attachment by a leeching
//				process.
//
// Arguments:
//	PTNLEECH* ppLeech				Pointer to place to store new leech.
//	PVOID pvSendConnectData			Optional pointer to data to send to other side.
//	DWORD dwSendConnectDataSize		Size of data to send to other side.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::CreateNewLeechAttachment(PTNLEECH* ppLeech,
											PVOID pvSendConnectData,
											DWORD dwSendConnectDataSize)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (ppLeech == NULL)
	{
		DPL(0, "Place to store leech pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not creating new leech because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

#ifdef DEBUG
	if (this->m_pRealExecutor == NULL)
	{
		DPL(0, "Real executor pointer is NULL!?", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (something's screwed)
#endif //DEBUG

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	(*ppLeech) = this->m_pRealExecutor->m_leeches.NewLeech(&(this->m_pRealExecutor->m_moduleID),
															this->m_pRealExecutor,
															pvSendConnectData,
															dwSendConnectDataSize);
	if ((*ppLeech) == NULL)
	{
		DPL(0, "Couldn't create new leech!", 0);
		return (E_FAIL);
	} // end if (couldn't allocate object)

	return (S_OK);
} // CTNExecutor::CreateNewLeechAttachment
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::DetachAndReleaseLeech()"
//==================================================================================
// CTNExecutor::DetachAndReleaseLeech
//----------------------------------------------------------------------------------
//
// Description: Disconnects the leech specified by the pointer, and NULLs it.
//
// Arguments:
//	PTNLEECH* ppLeech	Pointer to leech pointer.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::DetachAndReleaseLeech(PTNLEECH* ppLeech)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (ppLeech == NULL)
	{
		DPL(0, "Place holding leech pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not detaching leech because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->m_leeches.RemoveLeech(*ppLeech);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove leech %x from list!", 1, (*ppLeech));
		return (hr);
	} // end if (couldn't remove leech)

	(*ppLeech) = NULL;

	return (S_OK);
} // CTNExecutor::DetachAndReleaseLeech
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::AllowStaticIDAttachment()"
//==================================================================================
// CTNExecutor::AllowStaticIDAttachment
//----------------------------------------------------------------------------------
//
// Description: Lets the leech process attach using the given preplanned ID.
//				Should only be used if passing the dynamically created ID is not an
//				option.
//
// Arguments:
//	PTNLEECH pLeech			Leech object to allow.
//	char* szPreplannedID	Mutually decided upon ID that leech will connect using.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::AllowStaticIDAttachment(PTNLEECH pLeech, char* szPreplannedID)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if ((pLeech == NULL) || (szPreplannedID == NULL))
	{
		DPL(0, "Leech object or ID is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not allowing static ID attachment because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = pLeech->AllowStaticIDConnection(&(this->m_pRealExecutor->m_moduleID),
										szPreplannedID);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allow static ID (\"%s\") connection for leech %x!",
			2, szPreplannedID, pLeech);
	} // end if (couldn't allow static ID connection)

	return (hr);
} // CTNExecutor::AllowStaticIDAttachment
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::WaitForLeechConnection()"
//==================================================================================
// CTNExecutor::WaitForLeechConnection
//----------------------------------------------------------------------------------
//
// Description: Blocks execution until the specified leech gets connected (or some
//				other event causes testing to be halted).
//
// Arguments:
//	PTNLEECH pTNLeech		The leech to wait for.
//	DWORD dwTimeout			How long to wait for the connection until failing, in
//							milliseconds.
//
// Returns: TNLWR_WAITOK if successful, or TNLWR_xxx error code otherwise.
//==================================================================================
HRESULT CTNExecutor::WaitForLeechConnection(PTNLEECH pTNLeech, DWORD dwTimeout)
{
	HRESULT		hr;
	HANDLE*		pahWaitObjects = NULL;
	DWORD		dwNumWaitObjects;


	if (this == NULL)
	{
		DPL(0, "Slave object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (slave object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not waiting for leech connection because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	if (pTNLeech == NULL)
	{
		DPL(0, "Leech object isn't valid!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (leech isn't valid)

#ifdef DEBUG
	if (pTNLeech->m_hConnectEvent != NULL)
	{
		DPL(0, "Leech already has a connect event (%x)!?",
			1, pTNLeech->m_hConnectEvent);
		hr = E_FAIL;
		goto DONE;
	} // end if (the leech already has a connect event)
#endif // DEBUG

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();



	pTNLeech->m_hConnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (pTNLeech->m_hConnectEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create leech connection event!", 0);
		goto DONE;
	} // end if (the leech already has a connect event)


	dwNumWaitObjects = 1;
	if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
		dwNumWaitObjects++;

	pahWaitObjects = (HANDLE*) LocalAlloc(LPTR, ((dwNumWaitObjects + 1) * (sizeof (HANDLE))));
	if (pahWaitObjects == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't set it)

	if (! DuplicateHandle(GetCurrentProcess(), pTNLeech->m_hConnectEvent,
						GetCurrentProcess(), &(pahWaitObjects[0]),
						0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		hr = GetLastError();
		DPL(0, "Couldn't duplicate connect event!", 0);
		goto DONE;
	} // end if (couldn't duplicate event)

	if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
		pahWaitObjects[1] = this->m_pRealExecutor->m_hUserCancelEvent;

	pahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this causes kernel to rewalk array

	//BUGBUG this is not particularly thread safe
	// Just double check to see if it's already connected.  If it is, don't bother
	// waiting.
	if (pTNLeech->m_fConnected)
	{
		DPL(1, "WARNING: Leech \"%s\" already connected, not waiting.",
			1, pTNLeech->GetAttachPointID());
		hr = TNCWR_CONNECTED;
		goto DONE;
	} // end if (already connected)

	DPL(7, "Test ID %u waiting for %u objects (leech \"%s\") or user cancel.",
		3, this->m_pTest->m_dwUniqueID, dwNumWaitObjects,
		pTNLeech->GetAttachPointID());

REWAIT:
	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, pahWaitObjects, FALSE,
								dwTimeout, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the completion signal.  Make sure we're cool.

			if (pTNLeech->m_fConnected)
			{
				DPL(1, "Leech \"%s\" connected.", 1, pTNLeech->GetAttachPointID());
				hr = TNCWR_CONNECTED;
			} // end if (connected)
			else
			{
				DPL(0, "Completion indicated leech \"%s\" failed to connect!",
					1, pTNLeech->GetAttachPointID());
				hr = TNCWR_TIMEOUT;
			} // end else (not connected)
		  break;

		case WAIT_OBJECT_0 + 1:
			// User cancelled
			hr = TNCWR_USERCANCEL;
		  break;

		case WAIT_TIMEOUT:
			DPL(5, "Test ID %u's wait for leech \"%s\" connection timed out.",
				2, this->m_pTest->m_dwUniqueID, pTNLeech->GetAttachPointID());

			hr = TNCWR_TIMEOUT;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_FAILED:
			hr = GetLastError();

			DPL(0, "Wait failed!  %e", 1, hr);

			if (hr == S_OK)
				hr = E_FAIL;
		  break;

		default:
			DPL(0, "Test ID %u got unexpected return from WaitForMultipleObjects!",
				1, this->m_pTest->m_dwUniqueID);
		  break;
	} // end switch (on wait result)


DONE:

	if (pahWaitObjects != NULL)
	{
		CloseHandle(pahWaitObjects[0]);
		pahWaitObjects[0] = NULL;

		LocalFree(pahWaitObjects);
		pahWaitObjects = NULL;
	} // end if (allocated an array)

	// Give up our time slice to hopefully make sure the receive thread had time
	// to close this handle in the success case.  It's not a huge deal, because
	// we ignore errors from CloseHandle in both places anyway.
	Sleep(0); 

	if (pTNLeech->m_hConnectEvent != NULL)
	{
		CloseHandle(pTNLeech->m_hConnectEvent);
		pTNLeech->m_hConnectEvent = NULL;
	} // end if (have event)

	return (hr);

} // CTNExecutor::WaitForLeechConnection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetLeechConnectData()"
//==================================================================================
// CTNExecutor::GetLeechConnectData
//----------------------------------------------------------------------------------
//
// Description: Sets the pointers passed in to the data sent by the other side when
//				it connected to the given leech.
//				Note: this is not a copy of the data.
//
// Arguments:
//	PTNLEECH pTNLeech	Pointer to leech to retrieve data for.
//	PVOID* ppvData		Pointer to have set to point to data sent by other side
//						when it connected.
//	DWORD* pdwDataSize	Place to store size of data sent by other side.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetLeechConnectData(PTNLEECH pTNLeech, PVOID* ppvData,
										DWORD* pdwDataSize)
{
	if (this == NULL)
	{
		DPL(0, "This object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	return (pTNLeech->GetConnectData(ppvData, pdwDataSize));
} // CTNExecutor::GetLeechConnectData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ExecLeechTestCase()"
//==================================================================================
// CTNExecutor::ExecLeechTestCase
//----------------------------------------------------------------------------------
//
// Description: Instructs the given leech to execute the specifed case, using the
//				the specified tester number & input data, and storing the results
//				on the front of test result chain.
//				This behaves similar to ExecSubTestCase, except the subtest is run
//				in the other process.
//
// Arguments:
//	PTNLEECH pLeech				Leech which should run the test.
//	char* szCaseModuleID		Module specified test case ID to execute.
//	PVOID pvSubInputData		Pointer to buffer to use as the input data for the
//								leech test.
//	DWORD dwSubInputDataSize	Size of the input data buffer.
//	int iNumMachines			How many testers appear in the following variable
//								parameter list, or 0 to use all the current
//								testers in the same order.
//	...							The testers from the current test who should run the
//								subtest, listed in order of their new positions for
//								execute the subtest.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ExecLeechTestCase(PTNLEECH pLeech, char* szCaseModuleID,
									PVOID pvSubInputData, DWORD dwSubInputDataSize,
									int iNumMachines, ...)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLECASE	pCase = NULL;
	int*				paiTesterArray = NULL;
	va_list				currentparam;
	int					i;


	// Any changes to this function should go in ExecSubTestCase too.


	DPL(9, "(%x) ==>(%x, \"%s\", %x, %u, %i, ...)", 6, this, pLeech, szCaseModuleID,
		pvSubInputData, dwSubInputDataSize, iNumMachines);

	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)

	if (iNumMachines < 0)
	{
		DPL(0, "Module called function without even 1 tester (%i)!",
			1, iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if (iNumMachines == 0)
	{
		iNumMachines = this->m_pTest->m_iNumMachines;
		paiTesterArray = (int*) LocalAlloc(LPTR, (iNumMachines * (sizeof (int))));
		if (paiTesterArray == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
		{
			paiTesterArray[i] = i;
		} // end for (each tester in the param list)
	} // end if (should build a list of all testers)
	else
	{
		paiTesterArray = (int*) LocalAlloc(LPTR, (iNumMachines * (sizeof (int))));
		if (paiTesterArray == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)


		va_start(currentparam, iNumMachines);
		for(i = 0; i < iNumMachines; i++)
		{
			paiTesterArray[i] = va_arg(currentparam, int);
		} // end for (each tester in the param list)
		va_end(currentparam);
	} // end else (got passed a specific list of testers)


	hr = this->ExecLeechTestCaseArray(pLeech, szCaseModuleID,
									pvSubInputData, dwSubInputDataSize,
									iNumMachines, paiTesterArray);
	if (hr != S_OK)
	{
		DPL(0, "Running leech test case ID %s failed!", 1, szCaseModuleID);
		goto DONE;
	} // end if (failed to run the test)


DONE:

	if (paiTesterArray != NULL)
	{
		LocalFree(paiTesterArray);
		paiTesterArray = NULL;
	} // end if (allocated a tester array)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::ExecLeechTestCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX





#ifndef _XBOX // no IPC supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ExecLeechTestCaseArray()"
//==================================================================================
// CTNExecutor::ExecLeechTestCaseArray
//----------------------------------------------------------------------------------
//
// Description: Instructs the given leech to execute the specifed case, using the
//				the specified tester number & input data, and storing the results
//				on the front of test result chain.
//				This behaves similar to ExecSubTestCase, except the subtest is run
//				in the other process.
//
// Arguments:
//	PTNLEECH pLeech				Leech which should run the test.
//	char* szCaseModuleID		Module specified test case ID to execute.
//	PVOID pvSubInputData		Pointer to buffer to use as the input data for the
//								leech test.
//	DWORD dwSubInputDataSize	Size of the input data buffer.
//	int iNumMachines			How many testers appear in the following variable
//								parameter list.
//	int* aiTesterArray			The testers from the current test who should run the
//								subtest, listed in order of their new positions for
//								execute the subtest.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ExecLeechTestCaseArray(PTNLEECH pLeech, char* szCaseModuleID,
									LPVOID pvSubInputData, DWORD dwSubInputDataSize,
									int iNumMachines, int* aiTesterArray)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLECASE	pCase = NULL;
	PTNCTRLMACHINEID	paTesters = NULL;
	int					i;


	// Any changes to this function should go in ExecSubTestCase too.


	DPL(9, "(%x) ==>(%x, \"%s\", %x, %u, %i, %x)", 7, this, pLeech, szCaseModuleID,
		pvSubInputData, dwSubInputDataSize, iNumMachines, aiTesterArray);

	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not executing leech test because not in a session!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we're not in a session right now)

	if (iNumMachines < 1)
	{
		DPL(0, "Module called function without even 1 tester (%i)!",
			1, iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if (iNumMachines > this->m_pTest->m_iNumMachines)
	{
		DPL(0, "Module called function with more testers (%i) than exist in the current test (%i)!",
			2, iNumMachines, this->m_pTest->m_iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if (!((this->m_pTest->m_pCase->m_dwOptionFlags & TNTCO_SCENARIO)))
	{
		DPL(0, "Current/parent test we're running did not indicate it would run subtests!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (invalid testing setup

	pCase = this->m_pRealExecutor->m_testtable.GetTest(szCaseModuleID);
	if (pCase == NULL)
	{
		DPL(0, "Couldn't find sub test ID %s in test table!",
			1, szCaseModuleID);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
												"Couldn't find leech test ID %s in test table!",
												1, szCaseModuleID);

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't get test)


	// If the test requires a fixed number of testers and we weren't given that,
	// that's an error.  Otherwise make sure it meets the minimum number required.
	if (pCase->m_iNumMachines > 0)
	{
		if (iNumMachines != pCase->m_iNumMachines)
		{
			DPL(0, "Trying to run sub test ID %s with %i testers (it requires exactly %i)!",
				3, szCaseModuleID, iNumMachines, pCase->m_iNumMachines);

			this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
													"Trying to run sub test ID %s with %i testers (it requires exactly %i)!",
													3, szCaseModuleID, iNumMachines,
													pCase->m_iNumMachines);

			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (incorrect number of machines)
	} // end if (requires exact number of machines)
#pragma BUGBUG(vanceo, "Remove if clause when everyone switches over to ADDDATA")
	else if (pCase->m_iNumMachines < 0)
	{
		if (iNumMachines < (-1 * pCase->m_iNumMachines))
		{
			DPL(0, "Trying to run sub test ID %s with %i testers (it requires at least %i)!",
				3, szCaseModuleID, iNumMachines, (-1 * pCase->m_iNumMachines));

			this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
													"Trying to run sub test ID %s with %i testers (it requires at least %i)!",
													3, szCaseModuleID, iNumMachines,
													(-1 * pCase->m_iNumMachines));

			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (incorrect number of machines)
	} // end else if (requires minimum number of machines)

	if (! (pCase->m_dwOptionFlags & TNTCO_SUBTEST))
	{
		DPL(0, "Leech test ID %s does not indicate it can be run as a subtest!",
			1, szCaseModuleID);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CRITICAL,
												"Sub test ID %s does not indicate it can be run as a subtest!",
												1, szCaseModuleID);

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't get test)

	paTesters = (PTNCTRLMACHINEID) LocalAlloc(LPTR, (iNumMachines * sizeof (TNCTRLMACHINEID)));
	if (paTesters == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	for(i = 0; i < iNumMachines; i++)
	{
		CopyMemory(&(paTesters[i]),
					&(this->m_pTest->m_paTesterSlots[aiTesterArray[i]].id),
					sizeof (TNCTRLMACHINEID));
	} // end for (each subtester)


	// Update the control layer to let it know we're still alive
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();



	hr = this->m_pRealExecutor->CreateSubTest(this->m_pTest,
											pCase,
											iNumMachines,
											paTesters);
	//BUGBUG what about user cancel, etc
	if (hr != S_OK)
	{
		DPL(0, "Failed to create sub test!", 0);
		goto DONE;
	} // end if (couldn't create subtest)

#ifdef DEBUG
	if (this->m_pTest->m_pSubTest == NULL)
	{
		DPL(0, "Test returned was NULL!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (sub test doesn't exist)

	if ((this->m_pTest->m_pSubTest->m_dwUniqueID == 0) ||
		(this->m_pTest->m_pSubTest->m_dwUniqueID == 0xFFFFFFFF))
	{
		DPL(0, "Test ID returned was invalid (%u)!",
			1, this->m_pTest->m_pSubTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (sub test is bogus)

	// Just double check to make sure we got the right tests
	if (this->m_pTest->m_pSubTest->m_pCase != pCase)
	{
		DPL(0, "Got unexpected test (%s) while waiting for %s!",
			2, this->m_pTest->m_pSubTest->m_pCase->m_pszID,
			szCaseModuleID);
		hr = E_FAIL;
		goto DONE;
	} // end if (something went wrong)

	// The user should be specifying input data, not the master
	if (((PTNTESTINSTANCES) (this->m_pTest->m_pSubTest))->m_pvInputData != NULL)
	{
		DPL(0, "Subtest response received from master had input data!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (something went wrong)
#endif // DEBUG


	// We seem to be okay, so have the leech run it.  First make sure deadlock
	// checking is off.
#pragma BUGBUG(vanceo, "This is a hack, do this right")
	BOOL		fOldCheckStatus;

	fOldCheckStatus = this->m_pRealExecutor->m_fDeadlockCheck; 
	this->m_pRealExecutor->m_fDeadlockCheck = FALSE;

	hr = pLeech->HaveLeechRunTest((PTNTESTINSTANCES) (this->m_pTest->m_pSubTest),
									pvSubInputData, dwSubInputDataSize);

	this->m_pRealExecutor->m_fDeadlockCheck = fOldCheckStatus;

	if (hr != S_OK)
	{
		DPL(0, "Running leech test case ID %s failed!", 1, szCaseModuleID);
		goto DONE;
	} // end if (failed to run the test)


DONE:

	if (paTesters != NULL)
	{
		LocalFree(paTesters);
		paTesters = NULL;
	} // end if (allocated array)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::ExecLeechTestCaseArray
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::CreateNewFaultSim()"
//==================================================================================
// CTNExecutor::CreateNewFaultSim
//----------------------------------------------------------------------------------
//
// Description: Creates a new fault simulator of the specified type and initializes
//				it with the given data.
//
// Arguments:
//	PTNFAULTSIM* ppFaultSim		Pointer to place to store new fault sim.
//	DWORD dwFaultSimID			Type of fault simulator to create.
//	PVOID pvInitData			Pointer to data to give to fault simulator when
//								initializing.  FaultSim dependant.
//	DWORD dwInitDataSize		Size of data to use when initializing.  FaultSim
//								dependant.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::CreateNewFaultSim(PTNFAULTSIM* ppFaultSim, DWORD dwFaultSimID,
										PVOID pvInitData, DWORD dwInitDataSize)
{
	HRESULT		hr;
	BOOL		fInitted = FALSE;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (object is invalid)

	if (ppFaultSim == NULL)
	{
		DPL(0, "Place to store fault sim pointer is NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not creating fault sim because not in a session!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we're not in a session right now)

#ifdef DEBUG
	if (this->m_pRealExecutor == NULL)
	{
		DPL(0, "Real executor pointer is NULL!?", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (something's screwed)
#endif //DEBUG

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	switch (dwFaultSimID)
	{
		case TN_FAULTSIM_IMTEST:
			(*ppFaultSim) = new (CTNFaultSimIMTest);
		  break;

		default:
			DPL(0, "Unknown fault simulator ID %u!", 1, dwFaultSimID);
			hr = ERROR_INVALID_PARAMETER;
			goto ERROR_EXIT;
		  break;
	} // end switch (on the type of fault simulator)

	if ((*ppFaultSim) == NULL)
	{
		DPL(0, "Couldn't create new fault simulator of type %u!",
			1, dwFaultSimID);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (couldn't allocate object)

	hr = (*ppFaultSim)->Initialize(pvInitData, dwInitDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't initialize fault simulator type %u!", 1, dwFaultSimID);
		goto ERROR_EXIT;
	} // end if (couldn't initialize fault sim)

	fInitted = TRUE;


	hr = this->m_pRealExecutor->m_faultsims.Add(*ppFaultSim);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add fault simulator object %x to list!", 1, (*ppFaultSim));
		goto ERROR_EXIT;
	} // end if (couldn't add item)


	return (S_OK);


ERROR_EXIT:

	if (fInitted)
	{
		// Ignore error.
		(*ppFaultSim)->Release();
		fInitted = FALSE;
	} // end if (initialized fault sim)

	if ((*ppFaultSim) != NULL)
	{
		delete (*ppFaultSim);
		(*ppFaultSim) = NULL;
	} // end if (have object)

	return (hr);
} // CTNExecutor::CreateNewFaultSim
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ReleaseFaultSim()"
//==================================================================================
// CTNExecutor::ReleaseFaultSim
//----------------------------------------------------------------------------------
//
// Description: Releases the fault simulator specified by the pointer, and NULLs it.
//
// Arguments:
//	PTNFAULTSIM* ppFaultSim		Pointer to fault sim pointer.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ReleaseFaultSim(PTNFAULTSIM* ppFaultSim)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (ppFaultSim == NULL)
	{
		DPL(0, "Place holding fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not releasing fault sim because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	hr = this->m_pRealExecutor->m_faultsims.RemoveFirstReference(*ppFaultSim);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove fault sim %x from list!", 1, (*ppFaultSim));
		return (hr);
	} // end if (couldn't remove fault sim)

	hr = (*ppFaultSim)->Release();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't release fault sim %x!", 1, (*ppFaultSim));
		return (hr);
	} // end if (couldn't release fault sim)

	if ((*ppFaultSim)->m_dwRefCount == 0)
	{
		delete (*ppFaultSim);
	} // end if (can delete object)
	else
	{
		DPL(0, "WARNING: Can't delete fault simulator %x, its refcount is %u!",
			2, (*ppFaultSim), (*ppFaultSim)->m_dwRefCount);
	} // end else (can't delete object)

	(*ppFaultSim) = NULL;

	return (S_OK);
} // CTNExecutor::ReleaseFaultSim
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SetFaultSimBandwidth()"
//==================================================================================
// CTNExecutor::SetFaultSimBandwidth
//----------------------------------------------------------------------------------
//
// Description: Sets the artificial send or receive bandwidth limit for the given
//				fault simulator.
//				Pass in zero to turn bandwidth limiting off.
//
// Arguments:
//	PTNFAULTSIM pFaultSim		Pointer to fault simulator to use.
//	BOOL fSend					TRUE to limit the send (outgoing) bandwidth, FALSE
//								to limit the receive (incoming) bandwidth.
//	DWORD dwHundredBytesPerSec	Rate to constrict the bandwidth to, or 0 for none.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SetFaultSimBandwidth(PTNFAULTSIM pFaultSim,
										BOOL fSend, DWORD dwHundredBytesPerSec)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (pFaultSim == NULL)
	{
		DPL(0, "Fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not setting bandwidth because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	if (dwHundredBytesPerSec > 0)
	{
		DPL(1, "Setting fault simulator %s bandwidth to %u00 bytes per sec.",
			2, ((fSend) ? "send" : "receive"), dwHundredBytesPerSec);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
				"Setting fault simulator %s bandwidth to %u00 bytes per sec.",
				2, ((fSend) ? "send" : "receive"), dwHundredBytesPerSec);
	} // end if (turning on bandwidth limitation)
	else
	{
		DPL(1, "Turning off fault simulator %s bandwidth limitation.",
			1, ((fSend) ? "send" : "receive"));

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Turning off fault simulator %s bandwidth limitation.",
			1, ((fSend) ? "send" : "receive"));
	} // end else (turning off bandwidth limitation)

	
	hr = pFaultSim->SetBandwidth(fSend, dwHundredBytesPerSec);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't set fault sim %x's %s bandwidth to %u00 bytes per sec!",
			3, pFaultSim, ((fSend) ? "send" : "receive"), dwHundredBytesPerSec);
	} // end if (couldn't set bandwidth)

	return (hr);
} // CTNExecutor::SetFaultSimBandwidth
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SetFaultSimLatency()"
//==================================================================================
// CTNExecutor::SetFaultSimLatency
//----------------------------------------------------------------------------------
//
// Description: Sets the artificial send or receive latency for the given fault
//				simulator.
//				Pass in zero to turn artificial latency off.
//
// Arguments:
//	PTNFAULTSIM pFaultSim	Pointer to fault simulator to use.
//	BOOL fSend				TRUE to incur the latency on sends (outgoing data),
//							FALSE to incur the latency on receives (incoming data).
//	DWORD dwMSDelay			Number of milliseconds to increase the latency by, or 0
//							for none.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SetFaultSimLatency(PTNFAULTSIM pFaultSim,
										BOOL fSend, DWORD dwMSDelay)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (pFaultSim == NULL)
	{
		DPL(0, "Fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not setting latency because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	if (dwMSDelay > 0)
	{
		DPL(1, "Setting fault simulator %s artificial latency to %u ms.",
			2, ((fSend) ? "send" : "receive"), dwMSDelay);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
				"Setting fault simulator %s artificial latency to %u ms.",
				2, ((fSend) ? "send" : "receive"), dwMSDelay);
	} // end if (turning on latency)
	else
	{
		DPL(1, "Turning off fault simulator %s artificial latency.",
			1, ((fSend) ? "send" : "receive"));

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Turning off fault simulator %s artificial latency.",
			1, ((fSend) ? "send" : "receive"));
	} // end else (turning off latency)

	
	hr = pFaultSim->SetLatency(fSend, dwMSDelay);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't set fault sim %x's %s artificial latency to %u ms!",
			3, pFaultSim, ((fSend) ? "send" : "receive"), dwMSDelay);
	} // end if (couldn't set latency)

	return (hr);
} // CTNExecutor::SetFaultSimLatency
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SetFaultSimDropPacketsPercent()"
//==================================================================================
// CTNExecutor::SetFaultSimDropPacketsPercent
//----------------------------------------------------------------------------------
//
// Description: Sets the percentage of sent or received packets which should be
//				randomly dropped.
//				Pass 0 to turn function off.
//
// Arguments:
//	PTNFAULTSIM pFaultSim	Pointer to fault simulator to use.
//	BOOL fSend				TRUE to set the drop percentage for sends (outgoing
//							data), FALSE to set the drop percentage for receives
//							(incoming data).
//	DWORD dwPercent			Percentage of packets to randomly drop, or 0 for none.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SetFaultSimDropPacketsPercent(PTNFAULTSIM pFaultSim,
													BOOL fSend, DWORD dwPercent)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (pFaultSim == NULL)
	{
		DPL(0, "Fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not setting drop packets percent because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	if (dwPercent > 0)
	{
		DPL(1, "Setting fault simulator %s packet loss to %u%.",
			2, ((fSend) ? "send" : "receive"), dwPercent);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
				"Setting fault simulator %s packet loss to %u%.",
				2, ((fSend) ? "send" : "receive"), dwPercent);
	} // end if (turning on packet loss)
	else
	{
		DPL(1, "Turning off fault simulator %s packet loss.",
			1, ((fSend) ? "send" : "receive"));

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Turning off fault simulator %s packet loss.",
			1, ((fSend) ? "send" : "receive"));
	} // end else (turning off packet loss)

	
	hr = pFaultSim->SetDropPacketsPercent(fSend, dwPercent);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't set fault sim %x's %s packet loss to %u%!",
			3, pFaultSim, ((fSend) ? "send" : "receive"), dwPercent);
	} // end if (couldn't set drop percent)

	return (hr);
} // CTNExecutor::SetFaultSimDropPacketsPercent
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FaultSimReconnect()"
//==================================================================================
// CTNExecutor::FaultSimReconnect
//----------------------------------------------------------------------------------
//
// Description: Reconnects either the send/receive link
//
// Arguments:
//	PTNFAULTSIM pFaultSim	Pointer to fault simulator to use.
//	BOOL fSend				TRUE to connect send link
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::FaultSimReconnect(PTNFAULTSIM pFaultSim,
										BOOL fSend)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (pFaultSim == NULL)
	{
		DPL(0, "Fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not setting drop packets percent because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	if(fSend)
	{
		DPL(1, "Setting fault simulator reconnect for send side", 0);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Setting fault simulator reconnect for send side", 0);
	} // end if (reconnecting send side)
	else
	{
		DPL(1, "Setting fault simulator reconnect for receive side", 0);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Setting fault simulator reconnect for receive side", 0);
	} // end if (reconnecting receive side)
	
	
	hr = pFaultSim->Reconnect(fSend);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't reconnect!", 0);
	} // end if (couldn't reconnect)

	return (hr);
} // CTNExecutor::FaultSimReconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FaultSimDisconnect()"
//==================================================================================
// CTNExecutor::FaultSimDisconnect
//----------------------------------------------------------------------------------
//
// Description: Disconnects either the send/receive link
//
// Arguments:
//	PTNFAULTSIM pFaultSim	Pointer to fault simulator to use.
//	BOOL fSend				TRUE to connect send link
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::FaultSimDisconnect(PTNFAULTSIM pFaultSim,
										BOOL fSend)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if (pFaultSim == NULL)
	{
		DPL(0, "Fault sim pointer is NULL!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (param is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	if ((this->m_pRealExecutor->m_pOwningSlave != NULL) &&
		(! this->m_pRealExecutor->m_pOwningSlave->m_fInSession))
	{
		DPL(0, "Not setting drop packets percent because not in a session!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we're not in a session right now)

	
	// The user is making a function call, so the test thread is still alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	if(fSend)
	{
		DPL(1, "Setting fault simulator disconnect for send side", 0);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Setting fault simulator disconnect for send side", 0);
	} // end if (disconnecting send side)
	else
	{
		DPL(1, "Setting fault simulator disconnect for receive side", 0);

		this->m_pRealExecutor->SprintfLogInternal(TNLST_CONTROLLAYER_INFO,
			"Setting fault simulator disconnect for receive side", 0);
	} // end if (disconnecting receive side)
	
	
	hr = pFaultSim->Disconnect(fSend);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't disconnect!", 0);
	} // end if (couldn't disconnect)

	return (hr);
} // CTNExecutor::FaultSimDisconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::NoteTestIsStillRunning()"
//==================================================================================
// CTNExecutor::NoteTestIsStillRunning
//----------------------------------------------------------------------------------
//
// Description: Resets the deadlock check counter.  Use this function if your test
//				performs long operations without making an ITNExcutor call; to
//				prevent deadlock check from firing if your test is still active.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNExecutor::NoteTestIsStillRunning(void)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return;
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return;
	} // end if (in documentation mode)


	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();
} // CTNExecutor::NoteTestIsStillRunning
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::SetDeadlockCheck()"
//==================================================================================
// CTNExecutor::SetDeadlockCheck
//----------------------------------------------------------------------------------
//
// Description: Resets the deadlock check counter.  Use this function if your test
//				performs long operations without making an ITNExcutor call; to
//				prevent deadlock check from firing if your test is still active.
//
// Arguments:
//	BOOL fOn	Whether to turn deadlock checking on or off.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::SetDeadlockCheck(BOOL fOn)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	// Update the count, so the check doesn't go nuts if it's been off for a while.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();

	if (this->m_pRealExecutor->m_fDeadlockCheck == fOn)
	{
		DPL(0, "WARNING: Deadlock checking is already %s in test %u (case \"%s\").",
			3, ((fOn) ? "on" : "off"), this->m_pTest->m_dwUniqueID,
			this->m_pTest->m_pCase->m_pszID);
	} // end if (already set that way)
	else
	{
		DPL(0, "NOTE: Turning deadlock checking %s in test %u (case \"%s\").",
			3, ((fOn) ? "on" : "off"), this->m_pTest->m_dwUniqueID,
			this->m_pTest->m_pCase->m_pszID);

		this->m_pRealExecutor->m_fDeadlockCheck = fOn;
	} // end else (changing settings)

	return (S_OK);
} // CTNExecutor::SetDeadlockCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FreeResults()"
//==================================================================================
// CTNExecutor::FreeResults
//----------------------------------------------------------------------------------
//
// Description: Frees all results matching the given criteria, if specified.  If
//				both parameters are NULL, all results are freed.
//				Note that all output data and variables are destroyed as well.
//
// Arguments:
//	char* pszCaseID			Case ID to match, or NULL for all.
//	char* pszInstanceID		Instance ID to match, or NULL for all.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::FreeResults(char* pszCaseID, char* pszInstanceID)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	// Update the count, so the check doesn't go nuts if it's been off for a while.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();
	
	return (this->m_pRealExecutor->FreeResultsInternal(pszCaseID, pszInstanceID,
														this->m_pTest));
} // CTNExecutor::FreeResults
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::FreeOutputVars()"
//==================================================================================
// CTNExecutor::FreeOutputVars
//----------------------------------------------------------------------------------
//
// Description: Frees all output variables matching the given criteria.  If a
//				parameter is not NULL, then only vars from the corresponding test,
//				with a matching name, etc. are freed.  If all parameters are NULL,
//				all output vars are freed.
//
// Arguments:
//	char* pszCaseID			Case ID to match, or NULL for all.
//	char* pszInstanceID		Instance ID to match, or NULL for all.
//	char* pszName			Output variable name to match, or NULL for all.
//	char* pszType			Output variable type to match, or NULL for all.
//	BOOL fFreeData			Whether the data can be freed, too, if no variables are
//							left for a given result.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::FreeOutputVars(char* pszCaseID, char* pszInstanceID,
									char* pszName, char* pszType, BOOL fFreeData)
{
	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


	// Update the count, so the check doesn't go nuts if it's been off for a while.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();

	return (this->m_pRealExecutor->FreeOutputVarsInternal(pszCaseID, pszInstanceID,
														pszName, pszType, fFreeData,
														this->m_pTest));
} // CTNExecutor::FreeOutputVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::PromptUser()"
//==================================================================================
// CTNExecutor::PromptUser
//----------------------------------------------------------------------------------
//
// Description: Displays a dialog box for the user, with different behaviors
//				depending on the dialog type specified.
//				The main message body string will be parsed for special tokens using
//				the usual routine, see tncommon\sprintf.cpp for possible arguments.
//				If a tester in the passed in array drops or leaves the test, this
//				function returns TNWR_LOSTTESTER.  If piResponse is not NULL, then
//				the integer it points to is set to the tester number who was lost.
//				paiRelevantTesters can be NULL and iNumRelevantTesters be -1 to have
//				all other testers be relevant.  Specifying this slave's own tester
//				number is ignored.
//
// Arguments:
//	char* szTitle				Title for the message box.
//	char* szMessageFormat		String with optional special tokens that will be
//								displayed in the body of the message box.
//	DWORD dwDialogType			Style of the message box, see TNPUDT_xxx.
//	PVOID pvDialogData			Pointer to additional data for dialog box, if any.
//	DWORD dwDialogDataSize		Size of additional data for dialog box, if any.
//	int* paiRelevantTesters		Array of tester numbers that should cause this
//								function to fail if they drop, if any.
//	int iNumRelevantTesters		Number of testers in previous array, or -1 to
//								use all other testers..
//	int* piResponse				Optional place to store what the user responded
//								with.
//	DWORD dwNumParms			Number of parameters in the following variable
//								parameter list.
//	...							Variable parameter list; items are interpreted as
//								the special token replacements in szMessageFormat
//								string.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNExecutor::PromptUser(char* szTitle, char* szMessageFormat,
								DWORD dwDialogType,
								PVOID pvDialogData, DWORD dwDialogDataSize,
								int* paiRelevantTesters, int iNumRelevantTesters,
								int* piResponse,
								DWORD dwNumParms, ...)
{
	HRESULT		hr = S_OK;
	PVOID*		papvParams = NULL;
	va_list		currentparam;
	DWORD		dwCurrentItem = 0;
	char*		pszMessage = NULL;
	BOOL		fHaveTestMasterOpLock = FALSE;
	HWND		hPromptWnd = NULL;
	int			i;
	DWORD		dwTemp;
	DWORD		dwNumWaitObjects = 0;
	HANDLE		ahWaitObjects[3];
	MSG			msg;


	if (this == NULL)
	{
		DPL(0, "Executor object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (object is invalid)

	if ((szTitle == NULL) || (szMessageFormat == NULL))
	{
		DPL(0, "Title and message format strings cannot be NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (not given a title or message format)

	if ((iNumRelevantTesters < -1) || (iNumRelevantTesters >= this->m_pTest->m_iNumMachines))
	{
		DPL(0, "Test ID %u called function with invalid number of relevant testers (%i is <-1 or >=%i)!",
			3, this->m_pTest->m_dwUniqueID, iNumRelevantTesters,
			this->m_pTest->m_iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if ((paiRelevantTesters != NULL) && (iNumRelevantTesters == -1))
	{
		DPL(0, "Test ID %u called function with array of relevant testers but the all-other-testers special number!",
			1, this->m_pTest->m_dwUniqueID);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)

	if ((iNumRelevantTesters > 0) && (paiRelevantTesters == NULL))
	{
		DPL(0, "Test ID %u called function specifying %i relevant testers but no array!",
			2, this->m_pTest->m_dwUniqueID, iNumRelevantTesters);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (we got an invalid parameter)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	// If there are tokens to replace in the format string. build an array
	// and loop through the variable arguments and put them into that array.

	if (dwNumParms > 0)
	{
		papvParams = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
		if (papvParams == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		va_start(currentparam, dwNumParms);
		while(dwCurrentItem < dwNumParms)
		{
			papvParams[dwCurrentItem] = va_arg(currentparam, PVOID);
			dwCurrentItem++;
		} // end while (there are more variable parameter)
		va_end(currentparam);
	} // end if (there are parms)

	TNsprintf_array(&pszMessage, szMessageFormat, dwNumParms, papvParams);




	EnterCriticalSection(&(this->m_pTest->m_csMasterOp));
	fHaveTestMasterOpLock = TRUE;

	// We need to set a request ID (even though we won't actually use it) so that the
	// HandleLostTester code will know to ping our event.
	this->m_pTest->m_dwRequestID = 0xFFFFFFFF;


	if (this->m_pTest->m_hResponseEvent != NULL)
	{
		DPL(0, "Test ID %u's response event already in use?!",
			1, this->m_pTest->m_dwUniqueID);
		hr = ERROR_ALREADY_EXISTS;
		goto DONE;
	} // end if (we're already syncing)

	this->m_pTest->m_hResponseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_pTest->m_hResponseEvent == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't create lost tester event for test ID %u!",
			1, this->m_pTest->m_dwUniqueID);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't create event)


	// If there's an array of relevant testers, validate it.
	if (iNumRelevantTesters > 0)
	{
		for(i = 0; i < iNumRelevantTesters; i++)
		{
			// Make sure it's in bounds.
			if ((paiRelevantTesters[i] < 0) ||
				(paiRelevantTesters[i] >= this->m_pTest->m_iNumMachines))
			{
				DPL(0, "Relevant tester at index %i is invalid (%i is <0 or >=%i)!",
					3, i, paiRelevantTesters[i], this->m_pTest->m_iNumMachines);
				hr = ERROR_INVALID_PARAMETER;
				goto DONE;
			} // end if (invalid number)

			// Make sure this tester hasn't already left.
			if (this->m_pTest->m_paTesterSlots[paiRelevantTesters[i]].fGone)
			{
				DPL(0, "Tester number %i is already gone.",
					1, paiRelevantTesters[i]);

				if (piResponse != NULL)
					(*piResponse) = paiRelevantTesters[i];

				hr = TNWR_LOSTTESTER;
				goto DONE;
			} // end if (tester already gone)
		} // end for (each relevant tester)
	} // end if (there should be an array of relevant testers)



	switch (dwDialogType)
	{
		case TNPUDT_OK:
			if ((pvDialogData != NULL) || (dwDialogDataSize != 0))
			{
				DPL(0, "Cannot specify dialog data (%x != NULL or %u != 0) for TNPUDT_OK dialogs!",
					2, pvDialogData, dwDialogDataSize);
				hr = ERROR_INVALID_PARAMETER;
				goto DONE;
			} // end if (dialog data was specified)


			hPromptWnd = CreateDialog(s_hInstance, MAKEINTRESOURCE(IDD_PROMPT_OK),
									NULL, PromptAllDlgProc);
			if (hPromptWnd == NULL)
			{
				hr = GetLastError();

				DPL(0, "Couldn't load OK prompt dialog!  %e", 1, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't create dialog)
		  break;

		case TNPUDT_YESNO:
			if ((pvDialogData != NULL) || (dwDialogDataSize != 0))
			{
				DPL(0, "Cannot specify dialog data (%x != NULL or %u != 0) for TNPUDT_YESNO dialogs!",
					2, pvDialogData, dwDialogDataSize);
				hr = ERROR_INVALID_PARAMETER;
				goto DONE;
			} // end if (dialog data was specified)


			hPromptWnd = CreateDialog(s_hInstance, MAKEINTRESOURCE(IDD_PROMPT_YESNO),
									NULL, PromptAllDlgProc);
			if (hPromptWnd == NULL)
			{
				hr = GetLastError();

				DPL(0, "Couldn't load YesNo prompt dialog!  %e", 1, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't create dialog)
		  break;

		case TNPUDT_SELECTCOMBO:
			if ((pvDialogData == NULL) || (dwDialogDataSize < sizeof (char*)))
			{
				DPL(0, "Must specify at least one char* for dialog data (%x == NULL or %u < %u) for TNPUDT_OK dialogs!",
					3, pvDialogData, dwDialogDataSize, sizeof (char*));
				hr = ERROR_INVALID_PARAMETER;
				goto DONE;
			} // end if (dialog data wasn't specified)


			hPromptWnd = CreateDialog(s_hInstance, MAKEINTRESOURCE(IDD_PROMPT_SELECTCOMBO),
									NULL, PromptAllDlgProc);
			if (hPromptWnd == NULL)
			{
				hr = GetLastError();

				DPL(0, "Couldn't load Select Combo prompt dialog!  %e", 1, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (couldn't create dialog)

			ComboBox_AddString(GetDlgItem(hPromptWnd, IDCB_PROMPT),
							"Please select an item:");

			for(dwTemp = 0; dwTemp < (dwDialogDataSize / sizeof (char*)); dwTemp++)
			{
				ComboBox_AddString(GetDlgItem(hPromptWnd, IDCB_PROMPT),
								((char**) pvDialogData)[dwTemp]);
			} // end for (each entry)

			// Set the selection to be the "Please select" entry.
			ComboBox_SetCurSel(GetDlgItem(hPromptWnd, IDCB_PROMPT), 0);
		  break;

		default:
			DPL(0, "Unrecognized dialog type %u!", 1, dwDialogType);
			hr = E_NOTIMPL;
			goto DONE;
		  break;
	} // end switch (on dialog type)

	if (hPromptWnd == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't create user prompt window (type %u)!  %e",
			2, dwDialogType, hr);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't create window)


	SetWindowText(hPromptWnd, szTitle);
	SetWindowText(GetDlgItem(hPromptWnd, IDT_MESSAGE), pszMessage);

	ShowWindow(hPromptWnd, SW_SHOW);

	// Make sure all the messages in the queue are processed.
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while (there are messages)

	

	ahWaitObjects[dwNumWaitObjects++] = this->m_pTest->m_hResponseEvent;
	if (this->m_pRealExecutor->m_hUserCancelEvent != NULL)
	{
		ahWaitObjects[dwNumWaitObjects++] = this->m_pRealExecutor->m_hUserCancelEvent;
	} // end if (there's a user cancel event)

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this causes kernel to rewalk array



REWAIT:
	// Make sure the test thread still looks alive.
	this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();


	// Since our window needs a message pump, we want to check the Windows message
	// queue and process any messages.  If it's the message saying the window is
	// now closed, we can bail.
	// We use a while loop to make sure all messages get processed in a timely
	// fashion.
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		// This is our indication that EndDialog is about to be called.
		if (msg.message == WM_MYCLOSEDIALOG)
		{
			switch (dwDialogType)
			{
				case TNPUDT_SELECTCOMBO:
					// The item returned is the index into the array
					// of selections.  Since the first one is bogus,
					// skip it.

					DPL(1, "User selected item %i, \"%s\".",
						2, msg.wParam - 1,
						((char**) pvDialogData)[msg.wParam - 1]);

					if (piResponse != NULL)
						(*piResponse) = (int) msg.wParam - 1;
				  break;

				default:
					if (piResponse != NULL)
						(*piResponse) = (int) msg.wParam;
				  break;
			} // end switch (on the dialog type)

			// We'll continue processing the message, but it should
			// result in msg.message becoming zero, so we drop out
			// below.
		} // end if (end of dialog)

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// This means we're done.
		if (msg.message == 0)
		{
			hr = S_OK;
			goto DONE;
		} // end if (end of dialog)
	} // end while (there are Windows messages)


	// Now we wait for a short time (quarter second) to see if a tester was lost,
	// the user cancelled, or an I/O completion occurred.
	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								250, TRUE);
	switch (hr)
	{
		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		case WAIT_TIMEOUT:
			// Check the Windows message queue and go back to waiting.
			goto REWAIT;
		  break;

		case WAIT_FAILED:
			hr = GetLastError();

			DPL(0, "Wait failed!  %e", 1, hr);

			if (hr == S_OK)
				hr = E_FAIL;

			goto DONE;
		  break;

		case WAIT_OBJECT_0:
			if (this->m_pTest->m_hrResponseResult != TNERR_LOSTTESTER)
			{
				hr = this->m_pTest->m_hrResponseResult;

				DPL(0, "Got unexpected LostTester event firing!  %e",
					1, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (not a lost tester error)


			// See which tester was lost (and if we care about him).
			if (iNumRelevantTesters == -1)
			{
				for(i = 0; i < this->m_pTest->m_iNumMachines; i++)
				{
					if (this->m_pTest->m_paTesterSlots[i].fGone)
					{
						DPL(0, "Lost tester %i during PromptUser in test ID %u!",
							2, i, this->m_pTest->m_dwUniqueID);

						// Note the tester who was lost if the user wants it.
						if (piResponse != NULL)
							(*piResponse) = i;

						hr = TNWR_LOSTTESTER;
						goto DONE;
					} // end if (found a tester who's gone)
				} // end for (each tester)
			} // end if (we care about all of the other testers)

			for(i = 0; i < iNumRelevantTesters; i++)
			{
				if (this->m_pTest->m_paTesterSlots[paiRelevantTesters[i]].fGone)
				{
					DPL(0, "Lost tester %i (array entry %i) during PromptUser in test ID %u!",
						3, paiRelevantTesters[i], i,
						this->m_pTest->m_dwUniqueID);

					// Note the tester who was lost if the user wants it.
					if (piResponse != NULL)
						(*piResponse) = paiRelevantTesters[i];

					hr = TNWR_LOSTTESTER;
					goto DONE;
				} // end if (syncing with dead guy)
			} // end for (each relevant tester)

			// If we got here, it means doesn't care about that guy.
			goto REWAIT;
		  break;

		case WAIT_OBJECT_0 + 1:
			// If we waited on a user cancel event, then it would be seen as
			// WAIT_OBJECT_0 + 1 if it got triggered.
			DPL(3, "User cancelled test ID %u during PromptUser.",
				1, this->m_pTest->m_dwUniqueID);

			hr = TNWR_USERCANCEL;
			goto DONE;
		  break;

		default:
			DPL(0, "Test ID %u got unexpected return from WaitForMultipleObjectsEx!  %e",
				2, this->m_pTest->m_dwUniqueID, hr);
			hr = E_FAIL;
			goto DONE;
		  break;
	} // end switch (on wait result)


DONE:

	if (fHaveTestMasterOpLock)
	{
		this->m_pTest->m_dwRequestID = 0;
		this->m_pTest->m_hrResponseResult = E_FAIL;

		if (this->m_pTest->m_hResponseEvent != NULL)
		{
			CloseHandle(this->m_pTest->m_hResponseEvent);
			this->m_pTest->m_hResponseEvent = NULL;
		} // end if (we have response event)

		fHaveTestMasterOpLock = FALSE;
		LeaveCriticalSection(&(this->m_pTest->m_csMasterOp));
	} // end if (have test master op lock)

	if (pszMessage != NULL)
		TNsprintf_free(&pszMessage);

	if (papvParams != NULL)
	{
		LocalFree(papvParams);
		papvParams = NULL;
	} // end if (have item array)

	return (hr);
} // CTNExecutor::PromptUser
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::GetMachineInfoForTester()"
//==================================================================================
// CTNExecutor::GetMachineInfoForTester
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the given tester's CTNMachineInfo object
//				NOTE: You must release your reference to the object returned with a
//				call to ReleaseMachineInfoForTester.
//
// Arguments:
//	int iTesterNum			Tester number whose info should be retrieved.
//	PTNMACHINEINFO* ppInfo	Place to store pointer to info of tester.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::GetMachineInfoForTester(int iTesterNum, PTNMACHINEINFO* ppInfo)
{
	HRESULT		hr = S_OK;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)

	DPL(9, "==>(%i, %x)", 2, iTesterNum, ppInfo);

	if (ppInfo == NULL)
	{
		DPL(0, "Must pass a non-NULL place to store info pointer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (bad destination pointer)

	if ((iTesterNum < 0) || (iTesterNum > this->m_pTest->m_iNumMachines))
	{
		DPL(0, "Didn't specify a valid tester number (%i is <0 or >%i)!",
			2, iTesterNum, this->m_pTest->m_iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (invalid tester number)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	if (iTesterNum == this->m_pTest->GetSlavesTesterNum(&(this->m_pRealExecutor->m_id)))
	{
		DPL(0, "WARNING: Getting information for this machine (tester %i)!",
			1, iTesterNum);

		// The user is making a function call, so the test thread is still alive.
		this->m_pRealExecutor->m_dwLastTestthreadPing = GetTickCount();

		(*ppInfo) = &(this->m_pRealExecutor->m_info);
		goto DONE;
	} // end if (getting own info)


	// We're not getting our own info, so we have to look it up.
	hr = this->m_pRealExecutor->GetTestersMachineInfoInternal(this->m_pTest,
															iTesterNum,
															ppInfo);


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::GetMachineInfoForTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::ReleaseMachineInfoForTester()"
//==================================================================================
// CTNExecutor::ReleaseMachineInfoForTester
//----------------------------------------------------------------------------------
//
// Description: Releases a machine info object after previously retrieving it with
//				GetMachineInfoForTester.
//
// Arguments:
//	PTNMACHINEINFO* ppInfo	Pointer to info object pointer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::ReleaseMachineInfoForTester(PTNMACHINEINFO* ppInfo)
{
	HRESULT					hr = S_OK;
	PTNOTHERMACHINEINFO		pOtherInfo = NULL;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	DPL(9, "==>(%x)", 1, ppInfo);

	if (ppInfo == NULL)
	{
		DPL(0, "Must pass a non-NULL pointer to info pointer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (bad pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in documentation mode)


	pOtherInfo = (PTNOTHERMACHINEINFO) (*ppInfo);
	(*ppInfo) = NULL;


	pOtherInfo->m_dwRefCount--;
	if (pOtherInfo->m_dwRefCount == 0)
	{
		DPL(7, "Deleting other info object %x.",
			1, pOtherInfo);
		delete (pOtherInfo);
	} // end if (last reference)
	else
	{
		DPL(7, "Not deleting other info object %x, its refcount is %u.",
			2, pOtherInfo, pOtherInfo->m_dwRefCount);
	} // end if (last reference)
	pOtherInfo = NULL;


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNExecutor::ReleaseMachineInfoForTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::NoteBeginTestCase()"
//==================================================================================
// CTNExecutor::NoteBeginTestCase
//----------------------------------------------------------------------------------
//
// Description: Notes the beginning of a test case.
//
// Arguments:
//	char* szDebugModule				Debug information module (for debug printing).
//	char* szDebugSection			Debug information section (for debug printing).
//	char* szBeginTestCaseFilepath	Path to source file where beginning of test
//									case code resides.
//	int iBeginTestCaseLineNum		Line number indicating beginning of test case.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::NoteBeginTestCase(char* szDebugModule,
										char* szDebugSection,
										char* szBeginTestCaseFilepath,
										int iBeginTestCaseLineNum)
{
	char	szTemp[1024];


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)



#ifndef _XBOX // removed unnecessary logging
	wsprintf(szTemp, "Entering test case \"%s\" (ID %u).",
			this->m_pTest->m_pCase->m_pszID, this->m_pTest->m_dwUniqueID);

	// This is an expansion of the DNB macro.
	TNInternalDebugNoteBlock(szDebugModule, szDebugSection, szBeginTestCaseFilepath,
							iBeginTestCaseLineNum, szTemp);
#endif // ! XBOX


	// If we're not doing any ErrToSrc stuff, skip the rest.
	if (this->m_pRealExecutor->m_hErrToSrcFile == INVALID_HANDLE_VALUE)
		return (S_OK);


#ifdef DEBUG
	if (this->m_pszTestSectionName != NULL)
	{
		DPL(0, "Test section name already exists (\"%s\")!?  DEBUGBREAK()-ing.",
			1, this->m_pszTestSectionName);
		DEBUGBREAK();
	} // end if (have string)

	if (this->m_pszTestSectionFilepath != NULL)
	{
		DPL(0, "Test section filepath already exists (\"%s\")!?  DEBUGBREAK()-ing.",
			1, this->m_pszTestSectionFilepath);
		DEBUGBREAK();
	} // end if (have string)
#endif // DEBUG


	this->m_pszTestSectionName = (char*) LocalAlloc(LPTR, strlen(szTemp) + 1);
	if (this->m_pszTestSectionName == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)
	strcpy(this->m_pszTestSectionName, szTemp);

	this->m_pszTestSectionFilepath = (char*) LocalAlloc(LPTR, strlen(szBeginTestCaseFilepath) + 1);
	if (this->m_pszTestSectionFilepath == NULL)
	{
		LocalFree(this->m_pszTestSectionName);
		this->m_pszTestSectionName = NULL;
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)
	strcpy(this->m_pszTestSectionFilepath, szBeginTestCaseFilepath);

	this->m_iTestSectionLineNum = iBeginTestCaseLineNum;


	return (S_OK);
} // CTNExecutor::NoteBeginTestCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::NoteNewTestSection()"
//==================================================================================
// CTNExecutor::NoteNewTestSection
//----------------------------------------------------------------------------------
//
// Description: Notes the beginning of a new test section.
//
// Arguments:
//	char* szTestSectionName		Name/description of test section.
//	char* szDebugModule			Debug information module (for debug printing).
//	char* szDebugSection		Debug information section (for debug printing).
//	char* szTestSectionFilepath	Path to source file where section test code resides.
//	int iTestSectionLineNum		Line number indicating section test code.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::NoteNewTestSection(char* szTestSectionName,
										char* szDebugModule,
										char* szDebugSection,
										char* szTestSectionFilepath,
										int iTestSectionLineNum)
{
	unsigned int	uiSize;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


#ifndef _XBOX // removed unnecessary logging
	// This is an expansion of the DPL macro.
	TNInternalDebugPrepLog(szDebugModule, szDebugSection);
	TNInternalDebugPrintLine(1, szTestSectionName, 0);
#endif // ! XBOX

	// Ignore error
	this->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID, szTestSectionName, 0);


#ifndef _XBOX // removed unnecessary logging
	// This is an expansion of the DNB macro.
	TNInternalDebugNoteBlock(szDebugModule, szDebugSection, szTestSectionFilepath,
							iTestSectionLineNum, szTestSectionName);
#endif // ! XBOX


	// If we're not doing any ErrToSrc stuff, skip the rest.
	if (this->m_pRealExecutor->m_hErrToSrcFile == INVALID_HANDLE_VALUE)
		return (S_OK);


#ifdef DEBUG
	if (this->m_pszTestSectionName == NULL)
	{
		DPL(0, "Test section name doesn't already exist (should have been begin test case)!?  DEBUGBREAK()-ing.", 0);
		DEBUGBREAK();
	} // end if (have string)

	if (this->m_pszTestSectionFilepath == NULL)
	{
		DPL(0, "Test section filepath doesn't already exist (should have been begin test case)!?  DEBUGBREAK()-ing.", 0);
		DEBUGBREAK();
	} // end if (have string)
#endif // DEBUG

	LocalFree(this->m_pszTestSectionName);
	this->m_pszTestSectionName = NULL;

	LocalFree(this->m_pszTestSectionFilepath);
	this->m_pszTestSectionFilepath = NULL;


	this->m_pszTestSectionName = (char*) LocalAlloc(LPTR, strlen(szTestSectionName) + 1);
	if (this->m_pszTestSectionName == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)
	strcpy(this->m_pszTestSectionName, szTestSectionName);


	uiSize = strlen(szTestSectionFilepath) + 1;

	// If we're replacing filepaths, make sure we've got room to hold the modified
	// version.
	if (this->m_pRealExecutor->m_pszErrToSrcPathReplace != NULL)
		uiSize += strlen(this->m_pRealExecutor->m_pszErrToSrcPathReplace);


	this->m_pszTestSectionFilepath = (char*) LocalAlloc(LPTR, uiSize);
	if (this->m_pszTestSectionFilepath == NULL)
	{
		LocalFree(this->m_pszTestSectionName);
		this->m_pszTestSectionName = NULL;
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)
	strcpy(this->m_pszTestSectionFilepath, szTestSectionFilepath);

	if (this->m_pRealExecutor->m_pszErrToSrcPathReplace != NULL)
	{
		StringReplaceAll(this->m_pszTestSectionFilepath,
						this->m_pRealExecutor->m_pszErrToSrcPathSearch,
						this->m_pRealExecutor->m_pszErrToSrcPathReplace,
						FALSE,
						0,
						uiSize);
	} // end if (modifying search paths)

	this->m_iTestSectionLineNum = iTestSectionLineNum;


	return (S_OK);
} // CTNExecutor::NoteNewTestSection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::NoteThrowTestResult()"
//==================================================================================
// CTNExecutor::NoteThrowTestResult
//----------------------------------------------------------------------------------
//
// Description: Notes a test result throw in the ErrToSrc file.
//
// Arguments:
//	char* szThrowTestResultFilepath		Path to source file where throw test result
//										code resides.
//	int iThrowTestResultLineNum			Line number indicating throw test result.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::NoteThrowTestResult(char* szThrowTestResultFilepath,
										int iThrowTestResultLineNum)
{
	unsigned int	uiSize;
	char*			pszModifiedThrowFilePath;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile != INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a regular build of this module but in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a regular build of this module but in documentation mode!", 0);
		return (E_FAIL);
	} // end if (in documentation mode)


#ifndef _XBOX // no ErrToSrcFile supported
	// If we're not doing any ErrToSrc stuff, there's nothing to do in here.
	if (this->m_pRealExecutor->m_hErrToSrcFile == INVALID_HANDLE_VALUE)
		return (S_OK);

	FileWriteLine(this->m_pRealExecutor->m_hErrToSrcFile, "[ThrowTestResult]");

	if ((this->m_pszTestSectionName != NULL) &&
		(this->m_pszTestSectionFilepath != NULL))
	{
		FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
							"LastTestSectionName=%s",
							1, this->m_pszTestSectionName);

		FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
							"LastTestSectionFile=%s",
							1, this->m_pszTestSectionFilepath);

		FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
							"LastTestSectionLine=%i",
							1, this->m_iTestSectionLineNum);
	} // end if (there's a test section)

	FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
						"ThrowTestID=%u",
						1, this->m_pTest->m_dwUniqueID);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
						"ThrowTestCase=%s",
						1, this->m_pTest->m_pCase->m_pszID);

	if (this->m_pRealExecutor->m_pszErrToSrcPathReplace == NULL)
	{
		FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
							"ThrowFile=%s",
							1, szThrowTestResultFilepath);
	} // end if (replace path)
	else
	{
		uiSize = strlen(szThrowTestResultFilepath) + strlen(this->m_pRealExecutor->m_pszErrToSrcPathReplace) + 1;

		// Make sure we've got room to hold the modified version.
		pszModifiedThrowFilePath = (char*) LocalAlloc(LPTR, uiSize);
		if (pszModifiedThrowFilePath == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate memory)
		strcpy(pszModifiedThrowFilePath, szThrowTestResultFilepath);

		StringReplaceAll(pszModifiedThrowFilePath,
						this->m_pRealExecutor->m_pszErrToSrcPathSearch,
						this->m_pRealExecutor->m_pszErrToSrcPathReplace,
						FALSE,
						0,
						uiSize);

		FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
							"ThrowFile=%s",
							1, pszModifiedThrowFilePath);

		LocalFree(pszModifiedThrowFilePath);
	} // end if (modifying search paths)

	FileSprintfWriteLine(this->m_pRealExecutor->m_hErrToSrcFile,
						"ThrowLine=%i",
						1, iThrowTestResultLineNum);

	// Double space
	FileWriteLine(this->m_pRealExecutor->m_hErrToSrcFile, "");

	return (S_OK);
#else // ! XBOX
	return (S_OK);
#endif // XBOX
} // CTNExecutor::NoteThrowTestResult
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no documentation builds supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::DocBuildBeginTestCase()"
//==================================================================================
// CTNExecutor::DocBuildBeginTestCase
//----------------------------------------------------------------------------------
//
// Description: Documents the beginning of a test case.
//
// Arguments:
//	char* szBeginTestCaseFilepath	Path to source file where beginning of test
//									case code resides.
//	int iBeginTestCaseLineNum		Line number indicating beginning of test case.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::DocBuildBeginTestCase(char* szBeginTestCaseFilepath,
										int iBeginTestCaseLineNum)
{

	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile == INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a documentation-only build of this module but not in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a documentation-only build of this module but not in documentation mode!", 0);
		return (E_FAIL);
	} // end if (not in documentation mode)
	

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"[%s]", 1, this->m_pTest->m_pCase->m_pszID);

	FileWriteLine(this->m_pRealExecutor->m_hDocFile, "{BeginCase}");

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"Name=%s", 1, this->m_pTest->m_pCase->m_pszName);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"File=%s", 1, szBeginTestCaseFilepath);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"Line=%i", 1, iBeginTestCaseLineNum);

	return (S_OK);
} // CTNExecutor::DocBuildBeginTestCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no documentation builds supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::DocBuildEndTestCase()"
//==================================================================================
// CTNExecutor::DocBuildEndTestCase
//----------------------------------------------------------------------------------
//
// Description: Documents the end of a test case.
//
// Arguments:
//	char* szEndTestCaseFilepath		Path to source file where end of test case code
//									resides.
//	int iEndTestCaseLineNum			Line number indicating end of test case.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::DocBuildEndTestCase(char* szEndTestCaseFilepath,
										int iEndTestCaseLineNum)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile == INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a documentation-only build of this module but not in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a documentation-only build of this module but not in documentation mode!", 0);
		return (E_FAIL);
	} // end if (not in documentation mode)
	

	FileWriteLine(this->m_pRealExecutor->m_hDocFile, "{EndCase}");

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"File=%s", 1, szEndTestCaseFilepath);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,

						"Line=%i", 1, iEndTestCaseLineNum);

	FileWriteLine(this->m_pRealExecutor->m_hDocFile, ""); // double space

	return (S_OK);
} // CTNExecutor::DocBuildEndTestCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no documentation builds supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::DocBuildNewTestSection()"
//==================================================================================
// CTNExecutor::DocBuildNewTestSection
//----------------------------------------------------------------------------------
//
// Description: Documents the beginning of a new test section.
//
// Arguments:
//	char* szTestSectionName		Name/description of test section.
//	char* szTestSectionFilepath	Path to source file where section test code resides.
//	int iTestSectionLineNum		Line number indicating section test code.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::DocBuildNewTestSection(char* szTestSectionName,
											char* szTestSectionFilepath,
											int iTestSectionLineNum)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile == INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a documentation-only build of this module but not in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a documentation-only build of this module but not in documentation mode!", 0);
		return (E_FAIL);
	} // end if (not in documentation mode)
	

	FileWriteLine(this->m_pRealExecutor->m_hDocFile, "{TestSection}");

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"Name=%s", 1, szTestSectionName);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"File=%s", 1, szTestSectionFilepath);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"Line=%i", 1, iTestSectionLineNum);

	return (S_OK);
} // CTNExecutor::DocBuildNewTestSection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef _XBOX // no documentation builds supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNExecutor::DocBuildTestSectionControl()"
//==================================================================================
// CTNExecutor::DocBuildTestSectionControl
//----------------------------------------------------------------------------------
//
// Description: Documents the beginning of a test section control statement block.
//
// Arguments:
//	char* szControl				Type of control statement.
//	char* pszControlExpression	Expression determining when block is executed, if
//								any.
//	char* szControlFilepath		Path to source file where block's test code resides.
//	int iControlLineNum			Line number indicating block's test code.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNExecutor::DocBuildTestSectionControl(char* szControl,
												char* pszControlExpression,
												char* szControlFilepath,
												int iControlLineNum)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bad object pointer)


	// Make sure we're in the right mode.
	if (this->m_pRealExecutor->m_hDocFile == INVALID_HANDLE_VALUE)
	{
		DPL(0, "Executing a documentation-only build of this module but not in documentation mode!", 0);
		this->Log(TNLF_CRITICAL,
					"Executing a documentation-only build of this module but not in documentation mode!", 0);
		return (E_FAIL);
	} // end if (not in documentation mode)
	

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"{%s}", 1, szControl);

	if (pszControlExpression != NULL)
	{
		FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
							"Expression=%s", 1, pszControlExpression);
	} // end if (there's an expression)

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"File=%s", 1, szControlFilepath);

	FileSprintfWriteLine(this->m_pRealExecutor->m_hDocFile,
						"Line=%i", 1, iControlLineNum);

	return (S_OK);
} // CTNExecutor::DocBuildTestSectionControl
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"ExecutorTestThreadProc()"
//==================================================================================
// ExecutorTestThreadProc
//----------------------------------------------------------------------------------
//
// Description: Simple wrapper for DoTestThread
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to a CTNExecutorPriv pointer.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI ExecutorTestThreadProc(LPVOID lpvParameter)
{
	HRESULT				hr;
	PTNEXECUTORPRIV		pThisObj = (PTNEXECUTORPRIV) lpvParameter;


#ifndef _XBOX // no GetCurrentProcessId
	DPL(1, "==> Starting up (this = %x, process = %x, thread = %x/%u).",
		4, pThisObj, GetCurrentProcessId(), GetCurrentThreadId(),
		GetCurrentThreadId());
#endif

	hr = pThisObj->DoTestThread();

#ifndef _XBOX // no GetCurrentProcessId
	DPL(1, "<== Shutting down (this = %x, process = %x, thread = %x/%u).  %e",
		5, pThisObj, GetCurrentProcessId(), GetCurrentThreadId(),
		GetCurrentThreadId(), hr);
#endif

	return (hr);
} // ExecutorTestThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"PromptAllDlgProc()"
//==================================================================================
// PromptAllDlgProc
//----------------------------------------------------------------------------------
//
// Description: Prompt Ok and YesNo dialogs window procedure.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK PromptAllDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND	hComboBox;
	int		iSelection;


	switch (uMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					DPL(3, "User pressed 'OK'.", 0);

					hComboBox = GetDlgItem(hWnd, IDCB_PROMPT);
					if (hComboBox != NULL)
					{
						iSelection = ComboBox_GetCurSel(hComboBox);
						PostMessage(hWnd, WM_MYCLOSEDIALOG, iSelection, 0);
					} // end if (there's a combo box)
					else
					{
						PostMessage(hWnd, WM_MYCLOSEDIALOG, TNPUR_OK, 0);
					} // end else (there's no combo box)
				  break;

				case IDYES:
					DPL(3, "User pressed 'Yes'.", 0);

					PostMessage(hWnd, WM_MYCLOSEDIALOG, TNPUR_YES, 0);
				  break;

				case IDNO:
					DPL(3, "User pressed 'No'.", 0);

					PostMessage(hWnd, WM_MYCLOSEDIALOG, TNPUR_NO, 0);
				  break;

				case IDCB_PROMPT:
					//BUGBUG figure out what this is
					if (HIWORD(wParam) == 9)
					{
						// Find out what just got selected.
						hComboBox = GetDlgItem(hWnd, LOWORD(wParam));
						iSelection = ComboBox_GetCurSel(hComboBox);

						// If the "Please select..." string was selected, then
						// the user should not be able to select OK.
						EnableWindow(GetDlgItem(hWnd, IDOK),
									((iSelection > 0) ? TRUE : FALSE));
					} // end if (?)
				  break;
			} // end switch (on the button pressed/control changed)
		  break;

		case WM_MYCLOSEDIALOG:
			EndDialog(hWnd, 0);
		  break;
	} // end switch (on the type of window message)

	return (0);
} // PromptYesNoDlgProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX