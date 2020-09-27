#ifndef _XBOX // no master supported
//==================================================================================
// Includes
//==================================================================================
#define INCL_WINSOCK_API_TYPEDEFS 1 // includes winsock2 fn proto's, for getprocaddress
#ifndef _XBOX
#include <winsock2.h>
#else // ! XBOX
#include <winsockx.h>		// Needed for XnetInitialize
#endif // XBOX
#include <windows.h>

#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\sprintf.h"
#include "..\tncommon\fileutils.h"

#include "tncontrl.h"
#include "main.h"
#include "sendq.h"
#include "comm.h"
#include "commtcp.h"
#include "reportitem.h"
#include "slaveinfo.h"
#include "script.h"

#include "master.h"







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CTNMaster()"
//==================================================================================
// CTNMaster constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNMaster object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNMaster::CTNMaster(void)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNMaster));


	// Any changes here should go in CloseControlSession as well.
	this->m_hKillJobThreadEvent = NULL; 

	this->m_dwMode = 0;
	ZeroMemory(&(this->m_moduleID), sizeof (TNMODULEID));
	this->m_pszSessionFilter = NULL;
	this->m_pszClosingReportPath = NULL;
	this->m_pszSiblingFailuresMemDumpDirPath = NULL;
	this->m_pszSkippedTestsPath = NULL;
	this->m_pFailureBreakRules = NULL;

	this->m_pfnGetStartupData = NULL;
	this->m_pfnLoadStartupData = NULL;
	this->m_pfnWriteStartupData = NULL;
	this->m_pfnHandleUserQuery = NULL;

	this->m_dwSessionID = 0;
	ZeroMemory(&this->m_testChecksum, sizeof (CHECKSUM));
	//this->m_hAllSlavesCommStatusEvent = NULL;
	this->m_dwCurrentUniqueID = 1;
	this->m_dwStartupTicks = 0;
	ZeroMemory(&(this->m_tStartupTime), sizeof (time_t));
	this->m_dwTimelimit = 0;
	this->m_dwAllowJoinersTime = 0;
	this->m_fJoinersAllowed = FALSE;
	this->m_fTestingComplete = FALSE;

	this->m_pvMetaMasterAddress = NULL;
	this->m_dwMetaMasterAddressSize = 0;
	ZeroMemory(&(this->m_id), sizeof (TNCTRLMACHINEID));
	this->m_fRegistered = FALSE;

	this->m_fPingSlaves = FALSE;
	this->m_hJobThread = NULL;

	this->m_dwAutoReportHeaders = 0;
	this->m_dwAutoReportBodyStyle = 0;
	this->m_dwAutoReportFlags = 0;
	this->m_iAutoReportNumReports = -1;
	this->m_pszAutoReportDirpath = NULL;
	this->m_fAutoReportDeleteNonPrinted = FALSE;

	this->m_iTotalNumSlaves = 0;

	this->m_fReportInBVTFormat = FALSE;
} // CTNMaster::CTNMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::~CTNMaster()"
//==================================================================================
// CTNMaster destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNMaster object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNMaster::~CTNMaster(void)
{
	HRESULT				hr;


	DPL(9, "this = %x", 1, this);

	hr = this->DestroyControlSession();
	if (hr != S_OK)
	{
		DPL(0, "Destroying control layer session failed!  %e", 1, hr);
	} // end if (destroying session failed)
} // CTNMaster::~CTNMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CreateControlSession()"
//==================================================================================
// CTNMaster::CreateControlSession
//----------------------------------------------------------------------------------
//
// Description: Starts up the control session and begins hosting.
//
// Arguments:
//	PTNCREATECONTROLSESSIONDATA pTNccsd		Parameter block.  See header file for
//											information.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CreateControlSession(PTNCREATECONTROLSESSIONDATA pTNccsd)
{

	HRESULT					hr;
	CONTROLCOMMINITPBLOCK	ctrlcomminitparams;
	DWORD					dwTemp;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we got passed a bad pointer)

	if (pTNccsd == NULL)
	{
		DPL(0, "Must pass a valid structure pointer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if (pTNccsd->dwSize != sizeof (TNCREATECONTROLSESSIONDATA))
	{
		DPL(0, "Must pass a structure of correct size!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if (pTNccsd->dwAPIVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "WARNING: Caller requesting different API version (%i != %i), problems may occur.",
			2, pTNccsd->dwAPIVersion, CURRENT_TNCONTROL_API_VERSION);
	} // end if (got passed an invalid parameter)

	if ((pTNccsd->dwMode != TNMODE_API) &&
		(pTNccsd->dwMode != TNMODE_STRESS) &&
		(pTNccsd->dwMode != TNMODE_PICKY) &&
		(pTNccsd->dwMode != TNMODE_POKE))
	{
		DPL(0, "Must pass a valid mode (%u is invalid)!", 1, pTNccsd->dwMode);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if ((pTNccsd->dwControlMethodID != TN_CTRLMETHOD_TCPIP_OPTIMAL) &&
			(pTNccsd->dwControlMethodID != TN_CTRLMETHOD_TCPIP_WINSOCK1))
	{
		DPL(0, "Must pass a control method ID (%u is invalid)!",
			1, pTNccsd->dwControlMethodID);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if ((pTNccsd->pModuleID == NULL) ||
		(pTNccsd->apfnLoadTestTable == NULL) ||
			(pTNccsd->dwNumLoadTestTables < 1) ||
			(pTNccsd->apfnLoadTestTable[0] == NULL))
	{
		DPL(0, "Must pass a valid module ID and LoadTestTable callbacks array!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if ((pTNccsd->pszTestlistFilepath == NULL) &&
		(pTNccsd->dwMode != TNMODE_POKE))
	{
		DPL(0, "Must pass a valid path to a script in modes other than poke mode!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if (pTNccsd->iNumSlaves < 0)
	{
		DPL(0, "Must pass a valid number of slaves expected, or 0 for a free join session (%i is invalid)!",
			1, pTNccsd->iNumSlaves);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if ((pTNccsd->dwMode != TNMODE_API) &&
		(pTNccsd->iNumSlaves == 0) && (pTNccsd->dwAllowAdditionalJoinersTime == 0))
	{
		DPL(0, "API mode requires the session to be closed before testing can begin, so you must pass an expected number of slaves or allow joiners time limit!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (API mode and no slave or time session close specified)

	switch (pTNccsd->dwMode)
	{
		case TNMODE_API:
			DPL(0, "Using API mode.", 0);
		break;

		case TNMODE_STRESS:
			DPL(0, "Using stress mode.", 0);
		  break;

		case TNMODE_POKE:
			DPL(0, "Using poke watcher mode.", 0);
		  break;

		case TNMODE_PICKY:
			DPL(0, "Using picky mode.", 0);
		  break;
	} // end switch (on mode)
	this->m_dwMode = pTNccsd->dwMode;

	/*
	this->m_hAllSlavesCommStatusEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_hAllSlavesCommStatusEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Failed to create comm status event!", 0);
		goto ERROR_EXIT;
	} // end if (failed to create event)
	*/


	ZeroMemory(&ctrlcomminitparams, sizeof (CONTROLCOMMINITPBLOCK));
	ctrlcomminitparams.dwSize = sizeof (CONTROLCOMMINITPBLOCK);
	ctrlcomminitparams.dwControlMethodID = pTNccsd->dwControlMethodID;
	ctrlcomminitparams.pControlLayerObj = this;
	ctrlcomminitparams.fMaster = TRUE;
	ctrlcomminitparams.dwFlags = pTNccsd->dwMethodFlags;
	ctrlcomminitparams.pvData = pTNccsd->pvMethodData;
	ctrlcomminitparams.dwDataSize = pTNccsd->dwMethodDataSize;

	// Initialize desired control method
	switch (pTNccsd->dwControlMethodID)
	{
		case TN_CTRLMETHOD_TCPIP_OPTIMAL:
			hr = CtrlCommTCPLoadOptimal(&ctrlcomminitparams, &(this->m_pCtrlComm));
			if (hr != S_OK)
			{
				DPL(0, "Initializing TCP/IP optimal control method failed!", 0);
				goto ERROR_EXIT;
			} // end if (initting TCP/IP optimal control method failed)
		break;

		case TN_CTRLMETHOD_TCPIP_WINSOCK1:
			hr = CtrlCommTCPLoadWinSock1(&ctrlcomminitparams, &(this->m_pCtrlComm));
			if (hr != S_OK)
			{
				DPL(0, "Initializing TCP/IP WinSock1 control method failed!", 0);
				goto ERROR_EXIT;
			} // end if (initting TCP/IP WinSock1 control method failed)
		  break;
	} // end switch (on control method ID)


	// Initialize our data
	CopyMemory(&(this->m_moduleID), pTNccsd->pModuleID, sizeof (TNMODULEID));

	if (pTNccsd->pszSessionFilter != NULL)
	{
		this->m_pszSessionFilter = (char*) LocalAlloc(LPTR, (strlen(pTNccsd->pszSessionFilter) + 1));
		if (this->m_pszSessionFilter == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)

		strcpy(this->m_pszSessionFilter, pTNccsd->pszSessionFilter);
	} // end if (theres a user session ID)

	if (pTNccsd->pszSiblingFailuresMemDumpDirPath != NULL)
	{
		this->m_pszSiblingFailuresMemDumpDirPath = (char*) LocalAlloc(LPTR, (strlen(pTNccsd->pszSiblingFailuresMemDumpDirPath) + 1));
		if (this->m_pszSiblingFailuresMemDumpDirPath == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)

		strcpy(this->m_pszSiblingFailuresMemDumpDirPath, pTNccsd->pszSiblingFailuresMemDumpDirPath);
	} // end if (theres a sibling failure dump dir)

	if (pTNccsd->pszSkippedTestsPath != NULL)
	{
		this->m_pszSkippedTestsPath = (char*) LocalAlloc(LPTR, (strlen(pTNccsd->pszSkippedTestsPath) + 1));
		if (this->m_pszSkippedTestsPath == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)

		strcpy(this->m_pszSkippedTestsPath, pTNccsd->pszSkippedTestsPath);
	} // end if (theres a skipped test path)
	
	
	this->m_pfnLogString = pTNccsd->pfnLogString;
	if (pTNccsd->hCompletionOrUpdateEvent != NULL)
	{
		if (! DuplicateHandle(GetCurrentProcess(), pTNccsd->hCompletionOrUpdateEvent,
								GetCurrentProcess(), &(this->m_hCompletionOrUpdateEvent),
								0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hr = GetLastError();
			DPL(0, "Couldn't duplicate user's completion or update event handle (%x)!",
				2, pTNccsd->hCompletionOrUpdateEvent);
			goto ERROR_EXIT;
		} // end if (couldn't duplicate handle)
	} // end if (there's an event)

	this->m_pfnGetStartupData = pTNccsd->pfnGetStartupData;
	this->m_pfnLoadStartupData = pTNccsd->pfnLoadStartupData;
	this->m_pfnWriteStartupData = pTNccsd->pfnWriteStartupData;
	this->m_dwTimelimit = pTNccsd->dwTimelimit;
	this->m_iSlaveNumLimit = pTNccsd->iNumSlaves;
	this->m_dwAllowJoinersTime = pTNccsd->dwAllowAdditionalJoinersTime;
	this->m_pFailureBreakRules = pTNccsd->pFailureBreakRules;
	this->m_fPingSlaves = pTNccsd->fPingSlaves;
	this->m_fReportInBVTFormat = pTNccsd->fReportInBVTFormat;


	if (! this->m_fPingSlaves)
	{
		DPL(0, "WARNING: Not pinging slaves!", 0);
		// Ignoring error
		this->Log(TNLST_CONTROLLAYER_INFO, "WARNING: Not pinging slaves!", 0);
	} // end if (not pinging)


	hr = ScriptLoadTestTable(&(this->m_testtable));
	if (hr != S_OK)
	{
		DPL(0, "Loading built-in script functions failed!", 0);
		goto ERROR_EXIT;
	} // end if (loading built-in script functions failed)

	for(dwTemp = 0; dwTemp < pTNccsd->dwNumLoadTestTables; dwTemp++)
	{
		hr = pTNccsd->apfnLoadTestTable[dwTemp](&(this->m_testtable));
		if (hr != S_OK)
		{
			DPL(0, "Module's LoadTestTable callback %u failed!", 1, dwTemp);
		goto ERROR_EXIT;
		} // end if (module's callback failed)
	} // end for (each LoadTestTable callback)

	hr = this->m_testtable.CalculateChecksum(&(this->m_testChecksum));
	if (hr != S_OK)
	{
		DPL(0, "Failed calculating checksum!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't calculate checksum)


	// If not in Poke mode, read the tests to run from a file.
	if (this->m_dwMode != TNMODE_POKE)
	{
		hr = this->ReadFromFile(pTNccsd->pszTestlistFilepath,
								pTNccsd->dwReps);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't read tests from file %s!",
				1, pTNccsd->pszTestlistFilepath);
			goto ERROR_EXIT;
		} // end if (module's callback failed)


		DPL(1, "Loaded list containing %i test(s) from file %s, %u rep(s).",
			3, this->m_loadedtests.Count(), pTNccsd->pszTestlistFilepath,
			pTNccsd->dwReps);

		// Ignore error
		this->Log(TNLST_CONTROLLAYER_INFO,
				"Loaded list containing %i tes%s from file %s, set for %i repetitio%s.",
				5,
				this->m_loadedtests.Count(),
				((this->m_loadedtests.Count() != 1) ? "ts" : "t"),
				pTNccsd->pszTestlistFilepath,
				pTNccsd->dwReps,
				((pTNccsd->dwReps != 1) ? "ns" : "n"));
	} // end if (not in poke mode)


	// Initialize control communication method
	hr = this->m_pCtrlComm->Initialize();
	if (hr != S_OK)
	{
		DPL(0, "Initializing control communications method failed!", 0);
		goto ERROR_EXIT;
	} // end if (initting ctrl comm failed)


	// If we're using a TCP/IP control method, we can gather some cool NAT related
	// info during the client join sequence.   To do that we need to gather the IP
	// addresses we think we're using.  WinSock will just have been initialized above
	// so we can grab them now. 
	if ((pTNccsd->dwControlMethodID == TN_CTRLMETHOD_TCPIP_OPTIMAL) ||
		(pTNccsd->dwControlMethodID == TN_CTRLMETHOD_TCPIP_WINSOCK1))
	{
		hr = this->m_ipaddrs.LoadLocalIPs();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't load local machine IPs!", 0);
			goto ERROR_EXIT;
		} // end if (couldn't load local IPs)
	} // end if (using a TCP/IP control method)


	// If we're supposed to register with a meta-master, do that.
	if (pTNccsd->pszMetaMasterAddress != NULL)
	{
		// Ignore error, assume BUFFER_TOO_SMALL
		this->m_pCtrlComm->ConvertStringToAddress(pTNccsd->pszMetaMasterAddress,
													NULL,
													0,
													NULL,
													&(this->m_dwMetaMasterAddressSize));
		this->m_pvMetaMasterAddress = LocalAlloc(LPTR, this->m_dwMetaMasterAddressSize);
		if (this->m_pvMetaMasterAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)

		hr = this->m_pCtrlComm->ConvertStringToAddress(pTNccsd->pszMetaMasterAddress,
														NULL,
														0,
														this->m_pvMetaMasterAddress,
														&(this->m_dwMetaMasterAddressSize));
		if (hr != S_OK)
		{
			DPL(0, "Converting \"%s\" into meta-master address failed!",
				1, pTNccsd->pszMetaMasterAddress);
			goto ERROR_EXIT;
		} // end if (initting ctrl comm failed)

		// Actually send the register request.
		hr = this->RegisterWithMetaMaster();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't register with meta-master!", 0);
			goto ERROR_EXIT;
		} // end if (initting ctrl comm failed)
	} // end if (a meta master was specified)


	// BUGBUG use a better method of getting the session ID?
#ifndef _XBOX // timeGetTime not supported
	this->m_dwSessionID = timeGetTime();
#else // ! XBOX
	this->m_dwSessionID = GetTickCount();
#endif // XBOX

	if (pTNccsd->pszClosingReportDirPath != NULL)
	{
		char	szTemp[64];
		DWORD	dwSize;


		wsprintf(szTemp, "final_%010u.txt", this->m_dwSessionID);

		dwSize = strlen(pTNccsd->pszClosingReportDirPath) + strlen(szTemp) + 1;
		// Make sure it ends with a backslash
		if (! StringEndsWith(pTNccsd->pszClosingReportDirPath, "\\", TRUE))
			dwSize++;

		this->m_pszClosingReportPath = (char*) LocalAlloc(LPTR, dwSize);
		if (this->m_pszClosingReportPath == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)

		strcpy(this->m_pszClosingReportPath, pTNccsd->pszClosingReportDirPath);
		if (! StringEndsWith(pTNccsd->pszClosingReportDirPath, "\\", TRUE))
			strcat(this->m_pszClosingReportPath, "\\");
		strcat(this->m_pszClosingReportPath, szTemp);
	} // end if (there's a report dir)


	this->m_dwStartupTicks = GetTickCount();
	time(&(this->m_tStartupTime));
	this->m_fJoinersAllowed = TRUE;


	// Kick off the job thread
	hr = this->StartJobThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't start Job thread!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't start the job thread)



	// Ping the event to let the app know the session is ready.
	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto ERROR_EXIT;
	} // end if (couldn't set the user's update event)

	return (S_OK);


ERROR_EXIT:

	// Ignore error
	this->DestroyControlSession();

	return (hr);
} // CTNMaster::CreateControlSession
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DestroyControlSession()"
//==================================================================================
// CTNMaster::DestroyControlSession
//----------------------------------------------------------------------------------
//
// Description: Closes the control session and releases the ctrl comm object if
//				they existed.
//				Note that errors will probably be masked.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::DestroyControlSession(void)
{
	HRESULT			hr;
	PTNSLAVEINFO	pSlave = NULL;



	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we got passed a bad pointer)


	DPL(9, "==>", 0);


	if (this->m_fRegistered)
	{
		hr = this->UnregisterWithMetaMaster();
		if (hr != S_OK)
		{
			DPL(0, "WARNING: Unregistering with meta-master failed!  %e", 1, hr);
		} // end if (failed unregistering)

		ZeroMemory(&(this->m_id), sizeof (TNCTRLMACHINEID));
		this->m_fRegistered = FALSE;
	} // end if (we were registered with the meta-master)
	

	// Print out all reports still in the buffer now that we're closing if the user
	// wants us to do that.
	if (this->m_pszClosingReportPath != NULL)
	{
		DPL(2, "Printing all remaining reports to \"%s\".",
			1, this->m_pszClosingReportPath);

		hr = this->PrintReports((TNRH_SESSIONINFO | TNRH_MACHINELIST | TNRH_ACTIVETESTS | TNRH_REMAININGTESTS | TNRH_SKIPPEDTESTS),
								TNRBS_PLAIN,
								TNREPORT_ALL,
								this->m_pszClosingReportPath,
								TRUE,
								FALSE);
		if (hr != S_OK)
		{
			DPL(0, "WARNING: Couldn't print remaining reports to \"%s\"!  %e",
				2, this->m_pszClosingReportPath, hr);
		} // end if (failed printing reports)

		LocalFree(m_pszClosingReportPath);
		this->m_pszClosingReportPath = NULL;
	} // end if (there's a report dir path)


	// Stop pinging slaves and all that.
	hr = this->KillJobThread();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Killing the Job thread failed!  %e", 1, hr);
	} // end if (killing thread failed)

	hr = this->m_jobs.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all remaining jobs failed!  %e", 1, hr);
	} // end if (removing jobs failed)


	// Empty out the loaded tests list.
	hr = this->m_loadedtests.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all loaded tests failed!  %e", 1, hr);
	} // end if (failed removing all tests)

	// Empty out the skipped tests list.
	hr = this->m_skippedtests.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all skipped tests failed!  %e", 1, hr);
	} // end if (failed removing all tests)

	// Empty out the remaining tests list.
	hr = this->m_remainingtests.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all remaining tests failed!  %e", 1, hr);
	} // end if (failed removing all tests)

	// Empty out the reports list.
	hr = this->m_reports.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all reports failed!  %e", 1, hr);
	} // end if (failed removing all reports)

	// Empty out the vars list.
	hr = this->m_builtinvars.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all built-in vars failed!  %e", 1, hr);
	} // end if (failed removing all vars)

	// Empty out the tests with vars list.
	hr = this->m_testswithvars.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Removing all tests with vars failed!  %e", 1, hr);
	} // end if (failed removing all ests)

	if (this->m_pCtrlComm != NULL)
	{
		// Flush the send queue
		hr = this->m_pCtrlComm->FlushSendQueue();
		if (hr != S_OK)
		{
			DPL(0, "WARNING: Flushing control method send queue failed!  %e", 1, hr);
		} // end if (flushing send queue failed)
	} // end if (there's a control method)

	// Now remove all the slaves
	this->m_slaves.EnterCritSection();
	while (this->m_slaves.Count() > 0)
	{
		pSlave = (PTNSLAVEINFO) this->m_slaves.PopFirstItem();
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't pop first slave in list!", 0);
			break;
		} // end if (couldn't pop item)

		// Disconnect him (if he wasn't already).
		hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pSlave->m_commdata));
		if (hr != S_OK)
		{
			DPL(0, "WARNING: Unbinding slave's comm data failed!  %e", 1, hr);
		} // end if (undbinding data failed)

		// Get rid of its current test, if it has one.
		if (pSlave->m_pCurrentTest != NULL)
		{
			pSlave->m_pCurrentTest->m_dwRefCount--;
			if (pSlave->m_pCurrentTest->m_dwRefCount == 0)
			{
				DPL(7, "Deleting slave %x's current test %x.",
					2, pSlave, pSlave->m_pCurrentTest);
				delete (pSlave->m_pCurrentTest);
			} // end if (refcount hit 0)
			else
			{
				DPL(7, "Not deleting slave %x's current test %x, it's refcount is %u.",
					3, pSlave, pSlave->m_pCurrentTest,
					pSlave->m_pCurrentTest->m_dwRefCount);
			} // end else (refcount didn't hit 0)
			pSlave->m_pCurrentTest = NULL;
		} // end if (slave has current test)

		pSlave->m_dwRefCount--;
		if (pSlave->m_dwRefCount == 0)
		{
			DPL(7, "Deleting slave %x.", 1, pSlave);
			delete (pSlave);
			pSlave = NULL;
		} // end if (deleting slave failed)
		else
		{
			DPL(1, "Can't delete slave %x, it's refcount is %u.",
				2, pSlave, pSlave->m_dwRefCount);
		} // end if (deleting slave failed)
	} // end for (each slave)
	this->m_slaves.LeaveCritSection();

	// Now remove all the dropped slaves.
	this->m_droppedslaves.EnterCritSection();
	while (this->m_droppedslaves.Count() > 0)
	{
		pSlave = (PTNSLAVEINFO) this->m_droppedslaves.PopFirstItem();
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't pop first slave in list!", 0);
			break;
		} // end if (couldn't pop item)

		// Disconnect him (if he wasn't already).
		hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pSlave->m_commdata));
		if (hr != S_OK)
		{
			DPL(0, "WARNING: Unbinding dropped slave's comm data failed!  %e", 1, hr);
		} // end if (undbinding data failed)

		// Get rid of its current test, if it has one.
		if (pSlave->m_pCurrentTest != NULL)
		{
			pSlave->m_pCurrentTest->m_dwRefCount--;
			if (pSlave->m_pCurrentTest->m_dwRefCount == 0)
			{
				DPL(7, "Deleting slave %x's current test %x.",
					2, pSlave, pSlave->m_pCurrentTest);
				delete (pSlave->m_pCurrentTest);
			} // end if (refcount hit 0)
			else
			{
				DPL(7, "Not deleting slave %x's current test %x, it's refcount is %u.",
					3, pSlave, pSlave->m_pCurrentTest,
					pSlave->m_pCurrentTest->m_dwRefCount);
			} // end else (refcount didn't hit 0)
			pSlave->m_pCurrentTest = NULL;
		} // end if (slave has current test)

		pSlave->m_dwRefCount--;
		if (pSlave->m_dwRefCount == 0)
		{
			DPL(7, "Deleting slave %x.", 1, pSlave);
			delete (pSlave);
			pSlave = NULL;
		} // end if (deleting slave failed)
		else
		{
			DPL(1, "Can't delete slave %x, it's refcount is %u.",
				2, pSlave, pSlave->m_dwRefCount);
		} // end if (deleting slave failed)
	} // end for (each slave)
	this->m_droppedslaves.LeaveCritSection();

	if (this->m_pCtrlComm != NULL)
	{
		this->m_pCtrlComm->m_dwRefCount--;
		if (this->m_pCtrlComm->m_dwRefCount == 0)
		{
			DPL(7, "Releasing and deleting control comm object %x.",
				1, this->m_pCtrlComm);

			hr = this->m_pCtrlComm->Release();
			if (hr != S_OK)
			{
				DPL(0, "WARNING: Releasing control communication method failed!  %e", 1, hr);
			} // end if (releasing control method failed)

			delete (this->m_pCtrlComm);
		} // end if (last reference)
		else
		{
			DPL(0, "WARNING: Not releasing and deleting control comm object %x, its refcount is %u!",
				2, this->m_pCtrlComm, this->m_pCtrlComm->m_dwRefCount);
		} // end else (not last reference)

		this->m_pCtrlComm = NULL;
	} // end if (we're still validly connected to the control method)

	/*
	if (this->m_hAllSlavesCommStatusEvent != NULL)
	{
		CloseHandle(this->m_hAllSlavesCommStatusEvent);
		this->m_hAllSlavesCommStatusEvent = NULL;
	} // end if (we created a comm status event)
	*/

	// Dump any dynamic variables we may have created
	hr = this->m_builtinvars.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Removing all dynamic variables from list failed!  %e", 1, hr);
	} // end if (emptying variable list failed)


	// Reset the members, just like in the constructor

	this->m_dwMode = 0;
	ZeroMemory(&(this->m_moduleID), sizeof (TNMODULEID));
	if (this->m_pszSessionFilter != NULL)
	{
		LocalFree(this->m_pszSessionFilter);
		this->m_pszSessionFilter = NULL;
	} // end if (allocated a session user ID)

	if (this->m_pszSiblingFailuresMemDumpDirPath != NULL)
	{
		LocalFree(this->m_pszSiblingFailuresMemDumpDirPath);
		this->m_pszSiblingFailuresMemDumpDirPath = NULL;
	} // end if (allocated a dump dir path)

	if (this->m_pszSkippedTestsPath != NULL)
	{
		LocalFree(this->m_pszSkippedTestsPath);
		this->m_pszSkippedTestsPath = NULL;
	} // end if (allocated a dump dir path)

	if (this->m_hCompletionOrUpdateEvent != NULL)
	{
		CloseHandle(this->m_hCompletionOrUpdateEvent);
		this->m_hCompletionOrUpdateEvent = NULL;
	} // end if (have event)

	this->m_pFailureBreakRules = NULL;

	this->m_pfnGetStartupData = NULL;
	this->m_pfnLoadStartupData = NULL;
	this->m_pfnWriteStartupData = NULL;
	this->m_pfnHandleUserQuery = NULL;

	this->m_dwSessionID = 0;
	ZeroMemory(&this->m_testChecksum, sizeof (CHECKSUM));
	//this->m_hAllSlavesCommStatusEvent = NULL;
	this->m_dwCurrentUniqueID = 1;
	this->m_dwStartupTicks = 0;
	ZeroMemory(&(this->m_tStartupTime), sizeof (time_t));
	this->m_dwTimelimit = 0;
	this->m_dwAllowJoinersTime = 0;
	this->m_fJoinersAllowed = FALSE;
	this->m_fTestingComplete = FALSE;

	this->m_dwAutoReportHeaders = 0;
	this->m_dwAutoReportBodyStyle = 0;
	this->m_dwAutoReportFlags = 0;
	this->m_iAutoReportNumReports = -1;
	if (this->m_pszAutoReportDirpath != NULL)
	{
		LocalFree(this->m_pszAutoReportDirpath);
		this->m_pszAutoReportDirpath = NULL;
	} // end if (there's an autoreport dirpath)
	this->m_fAutoReportDeleteNonPrinted = FALSE;

	this->m_iTotalNumSlaves = 0;


	if (this->m_pvMetaMasterAddress != NULL)
	{
		LocalFree(this->m_pvMetaMasterAddress);
		this->m_pvMetaMasterAddress = NULL;
	} // end if (allocated a metamaster address)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::DestroyControlSession
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetSessionID()"
//==================================================================================
// CTNMaster::GetSessionID
//----------------------------------------------------------------------------------
//
// Description: Returns the identifier of the current session.
//
// Arguments: None.
//
// Returns: ID of current session.
//==================================================================================
DWORD CTNMaster::GetSessionID(void)
{
	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (0);
	} // end if (slave object is invalid)

	if (this->m_dwStartupTicks == 0)
	{
		DPL(0, "Session not started up, no ID yet.", 0);
		return (0);
	} // end if (we haven't fully started up yet)
	
	return (this->m_dwSessionID);
} // CTNMaster::GetSessionID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetModuleString()"
//==================================================================================
// CTNMaster::GetModuleString
//----------------------------------------------------------------------------------
//
// Description: Returns a string describing the currently loaded module.
//
// Arguments:
//	char* szString	String buffer to copy results in.  Should be able to hold 64
//					characters.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::GetModuleString(char* szString)
{
	/*
	if (! this->m_fInSession)
	{
		DPL(0, "Not in session yet, can't determine module ID!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (not in a session)
	*/
	
	wsprintf(szString, "%s v%02u.%02u.%02u.%04u", this->m_moduleID.szBriefName,
			this->m_moduleID.dwMajorVersion, this->m_moduleID.dwMinorVersion1,
			this->m_moduleID.dwMinorVersion2, this->m_moduleID.dwBuildVersion);
	
	return (S_OK);
} // CTNMaster::GetModuleString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetIndexedMachineInfo()"
//==================================================================================
// CTNMaster::GetIndexedMachineInfo
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the TNMachineInfo object for the machine
//				at the index specified if it exists, or NULL otherwise.
//
// Arguments:
//	int iMachineNum		The machine number in list to retrieve.
//
// Returns: Pointer to object or NULL if an error occurred.
//==================================================================================
PTNMACHINEINFO CTNMaster::GetIndexedMachineInfo(int iMachineNum)
{
	PTNSLAVEINFO	pSlave;


	pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(iMachineNum);

	if (pSlave == NULL)
		return (NULL);

	if (pSlave->m_commdata.fDropped)
		return (NULL);

	return ((PTNMACHINEINFO) pSlave);
} // CTNMaster::GetIndexedMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetTotalStats()"
//==================================================================================
// CTNMaster::GetTotalStats
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the total test table stats object.
//
// Arguments: None.
//
// Returns: Pointer to stats object, or NULL if an error occurred.
//==================================================================================
PTNTESTSTATS CTNMaster::GetTotalStats(void)
{
	return (&(this->m_totalstats));
} // CTNMaster::GetTotalStats
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetNumberOfCases()"
//==================================================================================
// CTNMaster::GetNumberOfCases
//----------------------------------------------------------------------------------
//
// Description: Returns the number of test cases loaded.
//
// Arguments: None.
//
// Returns: The number of test cases loaded, or -1 if an error occurred.
//==================================================================================
int CTNMaster::GetNumberOfCases(void)
{
	HRESULT		hr;
	int			iNumCases = 0;


	hr = this->m_testtable.GetNumberOfCases(&iNumCases);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get number of cases!  %e", 1, hr);
		return (-1);
	} // end if (couldn't get number of cases in the testtable)

	return (iNumCases);
} // CTNMaster::GetNumberOfCases
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetNumberOfActiveAndRemainingTests()"
//==================================================================================
// CTNMaster::GetNumberOfActiveAndRemainingTests
//----------------------------------------------------------------------------------
//
// Description: Returns the number of test instances left to run in the current
//				repetition.
//
// Arguments: None.
//
// Returns: The number of test instances left to run, or -1 if an error occurred.
//==================================================================================
int CTNMaster::GetNumberOfActiveAndRemainingTests(void)
{
	HRESULT			hr;
	int				iTotal;
	CTNTestMsList	usedtests;
	int				i;
	PTNSLAVEINFO	pSlave;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (-1);
	} // end if (invalid object)

	iTotal = this->m_remainingtests.Count();
	if (iTotal < 0)
	{
		DPL(0, "Failed getting total number of tests in list!", 0);
		return (-1);
	} // end if (couldn't get total)


	this->m_slaves.EnterCritSection();
	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't get slave %i!", 1, i);
			this->m_slaves.LeaveCritSection();
			return (-1);
		} // end if (couldn't get item)

		// If the slave has a current test
		if (pSlave->m_pCurrentTest != NULL)
		{
			// If we haven't seen this test before
			if (usedtests.GetFirstIndex(pSlave->m_pCurrentTest) < 0)
			{
				hr = usedtests.Add(pSlave->m_pCurrentTest);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add test %x to temp list!  %e",
						2, pSlave->m_pCurrentTest, hr);
					this->m_slaves.LeaveCritSection();
					return (-1);
				} // end if (couldn't get item)
			} // end if (haven't already added the test)
		} // end if (slave is currently working on a test)
	} // end for (each slave)

	// Add the number of unique tests all the slaves are currently running
	iTotal += usedtests.Count();

	this->m_slaves.LeaveCritSection();


	return (iTotal);
} // CTNMaster::GetNumberOfActiveAndRemainingTests
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetNumTestsSkipped()"
//==================================================================================
// CTNMaster::GetNumTestsSkipped
//----------------------------------------------------------------------------------
//
// Description: Returns the number of test instances that had to be skipped.
//
// Arguments: None.
//
// Returns: The number of test instances skipped, or -1 if an error occurred.
//==================================================================================
int CTNMaster::GetNumTestsSkipped(void)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (-1);
	} // end if (invalid object)

	return (this->m_skippedtests.Count());
} // CTNMaster::GetNumTestsSkipped
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::IsSessionClosed()"
//==================================================================================
// CTNMaster::IsSessionClosed
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if no more joiners are allowed, FALSE otherwise.
//
// Arguments: None.
//
// Returns: TRUE if no joiners allowed, FALSE otherwise.
//==================================================================================
BOOL CTNMaster::IsSessionClosed(void)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (FALSE);
	} // end if (invalid object)

	return ((this->m_fJoinersAllowed) ? FALSE : TRUE);
} // CTNMaster::IsSessionClosed
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::IsTestingComplete()"
//==================================================================================
// CTNMaster::IsTestingComplete
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if all tests intended to be run have actually been.
//
// Arguments: None.
//
// Returns: TRUE if all tests are run, FALSE otherwise.
//==================================================================================
BOOL CTNMaster::IsTestingComplete(void)
{
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (FALSE);
	} // end if (invalid object)

	return (this->m_fTestingComplete);
} // CTNMaster::IsTestingComplete
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetCaseIndex()"
//==================================================================================
// CTNMaster::GetCaseIndex
//----------------------------------------------------------------------------------
//
// Description: Returns pointers to the ID, name, and stats for the case at the
//				given index.  If NULL is passed in for an argument, then that
//				item is not returned.
//
// Arguments:
//	int iIndex				Zero based index of case to retrieve.
//	char** ppszID,			Pointer to place to store pointer to the case ID string,
//							or NULL if not wanted.
//	char** ppszName			Pointer to place to store pointer to the case name
//							string, or NULL if not wanted.
//	PTNTESTSTATS* ppStats	Pointer to place to store pointer to the case stats, or
//							NULL if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::GetCaseIndex(int iIndex, char** ppszID, char** ppszName,
								PTNTESTSTATS* ppStats)
{
	HRESULT				hr;
	int					iNumItemsLeft = iIndex;
	PTNTESTTABLECASE	pItem = NULL;


	hr = this->m_testtable.GetCaseIndex(&iNumItemsLeft, &pItem);
	if (hr != S_OK)
	{
		DPL(0, "Failed getting case index %i!", 1, iIndex);
		return (hr);
	} // end if (couldn't get that item)

	if (pItem == NULL)
	{
		DPL(0, "Couldn't find case index %i!", 1, iIndex);
		return (ERROR_INVALID_PARAMETER);
	} // end if (couldn't get that item)


	// Otherwise, we found it so return what we need to.

	if (ppszID != NULL)
		(*ppszID) = pItem->m_pszID;

	if (ppszName != NULL)
		(*ppszName) = pItem->m_pszName;

	if (ppStats != NULL)
		(*ppStats) = &(pItem->m_stats);

	return (S_OK);
} // CTNMaster::GetCaseIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SearchStringForDynamicVars()"
//==================================================================================
// CTNMaster::SearchStringForDynamicVars
//----------------------------------------------------------------------------------
//
// Description: Searches the given string for the first dynamic variable (string
//				surrounded by @ signs).  ppszNameFound is set to point to name (past
//				the first @ sign) and pdwNameLength is set to the length of the name
//				(not including the trailing @ sign, and no NULL termination
//				obviously).
//				The two output parameters will be set to NULL and zero if none was
//				found.
//
// Arguments:
//	char* szString				String to search.
//	char** ppszNameFound		Pointer to set to start of variable name found.
//	DWORD* pdwNameLength		Pointer to set to length of name string.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::SearchStringForDynamicVars(char* szString, char** ppszNameFound,
											DWORD* pdwNameLength)
{
	int		iStartPos;
	int		iEndPos;


	if ((szString == NULL) || (ppszNameFound == NULL) || (pdwNameLength == NULL))
	{
		DPL(0, "Must pass non-NULL parameters!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed a bad parameter)


	(*ppszNameFound) = NULL;
	(*pdwNameLength) = 0;


	//BUGBUG we don't handle escaping

	iStartPos = StringContainsChar(szString, '@', TRUE, 0);
	if (iStartPos < 0)
	{
		return (S_OK);
	} // end if (didn't find a start token)

	iStartPos++; // skip token

	iEndPos = StringContainsChar(szString, '@', TRUE, iStartPos);
	if (iEndPos < 0)
	{
		return (S_OK);
	} // end if (didn't find an end token)

	(*ppszNameFound) = szString + iStartPos;
	(*pdwNameLength) = iEndPos - iStartPos;

	return (S_OK);
} // CTNMaster::SearchStringForDynamicVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::ExpandBuiltinStringVars()"
//==================================================================================
// CTNMaster::ExpandBuiltinStringVars
//----------------------------------------------------------------------------------
//
// Description: Expands all built-in dynamic variables of type "STRING" found in the
//				given string.  If the buffer pointer is NULL, the size need to hold
//				the new string is placed in pdwBufferSize, and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* szString			String to expand.
//	char* pszBuffer			Pointer to buffer to place results, or NULL to retrieve
//							size required.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::ExpandBuiltinStringVars(char* szString, char* pszBuffer,
											DWORD* pdwBufferSize)
{
	PTNOUTPUTVARSLIST	pList;


	if ((szString == NULL) || (pdwBufferSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters except for buffer!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed a bad parameter)

	pList = &(this->m_builtinvars);
	return (this->InternalExpandStringVars(szString, pszBuffer, pdwBufferSize,
											&pList, 1));
} // CTNMaster::ExpandBuiltinStringVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::ExpandSlaveStringVars()"
//==================================================================================
// CTNMaster::ExpandSlaveStringVars
//----------------------------------------------------------------------------------
//
// Description: Expands all dynamic variables set by the slave and ones built-in of
//				type "STRING" found in the given string.  If the buffer pointer is
//				NULL, the size need to hold the new string is placed in
//				pdwBufferSize, and ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* szString			String to expand.
//	PTNMACHINEINFO pSlave	Pointer to slave to check.
//	char* pszBuffer			Pointer to buffer to place results, or NULL to retrieve
//							size required.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::ExpandSlaveStringVars(char* szString, PTNMACHINEINFO pSlave,
										char* pszBuffer, DWORD* pdwBufferSize)
{
	HRESULT				hr;
	PTNTESTINSTANCEM	pTest;
	int					iTesterNum;
	PTNOUTPUTVARSLIST*	papLists = NULL;
	DWORD				dwNumItems = 0;
	int					i;


	this->m_testswithvars.EnterCritSection();

	if ((szString == NULL) || (pSlave == NULL) || (pdwBufferSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters except for buffer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed a bad parameter)

	// Allocate enough room for all the tests in the list, plus 1 for the
	// built in vars list.
	papLists = (PTNOUTPUTVARSLIST*) LocalAlloc(LPTR, ((this->m_testswithvars.Count() + 1) * sizeof (PTNOUTPUTVARSLIST)));
	if (papLists == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Loop through all the tests with output variables.
	for(i = 0; i < this->m_testswithvars.Count(); i++)
	{
		pTest = (PTNTESTINSTANCEM) this->m_testswithvars.GetItem(i);
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get test with data %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// If the specified slave was assigned to this test and he generated output
		// variables, add his list into the array.
		iTesterNum = pTest->GetSlavesTesterNum((PTNSLAVEINFO) pSlave);

		if ((iTesterNum >= 0) &&
			(pTest->m_paTesterSlots[iTesterNum].pVars != NULL))
		{
			papLists[dwNumItems++] = pTest->m_paTesterSlots[iTesterNum].pVars;
		} // end if (this tester was assigned to the test)
	} // end for (each test with data)

	papLists[dwNumItems++] = &(this->m_builtinvars);

	hr = this->InternalExpandStringVars(szString, pszBuffer, pdwBufferSize,
										papLists, dwNumItems);

DONE:

	this->m_testswithvars.LeaveCritSection();

	SAFE_LOCALFREE(papLists);

	return (hr);
} // CTNMaster::ExpandSlaveStringVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::ExpandTestersStringVars()"
//==================================================================================
// CTNMaster::ExpandTestersStringVars
//----------------------------------------------------------------------------------
//
// Description: Expands all dynamic variables set by any of the currently assigned
//				testers or one of the ones built-in of type "STRING" found in the
//				given string.  If the buffer pointer is NULL, the size need to hold
//				the new string is placed in pdwBufferSize, and
//				ERROR_BUFFER_TOO_SMALL is returned.
//				IMPORTANT: If there are no testers assigned to a particular tester
//				number, that slot is skipped.  This means that you cannot assume you
//				will get the variable you expect if you designated the test as
//				TNTCO_ALLOWHEADSTART. 
//
// Arguments:
//	char* szString			String to expand.
//	PTNTESTINSTANCEM pTest	Pointer to test with slaves to check.
//	char* pszBuffer			Pointer to buffer to place results, or NULL to retrieve
//							size required.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::ExpandTestersStringVars(char* szString, PTNTESTINSTANCEM pTest,
										char* pszBuffer, DWORD* pdwBufferSize)
{
	HRESULT				hr;
	PTNOUTPUTVARSLIST*	papLists = NULL;
	int					i;
	DWORD				dwNumItems = 0;



	if ((szString == NULL) || (pTest == NULL) || (pdwBufferSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters except for buffer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed a bad parameter)


	// Allocate enough room for all the machines for the test, plus 1 for the
	// built in vars list.
	papLists = (PTNOUTPUTVARSLIST*) LocalAlloc(LPTR, ((pTest->m_iNumMachines + 1) * sizeof (PTNOUTPUTVARSLIST)));
	if (papLists == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	for(i = 0; i < pTest->m_iNumMachines; i++);
	{
		// If there variables in this slot, add the vars list.
		if (pTest->m_paTesterSlots[i].pVars != NULL)
		{
			papLists[dwNumItems++] = pTest->m_paTesterSlots[i].pVars;
		} // end if (there's actually a slave here)
	} // end for (each variable)

	papLists[dwNumItems++] = &(this->m_builtinvars);

	hr = this->InternalExpandStringVars(szString, pszBuffer, pdwBufferSize,
										papLists, dwNumItems);

DONE:

	SAFE_LOCALFREE(papLists);

	return (hr);
} // CTNMaster::ExpandTestersStringVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::IsBuiltinVarOfType()"
//==================================================================================
// CTNMaster::IsBuiltinVarOfType
//----------------------------------------------------------------------------------
//
// Description: Checks if the given string is a builtin dynamic variable of the
//				given type.  If it is, then a pointer to its value and its size are
//				returned.  Otherwise they are set to NULL and 0.
//
// Arguments:
//	char* szString				String to check.
//	char* szType				Type of variable expected.
//	PVOID* ppvValueLocation		Pointer to set to value.
//	DWORD* pdwValueSize			Place to store size of the value.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::IsBuiltinVarOfType(char* szString, char* szType,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize)
{
	PTNOUTPUTVARSLIST	pList;


	if ((szString == NULL) || (szType == NULL) || (ppvValueLocation == NULL) ||
		(pdwValueSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed a bad parameter)

	pList = &(this->m_builtinvars);
	return (this->InternalIsVarOfType(szString, szType, ppvValueLocation,
									pdwValueSize, &pList, 1));
} // CTNMaster::IsBuiltinVarOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::IsSlavesVarOfType()"
//==================================================================================
// CTNMaster::IsSlavesVarOfType
//----------------------------------------------------------------------------------
//
// Description: Checks if the given string is a dynamic variable set by the slave
//				or one of the ones built-in.  It must match the type given.  If one
//				is found, then a pointer to its value and its size are returned.
//				Otherwise they are set to NULL and 0.
//
// Arguments:
//	char* szString				String to check.
//	char* szType				Type of variable expected.
//	PTNMACHINEINFO pSlave		Pointer to slave to use.
//	PVOID* ppvValueLocation		Pointer to set to value.
//	DWORD* pdwValueSize			Place to store size of the value.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::IsSlavesVarOfType(char* szString, char* szType,
									PTNMACHINEINFO pSlave,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize)
{
	HRESULT				hr;
	PTNTESTINSTANCEM	pTest;
	int					iTesterNum;
	PTNOUTPUTVARSLIST*	papLists = NULL;
	DWORD				dwNumItems = 0;
	int					i;


	this->m_testswithvars.EnterCritSection();

	if ((szString == NULL) || (szType == NULL) || (pSlave == NULL) ||
		(ppvValueLocation == NULL) || (pdwValueSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed a bad parameter)

	// Allocate enough room for all the tests in the list, plus 1 for the
	// built in vars list.
	papLists = (PTNOUTPUTVARSLIST*) LocalAlloc(LPTR, ((this->m_testswithvars.Count() + 1) * sizeof (PTNOUTPUTVARSLIST)));
	if (papLists == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Loop through all the tests with output variables.
	for(i = 0; i < this->m_testswithvars.Count(); i++)
	{
		pTest = (PTNTESTINSTANCEM) this->m_testswithvars.GetItem(i);
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get test with data %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// If the specified slave was assigned to this test and he generated output
		// variables, add his list into the array.
		iTesterNum = pTest->GetSlavesTesterNum((PTNSLAVEINFO) pSlave);

		if ((iTesterNum >= 0) &&
			(pTest->m_paTesterSlots[iTesterNum].pVars != NULL))
		{
			papLists[dwNumItems++] = pTest->m_paTesterSlots[iTesterNum].pVars;
		} // end if (this tester was assigned to the test)
	} // end for (each test with data)

	papLists[dwNumItems++] = &(this->m_builtinvars);

	hr = this->InternalIsVarOfType(szString, szType, ppvValueLocation,
									pdwValueSize, papLists, dwNumItems);

DONE:

	this->m_testswithvars.LeaveCritSection();

	SAFE_LOCALFREE(papLists);

	return (hr);
} // CTNMaster::IsSlavesVarOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::IsTestersVarOfType()"
//==================================================================================
// CTNMaster::IsTestersVarOfType
//----------------------------------------------------------------------------------
//
// Description: Checks if the given string is a dynamic variable set by any of the
//				currently assigned testers or one of the ones built-in.  It must
//				match the type given.  If it is, then a pointer to its value and its
//				size are returned.  Otherwise they are set to NULL and 0.
//				IMPORTANT: If there are no testers assigned to a particular tester
//				number, that slot is skipped.  This means that you cannot assume you
//				will get the variable you expect if you designated the test as
//				TNTCO_ALLOWHEADSTART. 
//
// Arguments:
//	char* szString				String to check.
//	char* szType				Type of variable required.
//	PTNTESTINSTANCEM pTest		Pointer to test with slaves to check.
//	PVOID* ppvValueLocation		Pointer to set to value.
//	DWORD* pdwValueSize			Place to store size of the value.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::IsTestersVarOfType(char* szString, char* szType,
									PTNTESTINSTANCEM pTest,
									LPVOID* ppvValueLocation,
									DWORD* pdwValueSize)
{
	HRESULT				hr;
	PTNOUTPUTVARSLIST*	papLists = NULL;
	int					i;
	DWORD				dwNumItems = 0;


	if ((szString == NULL) || (szType == NULL) || (pTest == NULL) ||
		(ppvValueLocation == NULL) || (pdwValueSize == NULL))
	{
		DPL(0, "Must pass non-NULL parameters!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed a bad parameter)

	// Allocate enough room for all the machines for the test, plus 1 for the
	// built in vars list.
	papLists = (PTNOUTPUTVARSLIST*) LocalAlloc(LPTR, ((pTest->m_iNumMachines + 1) * sizeof (PTNOUTPUTVARSLIST)));
	if (papLists == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	for(i = 0; i < pTest->m_iNumMachines; i++);
	{
		// If there's a slave assigned to this slot, add his vars list
		if (pTest->m_paTesterSlots[i].pVars != NULL)
		{
			papLists[dwNumItems++] = pTest->m_paTesterSlots[i].pVars;
			dwNumItems++;
		} // end if (there's actually a slave here)
	} // end for (each variable)

	papLists[dwNumItems++] = &(this->m_builtinvars);

	hr = this->InternalIsVarOfType(szString, szType, ppvValueLocation,
									pdwValueSize, papLists, dwNumItems);

DONE:

	SAFE_LOCALFREE(papLists);

	return (hr);
} // CTNMaster::IsTestersVarOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::PrintTestTableToFile()"
//==================================================================================
// CTNMaster::PrintTestTableToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the table of all tests loaded to the specified file path.
//
// Arguments:
//	char* szFilepath	Filepath to print to.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::PrintTestTableToFile(char* szFilepath)
{
	HRESULT		hr;
	HANDLE		hFile = INVALID_HANDLE_VALUE;


	hr = FileCreateAndOpenFile(szFilepath, FALSE, FALSE, FALSE, &hFile);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't open file \"%s\"!", 1, szFilepath);
		goto DONE;
	} // end if (couldn't open file)

	this->Log(TNLST_CONTROLLAYER_INFO, "Printing test table to file %s",
			1, szFilepath);


	// Ignoring error
	FileWriteLine(hFile, "ID\tName");
	FileWriteLine(hFile, "--\t----");

	hr = this->m_testtable.PrintToFile(hFile, TNTTPO_BUILTIN | TNTTPO_GROUPS);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't print test table to file \"%s\"!", 1, szFilepath);
	} // end if (couldn't open file)


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (opened file)

	return (hr);
} // CTNMaster::PrintTestTableToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::PrintResultsToFile()"
//==================================================================================
// CTNMaster::PrintResultsToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the current success/failure results for all tests loaded to
//				the specified file.
//
// Arguments:
//	char* szFilepath	Filepath to print to.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::PrintResultsToFile(char* szFilepath)
{
	HRESULT		hr;
	HANDLE		hFile = INVALID_HANDLE_VALUE;


	hr = FileCreateAndOpenFile(szFilepath, FALSE, FALSE, FALSE, &hFile);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't open \"%s\" for writing!", 1, szFilepath);
		goto DONE;
	} // end if (couldn't open file)

	this->Log(TNLST_CONTROLLAYER_INFO, "Printing test results to file %s",
			1, szFilepath);

	// Ignoring error
	FileWriteLine(hFile, " ID \t Name \t Successes \t Failures \t Total Run \t Warnings ");
	FileWriteLine(hFile, "----\t------\t-----------\t----------\t-----------\t----------");

	hr = this->m_testtable.PrintToFile(hFile, TNTTPO_SUCCESSES | TNTTPO_FAILURES | TNTTPO_TOTAL | TNTTPO_WARNINGS);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't print test results to file \"%s\"!", 1, szFilepath);
	} // end if (couldn't open file)


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (opened file)

	return (hr);
} // CTNMaster::PrintResultsToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::PrintReports()"
//==================================================================================
// CTNMaster::PrintReports
//----------------------------------------------------------------------------------
//
// Description: Prints the contents of the report log to the file specified (or
//				depending on the style, to individual files in the directory
//				specified).
//
// Arguments:
//	DWORD dwHeaders			Which headers should be printed.
//	DWORD dwBodyStyle		How the report log body should be printed.
//	DWORD dwFlags			What in the report log should be printed.
//	char* szFilepath		Where the report log should be printed.
//	BOOL fAppend			Should we add to (instead of overwrite) the file if it exists?
//	BOOL fDeletePrinted		Should the reports that were printed be removed from the
//							report log?
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::PrintReports(DWORD dwHeaders, DWORD dwBodyStyle, DWORD dwFlags,
								char* szFilepath, BOOL fAppend, BOOL fDeletePrinted)
{
	HRESULT				hr = S_OK;
	int					i;
	PTNREPORT			pReport = NULL;
	PTNSLAVEINFO		pSlave = NULL;
	PTNTESTINSTANCEM	pTest = NULL;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	CTNSlavesList		machinelist;
	CTNTestMsList		currenttotaltestlist;
	CTNTestMsList		testlist;


	DPL(5, "Printing reports to \"%s\".", 1, szFilepath);


	hr = FileCreateAndOpenFile(szFilepath, FALSE, fAppend, FALSE, &hFile);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't create the report file %s!", 1, szFilepath);
		goto DONE;
	} // end if (couldn't create file)

	this->Log(TNLST_CONTROLLAYER_INFO, "Printing reports to file \"%s\".",
			1, szFilepath);


	hr = this->DoBeginReport(hFile, dwHeaders);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't print beginning report items!", 0);
		goto DONE;
	} // end if (failed to print beginning report stuff)


	// Now print the reports requested.  Start by getting the first machine
	// or test if we're organizing them those ways

	this->m_slaves.EnterCritSection();
	this->m_reports.EnterCritSection();


	// Just for coolness, print a comment with how many reports are being printed
	// and how many there are in the log total.

	i = this->CountMatchingReports(dwFlags, NULL, NULL);

	DPL(1, "Printing %i reports out of %i total.",
		2, i, this->m_reports.Count());

	// Skip this when using the BVT format, though.
	if (! this->m_fReportInBVTFormat)
	{
		//FileWriteLine(hFile, ""); // double space
		FileSprintfWriteLine(hFile, "// Printing %i reports out of %i total.",
							2, i, this->m_reports.Count());
		FileWriteLine(hFile, ""); // double space
	} // end if (not BVT format)


	pReport = NULL;

	do
	{
		pReport = this->m_reports.GetNextMatchingReport(pReport, dwFlags,
														&machinelist, &testlist,
														fDeletePrinted);
		if (pReport == NULL)
		{
			// We're done.
			break;
		} // end if (no more matching reports)

		// BUGBUG always printing description for now

		if (dwBodyStyle == TNRBS_PLAIN_VERBOSE)
		{
			hr = pReport->PrintToFile(hFile, TRUE, this->m_fReportInBVTFormat);
		} // end if (we're printing verbosely)
		else
		{
			hr = pReport->PrintToFile(hFile, FALSE, this->m_fReportInBVTFormat);
		} // end else (we're not printing verbosely)

		if (hr != S_OK)
		{
			DPL(0, "Failed to print report to file!", 0);
			break;
		} // end if (failed to print report)

		// Double space
		FileWriteLine(hFile, "");
		FileWriteLine(hFile, "");
	} // end do
	while (TRUE); // while we found a matching item

	this->m_reports.LeaveCritSection();
	this->m_slaves.LeaveCritSection();


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (opened file)

	return (hr);
} // CTNMaster::PrintReports
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SetMaxNumberReports()"
//==================================================================================
// CTNMaster::SetMaxNumberReports
//----------------------------------------------------------------------------------
//
// Description: Sets the maximum number of reports the report log will store before
//				throwing out old reports to make room for new ones.
//
// Arguments:
//	int iNumReports		How many reports to keep.
//
// Returns: None.
//==================================================================================
void CTNMaster::SetMaxNumberReports(int iNumReports)
{
	this->m_reports.SetMaxNumberReports(iNumReports);
} // CTNMaster::SetMaxNumberReports
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::AutoPrintReports()"
//==================================================================================
// CTNMaster::AutoPrintReports
//----------------------------------------------------------------------------------
//
// Description: This enables automatic printing of the report log when it reaches a
//				certain size.  Pass -1 for iNumReports to disable.
//				Note that all printed items are automatically removed from the log,
//				and if fDeleteNonPrinted is TRUE, everything is removed from the
//				report log.
//
// Arguments:
//	DWORD dwHeaders			Which headers should be printed.
//	DWORD dwBodyStyle		How the report log body should be counted and printed.
//	DWORD dwFlags			What in the report log should be counted and printed.
//	int iNumReports			How often should the log be printed (when the log is
//							this big, the report log will be auto printed).  If
//							this is -1, auto-printing is disabled.
//	char* szDirpath			Directory where the report logs should be printed.
//	BOOL fDeleteNonPrinted	Should the reports that weren't printed also be removed
//							from the report log?
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::AutoPrintReports(DWORD dwHeaders, DWORD dwBodyStyle, DWORD dwFlags,
									int iNumReports, char* szDirpath, BOOL fDeleteNonPrinted)
{
	HRESULT		hr = S_OK;
	DWORD		dwSize;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)


	DPL(7, "Setting autoprint reports options (flags = %x, num reports = %i, path = \"%s\").",
		3, dwFlags, iNumReports, szDirpath);

	// Take lock
	this->m_reports.EnterCritSection();

	if (iNumReports < 0)
		goto DISABLE;


	this->m_dwAutoReportHeaders = dwHeaders;
	this->m_dwAutoReportBodyStyle = dwBodyStyle;
	this->m_dwAutoReportFlags = dwFlags;
	this->m_iAutoReportNumReports = iNumReports;

	if (this->m_pszAutoReportDirpath != NULL)
	{
		LocalFree(this->m_pszAutoReportDirpath);
		this->m_pszAutoReportDirpath = NULL;
	} // end if (there was a dirpath)

	// Make sure the path ends in a backslash.
	dwSize = strlen(szDirpath) + 1;
	if (! StringEndsWith(szDirpath, "\\", TRUE))
		dwSize++;

	this->m_pszAutoReportDirpath = (char*) LocalAlloc(LPTR, dwSize);
	if (this->m_pszAutoReportDirpath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DISABLE;
	} // end if (couldn't allocate memory)

	strcpy(this->m_pszAutoReportDirpath, szDirpath);
	if (! StringEndsWith(szDirpath, "\\", TRUE))
		strcat(this->m_pszAutoReportDirpath, "\\");

	this->m_fAutoReportDeleteNonPrinted = fDeleteNonPrinted;

	// Drop lock
	this->m_reports.LeaveCritSection();


	return (S_OK);


DISABLE:

	this->m_dwAutoReportHeaders = 0;
	this->m_dwAutoReportBodyStyle = 0;
	this->m_dwAutoReportFlags = 0;
	this->m_iAutoReportNumReports = -1;

	if (this->m_pszAutoReportDirpath != NULL)
	{
		LocalFree(this->m_pszAutoReportDirpath);
		this->m_pszAutoReportDirpath = NULL;
	} // end if (there was a dirpath)

	this->m_fAutoReportDeleteNonPrinted = FALSE;

	// Drop lock
	this->m_reports.LeaveCritSection();

	return (hr);
} // CTNMaster::AutoPrintReports
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::AreOnSameMachine()"
//==================================================================================
// CTNMaster::AreOnSameMachine
//----------------------------------------------------------------------------------
//
// Description: Checks whether the two given slaves are located on the same machine
//				and returns TRUE if so, FALSE if not.
//
// Arguments:
//	PTNMACHINEINFO pFirstSlave		Pointer to first machine to check.
//	PTNMACHINEINFO pSecondSlave		Pointer to other machine to check.
//
// Returns: TRUE if slaves are on same machine, FALSE otherwise.
//==================================================================================
BOOL CTNMaster::AreOnSameMachine(PTNMACHINEINFO pFirstSlave,
								PTNMACHINEINFO pSecondSlave)
{
	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (FALSE);
	} // end if (got bad pointer)

	if ((pFirstSlave == NULL) || (pSecondSlave == NULL))
	{
		DPL(0, "One or both of the two machine pointers (%x, %x) machine is NULL!",
			2, pFirstSlave, pSecondSlave);
		return (FALSE);
	} // end if (source or target are NULL)


	// If their multi-instance keys are different, they can't be on the same machine.
	if (((PTNSLAVEINFO) pFirstSlave)->m_dwMultiInstanceKey != ((PTNSLAVEINFO) pSecondSlave)->m_dwMultiInstanceKey)
	{
		return (FALSE);
	} // end if (keys are different)


	return (TRUE);
} // CTNMaster::AreOnSameMachine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CanReachViaIP()"
//==================================================================================
// CTNMaster::CanReachViaIP
//----------------------------------------------------------------------------------
//
// Description: Checks whether the given connector can reach the given listener via
//				the TCP/IP methods specified.  If so pfReachable is set to TRUE,
//				otherwise it is set to FALSE.
//				This should only be called from within a module's CanRun callback
//				because the slave list lock is assumed to be held with only one
//				level of recursion.
//
// Arguments:
//	PTNMACHINEINFO pConnector	Pointer to machine doing the connecting.
//	PTNMACHINEINFO pListener	Pointer to machine whose address is to be retrieved.
//	WORD wPort					Port to attempt the communication on.
//	DWORD dwMethodFlags			What type of communication is required.  Any
//								combination of TNCR_IP_xxx flags.
//	BOOL* pfReachable			Pointer to boolean to fill with TRUE if the
//								specified communication is possible, FALSE if not.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CanReachViaIP(PTNMACHINEINFO pConnector,
								PTNMACHINEINFO pListener,
								WORD wPort,
								DWORD dwMethodFlags,
								BOOL* pfReachable)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSource = (PTNSLAVEINFO) pConnector;
	PTNSLAVEINFO			pTarget = (PTNSLAVEINFO) pListener;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	int						i;
	DWORD					dwRemainingFlags = dwMethodFlags;
	DWORD					dwMethod = 0;
	PTNREACHCHECK			pReachCheck = NULL;
	PVOID					pvAddresses = NULL;
	DWORD					dwAddressesSize = 0;
	CTNIPAddressesList		acceptips;
	PTNIPADDRESS			pAddress = NULL;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)

	if ((pSource == NULL) || (pTarget == NULL))
	{
		DPL(0, "Either the source (%x) or the target (%x) machine is NULL, both must exist!",
			2, pSource, pTarget);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source or target are NULL)

	if (pSource == pTarget)
	{
		DPL(0, "The source and the target machines specified are the same (%x)!",
			1, pSource);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source and target are same)

	if (wPort == 0)
	{
		DPL(0, "Must specify a valid port to use!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (there's no port)

	if (dwMethodFlags & (~(TNCR_IP_UDPBROADCAST | TNCR_IP_UDP | TNCR_IP_TCP)))
	{
		DPL(0, "Invalid flags specified (%x)!", 1, dwMethodFlags);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (invalid flags specified)

	if (pfReachable == NULL)
	{
		DPL(0, "Must specify a pointer to a boolean to store result in!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no boolean specified)

	// Start by assuming it can't be reached.
	(*pfReachable) = FALSE;


	// If the slaves are actually 2 instances on the same machine, it's always
	// going to fail (both can't have the same port open).
	if (this->AreOnSameMachine(pSource, pTarget))
	{
		DPL(1, "Source and target slaves (IDs %u and %u) are on same machine (%s).",
			3, pSource->m_id.dwTime, pTarget->m_id.dwTime,
			pSource->m_szComputerName);
		goto DONE;
	} // end if (on same machine)


	// Look if we've already run this check before.

	pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pTarget->m_id));
	if (pReachCheckTarget != NULL)
	{
		for(i = 0; i < pReachCheckTarget->m_checks.Count(); i++)
		{
			pReachCheck = (PTNREACHCHECK) pReachCheckTarget->m_checks.GetItem(i);
			if (pReachCheck == NULL)
			{
				DPL(0, "Couldn't get reach check %i!", i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If we're still looking for a UDP broadcast check and this is one,
			// we've got a result.
			if ((dwRemainingFlags & TNCR_IP_UDPBROADCAST) &&
				(pReachCheck->m_dwMethod == TNRCM_UDPBROADCAST) &&
				(*((WORD*) pReachCheck->m_pvMethodData) == wPort))
			{
				// We've found a UDP broadcast check, we can stop looking for it.
				dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_UDPBROADCAST);

				// If the check failed, we can stop searching.
				if (! pReachCheck->m_fResult)
				{
					DPL(3, "%s couldn't reach %s via UDP broadcast on port %u earlier.",
						3, pSource->m_szComputerName,
						pTarget->m_szComputerName, wPort);
					goto DONE;
				} // end if (check failed)
			} // end if (found UDP broadcast)
			else if ((dwRemainingFlags & TNCR_IP_UDP) &&
					(pReachCheck->m_dwMethod == TNRCM_UDP) &&
					(*((WORD*) pReachCheck->m_pvMethodData) == wPort))
			{
				// We've found a UDP check, we can stop looking for it.
				dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_UDP);

				// If the check failed, we can stop searching.
				if (! pReachCheck->m_fResult)
				{
					DPL(3, "%s couldn't reach %s via directed UDP on port %u earlier.",
						3, pSource->m_szComputerName,
						pTarget->m_szComputerName, wPort);
					goto DONE;
				} // end if (check failed)
			} // end if (found UDP)
			else if ((dwRemainingFlags & TNCR_IP_TCP) &&
					(pReachCheck->m_dwMethod == TNRCM_TCP) &&
					(*((WORD*) pReachCheck->m_pvMethodData) == wPort))
			{
				// We've found a TCP check, we can stop looking for it.
				dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_TCP);

				// If the check failed, we can stop searching.
				if (! pReachCheck->m_fResult)
				{
					DPL(3, "%s couldn't reach %s via directed TCP on port %u earlier.",
						3, pSource->m_szComputerName,
						pTarget->m_szComputerName, wPort);
					goto DONE;
				} // end if (check failed)
			} // end if (found TCP)


			// If we found all the checks we need, and they were all successful,
			// we're done here.
			if (dwRemainingFlags == 0)
			{
				DPL(3, "%s successfully reached %s with the method(s) specified (flags = %x) earlier.",
					3, pSource->m_szComputerName, pTarget->m_szComputerName,
					dwMethodFlags);

				(*pfReachable) = TRUE;

				goto DONE;
			} // end if (found all checks we need)
		} // end for (each check)
	} // end if (source had tried to reach target before in some way)


	// If we're here, it means that either the source has never tried to reach the
	// target, or at least not in all of the IP ways requested.  We have to perform
	// the checks now.

	// Build a list of possible IPs for the connector to try.  Start with all the IP
	// addresses associated with the accepting slave.
	pTarget->m_ipaddrs.EnterCritSection();

	DPL(8, "Target %s has %i IP addresses.",
		2, pTarget->m_szComputerName, pTarget->m_ipaddrs.Count());

	for (i = 0; i < pTarget->m_ipaddrs.Count(); i++)
	{
		pAddress = (PTNIPADDRESS) pTarget->m_ipaddrs.GetItem(i);
		if (pAddress == NULL)
		{
			pTarget->m_ipaddrs.LeaveCritSection();
			DPL(0, "Couldn't get our IP item %i!", 1, i);
			goto DONE;
		} // end if (couldn't get item)

		// Add it (actually an alias to it) to the list we're building.
		hr = acceptips.Add(pAddress);
		if (hr != S_OK)
		{
			pTarget->m_ipaddrs.LeaveCritSection();
			DPL(0, "Couldn't add alias to acceptor's IP address to temporary list!", 0);
			goto DONE;
		} // end if (couldn't add alias)
	} // end for (each IP in the list)
	pTarget->m_ipaddrs.LeaveCritSection();


	// If we (the master) are considered behind a NAT by some slaves, it's
	// possible this particular acceptor slave is behind the same NAT.  Have
	// the connector try any of those IPs, too.
	// BUGBUG it would be best to only pass in the one NAT IP if this particular
	// connector sees one.
	this->m_ipaddrs.EnterCritSection();
	for (i = 0; i < this->m_ipaddrs.Count(); i++)
	{
		pAddress = (PTNIPADDRESS) this->m_ipaddrs.GetItem(i);
		if (pAddress == NULL)
		{
			this->m_ipaddrs.LeaveCritSection();
			DPL(0, "Couldn't get our IP item %i!", 1, i);
			goto DONE;
		} // end if (couldn't get item)

		// If it's not a NAT address, skip it
		if (pAddress->m_dwType != TNIPADDR_NAT)
			continue;

		// Add it (actually an alias to it) to the list we're building.
		hr = acceptips.Add(pAddress);
		if (hr != S_OK)
		{
			this->m_ipaddrs.LeaveCritSection();
			DPL(0, "Couldn't add alias to one of our NAT IP address to temporary list!", 0);
			goto DONE;
		} // end if (couldn't add alias)
	} // end for (each IP in the list)
	this->m_ipaddrs.LeaveCritSection();



	//Ignore error, assume BUFFER_TOO_SMALL
	acceptips.PackIntoBuffer(NULL, &dwAddressesSize);

	pvAddresses = LocalAlloc(LPTR, dwAddressesSize);
	if (pvAddresses == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	hr = acceptips.PackIntoBuffer(pvAddresses, &dwAddressesSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack accept IP addresses into a buffer!", 0);
		goto DONE;
	} // end if (couldn't pack into buffer)


	DPL(5, "Packed %i IP addresses for %s to try reaching %s with into %u byte buffer (at %x).",
		5, acceptips.Count(), pSource->m_szComputerName,
		pTarget->m_szComputerName, dwAddressesSize, pvAddresses);


	do
	{
		if (dwRemainingFlags & TNCR_IP_UDPBROADCAST)
		{
			// Turn off the UDP broadcast flag, since we're doing it now.
			dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_UDPBROADCAST);

			dwMethod = TNRCM_UDPBROADCAST;

			DPL(1, "Having %s try to reach %s via UDP broadcast on port %u.",
				3, pSource->m_szComputerName, pTarget->m_szComputerName, wPort);
		} // end if (need to do UDP broadcast still)
		else if (dwRemainingFlags & TNCR_IP_UDP)
		{
			// Turn off the UDP flag, since we're doing it now.
			dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_UDP);

			dwMethod = TNRCM_UDP;

			DPL(1, "Having %s try to reach %s via directed UDP sends on port %u.",
				3, pSource->m_szComputerName, pTarget->m_szComputerName, wPort);
		} // end else if (need to do UDP still)
		else if (dwRemainingFlags & TNCR_IP_TCP)
		{
			// Turn off the TCP flag, since we're doing it now.
			dwRemainingFlags = dwRemainingFlags & (~ TNCR_IP_TCP);

			dwMethod = TNRCM_TCP;

			DPL(1, "Having %s try to reach %s via directed TCP sends on port %u.",
				3, pSource->m_szComputerName, pTarget->m_szComputerName, wPort);
		} // end else if (need to do TCP still)
#ifdef DEBUG
		else
		{
			DPL(0, "Got into a wierd state!  dwRemainingFlags=%x  DEBUGBREAK()-ing.",
				1, dwRemainingFlags);

			DEBUGBREAK();

			hr = E_FAIL;
			goto DONE;
		} // end else (something wierd)
#endif // DEBUG


		hr = this->PerformReachCheck(dwMethod,
									pSource,
									&wPort,
									sizeof (WORD),
									((dwMethod == TNRCM_UDPBROADCAST) ? NULL : pvAddresses),
									((dwMethod == TNRCM_UDPBROADCAST) ? 0 : dwAddressesSize),
									pTarget,
									&wPort,
									sizeof (WORD),
									pfReachable);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't have %s perform IP reachability test via port %u to %s!",
				3, pSource->m_szComputerName, wPort, pTarget->m_szComputerName);
			goto DONE;
		} // end if (couldn't perform reach test)

		if ((*pfReachable) == FALSE)
		{
			DPL(0, "WARNING: %s can't reach %s via %s on port %u.",
				4, pSource->m_szComputerName, pTarget->m_szComputerName,
				((dwMethod == TNRCM_UDPBROADCAST) ? "UDP broadcast" : ((dwMethod == TNRCM_UDP) ? "UDP" : "TCP")),
				wPort);

			this->Log(TNLST_CRITICAL,
					"WARNING: %s can't reach %s via %s on port %u!",
					4, pSource->m_szComputerName, pTarget->m_szComputerName,
					((dwMethod == TNRCM_UDPBROADCAST) ? "UDP broadcast" : ((dwMethod == TNRCM_UDP) ? "UDP" : "TCP")),
					wPort);

			goto DONE;
		} // end if (source couldn't reach target)
	} // end do (while still methods left to check)
	while (dwRemainingFlags != 0);


DONE:

	if (pvAddresses != NULL)
	{
		LocalFree(pvAddresses);
		pvAddresses = NULL;
	} // end if (allocated buffer)

	return (hr);
} // CTNMaster::CanReachViaIP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CanReachViaIPX()"
//==================================================================================
// CTNMaster::CanReachViaIPX
//----------------------------------------------------------------------------------
//
// Description: Checks whether the first given machine can send data to and get a
//				reply from the second machine via IPX.  If so pfReachable is set to
//				TRUE, otherwise it is set to FALSE.
//				This should only be called from within a module's CanRun callback
//				because the slave list lock is assumed to be held with only one
//				level of recursion.
//
// Arguments:
//	PTNMACHINEINFO pSender		Pointer to machine doing the initial sending.
//	PTNMACHINEINFO pReceiver	Pointer to machine doing the initial receiving.
//	BOOL* pfReachable			Pointer to boolean to fill with TRUE if the
//								specified communication is possible, FALSE if not.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CanReachViaIPX(PTNMACHINEINFO pSender,
									PTNMACHINEINFO pReceiver,
									BOOL* pfReachable)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSource = (PTNSLAVEINFO) pSender;
	PTNSLAVEINFO			pTarget = (PTNSLAVEINFO) pReceiver;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	int						i;
	PTNREACHCHECK			pReachCheck = NULL;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)

	if ((pSource == NULL) || (pTarget == NULL))
	{
		DPL(0, "Either the source (%x) or the target (%x) machine is NULL, both must exist!",
			2, pSource, pTarget);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source or target are NULL)

	if (pSource == pTarget)
	{
		DPL(0, "The source and the target machines specified are the same (%x)!",
			1, pSource);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source and target are same)

	if (pfReachable == NULL)
	{
		DPL(0, "Must specify a pointer to a boolean to store result in!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no boolean specified)

	// Start by assuming it can't be reached.
	(*pfReachable) = FALSE;


	// If the slaves are actually 2 instances on the same machine, it's always
	// going to fail.
	if (this->AreOnSameMachine(pSource, pTarget))
	{
		DPL(1, "Source and target slaves (IDs %u and %u) are on same machine (%s).",
			3, pSource->m_id.dwTime, pTarget->m_id.dwTime,
			pSource->m_szComputerName);
		goto DONE;
	} // end if (on same machine)


	// Look if we've already run this check before.

	pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pTarget->m_id));
	if (pReachCheckTarget != NULL)
	{
		for(i = 0; i < pReachCheckTarget->m_checks.Count(); i++)
		{
			pReachCheck = (PTNREACHCHECK) pReachCheckTarget->m_checks.GetItem(i);
			if (pReachCheck == NULL)
			{
				DPL(0, "Couldn't get reach check %i!", i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If we're found an IPX check already existing, we've got the
			// result we need.
			if (pReachCheck->m_dwMethod == TNRCM_IPX)
			{
				if (pReachCheck->m_fResult)
				{
					DPL(3, "%s already successfully sent to %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end if (check succeeded)
				else
				{
					DPL(3, "%s couldn't send to %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end else (check failed)

				(*pfReachable) = pReachCheck->m_fResult;

				goto DONE;
			} // end if (found IPX check)
		} // end for (each check)
	} // end if (source had tried to reach target before in some way)


	// If we're here, it means that either the source has never tried to reach the
	// target via IPX.  We have to perform the check now.


	if (! pSource->m_fIPXInstalled)
	{
		DPL(0, "Source %s doesn't have IPX installed, can't connect to %s.",
			2, pSource->m_szComputerName, pTarget->m_szComputerName);
		goto DONE;
	} // end if (couldn't get object)

	if (! pTarget->m_fIPXInstalled)
	{
		DPL(0, "Target %s doesn't have IPX installed, can't be connected to by %s.",
			2, pTarget->m_szComputerName, pSource->m_szComputerName);
		goto DONE;
	} // end if (couldn't get object)


	hr = this->PerformReachCheck(TNRCM_IPX,
								pSource,
								NULL,
								0,
								NULL,
								0,
								pTarget,
								NULL,
								0,
								pfReachable);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't have %s perform IPX reachability test to %s!",
			2, pSource->m_szComputerName, pTarget->m_szComputerName);
		goto DONE;
	} // end if (couldn't perform reach test)

	if ((*pfReachable) == FALSE)
	{
		DPL(0, "WARNING: %s can't reach %s via IPX!",
			2, pSource->m_szComputerName, pTarget->m_szComputerName);

		this->Log(TNLST_CRITICAL,
				"WARNING: %s can't reach %s via IPX!",
				2, pSource->m_szComputerName, pTarget->m_szComputerName);

		goto DONE;
	} // end if (source couldn't reach target)


DONE:

	return (hr);
} // CTNMaster::CanReachViaIPX
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CanReachViaModem()"
//==================================================================================
// CTNMaster::CanReachViaModem
//----------------------------------------------------------------------------------
//
// Description: Checks whether the given caller can connect to the given answerer
//				via a modem.  If so pfReachable is set to TRUE, otherwise it is set
//				to FALSE.
//				This should only be called from within a module's CanRun callback
//				because the slave list lock is assumed to be held with only one
//				level of recursion.
//
// Arguments:
//	PTNMACHINEINFO pCaller		Pointer to machine doing the calling.
//	PTNMACHINEINFO pAnswerer	Pointer to machine whose modem is to be called.
//	BOOL* pfReachable			Pointer to boolean to fill with TRUE if the
//								specified communication is possible, FALSE if not.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CanReachViaModem(PTNMACHINEINFO pCaller,
									PTNMACHINEINFO pAnswerer,
									BOOL* pfReachable)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSource = (PTNSLAVEINFO) pCaller;
	PTNSLAVEINFO			pTarget = (PTNSLAVEINFO) pAnswerer;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	int						i;
	PTNREACHCHECK			pReachCheck = NULL;
	char*					pszSourceDeviceNumber = NULL;
	char*					pszTargetDeviceNumber = NULL;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)

	if ((pSource == NULL) || (pTarget == NULL))
	{
		DPL(0, "Either the source (%x) or the target (%x) machine is NULL, both must exist!",
			2, pSource, pTarget);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source or target are NULL)

	if (pSource == pTarget)
	{
		DPL(0, "The source and the target machines specified are the same (%x)!",
			1, pSource);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source and target are same)

	if (pfReachable == NULL)
	{
		DPL(0, "Must specify a pointer to a boolean to store result in!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no boolean specified)

	// Start by assuming it can't be reached.
	(*pfReachable) = FALSE;


	// If the slaves are actually 2 instances on the same machine, it's always
	// going to fail.
	if (this->AreOnSameMachine(pSource, pTarget))
	{
		DPL(1, "Source and target slaves (IDs %u and %u) are on same machine (%s).",
			3, pSource->m_id.dwTime, pTarget->m_id.dwTime,
			pSource->m_szComputerName);
		goto DONE;
	} // end if (on same machine)


	// Look if we've already run this check before.

	pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pTarget->m_id));
	if (pReachCheckTarget != NULL)
	{
		for(i = 0; i < pReachCheckTarget->m_checks.Count(); i++)
		{
			pReachCheck = (PTNREACHCHECK) pReachCheckTarget->m_checks.GetItem(i);
			if (pReachCheck == NULL)
			{
				DPL(0, "Couldn't get reach check %i!", i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If we're found a modem check already existing, we've got the
			// result we need.
			if (pReachCheck->m_dwMethod == TNRCM_MODEM)
			{
				if (pReachCheck->m_fResult)
				{
					DPL(3, "%s already successfully called %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end if (check succeeded)
				else
				{
					DPL(3, "%s couldn't connect to %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end else (check failed)

				(*pfReachable) = pReachCheck->m_fResult;

				goto DONE;
			} // end if (found modem check)
		} // end for (each check)
	} // end if (source had tried to reach target before in some way)


	// If we're here, it means that either the source has never tried to reach the
	// target via modem.  We have to perform the check now.


#pragma BUGBUG(vanceo, "Check specific modems")
	hr = pSource->m_TAPIdevices.GetFirstTAPIDeviceNumber(&pszSourceDeviceNumber);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get source %s's first TAPI device number.  %e",
			2, pSource->m_szComputerName, hr);

		// We'll assume that means there is no modem device.
		hr = S_OK;

		goto DONE;
	} // end if (couldn't get device number)

	hr = pTarget->m_TAPIdevices.GetFirstTAPIDeviceNumber(&pszTargetDeviceNumber);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get target %s's first TAPI device number.  %e",
			2, pTarget->m_szComputerName, hr);

		// We'll assume that means there is no modem device.
		hr = S_OK;

		goto DONE;
	} // end if (couldn't get device number)


	DPL(5, "Got \"%s\" as number for %s when trying to reach %s (calling from \"%s\").",
		4, pszTargetDeviceNumber, pSource->m_szComputerName,
		pTarget->m_szComputerName, pszTargetDeviceNumber);


	hr = this->PerformReachCheck(TNRCM_MODEM,
								pSource,
								NULL,
								0,
								pszTargetDeviceNumber,
								(strlen(pszTargetDeviceNumber) + 1),
								pTarget,
								NULL,
								0,
								pfReachable);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't have %s perform modem reachability test to %s (number = \"%s\")!",
			3, pSource->m_szComputerName, pTarget->m_szComputerName,
			pszTargetDeviceNumber);
		goto DONE;
	} // end if (couldn't perform reach test)

	if ((*pfReachable) == FALSE)
	{
		DPL(0, "WARNING: %s can't reach %s via modem (number = \"%s\")!",
			3, pSource->m_szComputerName, pTarget->m_szComputerName,
			pszTargetDeviceNumber);

		this->Log(TNLST_CRITICAL,
				"WARNING: %s can't reach %s via modem (number = \"%s\")!",
				3, pSource->m_szComputerName, pTarget->m_szComputerName,
				pszTargetDeviceNumber);

		goto DONE;
	} // end if (source couldn't reach target)


DONE:

	return (hr);
} // CTNMaster::CanReachViaModem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CanReachViaSerial()"
//==================================================================================
// CTNMaster::CanReachViaSerial
//----------------------------------------------------------------------------------
//
// Description: Checks whether the first given machine can send data to and get a
//				reply from the second machine via a serial connection.  If so
//				pfReachable is set to TRUE, otherwise it is set to FALSE.
//				This should only be called from within a module's CanRun callback
//				because the slave list lock is assumed to be held with only one
//				level of recursion.
//
// Arguments:
//	PTNMACHINEINFO pSender		Pointer to machine doing the initial sending.
//	PTNMACHINEINFO pReceiver	Pointer to machine doing the initial receiving.
//	BOOL* pfReachable			Pointer to boolean to fill with TRUE if the
//								specified communication is possible, FALSE if not.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CanReachViaSerial(PTNMACHINEINFO pSender,
									PTNMACHINEINFO pReceiver,
									BOOL* pfReachable)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSource = (PTNSLAVEINFO) pSender;
	PTNSLAVEINFO			pTarget = (PTNSLAVEINFO) pReceiver;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	int						i;
	PTNREACHCHECK			pReachCheck = NULL;
	PTNCOMPORT				pCOMPortSource = NULL;
	PTNCOMPORT				pCOMPortTarget = NULL;
	DWORD					dwCOMPortSource;
	DWORD					dwCOMPortTarget;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)

	if ((pSource == NULL) || (pTarget == NULL))
	{
		DPL(0, "Either the source (%x) or the target (%x) machine is NULL, both must exist!",
			2, pSource, pTarget);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source or target are NULL)

	if (pSource == pTarget)
	{
		DPL(0, "The source and the target machines specified are the same (%x)!",
			1, pSource);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (source and target are same)

	if (pfReachable == NULL)
	{
		DPL(0, "Must specify a pointer to a boolean to store result in!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no boolean specified)

	// Start by assuming it can't be reached.
	(*pfReachable) = FALSE;


	// If the slaves are actually 2 instances on the same machine, it's always
	// going to fail.
	if (this->AreOnSameMachine(pSource, pTarget))
	{
		DPL(1, "Source and target slaves (IDs %u and %u) are on same machine (%s).",
			3, pSource->m_id.dwTime, pTarget->m_id.dwTime,
			pSource->m_szComputerName);
		goto DONE;
	} // end if (on same machine)


	// Look if we've already run this check before.

	pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pTarget->m_id));
	if (pReachCheckTarget != NULL)
	{
		for(i = 0; i < pReachCheckTarget->m_checks.Count(); i++)
		{
			pReachCheck = (PTNREACHCHECK) pReachCheckTarget->m_checks.GetItem(i);
			if (pReachCheck == NULL)
			{
				DPL(0, "Couldn't get reach check %i!", i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If we're found a serial check already existing, we've got the
			// result we need.
			if (pReachCheck->m_dwMethod == TNRCM_SERIAL)
			{
				if (pReachCheck->m_fResult)
				{
					DPL(3, "%s already successfully sent to %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end if (check succeeded)
				else
				{
					DPL(3, "%s couldn't send to %s earlier.",
						2, pSource->m_szComputerName,
						pTarget->m_szComputerName);
				} // end else (check failed)

				(*pfReachable) = pReachCheck->m_fResult;

				goto DONE;
			} // end if (found serial check)
		} // end for (each check)
	} // end if (source had tried to reach target before in some way)


	// If we're here, it means that either the source has never tried to reach the
	// target via serial.  We have to perform the check now.


#pragma BUGBUG(vanceo, "More features?")
	pCOMPortSource = (PTNCOMPORT) pSource->m_COMports.GetStringObject(pTarget->m_szComputerName, 0, FALSE);
	if (pCOMPortSource == NULL)
	{
		DPL(0, "Source %s doesn't have any COM ports connected to %s.",
			2, pSource->m_szComputerName, pTarget->m_szComputerName);

		// Assume that's not because of some wacky error.
		hr = S_OK;

		goto DONE;
	} // end if (couldn't get object)


	// Just double check that the target has a mirror image connection.

	pCOMPortTarget = (PTNCOMPORT) pTarget->m_COMports.GetStringObject(pSource->m_szComputerName, 0, FALSE);
	if (pCOMPortTarget == NULL)
	{
		DPL(0, "Target %s doesn't have a mirror image COM port connected to %s!",
			2, pTarget->m_szComputerName, pSource->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get object)


	dwCOMPortSource = pCOMPortSource->GetCOMPort();
	dwCOMPortTarget = pCOMPortTarget->GetCOMPort();

	DPL(5, "%s should use COM port %u when trying to reach %s (whose COM port should be %u).",
		4, pSource->m_szComputerName, dwCOMPortSource,
		pTarget->m_szComputerName, dwCOMPortTarget);


	hr = this->PerformReachCheck(TNRCM_SERIAL,
								pSource,
								NULL,
								0,
								&dwCOMPortSource,
								sizeof (DWORD),
								pTarget,
								NULL,
								0,
								pfReachable);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't have %s perform serial reachability test to %s (COM port = \"%s\")!",
			3, pSource->m_szComputerName, pTarget->m_szComputerName,
			dwCOMPortSource);
		goto DONE;
	} // end if (couldn't perform reach test)

	if ((*pfReachable) == FALSE)
	{
		DPL(0, "WARNING: %s can't reach %s via COM port %u!",
			3, pSource->m_szComputerName, pTarget->m_szComputerName,
			dwCOMPortSource);

		this->Log(TNLST_CRITICAL,
				"WARNING: %s can't reach %s via COM port %u!",
				3, pSource->m_szComputerName, pTarget->m_szComputerName,
				dwCOMPortSource);

		goto DONE;
	} // end if (source couldn't reach target)


DONE:

	return (hr);
} // CTNMaster::CanReachViaSerial
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetPlannedInputData()"
//==================================================================================
// CTNMaster::GetPlannedInputData
//----------------------------------------------------------------------------------
//
// Description: Called by CanRun functions to retrieve the input data that will be
//				used by the given tester if the permutation is actually run.  The
//				case's GetInputData function is called to do the conversion, so
//				naturally the module must implement that function as well.
//				If pvInputData is NULL, the size required is stored in pdwInputData
//				and ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	int iTesterNum				Tester number whose input data should be retrieved.
//	PTNCANRUNDATA pTNcrd		Pointer to TNCANRUNDATA structure passed in to
//								CanRun function calling this procedure.
//	PVOID pvInputData			Pointer to buffer to retrieve input data, or NULL to
//								retrieve size.
//	DWORD* pdwInputDataSize		Pointer to size of input data buffer, or place to
//								store size required.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::GetPlannedInputData(int iTesterNum, PTNCANRUNDATA pTNcrd,
										PVOID pvInputData, DWORD* pdwInputDataSize)
{
	HRESULT			hr = S_OK;
	TNGETINPUTDATA	tngid;


	if (this == NULL)
	{
		DPL(0, "Master object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got bad pointer)

	if (pTNcrd == NULL)
	{
		DPL(0, "Must pass valid TNCANRUNDATA!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (pointer is NULL)

	if (pdwInputDataSize == NULL)
	{
		DPL(0, "Must pass valid input data size pointer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (pointer is NULL)

	if (pTNcrd->dwSize != sizeof (TNCANRUNDATA))
	{
		DPL(0, "TNCANRUNDATA structure is invalid (%u != %u)!",
			2, pTNcrd->dwSize, sizeof (TNCANRUNDATA));
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (structure is invalid)

	if ((iTesterNum < 0) || (iTesterNum >= pTNcrd->iNumMachines))
	{
		DPL(0, "Tester number %i is invalid (<0 or >%i)!",
			2, iTesterNum, pTNcrd->iNumMachines);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (tester number is invalid)

	if (pTNcrd->pTest->m_pfnGetInputData == NULL)
	{
		DPL(0, "Test case \"%s\" does not have a GetInputData function!",
			1, pTNcrd->pTest->m_pszID);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (no GetInputData function implemented)


	ZeroMemory(&tngid, sizeof (TNGETINPUTDATA));
	tngid.dwSize = sizeof (TNGETINPUTDATA);
	tngid.pMaster = this;
	tngid.pMachine = pTNcrd->apTesters[iTesterNum];
	tngid.iTesterNum = iTesterNum;
	tngid.pTest = pTNcrd->pTest;
	tngid.pStringData = pTNcrd->pStringData;
	tngid.pvData = pvInputData;
	tngid.dwDataSize = (*pdwInputDataSize);

	hr = pTNcrd->pTest->m_pfnGetInputData(&tngid);
	if ((hr != S_OK) && (hr != ERROR_BUFFER_TOO_SMALL))
	{
		DPL(0, "Module's GetInputData callback for case \"%s\" failed!  %e",
			1, pTNcrd->pTest->m_pszID);
		//goto DONE;
	} // end if (module's callback failed)


DONE:

	return (hr);
} // CTNMaster::GetPlannedInputData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::ProcessJobQueue()"
//==================================================================================
// CTNMaster::ProcessJobQueue
//----------------------------------------------------------------------------------
//
// Description: Performs all jobs in the queue that are ready to be processed.
//				This function assumes the slave and job locks are taken.
//
// Arguments:
//	DWORD* pdwTimeToNextJob		How long to wait until the next job should be
//								performed.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::ProcessJobQueue(DWORD* pdwTimeToNextJob)
{
	HRESULT			hr = S_OK;
	BOOL			fHaveJobQueueLock = FALSE;
	PTNJOB			pJob = NULL;
	PTNSLAVEINFO	pSlave;
	DWORD			dwCurrentTime = 0;


	DPL(9, "==>", 0);


	// Take the locks.
	// We take it here because we don't want folks adding to the queue while we
	// trying to determine how long to sleep.
	// We take the slave lock here because:
	// 1) all of our job operations require it
	// 2) some other threads already have the slave lock and try to add a job
	//    to the queue which requires the job lock.  If we take the job lock
	//    first here, we will deadlock with that thread.
	this->m_slaves.EnterCritSection();
	this->m_jobs.EnterCritSection();
	fHaveJobQueueLock = TRUE;



	// Start by assuming we'll wait forever
	(*pdwTimeToNextJob) = INFINITE;


	// Keep looping until all the jobs are done or we find a job that's
	// not ready to be run yet.
	while (this->m_jobs.Count() > 0)
	{
		// Reset the event to reduce the times where it fires but we already
		// handled it in here.
		if (! ResetEvent(this->m_jobs.m_hNewJobEvent))
		{
			hr = GetLastError();

			DPL(0, "Couldn't reset new job event %x!",
				1, this->m_jobs.m_hNewJobEvent);

			if (hr != S_OK)
				hr = E_FAIL;

			goto DONE;
		} // end if (couldn't reset event)

		dwCurrentTime = GetTickCount();


		pJob = (PTNJOB) this->m_jobs.GetItem(0);
		if (pJob == NULL)
		{
			DPL(0, "Couldn't get first job!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get job)


		// If the job shouldn't be run right now, and it's not past
		// the time, we're done.
		if ((pJob->m_dwTime != 0) &&
			(pJob->m_dwTime > dwCurrentTime))
		{
			(*pdwTimeToNextJob) = pJob->m_dwTime - dwCurrentTime;

			DPL(9, "Next job (%x, op = %u) in %u ms.",
				3, pJob, pJob->m_dwOperation, (*pdwTimeToNextJob));

			pJob = NULL; // forget about it, so we don't free it below
			goto DONE;
		} // end if (this job shouldn't be run yet)


		hr = this->m_jobs.RemoveFirstReference(pJob);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove job %x from list!", 1, pJob);
			goto DONE;
		} // end if (couldn't take job off list)


		// Drop the job lock so other jobs can be added to the queue while
		// we're working on this one.
		this->m_jobs.LeaveCritSection();
		fHaveJobQueueLock = FALSE;


		DPL(9, "++Processing job %x (op %u, time after trigger time %u).",
			3, pJob, pJob->m_dwOperation,
			((pJob->m_dwTime == 0) ? 0 : dwCurrentTime - pJob->m_dwTime));



		// Actually perform the operation.
		switch (pJob->m_dwOperation)
		{
			case TNMJ_HANDLEREPORT:
				pSlave = this->m_slaves.GetSlaveByID(&(pJob->m_idSlave));
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't find slave (%u) in list!  Ignoring HandleReport.",
						1, pJob->m_idSlave.dwTime);

					// BUGBUG Fail?
					goto DONE;
				} // end if (couldn't get slaveinfo object)

				hr = this->DoHandleReport(pSlave, ((PCTRLMSG_REPORT) pJob->m_pvContext));
				if (hr != S_OK)
				{
					DPL(0, "Couldn't handle report for %s!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (couldn't handle report)
			  break;

			case TNMJ_GETNEXTCMD:
				pSlave = this->m_slaves.GetSlaveByID(&(pJob->m_idSlave));
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't find slave (%u) in list!  Ignoring GetNextCmd request.",
						1, pJob->m_idSlave.dwTime);

					// BUGBUG Fail?
					goto DONE;
				} // end if (couldn't get slaveinfo object)

				hr = this->GetNextCmdForMachine(pSlave);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't get next command for %s!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (couldn't get next command)
			  break;

			case TNMJ_PINGSLAVES:
				hr = this->DoPingCheck();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't do ping check!", 0);
					goto DONE;
				} // end if (couldn't do ping check)

				// If there are still slaves in the session, re-add this
				// job so we can check again.
				if (this->m_slaves.Count() > 0)
				{
					hr = this->m_jobs.AddJob(TNMJ_PINGSLAVES, SLAVE_CHECK_INTERVAL,
											NULL, NULL);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't readd ping check job!", 0);
						goto DONE;
					} // end if (couldn't re-add job)
				} // end if (there are slaves)
			  break;

			case TNMJ_READYTOTEST:
				hr = this->DoSlaveReadyToTest(&(pJob->m_idSlave));
				if (hr != S_OK)
				{
					DPL(0, "Couldn't mark slave (%u) as ready to test!",
						1, pJob->m_idSlave.dwTime);
					goto DONE;
				} // end if (couldn't mark slave as ready to test)
			  break;

			case TNMJ_CLOSESESSION:
				hr = this->CloseSession();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't close session!", 0);
					goto DONE;
				} // end if (couldn't close session)
			  break;

			default:
				DPL(0, "Unrecognized job type (%u)!", 1, pJob->m_dwOperation);
				hr = E_FAIL;
				goto DONE;
			  break;
		} // end switch (on job type)


		DPL(9, "--Done processing job %x, remaining = %i.",
			2, pJob, this->m_jobs.Count());


		if (pJob->m_dwRefCount == 0)
		{
			DPL(7, "Deleting job %x.", 1, pJob);
			delete (pJob);
		} // end if (last job reference)
		else
		{
			DPL(7, "Not deleting job %x, its refcount is %u.",
				2, pJob, pJob->m_dwRefCount);
		} // end else (shouldn't delete job)
		pJob = NULL;


		// Take the job lock again so we can check for more jobs.
		this->m_jobs.EnterCritSection();
		fHaveJobQueueLock = TRUE;
	} // end while (there are still jobs)

	DPL(9, "No next job.", 0);


DONE:

	if (fHaveJobQueueLock)
	{
		this->m_jobs.LeaveCritSection();
		fHaveJobQueueLock = FALSE;
	} // end if (have job queue lock)

	this->m_slaves.LeaveCritSection();

	if (pJob != NULL)
	{
		if (pJob->m_dwRefCount == 0)
		{
			DPL(7, "Deleting job %x.", 1, pJob);
			delete (pJob);
		} // end if (last job reference)
		else
		{
			DPL(7, "Not deleting job %x, its refcount is %u.",
				2, pJob, pJob->m_dwRefCount);
		} // end else (shouldn't delete job)
		pJob = NULL;
	} // end if (still have job pointer)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::ProcessJobQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleMessage()"
//==================================================================================
// CTNMaster::HandleMessage
//----------------------------------------------------------------------------------
//
// Description: Handles an incoming control session message.
//
// Arguments:
//	PVOID* ppvFromAddress		Pointer to pointer to data describing ctrl comm
//								from address.
//	DWORD dwFromAddressSize		Size of from address data.
//	PVOID pvData				Pointer to data received by ctrl comm.
//	DWORD dwDataSize			Size of data.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleMessage(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								  PVOID pvData, DWORD dwDataSize)
{
	PCTRLMSG_GENERIC	pGenericMsg = NULL;


	if (this->m_dwStartupTicks == 0)
	{
		DPL(0, "Received message before started up!  Ignoring.", 0);
		return (S_OK);
	} // end if (we haven't fully started up yet)

	if ((*ppvFromAddress) == NULL)
	{
		DPL(0, "WARNING!  No from address!", 0);
	} // end if (no from address info)

	if (dwDataSize < sizeof (CTRLMSG_GENERIC))
	{
		DPL(0, "Data too small to be valid message!  Ignoring.", 0);
		return (S_OK);
	} // end if (smaller than the smallest valid message)

	pGenericMsg = (PCTRLMSG_GENERIC) pvData;
	
	if (pGenericMsg->dwSize != dwDataSize)
	{
		DPL(0, "Incomplete or invalid message (expected size %i bytes differs from actual size %i bytes)!  Ignoring.",
			2, pGenericMsg->dwSize, dwDataSize);
		return (S_OK);
	} // end if (didn't get complete/valid message)

	switch(pGenericMsg->dwType)
	{
		case CTRLMSGID_ENUM:
			if (dwDataSize < sizeof (CTRLMSG_ENUM))
			{
				DPL(0, "Data too small to be valid Enum message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid enum message)

			return (this->HandleEnumMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_ENUM) pGenericMsg));
		  break;

		case CTRLMSGID_HANDSHAKE:
			if (dwDataSize < sizeof (CTRLMSG_HANDSHAKE))
			{
				DPL(0, "Data too small to be valid Handshake message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid handshake message)

			return (this->HandleHandshakeMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_HANDSHAKE) pGenericMsg));
		  break;

		case CTRLMSGID_JOIN:
			if (dwDataSize < sizeof (CTRLMSG_JOIN))
			{
				DPL(0, "Data too small to be valid Join message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid join message)

			return (this->HandleJoinMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_JOIN) pGenericMsg));
		  break;

		case CTRLMSGID_READYTOTEST:
			if (dwDataSize < sizeof (CTRLMSG_READYTOTEST))
			{
				DPL(0, "Data too small to be valid ReadyToTest message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid readytotest message)

			return (this->HandleReadyToTestMsg(ppvFromAddress, dwFromAddressSize,
											(PCTRLMSG_READYTOTEST) pGenericMsg));
		  break;

		case CTRLMSGID_ANNOUNCESUBTEST:
			if (dwDataSize < sizeof (CTRLMSG_ANNOUNCESUBTEST))
			{
				DPL(0, "Data too small to be valid AnnounceSubTest message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid announcesubtest message)

			return (this->HandleAnnounceSubTestMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_ANNOUNCESUBTEST) pGenericMsg));
		  break;

		case CTRLMSGID_REPORT:
			if (dwDataSize < (sizeof (CTRLMSG_REPORT)))
			{
				DPL(0, "Data too small to be valid Report message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid report message)

			return (this->HandleReportMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_REPORT) pGenericMsg));
		  break;

		case CTRLMSGID_WARNING:
			if (dwDataSize < (sizeof (CTRLMSG_WARNING)))
			{
				DPL(0, "Data too small to be valid Warning message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid report message)

			return (this->HandleWarningMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_WARNING) pGenericMsg));
		  break;

		case CTRLMSGID_REQUESTSYNC:
			if (dwDataSize < (sizeof (CTRLMSG_REQUESTSYNC)))
			{
				DPL(0, "Data too small to be valid RequestSync message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid requestsync message)

			return (this->HandleRequestSyncMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_REQUESTSYNC) pGenericMsg));
		  break;

		case CTRLMSGID_USERQUERY:
			if (dwDataSize < (sizeof (CTRLMSG_USERQUERY)))
			{
				DPL(0, "Data too small to be valid UserQuery message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid userquery message)

			return (this->HandleUserQueryMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_USERQUERY) pGenericMsg));
		  break;

		case CTRLMSGID_IDLE:
			if (dwDataSize < (sizeof (CTRLMSG_IDLE)))
			{
				DPL(0, "Data too small to be valid Idle message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid idle message)

			return (this->HandleIdleMsg(ppvFromAddress, dwFromAddressSize,
										(PCTRLMSG_IDLE) pGenericMsg));
		  break;

		case CTRLMSGID_REGISTERMASTERREPLY:
			if (dwDataSize < (sizeof (CTRLMSG_REGISTERMASTERREPLY)))
			{
				DPL(0, "Data too small to be valid RegisterMasterReply message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid registermasterreply message)

			return (this->HandleRegisterMasterReplyMsg(ppvFromAddress, dwFromAddressSize,
														(PCTRLMSG_REGISTERMASTERREPLY) pGenericMsg));
		  break;

		case CTRLMSGID_MASTERPINGSLAVEREPLY:
			if (dwDataSize < (sizeof (CTRLMSG_MASTERPINGSLAVEREPLY)))
			{
				DPL(0, "Data too small to be valid MasterPingSlaveReply message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid masterpingslavereply message)

			return (this->HandleMasterPingReplyMsg(ppvFromAddress, dwFromAddressSize,
													(PCTRLMSG_MASTERPINGSLAVEREPLY) pGenericMsg));
		  break;

		case CTRLMSGID_REQUESTPOKETEST:
			if (dwDataSize < (sizeof (CTRLMSG_REQUESTPOKETEST)))
			{
				DPL(0, "Data too small to be valid RequestPokeTest message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid requestpoketest message)

			return (this->HandleRequestPokeTestMsg(ppvFromAddress, dwFromAddressSize,
													(PCTRLMSG_REQUESTPOKETEST) pGenericMsg));
		  break;

		case CTRLMSGID_ACCEPTREACHCHECKREPLY:
			if (dwDataSize < (sizeof (CTRLMSG_ACCEPTREACHCHECKREPLY)))
			{
				DPL(0, "Data too small to be valid AcceptReachCheckReply message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid acceptreachcheckreply message)

			return (this->HandleAcceptReachCheckReplyMsg(ppvFromAddress, dwFromAddressSize,
														(PCTRLMSG_ACCEPTREACHCHECKREPLY) pGenericMsg));
		  break;

		case CTRLMSGID_CONNECTREACHCHECKREPLY:
			if (dwDataSize < (sizeof (CTRLMSG_CONNECTREACHCHECKREPLY)))
			{
				DPL(0, "Data too small to be valid ConnectReachCheckReply message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid connectreachcheckreply message)

			return (this->HandleConnectReachCheckReplyMsg(ppvFromAddress, dwFromAddressSize,
														(PCTRLMSG_CONNECTREACHCHECKREPLY) pGenericMsg));
		  break;

		case CTRLMSGID_FREEOUTPUTVARS:
			if (dwDataSize < (sizeof (CTRLMSG_FREEOUTPUTVARS)))
			{
				DPL(0, "Data too small to be valid FreeOutputVars message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid freeoutputvars message)

			return (this->HandleFreeOutputVarsMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_FREEOUTPUTVARS) pGenericMsg));
		  break;

		case CTRLMSGID_GETMACHINEINFO:
			if (dwDataSize < (sizeof (CTRLMSG_GETMACHINEINFO)))
			{
				DPL(0, "Data too small to be valid GetMachineInfo message!  Ignoring.", 0);
				break;
			} // end if (smaller than the smallest valid getmachineinfo message)

			return (this->HandleGetMachineInfoMsg(ppvFromAddress, dwFromAddressSize,
												(PCTRLMSG_GETMACHINEINFO) pGenericMsg));
		  break;

		default:
			DPL(0, "Message of invalid/unknown type (%u)!  Ignoring.", 1, pGenericMsg->dwType);
		  break;
	} // end switch (on message type)

	return (S_OK);
} // CTNMaster::HandleMessage
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleEnumMsg()"
//==================================================================================
// CTNMaster::HandleEnumMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session enum message.
//
// Arguments:
//	PVOID* ppvFromAddress		Pointer to pointer to data describing ctrl comm
//								from address.
//	DWORD dwFromAddressSize		Size of from address data.
//	PCTRLMSG_ENUM pEnumMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleEnumMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_ENUM pEnumMsg)
{
	HRESULT					hr = S_OK;
	PTNSENDDATA				pSendData = NULL;
	PCTRLMSG_ENUMREPLY		pEnumReplyMsg = NULL;


	//DPL(0, "Got Enum message.", 0);


	if (! this->m_fJoinersAllowed)
	{
		DPL(0, "Joiners are not currently allowed, ignoring.", 0);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring enumeration, joiners are not currently allowed.", 0);

		goto DONE;
	} // end if (joiners aren't allowed)


	// Compare shell versions
	if (pEnumMsg->dwVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "Enum request control version (%u) is different from ours (%u), ignoring.",
			2, pEnumMsg->dwVersion, CURRENT_TNCONTROL_API_VERSION);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring enumeration, control version is different.", 0);

		goto DONE;
	} // end if (version isn't the same)


	// Compare app IDs
	if ((strcmp(pEnumMsg->moduleID.szBriefName, this->m_moduleID.szBriefName) != 0) ||
		(pEnumMsg->moduleID.dwMajorVersion != this->m_moduleID.dwMajorVersion) ||
		(pEnumMsg->moduleID.dwMinorVersion1 != this->m_moduleID.dwMinorVersion1))
	{
		DPL(0, "Enum request module ID (_%s v%u.%u_.%u.%u) doesn't match ours (_%s v%u.%u_.%u.%u), ignoring.",
			10,
			pEnumMsg->moduleID.szBriefName,
			pEnumMsg->moduleID.dwMajorVersion,
			pEnumMsg->moduleID.dwMinorVersion1,
			pEnumMsg->moduleID.dwMinorVersion2,
			pEnumMsg->moduleID.dwBuildVersion,
			this->m_moduleID.szBriefName,
			this->m_moduleID.dwMajorVersion,
			this->m_moduleID.dwMinorVersion1,
			this->m_moduleID.dwMinorVersion2,
			this->m_moduleID.dwBuildVersion);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring enumeration, module ID doesn't match.", 0);

		goto DONE;
	} // end if (failed to match our app id)


	// Compare user specified session IDs

	if (this->m_pszSessionFilter == NULL)
	{
		if (strcmp((char*) (pEnumMsg + 1), "") != 0)
		{
			DPL(0, "Request looking for session ID (\"%s\"), but we don't have one specified.  Ignoring.",
				1, (pEnumMsg + 1));

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Ignoring enumeration, specified session ID/key but we don't have one.", 0);

			goto DONE;
		} // end if (the enum request is for a specific session)
	} // end if (we are not a specific session)
	else
	{
		if (strcmp((char*) (pEnumMsg + 1), this->m_pszSessionFilter) != 0)
		{
			DPL(0, "Request looking for different session ID (\"%s\" != \"%s\"), ignoring.",
				2, (pEnumMsg + 1), this->m_pszSessionFilter);

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Ignoring enumeration, specified different session ID/key.", 0);

			goto DONE;
		} // end if (the enum request is for a different/no session)
	} // end else (we are a specific session)



	if (pEnumMsg->dwMode != this->m_dwMode)
	{
		char	szTemp1[32];
		char	szTemp2[32];


		switch (pEnumMsg->dwMode)
		{
			case TNMODE_API:
				strcpy(szTemp1, "an API");
			  break;

			case TNMODE_STRESS:
				strcpy(szTemp1, "a stress");
			  break;

			case TNMODE_POKE:
				strcpy(szTemp1, "a poke");
			  break;

			case TNMODE_PICKY:
				strcpy(szTemp1, "a picky");
			  break;

			default:
				wsprintf(szTemp1, "a wierd (type %i)", pEnumMsg->dwMode);
			  break;
		} // end switch (on enum mode type)

		switch (this->m_dwMode)
		{
			case TNMODE_API:
				strcpy(szTemp2, "API");
			  break;

			case TNMODE_STRESS:
				strcpy(szTemp2, "stress");
			  break;

			case TNMODE_POKE:
				strcpy(szTemp2, "poke");
			  break;

			case TNMODE_PICKY:
				strcpy(szTemp2, "picky");
			  break;
		} // end switch (on our mode type)

		DPL(0, "Enum requestor is looking for %s session but we are in %s mode, ignoring.",
			2, szTemp1, szTemp2);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring enumeration, wrong mode.", 0);

		goto DONE;
	} // end if (modes don't match)


	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = FALSE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_ENUMREPLY);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pEnumReplyMsg = (PCTRLMSG_ENUMREPLY) pSendData->m_pvData;
	pEnumReplyMsg->dwSize = pSendData->m_dwDataSize;
	pEnumReplyMsg->dwType = CTRLMSGID_ENUMREPLY;

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNMaster::HandleEnumMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleHandshakeMsg()"
//==================================================================================
// CTNMaster::HandleHandshakeMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session handshake message.
//
// Arguments:
//	PVOID* ppvFromAddress				Pointer to pointer to data describing ctrl
//										comm from address.
//	DWORD dwFromAddressSize				Size of from address data.
//	PCTRLMSG_HANDSHAKE pHandshakeMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleHandshakeMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_HANDSHAKE pHandshakeMsg)
{
	HRESULT						hr = S_OK;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_HANDSHAKEREPLY		pHandshakeReplyMsg = NULL;
	DWORD						dwStartupDataSize = 0;


	//DPL(0, "Got Handshake message.", 0);


	//BUGBUG if we refuse the connection, we need to shutdown the link and/or notify
	//		 the other side

	if (! this->m_fJoinersAllowed)
	{
		DPL(0, "Joiners are not currently allowed, ignoring.", 0);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring join attempt, not currently allowed.", 0);

		goto DONE;
	} // end if (joiners aren't allowed)

	// Compare shell versions
	if (pHandshakeMsg->dwVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "Handshake request control version (%u) is different from ours (%u), ignoring.",
			2, pHandshakeMsg->dwVersion, CURRENT_TNCONTROL_API_VERSION);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring join attempt, control version is different.", 0);

		goto DONE;
	} // end if (version isn't the same)


	// Compare app IDs
	if ((strcmp(pHandshakeMsg->moduleID.szBriefName, this->m_moduleID.szBriefName) != 0) ||
		(pHandshakeMsg->moduleID.dwMajorVersion != this->m_moduleID.dwMajorVersion) ||
		(pHandshakeMsg->moduleID.dwMinorVersion1 != this->m_moduleID.dwMinorVersion1))
	{
		DPL(0, "Handshake request module ID (_%s v%u.%u_.%u.%u) doesn't match ours (_%s v%u.%u_.%u.%u), ignoring.",
			10,
			pHandshakeMsg->moduleID.szBriefName,
			pHandshakeMsg->moduleID.dwMajorVersion,
			pHandshakeMsg->moduleID.dwMinorVersion1,
			pHandshakeMsg->moduleID.dwMinorVersion2,
			pHandshakeMsg->moduleID.dwBuildVersion,
			this->m_moduleID.szBriefName,
			this->m_moduleID.dwMajorVersion,
			this->m_moduleID.dwMinorVersion1,
			this->m_moduleID.dwMinorVersion2,
			this->m_moduleID.dwBuildVersion);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring join attempt, module ID doesn't match.", 0);

		goto DONE;
	} // end if (failed to match our app id)


	// Compare user specified session IDs

	if (this->m_pszSessionFilter == NULL)
	{
		if (strcmp((char*) (pHandshakeMsg + 1), "") != 0)
		{
			DPL(0, "Request looking for specified session ID (\"%s\"), but we don't have one specified.  Ignoring.",
				1, (pHandshakeMsg + 1));

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Ignoring join attempt, specified session ID/key but we don't have one.", 0);

			goto DONE;
		} // end if (the enum request is for a specific session)
	} // end if (we are not a specific session)
	else
	{
		if (strcmp((char*) (pHandshakeMsg + 1), this->m_pszSessionFilter) != 0)
		{
			DPL(0, "Request looking for different session ID (\"%s\" != \"%s\"), ignoring.",
				2, (pHandshakeMsg + 1), this->m_pszSessionFilter);

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Ignoring join attempt, specified different session ID/key.", 0);

			goto DONE;
		} // end if (the enum request is for a different/no session)
	} // end else (we are a specific session)



	if (pHandshakeMsg->dwMode != this->m_dwMode)
	{
		char	szTemp1[32];
		char	szTemp2[32];


		switch (pHandshakeMsg->dwMode)
		{
			case TNMODE_API:
				strcpy(szTemp1, "an API");
			  break;

			case TNMODE_STRESS:
				strcpy(szTemp1, "a stress");
			  break;

			case TNMODE_POKE:
				strcpy(szTemp1, "a poke");
			  break;

			case TNMODE_PICKY:
				strcpy(szTemp1, "a picky");
			  break;

			default:
				wsprintf(szTemp1, "a wierd (type %i)", pHandshakeMsg->dwMode);
			  break;
		} // end switch (on handshake mode type)

		switch (this->m_dwMode)
		{
			case TNMODE_API:
				strcpy(szTemp2, "API");
			  break;

			case TNMODE_STRESS:
				strcpy(szTemp2, "stress");
			  break;

			case TNMODE_POKE:
				strcpy(szTemp2, "poke");
			  break;

			case TNMODE_PICKY:
				strcpy(szTemp2, "picky");
			  break;
		} // end switch (on our mode type)

		DPL(0, "Handshaker is looking for %s session but we are in %s mode, ignoring.",
			2, szTemp1, szTemp2);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Ignoring join attempt, wrong mode.", 0);

		goto DONE;
	} // end if (modes don't match)


	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed


	// If the module has startup data, get its size
	if (this->m_pfnGetStartupData != NULL)
		this->m_pfnGetStartupData(NULL, &dwStartupDataSize); // ignoring error


	pSendData->m_dwDataSize = sizeof (CTRLMSG_HANDSHAKEREPLY)
							+ dwStartupDataSize;

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		(*ppvFromAddress) = NULL;
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pHandshakeReplyMsg = (PCTRLMSG_HANDSHAKEREPLY) pSendData->m_pvData;
	pHandshakeReplyMsg->dwSize = pSendData->m_dwDataSize;
	pHandshakeReplyMsg->dwType = CTRLMSGID_HANDSHAKEREPLY;

	pHandshakeReplyMsg->dwSessionID = this->m_dwSessionID;
	pHandshakeReplyMsg->dwStartupDataSize = dwStartupDataSize;

	if ((this->m_pfnGetStartupData != NULL) && (dwStartupDataSize > 0))
	{
		hr = this->m_pfnGetStartupData((pHandshakeReplyMsg + 1), &dwStartupDataSize);
		if (hr != S_OK)
		{
			DPL(0, "User's GetStartupData function failed!", 0);
			goto DONE;
		} // end if (getting startup data failed)
	} // end if (the user has startup data)


	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
		delete (pSendData);

	return (hr);
} // CTNMaster::HandleHandshakeMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleJoinMsg()"
//==================================================================================
// CTNMaster::HandleJoinMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session join message.
//
// Arguments:
//	PVOID* ppvFromAddress		Pointer to pointer to data describing ctrl comm
//								from address.
//	DWORD dwFromAddressSize		Size of from address data.
//	PCTRLMSG_JOIN pJoinMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleJoinMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_JOIN pJoinMsg)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pNewMachine = NULL;
	PTNSLAVEINFO			pOtherSlave = NULL;
	PTNSENDDATA				pSendData = NULL;
	PCTRLMSG_JOINREPLY		pJoinReplyMsg = NULL;
	DWORD					dwPingSlaveThreadId = 0;
	char					szVar[1024];
	int						i;
	BOOL					fHaveSlaveListLock = FALSE;
	BOOL					fCloseSessionNow = FALSE;
	BOOL					fCloseSessionLater = FALSE;


	//DPL(0, "Got Join message.", 0);

	// Compare test checksums
	if (memcmp(&(pJoinMsg->testChecksum), &(this->m_testChecksum), sizeof (CHECKSUM)) != 0)
	{
		DPL(0, "Slave's test checksum doesn't match ours ({%v} != {%v})!  Ignoring.",
			4, &(pJoinMsg->testChecksum), sizeof (CHECKSUM),
			&(this->m_testChecksum),  sizeof (CHECKSUM));
		goto DONE;
	} // end if (failed to match test checksum)

	pNewMachine = new (CTNSlaveInfo);
	if (pNewMachine == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pNewMachine->m_dwRefCount++; // we're using it


	hr = pNewMachine->UnpackFromBuffer((pJoinMsg + 1), pJoinMsg->dwInfoSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't unpack machine info from join message!", 0);
		goto DONE;
	} // end if (couldn't unpack machine info item from buffer)

	// Assign it an ID
#ifndef _XBOX // timeGetTime not supported
	pNewMachine->m_id.dwTime = timeGetTime();
#else // ! XBOX
	pNewMachine->m_id.dwTime = GetTickCount();
#endif // XBOX

	//HACKHACK Make sure no 2 machines can get the same ID (by sleeping for well over
	//		   the timeGetTime resolution).  We need a real uniqueness ID though.
	Sleep(100);

	// Save the multi-instance key he's telling us about.
	pNewMachine->m_dwMultiInstanceKey = pJoinMsg->dwMultiInstanceKey;


	//pNewMachine->commdata.hStatusEvent = this->m_hAllSlavesCommStatusEvent;
	pNewMachine->m_commdata.fDropped = FALSE;
	pNewMachine->m_commdata.pvAddress = (*ppvFromAddress);
	pNewMachine->m_commdata.dwAddressSize = dwFromAddressSize;

	hr = this->m_pCtrlComm->BindDataToAddress(&(pNewMachine->m_commdata));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't bind new machine's control comm data!", 0);
		goto DONE;
	} // end if (couldn't bind ctrl comm address)


	// If we're using a TCP/IP control method, then we can do some extra
	// processing with regards to NAT stuff.
	if ((this->m_pCtrlComm->m_dwMethodID == TN_CTRLMETHOD_TCPIP_OPTIMAL) ||
		(this->m_pCtrlComm->m_dwMethodID == TN_CTRLMETHOD_TCPIP_WINSOCK1))
	{
		SOCKADDR_IN*	pSockAddr;


		// Since we know the control method we can crack the address the slave
		// spit back at us to find out what IP the slave is talking to us on. See
		// the commtcp file.

		pSockAddr = (SOCKADDR_IN*) (((LPBYTE) (pJoinMsg + 1)) + pJoinMsg->dwInfoSize);

		// If the slave is sending to us at a different IP address from the ones
		// WinSock told us we were using, then add it with the note that it's an
		// inbound NAT IP.  If multiple slaves tell us the same thing, the address
		// will only be added the first time.
		if (! (this->m_ipaddrs.DoesIPExist(&(pSockAddr->sin_addr))))
		{
			DPL(0, "Slave is talking to a different IP address from ones we know about (%o).",
				1, pSockAddr);

			hr = this->m_ipaddrs.AddIP(&(pSockAddr->sin_addr), TNIPADDR_NAT);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add new IP to our list!", 0);
				goto DONE;
			} // end if (couldn't add IP)
		} // end if (couldn't find the specified IP address in the list)


		// We can do the same for the slave, too: if we're getting data from a
		// different IP from the ones he knows about, make of a note of it.
		
		pSockAddr = (SOCKADDR_IN*) (*ppvFromAddress);

		if (! (pNewMachine->m_ipaddrs.DoesIPExist(&(pSockAddr->sin_addr))))
		{
			DPL(0, "Slave is connected via a different IP address from ones he knows about (%o).",
				1, pSockAddr);

			hr = this->m_ipaddrs.AddIP(&(pSockAddr->sin_addr), TNIPADDR_NAT);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add NAT IP to slave's list!", 0);
				goto DONE;
			} // end if (couldn't add IP)
		} // end if (couldn't find the actual IP in the slave's known list)

	} // end if (using a TCP/IP control method)


	this->m_slaves.EnterCritSection();
	fHaveSlaveListLock = TRUE;


	// Increase our total slave count
	this->m_iTotalNumSlaves++;


	// Create a dynamic variable for this machine
	wsprintf(szVar, "SLAVENAME%i", this->m_iTotalNumSlaves);
	hr = this->m_builtinvars.AddVariable(szVar, "STRING",
										pNewMachine->m_szComputerName,
										(strlen(pNewMachine->m_szComputerName) + 1));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add variable \"%s\" for slave %s!",
			2, szVar, pNewMachine->m_szComputerName);
		goto DONE;
	} // end if (failed adding a variable)

	// Update the slave count item.  This will replace the previous NUMSLAVES
	// item, if we had one.
	wsprintf(szVar, "%i", this->m_slaves.Count());
	hr = this->m_builtinvars.AddVariable("NUMSLAVES", "STRING",
										szVar, (strlen(szVar) + 1));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't (re)add \"NUMSLAVES\" variable!", 0);
		goto DONE;
	} // end if (failed adding a variable)


	// If that slave wants a timelimit, or if our user wants one for all slaves,
	// then set that limit as appropriate.
	if (pJoinMsg->dwTimelimit != 0)
		pNewMachine->m_dwEndtime = GetTickCount() + (pJoinMsg->dwTimelimit * 60000);
	else if (this->m_dwTimelimit != 0)
		pNewMachine->m_dwEndtime = GetTickCount() + (this->m_dwTimelimit * 60000);

	// If this is the first slave and there's a time limit for additional joiners,
	// start that timer.  The PeriodicCheck thread will do the closing.
	if (this->m_dwAllowJoinersTime != 0)
	{
		if (this->m_iSlaveNumLimit > 1)
		{
			DPL(0, "Will close session in %u seconds, unless %i slaves join first.",
				2, this->m_dwAllowJoinersTime, this->m_iSlaveNumLimit);

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Will close session in %u seconds, unless %i slaves join first.",
					2, this->m_dwAllowJoinersTime, this->m_iSlaveNumLimit);
		} // end if (slave limit)
		else if (this->m_iSlaveNumLimit == 0)
		{
			DPL(0, "Will close session in %u seconds.", 1, this->m_dwAllowJoinersTime);

			this->Log(TNLST_CONTROLLAYER_INFO, "Will close session in %u seconds.",
					1, this->m_dwAllowJoinersTime);
		} // end else (no slave limit)


		// Have the job thread close the session when the time is right.
		hr = this->m_jobs.AddJob(TNMJ_CLOSESESSION, this->m_dwAllowJoinersTime * 1000,
								NULL, NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't readd ping check job!", 0);
			goto DONE;
		} // end if (couldn't re-add job)

		this->m_dwAllowJoinersTime = 0; // reset it so we don't try again
		fCloseSessionLater = TRUE;
	} // end if (there's a join time limit)


	// If there's a slave limit, check if we've reached it.
	if (this->m_iSlaveNumLimit != 0)
	{
		// -1 because it won't actually be in the list until below.
		if (this->m_slaves.Count() == (this->m_iSlaveNumLimit - 1))
		{
			DPL(0, "%s joining triggers the slave limit of %i, will close session.",
				2, pNewMachine->m_szComputerName, this->m_iSlaveNumLimit);

			// We won't actually close the session now.  First, we have to close
			// it in the Job thread so that we don't hold up this receive thread
			// we're in.  Besides being bad form, it can also deadlock because some
			// operations inside CloseSession require communication with slaves,
			// and if the receive thread isn't moving, we'll never get their
			// responses.
			// We don't even add the job to close session until later, so if the
			// Job thread fires really quickly, it won't spew "Closing session"
			// before we've spewed "Slave added" (makes the spew looks more
			// natural).
			fCloseSessionNow = TRUE;
		} // end if (that's the last slave we should accept)
	} // end if (there's a slave number limit)



	// Let's spew some feedback for this join event.

	DPL(1, "Slave %s joined session.", 1, pNewMachine->m_szComputerName);

	this->Log(TNLST_CONTROLLAYER_INFO, "Slave %s joined session.",
				1, pNewMachine->m_szComputerName);



	// Print a note if this slave is another instance on a machine that has already
	// joined the session.  No need to check the last slave because that's the one
	// we just added.
	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pOtherSlave == NULL)
		{
			DPL(0, "Couldn't get existing slave %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get slave)

		if (this->AreOnSameMachine(pOtherSlave, pNewMachine))
		{
			// Make note of the fact that there are multiple instances.
			pOtherSlave->m_iNumOtherInstancesInSession++;
			pNewMachine->m_iNumOtherInstancesInSession++;
		} // end if (they're on the same machine)
	} // end for (each slave)

	if (pNewMachine->m_iNumOtherInstancesInSession > 0)
	{
		DPL(0, "WARNING: There are already %i TestNet slave instances from the same machine as %s in this session!",
			2, pNewMachine->m_iNumOtherInstancesInSession,
			pNewMachine->m_szComputerName);

		this->Log(TNLST_CRITICAL, 
					"WARNING: There %s already %u TestNet slave instance%sfrom the same machine as %s in this session!",
					4, ((pNewMachine->m_iNumOtherInstancesInSession == 1) ? "is" : "are"),
					pNewMachine->m_iNumOtherInstancesInSession,
					((pNewMachine->m_iNumOtherInstancesInSession == 1) ? " " : "s "),
					pNewMachine->m_szComputerName);
	} // end if (multiple instances on same machine)


	// Initialize our record of the last time we heard from this guy so we don't
	// kill him right away...
	hr = pNewMachine->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't initialize slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't init receive time)


	// Add it to the list
	hr = this->m_slaves.Add(pNewMachine);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add new machine to our list!", 0);
		goto DONE;
	} // end if (couldn't add machine to list)



	// Add a work item to remind us to ping, if the user wants pings and this is the
	// first slave in a previously empty session.  It will continually readd a job
	// as long as there are slaves in the session.  It won't be able to do anything
	// until we drop the slave list lock below, though.
	if ((this->m_fPingSlaves) && (this->m_slaves.Count() == 1))
	{
		hr = this->m_jobs.AddJob(TNMJ_PINGSLAVES, SLAVE_CHECK_INTERVAL, NULL, NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add ping check job!", 0);
			goto DONE;
		} // end if (couldn't add job)
	} // end if (we should ping the slaves)



	fHaveSlaveListLock = FALSE;
	this->m_slaves.LeaveCritSection();



	hr = this->m_reports.AddReport(RT_ADDMACHINE,
								1, &pNewMachine,
								NULL, 0, NULL, 0);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is one
	// of the ones we are counting, check to see if that pushed us over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)

	
	// Ping the event to let the app know about the new slave.
	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto DONE;
	} // end if (couldn't set the user's update event)


	// Send a reply to the joining slave.
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, dwFromAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pSendData->m_pvAddress, (*ppvFromAddress), dwFromAddressSize);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_JOINREPLY);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pJoinReplyMsg = (PCTRLMSG_JOINREPLY) pSendData->m_pvData;
	pJoinReplyMsg->dwSize = pSendData->m_dwDataSize;
	pJoinReplyMsg->dwType = CTRLMSGID_JOINREPLY;

	CopyMemory(&(pJoinReplyMsg->id), &(pNewMachine->m_id), sizeof (TNCTRLMACHINEID));

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)
	
	pSendData = NULL; // forget about it so it doesn't get freed below


	// Add a work item to close the session, if we're supposed to now.
	if (fCloseSessionNow)
	{
		// Make sure no one can join between now and the time the job thread
		// actually closes the session.
		this->m_fJoinersAllowed = FALSE;

		hr = this->m_jobs.AddJob(TNMJ_CLOSESESSION, 0, NULL, NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add close session job!", 0);
			goto DONE;
		} // end if (couldn't add job)

		//fCloseSession = FALSE;
	} // end if (should close session)
	else
	{
		// If there's a slave limit but no close time, print how close
		// we are to that limit.
		if ((this->m_iSlaveNumLimit != 0) && (fCloseSessionLater))
		{
			DPL(0, "Will close session when %i more slaves join.",
				1, (this->m_iSlaveNumLimit - this->m_slaves.Count()));

			this->Log(TNLST_CONTROLLAYER_INFO,
					"Will close session when %i more slave%s.",
					2, (this->m_iSlaveNumLimit - this->m_slaves.Count()),
					(((this->m_iSlaveNumLimit - this->m_slaves.Count()) == 1) ? " joins" : "s join"));
		} // end if (there's slave limit and no close time)
	} // end else (shouldn't close session)



	// Update the meta master if we're registered.
	if (this->m_fRegistered)
	{
		hr = this->SendMasterUpdate();
		if (hr != S_OK)
		{
			DPL(0, "Failed to send master update to meta-master!", 0);
			goto DONE;
		} // end if (failed send master update)
	} // end if (we're registered with a metamaster)


DONE:

	if (pNewMachine != NULL)
	{
		pNewMachine->m_dwRefCount--;
		if (pNewMachine->m_dwRefCount == 0)
		{
			DPL(1, "Deleting machine object %x (%s).",
				2, pNewMachine, pNewMachine->m_szComputerName);
			delete (pNewMachine);
			pNewMachine = NULL;
		} // end if (should delete object)
		else
		{
			DPL(7, "Not deleting machine object %x (%s), its refcount is %u.",
				3, pNewMachine, pNewMachine->m_szComputerName,
				pNewMachine->m_dwRefCount);
		} // end else (shouldn't delete object)
	} // end if (have machine object)

	if (fHaveSlaveListLock)
	{
		fHaveSlaveListLock = FALSE;
		this->m_slaves.LeaveCritSection();
	} // end if (still have lock)

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (have a left over send data object)

	return (hr);
} // CTNMaster::HandleJoinMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleReadyToTestMsg()"
//==================================================================================
// CTNMaster::HandleReadyToTestMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session request command message.
//
// Arguments:
//	PVOID* ppvFromAddress					Pointer to pointer to data describing
//											ctrl comm from address.
//	DWORD dwFromAddressSize					Size of from address data.
//	PCTRLMSG_READYTOTEST pReadyToTestMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleReadyToTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_READYTOTEST pReadyToTestMsg)
{
	HRESULT		hr;


	DPL(0, "Got ReadyToTest message.", 0);


	hr = this->m_jobs.AddJob(TNMJ_READYTOTEST, 0, &(pReadyToTestMsg->id), NULL);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add ReadyToTest job for slave ID %u!",
			1, pReadyToTestMsg->id.dwTime);
		//goto DONE;
	} // end if (couldn't add job)

	return (hr);
} // CTNMaster::HandleReadyToTestMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleAnnounceSubTestMsg()"
//==================================================================================
// CTNMaster::HandleAnnounceSubTestMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session announce subtest message.
//
// Arguments:
//	PVOID* ppvFromAddress							Pointer to pointer to data
//													describing ctrl comm from
//													address.
//	DWORD dwFromAddressSize							Size of from address data.
//	PCTRLMSG_ANNOUNCESUBTEST pAnnounceSubTestMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleAnnounceSubTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_ANNOUNCESUBTEST pAnnounceSubTestMsg)
{
	HRESULT				hr = S_OK;
	PTNSLAVEINFO		pSlave = NULL;
	PTNTESTINSTANCEM	pParentTest = NULL;
	DWORD				dwModuleIDSize = 0;
	char*				pszModuleID = NULL;
	TNCTRLMACHINEID*	paTesters = NULL;
	int					iTesterNum = -1;
	int					i;
	int					j;
	PTNTESTINSTANCEM	pSubTest = NULL;
	PTNSLAVEINFO		pParentTester = NULL;
	PTNSLAVEINFO		pExistingSubTester = NULL;



	DPL(3, "Got AnnounceSubTest message.", 0);


	// Take the slave list lock.
	this->m_slaves.EnterCritSection();


	pSlave = this->m_slaves.GetSlaveByID(&(pAnnounceSubTestMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find announcing slave (%u) in list!",
			1, pAnnounceSubTestMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	// Find the test with the specified parent ID
	pParentTest = pSlave->GetTest(pAnnounceSubTestMsg->dwTopLevelUniqueID,
									pAnnounceSubTestMsg->dwParentUniqueID);
	if (pParentTest == NULL)
	{
		DPL(0, "Couldn't get parent test ID %u (under ID %u)!",
			2, pAnnounceSubTestMsg->dwParentUniqueID,
			pAnnounceSubTestMsg->dwTopLevelUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find parent test with given ID)


#pragma TODO(vanceo, "All of this is duplicated in HandleRequestPokeTestMsg")

	dwModuleIDSize = strlen((char*) (pAnnounceSubTestMsg + 1)) + 1;

	// Get the case ID string for the subtest
	pszModuleID = (char*) LocalAlloc(LPTR, dwModuleIDSize);
	if (pszModuleID == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pszModuleID, (pAnnounceSubTestMsg + 1), dwModuleIDSize);

	paTesters = (TNCTRLMACHINEID*) (((LPBYTE) (pAnnounceSubTestMsg + 1)) + dwModuleIDSize);


	// Quick validate the passed in tester array to make sure the announcer isn't
	// trying anything naive, like using an invalid tester, or not including
	// himself.
	for(i = 0; i < pAnnounceSubTestMsg->iNumMachines; i++)
	{
		pParentTester = this->m_slaves.GetSlaveByID(&(paTesters[i]));
		if (pParentTester == NULL)
		{
#pragma BUGBUG(vanceo, "This workaround doesn't work for poke tests")
			// The slave may be dead, but we might recognize him from the parent
			// test.  If so, it means the slave should get a lost tester message
			// regarding that guy.
			for(j = 0; j < pParentTest->m_iNumMachines; j++)
			{
				if (memcmp(&(pParentTest->m_paTesterSlots[j].pSlave->m_id), &(paTesters[i]), sizeof (TNCTRLMACHINEID)) == 0)
				{
					DPL(0, "%s is trying to announce a subtest \"%s\" with a slave (%s, tester %i) who has left!  Assuming LostTester message is on its way.",
						3, pSlave->m_szComputerName, pszModuleID,
						pParentTest->m_paTesterSlots[j].pSlave->m_szComputerName,
						j);

#ifdef DEBUG
					// Double check to make sure the slot looks complete like it
					// should be after a slave dies.
					if (! pParentTest->m_paTesterSlots[j].fComplete)
					{
						DPL(0, "Gone slave %s's slot (%i) in test %u isn't marked as complete!?  DEBUGBREAK()-ing.",
							3, pParentTest->m_paTesterSlots[j].pSlave->m_szComputerName,
							j, pParentTest->m_dwUniqueID);

						DEBUGBREAK();
					} // end if (slot isn't complete
#endif // DEBUG

					goto DONE;
				} // end if (this is the desired tester)
			} // end for (each parent test tester)


			DPL(0, "%s is trying to announce a subtest \"%s\" with an invalid slave ID (%u, index %i)!",
				4, pSlave->m_szComputerName, pszModuleID,
				paTesters[i].dwTime, i);

			this->Log(TNLST_CRITICAL,
					"%s is trying to announce a subtest \"%s\" with an invalid slave ID (%u, index %i)!",
					4, pSlave->m_szComputerName, pszModuleID,
					paTesters[i].dwTime, i);

			hr = E_FAIL;
			goto DONE;
		} // end if (tester is out of range)

		// Check to see if it's the announcer.
		if (pParentTester == pSlave)
		{
			if (iTesterNum >= 0)
			{
				DPL(0, "%s is trying to announce subtest \"%s\" with itself as more than one of the testers (indicees %i and %i)!",
					4, pSlave->m_szComputerName, pszModuleID, iTesterNum, i);

				this->Log(TNLST_CRITICAL,
						"%s is trying to announce subtest \"%s\" with itself as more than one of the testers (indicees %i and %i)!",
						4, pSlave->m_szComputerName, pszModuleID, iTesterNum, i);

				hr = E_FAIL;
				goto DONE;
			} // end if (already found the announcer)

			iTesterNum = i;
		} // end if (it's the announcer)
		else
		{
			j = pParentTest->GetSlavesTesterNum(pParentTester);
			if (j < 0)
			{
				DPL(0, "%s is trying to announce subtest \"%s\" using a slave (%s) who isn't testing the parent test!",
					3, pSlave->m_szComputerName, pszModuleID,
					pParentTester->m_szComputerName);

				this->Log(TNLST_CRITICAL,
						"%s is trying to announce subtest \"%s\" using a slave (%s) who isn't testing the parent test!",
						3, pSlave->m_szComputerName, pszModuleID,
						pParentTester->m_szComputerName);

				hr = E_FAIL;
				goto DONE;
			} // end if (slave isn't working on parent test)

			// Make sure the person hasn't already completed the test.
			if (pParentTest->m_paTesterSlots[j].fComplete)
			{
				DPL(0, "%s is trying to announce subtest \"%s\" using a tester (%i, %s) who already completed the parent test!",
					4, pSlave->m_szComputerName, pszModuleID, j,
					pParentTester->m_szComputerName);

				this->Log(TNLST_CRITICAL,
						"%s is trying to announce subtest \"%s\" using a tester (%i, %s) who already completed the parent test!",
						4, pSlave->m_szComputerName, pszModuleID, j,
						pParentTester->m_szComputerName);

				hr = E_FAIL;
				goto DONE;
			} // end if (parent tester already completed test)
		} // end else (it's not the announcer)
	} // end for (each sub tester)

	if (iTesterNum < 0)
	{
		DPL(0, "%s is trying to announce subtest \"%s\" without itself as one of the testers!",
			2, pSlave->m_szComputerName, pszModuleID);

		this->Log(TNLST_CRITICAL,
				"%s is trying to announce subtest \"%s\" without itself as one of the testers!",
				2, pSlave->m_szComputerName, pszModuleID);

		hr = E_FAIL;
		goto DONE;
	} // end if (the slave did not include itself in the array)


	pSubTest = this->GetActiveTest(pParentTest,
									pszModuleID,
									pAnnounceSubTestMsg->iNumMachines,
									paTesters);

	// If we didn't find one, create it.
	if (pSubTest == NULL)
	{
		PTNTESTTABLECASE	pCase;


		pCase = this->m_testtable.GetTest(pszModuleID);
		if (pCase == NULL)
		{
			DPL(0, "Couldn't find test with module case ID %s!",
				1, pszModuleID);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't find test with given case ID)
		
		if (pCase->m_iNumMachines > 0)
		{
			if (pAnnounceSubTestMsg->iNumMachines != pCase->m_iNumMachines)
			{
				DPL(0, "Announcing slave doesn't list the correct number of testers for case ID %s (he says %i but it requires exactly %i)!",
					3, pszModuleID, pAnnounceSubTestMsg->iNumMachines,
					pCase->m_iNumMachines);
				hr = E_FAIL;
				goto DONE;
			} // end if (announcer is working with the wrong number of machines)
		} // end if (exact number of machines required)
#pragma BUGBUG(vanceo, "Remove if clause when all tests switch to ADDTESTDATA")
		else if (pCase->m_iNumMachines < 0)
		{
			if (pAnnounceSubTestMsg->iNumMachines < (-1 * pCase->m_iNumMachines))
			{
				DPL(0, "Announcing slave doesn't list the correct number of testers for case ID %s (he says %i but it requires at least %i)!",
					3, pszModuleID, pAnnounceSubTestMsg->iNumMachines,
					(-1 * pCase->m_iNumMachines));
				hr = E_FAIL;
				goto DONE;
			} // end if (announcer is working with the wrong number of machines)
		} // end else if (minimum number of machines required)

		pSubTest = new (CTNTestInstanceM)(this->m_dwCurrentUniqueID++,
											NULL,
											pCase,
											pAnnounceSubTestMsg->iNumMachines,
											//0,
											pParentTest);
		if (pSubTest == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)


		DPL(2, "No active subtest \"%s\" (%i machines) found, created test %u for %s.",
			4, pszModuleID, pAnnounceSubTestMsg->iNumMachines,
			pSubTest->m_dwUniqueID, pSlave->m_szComputerName);


		// Loop through each of the testers in the list the slave wants to assign,
		// and attach a placeholder for them to the subtest.
		// We won't notify anyone right now.
		for(i = 0; i < pAnnounceSubTestMsg->iNumMachines; i++)
		{
			// We're not checking the validity, it should have been caught above.
			pParentTester = this->m_slaves.GetSlaveByID(&(paTesters[i]));

			hr = this->AssignSlaveToTest(pParentTester, pSubTest, i, TRUE,
										FALSE, NULL, 0);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't assign placeholder for slave %s (tester %i) to test %s (unique ID %u)!",
					4, pParentTester->m_szComputerName, i,
					pSubTest->m_pCase->m_pszID, pSubTest->m_dwUniqueID);
				goto DONE;
			} // end if (couldn't assign slave to test)
		} // end for (each tester in the list)

		hr = pParentTest->m_subtests.Add(pSubTest);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add test (unique ID %u) to parent test (unique ID %u)!",
				2, pSubTest->m_dwUniqueID, pParentTest->m_dwUniqueID);
			delete (pSubTest);
			pSubTest = NULL;
			goto DONE;
		} // end if (couldn't add the test to the list)
	} // end if (the test doesn't already exist)
	else
	{
		DPL(4, "Active subtest \"%s\" (%i machines) for %s found, ID = %u.",
			4, pszModuleID, pAnnounceSubTestMsg->iNumMachines,
			pSlave->m_szComputerName, pSubTest->m_dwUniqueID);
	} // end else (the test already exists)


	// Make sure the announcer isn't sending something messed up.
	if (pAnnounceSubTestMsg->dwRequestID == 0)
	{
		DPL(0, "%s's announcement request ID is 0!",
			1, pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (no announce ID)


	// Make sure his announcement request ID isn't already filled...
	if (pSubTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID != 0)
	{
		DPL(0, "Announcement request ID already filled with %u (is %s announcing twice?)!",
			2, pSubTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID,
			pSlave->m_szComputerName);
		hr = ERROR_ALREADY_EXISTS;
		goto DONE;
	} // end if (announce ID filled already)



	// Otherwise, store it so it can be echoed back to user when notifying
	// him.
	pSubTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID = pAnnounceSubTestMsg->dwRequestID;


	// Assign a non-placeholder for ourselves.  This may kick off the testing if
	// we're the last ones.
#pragma BUGBUG(vanceo, "Announce subtest data, too? see announcepoke")
	hr = this->AssignSlaveToTest(pSlave,
								pSubTest,
								iTesterNum,
								FALSE,
								FALSE,
								NULL,
								0);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't assign non-placeholder for %s (tester %i) to test %s (unique ID %i)!",
			4, pSlave->m_szComputerName, iTesterNum,
			pSubTest->m_pCase->m_pszID, pSubTest->m_dwUniqueID);
		goto DONE;
	} // end if (couldn't assign slave to test)


DONE:

	if (pszModuleID != NULL)
	{
		LocalFree(pszModuleID);
		pszModuleID = NULL;
	} // end if (we allocated a module ID)

	// Drop the slave list lock.
	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleAnnounceSubTestMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleReportMsg()"
//==================================================================================
// CTNMaster::HandleReportMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session report message.
//
// Arguments:
//	PVOID* ppvFromAddress			Pointer to pointer to data describing ctrl comm
//									from address.
//	DWORD dwFromAddressSize			Size of from address data.
//	PCTRLMSG_REPORT pReportMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleReportMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_REPORT pReportMsg)
{
	HRESULT				hr;
	PVOID				pvReportMsgCopy = NULL;
	PTNSLAVEINFO		pSlave = NULL;


	DPL(9, "==>(%x, %u, %x)",
		3, ppvFromAddress, dwFromAddressSize, pReportMsg);


	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pReportMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find reporting slave!", 0);

		this->m_slaves.LeaveCritSection();

		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find that slave in the list)


	DPL(1, "Got Report message from %s.", 1, pSlave->m_szComputerName);


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)

	this->m_slaves.LeaveCritSection();



	// Copy the info of this message for delayed processing.
	pvReportMsgCopy = LocalAlloc(LPTR, (sizeof (CTRLMSG_REPORT) + pReportMsg->dwOutputDataSize + pReportMsg->dwVarsSize));
	if (pvReportMsgCopy == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	CopyMemory(pvReportMsgCopy, pReportMsg,
				(sizeof (CTRLMSG_REPORT) + pReportMsg->dwOutputDataSize + pReportMsg->dwVarsSize));

	// Pass the handling of this message off to the job thread.
	hr = this->m_jobs.AddJob(TNMJ_HANDLEREPORT, 0, &(pReportMsg->id),
							pvReportMsgCopy);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add HandleReportMsg job!", 0);

		LocalFree(pvReportMsgCopy);
		pvReportMsgCopy = NULL;

		goto DONE;
	} // end if (couldn't add job)

	// We successfully added the job, so forget about the memory we allocated, it will
	// be freed when the job is processed.
	pvReportMsgCopy = NULL;


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::HandleReportMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleWarningMsg()"
//==================================================================================
// CTNMaster::HandleWarningMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session warning message.
//
// Arguments:
//	PVOID* ppvFromAddress			Pointer to pointer to data describing ctrl comm
//									from address.
//	DWORD dwFromAddressSize			Size of from address data.
//	PCTRLMSG_WARNING pWarningMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleWarningMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_WARNING pWarningMsg)
{
	HRESULT				hr;
	PTNTESTINSTANCEM	pTest = NULL;
	PTNSLAVEINFO		pSlave = NULL;
	int					iTesterNum;
	DWORD				dwReportType = RT_WARNING;

	
	//DPL(0, "Got Warning message.", 0);

	// BUGBUG Validate machine ID

	pSlave = this->m_slaves.GetSlaveByID(&(pWarningMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find warning slave!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find that slave in the list)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	pTest = pSlave->GetTest(pWarningMsg->dwTopLevelUniqueID, pWarningMsg->dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get test %u:%u %s is warning for!",
			3, pWarningMsg->dwTopLevelUniqueID, pWarningMsg->dwTestUniqueID,
			pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get test ID)


	DPL(0, "Test with unique ID %u:%u, hr = %x, data size = %u, data = %x.",
		4, pWarningMsg->dwTopLevelUniqueID, pWarningMsg->dwTestUniqueID,
		pWarningMsg->hresult, pWarningMsg->dwUserDataSize,
		((pWarningMsg->dwUserDataSize > 0) ? (pWarningMsg + 1) : NULL));

	this->Log(TNLST_CONTROLLAYER_TESTWARNING,
				"%s warns for test unique ID %u",
				2, pSlave->m_szComputerName,
				pWarningMsg->dwTestUniqueID);
	this->Log(TNLST_CONTROLLAYER_TESTWARNING,
			"Warning code = %e",
			1, pWarningMsg->hresult);


	hr = this->m_reports.AddReport(dwReportType,
								1, &pSlave,
								pTest,
								pWarningMsg->hresult,
								((pWarningMsg->dwUserDataSize > 0) ? (pWarningMsg + 1) : NULL),
								pWarningMsg->dwUserDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is one
	// of the ones we are counting, check to see if that pushed us over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & (TNREPORT_DURINGTESTWARNINGS | TNREPORT_TESTASSIGNMENTSTESTWARNED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTWARNED)))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	iTesterNum = pTest->GetSlavesTesterNum(pSlave);
	if (iTesterNum < 0)
	{
		DPL(0, "Couldn't slave %s's tester number in test %u!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get tester number)


	// Make sure the test is cookin'.
	if (! pTest->m_fStarted)
	{
		DPL(0, "%s is trying to warn with a test (ID %u) which hasn't started yet?!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (the test hasn't started)

	// Make sure the dude hasn't already claimed he finished it.
	if (pTest->m_paTesterSlots[iTesterNum].fComplete)
	{
		DPL(0, "%s is trying to warn during a test (ID %u) he already said he finished?!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (he already finished the test)


	pTest->m_paTesterSlots[iTesterNum].iNumWarnings++;

	this->m_totalstats.IncrementWarnings();
	pTest->m_pCase->m_stats.IncrementWarnings();

	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update stats event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto DONE;
	} // end if (couldn't set the user's update stats event)


DONE:

	return (hr);
} // CTNMaster::HandleWarningMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleRequestSyncMsg()"
//==================================================================================
// CTNMaster::HandleRequestSyncMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session request sync message.
//
// Arguments:
//	PVOID* ppvFromAddress					Pointer to pointer to data describing
//											ctrl comm from address.
//	DWORD dwFromAddressSize					Size of from address data.
//	PCTRLMSG_REQUESTSYNC pRequestSyncMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleRequestSyncMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
										PCTRLMSG_REQUESTSYNC pRequestSyncMsg)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSlave = NULL;
	PTNTESTINSTANCEM		pTest = NULL;
	int						iTesterNum = -1;
	int						i;
	char*					pszSyncName = NULL;
	int*					piTempTesterNum = NULL;
	PTNSLAVEINFO			pTempSlave = NULL;
	CTNSyncDataList			insynclist;
	BOOL					fInSync = FALSE;
	PTNSYNCDATA				pSyncData = NULL;
	CTNSendDataQueue		tempsendqueue;
	DWORD					dwSyncDataBufferSize = 0;
	PTNSENDDATA				pSendData = NULL;
	PCTRLMSG_SYNCREPLY		pSyncReplyMsg = NULL;



	//DPL(0, "Got RequestSync message.", 0);

	pSlave = this->m_slaves.GetSlaveByID(&(pRequestSyncMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Received invalid slave ID!  Ignoring.", 0);
		hr = S_OK;
		goto DONE;
	} // end if (invalid ID)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	pTest = pSlave->GetTest(pRequestSyncMsg->dwTopLevelUniqueID,
							pRequestSyncMsg->dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get test %u:%u %s is syncing in!",
			3, pRequestSyncMsg->dwTopLevelUniqueID, pRequestSyncMsg->dwTestUniqueID,
			pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get test ID)


	iTesterNum = pTest->GetSlavesTesterNum(pSlave);

	// If we didn't find it, then bail.
	if (iTesterNum < 0)
	{
		DPL(0, "Couldn't find the syncing slave (%s) in %i's tester list!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find the syncing tester in the list)


	// Create the test's sync list if it doesn't exist

	if (pTest->m_pSyncDataList == NULL)
	{
		pTest->m_pSyncDataList = new (CTNSyncDataList);
		if (pTest->m_pSyncDataList == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)
	} // end if (the list doesn't exist)

	pszSyncName = (char*) (pRequestSyncMsg + 1);

	piTempTesterNum = (int*) (((LPBYTE) (pRequestSyncMsg + 1)) + pRequestSyncMsg->dwNameSize);
	// Validate the syncers target list (and make sure the target hasn't already
	// completed the test).
	for (i = 0; i < pRequestSyncMsg->iNumMachines; i++)
	{
        //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		if (((*(UNALIGNED int*)piTempTesterNum) < 0) || ((*(UNALIGNED int*)piTempTesterNum) >= pTest->m_iNumMachines))
		{
			DPL(0, "%s (tester %i) is trying to sync with out-of-range tester %i (its <0 or >=%i)!",
				4, pSlave->m_szComputerName, iTesterNum,
				(*(UNALIGNED int*)piTempTesterNum), pTest->m_iNumMachines);

			this->Log(TNLST_CRITICAL,
					"%s (tester %i) is trying to sync with out-of-range tester %i (its <0 or >%i)!",
					4, pSlave->m_szComputerName, iTesterNum,
					(*(UNALIGNED int*)piTempTesterNum), pTest->m_iNumMachines);

			hr = E_FAIL;
			goto DONE;
		} // end if (syncer is out of range)

		pTempSlave = pTest->m_paTesterSlots[(*(UNALIGNED int*)piTempTesterNum)].pSlave;

		if (pTempSlave == NULL)
		{
			DPL(0, "Sync target slot (%i) has not been filled yet.  Continuing.",
				1, (*(UNALIGNED int*)piTempTesterNum));
			//hr = E_FAIL;
			//goto DONE;
			piTempTesterNum++;
			continue;
		} // end if (target is invalid)

		if (pTempSlave == pSlave)
		{
			DPL(0, "%s (tester %i) is trying to sync with self in test %u!",
				3, pSlave->m_szComputerName, iTesterNum,
				pTest->m_dwUniqueID);

			this->Log(TNLST_CRITICAL,
					"%s (tester %i) is trying to sync with self in test %u!",
					3, pSlave->m_szComputerName, iTesterNum,
					pTest->m_dwUniqueID);

			hr = E_FAIL;
			goto DONE;
		} // end if (trying to sync with self)


		// If this slave already finished the test for this sync, then syncs
		// obviously can't be triggered, so send a tester lost message.
        //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		if (pTest->m_paTesterSlots[(*(UNALIGNED int*)piTempTesterNum)].fComplete)
		{
			DPL(0, "%s (tester %i) has already completed test %u, aborting %s (tester %i)'s sync.",
				5, pTempSlave->m_szComputerName,
				((int*) (pRequestSyncMsg + 1))[i],
				pTest->m_dwUniqueID,
				pSlave->m_szComputerName,
				iTesterNum);

			hr = this->SendLostTesterTo(pSlave, pTest, (*(UNALIGNED int*)piTempTesterNum));
			if (hr != S_OK)
			{
				if (hr != TNERR_CONNECTIONDROPPED)
				{
					DPL(0, "Couldn't send lost tester to %s!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (not connectiondropped)
				else
				{
					DPL(0, "WARNING: Not sending LostTester message to %s because he's gone!",
						1, pSlave->m_szComputerName);
					hr = S_OK;
				} // end else (connection dropped)
			} // end if (couldn't send lost tester message)

			goto DONE;
		} // end if (the target already finished the test)

		piTempTesterNum++;
	} // end for (each sync target)


	hr = pTest->m_pSyncDataList->AddAndDoSyncCheck(pRequestSyncMsg->dwRequestID,
													pszSyncName,
													iTesterNum,
													pRequestSyncMsg->iNumMachines,
													(int*) (((LPBYTE) (pRequestSyncMsg + 1))
																	+ pRequestSyncMsg->dwNameSize),
													((pRequestSyncMsg->dwDataSize == 0) ? NULL :
														(((LPBYTE) (pRequestSyncMsg + 1))
															+ pRequestSyncMsg->dwNameSize
															+ pRequestSyncMsg->iNumMachines * sizeof (int))),
													pRequestSyncMsg->dwDataSize,
													&insynclist,
													&fInSync);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add %s (tester %i) to test %u's \"%s\" sync list!",
			4, pSlave->m_szComputerName, iTesterNum,
			pTest->m_dwUniqueID, pszSyncName);
		goto DONE;
	} // end if (couldn't add syncer to list)


	// If adding him did not change anybody's sync status (i.e. release them
	// from the sync block), then we're done.

	if (! fInSync)
	{
		DPL(8, "Adding %s (tester %u) to test %u's \"%s\" sync has not released everyone yet, continuing to wait.",
			4, pSlave->m_szComputerName, iTesterNum,
			pTest->m_dwUniqueID, pszSyncName);
		goto DONE;
	} // end if (not in sync yet)


	// Loop through everyone who became in sync and notify them.

	for(i = 0; i < insynclist.Count(); i++)
	{
		pSyncData = (PTNSYNCDATA) insynclist.GetItem(i);
		if (pSyncData == NULL)
		{
			DPL(0, "Couldn't get in-sync person %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)


		pSlave = pTest->m_paTesterSlots[pSyncData->m_iTesterNum].pSlave;
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't get now in-sync tester %i!", 1, pSyncData->m_iTesterNum);
			hr = E_FAIL;
			goto DONE;
		} // end if (tester number isn't valid)


		DPL(5, "Releasing %s (tester %i) from sync \"%s\" in test %u.",
			4, pSlave->m_szComputerName, pSyncData->m_iTesterNum,
			pSyncData->m_pszSyncName, pTest->m_dwUniqueID);


		if (pSyncData->m_dwRequestID == 0)
		{
			DPL(0, "No request ID given for item (%s's sync \"%s\" in test %u)!",
				3, pSlave->m_szComputerName, pSyncData->m_pszSyncName,
				pTest->m_dwUniqueID);
			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (no request ID)


		// Build the "release" message to this person, it's buffered so we won't
		// actually send it right away.

		pSendData = new (CTNSendData);
		if (pSendData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (out of memory)

		pSendData->m_fGuaranteed = TRUE;
		pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
		pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
		if (pSendData->m_pvAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (out of memory)
		CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
				pSendData->m_dwAddressSize);

		// ignore error
		insynclist.PackDataForTesterIntoBuffer(pSyncData->m_iTesterNum,
												NULL, &dwSyncDataBufferSize);

		pSendData->m_dwDataSize = sizeof (CTRLMSG_SYNCREPLY)
								+ dwSyncDataBufferSize;

		pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
		if (pSendData->m_pvData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		pSyncReplyMsg = (PCTRLMSG_SYNCREPLY) pSendData->m_pvData;
		pSyncReplyMsg->dwSize = pSendData->m_dwDataSize;
		pSyncReplyMsg->dwType = CTRLMSGID_SYNCREPLY;
		pSyncReplyMsg->dwResponseID = pSyncData->m_dwRequestID;
		pSyncReplyMsg->dwDataSize = dwSyncDataBufferSize;

		if (dwSyncDataBufferSize > 0)
		{
			hr = insynclist.PackDataForTesterIntoBuffer(pSyncData->m_iTesterNum,
														(pSyncReplyMsg + 1),
														&dwSyncDataBufferSize);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't pack sync data list for tester %i into buffer!",
					1, pSyncData->m_iTesterNum);
				goto DONE;
			} // end if (failed to pack sync data list into buffer)
		} // end if (there's actually sync data)
		

		// Add the item to our temporary send queue.  We don't add it to
		// the real one because if it went out before we had a chance to
		// reset the synchronization, the slave might continue on with
		// the test, try to sync again, and we might get goofy because
		// we'd have stale data.  So hold off on sending the items until
		// everybody is ready.

		hr = tempsendqueue.Add(pSendData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add send item to temporary queue!", 0);
			goto DONE;
		} // end if (failed to add item to queue)

		pSendData = NULL; // forget about it so it doesn't get freed below

	} // end for (each person now in sync)


	// Go through the loop again and clear out the syncing data for everyone
	// who became in sync so they are back to a normal state, and can sync
	// again.
	i = 0;
	do
	{
		pSyncData = (PTNSYNCDATA) insynclist.PopFirstItem();
		if (pSyncData == NULL)
		{
			DPL(0, "Couldn't pop in-sync person %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't pop item)

		// Pull it off the test's master sync list
		hr = pTest->m_pSyncDataList->RemoveFirstReference(pSyncData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove in-sync person %i from test's master sync list!",
				1, i);
			goto DONE;
		} // end if (couldn't remove item)

		// We don't need his list of schmoes he's syncing with anymore,
		// so dump it.
		delete (pSyncData->m_pSyncList);
		pSyncData->m_pSyncList = NULL;


		// Delete the object, if it's time.
		pSyncData->m_dwRefCount--;
		if (pSyncData->m_dwRefCount == 0)
		{
			DPL(7, "Deleting sync data object %x.", 1, pSyncData);

			delete (pSyncData);
			pSyncData = NULL;
		} // end if (can remove item)
		else
		{
			DPL(7, "Not deleting sync data object %x, it's refcount is %u.",
				2, pSyncData, pSyncData->m_dwRefCount);
		} // end else (can't remove item)

		i++;
	} // end do (while there are items remaining)
	while (insynclist.Count() > 0);


	// If there aren't any more people trying to sync in this test, then we can
	// free up the list.
	if (pTest->m_pSyncDataList->Count() <= 0)
	{
		delete (pTest->m_pSyncDataList);
		pTest->m_pSyncDataList = NULL;
	} // end if (there aren't any syncs pending)



	// Finally transfer everything from our temp send queue to the real one.
	while(tempsendqueue.Count() > 0)
	{
		pSendData = (PTNSENDDATA) tempsendqueue.PopFirstItem();
		if (pSendData == NULL)
		{
			DPL(0, "Couldn't pop first item from temp send queue!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't pop that item)

		hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add send item to real send queue!", 0);
			goto DONE;
		} // end if (couldn't get that item)

		pSendData->m_dwRefCount--;
		if (pSendData->m_dwRefCount == 0)
		{
			DPL(0, "Deleting send data object %x!?",
				1, pSendData);

			delete (pSendData);
		} // end if (have to delete object)
		pSendData = NULL; // forget about it so we don't free it below
	} // end while (there are still items in the temporary queue)


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (there's send data)

	return (hr);
} // CTNMaster::HandleRequestSyncMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleUserQueryMsg()"
//==================================================================================
// CTNMaster::HandleUserQueryMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session user query message.
//
// Arguments:
//	PVOID* ppvFromAddress				Pointer to pointer to data describing ctrl
//										comm from address.
//	DWORD dwFromAddressSize				Size of from address data.
//	PCTRLMSG_USERQUERY pUserQueryMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleUserQueryMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
									PCTRLMSG_USERQUERY pUserQueryMsg)
{
	HRESULT					hr = S_OK;
	PTNSENDDATA				pSendData = NULL;
	PCTRLMSG_USERRESPONSE	pUserResponseMsg = NULL;
	DWORD					dwResponse = 0;
	DWORD					dwResponseDataSize = 0;


	//DPL(0, "Got UserQuery message.", 0);

	if (this->m_pfnHandleUserQuery == NULL)
	{
		DPL(0, "UserQuery received but the master didn't specify a handler!  Ignoring.", 0);
		hr = S_OK;
		goto DONE;
	} // end if (the user doesn't have UserRequest handler)

	hr = this->m_pfnHandleUserQuery(pUserQueryMsg->dwQuery,
								(pUserQueryMsg + 1), pUserQueryMsg->dwDataSize,
								&dwResponse, NULL, &dwResponseDataSize);
	if (hr != S_OK)
	{
		DPL(0, "App's HandleUserRequestProc with no response data buffer failed!", 0);
		goto DONE;
	} // end if (app's proc failed)


	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_USERRESPONSE)
							+ dwResponseDataSize;

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pUserResponseMsg = (PCTRLMSG_USERRESPONSE) pSendData->m_pvData;
	pUserResponseMsg->dwSize = pSendData->m_dwDataSize;
	pUserResponseMsg->dwType = CTRLMSGID_USERRESPONSE;

	pUserResponseMsg->dwResponse = dwResponse;
	pUserResponseMsg->dwDataSize = dwResponseDataSize;
	if (dwResponseDataSize > 0)
	{
		hr = this->m_pfnHandleUserQuery(pUserQueryMsg->dwQuery,
									(pUserQueryMsg + 1), pUserQueryMsg->dwDataSize,
									&dwResponse, (pUserResponseMsg + 1),
									&dwResponseDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Module's HandleUserQueryProc function failed!", 0);
			goto DONE;
		} // end if (module's proc failed)
	} // end if (there's actually extra response data)

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNMaster::HandleUserQueryMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


	



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleIdleMsg()"
//==================================================================================
// CTNMaster::HandleIdleMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session idle message.
//
// Arguments:
//	PVOID* ppvFromAddress		Pointer to pointer to data describing ctrl comm from
//								address.
//	DWORD dwFromAddressSize		Size of from address data.
//	PCTRLMSG_IDLE pIdleMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleIdleMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
								PCTRLMSG_IDLE pIdleMsg)
{
	HRESULT				hr = S_OK;
	PTNSLAVEINFO		pSlave = NULL;
	PTNSENDDATA			pSendData = NULL;
	PCTRLMSG_IDLEREPLY	pIdleReplyMsg = NULL;
	DWORD				dwReturnStatus = 0;


	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pIdleMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Received invalid slave ID!  Ignoring.", 0);
		hr = S_OK;
		goto DONE;
	} // end if (invalid ID)


	DPL(1, "Got Idle message from %s.", 1, pSlave->m_szComputerName);


	// Validate what the slave is saying
	switch (pIdleMsg->dwWaitingOp)
	{
		case CTRLIDLE_NOTHING:
		case CTRLIDLE_ANNOUNCESUBTEST:
		case CTRLIDLE_SYNC:

#pragma BUGBUG(vanceo, "Verify if he is in the state indicated.")

			dwReturnStatus = CTRLIDLEREPLY_WAITINGFOROTHERTESTERS;
		  break;

		default:
			DPL(0, "Slave %s is reporting an unrecognized op %u!",
				2, pSlave->m_szComputerName, pIdleMsg->dwWaitingOp);
			goto DONE;
		  break;
	} // end switch (on the op the slave says he's performing)


	// We're in sync, so update his time.
	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)



	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_IDLEREPLY);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pIdleReplyMsg = (PCTRLMSG_IDLEREPLY) pSendData->m_pvData;
	pIdleReplyMsg->dwSize = pSendData->m_dwDataSize;
	pIdleReplyMsg->dwType = CTRLMSGID_IDLEREPLY;
	pIdleReplyMsg->dwStatus = dwReturnStatus;

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated a send data object)

	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleIdleMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleRegisterMasterReplyMsg()"
//==================================================================================
// CTNMaster::HandleRegisterMasterReplyMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session register master reply message.
//
// Arguments:
//	PVOID* ppvFromAddress									Pointer to pointer to
//															data describing ctrl
//															comm from address.
//	DWORD dwFromAddressSize									Size of from address
//															data.
//	PCTRLMSG_REGISTERMASTERREPLY pRegisterMasterReplyMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleRegisterMasterReplyMsg(PVOID* ppvFromAddress,
												DWORD dwFromAddressSize,
												PCTRLMSG_REGISTERMASTERREPLY pRegisterMasterReplyMsg)
{
	DPL(0, "Got RegisterMasterReply message.", 0);

	if (this->m_fRegistered)
	{
		DPL(0, "Received registration reply but we're already registered.  Ignoring.", 0);
		return (S_OK);
	} // end if (we're already register)

	this->m_fRegistered = TRUE;
	CopyMemory(&(this->m_id), &(pRegisterMasterReplyMsg->id), sizeof (TNCTRLMACHINEID));

	return (S_OK);
} // CTNMaster::HandleRegisterMasterReplyMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleMasterPingReplyMsg()"
//==================================================================================
// CTNMaster::HandleRegisterMasterPingReplyMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a master ping reply message.
//
// Arguments:
//	PVOID* ppvFromAddress							Pointer to pointer to data
//													describing ctrl comm from
//													address.
//	DWORD dwFromAddressSize							Size of from address data.
//	PCTRLMSG_MASTERPINGREPLYMSG pMasterReplyMsg		Reply message from the slave.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleMasterPingReplyMsg(PVOID* ppvFromAddress,
											DWORD dwFromAddressSize,
											PCTRLMSG_MASTERPINGSLAVEREPLY pMasterReplyMsg)
{
	HRESULT			hr = S_OK;
	PTNSLAVEINFO	pSlave = NULL;
	

	DPL(8, "Got master reply ping message.", 0);


	pSlave = this->m_slaves.GetSlaveByID(&pMasterReplyMsg->id);
	if (pSlave == NULL)
	{
		DPL(0, "Received invalid slave ID!  Ignoring.", 0);
		goto DONE;
	} // end if (couldn't find slave)

	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


DONE:

	return hr;
} // CTNMaster::HandleMasterPingReplyMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleRequestPokeTestMsg()"
//==================================================================================
// CTNMaster::HandleRequestPokeTestMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session request poke test message.
//
// Arguments:
//	PVOID* ppvFromAddress							Pointer to pointer to data
//													describing ctrl comm from
//													address.
//	DWORD dwFromAddressSize							Size of from address data.
//	PCTRLMSG_REQUESTPOKETEST pRequestPokeTestMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleRequestPokeTestMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_REQUESTPOKETEST pRequestPokeTestMsg)
{
	HRESULT				hr = S_OK;
	PTNSLAVEINFO		pSlave = NULL;
	PTNSLAVEINFO		pOtherSlave = NULL;
	PTNSLAVEINFO		pExistingTester = NULL;
	PTNTESTINSTANCEM	pTest = NULL;
	DWORD				dwModuleIDSize = 0;
	LPBYTE				lpCurrent;
	char*				pszModuleID = NULL;
	PTNCTRLMACHINEID	paTesterArray = NULL;
	PVOID				pvInputData;
	int					iTesterNum = -1;
	int					i;


	DPL(3, "Got RequestPokeTest message.", 0);

	// Take the slave list lock.
	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pRequestPokeTestMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find requesting slave (%u) in list!",
			1, pRequestPokeTestMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


#pragma TODO(vanceo, "Most of this is duplicated in HandleAnnounceSubTestMsg")

	lpCurrent = (LPBYTE) (pRequestPokeTestMsg + 1);

	dwModuleIDSize = strlen((char*) lpCurrent) + 1;

	// Get the case ID string for the subtest
	pszModuleID = (char*) LocalAlloc(LPTR, dwModuleIDSize);
	if (pszModuleID == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	CopyAndMoveSrcPointer(pszModuleID, lpCurrent, dwModuleIDSize);

	paTesterArray = (PTNCTRLMACHINEID) lpCurrent;

	if (pRequestPokeTestMsg->dwInputDataSize > 0)
	{
		lpCurrent += sizeof (TNCTRLMACHINEID) * pRequestPokeTestMsg->iNumMachines;
		pvInputData = lpCurrent;
	} // end if (there's input data)
	else
	{
		pvInputData = NULL;
	} // end else (there's no input data)


	// Quick validate the passed in tester array to make sure the announcer isn't
	// trying anything naive, like using an invalid tester, or not including
	// himself.
	for(i = 0; i < pRequestPokeTestMsg->iNumMachines; i++)
	{
		pOtherSlave = this->m_slaves.GetSlaveByID(&(paTesterArray[i]));
		if (pOtherSlave == NULL)
		{
			DPL(0, "%s is trying to request a poke test \"%s\" with an invalid slave ID (%u, index %i)!",
				4, pSlave->m_szComputerName, pszModuleID,
				paTesterArray[i].dwTime, i);

			this->Log(TNLST_CRITICAL,
					"%s is trying to request a poke test \"%s\" with an invalid slave ID (%u, index %i)!",
				4, pSlave->m_szComputerName, pszModuleID,
				paTesterArray[i].dwTime, i);

			hr = E_FAIL;
			goto DONE;
		} // end if (tester is out of range)

		// Check to see if it's the requester.
		if (pOtherSlave == pSlave)
		{
			if (iTesterNum >= 0)
			{
				DPL(0, "%s is trying to request a poke test \"%s\" with itself as more than one of the testers (indices %i and %i)!",
					4, pSlave->m_szComputerName, pszModuleID, iTesterNum, i);

				this->Log(TNLST_CRITICAL,
						"%s is trying to request a poke test \"%s\" with itself as more than one of the testers (indices %i and %i)!",
						4, pSlave->m_szComputerName, pszModuleID, iTesterNum, i);

				hr = E_FAIL;
				goto DONE;
			} // end if (already found the requester)

			iTesterNum = i;
		} // end if (it's the requester)
	} // end for (each tester)

	if (iTesterNum < 0)
	{
		DPL(0, "%s is trying to request a poke test \"%s\" without itself as one of the testers!",
			2, pSlave->m_szComputerName, pszModuleID);

		this->Log(TNLST_CRITICAL,
				"%s is trying to request a poke test \"%s\" without itself as one of the testers!",
				2, pSlave->m_szComputerName, pszModuleID);

		hr = E_FAIL;
		goto DONE;
	} // end if (the slave did not include itself in the array)


	pTest = this->GetActiveTest(NULL,
								pszModuleID,
								pRequestPokeTestMsg->iNumMachines,
								paTesterArray);

	// If we didn't find one, create it.
	if (pTest == NULL)
	{
		PTNTESTTABLECASE	pCase;


		pCase = this->m_testtable.GetTest(pszModuleID);
		if (pCase == NULL)
		{
			DPL(0, "Couldn't find test with module case ID %s!",
				1, pszModuleID);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't find test with given case ID)
		
		if (pCase->m_iNumMachines > 0)
		{
			if (pRequestPokeTestMsg->iNumMachines != pCase->m_iNumMachines)
			{
				DPL(0, "Requesting slave doesn't list the correct number of testers for case ID %s (he says %i but it requires exactly %i)!",
					3, pszModuleID, pRequestPokeTestMsg->iNumMachines,
					pCase->m_iNumMachines);
				hr = E_FAIL;
				goto DONE;
			} // end if (requestor is working with the wrong number of machines)
		} // end if (exact number of machines required)
#pragma BUGBUG(vanceo, "Remove if clause when all tests switch to ADDTESTDATA")
		else if (pCase->m_iNumMachines < 0)
		{
			if (pRequestPokeTestMsg->iNumMachines < (-1 * pCase->m_iNumMachines))
			{
				DPL(0, "Requesting slave doesn't list the correct number of testers for case ID %s (he says %i but it requires at least %i)!",
					3, pszModuleID, pRequestPokeTestMsg->iNumMachines,
					(-1 * pCase->m_iNumMachines));
				hr = E_FAIL;
				goto DONE;
			} // end if (requestor is working with the wrong number of machines)
		} // end else if (minimum number of machines required)


		pTest = new (CTNTestInstanceM)(this->m_dwCurrentUniqueID++,
										NULL,
										pCase,
										pRequestPokeTestMsg->iNumMachines,
										//0,
										NULL);
		if (pTest == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)


		DPL(2, "No active test \"%s\" (%i machines) found, created test %u for %s.",
			4, pszModuleID, pRequestPokeTestMsg->iNumMachines,
			pTest->m_dwUniqueID, pSlave->m_szComputerName);


		// Loop through each of the testers in the list the slave wants to assign, and
		// attach a placeholder for them to the poke test.
		// We won't notify anyone right now.
		for(i = 0; i < pRequestPokeTestMsg->iNumMachines; i++)
		{
			// Not checking validity, it should have been caught above.
			pOtherSlave = this->m_slaves.GetSlaveByID(&(paTesterArray[i]));

			hr = this->AssignSlaveToTest(pOtherSlave, pTest, i,
										TRUE, FALSE, NULL, 0);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't assign slave %s (tester %i) to test %s (unique ID %i)!",
					4, pOtherSlave->m_szComputerName, i,
					pTest->m_pCase->m_pszID, pTest->m_dwUniqueID);
				goto DONE;
			} // end if (couldn't assign slave to test)
		} // end for (each tester in the list)
	} // end if (the test doesn't already exist)
	else
	{
		DPL(2, "Active test \"%s\" (%i machines) for %s found, ID = %u.",
			4, pszModuleID, pRequestPokeTestMsg->iNumMachines,
			pSlave->m_szComputerName, pTest->m_dwUniqueID);
	} // end else (the test already exists)


#ifdef DEBUG
	if (pSlave->m_pCurrentTest != NULL)
	{
		DPL(0, "Slave %s already has a current test (unique ID %u)!",
			2, pSlave->m_szComputerName, pSlave->m_pCurrentTest->m_dwUniqueID);
		DEBUGBREAK();
	} // end if (slave has a current test)
#endif // DEBUG

	pTest->m_dwRefCount++;
	pSlave->m_pCurrentTest = pTest;


	// Make sure his request ID isn't already filled...
	if (pTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID != 0)
	{
		DPL(0, "Announcement request ID already filled with %u (is %s requesting poke test twice?)!",
			2, pTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID,
			pSlave->m_szComputerName);
		hr = ERROR_ALREADY_EXISTS;
		goto DONE;
	} // end if (announce ID filled already)

	// Otherwise, store it so it can be echoed back to user when notifying
	// him.
	pTest->m_paTesterSlots[iTesterNum].dwAnnounceRequestID = pRequestPokeTestMsg->dwRequestID;



	// Assign a non-placeholder for ourselves.  This may kick off the testing if
	// we're the last ones.
#pragma BUGBUG(vanceo, "See announcesubtest")
	hr = this->AssignSlaveToTest(pSlave,
								pTest,
								iTesterNum,
								FALSE,
								FALSE,
								pvInputData,
								pRequestPokeTestMsg->dwInputDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't assign non-placeholder for %s (tester %i) to test %s (unique ID %i)!",
			4, pSlave->m_szComputerName, iTesterNum,
			pTest->m_pCase->m_pszID, pTest->m_dwUniqueID);
		goto DONE;
	} // end if (couldn't assign slave to test)



DONE:

	if (pszModuleID != NULL)
	{
		LocalFree(pszModuleID);
		pszModuleID = NULL;
	} // end if (we allocated a module ID)

	// Drop the slave list lock.
	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleRequestPokeTestMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleAcceptReachCheckReplyMsg()"
//==================================================================================
// CTNMaster::HandleAcceptReachCheckReplyMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session accept reach check reply message.
//
// Arguments:
//	PVOID* ppvFromAddress						Pointer to pointer to data
//												describing ctrl comm from address.
//	DWORD dwFromAddressSize						Size of from address data.
//	PCTRLMSG_ACCEPTREACHCHECKREPLY pReplyMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleAcceptReachCheckReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_ACCEPTREACHCHECKREPLY pReplyMsg)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSlave = NULL;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	PTNREACHCHECK			pReachCheck = NULL;


	DPL(1, "Got AcceptReachCheckReply message (ID %u, fReady = %B, hresult = %x).",
		3, pReplyMsg->dwID, pReplyMsg->fReady, pReplyMsg->hresult);


	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pReplyMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find replying slave (%u) in list!",
			1, pReplyMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	pReachCheckTarget = pSlave->m_reachchecktargets.GetTarget(&(pSlave->m_id));
	if (pReachCheckTarget == NULL)
	{
		DPL(0, "Couldn't get list of self targets (i.e. no outstanding accepts) for replying slave %s!",
			1, pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get target)

	pReachCheck = pReachCheckTarget->m_checks.GetCheck(pReplyMsg->dwID);
	if (pReachCheckTarget == NULL)
	{
		DPL(0, "Couldn't get reach check ID %u for replying slave %s!",
			2, pReplyMsg->dwID, pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get check)

	pReachCheck->m_fResult = pReplyMsg->fReady;
	pReachCheck->m_hresult = pReplyMsg->hresult;

	if (! SetEvent(pReachCheck->m_hEvent))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set reach check event %x!", 1, pReachCheck->m_hEvent);
		goto DONE;
	} // end if (couldn't set event)


DONE:

	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleAcceptReachCheckReplyMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleConnectReachCheckReplyMsg()"
//==================================================================================
// CTNMaster::HandleConnectReachCheckReplyMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session connect reach check reply message.
//
// Arguments:
//	PVOID* ppvFromAddress						Pointer to pointer to data
//												describing ctrl comm from address.
//	DWORD dwFromAddressSize						Size of from address data.
//	PCTRLMSG_CONNECTREACHCHECKREPLY pReplyMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleConnectReachCheckReplyMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
												PCTRLMSG_CONNECTREACHCHECKREPLY pReplyMsg)
{
	HRESULT					hr = S_OK;
	PTNSLAVEINFO			pSlave = NULL;
	int						i;
	PTNREACHCHECKTARGET		pReachCheckTarget = NULL;
	PTNREACHCHECK			pReachCheck = NULL;


	DPL(1, "Got ConnectReachCheckReply message (ID %u, fReachable = %B, hresult = %x).",
		3, pReplyMsg->dwID, pReplyMsg->fReachable, pReplyMsg->hresult);


	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pReplyMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find replying slave (%u) in list!",
			1, pReplyMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)


	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	for(i = 0; i < pSlave->m_reachchecktargets.Count(); i++)
	{
		pReachCheckTarget = (PTNREACHCHECKTARGET) pSlave->m_reachchecktargets.GetItem(i);
		if (pReachCheckTarget == NULL)
		{
			DPL(0, "Couldn't get %s's reach check target %i!",
				2, pSlave->m_szComputerName, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// Skip the self accepts list, if this is it.
		if (memcmp(&(pReachCheckTarget->m_id), &(pSlave->m_id), sizeof (TNCTRLMACHINEID)) == 0)
			continue;

		pReachCheck = pReachCheckTarget->m_checks.GetCheck(pReplyMsg->dwID);
		if (pReachCheck != NULL)
		{
			pReachCheck->m_fResult = pReplyMsg->fReachable;
			pReachCheck->m_hresult = pReplyMsg->hresult;

			if (! SetEvent(pReachCheck->m_hEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't set reach check event %x!",
					1, pReachCheck->m_hEvent);
			} // end if (couldn't set event)

			goto DONE;
		} // end if (found check)
	} // end for (each target)


	// If we got here, it means we couldn't find the given check.

	DPL(0, "Couldn't get reach check ID %u for replying slave %s!",
		2, pReplyMsg->dwID, pSlave->m_szComputerName);
	hr = E_FAIL;


DONE:

	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleConnectReachCheckReplyMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleFreeOutputVarsMsg()"
//==================================================================================
// CTNMaster::HandleFreeOutputVarsMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session free output vars message.
//
// Arguments:
//	PVOID* ppvFromAddress						Pointer to pointer to data
//												describing ctrl comm from address.
//	DWORD dwFromAddressSize						Size of from address data.
//	PCTRLMSG_FREEOUTPUTVARS pFreeOutputVarsMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleFreeOutputVarsMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_FREEOUTPUTVARS pFreeOutputVarsMsg)
{
	HRESULT			hr = S_OK;
	PTNSLAVEINFO	pSlave = NULL;
	LPBYTE			lpCurrent;
	char*			pszCaseID = NULL;
	char*			pszInstanceID = NULL;
	char*			pszName = NULL;
	char*			pszType = NULL;


	DPL(3, "Got FreeOutputVars message.", 0);


	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pFreeOutputVarsMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find freeing slave (%u) in list!",
			1, pFreeOutputVarsMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)

	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


	lpCurrent = (LPBYTE) (pFreeOutputVarsMsg + 1);

	if (pFreeOutputVarsMsg->dwCaseIDSize > 0)
	{
		pszCaseID = (char*) lpCurrent;
		lpCurrent += pFreeOutputVarsMsg->dwCaseIDSize;
	} // end if (there's a case ID)

	if (pFreeOutputVarsMsg->dwInstanceIDSize > 0)
	{
		pszInstanceID = (char*) lpCurrent;
		lpCurrent += pFreeOutputVarsMsg->dwInstanceIDSize;
	} // end if (there's an instance ID)

	if (pFreeOutputVarsMsg->dwNameSize > 0)
	{
		pszName = (char*) lpCurrent;
		lpCurrent += pFreeOutputVarsMsg->dwNameSize;
	} // end if (there's a name)

	if (pFreeOutputVarsMsg->dwTypeSize > 0)
	{
		pszType = (char*) lpCurrent;
		lpCurrent += pFreeOutputVarsMsg->dwTypeSize;
	} // end if (there's a type)


	hr = this->FreeOutputVarsAndData(pszCaseID, pszInstanceID, pSlave, pszName,
									pszType);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't free %s's output variables and data!", 0);
		goto DONE;
	} // end if (couldn't free data)


DONE:

	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleFreeOutputVarsMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::HandleGetMachineInfoMsg()"
//==================================================================================
// CTNMaster::HandleGetMachineInfoMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a control session GetMachineInfo message.
//
// Arguments:
//	PVOID* ppvFromAddress						Pointer to pointer to data
//												describing ctrl comm from address.
//	DWORD dwFromAddressSize						Size of from address data.
//	PCTRLMSG_GETMACHINEINFO pGetMachineInfoMsg	The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::HandleGetMachineInfoMsg(PVOID* ppvFromAddress, DWORD dwFromAddressSize,
											PCTRLMSG_GETMACHINEINFO pGetMachineInfoMsg)
{
	HRESULT							hr;
	PTNSLAVEINFO					pSlave = NULL;
	PTNSLAVEINFO					pInfoSlave = NULL;
	DWORD							dwMachineInfoSize = 0;
	PTNSENDDATA						pSendData = NULL;
	PCTRLMSG_GETMACHINEINFOREPLY	pReplyMsg;



	DPL(3, "Got GetMachineInfo message.", 0);

	this->m_slaves.EnterCritSection();

	pSlave = this->m_slaves.GetSlaveByID(&(pGetMachineInfoMsg->id));
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find requesting slave (%u) in list!",
			1, pGetMachineInfoMsg->id.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)

	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)



	// Find the slave whose info he's trying to retrieve.
	pInfoSlave = this->m_slaves.GetSlaveByID(&(pGetMachineInfoMsg->idMachineForInfo));
	if (pInfoSlave == NULL)
	{
		DPL(0, "Couldn't find slave (%u) in list to retrieve info!",
			1, pGetMachineInfoMsg->idMachineForInfo.dwTime);

		// BUGBUG Fail?
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get slaveinfo object)


	// Ignore error, assume ERROR_BUFFER_TOO_SMALL
	pInfoSlave->PackIntoBuffer(NULL, &dwMachineInfoSize);



	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = dwFromAddressSize;
	pSendData->m_pvAddress = (*ppvFromAddress);

	(*ppvFromAddress) = NULL; // forget about it so it doesn't get freed

	pSendData->m_dwDataSize = sizeof (CTRLMSG_GETMACHINEINFOREPLY)
							+ dwMachineInfoSize;

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pReplyMsg = (PCTRLMSG_GETMACHINEINFOREPLY) pSendData->m_pvData;
	pReplyMsg->dwSize = pSendData->m_dwDataSize;
	pReplyMsg->dwType = CTRLMSGID_GETMACHINEINFOREPLY;
	pReplyMsg->dwResponseID = pGetMachineInfoMsg->dwRequestID; // echo back
	pReplyMsg->dwMachineInfoSize = dwMachineInfoSize;

	// Now pack the machine info into the message buffer.
	hr = pInfoSlave->PackIntoBuffer((pReplyMsg + 1), &dwMachineInfoSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack machine info into message buffer!", 0);
		goto DONE;
	} // end if (couldn't pack info into buffer)


	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated a send data object)

	this->m_slaves.LeaveCritSection();

	return (hr);
} // CTNMaster::HandleGetMachineInfoMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetNextCmdForMachine()"
//==================================================================================
// CTNMaster::GetNextCmdForMachine
//----------------------------------------------------------------------------------
//
// Description: Gets the next test to run or command to process for the specified
//				machine.  Any notification necessary is done within this function.
//				The slave list lock is assumed to be held with only one level of
//				recursion.
//
// Arguments:
//	PTNSLAVEINFO pSlave		Machine to check command/status.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::GetNextCmdForMachine(PTNSLAVEINFO pSlave)
{
	HRESULT				hr = S_OK;
	int					i;
	BOOL				fActiveTest = FALSE;
	PTNSLAVEINFO		pOtherSlave;
	PTNTESTINSTANCEM	pTestToUse = NULL;
	PTNTESTINSTANCEM	pTest = NULL;
	int					iTesterNum = -1;
	PTNTESTFROMFILE		pLoadedTest = NULL;



	DPL(7, "Getting next command for slave %x [%s, ID %u].",
		3, pSlave, pSlave->m_szComputerName, pSlave->m_id.dwTime);


	this->m_remainingtests.EnterCritSection();


#ifdef DEBUG
	if (this->m_dwMode == TNMODE_POKE)
	{
		DPL(0, "Getting next command in Poke mode!?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (in Poke mode)

	if (! pSlave->m_fReadyToTest)
	{
		DPL(0, "%s is not ready to test!?", 1, pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (slave not ready)
#endif // DEBUG


	// If the user specified a specific number of testers expected, but the
	// session is still open (indicating that not enough slaves have joined
	// yet), then we should hold off from running anything yet.
	if ((this->m_dwMode == TNMODE_API) && (this->m_fJoinersAllowed))
	{
		DPL(0, "Can't start testing in API mode because the session is still open!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (slave limit but not reached)


	// If this guy currently has a test assignment, that's funky.
	if (pSlave->m_pCurrentTest != NULL)
	{
		DPL(0, "Getting command for %s but he still has an active test!  DEBUGBREAK()ing.",
			1, pSlave->m_szComputerName);
		DEBUGBREAK();
		hr = E_FAIL;
		goto DONE;
	} // end if (have active test)


	// Loop through all the slaves to see if they have a test that is
	// waiting for us.  
	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pOtherSlave == NULL)
		{
			DPL(0, "Couldn't get slave %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// Skip the guy passed in.
		if (pOtherSlave == pSlave)
			continue;

		if (pOtherSlave->m_pCurrentTest != NULL)
		{
			fActiveTest = TRUE;

#pragma BUGBUG(vanceo, "What about ongoing tests?")

			// If this test hasn't started yet, and the passed in slave is
			// assigned to it...
			// we haven't found a test yet or
			// this test is earlier in the sequence than the last one we found
			// might be waiting for us.
			if ((! pOtherSlave->m_pCurrentTest->m_fStarted) &&
				(pOtherSlave->m_pCurrentTest->GetSlavesTesterNum(pSlave) >= 0))
			{
				if (pTestToUse == NULL)
				{
					DPL(2, "%s's current test %u (\"%s-%s\") is waiting for %s.",
						5, pOtherSlave->m_szComputerName,
						pOtherSlave->m_pCurrentTest->m_dwUniqueID,
						pOtherSlave->m_pCurrentTest->m_pCase->m_pszID,
						pOtherSlave->m_pCurrentTest->m_pLoadedTest->m_pszInstanceID,
						pSlave->m_szComputerName);

					pOtherSlave->m_pCurrentTest->m_dwRefCount++; // we're using it
					pTestToUse = pOtherSlave->m_pCurrentTest;
				} // end if (first test found)
				else if (pOtherSlave->m_pCurrentTest->m_dwUniqueID < pTestToUse->m_dwUniqueID)
				{
					DPL(2, "%s's current test %u (\"%s-%s\") is waiting for %s, using instead of later test %u.",
						6, pOtherSlave->m_szComputerName,
						pOtherSlave->m_pCurrentTest->m_dwUniqueID,
						pOtherSlave->m_pCurrentTest->m_pCase->m_pszID,
						pOtherSlave->m_pCurrentTest->m_pLoadedTest->m_pszInstanceID,
						pSlave->m_szComputerName,
						pTestToUse->m_dwUniqueID);

					// Free up our previous reference
					pTestToUse->m_dwRefCount--;
					if (pTestToUse->m_dwRefCount == 0)
					{
						DPL(1, "Deleting test %x!?",
							1, pTestToUse->m_dwRefCount);
						delete (pTestToUse);
					} // end if (should delete object)

					pOtherSlave->m_pCurrentTest->m_dwRefCount++; // we're using it
					pTestToUse = pOtherSlave->m_pCurrentTest;
				} // end else if (earlier than current test)
				else
				{
					DPL(5, "%s's current test %u (\"%s-%s\") is waiting for %s, but it's after test %u.",
						6, pOtherSlave->m_szComputerName,
						pOtherSlave->m_pCurrentTest->m_dwUniqueID,
						pOtherSlave->m_pCurrentTest->m_pCase->m_pszID,
						pOtherSlave->m_pCurrentTest->m_pLoadedTest->m_pszInstanceID,
						pSlave->m_szComputerName,
						pTestToUse->m_dwUniqueID);
				} // end else (later than current test)
			} // end if (test hasn't started yet and uses slave)
		} // end if (he's working on something)
	} // end for (each slave)


	if ((fActiveTest) && (pTestToUse == NULL))
	{
		DPL(1, "There are active tests, but none are waiting for %s.",
			1, pSlave->m_szComputerName);
	} // end if (active tests)


	if (this->m_dwMode == TNMODE_API)
	{
RECHECK:

		// Loop through all the remaining tests to find the first one assigned
		// to us.
		for(i = 0; i < this->m_remainingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_remainingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get test item %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If the test involves the current guy, this may be the one we want.
			if (pTest->GetSlavesTesterNum(pSlave) >= 0)
			{
				if (pTestToUse == NULL)
				{
					DPL(2, "%s is assigned to remaining test %u (\"%s-%s\").",
						4, pSlave->m_szComputerName,
						pTest->m_dwUniqueID,
						pTest->m_pCase->m_pszID,
						pTest->m_pLoadedTest->m_pszInstanceID);

					pTest->m_dwRefCount++; // we're using it
					pTestToUse = pTest;
				} // end if (first test found)
				else if (pTest->m_dwUniqueID < pTestToUse->m_dwUniqueID)
				{
					DPL(2, "%s is assigned to remaining test %u (\"%s-%s\"), using instead of later test %u.",
						5, pSlave->m_szComputerName,
						pTest->m_dwUniqueID,
						pTest->m_pCase->m_pszID,
						pTest->m_pLoadedTest->m_pszInstanceID,
						pTestToUse->m_dwUniqueID);

					// Free up our previous reference
					pTestToUse->m_dwRefCount--;
					if (pTestToUse->m_dwRefCount == 0)
					{
						DPL(1, "Deleting test %x!?",
							1, pTestToUse->m_dwRefCount);
						delete (pTestToUse);
					} // end if (should delete object)

					pTest->m_dwRefCount++; // we're using it
					pTestToUse = pTest;
				} // end else if (earlier than current test)
				else
				{
					DPL(5, "%s is assigned to remaining test %u (\"%s-%s\"), but it's after test %u.",
						5, pSlave->m_szComputerName,
						pTest->m_dwUniqueID,
						pTest->m_pCase->m_pszID,
						pTest->m_pLoadedTest->m_pszInstanceID,
						pTestToUse->m_dwUniqueID);
				} // end else (later than current test)

				// Even if it's not the lowest test, we still stop searching because
				// the remaining tests list is in test ID order, so there can't be
				// any tests assigned to this slave later in the list that have
				// earlier test IDs.
				break;
			} // end if (found one we want)

			DPL(9, "%s is not involved in remaining test %u (\"%s-%s\"), skipping.",
				4, pSlave->m_szComputerName,
				pTest->m_dwUniqueID,
				pTest->m_pCase->m_pszID,
				pTest->m_pLoadedTest->m_pszInstanceID);
		} // end for (each test)


		// If we didn't find any more tests for this guy, he's done.
		if (pTestToUse == NULL)
		{
			DPL(0, "%s has completed all tests for him (for this pass).",
				1, pSlave->m_szComputerName);


			// Check to see if all testing for this pass is done.
			if (this->m_remainingtests.Count() == 0)
			{
				if (fActiveTest)
				{
					DPL(1, "Active tests remain, %s will wait for those to complete.",
						1, pSlave->m_szComputerName);
				} // end if (there's an active test)
				else
				{
					DPL(0, "Test list for repetition completed, notifying all slaves.", 0);

#pragma TODO(vanceo, "Think of way to prevent this from being sent when testing is just starting")

					// Loop through all the slaves and tell them that the
					// repetition is complete.
					for(i = 0; i < this->m_slaves.Count(); i++)
					{
						pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
						if (pOtherSlave == NULL)
						{
							DPL(0, "Couldn't get slave %i!", 1, i);
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't get slave)

						hr = this->SendTestingStatusMessageTo(pOtherSlave,
															CTRLTS_REPETITIONCOMPLETE);
						if (hr != S_OK)
						{
							if (hr != TNERR_CONNECTIONDROPPED)
							{
								DPL(0, "Couldn't send repetition complete message to %s!",
									1, pOtherSlave->m_szComputerName);

								goto DONE;
							} // end if (the error isn't because he's already gone)
							else
							{
								DPL(0, "Not sending repetition complete message to %s because he's gone.",
									1, pOtherSlave->m_szComputerName);
								hr = S_OK;
							} // end else (the slave is already gone)
						} // end if (couldn't get send teststatus message)
					} // end for (each slave in session)


#pragma BUGBUG(vanceo, "Make freeing output vars after repetition an option?")

					// Free any output vars we accumulated from the last repetition.
					hr = this->FreeOutputVarsAndData(NULL, NULL, NULL, NULL, NULL);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't free all output vars and data from last repetition!", 0);
						goto DONE;
					} // end if (couldn't free all output vars)



					// Check to see if all the repetitions are done, too.
					if (this->m_loadedtests.Count() == 0)
					{
						DPL(0, "No more tests or repetitions for %s to run, telling him he's complete.",
							1, pSlave->m_szComputerName);

						hr = this->SendTestingStatusMessageTo(pSlave, CTRLTS_ALLCOMPLETE);
						if (hr != S_OK)
						{
							if (hr != TNERR_CONNECTIONDROPPED)
							{
								DPL(0, "Couldn't send all testing complete message to %s!",
									1, pSlave->m_szComputerName);

								goto DONE;
							} // end if (the error isn't because he's already gone)
							else
							{
								DPL(0, "Not sending all testing complete message to %s because he's gone.",
									1, pSlave->m_szComputerName);
								hr = S_OK;
							} // end else (the slave is already gone)
						} // end if (couldn't get send teststatus message)

						goto DONE;
					} // end if (no reps left either)


					// If we got here, there are more repetitions left.  We need to
					// rebuild the test list for another pass.
					hr = this->BuildAPITestList();
					if (hr != S_OK)
					{
						DPL(0, "Couldn't rebuild API test list for another pass!", 0);
						goto DONE;
					} // end if (couldn't build test list)


					// If we tried to repopulate the list, but didn't get any valid
					// permutations, we're done.  BuildAPITestList should have also
					// noticed this and removed any additional repetitions from the
					// list.
					if (this->m_remainingtests.Count() == 0)
					{
						DPL(0, "WARNING: Didn't build any valid permutations from test list!  Telling %s he is complete.",
							1, pSlave->m_szComputerName);

#ifdef DEBUG
						if (this->m_loadedtests.Count() > 0)
						{
							DPL(0, "Still %i loaded tests in list!?  DEBUGBREAK()-ing.",
								1, this->m_loadedtests.Count());

							DEBUGBREAK();
						} // end if (still loaded tests in list)
#endif // DEBUG

						hr = this->SendTestingStatusMessageTo(pSlave, CTRLTS_ALLCOMPLETE);
						if (hr != S_OK)
						{
							if (hr != TNERR_CONNECTIONDROPPED)
							{
								DPL(0, "Couldn't send testing complete message to %s!",
									1, pSlave->m_szComputerName);

								goto DONE;
							} // end if (the error isn't because he's already gone)
							else
							{
								DPL(0, "Not sending testing complete message to %s because he's gone.",
									1, pSlave->m_szComputerName);
								hr = S_OK;
							} // end else (the slave is already gone)
						} // end if (couldn't get send teststatus message)

						goto DONE;
					} // end if (no items in list)

					// We successfully repopulated the list, so we need to recheck
					// for things to do.
					goto RECHECK;
				} // end else (there aren't any active tests)
			} // end if (nothing left in the pass)

			goto DONE;
		} // end if (didn't find any tests)
	} // end if (in API mode)
	else
	{
		CTNTestFromFilesList	untriedloadedtests;
		CTNTestMsList			permutations;
		int						iMaxNumPermutations;


#ifdef DEBUG
		if (this->m_dwMode != TNMODE_STRESS)
		{
			DPL(0, "In some wacky mode %u!", 1, this->m_dwMode);
			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (not in stress mode)
#endif // DEBUG


		if (pTestToUse == NULL)
		{
			if (this->m_loadedtests.Count() <= 0)
			{
				DPL(0, "WARNING: No more loaded tests, unable to build untried list!", 0);
			} // end if (no more loaded tests)


			// Copy the list of loaded tests.
			for(i = 0; i < this->m_loadedtests.Count(); i++)
			{
				pLoadedTest = (PTNTESTFROMFILE) this->m_loadedtests.GetItem(i);
				if (pLoadedTest == NULL)
				{
					DPL(0, "Couldn't get loaded test %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get test)

				hr = untriedloadedtests.Add(pLoadedTest);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add loaded test %i to temp list!", 1, i);
					goto DONE;
				} // end if (couldn't add loaded test to temp list)
			} // end for (each loaded test)


			// Keep looping until we've attempted every loaded test.
			while (untriedloadedtests.Count() > 0)
			{
#pragma BUGBUG(vanceo, "Need the more-random algorithm")
				i = rand() % untriedloadedtests.Count();


				DPL(8, "Picking untried loaded test index %i (%i total).",
					2, i, untriedloadedtests.Count());


				pLoadedTest = (PTNTESTFROMFILE) untriedloadedtests.GetItem(i);
				if (pLoadedTest == NULL)
				{
					DPL(0, "Couldn't get loaded test %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				pLoadedTest->m_dwRefCount++; // we're using it

				hr = untriedloadedtests.Remove(i);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't remove loaded test \"%s-%s\" from remaining untried list!",
						2, pLoadedTest->m_pCase->m_pszID,
						pLoadedTest->m_pszInstanceID);
					goto DONE;
				} // end if (couldn't remove item)


				hr = this->BuildTestPermutationsList(pLoadedTest, &permutations,
													&iMaxNumPermutations);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't build permutations of \"%s-%s\"!",
						2, pLoadedTest->m_pCase->m_pszID,
						pLoadedTest->m_pszInstanceID);
					goto DONE;
				} // end if (couldn't add loaded test to temp list)

				// If there aren't any valid permutations and the session is closed
				// skip the test, since there won't ever be any valid permutations.
				if ((permutations.Count() <= 0) && (! this->m_fJoinersAllowed))
				{
					DPL(0, "WARNING: Test \"%s-%s\" was skipped because there aren't any valid permutations and the session is closed!",
						2, pLoadedTest->m_pCase->m_pszID, pLoadedTest->m_pszInstanceID);


					// Take it out of the main list so it won't ever be tried
					// again.
					hr = this->m_loadedtests.RemoveFirstReference(pLoadedTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't remove loaded test %x from loadedtests list!",
							1, pLoadedTest);
						goto DONE;
					} // end if (couldn't remove test from list)


#pragma TODO(vanceo, "What if there aren't any more runnable tests?  Need to end session like in API mode")
					if (this->m_loadedtests.Count() <= 0)
					{
						DPL(0, "WARNING: No more loaded tests!", 0);
					} // end if (no more loaded tests)


					// We're going to build a fake test so we can add a report
					// without adding a whole bunch of special case code.
					// Hee hee, isn't this special case code?

					pTest = new (CTNTestInstanceM)(this->m_dwCurrentUniqueID++,
													pLoadedTest,
													pLoadedTest->m_pCase,
													1,
													//0,
													NULL);
					if (pTest == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate object)

					hr = this->m_skippedtests.Add(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
						goto DONE;
					} // end if (couldn't add item)


					// Add the skipped test report.
					hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
													pTest, 0, NULL, 0);
					if (hr != S_OK)
					{
						DPL(0, "Failed to add skip report to list!", 0);
						goto DONE;
					} // end if (failed add report)


					pTest = NULL;


					// If we are doing the auto-report thing, and the report we just added is
					// one of the ones we are counting, check to see if that pushed us over the
					// limit.
					if ((this->m_iAutoReportNumReports > 0) &&
						(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
					{
						hr = this->DoAutoReport();
						if (hr != S_OK)
						{
							DPL(0, "Couldn't auto-report!", 0);
							goto DONE;
						} // end if (couldn't print reports)
					} // end if (auto reporting is on and this was a matching report type)
				} // end if (no valid permuations)


				// Keep looping until we've attempted every permutation.
				while (permutations.Count() > 0)
				{
#pragma BUGBUG(vanceo, "Need the more-random algorithm")
					i = rand() % permutations.Count();


					DPL(8, "Picking permutation index %i (%i total).",
						2, i, permutations.Count());


					pTestToUse = (PTNTESTINSTANCEM) permutations.GetItem(i);
					if (pTestToUse == NULL)
					{
						DPL(0, "Couldn't get permutation %i!", 1, i);
						hr = E_FAIL;
						goto DONE;
					} // end if (couldn't get item)

					pTestToUse->m_dwRefCount++; // we're using it

					hr = permutations.Remove(i);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't remove permutation %x from permutations list!",
							1, pTestToUse);
						goto DONE;
					} // end if (couldn't remove item)


					// If this permutation involves us, use it.
					if (pTestToUse->GetSlavesTesterNum(pSlave) >= 0)
					{
						// Assign it a unique ID.
						pTestToUse->m_dwUniqueID = this->m_dwCurrentUniqueID++;

						// Decrement the reps for the loaded test that generated
						// this permutation.  Take it off the list if necessary.
						if (pLoadedTest->m_dwRepsRemaining == 1)
						{
							hr = this->m_loadedtests.RemoveFirstReference(pLoadedTest);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't remove loaded test %x (\"%s-%s\") from loaded tests list!",
									3, pLoadedTest,
									pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
								goto DONE;
							} // end if (couldn't add loaded test to temp list)
						} // end if (last repetition for item)
						else
						{
							// If not infinite reps, decrement the number of reps
							// remaining, but regardless, keep it in the list.
							if (pLoadedTest->m_dwRepsRemaining != 0)
								pLoadedTest->m_dwRepsRemaining--;
						} // end else (not last repetition for item)

						// Get out of the permutation while loop.
						break;
					} // end if (found permutation for us)


					// Otherwise, we don't care about this permutation.  Keep looping.
					pTestToUse->m_dwRefCount--;
					if (pTestToUse->m_dwRefCount == 0)
					{
						DPL(7, "Deleting permutation %x.", 1, pTestToUse);
						delete (pTestToUse);
					} // end if (should delete object)
					else
					{
						DPL(7, "Not deleting permutation %x, its refcount is %u.",
							2, pTestToUse, pTestToUse->m_dwRefCount);
					} // end if (should delete object)
					pTestToUse = NULL;
				} // end while (still permutations left to check)


				// We don't care about the loaded test anymore.
				pLoadedTest->m_dwRefCount--;
				if (pLoadedTest->m_dwRefCount == 0)
				{
					DPL(7, "Deleting loaded test %x.", 1, pLoadedTest);
					delete (pLoadedTest);
				} // end if (should delete object)
				else
				{
					DPL(7, "Not deleting loaded test %x, its refcount is %u.",
						2, pLoadedTest, pLoadedTest->m_dwRefCount);
				} // end if (should delete object)
				pLoadedTest = NULL;


				// If found a test to use, stop looping.
				if (pTestToUse != NULL)
					break;
			} // end while (still untried items left)
		} // end if (didn't find active test to run)


		// If we didn't find any test to run, then we're done here.  If there
		// aren't any active tests, and nobody can join the session, then this
		// slave is done.
		if (pTestToUse == NULL)
		{
			if (fActiveTest)
			{
				DPL(1, "Active tests remain, %s will wait for those to complete.",
					1, pSlave->m_szComputerName);
			} // end if (there's an active test)
			else
			{
				// If nobody else can join the session, the permutations won't
				// change, so this guy will never be able to run another test.
				if (! this->m_fJoinersAllowed)
				{
					DPL(0, "No more tests or repetitions for %s to run, telling him he's complete.",
						1, pSlave->m_szComputerName);

					hr = this->SendTestingStatusMessageTo(pSlave, CTRLTS_ALLCOMPLETE);
					if (hr != S_OK)
					{
						if (hr != TNERR_CONNECTIONDROPPED)
						{
							DPL(0, "Couldn't send testing complete message to %s!",
								1, pSlave->m_szComputerName);

							goto DONE;
						} // end if (the error isn't because he's already gone)
						else
						{
							DPL(0, "Not sending testing complete message to %s because he's gone.",
								1, pSlave->m_szComputerName);
							hr = S_OK;
						} // end else (the slave is already gone)
					} // end if (couldn't get send teststatus message)
				} // end if (no joiners allowed)
				else
				{
					DPL(1, "No more tests or repetitions for %s to run, but joiners are still allowed so he will wait.",
						1, pSlave->m_szComputerName);
				} // end else (joiners allowed)
			} // end else (no active tests)
			
			goto DONE;
		} // end if (didn't find test to run)
	} // end else (in stress mode)


	// If we got here, pTestToUse holds the test he should do next.

	iTesterNum = pTestToUse->GetSlavesTesterNum(pSlave);

	pTestToUse->m_dwRefCount++;
	pSlave->m_pCurrentTest = pTestToUse;


	hr = this->AssignSlaveToTest(pSlave, pTestToUse, iTesterNum, FALSE, TRUE, NULL, 0);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't assign non-placeholder for %s (tester %i) to test %u!",
			3, pSlave->m_szComputerName, iTesterNum, pTestToUse->m_dwUniqueID);
		goto DONE;
	} // end if (couldn't assign slave to test)



DONE:

	if (pLoadedTest != NULL)
	{
		pLoadedTest->m_dwRefCount--;
		if (pLoadedTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting still held loaded test %x.", 1, pLoadedTest);
			delete (pLoadedTest);
		} // end if (should delete object)
		else
		{
			DPL(7, "Not deleting still held loaded test %x, its refcount is %u.",
				2, pLoadedTest, pLoadedTest->m_dwRefCount);
		} // end if (should delete object)
		pLoadedTest = NULL;
	} // end if (still have loaded test)

	if (pTestToUse != NULL)
	{
		pTestToUse->m_dwRefCount--;
		if (pTestToUse->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test to use %x.", 1, pTestToUse);
			delete (pTestToUse);
		} // end if (should delete object)
		else
		{
			DPL(7, "Not deleting test to use %x, its refcount is %u.",
				2, pTestToUse, pTestToUse->m_dwRefCount);
		} // end if (should delete object)
		pTestToUse = NULL;
	} // end if (have test to use)

	this->m_remainingtests.LeaveCritSection();

	return (hr);
} // CTNMaster::GetNextCmdForMachine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::AssignSlaveToTest()"
//==================================================================================
// CTNMaster::AssignSlaveToTest
//----------------------------------------------------------------------------------
//
// Description: Assigns the slave to the given test (if he wasn't already), and adds
//				a report about the assignment.
//				If that's the last assignment needed for the test, then the slave is
//				notified that he can begin testing it.
//				If fPlaceholder is TRUE, then the slave's slot is filled, but it
//				doesn't initiate any testing, 
//				If fGetInputData is TRUE, then the module's GetInputData callback
//				will be used to retrieve the input data to send to the slave, if it
//				exists.  If fGetInputData is FALSE and pvInputData is not NULL, then
//				this data will be recorded as what the slave is using for it's input
//				data.
//				The slave list lock is assumed to be held.
//
// Arguments:
//	PTNSLAVEINFO pSlave			Slave to assign.
//	PTNTESTINSTANCEM pTest		Test to assign slave to.
//	int iTesterNum				Tester slot to assign slave to.
//	BOOL fPlaceholder			TRUE if this assignment is a placeholder, FALSE if
//								slave is assigning self.
//	BOOL fGetInputData			Should we try to retrieve input data for the slave?
//	PVOID pvInputData			Pointer to input data the slave is already using, if
//								any.
//	DWORD dwInputDataSize		Size of input data the slave is already using, if
//								any.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::AssignSlaveToTest(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									int iTesterNum, BOOL fPlaceholder,
									BOOL fGetInputData, PVOID pvInputData,
									DWORD dwInputDataSize)
{
	HRESULT				hr = S_OK;
	int					i;
	PTNSLAVEINFO*		paTempTestersArray = NULL;
	TNGETINPUTDATA		tngid;
	int					iNumSelfAssigns = 0;
	CTNSendDataQueue	bufferedsends;
	PTNSENDDATA			pSendData = NULL;


	ZeroMemory(&tngid, sizeof (TNGETINPUTDATA));


	if (pTest->m_paTesterSlots[iTesterNum].pSlave != NULL)
	{
		if (pTest->m_paTesterSlots[iTesterNum].pSlave != pSlave)
		{
			DPL(0, "Can't use %s to fill tester slot %i of test %u, it's already filled by %s!",
				4, pSlave->m_szComputerName, iTesterNum, pTest->m_dwUniqueID,
				pTest->m_paTesterSlots[iTesterNum].pSlave->m_szComputerName);
			hr = E_FAIL;
			goto DONE;
		} // end if (already complete)

		if (pTest->m_paTesterSlots[iTesterNum].fComplete)
		{
			DPL(0, "Can't refill tester slot %i of test %u with %s because it's marked as complete!",
				3, iTesterNum, pTest->m_dwUniqueID, pSlave->m_szComputerName);
			hr = E_FAIL;
			goto DONE;
		} // end if (already complete)

		DPL(6, "Refilling tester slot %i of test %u with%s%s.",
			4, iTesterNum,
			pTest->m_dwUniqueID,
			(fPlaceholder ? " placeholder for " : " "),
			pSlave->m_szComputerName);
	} // end if (slot already full)
	else
	{
		DPL(3, "Filling tester slot %i of test %u with%s%s.",
			4, iTesterNum,
			pTest->m_dwUniqueID,
			(fPlaceholder ? " placeholder for " : " "),
			pSlave->m_szComputerName);

		pSlave->m_dwRefCount++; // it's going to be in the slot
		pTest->m_paTesterSlots[iTesterNum].pSlave = pSlave;
	} // end else (slot not full)

	// If it's a placeholder, we're done.
	if (fPlaceholder)
		goto DONE;


	if (pTest->m_paTesterSlots[iTesterNum].dwAssignTime)
	{
		DPL(0, "%s is assigning self to tester %i of test %u, but it was already not a placeholder!",
			3, pSlave->m_szComputerName, iTesterNum, pTest->m_dwUniqueID);

		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (already filled self)

	pTest->m_paTesterSlots[iTesterNum].dwAssignTime = GetTickCount();


	// Loop through all the testers to see if this is the last person
	// assigning himself.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		// If this slot has been "self-filled" note that.
		if (pTest->m_paTesterSlots[i].dwAssignTime != 0)
			iNumSelfAssigns++;
	} // end for (each tester)


	// If we're the only people self-filled as of now...
	if (iNumSelfAssigns == 1)
	{
		DPL(1, "%s has first self-assignment for test ID %u.",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);

		// If we're in API mode, and it's a top level test, then remove it from
		// the list of remaining items since we're starting it.
		if ((this->m_dwMode == TNMODE_API) &&
			(pTest->m_pParentTest == NULL))
		{
			hr = this->m_remainingtests.RemoveFirstReference(pTest);
			if (hr != S_OK)
			{
				DPL(0, "Failed to remove test %x from remaining tests list!",
					1, pTest);
				goto DONE;
			} // end if (failed removing references to test)
		} // end if (API mode)
	} // end if (first tester assigned)


	// If we can't retrieve input data (because we may be getting handed it),
	// we can add the assignment report now (along with the data).  If not,
	// we're going to wait until we're actually ready to retrieve and send the
	// input data to add the report.
	// The only time input data should be retrievable on the master is when
	// assigning a API or stress mode top level test.
	if (! fGetInputData)
	{
		// If there's no parent test, then it is a test, otherwise a subtest
		// report.
		hr = this->m_reports.AddReport(((pTest->m_pParentTest == NULL) ? RT_TESTASSIGNED : RT_TESTASSIGNED_SUBTEST),
									1, &(pTest->m_paTesterSlots[iTesterNum].pSlave),
									pTest,
									0,
									pvInputData,
									dwInputDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add test assignment report!", 0);
			this->m_remainingtests.LeaveCritSection();
			goto DONE;
		} // end if (failed to add report)


		// If we are doing the auto-report thing, and the report we just added is
		// one of the ones we are counting, check to see if that pushed us over the
		// limit.
		if ((this->m_iAutoReportNumReports > 0) &&
			(this->m_dwAutoReportFlags & TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE))
		{
			hr = this->DoAutoReport();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't auto-report!", 0);
				goto DONE;
			} // end if (couldn't print reports)
		} // end if (auto reporting is on and this was a matching report type)
	} // end if (can't get input data)
#ifdef DEBUG
	else
	{
		if (pTest->m_pParentTest != NULL)
		{
			DPL(0, "Able to get input data for a subtest (ID %u)!?",
				1, pTest->m_dwUniqueID);

			DEBUGBREAK();

			hr = E_FAIL;
			goto DONE;
		} // end if (not a top level test)

		if (this->m_dwMode == TNMODE_POKE)
		{
			DPL(0, "Able to get input data for a test in poke mode!?", 0);

			DEBUGBREAK();

			hr = E_FAIL;
			goto DONE;
		} // end if (in Poke mode)
	} // end else (can get input data) 
#endif // DEBUG


	// If we're not the last person to self-fill, we're done.
	if (iNumSelfAssigns != pTest->m_iNumMachines)
		goto DONE;


	DPL(1, "%s has last self-assignment for test ID %u.",
		2, pSlave->m_szComputerName, pTest->m_dwUniqueID);



	if (pTest->m_fStarted)
	{
		DPL(0, "Starting test %s (unique ID %u) twice!?",
			2, pTest->m_pCase->m_pszID, pTest->m_dwUniqueID);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we're starting this test twice)

	// Hey, the test is now going.
	pTest->m_fStarted = TRUE;



	// Loop through all the testers.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		// If we can get input data, try to do it now.  Also add the test
		// report since we couldn't before, too.
		if (fGetInputData)
		{
			if (pTest->m_pCase->m_pfnGetInputData != NULL)
			{
				tngid.dwSize = sizeof (TNGETINPUTDATA);
				tngid.pMaster = this;
				tngid.pMachine = pTest->m_paTesterSlots[i].pSlave;
				tngid.iTesterNum = i;
				tngid.pTest = pTest->m_pCase;
				tngid.pStringData = pTest->m_pLoadedTest->m_pStoredDataList;
				//tngid.pvData = NULL;
				//tngid.dwDataSize = 0;

				hr = pTest->m_pCase->m_pfnGetInputData(&tngid);
				if (hr != ERROR_BUFFER_TOO_SMALL)
				{
					DPL(0, "Module's GetInputData function for case \"%s\" didn't return ERROR_BUFFER_TOO_SMALL!  %e",
						2, pTest->m_pCase->m_pszID, hr);

					if (hr == S_OK)
						hr = E_FAIL;

					goto DONE;
				} // end if (get input data failed)

				
				tngid.pvData = LocalAlloc(LPTR, tngid.dwDataSize);
				if (tngid.pvData == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				//tngid.dwDataSize = tngid.dwDataSize;

				hr = pTest->m_pCase->m_pfnGetInputData(&tngid);
				if (hr != S_OK)
				{
					DPL(0, "Module's GetInputData callback for case \"%s\" failed!",
						1, pTest->m_pCase->m_pszID);
					goto DONE;
				} // end if (module's callback failed)
			} // end if (the test can get input data)


			hr = this->m_reports.AddReport(RT_TESTASSIGNED,
										1, &(pTest->m_paTesterSlots[i].pSlave),
										pTest,
										0,
										tngid.pvData,
										tngid.dwDataSize);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add test assignment report!", 0);
				this->m_remainingtests.LeaveCritSection();
				goto DONE;
			} // end if (failed to add report)


			// If we are doing the auto-report thing, and the report we just added
			// is one of the ones we are counting, check to see if that pushed us
			// over the limit.
			if ((this->m_iAutoReportNumReports > 0) &&
				(this->m_dwAutoReportFlags & TNREPORT_TESTASSIGNMENTSTESTNOTCOMPLETE))
			{
				hr = this->DoAutoReport();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't auto-report!", 0);
					goto DONE;
				} // end if (couldn't print reports)
			} // end if (auto reporting is on and this was a matching report type)
		} // end if (can get input data)



		// Now actually send the message out to the person.  If it's an unprompted
		// test, send the whole shebang to him.  If the slave was the one telling
		// us about the test, we only need to send a small part of the information.
		// If we retrieved any input data, we'll send that.
		if (pTest->m_paTesterSlots[i].dwAnnounceRequestID == 0)
		{
			hr = this->SendNewTestMessageTo(pTest->m_paTesterSlots[i].pSlave,
											pTest,
											tngid.pvData,
											tngid.dwDataSize,
											&bufferedsends);
			if (hr != S_OK)
			{
				if (hr != TNERR_CONNECTIONDROPPED)
				{
					DPL(0, "Couldn't get send new test message to machine %s!",
						1, pSlave->m_szComputerName);
				} // end if (the error isn't because he's already gone)
				else
				{
					DPL(0, "Not sending new test message to %s because he's gone.",
						1, pSlave->m_szComputerName);
					hr = S_OK;
				} // end else (the slave is already gone)
			} // end if (couldn't get send teststatus message)
		} // end if (not a response to a slave's request)
		else
		{
			hr = this->SendAnnounceTestReplyMessageTo(pTest->m_paTesterSlots[i].pSlave,
													pTest->m_paTesterSlots[i].dwAnnounceRequestID,
													pTest->m_dwUniqueID,
													&bufferedsends);
			if (hr != S_OK)
			{
				if (hr != TNERR_CONNECTIONDROPPED)
				{
					DPL(0, "Couldn't get send announce test reply message to machine %s!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (not because connectiondropped)
				else
				{
					DPL(0, "WARNING: Connection to %s dropped, not sending announce test reply message!",
						1, pSlave->m_szComputerName);
					hr = S_OK;
				} // end else (connection dropped)
			} // end if (couldn't send announcetestreply message)

			// Clear the slot so it can be used again
			pTest->m_paTesterSlots[i].dwAnnounceRequestID = 0;
		} // end else (is a response to a slave's request)


		// If we retrieved input data, reset it.
		if (tngid.pvData != NULL)
		{
			LocalFree(tngid.pvData);
			tngid.pvData = NULL;
			tngid.dwDataSize = 0;
		} // end if (got input data)
	} // end for (each tester)


	DPL(3, "Test %u (\"%s\") has been completely assigned (%i testers).",
		3, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID,
		pTest->m_iNumMachines);


	paTempTestersArray = (PTNSLAVEINFO*) LocalAlloc(LPTR, (pTest->m_iNumMachines * sizeof (PTNSLAVEINFO)));
	if (paTempTestersArray == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		paTempTestersArray[i] = pTest->m_paTesterSlots[i].pSlave;
	} // end for (each machine))

	// Add the report that that was the last person notified about the test
	hr = this->m_reports.AddReport(RT_TESTCOMPLETELYASSIGNED,
								pTest->m_iNumMachines,
								paTempTestersArray,
								pTest,
								0, NULL, 0);

	LocalFree(paTempTestersArray);
	paTempTestersArray = NULL;

	if (hr != S_OK)
	{
		DPL(0, "Couldn't add test assignment completion report!", 0);
		goto DONE;
	} // end if (failed to add report)


	// If we are doing the auto-report thing, and the report we just added is
	// one of the ones we are counting, check to see if that pushed us over the
	// limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTNOTCOMPLETE))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	// Now actually allow the notification messages we queued up to be sent.
	while (bufferedsends.Count() > 0)
	{
		pSendData = (PTNSENDDATA) bufferedsends.PopFirstItem();
		if (pSendData == NULL)
		{
			DPL(0, "Couldn't pop first buffered send!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't pop first item)

		// We don't actually need our refcount, but we won't delete it, we
		// just add it to the real send queue.
		pSendData->m_dwRefCount--;

#ifdef DEBUG
		if (pSendData->m_dwRefCount != 0)
		{
			DPL(0, "Refcount on buffered send %x is not 0 as expected (its %u)!",
				2, pSendData, pSendData->m_dwRefCount);

			DEBUGBREAK();
		} // end if (refcount is funky)
#endif // DEBUG

		hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add buffered send data %x to queue!", 0);

			delete (pSendData);
			pSendData = NULL;

			goto DONE;
		} // end if (couldn't add send data)
	} // end for (each item)


DONE:

	if (tngid.pvData != NULL)
	{
		LocalFree(tngid.pvData);
		tngid.pvData = NULL;
	} // end if (we allocated data)

	if (paTempTestersArray != NULL)
	{
		LocalFree(paTempTestersArray);
		paTempTestersArray = NULL;
	} // end if (we allocated an array)

	return (hr);
} // CTNMaster::AssignSlaveToTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CompleteSlavesTest()"
//==================================================================================
// CTNMaster::CompleteSlavesTest
//----------------------------------------------------------------------------------
//
// Description: Completes the given test that the slave is working on.  Logs the
//				final report if that was the last slave needed to complete.
//				If not in Poke mode, this was not a sub or ongoing test, and this is
//				the last slave to complete, then the next command for each of the
//				testers is retrieved, if it's appropriate (not Poke mode, not dead,
//				etc.).
//				If the slave is working on a subtest, then it is completed with
//				the same success parameter as well.
//				The slave lock is assumed to be held.
//
// Arguments:
//	PTNSLAVEINFO pSlave		Slave completing the test.
//	PTNTESTINSTANCEM pTest	Test being completed.
//	BOOL fSuccess			Whether its a successful completion or not.
//	BOOL fSubTest			Whether the test was a subtest.
//	BOOL fOngoing			Whether the test was ongoing.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CompleteSlavesTest(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									BOOL fSuccess, BOOL fSubTest, BOOL fOngoing)
{
	HRESULT					hr = S_OK;
	HRESULT					hrFinalResult = S_OK;
	PTNTESTINSTANCEM		pSubTest = NULL;
	BOOL					fFinalSuccess = FALSE;
	PTNSUCCESSFILTERITEM	paTesterResults = NULL;
	PTNSLAVEINFO*			paTempTestersArray = NULL;
	PVOID					pvFinalCompleteReportData = NULL;
	DWORD					dwFinalCompleteReportDataSize = 0;
	DWORD					dwReportType;
	int						i;
	int						j;
	int						iTesterNum;
	char*					pszTemp = NULL;
	PTNSLAVEINFO			pOtherSlave = NULL;
	BOOL					fAdded = FALSE;


	DPL(9, "==>(%x [%s], %x [%u], %B, %B, %B)",
		7, pSlave, pSlave->m_szComputerName, pTest, pTest->m_dwUniqueID,
		fSuccess, fSubTest, fOngoing);


	// Make sure we hold on to it so no one deletes it behind our backs.
	//BUGBUG doesn't completely work, since we need the lock to do this
	pTest->m_dwRefCount++;


	if (! pTest->m_fStarted)
	{
		DPL(0, "WARNING: Completing test %u for %s, even though it hasn't started.",
			2, pTest->m_dwUniqueID, pSlave->m_szComputerName);
	} // end if (test not started)


	// If there are any subtests involving the slave, complete them too.
	for(i = 0; i < pTest->m_subtests.Count(); i++)
	{
		pSubTest = (PTNTESTINSTANCEM) pTest->m_subtests.GetItem(i);
		if (pSubTest == NULL)
		{
			DPL(0, "Couldn't get subtest %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		pSubTest->m_dwRefCount++;

#pragma BUGBUG(vanceo, "How do we notify poke testers of lost top level guy during request?")
		// If the test hasn't started, we don't care about it.
		if (pSubTest->m_fStarted)
		{
			iTesterNum = pSubTest->GetSlavesTesterNum(pSlave);

			// If the slave is assigned to this subtest, and the slave hasn't
			// already completed it, force him to complete it if it started,
			// or alert the other waiting testers if it hasn't.
			if ((iTesterNum >= 0) &&
				(! pSubTest->m_paTesterSlots[iTesterNum].fComplete))
			{
				DPL(1, "Forcing completion of %s (subtester %i)'s subtest ID %u.",
					3, pSlave->m_szComputerName, iTesterNum,
					pSubTest->m_dwUniqueID);

				hr = this->CompleteSlavesTest(pSlave, pSubTest, fSuccess, TRUE, FALSE);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't complete %s's subtest ID %u!",
						2, pSlave->m_szComputerName, pSubTest->m_dwUniqueID);
					goto DONE;
				} // end if (couldn't complete subtest)
			} // end if (the slave was working on the sub test)
		} // end if (the test has started)

		pSubTest->m_dwRefCount--;
		if (pSubTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting subtest %x.", 1, pSubTest);
			delete (pSubTest);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting subtest %x, its refcount is %u.",
				2, pSubTest, pSubTest->m_dwRefCount);
		} // end else (no last reference)
		pSubTest = NULL;
	} // end for (each sub test)


	iTesterNum = pTest->GetSlavesTesterNum(pSlave);
	if (iTesterNum < 0)
	{
		DPL(0, "%s isn't working on test %u!?", 2, pSlave->m_szComputerName,
			pTest->m_dwUniqueID);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (not working on test)


	// Remove any failed sync operations started by the slave.
	if (pTest->m_pSyncDataList != NULL)
	{
		PTNSYNCDATA		pSyncData;


		pTest->m_pSyncDataList->EnterCritSection();

		for(i = 0; i < pTest->m_pSyncDataList->Count(); i++)
		{
			pSyncData = (PTNSYNCDATA) pTest->m_pSyncDataList->GetItem(i);
			if (pSyncData == NULL)
			{
				DPL(0, "Couldn't get sync data item %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)

			// If it's a non-placeholder item for the tester, remove the data.
			if ((pSyncData->m_iTesterNum == iTesterNum) &&
				(pSyncData->m_pSyncList != NULL))
			{
				delete (pSyncData->m_pSyncList);
				pSyncData->m_pSyncList = NULL;
			} // end if (it's a non-placeholder for the right tester)
		} // end for (each sync item)

		pTest->m_pSyncDataList->LeaveCritSection();
	} // end if (there are sync data items)



	// When the guy is done with a test, we need to fire LostTester messages
	// at anybody left in the test.  If they're doing something that requires
	// him to be there, they'll abort it.  Otherwise, they'll ignore it.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		// Skip the current completer.
		if (i == iTesterNum)
			continue;

		// If this current tester isn't a placeholder and hasn't already
		// completed the test, notify him of the dead slave.
		if ((pTest->m_paTesterSlots[i].dwAssignTime != 0) &&
			(! pTest->m_paTesterSlots[i].fComplete))
		{
			DPL(1, "Sending LostTester message to %s because of completion for test ID %u by %s.",
				3, pTest->m_paTesterSlots[i].pSlave->m_szComputerName,
				pTest->m_dwUniqueID, pSlave->m_szComputerName);

			hr = this->SendLostTesterTo(pTest->m_paTesterSlots[i].pSlave,
										pTest, iTesterNum);
			if (hr != S_OK)
			{
				if (hr != TNERR_CONNECTIONDROPPED)
				{
					DPL(0, "Failed to send LostTester message to %s!",
						1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
					goto DONE;
				} // end if (not connectiondropped)
				else
				{
					DPL(0, "WARNING: Not sending LostTester message to %s because he's gone!",
						1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
					hr = S_OK;
				} // end else (connection dropped)
			} // end if (failed to send message)
		} // end if (this tester has not completed yet)
	} // end if (for each other tester)


	if (pTest->m_paTesterSlots[iTesterNum].dwAssignTime == 0)
	{
		DPL(8, "Test %u only has placeholder for %s (tester %i).",
			3, pTest->m_dwUniqueID, pSlave->m_szComputerName, iTesterNum);
		goto DONE;
	} // end if (placeholder)


	pTest->m_paTesterSlots[iTesterNum].fComplete = TRUE;
	pTest->m_paTesterSlots[iTesterNum].fSuccess = fSuccess;


	if (! fSubTest)
	{
		if (fOngoing)
		{
			hr = pSlave->m_ongoingtestslist.RemoveFirstReference(pTest);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't pull test unique ID %u from slave %s's ongoing list!",
					2, pTest->m_dwUniqueID, pSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't remove the test from the list)


			if (pTest->m_dwRefCount == 0)
			{
				DPL(0, "Ongoing test (%x) refcount hit 0?", 1, pTest);

				hr = E_FAIL;
				goto DONE;
			} // end if (should delete test)
		} // end if (this is an ongoing test)
		else
		{
			// If this was the reporter's current test, clear it.

			pSlave->m_pCurrentTest->m_dwRefCount--;
			if (pSlave->m_pCurrentTest->m_dwRefCount == 0)
			{
				DPL(0, "Current test (%x) refcount hit 0?",
					1, pSlave->m_pCurrentTest);

				hr = E_FAIL;
				goto DONE;
			} // end if (should delete test)

			pSlave->m_pCurrentTest = NULL;
		} // end if (it's not an ongoing test)
	} // end if (it's not a subtest)
	else
	{
		// We can't remove it here, only the last subtest completion can get
		// rid of it.  Otherwise, testers still working on it will get
		// unrecognized test type problems when they report, sync, etc.
	} // end else (it's a subtest)


	paTempTestersArray = (PTNSLAVEINFO*) LocalAlloc(LPTR, (pTest->m_iNumMachines * sizeof (PTNSLAVEINFO)));
	if (paTempTestersArray == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// If there's a FilterSuccess callback, build an array for it.
	if (pTest->m_pCase->m_pfnFilterSuccess != NULL)
	{
		paTesterResults = (PTNSUCCESSFILTERITEM) LocalAlloc(LPTR, (pTest->m_iNumMachines * sizeof (TNSUCCESSFILTERITEM)));
		if (paTesterResults == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
	} // end if (there's a FilterSuccess callback)


	// It only takes one failed complete to consider the test a failure.  This
	// is the default value, but the module's FilterSuccess callback may adjust
	// this.
	fFinalSuccess = TRUE;

	// Loop through all the testers.  If someone else hasn't completed the test
	// yet, we're not the last one.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
#ifdef DEBUG
		if (pTest->m_paTesterSlots[i].pSlave == NULL)
		{
			DPL(0, "Test slot %i for test %u hasn't been assigned yet?!",
				2, i, pTest->m_dwUniqueID);
			hr = E_FAIL;
			goto DONE;
		} // end if (slot is not assigned)
#endif // DEBUG

		if (! pTest->m_paTesterSlots[i].fComplete)
		{
			// Well, someone hasn't completed it yet, so we're finished with
			// this function.
			DPL(8, "Tester %i (%s) hasn't completed test %u yet, not final completion.",
				3, i, pTest->m_paTesterSlots[i].pSlave->m_szComputerName,
				pTest->m_dwUniqueID);
			goto DONE;
		} // end if (not done yet)

		// If even one completion was a failure, the whole thing is.
		if (! pTest->m_paTesterSlots[i].fSuccess)
			fFinalSuccess = FALSE;

		paTempTestersArray[i] = pTest->m_paTesterSlots[i].pSlave;

		// If there's a FilterSuccess callback, populate the corresponding
		// array entry.
		if (pTest->m_pCase->m_pfnFilterSuccess != NULL)
		{
			paTesterResults[i].pSlave = pTest->m_paTesterSlots[i].pSlave;
			paTesterResults[i].hresult = pTest->m_paTesterSlots[i].hresult;
			paTesterResults[i].fSuccess = pTest->m_paTesterSlots[i].fSuccess;
			paTesterResults[i].pvOutputData = pTest->m_paTesterSlots[i].pvOutputData;
			paTesterResults[i].dwOutputDataSize = pTest->m_paTesterSlots[i].dwOutputDataSize;
			paTesterResults[i].pVars = pTest->m_paTesterSlots[i].pVars;
		} // end if (there's a FilterSuccess callback)
	} // end for (each tester)



	DPL(0, "That completed %sest with unique ID %u.",
		2, ((fSubTest) ? "subt" : "t"), pTest->m_dwUniqueID);


	// If the test is a subtest, we couldn't remove it above, so we do it here.
	if (fSubTest)
	{
		hr = pTest->m_pParentTest->m_subtests.RemoveFirstReference(pTest);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't pull test unique ID %u from test %u's subtest list!",
				1, pTest->m_dwUniqueID, pTest->m_pParentTest);
			goto DONE;
		} // end if (couldn't remove the test from the list)

		if (pTest->m_dwRefCount == 0)
		{
			DPL(0, "Subtest (%x) refcount hit 0?", 1, pTest);

			hr = E_FAIL;
			goto DONE;
		} // end if (should delete test)
	} // end if (subtest)


	if (pTest->m_pCase->m_pfnFilterSuccess != NULL)
	{
		TNFILTERSUCCESSDATA		tnfsd;


		ZeroMemory(&tnfsd, sizeof (TNFILTERSUCCESSDATA));
		tnfsd.dwSize = sizeof (TNFILTERSUCCESSDATA);
		tnfsd.hresult = hrFinalResult;
		tnfsd.fSuccess = fFinalSuccess;
		tnfsd.iNumMachines = pTest->m_iNumMachines;
		tnfsd.aTesterResults = paTesterResults;
		//tnfsd.pvData = NULL;
		//tnfsd.dwDataSize = 0;

		hr = pTest->m_pCase->m_pfnFilterSuccess(&tnfsd);
		if (hr == ERROR_BUFFER_TOO_SMALL)
		{
			dwFinalCompleteReportDataSize = tnfsd.dwDataSize;
			pvFinalCompleteReportData = LocalAlloc(LPTR, dwFinalCompleteReportDataSize);
			if (pvFinalCompleteReportData == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			tnfsd.pvData = pvFinalCompleteReportData;

			// Try again with the new report data buffer.
			hr = pTest->m_pCase->m_pfnFilterSuccess(&tnfsd);
		} // end if (need to allocate a data buffer)

		if (hr != S_OK)
		{
			DPL(0, "Module's FilterSuccess callback failed!", 0);
			goto DONE;
		} // end if (module's filter success callback failed)

		if (fFinalSuccess != tnfsd.fSuccess)
		{
			if (fFinalSuccess)
			{
				DPL(0, "All testers reported success for test %u (case \"%s\"), but module's FilterSuccess callback considers it a failure.",
					2, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID);
				fFinalSuccess = FALSE;
			} // end if (it was changed from a success)
			else
			{
				DPL(0, "Module tried to change a failed test case (\"%s\", unique ID %u) into a success!",
					2, pTest->m_pCase->m_pszID, pTest->m_dwUniqueID);
				hr = E_FAIL;
				goto DONE;
			} // end else (it was changed from a failure)
		} // end if (callback changed result)

		hrFinalResult = tnfsd.hresult;
	} // end if (there's a FilterSuccess callback)



	// Free any failed syncs left in the test.
	if (pTest->m_pSyncDataList != NULL)
	{
		delete (pTest->m_pSyncDataList);
		pTest->m_pSyncDataList = NULL;
	} // end if (there are sync operations still)



	// If we're not supposed to save results, remove the results from any
	// subtests we invoked.
	// See below.
	if (pTest->m_pTestsToFree != NULL)
	{
		while (pTest->m_pTestsToFree->Count() > 0)
		{
			pSubTest = (PTNTESTINSTANCEM) pTest->m_pTestsToFree->PopFirstItem();
			if (pSubTest == NULL)
			{
				DPL(0, "Couldn't pop first subtest to free from list!", 0);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't pop first item)


			// Pull it off the vars list.  We're going to ignore errors,
			// because it may have been removed by some other means, like
			// the user explicitly calling a FreeOutputVars function.
			hr = this->m_testswithvars.RemoveFirstReference(pSubTest);
			if (hr != S_OK)
			{
				DPL(4, "Couldn't remove subtest %u (under test %u) from tests with vars list, ignoring.  %e",
					3, pSubTest->m_dwUniqueID, pTest->m_dwUniqueID,
					hr);

				hr = S_OK;
			} // end if (remove first reference failed)


			// Loop through all the slots and free any output vars or data.
			for(i = 0; i < pTest->m_iNumMachines; i++)
			{
				// If there are variables, there must be data, and there
				// should never be data but no vars.
				if (pTest->m_paTesterSlots[i].pVars != NULL)
				{
					delete (pTest->m_paTesterSlots[i].pVars);
					pTest->m_paTesterSlots[i].pVars = NULL;

					LocalFree(pTest->m_paTesterSlots[i].pvOutputData);
					pTest->m_paTesterSlots[i].pvOutputData = NULL;
					pTest->m_paTesterSlots[i].dwOutputDataSize = 0;
				} // end if (there's output vars)
			} // end for (each tester)


			// Take off the ref given by PopFirstItem.
			pSubTest->m_dwRefCount--;
			if (pSubTest->m_dwRefCount == 0)
			{
				DPL(7, "Deleting subtest %x.", 1, pSubTest);
				delete (pSubTest);
			} // end if (should delete test)
			else
			{
				DPL(7, "Not deleting subtest %x, its refcount is %u.",
					2, pSubTest, pSubTest->m_dwRefCount);
			} // end else (shouldn't delete test)
			pSubTest = NULL;
		} // end while (still more tests to free)


		// We don't need the list object any more.
		delete (pTest->m_pTestsToFree);
		pTest->m_pTestsToFree = NULL;
	} // end if (we shouldn't save results)



	// Loop through all the tester slots looking for output data.  If there's
	// output data but no variables, there's no way the module (master side)
	// can retrieve the data again, we might as well free it up.  If we're
	// not saving results for this test, we will free any data we find.
	// Otherwise, we want to save this test so the variables can be accessed
	// later.  But only save only one alias to the test.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		if (pTest->m_paTesterSlots[i].pvOutputData != NULL)
		{
			if (pTest->m_paTesterSlots[i].pVars != NULL)
			{
				if (pTest->m_pCase->m_dwOptionFlags & TNTCO_DONTSAVERESULTS)
				{
					delete (pTest->m_paTesterSlots[i].pVars);
					pTest->m_paTesterSlots[i].pVars = NULL;
				} // end if (shouldn't save results)
				else if (! fAdded)
				{
					// Add this test to our list of items to save.
					hr = this->m_testswithvars.Add(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add alias to test %x to testswithvars list!",
							1, pTest);
						goto DONE;
					} // end if (couldn't add alias)

					// We only want to add it to the list once.
					fAdded = TRUE;
				} // end else if (haven't added this test already)
			} // end if (there are output vars)

			// If there weren't any variables, or we just freed them,
			// free the output data, too.
			if (pTest->m_paTesterSlots[i].pVars == NULL)
			{
				LocalFree(pTest->m_paTesterSlots[i].pvOutputData);
				pTest->m_paTesterSlots[i].pvOutputData = NULL;
				pTest->m_paTesterSlots[i].dwOutputDataSize = 0;
			} // end if (no output vars)
		} // end if (there's output data)
	} // end for (each tester)
	

	// If this test had some variables & was added to the list, and it's a
	// subtest, check to see if any of the parent tests specified that results
	// were not to be saved.  If so, we need to register with that test so it
	// can remove our results when it completes.
	// See above.
	if ((fAdded) && (fSubTest))
	{
		PTNTESTINSTANCEM	pParentTest;


		pParentTest = pTest->m_pParentTest;
		do
		{
			if (pParentTest->m_pTestsToFree != NULL)
			{
				// Add it to that parent's list of tests with data to free
				// when it completes.
				hr = pParentTest->m_pTestsToFree->Add(pTest);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add test %u to parent (%u)'s list of tests to free!",
						2, pTest->m_dwUniqueID, pParentTest->m_dwUniqueID);
					goto DONE;
				} // end if (couldn't add test)

				// We found one, so we can stop searching.
				break;
			} // end if (parent test shouldn't save results)

			pParentTest = pParentTest->m_pParentTest;
		} // end do (while there's a parent test)
		while (pParentTest != NULL);
	} // end else (possibly should save results)


	if (fFinalSuccess)
	{
		if (fSubTest)
			dwReportType = RT_SUCCESSCOMPLETE_SUBTEST;
		else
			dwReportType = RT_SUCCESSCOMPLETE;
	} // end if (there weren't any failurecompletions)
	else
	{
		if (fSubTest)
			dwReportType = RT_FAILURECOMPLETE_SUBTEST;
		else
			dwReportType = RT_FAILURECOMPLETE;
	} // end else (there was at least one failurecompletion)


	// Add the completion report.  This was originally based on whether everyone
	// succeeded or at least one person failed, but the module could have
	// overridden it with the FilterSuccess function.
	// Also pass the list of machines involved in the test and any data the
	// FilterSuccess function may have allocated.
	hr = this->m_reports.AddReport(dwReportType,
								pTest->m_iNumMachines,
								paTempTestersArray,
								pTest,
								hrFinalResult,
								pvFinalCompleteReportData,
								dwFinalCompleteReportDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is one
	// of the ones we are counting, check to see if that pushed us over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		((fFinalSuccess) &&
			(this->m_dwAutoReportFlags & (TNREPORT_TESTSUCCESSES | TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTSUCCEEDED))) ||
		((! fFinalSuccess) &&
			(this->m_dwAutoReportFlags & (TNREPORT_TESTFAILURES | TNREPORT_TESTASSIGNMENTSTESTFAILED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED))))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	// Update statistics for the test, and the global statistics if it's
	// not a subtest.
	if (fFinalSuccess)
	{
		if (! fSubTest)
			this->m_totalstats.IncrementSuccesses();

		pTest->m_pCase->m_stats.IncrementSuccesses();
	} // end if (its a success report)
	else
	{
		if (! fSubTest)
			this->m_totalstats.IncrementFailures();

		pTest->m_pCase->m_stats.IncrementFailures();
	} // end else (its a failure report)

	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update stats event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto DONE;
	} // end if (couldn't set the user's update stats event)


	// Poke mode doesn't automatically request next command.
	if (this->m_dwMode == TNMODE_POKE)
		goto DONE;

	// Subtests and ongoing tests don't request next commands.
	if ((fSubTest) || (fOngoing))
		goto DONE;


	DPL(3, "Getting next commands for any other slaves without current tests.", 0);


	// Loop through all the slaves still in the session with nothing to do
	// and try to get them working on something.
	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pOtherSlave == NULL)
		{
			DPL(0, "Couldn't get slave %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		if (pOtherSlave->m_pCurrentTest != NULL)
		{
			DPL(5, "Slave %s is currently working on test %u, not retrieving next command.",
				2, pOtherSlave->m_szComputerName,
				pOtherSlave->m_pCurrentTest->m_dwUniqueID);

			continue;
		} // end if (slave is working on something)

		this->m_jobs.EnterCritSection();

		// If it's not the completing slave, we should make sure there isn't
		// a GETNEXTCMD for the slave already in the queue.  We don't bother
		// checking the current slave since there should not be one in the
		// queue (however, for DEBUG builds we'll double check to make sure
		// that's the case).
#ifdef DEBUG
		if (pOtherSlave != pSlave)
#endif // DEBUG
		{
			PTNJOB	pJob;
			BOOL	fFound;


			fFound = FALSE;

			for(j = 0; j < this->m_jobs.Count(); j++)
			{
				pJob = (PTNJOB) this->m_jobs.GetItem(j);
				if (pJob == NULL)
				{
					DPL(0, "Couldn't get job %i!", 1, j);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// If we found a GETNEXTCMD job, and it's for this slave,
				// note that this slave should be skipped.
				if ((pJob->m_dwOperation == TNMJ_GETNEXTCMD) &&
					(memcmp(&(pJob->m_idSlave), &(pOtherSlave->m_id), sizeof (TNCTRLMACHINEID)) == 0))
				{
					DPL(2, "Found GETNEXTCMD job for %s (ID %u) already, not adding another one.",
						2, pOtherSlave->m_szComputerName,
						pOtherSlave->m_id.dwTime);
					fFound = TRUE;
					break;
				} // end if (get next command operation)
			} // end for (each job)

			if (fFound)
			{
#ifdef DEBUG
				if (pOtherSlave == pSlave)
				{
					DPL(0, "Found GETNEXTCMD job for current completing slave %s (ID %u)!?  DEBUGBREAK()-ing.",
						2, pSlave->m_szComputerName, pSlave->m_id.dwTime);
					DEBUGBREAK();
				} // end if (it's this current slave)
#endif // DEBUG

				this->m_jobs.LeaveCritSection();
				continue; // skip this slave
			} // end if (found job already)
		} // end if (current slave isn't the completer)

		hr = this->m_jobs.AddJob(TNMJ_GETNEXTCMD, 0, &(pOtherSlave->m_id), NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add GetNextCmd job for %s!",
				1, pOtherSlave->m_szComputerName);

			this->m_jobs.LeaveCritSection();

			goto DONE;
		} // end if (couldn't add job)

		this->m_jobs.LeaveCritSection();
	} // end for (each tester)


DONE:

	if (pTest->m_dwRefCount == 0xFFFFFFFF)
	{
		DPL(0, "Refcount for test %x is screwed!", 1, pTest);
		DEBUGBREAK();
	} // end if (refcount is screwed)

	pTest->m_dwRefCount--;
	if (pTest->m_dwRefCount == 0)
	{
		DPL(0, "WARNING: Nuking the test object %x!?", 1, pTest);
		delete (pTest);
		pTest = NULL;
	}  // end if (that was the last reference)

	if (paTempTestersArray != NULL)
	{
		LocalFree(paTempTestersArray);
		paTempTestersArray = NULL;
	} // end if (allocated array)

	if (paTesterResults != NULL)
	{
		LocalFree(paTesterResults);
		paTesterResults = NULL;
	} // end if (allocated array)

	if (pvFinalCompleteReportData != NULL)
	{
		LocalFree(pvFinalCompleteReportData);
		pvFinalCompleteReportData = NULL;
	} // end if (allocated data)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::CompleteSlavesTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::KillSlave()"
//==================================================================================
// CTNMaster::KillSlave
//----------------------------------------------------------------------------------
//
// Description: Kills the specified slave (adding report, completing tests, etc.).
//				The slave list lock is assumed to be held.
//				It is assumed to be called within the Job thread.
//
// Arguments:
//	PTNSLAVEINFO pSlave		Slave to kill.
//	DWORD dwKillReason		Reason slave is being killed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::KillSlave(PTNSLAVEINFO pSlave, DWORD dwKillReason)
{
	HRESULT				hr;
	int					i;
	int					j;
	PTNSLAVEINFO		pOtherSlave = NULL;
	PTNTESTINSTANCEM	pTest;
	CTNSlavesList		affectedslaves;
	CTNTestMsList		affectedtests;



	DPL(9, "==>(%x [%s], %u)",
		3, pSlave, pSlave->m_szComputerName, dwKillReason);


	DPL(1, "Killing %s, reason = %u.",
		2, pSlave->m_szComputerName, dwKillReason);


	// Flush the send queue
	hr = this->m_pCtrlComm->FlushSendQueue();
	if (hr != S_OK)
	{
		DPL(0, "Flushing control method send queue failed!", 0);
		goto DONE;
	} // end if (flushing send queue failed)


	// Disconnect him (if he wasn't already).
	hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pSlave->m_commdata));
	if (hr != S_OK)
	{
		DPL(0, "WARNING: Couldn't unbind slave %s's data/address!  Ignoring.  %e",
			2, pSlave->m_szComputerName, hr);
		//hr = S_OK;
	} // end if (couldn't unbind data from address)

	// Pull the slave from the session list.
	hr = this->m_slaves.RemoveFirstReference(pSlave);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove %s from slave list!",
			1, pSlave->m_szComputerName);
		goto DONE;
	} // end if (couldn't remove slave)

	// Add him to the list of dropped slaves.
	hr = this->m_droppedslaves.Add(pSlave);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add %s to dropped slave list!",
			1, pSlave->m_szComputerName);
		goto DONE;
	} // end if (couldn't add slave)


	// Add the report for this
	hr = this->m_reports.AddReport(RT_REMOVEMACHINE,
									1, &pSlave,
									NULL,
									dwKillReason,
									NULL, 0);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added
	// is one of the ones we are counting, check to see if that pushed us
	// over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)



	// If in API mode, all of the tests he's scheduled to work on should
	// be marked as skipped.
	if (this->m_dwMode == TNMODE_API)
	{
		this->m_remainingtests.EnterCritSection();
		for(i = 0; i < this->m_remainingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_remainingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get remaining test %i!", 1, i);
				hr = E_FAIL;
				this->m_remainingtests.LeaveCritSection();
				goto DONE;
			} // end if (couldn't get item)

			if (pTest->GetSlavesTesterNum(pSlave) >= 0)
			{
				DPL(0, "WARNING: Remaining test %u has to be skipped because %s is now gone!",
					2, pTest->m_dwUniqueID, pSlave->m_szComputerName);


				hr = this->m_skippedtests.Add(pTest);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
					this->m_remainingtests.LeaveCritSection();
					goto DONE;
				} // end if (couldn't add item)


				// Add the skipped test report.
				hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
												pTest, 0, NULL, 0);
				if (hr != S_OK)
				{
					DPL(0, "Failed to add skip report to list!", 0);
					this->m_remainingtests.LeaveCritSection();
					goto DONE;
				} // end if (failed add report)


				// If we are doing the auto-report thing, and the report
				// we just added is one of the ones we are counting,
				// check to see if that pushed us over the limit.
				if ((this->m_iAutoReportNumReports > 0) &&
					(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
				{
					hr = this->DoAutoReport();
					if (hr != S_OK)
					{
						DPL(0, "Couldn't auto-report!", 0);
						this->m_remainingtests.LeaveCritSection();
						goto DONE;
					} // end if (couldn't print reports)
				} // end if (auto reporting is on and this was a matching report type)


				// Pull it off the list.
				hr = this->m_remainingtests.Remove(i);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't remove remaining test %i (ID %u)!",
						2, i, pTest->m_dwUniqueID);
					this->m_remainingtests.LeaveCritSection();
					goto DONE;
				} // end if (couldn't remove test)

				// Make sure our counter doesn't get out of sync with the
				// adjusted list size.
				i--;
			} // end if (the slave would be working on this test)
		} // end for (each remaining test)
		this->m_remainingtests.LeaveCritSection();
	} // end if (in API mode)



	// Complete the slave's current test, if it exists.
	if (pSlave->m_pCurrentTest != NULL)
	{
		// We're using it.
		pSlave->m_pCurrentTest->m_dwRefCount++;
		pTest = pSlave->m_pCurrentTest;


		DPL(8, "Completing %s's current test (ID = %u).",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);


		// Complete the slave's current test with a failure
		hr = this->CompleteSlavesTest(pSlave, pTest, FALSE, FALSE, FALSE);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't complete %s's current test!",
				1, pSlave->m_szComputerName);
			goto DONE;
		} // end if (couldn't complete test)

		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting current test %x.", 1, pTest);
			delete (pTest);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting current test %x, its refcount is %u.",
				2, pTest, pTest->m_dwRefCount);
		} // end else (no last reference)
		pTest = NULL;
	} // end if (the slave's current test exists)


	// Complete all the slave's ongoing tests.
	do
	{
		pTest = (PTNTESTINSTANCEM) pSlave->m_ongoingtestslist.GetItem(0);
		if (pTest == NULL)
			break;

		pTest->m_dwRefCount++; // we're using it


		DPL(8, "Completing %s's ongoing test ID %u.",
			2, pSlave->m_szComputerName,
			pTest->m_dwUniqueID);

		// Complete the slave's current test with a failure
		hr = this->CompleteSlavesTest(pSlave, pTest, FALSE, FALSE, TRUE);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't one of complete %s's ongoing tests!",
				1, pSlave->m_szComputerName);
			goto DONE;
		} // end if (couldn't complete test)


		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting ongoing test %x.", 1, pTest);
			delete (pTest);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting ongoing test %x, its refcount is %u.",
				2, pTest, pTest->m_dwRefCount);
		} // end else (no last reference)
		pTest = NULL;
	} // end do (while there are ongoing tests)
	while (pTest != NULL);


	// Now we need to check to make sure nobody is sitting in a test
	// waiting for him to join.
	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pOtherSlave == NULL)
		{
			DPL(0, "Couldn't get other slave %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get slave)

		// If the slave has a current test that's not started and
		// the dropped slave is supposed to test it, we gotta change
		// that.
		if ((pOtherSlave->m_pCurrentTest != NULL) &&
			(! pOtherSlave->m_pCurrentTest->m_fStarted) &&
			(pOtherSlave->m_pCurrentTest->GetSlavesTesterNum(pSlave) >= 0))
		{
			for(j = 0; j < pOtherSlave->m_pCurrentTest->m_iNumMachines; j++)
			{
				// If it's the dropped slave, skip it.
				if (pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave == pSlave)
					continue;

				// If it's the current slave, skip it.
				if (pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave == pOtherSlave)
					continue;


				if (pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].dwAssignTime > 0)
				{
#ifdef DEBUG
					// If we've already handled this guy, something's screwy.
					if (affectedslaves.GetFirstIndex(pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave) >= 0)
					{
						DPL(0, "Already handled slave %s!?",
							1, pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_szComputerName);

						DEBUGBREAK();

						hr = E_FAIL;
						goto DONE;
					} // end if (already handled this guy)
#endif // DEBUG

					hr = affectedslaves.Add(pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add slave %x (%s) to affect list!",
							2, pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave,
							pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_szComputerName);
						goto DONE;
					} // end if (couldn't add slave to list)

					// Reset the notification
					//pOtherSlave->m_pCurrentTest->m_paTesterSlots[k].dwAssignTime = 0;

#ifdef DEBUG
					if (pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_pCurrentTest != pOtherSlave->m_pCurrentTest)
					{
						DPL(0, "Slave %s is not currently working on test %x (it's %x)!?",
							3, pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_szComputerName,
							pOtherSlave->m_pCurrentTest,
							pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_pCurrentTest);

						DEBUGBREAK();

						hr = ERROR_BAD_ENVIRONMENT;
						goto DONE;
					} // end if (current test doesn't match)
#endif // DEBUG

					// He's not working on it anymore
					pOtherSlave->m_pCurrentTest->m_paTesterSlots[j].pSlave->m_pCurrentTest = NULL;
					pOtherSlave->m_pCurrentTest->m_dwRefCount--;

					// Assuming refcount won't hit 0
				} // end if (this is not a placeholder)
			} // end for (each tester)

#ifdef DEBUG
			// If we've already handled this test, something's screwy.
			if (affectedtests.GetFirstIndex(pOtherSlave->m_pCurrentTest) >= 0)
			{
				DPL(0, "Already handled test %u!?",
					1, pOtherSlave->m_pCurrentTest->m_dwRefCount);

				DEBUGBREAK();

				hr = E_FAIL;
				goto DONE;
			} // end if (already handled this guy)
#endif // DEBUG

			hr = affectedtests.Add(pOtherSlave->m_pCurrentTest);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add test %x (%u) to affect list!",
					2, pOtherSlave->m_pCurrentTest,
					pOtherSlave->m_pCurrentTest->m_dwRefCount);
				goto DONE;
			} // end if (couldn't add test to list)

#ifdef DEBUG
			// If we've already handled this guy, something's screwy.
			if (affectedslaves.GetFirstIndex(pOtherSlave) >= 0)
			{
				DPL(0, "Already handled slave %s!?",
					1, pOtherSlave->m_szComputerName);

				DEBUGBREAK();

				hr = E_FAIL;
				goto DONE;
			} // end if (already handled this guy)
#endif // DEBUG

			hr = affectedslaves.Add(pOtherSlave);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add slave %x (%s) to affect list!",
					2, pOtherSlave, pOtherSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't add slave to list)

			// Reset the notification
			//pOtherSlave->m_pCurrentTest->m_paTesterSlots[pOtherSlave->m_pCurrentTest->GetSlavesTesterNum(pOtherSlave)].dwAssignTime = 0;


			// We're not working on it anymore
			pOtherSlave->m_pCurrentTest->m_dwRefCount--;
			pOtherSlave->m_pCurrentTest = NULL;

			// Assuming refcount won't hit 0

		} // end if (slave has a current test)
	} // end for (each slave)


	// Loop through all the affected slaves and reassign them to
	// new tests.
	while (affectedslaves.Count() > 0)
	{
		pOtherSlave = (PTNSLAVEINFO) affectedslaves.PopFirstItem();
		if (pOtherSlave == NULL)
		{
			DPL(0, "Couldn't get other slave %i!", 1, j);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get slave)

		if (this->m_dwMode == TNMODE_POKE)
		{
#pragma BUGBUG(vanceo, "What to do?  Notify of failure somehow?")
		} // end if (not in poke mode)
		else
		{
			DPL(0, "%s was affected by %s dropping, reassigning to new test.",
				2, pOtherSlave->m_szComputerName,
				pSlave->m_szComputerName);

			// We can call this directly instead of adding a job for it because
			// this function (KillSlave) should only be being called inside the
			// Job thread itself.
			hr = this->GetNextCmdForMachine(pOtherSlave);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get next command for %s (affected by %s dropping)!",
					2, pOtherSlave->m_szComputerName,
					pSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't get next command)
		} // end else (not in poke mode)

		pOtherSlave->m_dwRefCount--;
		if (pOtherSlave->m_dwRefCount == 0)
		{
			DPL(0, "Slave %x (%s) refcount hit 0!?",
				2, pOtherSlave, pOtherSlave->m_szComputerName);

			DEBUGBREAK();

			delete (pOtherSlave);
			pOtherSlave = NULL;
		} // end if (refcount hit 0)
	} // end while (there are affected slaves left)


	// Loop through all the affected tests and mark them as skipped.
	while (affectedtests.Count() > 0)
	{
		pTest = (PTNTESTINSTANCEM) affectedtests.PopFirstItem();
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get remaining test %i!", 1, j);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)


		DPL(0, "WARNING: Active test %u was affected by %s dropping, skipping it!",
			2, pTest->m_dwUniqueID, pSlave->m_szComputerName);


		hr = this->m_skippedtests.Add(pTest);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
			goto DONE;
		} // end if (couldn't add item)


		// Add the skipped test report.
		hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
										pTest, 0, NULL, 0);
		if (hr != S_OK)
		{
			DPL(0, "Failed to add skip report to list!", 0);
			goto DONE;
		} // end if (failed add report)


		// If we are doing the auto-report thing, and the report
		// we just added is one of the ones we are counting,
		// check to see if that pushed us over the limit.
		if ((this->m_iAutoReportNumReports > 0) &&
			(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
		{
			hr = this->DoAutoReport();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't auto-report!", 0);
				goto DONE;
			} // end if (couldn't print reports)
		} // end if (auto reporting is on and this was a matching report type)

		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test %x.", 1, pTest);
			delete (pTest);
		} // end if (should delete slave)
		else
		{
			DPL(7, "Not deleting test %x, refcount is %u.",
				2, pTest, pTest->m_dwRefCount);
		} // end else (shouldn't delete slave)
		pTest = NULL;
	} // end while (there are affected tests left)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::KillSlave
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CompleteAllTesting()"
//==================================================================================
// CTNMaster::CompleteAllTesting
//----------------------------------------------------------------------------------
//
// Description: Moves any tests left in the remaining test list into the skipped
//				list, adds the appropriate reports, and signals the user.
//				The test list lock is assumed to be held.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::CompleteAllTesting(void)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	int					i;
	PTNTESTINSTANCEM	pTest;
	PTNSLAVEINFO		pSlave;
	char				szTemp[1024];


	if (this->m_fTestingComplete)
	{
		DPL(0, "Testing already complete!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (testing already complete)

	while (this->m_remainingtests.Count() > 0)
	{
		pTest = (PTNTESTINSTANCEM) this->m_remainingtests.GetItem(0);
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get first remaining test!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get first item)

		hr = this->m_remainingtests.RemoveFirstReference(pTest);
		if (hr != S_OK)
		{
			DPL(0, "Failed to remove first test from list!", 0);
			goto DONE;
		} // end if (failed add report)


		DPL(0, "WARNING: Test %u (case %s) was skipped!",
			2, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID);


		hr = this->m_skippedtests.Add(pTest);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
			goto DONE;
		} // end if (couldn't add item)


		// Add the skipped test report.
		hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
										pTest, 0, NULL, 0);
		if (hr != S_OK)
		{
			DPL(0, "Failed to add skip report to list!", 0);
			goto DONE;
		} // end if (failed add report)


		// If we are doing the auto-report thing, and the report we just added is
		// one of the ones we are counting, check to see if that pushed us over the
		// limit.
		if ((this->m_iAutoReportNumReports > 0) &&
			(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
		{
			hr = this->DoAutoReport();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't auto-report!", 0);
				goto DONE;
			} // end if (couldn't print reports)
		} // end if (auto reporting is on and this was a matching report type)
	} // end while (there are still tests left)


	// If we're not in stress mode, having any items still in the repetition list is
	// bad.
	if ((this->m_loadedtests.Count()) && (this->m_dwMode != TNMODE_STRESS))
	{
		DPL(0, "WARNING: All testing finished but %i tests remain in repetition list!",
			1, this->m_loadedtests.Count());

		this->Log(TNLST_CRITICAL,
				"WARNING: All testing finished but %i test%sremain in repetition list!",
				2, this->m_loadedtests.Count(),
				((this->m_loadedtests.Count() == 1) ? " " : "s "));
	} // end if (still loaded tests)


	if ((this->m_skippedtests.Count() > 0) && 
		(this->m_pszSkippedTestsPath != NULL))
	{
		DPL(0, "Writing skipped tests list to %s",
			1, this->m_pszSkippedTestsPath);

		this->Log(TNLST_CONTROLLAYER_INFO,
				"Writing skipped tests list to %s",
				1, this->m_pszSkippedTestsPath);

		hr = FileCreateAndOpenFile(this->m_pszSkippedTestsPath, FALSE, TRUE,
									FALSE, &hFile);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't create and open file \"%s\"!",
				1, this->m_pszSkippedTestsPath);
			goto DONE;
		} // end if (couldn't create skipped file path)

		FileWriteLine(hFile, "//==================================================================================");
		StringGetCurrentDateStr(szTemp);
		FileSprintfWriteLine(hFile, "// Session %010u skipped tests, %s",
							2, this->m_dwSessionID, szTemp);
		FileWriteLine(hFile, "//==================================================================================");

		// Ignoring errors
		FileWriteLine(hFile, "[Info: SkippedTestsList]");


		for (i = 0; i < this->m_skippedtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_skippedtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get skipped test %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get first item)

			hr = pTest->PrintToFile(hFile, TRUE, FALSE);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get print test %x to file!", 1, pTest);
				goto DONE;
			} // end if (couldn't print to file)
		
			// Single space
			FileWriteLine(hFile, "");
		} // end for (each skipped test)

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (should print skipped list file)


	if (this->m_slaves.Count() > 0)
	{
		DPL(0, "WARNING: Testing is complete but %i slaves remain!",
			1, this->m_slaves.Count());

		// Let all of the slaves know that testing is done, then remove them.
		do
		{
			pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(0);
			if (pSlave == NULL)
			{
				DPL(0, "Couldn't get slave %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)


			// If the slave is still working on a test, something's busted.
			if ((pSlave->m_pCurrentTest != NULL) ||
				(pSlave->m_ongoingtestslist.Count() > 0))
			{
				DPL(0, "%s is still working on a test!?",
					1, pSlave->m_szComputerName);
				hr = E_FAIL;
				goto DONE;
			} // end if (there are still tests)


			DPL(0, "Telling %s that testing is complete.",
				1, pSlave->m_szComputerName);


			// Send the message to him.
			hr = this->SendTestingStatusMessageTo(pSlave, CTRLTS_ALLCOMPLETE);
			if (hr != S_OK)
			{
				if (hr != TNERR_CONNECTIONDROPPED)
				{
					DPL(0, "Couldn't send testing complete message to %s!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (the error isn't because he's already gone)
				else
				{
					DPL(0, "Not sending testing complete message to %s because he's gone.",
						1, pSlave->m_szComputerName);
					hr = S_OK;
				} // end else (the slave is already gone)
			} // end if (couldn't get send teststatus message)


			// Flush the send queue
			hr = this->m_pCtrlComm->FlushSendQueue();
			if (hr != S_OK)
			{
				DPL(0, "Flushing control method send queue failed!", 0);
				goto DONE;
			} // end if (flushing send queue failed)


			// Disconnect him (if he wasn't already).
			hr = this->m_pCtrlComm->UnbindDataFromAddress(&(pSlave->m_commdata));
			if (hr != S_OK)
			{
				DPL(0, "WARNING: Couldn't unbind slave %s's data/address!  Ignoring.",
					2, pSlave->m_szComputerName, hr);
				//hr = S_OK;
			} // end if (couldn't unbind data from address)


			// Pull the slave from the session list.
			hr = this->m_slaves.RemoveFirstReference(pSlave);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't remove %s from slave list!",
					1, pSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't remove slave)

			// Add him to the list of dropped slaves.
			hr = this->m_droppedslaves.Add(pSlave);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add %s to dropped slave list!",
					1, pSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't add slave)


			// Add the report for this.
			hr = this->m_reports.AddReport(RT_REMOVEMACHINE,
											1, &pSlave,
											NULL,
											RRM_DONETESTING,
											NULL, 0);
			if (hr != S_OK)
			{
				DPL(0, "Failed to add report to list!", 0);
				goto DONE;
			} // end if (failed add report)


			// If we are doing the auto-report thing, and the report we just added is
			// one of the ones we are counting, check to see if that pushed us over the
			// limit.
			if ((this->m_iAutoReportNumReports > 0) &&
				(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
			{
				hr = this->DoAutoReport();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't auto-report!", 0);
					goto DONE;
				} // end if (couldn't print reports)
			} // end if (auto reporting is on and this was a matching report type)


			// Double check if we have to delete him.
			if (pSlave->m_dwRefCount == 0)
			{
				DPL(0, "Deleting slave %x (%s).", 2, pSlave, pSlave->m_szComputerName);
				delete (pSlave);
			} // end if (should delete slave)
			else
			{
				DPL(7, "Not deleting slave %x (%s), refcount is %u.",
					3, pSlave, pSlave->m_szComputerName, pSlave->m_dwRefCount);
			} // end else (shouldn't deleting slave)
			pSlave = NULL;
		} // end do (while still slaves left)
		while (this->m_slaves.Count() > 0);
	} // end if (there are slaves still)


	// Add the testing completion report.
	hr = this->m_reports.AddReport(RT_ALLTESTINGCOMPLETE, 0, NULL,
									NULL, 0, NULL, 0);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add testing completion report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is one
	// of the ones we are counting, check to see if that pushed us over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	this->m_fTestingComplete = TRUE;


	DPL(0, "All testing complete.", 0);
	// Ignore error
	this->Log(TNLST_CONTROLLAYER_INFO, "All testing complete!", 0);

	if (this->m_skippedtests.Count() > 0)
	{
		DPL(0, "IMPORTANT: %u test%s skipped!",
			2, this->m_skippedtests.Count(),
			((this->m_skippedtests.Count() == 1) ? " was" : "s were"));
		// Ignore error
		this->Log(TNLST_CRITICAL, "IMPORTANT: %u test%s skipped!",
				2, this->m_skippedtests.Count(),
				((this->m_skippedtests.Count() == 1) ? " was" : "s were"));
	} // end if (there were tests skipped)

	if (this->m_totalstats.GetFailures() > 0)
	{
		DPL(0, "IMPORTANT: %u failure%soccurred (not including subtests)!",
			2, this->m_totalstats.GetFailures(),
			((this->m_totalstats.GetFailures() == 1) ? " " : "s "));
		// Ignore error
		this->Log(TNLST_CONTROLLAYER_TESTFAILURE, "IMPORTANT: %u failure%soccurred (not including subtests)!",
				2, this->m_totalstats.GetFailures(),
				((this->m_totalstats.GetFailures() == 1) ? " " : "s "));
	} // end if (there were failures)

	if (this->m_totalstats.GetWarnings() > 0)
	{
		DPL(0, "NOTE: %u warning%soccurred.",
			2, this->m_totalstats.GetWarnings(),
			((this->m_totalstats.GetWarnings() == 1) ? " " : "s "));
		// Ignore error
		this->Log(TNLST_CONTROLLAYER_TESTWARNING, "NOTE: %u warning%soccurred.",
				2, this->m_totalstats.GetWarnings(),
				((this->m_totalstats.GetWarnings() == 1) ? " " : "s "));
	} // end if (there were warnings)

	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update stats event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto DONE;
	} // end if (couldn't set the user's update stats event)


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (opened file)

	return (hr);
} // CTNMaster::CompleteAllTesting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendTestingStatusMessageTo()"
//==================================================================================
// CTNMaster::SendTestingStatusMessageTo
//----------------------------------------------------------------------------------
//
// Description: Sends the passed in machine a testing status message of the type
//				given.
//
// Arguments:
//	PTNSLAVEINFO pSlave		Slave to send to.
//	DWORD dwStatus			Status to send.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendTestingStatusMessageTo(PTNSLAVEINFO pSlave, DWORD dwStatus)
{
	HRESULT						hr;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_TESTINGSTATUS		pTestingStatusMsg = NULL;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send msg (status = %u) to %s because his connection has been dropped!",
			2, dwStatus, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection to slave is down)

	DPL(8, "Sending testing status message (status = %u) to %s.",
		2, dwStatus, pSlave->m_szComputerName);

	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_TESTINGSTATUS);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pTestingStatusMsg = (PCTRLMSG_TESTINGSTATUS) pSendData->m_pvData;
	pTestingStatusMsg->dwSize = pSendData->m_dwDataSize;
	pTestingStatusMsg->dwType = CTRLMSGID_TESTINGSTATUS;
	pTestingStatusMsg->dwStatus = dwStatus;

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so we don't free it below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNMaster::SendTestingStatusMessageTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendNewTestMessageTo()"
//==================================================================================
// CTNMaster::SendNewTestMessageTo
//----------------------------------------------------------------------------------
//
// Description: Instructs the passed in machine to the run the given test.
//
// Arguments:
//	PTNSLAVEINFO pSlave				Slave to send to.
//	PTNTESTINSTANCEM pTest			New test slave is supposed to run.
//	PVOID pvInputData				Pointer to input data to use.
//	DWORD dwInputDataSize			Size of input data to use.
//	PTNSENDDATAQUEUE pSendsBuffer	Queue to use to buffer up the messages.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendNewTestMessageTo(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
										PVOID pvInputData, DWORD dwInputDataSize,
										PTNSENDDATAQUEUE pSendsBuffer)
{
	HRESULT				hr;
	DWORD				dwCaseIDSize;
	DWORD				dwInstanceIDSize;
	PTNSENDDATA			pSendData = NULL;
	PCTRLMSG_NEWTEST	pNewTestMsg = NULL;
	LPBYTE				lpCurrent;
	int					i;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send msg (test %u, case \"%s\", instance \"%s\") to %s because his connection has been dropped!",
			4, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID,
			pTest->m_pLoadedTest->m_pszInstanceID,
			pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection to slave is down)

	DPL(8, "Sending testing status message (test %u, case \"%s\", instance \"%s\") to %s.",
		4, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID,
		pTest->m_pLoadedTest->m_pszInstanceID,
		pSlave->m_szComputerName);

	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);


	dwCaseIDSize = strlen(pTest->m_pCase->m_pszID) + 1;
	dwInstanceIDSize = strlen(pTest->m_pLoadedTest->m_pszInstanceID) + 1;

	pSendData->m_dwDataSize = sizeof (CTRLMSG_NEWTEST)
								+ dwCaseIDSize
								+ dwInstanceIDSize
								+ (pTest->m_iNumMachines * sizeof (TNCTRLMACHINEID))
								+ dwInputDataSize;

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pNewTestMsg = (PCTRLMSG_NEWTEST) pSendData->m_pvData;
	pNewTestMsg->dwSize = pSendData->m_dwDataSize;
	pNewTestMsg->dwType = CTRLMSGID_NEWTEST;
	pNewTestMsg->dwUniqueTestID = pTest->m_dwUniqueID;
	pNewTestMsg->iNumMachines = pTest->m_iNumMachines;
	pNewTestMsg->dwInputDataSize = dwInputDataSize;

	lpCurrent = (LPBYTE) (pNewTestMsg + 1);

	CopyAndMoveDestPointer(lpCurrent, pTest->m_pCase->m_pszID, dwCaseIDSize);
	CopyAndMoveDestPointer(lpCurrent, pTest->m_pLoadedTest->m_pszInstanceID,
							dwInstanceIDSize);

	// Loop through each tester and copy array of IDs into message.
	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		CopyAndMoveDestPointer(lpCurrent,
								&(pTest->m_paTesterSlots[i].pSlave->m_id),
								sizeof (TNCTRLMACHINEID));
	} // end for (each tester)

	if (dwInputDataSize > 0)
	{
		//CopyAndMoveDestPointer(lpCurrent, pvInputData, dwInputDataSize);
		CopyMemory(lpCurrent, pvInputData, dwInputDataSize);
	} // end if (there's input data)

	hr = pSendsBuffer->Add(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to buffer queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so we don't free it below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNMaster::SendNewTestMessageTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendAnnounceTestReplyMessageTo()"
//==================================================================================
// CTNMaster::SendAnnounceTestReplyMessageTo
//----------------------------------------------------------------------------------
//
// Description: Sends the passed in machine an announce test reply message using
//				the given response and unique test IDs.
//
// Arguments:
//	PTNSLAVEINFO pSlave				Slave to send to.
//	DWORD dwResponseID				Response ID to use.
//	DWORD dwUniqueTestID			Unique ID of test generated.
//	PTNSENDDATAQUEUE pSendsBuffer	Queue to use to buffer up the messages.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendAnnounceTestReplyMessageTo(PTNSLAVEINFO pSlave,
													DWORD dwResponseID,
													DWORD dwUniqueTestID,
													PTNSENDDATAQUEUE pSendsBuffer)
{
	HRESULT						hr;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_ANNOUNCETESTREPLY	pAnnounceTestReplyMsg = NULL;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send msg (rid = %u, tid = %u) to %s because his connection has been dropped!",
			3, dwResponseID,  dwUniqueTestID, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection to slave is down)

	DPL(8, "Sending announce test reply message (rid = %u, tid = %u) to %s.",
		3, dwResponseID, dwUniqueTestID, pSlave->m_szComputerName);

	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_ANNOUNCETESTREPLY);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pAnnounceTestReplyMsg = (PCTRLMSG_ANNOUNCETESTREPLY) pSendData->m_pvData;
	pAnnounceTestReplyMsg->dwSize = pSendData->m_dwDataSize;
	pAnnounceTestReplyMsg->dwType = CTRLMSGID_ANNOUNCETESTREPLY;
	pAnnounceTestReplyMsg->dwResponseID = dwResponseID;
	pAnnounceTestReplyMsg->dwUniqueTestID = dwUniqueTestID;

	hr = pSendsBuffer->Add(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to buffer queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so we don't free it below


DONE:

	if (pSendData != NULL)
		delete (pSendData);

	return (hr);
} // CTNMaster::SendAnnounceTestReplyMessageTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendLostTesterTo()"
//==================================================================================
// CTNMaster::SendLostTesterTo
//----------------------------------------------------------------------------------
//
// Description: Sends a message to the given slave that one of the fellow testers
//				exited from the test.  The slave should use that information to
//				abort any syncs, etc.
//
// Arguments:
//	PTNSLAVEINFO pSlave		The slave to send a lost-tester message to.
//	PTNTESTINSTANCEM pTest	The test which lost a tester.
//	int iTesterNum			The tester who was lost.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendLostTesterTo(PTNSLAVEINFO pSlave, PTNTESTINSTANCEM pTest,
									int iTesterNum)
{
	HRESULT					hr;
	PTNSENDDATA				pSendData = NULL;
	PTNTESTINSTANCEM		pTopLevelTest = NULL;
	PCTRLMSG_LOSTTESTER		pLostTesterMsg = NULL;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send LostTester msg (test %u, number %i) to %s because his connection has been dropped!",
			3, pTest->m_dwUniqueID, iTesterNum, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection to slave is down)

	pTopLevelTest = pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test %u!", 1, pTest->m_dwUniqueID);
		goto DONE;
	} // end if (couldn't get top test)


	DPL(3, "Sending lost tester (%i, in %u:%u) to %s.",
		4, iTesterNum, pTopLevelTest->m_dwUniqueID, pTest->m_dwUniqueID,
		pSlave->m_szComputerName);

	// Send a message to this person
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_LOSTTESTER);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pLostTesterMsg = (PCTRLMSG_LOSTTESTER) pSendData->m_pvData;
	pLostTesterMsg->dwSize = pSendData->m_dwDataSize;
	pLostTesterMsg->dwType = CTRLMSGID_LOSTTESTER;
	pLostTesterMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pLostTesterMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pLostTesterMsg->iTesterNum = iTesterNum;
	
	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated a send object)

	return (hr);
} // CTNMaster::SendLostTesterTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendBreakMsgTo()"
//==================================================================================
// CTNMaster::SendBreakMsgTo
//----------------------------------------------------------------------------------
//
// Description: Sends a request to DEBUGBREAK() to the given slave.  The specified
//				test ID is just for debug spew, since the tester may have moved on
//				by the time he gets this message.
//
// Arguments:
//	PTNSLAVEINFO pSlave		The slave to send a lost-tester message to.
//	DWORD dwUniqueTestID	Test that lost the syncer.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendBreakMsgTo(PTNSLAVEINFO pSlave, DWORD dwUniqueTestID)
{
	HRESULT				hr;
	PTNSENDDATA			pSendData = NULL;
	PCTRLMSG_BREAK		pBreakMsg = NULL;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send msg (test %u) to %s because his connection has been dropped!",
			2, dwUniqueTestID, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection is down)

	// Send a message to this person
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_BREAK);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pBreakMsg = (PCTRLMSG_BREAK) pSendData->m_pvData;
	pBreakMsg->dwSize = pSendData->m_dwDataSize;
	pBreakMsg->dwType = CTRLMSGID_BREAK;
	pBreakMsg->dwUniqueTestID = dwUniqueTestID;
	
	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (there's a leftover object)

	return (hr);
} // CTNMaster::SendBreakMsgTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendDumpLogMsgTo()"
//==================================================================================
// CTNMaster::SendDumpLogMsgTo
//----------------------------------------------------------------------------------
//
// Description: Sends a request to dump the memory log to the given slave to the
//				given path.  If the path is NULL, then the slave dumps it to his
//				default test failure log directory if he has one, or ignores the
//				request if he doesn't.
//
// Arguments:
//	PTNSLAVEINFO pSlave		The slave to send a dump log message to.
//	DWORD dwUniqueTestID	Test ID this message pertains to.
//	char* pszPath			Optional path to have slave dump to.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendDumpLogMsgTo(PTNSLAVEINFO pSlave, DWORD dwUniqueTestID,
									char* pszPath)
{
	HRESULT				hr;
	PTNSENDDATA			pSendData = NULL;
	PCTRLMSG_DUMPLOG	pDumpLogMsg = NULL;


	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send msg to %s because his connection has been dropped!",
			1, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection is down)

	// Send a message to this person
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSlave->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSlave->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_DUMPLOG);
	if (pszPath != NULL)
		pSendData->m_dwDataSize += strlen(pszPath);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pDumpLogMsg = (PCTRLMSG_DUMPLOG) pSendData->m_pvData;
	pDumpLogMsg->dwSize = pSendData->m_dwDataSize;
	pDumpLogMsg->dwType = CTRLMSGID_DUMPLOG;
	pDumpLogMsg->dwUniqueTestID = dwUniqueTestID;
	//pDumpLogMsg->dwPathSize = 0;
	if (pszPath != NULL)
	{
		pDumpLogMsg->dwPathSize = strlen(pszPath);

		CopyMemory((pDumpLogMsg + 1), pszPath, pDumpLogMsg->dwPathSize);
	} // end if (there's a path to use)
	
	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (there's a leftover object)

	return (hr);
} // CTNMaster::SendDumpLogMsgTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendMasterUpdate()"
//==================================================================================
// CTNMaster::SendMasterUpdate
//----------------------------------------------------------------------------------
//
// Description: Sends an update message to the meta-master.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendMasterUpdate(void)
{
	HRESULT					hr;
	PTNSENDDATA				pSendData = NULL;
	PCTRLMSG_MASTERUPDATE	pMasterUpdateMsg;

	
	// Send a reply
	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = this->m_dwMetaMasterAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pSendData->m_pvAddress, this->m_pvMetaMasterAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_MASTERUPDATE);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pMasterUpdateMsg = (PCTRLMSG_MASTERUPDATE) pSendData->m_pvData;
	pMasterUpdateMsg->dwSize = pSendData->m_dwDataSize;
	pMasterUpdateMsg->dwType = CTRLMSGID_MASTERUPDATE;

	CopyMemory(&(pMasterUpdateMsg->id), &(this->m_id), sizeof (TNCTRLMACHINEID));
	pMasterUpdateMsg->iNumSlaves = this->m_slaves.Count();
	pMasterUpdateMsg->fJoinersAllowed = this->m_fJoinersAllowed;

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)
	
	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (have a left over send data object)

	return (hr);
} // CTNMaster::SendMasterUpdate
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::RegisterWithMetaMaster()"
//==================================================================================
// CTNMaster::RegisterWithMetaMaster
//----------------------------------------------------------------------------------
//
// Description: Sends a register request to the meta-master.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::RegisterWithMetaMaster(void)
{
	HRESULT						hr;
	PTNSENDDATA					pSendData = NULL;
	char*						pszSelfAddress = NULL;
	DWORD						dwSelfAddressSize = 0;
	PCTRLMSG_REGISTERMASTER		pRegisterMasterMsg = NULL;
	LPBYTE						lpCurrent = NULL;


	// Ignore error, assume ERROR_BUFFER_TOO_SMALL
	this->m_pCtrlComm->GetSelfAddressAsString(NULL, &dwSelfAddressSize);

	pszSelfAddress = (char*) LocalAlloc(LPTR, dwSelfAddressSize);
	if (pszSelfAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	hr = this->m_pCtrlComm->GetSelfAddressAsString(pszSelfAddress,
													&dwSelfAddressSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get self address as string!", 0);
		goto DONE;
	} // end if (couldn't retrieve address)


	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = this->m_dwMetaMasterAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	CopyMemory(pSendData->m_pvAddress, this->m_pvMetaMasterAddress,
			pSendData->m_dwAddressSize);


	pSendData->m_dwDataSize = sizeof (CTRLMSG_REGISTERMASTER)
								+ dwSelfAddressSize - 1;

	if (this->m_pszSessionFilter != NULL)
		pSendData->m_dwDataSize += strlen(this->m_pszSessionFilter);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pRegisterMasterMsg = (PCTRLMSG_REGISTERMASTER) pSendData->m_pvData;
	pRegisterMasterMsg->dwSize = pSendData->m_dwDataSize;
	pRegisterMasterMsg->dwType = CTRLMSGID_REGISTERMASTER;
	pRegisterMasterMsg->dwMetaVersion = CURRENT_METAMASTER_API_VERSION;
	pRegisterMasterMsg->dwControlVersion = CURRENT_TNCONTROL_API_VERSION;
	pRegisterMasterMsg->dwSessionID = this->m_dwSessionID;
	CopyMemory(&(pRegisterMasterMsg->moduleID), &(this->m_moduleID), sizeof (TNMODULEID));
	pRegisterMasterMsg->dwMode = this->m_dwMode;
	pRegisterMasterMsg->dwAddressSize = dwSelfAddressSize - 1;


	lpCurrent = (LPBYTE) (pRegisterMasterMsg + 1);
	CopyAndMoveDestPointer(lpCurrent, pszSelfAddress, dwSelfAddressSize - 1);
	if (this->m_pszSessionFilter != NULL)
	{
		pRegisterMasterMsg->dwSessionFilterSize = strlen(this->m_pszSessionFilter);

		CopyAndMoveDestPointer(lpCurrent, this->m_pszSessionFilter,
								pRegisterMasterMsg->dwSessionFilterSize);
	} // end if (there's a session ID)
	else
		pRegisterMasterMsg->dwSessionFilterSize = 0;	

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated object)

	if (pszSelfAddress != NULL)
	{
		LocalFree(pszSelfAddress);
		pszSelfAddress = NULL;
	} // end if (allocated memory)

	return (hr);
} // CTNMaster::RegisterWithMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::UnregisterWithMetaMaster()"
//==================================================================================
// CTNMaster::UnregisterWithMetaMaster
//----------------------------------------------------------------------------------
//
// Description: Sends an uregistration request to the meta-master.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::UnregisterWithMetaMaster(void)
{
	HRESULT						hr;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_UNREGISTERMASTER	pUnregisterMasterMsg = NULL;



	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = this->m_dwMetaMasterAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, this->m_pvMetaMasterAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_UNREGISTERMASTER);
	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pUnregisterMasterMsg = (PCTRLMSG_UNREGISTERMASTER) pSendData->m_pvData;
	pUnregisterMasterMsg->dwSize = pSendData->m_dwDataSize;
	pUnregisterMasterMsg->dwType = CTRLMSGID_UNREGISTERMASTER;
	CopyMemory(&(pUnregisterMasterMsg->id), &(this->m_id), sizeof (TNCTRLMACHINEID));
	

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated a send object)

	return (hr);
} // CTNMaster::UnregisterWithMetaMaster
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DoBeginReport()"
//==================================================================================
// CTNMaster::DoBeginReport
//----------------------------------------------------------------------------------
//
// Description: Prints some generic information to the file passed in.  Only the
//				parts specified by dwStyle are printed.
//
// Arguments:
//	HANDLE hFile	Open file to print to.
//	DWORD dwStyle	Options on what to print.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::DoBeginReport(HANDLE hFile, DWORD dwStyle)
{
	HRESULT				hr;
	char				szName[MAX_COMPUTERNAME_SIZE];
	DWORD				dwNameSize = MAX_COMPUTERNAME_SIZE;
	int					i;
	PTNSLAVEINFO		pSlave = NULL;
	PTNTESTINSTANCEM	pTest = NULL;
	char				szBuffer[1024];
	time_t 				temptimet;
	tm*					pTempTime;
	CLStringList		startupstrings;
	CTNTestMsList		temptests;


	ZeroMemory(szName, MAX_COMPUTERNAME_SIZE * sizeof (char));

	// Print a comment header if we're not reporting in BVT format.
	if (! this->m_fReportInBVTFormat)
	{
		time(&temptimet);
		pTempTime = localtime(&temptimet);

		// Ignoring write errors
		FileWriteLine(hFile, "//==================================================================================");
		strftime(szBuffer, 1024, "// Report generated on %I:%M:%S%p  %m/%d/%y", pTempTime);
		FileWriteLine(hFile, szBuffer);
		FileWriteLine(hFile, "//==================================================================================");
	} // end if (not printing BVT)

	if (dwStyle & TNRH_SESSIONINFO)
	{
		FileWriteLine(hFile, "[Info: Session]"); //ignoring errors

		FileSprintfWriteLine(hFile, "SessionID=%010u", 1, this->m_dwSessionID);

		pTempTime = localtime(&(this->m_tStartupTime));
		strftime(szBuffer, 1024, "StartTime= %I:%M:%S%p  %m/%d/%y", pTempTime);
		FileWriteLine(hFile, szBuffer); //ignoring errors

		switch (this->m_dwMode)
		{
			case TNMODE_API:
				FileWriteLine(hFile, "Mode= API");
			  break;
			case TNMODE_STRESS:
				FileWriteLine(hFile, "Mode= Stress");
			  break;
			case TNMODE_POKE:
				FileWriteLine(hFile, "Mode= Poke");
			  break;
			case TNMODE_PICKY:
				FileWriteLine(hFile, "Mode= Picky");
			  break;
		} // end switch (on mode)

#ifndef _XBOX // GetComputerName not supported
		if (! GetComputerName(szName, &dwNameSize))
			strcpy(szName, "unknown");
#else // ! XBOX
#pragma TODO(tristanj, "Need to find a replacement for GetComputerName")
#endif // XBOX

		FileSprintfWriteLine(hFile, "MasterName= %s", 1, szName);

		//FileWriteLine(hFile, "ModuleName= ?");

		FileSprintfWriteLine(hFile, "ModuleID= \"%s\", %i.%i.%i.%i",
							5, this->m_moduleID.szBriefName,
							this->m_moduleID.dwMajorVersion,
							this->m_moduleID.dwMinorVersion1,
							this->m_moduleID.dwMinorVersion2,
							this->m_moduleID.dwBuildVersion);

		FileSprintfWriteLine(hFile, "Timelimit= %u", 1, this->m_dwTimelimit);

		FileSprintfWriteLine(hFile, "NewSlavesAllowed=%B",
							1, this->m_fJoinersAllowed);

		FileSprintfWriteLine(hFile, "CurrentNumMachines= %i", 1, this->m_slaves.Count());

		FileSprintfWriteLine(hFile, "NumDroppedMachines= %i",
							1, this->m_droppedslaves.Count());

		FileSprintfWriteLine(hFile, "TotalNumMachines= %i",
							1, this->m_iTotalNumSlaves);

		FileSprintfWriteLine(hFile, "TotalNumTestsRun= %u",
							1, this->m_totalstats.GetTotalComplete());

		FileSprintfWriteLine(hFile, "TotalNumRemainingTests= %i",
							1, this->m_remainingtests.Count());

		FileSprintfWriteLine(hFile, "TotalNumSucceededTests= %u",
							1, this->m_totalstats.GetSuccesses());

		FileSprintfWriteLine(hFile, "TotalNumFailedTests= %u",
							1, this->m_totalstats.GetFailures());

		FileSprintfWriteLine(hFile, "TotalNumWarnings= %u",
							1, this->m_totalstats.GetWarnings());

		FileSprintfWriteLine(hFile, "TotalNumSkipped= %u",
							1, this->m_skippedtests.Count());

		if (this->m_pfnWriteStartupData != NULL)
		{
			hr = this->m_pfnWriteStartupData(&startupstrings);
			if (hr != S_OK)
			{
				DPL(0, "Module's WriteStartupData callback failed!", 0);
				return (hr);
			} // end if (module's callback failed)

			if (startupstrings.Count() > 0)
				FileWriteLine(hFile, "{ModuleStartupData}");

			for (i = 0; i < startupstrings.Count(); i++)
			{
				FileSprintfWriteLine(hFile, "\t%s",
									1, startupstrings.GetIndexedString(i));
			} // end for (each string the module wants)
		} // end if (the module is able to load startup data)

		// The BVT format specification requires an ending block.
		if (this->m_fReportInBVTFormat)
		{
			FileWriteLine(hFile, "[/Info: Session]");
		} // end if (reporting in BVT format)

		// Double space
		FileWriteLine(hFile, "");
		FileWriteLine(hFile, "");
	} // end if (we're supposed to print the session info)

	if (dwStyle & TNRH_MACHINELIST)
	{
		FileWriteLine(hFile, "[Info: TotalMachineList]");

		this->m_slaves.EnterCritSection();
		for(i = 0; i < this->m_slaves.Count(); i++)
		{
			pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
			if (pSlave == NULL)
			{
				DPL(0, "Couldn't get slave %i!", 1, i);
				this->m_slaves.LeaveCritSection();
				return (E_FAIL);
			} // end if (couldn't get that item)

			FileSprintfWriteLine(hFile, "{%s}", 1, pSlave->m_szComputerName);

			FileWriteLine(hFile, "\tStillInSession= TRUE");

			pSlave->PrintToFile(hFile);

			// Single space
			FileWriteLine(hFile, "");
		} // end for (each slave)
		this->m_slaves.LeaveCritSection();


		this->m_droppedslaves.EnterCritSection();
		for(i = 0; i < this->m_droppedslaves.Count(); i++)
		{
			pSlave = (PTNSLAVEINFO) this->m_droppedslaves.GetItem(i);
			if (pSlave == NULL)
			{
				DPL(0, "Couldn't get slave %i!", 1, i);
				this->m_droppedslaves.LeaveCritSection();
				return (E_FAIL);
			} // end if (couldn't get that item)

			FileSprintfWriteLine(hFile, "{%s}", 1, pSlave->m_szComputerName);

			FileWriteLine(hFile, "\tStillInSession= FALSE");

			pSlave->PrintToFile(hFile);

			// Single space
			FileWriteLine(hFile, "");
		} // end for (each slave)
		this->m_droppedslaves.LeaveCritSection();


		// The BVT format specification requires an ending block.
		if (this->m_fReportInBVTFormat)
		{
			FileWriteLine(hFile, "[/Info: TotalMachineList]");
		} // end if (reporting in BVT format)


		// Single space
		FileWriteLine(hFile, "");
	} // end if (we're supposed to print the machine list)

	if (dwStyle & TNRH_ACTIVETESTS)
	{
		FileWriteLine(hFile, "[Info: ActiveTests]");

		this->m_slaves.EnterCritSection();
		for(i = 0; i < this->m_slaves.Count(); i++)
		{
			pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
			if (pSlave == NULL)
			{
				DPL(0, "Couldn't get slave %i!", 1, i);
				this->m_slaves.LeaveCritSection();
				return (E_FAIL);
			} // end if (couldn't get that item)


#pragma BUGBUG(vanceo, "What about ongoing tests?")
			if (pSlave->m_pCurrentTest != NULL)
			{
				if (temptests.GetFirstIndex(pSlave->m_pCurrentTest) < 0)
				{
					hr = pSlave->m_pCurrentTest->PrintToFile(hFile, TRUE, FALSE);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't get print slave %s's current test (%u)!",
							2, pSlave->m_szComputerName,
							pSlave->m_pCurrentTest->m_dwUniqueID);
						this->m_slaves.LeaveCritSection();
						return (hr);
					} // end if (couldn't print to file)

					// Single space
					FileWriteLine(hFile, "");


					// Add it to our used list.
					hr = temptests.Add(pSlave->m_pCurrentTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add slave %s's current test %u!",
							2, pSlave->m_szComputerName,
							pSlave->m_pCurrentTest->m_dwUniqueID);
						this->m_slaves.LeaveCritSection();
						return (hr);
					} // end if (couldn't add test)
				} // end if (haven't already used this test)
			} // end if (slave has a current test)
		} // end for (each slave)
		this->m_slaves.LeaveCritSection();


		// The BVT format specification requires an ending block.
		if (this->m_fReportInBVTFormat)
		{
			FileWriteLine(hFile, "[/Info: ActiveTests]");
		} // end if (reporting in BVT format)


		// Single space
		FileWriteLine(hFile, "");
	} // end if (we're supposed to print the active tests list)


	if (dwStyle & TNRH_REMAININGTESTS)
	{
		FileWriteLine(hFile, "[Info: RemainingTestsList]");

		this->m_remainingtests.EnterCritSection();
		for(i = 0; i < this->m_remainingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_remainingtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get test item %i!", 1, i);
				this->m_remainingtests.LeaveCritSection();
				return (E_FAIL);
			} // end if (couldn't get that item)

			hr = pTest->PrintToFile(hFile, TRUE, FALSE);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get print test %i to file!", 1, i);
				this->m_remainingtests.LeaveCritSection();
				return (hr);
			} // end if (couldn't print to file)
		
			// Single space
			FileWriteLine(hFile, "");
		} // end for (each test)
		this->m_remainingtests.LeaveCritSection();

		// The BVT format specification requires an ending block.
		if (this->m_fReportInBVTFormat)
		{
			FileWriteLine(hFile, "[/Info: RemainingTestsList]");
		} // end if (reporting in BVT format)


		// Single space
		FileWriteLine(hFile, "");
	} // end if (we're supposed to print the remaining test list)

	if (dwStyle & TNRH_SKIPPEDTESTS)
	{
		FileWriteLine(hFile, "[Info: SkippedTestsList]");

		this->m_skippedtests.EnterCritSection();
		for(i = 0; i < this->m_skippedtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_skippedtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get test item %i!", 1, i);
				this->m_skippedtests.LeaveCritSection();
				return (E_FAIL);
			} // end if (couldn't get that item)

			hr = pTest->PrintToFile(hFile, TRUE, FALSE);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get print test %i to file!", 1, i);
				this->m_skippedtests.LeaveCritSection();
				return (hr);
			} // end if (couldn't print to file)
		
			// Single space
			FileWriteLine(hFile, "");
		} // end for (each test)
		this->m_skippedtests.LeaveCritSection();

		// The BVT format specification requires an ending block.
		if (this->m_fReportInBVTFormat)
		{
			FileWriteLine(hFile, "[/Info: SkippedTestsList]");
		} // end if (reporting in BVT format)


		// Single space
		FileWriteLine(hFile, "");
	} // end if (we're supposed to print the skipped test list)

	return (S_OK);
} // CTNMaster::DoBeginReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CountMatchingReports()"
//==================================================================================
// CTNMaster::CountMatchingReports
//----------------------------------------------------------------------------------
//
// Description: Returns the number of reports in the log that match the given flags.
//
// Arguments:
//	DWORD dwFlags				What in the report log should be counted.
//	PTNSLAVESLIST pMachineList	List of machines to filter reports by, or NULL for
//								none.
//	PTNTESTSLIST pTestList		List of tests to filter reports by, or NULL for
//								none.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
int CTNMaster::CountMatchingReports(DWORD dwFlags, PTNSLAVESLIST pMachineList,
									PTNTESTMSLIST pTestList)
{
	int			iCount = 0;
	PTNREPORT	pReport = NULL;


	pReport = NULL;
	this->m_reports.EnterCritSection();
	do
	{
		pReport = this->m_reports.GetNextMatchingReport(pReport, dwFlags,
														pMachineList, pTestList,
														FALSE);
		if (pReport == NULL)
		{
			break;
		} // end if (no more matching reports)

		iCount++;
	} // end do (while more reports)
	while (TRUE);

	this->m_reports.LeaveCritSection();


	return (iCount);
} // CTNMaster::CountMatchingReports
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DoAutoReport()"
//==================================================================================
// CTNMaster::DoAutoReport
//----------------------------------------------------------------------------------
//
// Description: Performs the autoprinting.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::DoAutoReport(void)
{
	HRESULT		hr = S_OK;



	this->m_reports.EnterCritSection();

	// Only do the auto-reporting if it's time.
	if (this->CountMatchingReports(this->m_dwAutoReportFlags, NULL, NULL) >= this->m_iAutoReportNumReports)
	{
		char	szTemp[256];


		hr = this->PrintReports(this->m_dwAutoReportHeaders,
								this->m_dwAutoReportBodyStyle,
								this->m_dwAutoReportFlags,
								szTemp,
								TRUE,
								TRUE);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-print reports to \"%s\"!", 1, szTemp);
			goto DONE;
		} // end if (couldn't print reports)


		// If the user wants the log to be empty after a print (even if we
		// didn't print all of the items in it), clear the remaining items.
		if (this->m_fAutoReportDeleteNonPrinted)
		{
			hr = this->m_reports.RemoveAll();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't remove remaining reports!", 0);
				goto DONE;
			} // end if (couldn't print reports)
		} // end if (should delete remaining reports)
	} // end if (that report put us over the limit)


DONE:

	this->m_reports.LeaveCritSection();

	return (hr);
} // CTNMaster::DoAutoReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::CloseSession()"
//==================================================================================
// CTNMaster::CloseSession
//----------------------------------------------------------------------------------
//
// Description: Closes the session, and performs all the appropriate actions.
//				The slave list lock is assumed to be held with only one level of
//				recursion.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::CloseSession()
{
	HRESULT			hr;
	int				i;
	PTNJOB			pJob;
	PTNSLAVEINFO	pSlave;
	BOOL			fAllReady = TRUE;


	// Make sure no one can join, in case they currently can.
	this->m_fJoinersAllowed = FALSE;


	DPL(0, "Closing session, no additional joiners will be allowed.", 0);
	// Ignoring error

	this->Log(TNLST_CONTROLLAYER_INFO,
			"Closing session, no additional joiners will be allowed.", 0);



	// Make sure there's no duplicate CloseSession jobs pending.

	this->m_jobs.EnterCritSection();

	for(i = 0; i < this->m_jobs.Count(); i++)
	{
		pJob = (PTNJOB) this->m_jobs.GetItem(i);
		if (pJob == NULL)
		{
			DPL(0, "Couldn't get job %i!", 1, i);
			this->m_jobs.LeaveCritSection();
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		if (pJob->m_dwOperation == TNMJ_CLOSESESSION)
		{
			hr = this->m_jobs.Remove(i);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't remove job %i!", 1, i);
				this->m_jobs.LeaveCritSection();
				goto DONE;
			} // end if (couldn't remove item)

			i--; // back up so we don't screw up our index
		} // end if (it's a close session)
	} // end for (each job)

	this->m_jobs.LeaveCritSection();



	// Add the report.
	hr = this->m_reports.AddReport(RT_CLOSEDSESSION, 0, NULL,
									NULL, 0, NULL, 0);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add session close report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is
	// one of the ones we are counting, check to see if that pushed us over the
	// limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	// If we're registered with a meta-master, we should send a master update
	// message.
	if (this->m_fRegistered)
	{
		hr = this->SendMasterUpdate();
		if (hr != S_OK)
		{
			DPL(0, "Failed to send master update to meta-master!", 0);
			goto DONE;
		} // end if (failed send master update)
	} // end if (we're registered with a metamaster)


	// Ping the event to let the app know our new status.
	if ((this->m_hCompletionOrUpdateEvent != NULL) &&
		(! SetEvent(this->m_hCompletionOrUpdateEvent)))
	{
		hr = GetLastError();
		DPL(0, "Couldn't set the user's update event (%x)!",
			1, this->m_hCompletionOrUpdateEvent);
		goto DONE;
	} // end if (couldn't set the user's update event)



	// If we're in API mode, we need to kick everybody off in their tests
	// because we were waiting for the session to close.
	// If we're in stress mode, we need to see if everybody who
	// isn't already running a test actually can now that the session is closed.
	if (this->m_dwMode != TNMODE_POKE) 
	{
		if (this->m_dwMode == TNMODE_API)
		{
			for(i = 0; i < this->m_slaves.Count(); i++)
			{
				pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// If even one other guy is not ready to test, we don't want to
				// make any assumptions yet.
				if (! pSlave->m_fReadyToTest)
				{
					fAllReady = FALSE;
					break;
				} // end if (not ready)
			} // end for (each tester)
		} // end if (in API mode)

		if (fAllReady)
		{
			if (this->m_dwMode == TNMODE_API)
			{
				DPL(1, "Session closed and all slaves are currently ready, getting commands.", 0);
			} // end if (in API mode)
			else
			{
				DPL(6, "Refreshing all slaves ready to test with nothing to do currently.", 0);
			} // end else (not in API mode)

			for(i = 0; i < this->m_slaves.Count(); i++)
			{
				pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// This (sort of) races with ReadyToTest, plus in stress mode he
				// may have already started on something, so leave him alone if
				// he's already cookin'.
				if ((pSlave->m_fReadyToTest) && (pSlave->m_pCurrentTest == NULL))
				{
					hr = this->GetNextCmdForMachine(pSlave);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't get next test/command for %s!",
							1, pSlave->m_szComputerName);
						goto DONE;
					} // end if (failed to get next cmd)
				} // end if (in API mode or this guy is idle)
			} // end for (each tester)
		} // end if (all testers are ready)
	} // end if (not in Poke mode)


DONE:

	return (hr);
} // CTNMaster::CloseSession
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DoPingCheck()"
//==================================================================================
// CTNMaster::DoPingCheck
//----------------------------------------------------------------------------------
//
// Description: Pings slaves to see if they are still alive, if it hasn't heard back
// 				in TIMEOUT_SLAVE_PING milliseconds.  If it hasn't heard back in
//				TIMEOUT_SLAVE_KILL milliseconds, the slave will be killed.
//				The slave list lock is assumed to be held.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::DoPingCheck(void)
{
	HRESULT				hr = S_OK;
	PTNSLAVEINFO		pSlave = NULL;
	DWORD				dwNotHeardTime;
	int					i;
	DWORD				dwKillReason;
	BOOL				fKilledSlave = FALSE;


	DPL(9, "==>", 0);


	// Go through the list of slaves and ping those not heard from
	// in SLAVE_CHECK_INTERVAL milliseconds. If we haven't heard from
	// a slave in TIMEOUT_SLAVE_PING milliseconds, kill it.

	for(i = 0; i < this->m_slaves.Count(); i++)
	{
		pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't get slave %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)
		
		
		pSlave->m_dwRefCount++; // we're using it


		dwKillReason = 0;

		// Check if the slave has disconnected.
		if (pSlave->m_commdata.fDropped)
		{
			DPL(0, "Connection to %s dropped, removing him.",
				1, pSlave->m_szComputerName);

			// Ignoring error
			this->Log(TNLST_CONTROLLAYER_INFO,
					"Connection to %s dropped, removing him.",
					1, pSlave->m_szComputerName);

			dwKillReason = RRM_LEFT;
		} // end if (connection dropped)
		else
		{
			// If we're timing and this slave is past the time he should be done
			// testing, tell him and then remove him.
			if ((pSlave->m_dwEndtime > 0) &&
				(GetTickCount() > pSlave->m_dwEndtime))
			{
				DPL(0, "%s has passed the %i minute timelimit, removing and notifying him.",
					2, pSlave->m_szComputerName, this->m_dwTimelimit);

				// Send the message to him.
				hr = this->SendTestingStatusMessageTo(pSlave, CTRLTS_ALLCOMPLETE);
				if (hr != S_OK)
				{
					if (hr != TNERR_CONNECTIONDROPPED)
					{
						DPL(0, "Couldn't send testing complete message to %s!",
							1, pSlave->m_szComputerName);
						goto DONE;
					} // end if (the error isn't because he's already gone)
					else
					{
						DPL(0, "Not sending testing complete message to %s because he's already gone.",
							1, pSlave->m_szComputerName);
						hr = S_OK;
					} // end else (the slave is already gone)
				} // end if (couldn't get send teststatus message)

				// NOTE: If the slave also happens to be killed by lack of ping
				// replies, then this will be overwritten.  Oh well.
				dwKillReason = RRM_TIMEREXPIRED;
			} // end if (there are no tests left)


			// Get the current time and the last access time 
			// of the slave

			dwNotHeardTime = pSlave->TimeSinceLastReceive();
			if (dwNotHeardTime > TIMEOUT_SLAVE_KILL)
			{
				DPL(0, "Haven't heard from %s in %u ticks, killing him.",
					2, pSlave->m_szComputerName, dwNotHeardTime);

				// Ignoring error
				this->Log(TNLST_CONTROLLAYER_INFO,
						"Haven't heard from %s in %u ticks, killing him.",
						2, pSlave->m_szComputerName, dwNotHeardTime);

				dwKillReason = RRM_DROPPED;
			} // end if (time to kill the slave)
			else if (dwNotHeardTime > TIMEOUT_SLAVE_PING)
			{
				// If we've already sent the maximum # of pings, skip this guy
				if (pSlave->PingsSinceLastReceive() >= NUM_PINGS_TO_ATTEMPT)
				{
					DPL(5, "Still haven't heard from %s (%u ticks), but not pinging.",
						2, pSlave->m_szComputerName, dwNotHeardTime);
				} // end if (we've already sent the max number of pings)
				else
				{
					// Send a ping to the slave
					hr = this->SendPingTo(pSlave);
					if (hr != S_OK)
					{
						if (hr != TNERR_CONNECTIONDROPPED)
						{
							DPL(0, "Sending ping to slave %s failed!",
								1, pSlave->m_szComputerName);
							goto DONE;
						} // end if (the error isn't because he's already gone)
						else
						{
							DPL(0, "Not sending ping to %s because he's already gone.",
								1, pSlave->m_szComputerName);

							dwKillReason = RRM_LEFT;
							hr = S_OK;
						} // end else (the slave is already gone)
					} // end if (failed sending ping)
				} // end else (haven't sent maximum number of pings)
			} // end else if (should ping the slave)
		} // end else (the connection hasn't dropped yet)

		if (dwKillReason != 0)
		{
			// This removes him from the list.
			hr = this->KillSlave(pSlave, dwKillReason);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't kill %s!", 1, pSlave->m_szComputerName);
				goto DONE;
			} // end if (couldn't kill slave)

			// Back up so our counter doesn't get out of sync with our
			// new slave list size.
			i--;

			fKilledSlave = TRUE;


			// If the session is closed and we just killed the last slave,
			// then make sure testing is done.
			if ((! this->m_fJoinersAllowed) &&
				(this->m_slaves.Count() == 0))
			{
				hr = this->CompleteAllTesting();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't complete all testing!", 0);
					goto DONE;
				} // end if (couldn't complete all testing)
			} // end if (session closed and last slave gone)
		} // end if (should kill the slave)


		// Check if we have to delete him.
		pSlave->m_dwRefCount--;
		if (pSlave->m_dwRefCount == 0)
		{
			DPL(7, "Deleting slave %x.", 1, pSlave);
			delete (pSlave);
		} // end if (should delete slave)
		else
		{
			DPL(7, "Not deleting slave %x, refcount is %u.",
				2, pSlave, pSlave->m_dwRefCount);
		} // end else (shouldn't delete slave)
		pSlave = NULL;
	} // end for (each slave)


	// If our slave list changed, we need to update the meta-master if we're
	// registered with one, and notify the app.
	if (fKilledSlave)
	{
		// If we're registered with a meta-master, we should send a master update
		// message.
		if (this->m_fRegistered)
		{
			hr = this->SendMasterUpdate();
			if (hr != S_OK)
			{
				DPL(0, "Failed to send master update to meta-master!", 0);
				goto DONE;
			} // end if (failed send master update)
		} // end if (we're registered with a metamaster)


		// Ping the event to let the app know our new status.
		if ((this->m_hCompletionOrUpdateEvent != NULL) &&
			(! SetEvent(this->m_hCompletionOrUpdateEvent)))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set the user's update event (%x)!",
				1, this->m_hCompletionOrUpdateEvent);
			goto DONE;
		} // end if (couldn't set the user's update event)
	} // end if (our status changed)


DONE:

	if (pSlave != NULL)
	{
		pSlave->m_dwRefCount--;
		if (pSlave->m_dwRefCount == 0)
		{
			DPL(7, "Deleting still held slave %x.", 1, pSlave);
			delete (pSlave);
		} // end if (should delete slave)
		else
		{
			DPL(7, "Not deleting still held slave %x, refcount is %u.",
				2, pSlave, pSlave->m_dwRefCount);
		} // end else (shouldn't delete slave)
		pSlave = NULL;
	} // end if (have slave object)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::DoPingCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DoSlaveReadyToTest()"
//==================================================================================
// CTNMaster::DoSlaveReadyToTest
//----------------------------------------------------------------------------------
//
// Description: Marks the given slave as ready to test, and kicks off testing as
//				appropriate.
//				The slave list lock is assumed to be held.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::DoSlaveReadyToTest(PTNCTRLMACHINEID pSlaveID)
{
	HRESULT			hr = S_OK;
	PTNSLAVEINFO	pSlave = NULL;
	int				i;
	BOOL			fAllReady = TRUE;


	pSlave = this->m_slaves.GetSlaveByID(pSlaveID);
	if (pSlave == NULL)
	{
		DPL(0, "Couldn't find slave (%u) in list!  Ignoring ReadyToTest request.",
			1, pSlaveID->dwTime);

		// BUGBUG Fail?
		goto DONE;
	} // end if (couldn't get slaveinfo object)


#pragma BUGBUG(vanceo, "This is kinda weak (doing it here)...")
	hr = pSlave->UpdateLastReceiveTime();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't update slave's last receive time!", 0);
		goto DONE;
	} // end if (couldn't update receive time)


#ifdef DEBUG
	if (pSlave->m_fReadyToTest)
	{
		DPL(0, "%s has already indicated readiness to test!",
			1, pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (slave already marked ready to test)
#endif // DEBUG


	DPL(2, "Slave %s is now ready to test.", 1, pSlave->m_szComputerName);

	pSlave->m_fReadyToTest = TRUE;


	// If we're in Poke mode, we don't do any command retrieval.
	if (this->m_dwMode == TNMODE_POKE)
	{
		goto DONE;
	} // end if (in poke mode)


	// If the session is still open and we're in API mode, we can't do
	// any command retrieval either.
	if ((this->m_fJoinersAllowed) &&
		(this->m_dwMode == TNMODE_API))
	{
		goto DONE;
	} // end if (the session is still open)


	// If we're in API mode, we need to see if this was the last slave waiting
	// to get ready.  If so, we need to kick everybody off in their tests
	// because we were waiting for the session to close.
	// If we're in stress mode, we need to see if everybody who
	// isn't already running a test actually can now that a new slave joined.
	if (this->m_dwMode != TNMODE_POKE) 
	{
		if (this->m_dwMode == TNMODE_API)
		{
			for(i = 0; i < this->m_slaves.Count(); i++)
			{
				pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// If even one other guy is not ready to test, we don't want to
				// make any assumptions yet.
				if (! pSlave->m_fReadyToTest)
				{
					fAllReady = FALSE;
					break;
				} // end if (not ready)
			} // end for (each tester)
		} // end if (in API mode)

		if (fAllReady)
		{
			if (this->m_dwMode == TNMODE_API)
			{
				DPL(1, "Last slave to get ready, getting commands for all slaves.", 0);
			} // end if (in API mode)
			else
			{
				DPL(6, "Refreshing all slaves ready to test with nothing to do currently.", 0);
			} // end else (not in API mode)

			for(i = 0; i < this->m_slaves.Count(); i++)
			{
				pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)


				// This (sort of) races with CloseSession, plus in stress mode he
				// may have already started on something, so leave him alone if
				// he's already cookin'.
				if ((pSlave->m_fReadyToTest) && (pSlave->m_pCurrentTest == NULL))
				{
					hr = this->GetNextCmdForMachine(pSlave);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't get next test/command for %s!",
							1, pSlave->m_szComputerName);
						goto DONE;
					} // end if (failed to get next cmd)
				} // end if (in API mode or this guy is idle)
			} // end for (each tester)
		} // end if (all testers are ready)
	} // end if (not in Poke mode)


DONE:

	return (hr);
} // CTNMaster::DoSlaveReadyToTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::DoHandleReport()"
//==================================================================================
// CTNMaster::DoHandleReport
//----------------------------------------------------------------------------------
//
// Description: Handles a report from a slave.
//				The slave list lock is assumed to be held.
//
// Arguments:
//	PTNSLAVEINFO pSlave				Slave who is reporting.
//	PCTRLMSG_REPORT pReportMsg		The message received.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::DoHandleReport(PTNSLAVEINFO pSlave, PCTRLMSG_REPORT pReportMsg)
{
	HRESULT				hr;
	PTNTESTINSTANCEM	pTest = NULL;
	BOOL				fHaveTestRef = FALSE;
	BOOL				fSubTest = FALSE;
	int					iTesterNum = -1;
	DWORD				dwReportType = 0;
	BOOL				fOngoing = FALSE;
	int					i;
	BOOL				fDebugBreak = FALSE;


	DPL(9, "==>(%x [%s], %x)", 3, pSlave, pSlave->m_szComputerName, pReportMsg);

	pTest = pSlave->GetTest(pReportMsg->dwTopLevelUniqueID, pReportMsg->dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get test %u:%u %s is reporting on!",
			3, pReportMsg->dwTopLevelUniqueID, pReportMsg->dwTestUniqueID,
			pSlave->m_szComputerName);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get test ID)

	if (pReportMsg->dwTopLevelUniqueID != pReportMsg->dwTestUniqueID)
		fSubTest = TRUE;

	if (pSlave->m_ongoingtestslist.GetTopLevelTest(pReportMsg->dwTopLevelUniqueID) != NULL)
		fOngoing = TRUE;


	// Make sure we hold on to it so no one deletes it behind our backs.
	pTest->m_dwRefCount++;
	fHaveTestRef = TRUE;


	iTesterNum = pTest->GetSlavesTesterNum(pSlave);

	// If we didn't find it, then bail.
	if (iTesterNum < 0)
	{
		DPL(0, "Couldn't find the reporting slave (%s) in %i's tester list!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find the reporting tester in the list)


	// Make sure the test is cookin'.
	if (! pTest->m_fStarted)
	{
		DPL(0, "%s is trying to report on a test (ID %u) which hasn't started yet?!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (the test hasn't started)

	// Make sure the dude hasn't already claimed he finished it.
	if (pTest->m_paTesterSlots[iTesterNum].fComplete)
	{
		DPL(0, "%s is trying to report on a test (ID %u) he already said he finished?!",
			2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (he already finished the test)

	

	if (pReportMsg->fSuccess)
	{
		if (fSubTest)
			dwReportType = RT_SUCCESS_SUBTEST;
		else
			dwReportType = RT_SUCCESS;
	} // end if (we're reporting a success)
	else
	{
		if (fSubTest)
			dwReportType = RT_FAILURE_SUBTEST;
		else
			dwReportType = RT_FAILURE;
	} // end else (we're reporting a failure)

	hr = this->m_reports.AddReport(dwReportType,
								1, &pSlave,
								pTest,
								pReportMsg->hresult,
								((pReportMsg->dwOutputDataSize > 0) ? (pReportMsg + 1) : NULL),
								pReportMsg->dwOutputDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add report to list!", 0);
		goto DONE;
	} // end if (failed add report)


	// If we are doing the auto-report thing, and the report we just added is one
	// of the ones we are counting, check to see if that pushed us over the limit.
	if ((this->m_iAutoReportNumReports > 0) &&
		((pReportMsg->fSuccess) &&
			(this->m_dwAutoReportFlags & (TNREPORT_DURINGTESTSUCCESSES | TNREPORT_TESTASSIGNMENTSTESTSUCCEEDED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTSUCCEEDED))) ||
		((! pReportMsg->fSuccess) &&
			(this->m_dwAutoReportFlags & (TNREPORT_DURINGTESTFAILURES | TNREPORT_TESTASSIGNMENTSTESTFAILED | TNREPORT_TESTASSIGNMENTCOMPLETIONSTESTFAILED))))
	{
		hr = this->DoAutoReport();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't auto-report!", 0);
			goto DONE;
		} // end if (couldn't print reports)
	} // end if (auto reporting is on and this was a matching report type)


	DPL(0, "%s %s %sest unique ID %u:%u %s, hr = %x, data size = %u, data = %x",
		9, pSlave->m_szComputerName,
		((pReportMsg->fTestComplete) ? "completed" : "is continuing"),
		((fSubTest) ? "subt" : "t"),
		pReportMsg->dwTopLevelUniqueID,
		pReportMsg->dwTestUniqueID,
		((pReportMsg->fSuccess) ? "successfully" : "with failure"),
		pReportMsg->hresult,
		pReportMsg->dwOutputDataSize,
		((pReportMsg->dwOutputDataSize > 0) ? (pReportMsg + 1) : NULL));

	this->Log(((pReportMsg->fSuccess) ? TNLST_CONTROLLAYER_TESTSUCCESS : TNLST_CONTROLLAYER_TESTFAILURE),
					"%s %s test unique ID %u %s.",
					4, pSlave->m_szComputerName,
					((pReportMsg->fTestComplete) ? "completed" : "is continuing"),
					pReportMsg->dwTestUniqueID,
					((pReportMsg->fSuccess) ? "successfully" : "with failure"));

	this->Log(((pReportMsg->fSuccess) ? TNLST_CONTROLLAYER_TESTSUCCESS : TNLST_CONTROLLAYER_TESTFAILURE),
				"Test result = %e", 1, pReportMsg->hresult);

	if ((! pReportMsg->fSuccess) && 
		(this->m_pszSiblingFailuresMemDumpDirPath != NULL))
	{
		for(i = 0; i < pTest->m_iNumMachines; i++)
		{
			// Make sure this tester exists, and isn't the guy doing the reporting.
			if (pTest->m_paTesterSlots[i].pSlave != NULL)
			{
				if (pTest->m_paTesterSlots[i].pSlave != pSlave)
				{
					char*	pszFilepath;
					char	szTemp[256];
					DWORD	dwSize;


					// If the user specified "", then have the slaves use their default
					// directory, otherwise build a path for the slave.
					pszFilepath = NULL;
					if (strcmp(this->m_pszSiblingFailuresMemDumpDirPath, "") != 0)
					{
						wsprintf(szTemp, "req_memlog_%stest_%010u_",
								((fSubTest) ? "subt" : "t"),
								pTest->m_dwUniqueID);

						dwSize = strlen(this->m_pszSiblingFailuresMemDumpDirPath)
								+ strlen(szTemp)
								+ strlen(pTest->m_paTesterSlots[i].pSlave->m_szComputerName)
								+ 5; // ".txt" + NULL termination
						// Make sure it ends with a backslash
						if (! StringEndsWith(this->m_pszSiblingFailuresMemDumpDirPath, "\\", TRUE))
							dwSize++;

						pszFilepath = (char*) LocalAlloc(LPTR, dwSize);
						if (pszFilepath == NULL)
						{
							hr = E_OUTOFMEMORY;
							goto DONE;
						} // end if (couldn't allocate memory)

						strcpy(pszFilepath, this->m_pszSiblingFailuresMemDumpDirPath);
						if (! StringEndsWith(this->m_pszSiblingFailuresMemDumpDirPath, "\\", TRUE))
							strcat(pszFilepath, "\\");
						strcat(pszFilepath, szTemp);
						strcat(pszFilepath, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
						strcat(pszFilepath, ".txt");
					} // end if (should build path)

					hr = this->SendDumpLogMsgTo(pTest->m_paTesterSlots[i].pSlave,
												pTest->m_dwUniqueID,
												pszFilepath);
					if (pszFilepath != NULL)
					{
						LocalFree(pszFilepath);
						pszFilepath = NULL;
					} // end if (allocated filepath)

					if (hr != S_OK)
					{
						if (hr != TNERR_CONNECTIONDROPPED)
						{
							DPL(0, "Couldn't send dump log message to %s!",
								1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
							goto DONE;
						} // end if (not connectiondropped)
						else
						{
							DPL(0, "WARNING: Couldn't have %s dump his log because he's gone!",
								1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
							hr = S_OK;
						} // end else (connection dropped)
					} // end if (failed to send message)
				} // end if (it's not the reporting tester)
			} // end if (there's a tester here)
			else
			{
				DPL(0, "WARNING: Can't tell tester %i to dump the log because slot isn't filled yet!",
					1, i);
			} // end else (there's no tester assigned yet)

		} // end for (each possible tester slot)
	} // end if (it's a failure)


	// If it's a failure report, check to see if we should have all the testers
	// drop into the debugger
	if ((! pReportMsg->fSuccess) && (this->m_pFailureBreakRules != NULL))
	{
		char		szNumber[32];
		int			i;
		PLSTRING	pBreakRule;
		char*		pszTemp;
		DWORD		dwNumFields;
		

		wsprintf(szNumber, "0x%08x", pReportMsg->hresult);

		this->m_pFailureBreakRules->EnterCritSection();
		for(i = 0; i < this->m_pFailureBreakRules->Count(); i++)
		{
			pBreakRule = (PLSTRING) this->m_pFailureBreakRules->GetItem(i);
			if (pBreakRule == NULL)
			{
				DPL(0, "Couldn't get failure break rule %i!", 1, i);
				this->m_pFailureBreakRules->LeaveCritSection();
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get break rule)

			pszTemp = (char*) LocalAlloc(LPTR, strlen(pBreakRule->GetString()) + 1);
			if (pszTemp == NULL)
			{
				this->m_pFailureBreakRules->LeaveCritSection();
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't get break rule)

			// Ignoring possible return of 0.
			dwNumFields = StringSplitIntoFields(pBreakRule->GetString(), " ",
												pszTemp, NULL);

			fDebugBreak = StringMatchesWithWildcard(StringGetFieldPtr(pszTemp, 0),
													pTest->m_pCase->m_pszID,
													TRUE);
			if ((fDebugBreak) && (dwNumFields >= 2))
			{
				fDebugBreak = StringMatchesWithWildcard(StringGetFieldPtr(pszTemp, 1),
														szNumber,
														TRUE);
			} // end if (only one field)

			LocalFree(pszTemp);
			pszTemp = NULL;

			// If we matched, stop searching.
			if (fDebugBreak)
			{
				DPL(5, "\"%s %x\" matched \"%s\"", 3, pTest->m_pCase->m_pszID,
					pReportMsg->hresult, pBreakRule->GetString());
				break;
			} // end if (matched)
		} // end for (each failure break rule)
		this->m_pFailureBreakRules->LeaveCritSection();


		if (fDebugBreak)
		{
			//Ignoring error
			this->Log(TNLST_CRITICAL,
					"Failure for test %u (case %s, \"%s\", result = %x) fits break rules, telling testers to DebugBreak().",
					4, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID,
					pTest->m_pCase->m_pszName, pReportMsg->hresult);
			
			DPL(0, "Failure for test %u (case %s, \"%s\", result = %x) fits break rules, telling testers to DebugBreak().",
				4, pTest->m_dwUniqueID, pTest->m_pCase->m_pszID,
				pTest->m_pCase->m_pszName, pReportMsg->hresult);

			for(i = 0; i < pTest->m_iNumMachines; i++)
			{
				if (pTest->m_paTesterSlots[i].pSlave != NULL)
				{
					hr = this->SendBreakMsgTo(pTest->m_paTesterSlots[i].pSlave,
												pTest->m_dwUniqueID);
					if (hr != S_OK)
					{
						if (hr != TNERR_CONNECTIONDROPPED)
						{
							DPL(0, "Couldn't send break message to %s!",
								1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
							goto DONE;
						} // end if (not connectiondropped)
						else
						{
							DPL(0, "WARNING: Couldn't send break message to %s because he's gone!",
								1, pTest->m_paTesterSlots[i].pSlave->m_szComputerName);
							hr = S_OK;
						} // end else (connection dropped)
					} // end if (failed to send message)
				} // end if (there's a tester here)
				else
				{
					DPL(0, "WARNING: Can't tell tester %i to DEBUGBREAK() because slot isn't filled yet!",
						1, i);
				} // end else (there's no tester assigned yet)

			} // end for (each possible tester slot)
		} // end if (this matches a break rule)
	} // end if (this tester is reporting a failure and there are break rules)

	if (pReportMsg->fTestComplete)
	{
		pTest->m_paTesterSlots[iTesterNum].hresult = pReportMsg->hresult;
		pTest->m_paTesterSlots[iTesterNum].fSuccess = pReportMsg->fSuccess;

		// If there's output data, we need to duplicate it over here.
		if (pReportMsg->dwOutputDataSize > 0)
		{
			pTest->m_paTesterSlots[iTesterNum].pvOutputData = LocalAlloc(LPTR, pReportMsg->dwOutputDataSize);
			if (pTest->m_paTesterSlots[iTesterNum].pvOutputData == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			pTest->m_paTesterSlots[iTesterNum].dwOutputDataSize = pReportMsg->dwOutputDataSize;

			CopyMemory(pTest->m_paTesterSlots[iTesterNum].pvOutputData, (pReportMsg + 1),
						pReportMsg->dwOutputDataSize);

			// If there are output variables, we need to duplicate them to.
			if (pReportMsg->dwVarsSize > 0)
			{
				pTest->m_paTesterSlots[iTesterNum].pVars = new (CTNOutputVarsList);
				if (pTest->m_paTesterSlots[iTesterNum].pVars == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate object)

				// Let the object know where the buffer is.
				pTest->m_paTesterSlots[iTesterNum].pVars->m_pvOutputDataStart = pTest->m_paTesterSlots[iTesterNum].pvOutputData;

				// Unpack the list.
				hr = pTest->m_paTesterSlots[iTesterNum].pVars->UnpackFromBuffer(((LPBYTE) (pReportMsg + 1)) + pReportMsg->dwOutputDataSize,
																					pReportMsg->dwVarsSize);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't unpack variables from buffer!", 0);
					goto DONE;
				} // end if (couldn't unpack variables from buffer)
			} // end if (there are variables)
		} // end if (there's output data)

		hr = this->CompleteSlavesTest(pSlave, pTest, pReportMsg->fSuccess,
									fSubTest, fOngoing);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't complete %s's test (ID %u)!",
				2, pSlave->m_szComputerName, pTest->m_dwUniqueID);
			goto DONE;
		} // end if (failed completing test)
	} // end if (test was completed)


DONE:

	if ((pTest != NULL) && (pTest->m_dwRefCount == 0xFFFFFFFF))
	{
		DPL(0, "Refcount for test %x is screwed!", 1, pTest);
		DEBUGBREAK();
	} // end if (refcount is screwed)

	if (fHaveTestRef)
	{
		pTest->m_dwRefCount--;
		if (pTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting test object %x.", 1, pTest);
			delete (pTest);
		}  // end if (that was the last reference)
		else
		{
			DPL(7, "Not deleting test object %x, its refcount is %u",
				2, pTest, pTest->m_dwRefCount);
		}  // end if (that was the last reference)
		pTest = NULL;
	} // end if (have test refcount)


	// We always want to free the report message passed in because it's a copy and
	// it was left to us to free it when we're done.
	LocalFree(pReportMsg);


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::DoHandleReport
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::ReadFromFile()"
//==================================================================================
// CTNMaster::ReadFromFile
//----------------------------------------------------------------------------------
//
// Description: Reads the passed in text file and loads any tests it contains.
//
// Arguments:
//	char* szFilepath	File to use.
//	DWORD dwNumReps		Number of repetitions to use for tests in the file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::ReadFromFile(char* szFilepath, DWORD dwNumReps)
{
	HRESULT					hr;
	READSCRIPTFILECONTEXT	rsfc;
	CLStringList			startupstrings;
	PTNTESTINSTANCEM		pTest = NULL;


	if ((szFilepath == NULL) || (strcmp(szFilepath, "") == 0))
	{
		DPL(0, "Got passed an invalid filepath!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed an invalid file)

	DPL(0, "Loading tests from file: %s", 1, szFilepath);


	ZeroMemory(&rsfc, sizeof (READSCRIPTFILECONTEXT));
	rsfc.pMaster = this;
	rsfc.dwNumReps = dwNumReps;
	rsfc.pStartupStrings = &startupstrings;
	//rsfc.dwUniqueID = 0;
	//rsfc.pCase = NULL;
	//rsfc.pszInstanceID = NULL;
	//rsfc.pszFriendlyName = NULL;
	//rsfc.fNumMachinesSpecified = FALSE;
	//rsfc.iNumMachines = 0;
	//rsfc.dwPermutationMode = 0;
	////rsfc.dwTestOptions = 0;
	//rsfc.pStoredData = NULL;
	//rsfc.pTesterAssignments = NULL;
	rsfc.iTesterNum = -1;


	hr = FileReadTextFile(szFilepath, FILERTFO_EXTENDEDINIOPTIONS, MasterReadFileCB, &rsfc);
	if (hr != S_OK)
	{
		if (hr == ERROR_FILE_NOT_FOUND)
		{
			DPL(0, "Text file \"%s\" doesn't exist!", 1, szFilepath);
			this->Log(TNLST_CRITICAL, "Text file \"%s\" doesn't exist!",
					1, szFilepath);
		} // end if (couldn't find file)
		else
		{
			DPL(0, "Couldn't read text file \"%s\"!", 1, szFilepath);
			this->Log(TNLST_CRITICAL, "Couldn't read text file \"%s\"!",
					1, szFilepath);
		} // end else (some other error)

		goto DONE;
	} // end if (couldn't read text file)


	// If we hit the end of the file before getting a new section (and thus didn't
	// get to dump the current gathered data into the queue), then we have to do
	// it here (assuming its a valid test).
	hr = this->StoreReadTest(&rsfc);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't store read test!", 0);
		goto DONE;
	} // end if (couldn't store the last test)

	/*
	if (this->m_pfnLoadStartupData != NULL)
	{
		hr = this->m_pfnLoadStartupData(&startupstrings);
		if (hr != S_OK)
		{
			DPL(0, "Module's LoadStartupData callback failed!", 0);
			goto EXIT_ERROR;
		} // end if (module's callback failed)
	} // end if (the module is able to load startup data)
	*/


DONE:
	
	if (rsfc.pszInstanceID != NULL)
	{
		LocalFree(rsfc.pszInstanceID);
		rsfc.pszInstanceID = NULL;
	} // end if (allocated an instance ID)

	if (rsfc.pszFriendlyName != NULL)
	{
		LocalFree(rsfc.pszFriendlyName);
		rsfc.pszFriendlyName = NULL;
	} // end if (allocated a friendly name)

	if (rsfc.pStoredData != NULL)
	{
		delete (rsfc.pStoredData);
		rsfc.pStoredData = NULL;
	} // end if (allocated stored data)

	if (rsfc.pTesterAssignments != NULL)
	{
		delete (rsfc.pTesterAssignments);
		rsfc.pTesterAssignments = NULL;
	} // end if (allocated tester assignments)

	return (hr);
} // CTNMaster::ReadFromFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::StoreReadTest()"
//==================================================================================
// CTNMaster::StoreReadTest
//----------------------------------------------------------------------------------
//
// Description: Compiles the current information and loads a test from it, if
//				possible.
//
// Arguments:
//	PREADSCRIPTFILECONTEXT pData	Pointer to info to use when working with the
//									test.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::StoreReadTest(PREADSCRIPTFILECONTEXT pData)
{
	HRESULT				hr = S_OK;
	PTNTESTFROMFILE		pTest = NULL;


	// If there was a valid item being worked on, commit it.
	if (pData->dwUniqueID != 0)
	{
		if ((pData->pCase != NULL) && (pData->pszInstanceID != NULL))
		{
			// Make sure the mode is appropriate.
			if ((this->m_dwMode == TNMODE_API) &&
				(! (pData->pCase->m_dwOptionFlags & TNTCO_API)))
			{
				DPL(0, "Can't add test \"%s-%s\" because the case did not specify it could be run in API mode!",
					2, pData->pCase->m_pszID, pData->pszInstanceID);
				hr = E_FAIL;
				goto DONE;
			} // end if (API mode not supported)
			else if ((this->m_dwMode == TNMODE_STRESS) &&
					(! (pData->pCase->m_dwOptionFlags & TNTCO_STRESS)))
			{
				DPL(0, "Can't add test \"%s-%s\" because the case did not specify it could be run in stress mode!",
					2, pData->pCase->m_pszID, pData->pszInstanceID);
				hr = E_FAIL;
				goto DONE;
			} // end else if (stress mode not supported)


			// If it's not a test loaded from a script section, search to
			// see if we loaded this test already.

			if (pData->dwUniqueID != 0xFFFFFFFF)
			{
				pTest = (PTNTESTFROMFILE) this->m_loadedtests.GetTest(pData->dwUniqueID);
			} // end if (test ID already committed)

			if (pTest == NULL)
			{
				// If the user specified the number of machines to use...
				if (pData->fNumMachinesSpecified)
				{
					// Make sure the number is valid.
					if (pData->pCase->m_iNumMachines > 0)
					{
						if (pData->iNumMachines != pData->pCase->m_iNumMachines)
						{
							DPL(0, "Can't add test because the case \"%s\" (instance ID \"%s\") requires a different number of testers (%i) from what the user specified (%i)!",
								4, pData->pCase->m_pszID, pData->pszInstanceID,
								pData->pCase->m_iNumMachines, pData->iNumMachines);
							hr = E_FAIL;
							goto DONE;
						} // end if (requestor is working with the wrong number of machines)
					} // end if (exact number of machines required)
					else
					{
						if (pData->iNumMachines <= 0)
						{
							DPL(2, "Using all machines in session to run test case \"%s\" (instance ID \"%s\").",
								2, pData->pCase->m_pszID, pData->pszInstanceID);
						} // end if (all machines indicated)
						else
						{
#pragma BUGBUG(vanceo, "Remove if clause when all tests switch to ADDTESTDATA")
							if (pData->pCase->m_iNumMachines < 0)
							{
								if (pData->iNumMachines < (-1 * pData->pCase->m_iNumMachines))
								{
									DPL(0, "Can't add test because the case \"%s\" (instance ID \"%s\") requires a larger number of testers (%i) from what the user specified (%i)!",
										4, pData->pCase->m_pszID, pData->pszInstanceID,
										(-1 * pData->pCase->m_iNumMachines), pData->iNumMachines);
									hr = E_FAIL;
									goto DONE;
								} // end if (requestor is working with the wrong number of machines)

								pData->pCase->m_iNumMachines = pData->iNumMachines;
							} // end if (minimum number of machines required)

							DPL(2, "Using %i machines to run test case \"%s\" (instance ID \"%s\").",
								3, pData->iNumMachines, pData->pCase->m_pszID,
								pData->pszInstanceID);
						} // end else (specific number of machines indicated)
					} // end else (number of machines can vary)
				} // end if (number of machines wasn't specified)
				else
				{
					// Just use the value the case specifies.  Note that this may
					// still be a special negative token, in which case all 
					// machines will be used.
					pData->iNumMachines = pData->pCase->m_iNumMachines;
				} // end else (the number of machines wasn't specified)

				if (pData->dwUniqueID != 0xFFFFFFFF)
				{
					if (pData->dwPermutationMode != 0)
					{
						DPL(0, "WARNING: Permutation mode set to %u for test %u?  Ignoring and using ONCE.",
							2, pData->dwPermutationMode, pData->dwUniqueID);
						pData->dwPermutationMode = TNTPM_ONCE;
					} // end if (no permutation mode set)
				} // end if (not a script test)
				else
				{
					// If the permutation mode isn't set, assume ONCE.
					if (pData->dwPermutationMode == 0)
					{
						pData->dwPermutationMode = TNTPM_ONCE;
					} // end if (no permutation mode set)
					else
					{
						if (this->m_dwMode == TNMODE_STRESS)
						{
							DPL(0, "WARNING: Permutation mode set to %u for test \"%s-%s\".  Ignoring and using ONCE.",
								3, pData->dwPermutationMode,
								pData->pCase->m_pszID,
								pData->pszInstanceID);

							pData->dwPermutationMode = TNTPM_ONCE;
						} // end if (in stress mode)
					} // end else (permutation mode set)
				} // end else (a script test)

				pTest = new (CTNTestFromFile)(pData->dwUniqueID,
											pData->pCase,
											pData->pszInstanceID,
											pData->pszFriendlyName,
											pData->iNumMachines,
											pData->dwPermutationMode,
											//pData->dwOptions,
											pData->pStoredData,
											pData->pTesterAssignments,
											pData->dwNumReps);
				if (pTest == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate object)

				// The test object owns these now
				pData->pszInstanceID = NULL;
				pData->pszFriendlyName = NULL;
				pData->pStoredData = NULL;
				pData->pTesterAssignments = NULL;


				hr = this->m_loadedtests.Add(pTest);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add test to list!", 0);
					delete (pTest);
					pTest = NULL;
					goto DONE;
				} // end if (couldn't add test)
			} // end if (the test hasn't already been added to the list)
			else
			{
#pragma BUGBUG(vanceo, "Verify that the items (pCase) still match")

				if (pData->pStoredData != NULL)
				{
					if (pTest->m_pStoredDataList == NULL)
					{
						pTest->m_pStoredDataList = new (CTNStoredDataList);
						if (pTest->m_pStoredDataList == NULL)
						{
							hr = E_OUTOFMEMORY;
							goto DONE;
						} // end if (couldn't allocate object)
					} // end if (there wasn't a stored data list before)

					hr = pTest->m_pStoredDataList->Add(pData->pStoredData);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't load add stored data to test (unique ID = %i)!",
							1, pData->dwUniqueID);
						goto DONE;
					} // end if (couldn't add test to list)
				} // end if (there's an item)
				pData->pStoredData = NULL;

#pragma BUGBUG(vanceo, "Have additive tester assignments?")
			} // end else (it has been added to the list)
		} // end if (we got valid data on a test)
		else
		{
			DPL(0, "WARNING: Not loading test because it is partial or invalid (make sure they have case and sub-case IDs)!", 0);

			this->Log(TNLST_CONTROLLAYER_INFO,
					"WARNING: Not loading test because it is partial or invalid (make sure they have case and sub-case IDs)!", 0);
		} // end else (test data was partial or invalid)
	} // end if (we started a new test)

	
	// Reset these
	pData->dwUniqueID = 0;
	pData->pCase = NULL;

	if (pData->pszInstanceID != NULL)
	{
		LocalFree(pData->pszInstanceID);
		pData->pszInstanceID = NULL;
	} // end if (allocated an instance ID)

	if (pData->pszFriendlyName != NULL)
	{
		LocalFree(pData->pszFriendlyName);
		pData->pszFriendlyName = NULL;
	} // end if (allocated a friendly test name)

	pData->fNumMachinesSpecified = FALSE;
	pData->iNumMachines = -1;
	pData->dwPermutationMode = 0;
	//pData->dwTestOptions = 0;

	if (pData->pStoredData != NULL)
	{
		delete (pData->pStoredData);
		pData->pStoredData = NULL;
	} // end if (allocated stored data)

	if (pData->pTesterAssignments != NULL)
	{
		delete (pData->pTesterAssignments);
		pData->pTesterAssignments = NULL;
	} // end if (have tester assignments)

	pData->iTesterNum = -1; 


DONE:

	return (hr);
} // CTNMaster::StoreReadTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::BuildAPITestList()"
//==================================================================================
// CTNMaster::BuildAPITestList
//----------------------------------------------------------------------------------
//
// Description: Starts up the control session and begins hosting.
//				The slave list lock is assumed to be held with only one level of
//				recursion.
//
// Arguments: None.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::BuildAPITestList(void)
{
#define MATRIX2D_ELEMENT(pMatrix, iSize1, iSize2, iEntry1, iEntry2)\
						(*(pMatrix + ((iEntry1 * iSize2) + iEntry2)))

#define SLAVETESTERSLOT(iSlave, iTesterNum)\
						MATRIX2D_ELEMENT(paiSlaveTesterSlots, this->m_slaves.Count(),\
										pLoadedTest->m_iNumMachines, iSlave, iTesterNum)			
	
	HRESULT				hr;
	DWORD				dwTime;
	CTNTestMsList		permutations;
	int					iNumLoadedTests;
	int					i;
	int					j;
	int					k;
	int					m;
	int					n;
	int					iBestRanking;
	int					iCurrentRanking;
	PTNTESTFROMFILE		pLoadedTest = NULL;
	PTNTESTINSTANCEM	pTest = NULL;
	PTNTESTINSTANCEM	pCompareTest = NULL;
	int*				paiSlaveTesterSlots = NULL;
	PTNTESTMSLIST*		papSlotRankings = NULL;
	int					iSlaveIndex;
	int					iCompareSlaveIndex;
	BOOL				fFoundSlaveWhoHasRun;
	BOOL				fFoundSlaveWhoHasntRun;
	int					iTimesSlaveHasRunTest;
	int					iTimesCompareSlaveHasRunTest;
	BOOL				fHandled;
	PTNSLAVEINFO		pSlave;
	int					iMaxNumPermutations;
	BOOL				fSkippedSome;


	// Double check to make sure we're not being called at an inappropriate time.
	if (this->m_remainingtests.Count() > 0)
	{
		DPL(0, "Building API test list when %i tests remain in the list!?",
			1, this->m_remainingtests.Count());
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (anything in list)


	iNumLoadedTests = this->m_loadedtests.Count();


	DPL(0, "Building API test list from %i loaded tests.", 1, iNumLoadedTests);
	//Ignore error
	this->Log(TNLST_CONTROLLAYER_INFO, "Building API test list...", 0);

#ifndef _XBOX // timeGetTime not supported
	dwTime = timeGetTime();
#else // XBOX
	dwTime = GetTickCount();
#endif // ! XBOX

	// Loop through all of the currently loaded items once.
	for(i = 0; i < iNumLoadedTests; i++)
	{
		// Pop the first item from the list.
		pLoadedTest = (PTNTESTFROMFILE) this->m_loadedtests.PopFirstItem();
		if (pLoadedTest == NULL)
		{
			DPL(0, "Couldn't pop first loaded test from list!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't pop first item)

		// If there are more repetitions of this item to run, then we need to put it
		// back on the list.  That won't mess up our current for loop because we set
		// the number of iterations in stone.
		if (pLoadedTest->m_dwRepsRemaining != 1)
		{
			// If it's not infinite repetitions, decrement it
			if (pLoadedTest->m_dwRepsRemaining != 0)
				pLoadedTest->m_dwRepsRemaining--;

			// Add it back to the end of the list.
			hr = this->m_loadedtests.Add(pLoadedTest);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't re-add loaded test %x to end of list!",
					1, pLoadedTest);
				goto DONE;
			} // end if (couldn't readd item)
		} // end if (there are repetitions remaining)


		// Build the list of valid permutations for the object
		hr = this->BuildTestPermutationsList(pLoadedTest, &permutations,
											&iMaxNumPermutations);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't build test permutations list!", 0);
			goto DONE;
		} // end if (couldn't build test permutations)


		// If there aren't any valid permutations, skip the test, since the session
		// is closed and there won't ever be any valid permutations.
		if (permutations.Count() <= 0)
		{
			DPL(0, "WARNING: Test \"%s-%s\" was skipped because there aren't any valid permutations!",
				2, pLoadedTest->m_pCase->m_pszID,
				pLoadedTest->m_pszInstanceID);

			this->Log(TNLST_CRITICAL,
					"WARNING: Test \"%s-%s\" was skipped because there aren't any valid permutations!",
					2, pLoadedTest->m_pCase->m_pszID,
					pLoadedTest->m_pszInstanceID);


			// We're going to build a fake test so we can add a report without
			// adding a whole bunch of special case code.
			// Hee hee, isn't this special case code?

			pTest = new (CTNTestInstanceM)(this->m_dwCurrentUniqueID++,
											pLoadedTest,
											pLoadedTest->m_pCase,
											1,
											//0,
											NULL);
			if (pTest == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			hr = this->m_skippedtests.Add(pTest);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
				goto DONE;
			} // end if (couldn't add item)


			// Add the skipped test report.
			hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
											pTest, 0, NULL, 0);
			if (hr != S_OK)
			{
				DPL(0, "Failed to add skip report to list!", 0);
				goto DONE;
			} // end if (failed add report)


			pTest = NULL;


			// If we are doing the auto-report thing, and the report we just added
			// is one of the ones we are counting, check to see if that pushed us
			// over the limit.
			if ((this->m_iAutoReportNumReports > 0) &&
				(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
			{
				hr = this->DoAutoReport();
				if (hr != S_OK)
				{
					DPL(0, "Couldn't auto-report!", 0);
					goto DONE;
				} // end if (couldn't print reports)
			} // end if (auto reporting is on and this was a matching report type)
		} // end if (there aren't any permutations)
		else
		{
			// If we're not only using one permutation, we need to check to make
			// sure all the permutations necessary were used.  Otherwise, we need
			// to warn about skipping.
			if (pLoadedTest->m_dwPermutationMode != TNTPM_ONCE)
			{
				fSkippedSome = FALSE;

				if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLPOSSIBLE)
				{
					if (permutations.Count() != iMaxNumPermutations)
					{
						DPL(1, "Loaded test \"%s-%s\" didn't use all permutations (can only run %i of %i) as requested by AllPossible mode.",
							4, pLoadedTest->m_pCase->m_pszID,
							pLoadedTest->m_pszInstanceID,
							permutations.Count(), iMaxNumPermutations);

						fSkippedSome = TRUE;
					} // end if (didn't retrieve all permutations)
				} // end if (all possible)
				else
				{
					// Loop through all the slaves.
					for(j = 0; j < this->m_slaves.Count(); j++)
					{
						pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(j);
						if (pSlave == NULL)
						{
							DPL(0, "Couldn't get slave %i!", 1, j);
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't get slave)


						if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
						{
							// Loop through all the permutations.
							for(m = 0; m < permutations.Count(); m++)
							{
								pTest = (PTNTESTINSTANCEM) permutations.GetItem(m);
								if (pTest == NULL)
								{
									DPL(0, "Couldn't get permutation %i from list!", 1, m);
									hr = E_FAIL;
									goto DONE;
								} // end if (couldn't get item)


								iSlaveIndex = pTest->GetSlavesTesterNum(pSlave);

								// We only need to make sure the slave ran this
								// test once.  If so, we're done here.
								if (iSlaveIndex >= 0)
									break;
							} // end for (each permutation)
						} // end if (all slaves)
						else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
						{
							// Loop through all the permutations.
							for(m = 0; m < permutations.Count(); m++)
							{
								pTest = (PTNTESTINSTANCEM) permutations.GetItem(m);
								if (pTest == NULL)
								{
									DPL(0, "Couldn't get permutation %i from list!", 1, m);
									hr = E_FAIL;
									goto DONE;
								} // end if (couldn't get item)


								iSlaveIndex = pTest->GetSlavesTesterNum(pSlave);

								// We need to make sure the slave ran this test
								// as tester number 0 at least once.  If so, we're
								// done here.
								if (iSlaveIndex >= 0)
									break;
							} // end for (each permutation)


							// If we didn't find a permutation to our liking, we can
							// stop searching.
							if (iSlaveIndex < 0)
								break;


							// Loop through all the permutations again.
							for(m = 0; m < permutations.Count(); m++)
							{
								pTest = (PTNTESTINSTANCEM) permutations.GetItem(m);
								if (pTest == NULL)
								{
									DPL(0, "Couldn't get permutation %i from list!", 1, m);
									hr = E_FAIL;
									goto DONE;
								} // end if (couldn't get item)


								iSlaveIndex = pTest->GetSlavesTesterNum(pSlave);

								// We need to make sure the slave ran this test
								// as a tester number other than 0 at least once.
								// If so, we're done here.
								if (iSlaveIndex >= 0)
									break;
							} // end for (each permutation)
						} // end else if (all slaves as testers 0 and non 0)
						else
						{
							// Loop through all the testing positions.
							for(k = 0; k < pLoadedTest->m_iNumMachines; k++)
							{
								// Loop through all the permutations.
								for(m = 0; m < permutations.Count(); m++)
								{
									pTest = (PTNTESTINSTANCEM) permutations.GetItem(m);
									if (pTest == NULL)
									{
										DPL(0, "Couldn't get permutation %i from list!", 1, m);
										hr = E_FAIL;
										goto DONE;
									} // end if (couldn't get item)

									iSlaveIndex = pTest->GetSlavesTesterNum(pSlave);

									// We need to make sure the slave ran this test
									// once as this tester number.  If so, we're
									// done here.
									if (iSlaveIndex == k)
										break;

									// Otherwise reset this so the check below
									// works.
									iSlaveIndex = -1;
								} // end for (each permutation)

								// If we found a permutation to our liking, we can stop
								// searching.
								if (iSlaveIndex >= 0)
									break;
							} // end for (each testing position)
						} // end else (all slave as all testers)


						// If we didn't find a permutation to our liking, we can
						// stop searching.
						if (iSlaveIndex < 0)
							break;
					} // end for (each tester)


					// If we didn't find a permutation to our liking on the last
					// slave we checked, note the fact that permutations were
					// skipped.
					if (iSlaveIndex < 0)
					{
						// pSlave should still hold the slave which didn't have
						// the permutations.
						DPL(1, "Slave %s didn't have all the permutations for \"%s-%s\" requested for mode %u.",
							4, pSlave->m_szComputerName,
							pLoadedTest->m_pCase->m_pszID,
							pLoadedTest->m_pszInstanceID,
							pLoadedTest->m_dwPermutationMode);

						fSkippedSome = TRUE;
					} // end if (no permutation to our liking)
				} // end else (not all possible)

				if (fSkippedSome)
				{
					DPL(0, "WARNING: Some requested permutations of test \"%s-%s\" were skipped!",
						2, pLoadedTest->m_pCase->m_pszID,
						pLoadedTest->m_pszInstanceID);

					this->Log(TNLST_CRITICAL,
							"WARNING: Some requested permutations of test \"%s-%s\" were skipped!",
							2, pLoadedTest->m_pCase->m_pszID,
							pLoadedTest->m_pszInstanceID);


					// We're going to build a fake test so we can add a report without
					// adding a whole bunch of special case code.
					// Hee hee, isn't this special case code?

					pTest = new (CTNTestInstanceM)(this->m_dwCurrentUniqueID++,
													pLoadedTest,
													pLoadedTest->m_pCase,
													1,
													//0,
													NULL);
					if (pTest == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate object)

					hr = this->m_skippedtests.Add(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add test %x to skipped list!", 1, pTest);
						goto DONE;
					} // end if (couldn't add item)


					// Add the skipped test report.
					hr = this->m_reports.AddReport(RT_TESTSKIPPED, 0, NULL,
													pTest, 0, NULL, 0);
					if (hr != S_OK)
					{
						DPL(0, "Failed to add skip report to list!", 0);
						goto DONE;
					} // end if (failed add report)


					pTest = NULL;


					// If we are doing the auto-report thing, and the report we just added
					// is one of the ones we are counting, check to see if that pushed us
					// over the limit.
					if ((this->m_iAutoReportNumReports > 0) &&
						(this->m_dwAutoReportFlags & TNREPORT_NONTESTREPORTS))
					{
						hr = this->DoAutoReport();
						if (hr != S_OK)
						{
							DPL(0, "Couldn't auto-report!", 0);
							goto DONE;
						} // end if (couldn't print reports)
					} // end if (auto reporting is on and this was a matching report type)
				} // end if (there aren't any permutations)
			} // end if (not once permutation mode)

			// If this is a run once test, then we only need to run the first
			// permutation.  If we want to run all possible, then just copy all of
			// the permutations.
			if ((pLoadedTest->m_dwPermutationMode == TNTPM_ONCE) ||
				(pLoadedTest->m_dwPermutationMode == TNTPM_ALLPOSSIBLE))
			{
				// Loop until we've handled each permutation generated.
				do
				{
					// Pop the first item from the list.
					pTest = (PTNTESTINSTANCEM) permutations.PopFirstItem();
					if (pTest == NULL)
					{
						DPL(0, "Couldn't pop first remaining permutation from list!", 0);
						hr = E_FAIL;
						goto DONE;
					} // end if (couldn't pop first item)

					// Assign it a valid ID.
					pTest->m_dwUniqueID = this->m_dwCurrentUniqueID++;

					// Add it to the list of tests to run.
					hr = this->m_remainingtests.Add(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add first permutation to list!", 0);
						goto DONE;
					} // end if (couldn't add remaining tests)

					pTest->m_dwRefCount--;
					if (pTest->m_dwRefCount == 0)
					{
						DPL(0, "Deleting first permutation object %x!?",
							1, pTest);
						delete (pTest);
					} // end if (should delete object)
					pTest = NULL;

					// If we're only running one permutation, dump the rest.
					if (pLoadedTest->m_dwPermutationMode == TNTPM_ONCE)
					{
						DPL(1, "Removing %i extra permutations of test \"%s-%s\" because the permutation mode is TNTPM_ONCE.",
							3, permutations.Count(),
							pLoadedTest->m_pCase->m_pszID,
							pLoadedTest->m_pszInstanceID);

						// Remove all the other permutations.
						hr = permutations.RemoveAll();
						if (hr != S_OK)
						{
							DPL(0, "Couldn't remove all remaining permutations!", 0);
							goto DONE;
						} // end if (couldn't remove all)
					} // end if (only running once)
				} // end do (while there are more permutations)
				while (permutations.Count() > 0);
			} // end if (only need one permutation)
			else
			{
				// Build a two-dimensional matrix of slaves and tester slots.
				paiSlaveTesterSlots = (int*) LocalAlloc(LPTR, (this->m_slaves.Count() * pLoadedTest->m_iNumMachines * sizeof (int)));
				if (paiSlaveTesterSlots == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				// Build an array of list pointers that we'll use to rank the
				// remaining permutations by the values in the previous array.
				papSlotRankings = (PTNTESTMSLIST*) LocalAlloc(LPTR, (pLoadedTest->m_iNumMachines * sizeof (PTNTESTMSLIST)));
				if (papSlotRankings == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)


				// Loop until we've handled each permutation generated.
				do
				{
					// Reset the ranking array.
					for(j = 0; j < pLoadedTest->m_iNumMachines; j++)
					{
						// If the slot doesn't have a list, create one,
						// otherwise reset it.
						if (papSlotRankings[j] == NULL)
						{
							papSlotRankings[j] = new (CTNTestMsList);
							if (papSlotRankings[j] == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't allocate object)
						} // end if (need new object)
						else
						{
							hr = papSlotRankings[j]->RemoveAll();
							if (hr != S_OK)
							{
								DPL(0, "Couldn't remove all items in the list!", 0);
								goto DONE;
							} // end if (couldn't remove all items)
						} // end else (already have object)
					} // end for (each slot)

					fHandled = FALSE;


					// Loop through each permutation left.
					for(j = 0; j < permutations.Count(); j++)
					{
						pTest = (PTNTESTINSTANCEM) permutations.GetItem(j);
						if (pTest == NULL)
						{
							DPL(0, "Couldn't get permutation %i from list!", 1, j);
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't get item)

						fFoundSlaveWhoHasRun = FALSE;
						fFoundSlaveWhoHasntRun = FALSE;

						for(k = 0; k < pLoadedTest->m_iNumMachines; k++)
						{
							iSlaveIndex = this->m_slaves.GetFirstIndex(pTest->m_paTesterSlots[k].pSlave);
							if (iSlaveIndex < 0)
							{
								DPL(0, "Couldn't get slave index of %s!",
									1, pTest->m_paTesterSlots[k].pSlave->m_szComputerName);
								hr = E_FAIL;
								goto DONE;
							} // end if (couldn't get index)

							if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
							{
								iTimesSlaveHasRunTest = 0;

								// Calculate how many times this slave has run this
								// test in any position.
								for(m = 0; m < pLoadedTest->m_iNumMachines; m++)
								{
									iTimesSlaveHasRunTest += SLAVETESTERSLOT(iSlaveIndex, m);
								} // end for (each testing slot)

								if (iTimesSlaveHasRunTest > 0)
									fFoundSlaveWhoHasRun = TRUE;
								else
									fFoundSlaveWhoHasntRun = TRUE;
							} // end if (in ALLSLAVES mode)
							else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
							{
								// Note whether this slave has run the test as
								// tester 0 or tester number other than 0 yet.
								if (k == 0)
								{
									// Tester 0, so look for other tester 0 runs.

									if (SLAVETESTERSLOT(iSlaveIndex, 0) != 0)
										fFoundSlaveWhoHasRun = TRUE;
									else
										fFoundSlaveWhoHasntRun = TRUE;
								} // end if (tester 0)
								else
								{
									// Not tester 0, so look for other non-tester 0
									// runs.

									iTimesSlaveHasRunTest = 0;

									// Calculate how many times this slave has run
									// this test in a non-0 position.
									for(m = 1; m < pLoadedTest->m_iNumMachines; m++)
									{
										iTimesSlaveHasRunTest += SLAVETESTERSLOT(iSlaveIndex, m);
									} // end for (each testing slot)

									if (iTimesSlaveHasRunTest > 0)
										fFoundSlaveWhoHasRun = TRUE;
									else
										fFoundSlaveWhoHasntRun = TRUE;
								} // end else (not tester 0)
							} // end else if (in ALLSLAVESASTESTERS0ANDNON0 mode)
							else
							{
								// Note whether this slave has run the test in this
								// position yet.
								if (SLAVETESTERSLOT(iSlaveIndex, k) != 0)
									fFoundSlaveWhoHasRun = TRUE;
								else
									fFoundSlaveWhoHasntRun = TRUE;
							} // end else (in ALLSLAVESASALLTESTERS mode)
						} // end for (each tester slot)


						// If every slave in this permutation has never run this
						// test (specifically as tester 0 & non-tester 0, if in
						// ALLSLAVESASTESTERS0ANDNO0 permutation mode, or as that
						// tester number, if in ALLSLAVESASALLTESTERS permutation
						// mode), we found one we want to use.
						if (! fFoundSlaveWhoHasRun)
						{
							if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
							{
								DPL(1, "Using permutation %x of test \"%s-%s\" because it has not been run by any of those slaves yet.",
									3, pTest,
									pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
							} // end if (all slaves)
							else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
							{
								DPL(1, "Using permutation %x of test \"%s-%s\" because it has not been run by %s as tester 0 or by one of the other slaves in a non-tester-0 position yet.",
									4, pTest, pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID,
									pTest->m_paTesterSlots[0].pSlave->m_szComputerName);
							} // end else if (all slaves as tester 0 and non 0)
							else
							{
								DPL(1, "Using permutation %x of test \"%s-%s\" because it has not been run by any of those slaves in those tester positions yet.",
									3, pTest, pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
							} // end else (all slaves as all testers)


							fHandled = TRUE;

							// Stop looking at each permutation.
							break;
						} // end if (everyone has not run as tester)
						else if (! fFoundSlaveWhoHasntRun)
						{
							// If every slave in this permutation has already run
							// this test (as that tester number, if in
							// ALLSLAVESASALLTESTERS permutation mode) at least
							// once, we want to pull this permutation from the
							// list.  We don't want to bother running duplicate
							// permutations.

							if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
							{
								DPL(1, "Removing extra permutation %x of test \"%s-%s\" because it has already been run by all of those slaves at least once.",
									3, pTest, pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
							} // end if (all slaves)
							else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
							{
								DPL(1, "Removing extra permutation %x of test \"%s-%s\" because it has already been run as tester number 0 and non-0 by all of those slaves at least once.",
									3, pTest, pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
							} // end else if (all slaves as testers 0 and non 0)
							else
							{
								DPL(1, "Removing extra permutation %x of test \"%s-%s\" because it has already been run by all of those slaves in those tester positions at least once.",
									3, pTest, pLoadedTest->m_pCase->m_pszID,
									pLoadedTest->m_pszInstanceID);
							} // end else (all slaves as all testers)


							// Remove the permutation we're looking at from the
							// list of remaining ones.
							hr = permutations.Remove(j);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't remove permutation %i!", 1, j);
								goto DONE;
							} // end if (couldn't remove item)

							// Have to decrement so we don't screw up our counter.
							// A little ugly, but oh well.
							j--;

							pTest = NULL; // for safety's sake
						} // end else if (not everyone has run as tester)
						else
						{
							// If we are here, some people have run the permutation
							// (as that tester number, if in ALLSLAVESASALLTESTERS
							// mode), and some haven't.  Loop back through the tester
							// slots and rank this test for each, with people running
							// the fewest times first (so we get as even a
							// distribution as possible).
							for(k = 0; k < pLoadedTest->m_iNumMachines; k++)
							{
								iSlaveIndex = this->m_slaves.GetFirstIndex(pTest->m_paTesterSlots[k].pSlave);
								if (iSlaveIndex < 0)
								{
									DPL(0, "Couldn't get slave index of %s!",
										1, pTest->m_paTesterSlots[k].pSlave->m_szComputerName);
									hr = E_FAIL;
									goto DONE;
								} // end if (couldn't get index)

								if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
								{
									iTimesSlaveHasRunTest = 0;

									// Calculate how many times that slave has run the
									// test in any position.
									for(n = 0; n < pLoadedTest->m_iNumMachines; n++)
									{
										iTimesSlaveHasRunTest += SLAVETESTERSLOT(iSlaveIndex, n);
									} // end for (each testing slot)
								} // end if (in ALLSLAVES permutation mode)
								else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
								{
									iTimesSlaveHasRunTest = 0;

									// Calculate how many times that slave has run the
									// test in a non-tester 0 position.
									for(n = 1; n < pLoadedTest->m_iNumMachines; n++)
									{
										iTimesSlaveHasRunTest += SLAVETESTERSLOT(iSlaveIndex, n);
									} // end for (each testing slot)
								} // end if (in TNTPM_ALLSLAVESASTESTERS0ANDNON0 permutation mode)


								// Loop through all the previous permutations we've
								// looked at and insert this current one in order
								// into the ranking list.
								for(m = 0; m < j; m++)
								{
									pCompareTest = (PTNTESTINSTANCEM) papSlotRankings[k]->GetItem(m);
									if (pCompareTest == NULL)
									{
										DPL(0, "Couldn't get test to compare %i from ranking list!",
											1, m);
										hr = E_FAIL;
										goto DONE;
									} // end if (couldn't get item)

									iCompareSlaveIndex = this->m_slaves.GetFirstIndex(pCompareTest->m_paTesterSlots[k].pSlave);
									if (iCompareSlaveIndex < 0)
									{
										DPL(0, "Couldn't get compare slave index of %s!",
											1, pCompareTest->m_paTesterSlots[k].pSlave->m_szComputerName);
										hr = E_FAIL;
										goto DONE;
									} // end if (couldn't get index)

									if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES)
									{
										iTimesCompareSlaveHasRunTest = 0;

										// Calculate how many times that slave has run the
										// test in any position.
										for(n = 0; n < pLoadedTest->m_iNumMachines; n++)
										{
											iTimesCompareSlaveHasRunTest += SLAVETESTERSLOT(iCompareSlaveIndex, n);
										} // end for (each testing slot)
									} // end if (in ALLSLAVES permutation mode)
									else if (pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0)
									{
										iTimesCompareSlaveHasRunTest = 0;

										// Calculate how many times that slave has run the
										// test in a non-tester 0 position.
										for(n = 1; n < pLoadedTest->m_iNumMachines; n++)
										{
											iTimesCompareSlaveHasRunTest += SLAVETESTERSLOT(iCompareSlaveIndex, n);
										} // end for (each testing slot)
									} // end if (in TNTPM_ALLSLAVESASTESTERS0ANDNON0 permutation mode)


#pragma TODO(vanceo, "Double check testers 0 and non 0 logic (is it really ranking correctly?)")

									// If this previous test was run more times than our
									// current test, insert the current test before it.
									if (((pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVES) &&
											(iTimesCompareSlaveHasRunTest > iTimesSlaveHasRunTest)) ||

										((pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASTESTERS0ANDNON0) &&
											(SLAVETESTERSLOT(iCompareSlaveIndex, 0) > SLAVETESTERSLOT(iSlaveIndex, 0)) &&
											(iTimesCompareSlaveHasRunTest > iTimesSlaveHasRunTest)) ||

										((pLoadedTest->m_dwPermutationMode == TNTPM_ALLSLAVESASALLTESTERS) &&
											(SLAVETESTERSLOT(iCompareSlaveIndex, k) > SLAVETESTERSLOT(iSlaveIndex, k))))
									{
										hr = papSlotRankings[k]->InsertBeforeIndex(pTest, m);
										if (hr != S_OK)
										{
											DPL(0, "Couldn't insert permutation %i into ranking list before %i!",
												2, j, m);
											goto DONE;
										} // end if (couldn't add item)

										fHandled = TRUE;
										break;
									} // end if (previous test run more times than current)
								} // end for (each previous test)

								// If we found a place to insert into the ranking, just
								// reset the boolean so we don't screw up our outer
								// loop below.
								// If we didn't find a place, just add it at the end.
								if (fHandled)
								{
									fHandled = FALSE;
								} // end if (inserted test)
								else
								{
									hr = papSlotRankings[k]->Add(pTest);
									if (hr != S_OK)
									{
										DPL(0, "Couldn't add permutation %i (%x) to end of ranking list!",
											2, j, pTest);
										goto DONE;
									} // end if (couldn't add item)
								} // end else (didn't insert test)
							} // end for (each tester slot)

							pTest = NULL; // for safety's sake
						} // end else (it's a mixed bag)
					} // end for (each permutation)


					// If all of the permutations left were removed, then it
					// looks like we're done.  Get out of the do-while loop.
					if (permutations.Count() <= 0)
						break;


					// If we looped through all the permutations but didn't find
					// an easy one with all 0 usages, we need to use the best
					// possible permutation (most slaves with the fewest times in
					// each position).  We generated the rankings above as we went
					// along.
					if (! fHandled)
					{
						// Reset our current champion and his rating.
						pCompareTest = NULL;
						iBestRanking = 0;

						// Loop through each remaining permutation.
						for(j = 0; j < permutations.Count(); j++)
						{
							pTest = (PTNTESTINSTANCEM) permutations.GetItem(j);
							if (pTest == NULL)
							{
								DPL(0, "Couldn't get permutation %i from list!", 1, j);
								hr = E_FAIL;
								goto DONE;
							} // end if (couldn't get item)

							iCurrentRanking = 0;

							// Loop through each tester slot.
							for(k = 0; k < pLoadedTest->m_iNumMachines; k++)
							{
								m = papSlotRankings[k]->GetFirstIndex(pTest);
								if (m < 0)
								{
									DPL(0, "Couldn't find test %x (%u) in slot %i rankings!?",
										3, pTest, pTest->m_dwUniqueID, k);

									DEBUGBREAK();

									hr = E_FAIL;
									goto DONE;
								} // end if (couldn't find test)

								// Increase the ranking based on its order for
								// this slot.
								iCurrentRanking += m;

								// If the ranking is now over the best so far,
								// there's no point in continuing, it already
								// lost.  If this is the first item, then skip
								// this check.
								if ((pCompareTest != NULL) &&
									(iCurrentRanking > iBestRanking))
								{
									break;
								} // end if (started ranking and this is worse)
							} // end for (each tester slot)

							// If this is the first item, or the total ranking is
							// the best so far, save it.
							if ((pCompareTest == NULL) ||
								(iCurrentRanking < iBestRanking))
							{
								pCompareTest = pTest;
								iBestRanking = iCurrentRanking;
							} // end if (started ranking and this is worse)
						} // end for (each permutation)


						pTest = pCompareTest;

						DPL(7, "Permutation %x has the best ranking (%i).",
							2, pTest, iBestRanking);
					} // end if (didn't find an easy one)


					// pTest should now hold the one we want to use


					// Assign it a valid ID.
					pTest->m_dwUniqueID = this->m_dwCurrentUniqueID++;

					// Add it to the list of tests to run.
					hr = this->m_remainingtests.Add(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add permutation %x to list!",
							1, pTest);
						goto DONE;
					} // end if (couldn't add remaining tests)

					// Update the array of slave & slot usage.
					for(j = 0; j < pLoadedTest->m_iNumMachines; j++)
					{
						iSlaveIndex = this->m_slaves.GetFirstIndex(pTest->m_paTesterSlots[j].pSlave);
						if (iSlaveIndex < 0)
						{
							DPL(0, "Couldn't get slave index of %s!",
								1, pTest->m_paTesterSlots[j].pSlave->m_szComputerName);
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't get index)

						SLAVETESTERSLOT(iSlaveIndex, j)++;
					} // end for (each tester slot)

					// Remove it from the list of remaining ones.
					hr = permutations.RemoveFirstReference(pTest);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't remove permutation %x from remaining permutations list!",
							1, pTest);
						goto DONE;
					} // end if (couldn't remove item)

#ifdef DEBUG
					if (pTest->m_dwRefCount == 0)
					{
						DPL(0, "Deleting permutation %x because it has a refcount of 0!?",
							1, pTest);

						DEBUGBREAK();

						delete (pTest);
						pTest = NULL;
					} // end if (should delete item)
#endif // DEBUG
				} // end do (while there are more permutations)
				while (permutations.Count() > 0);


				// Cleanup

				LocalFree(paiSlaveTesterSlots);
				paiSlaveTesterSlots = NULL;

				// Delete the slot ranking lists.
				for(j = 0; j < pLoadedTest->m_iNumMachines; j++)
				{
					if (papSlotRankings[j] != NULL)
					{
						delete (papSlotRankings[j]);
						papSlotRankings[j] = NULL;
					} // end if (there's a list pointer there)
				} // end for (each tester slot)

				LocalFree(papSlotRankings);
				papSlotRankings = NULL;
			} // end else (other permutation mode)
		} // end else (there are permutations)


		// Take off our ref.
		pLoadedTest->m_dwRefCount--;
		if (pLoadedTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting loaded test %x.", 1, pLoadedTest);
			delete (pLoadedTest);
		} // end if (should delete object)
		else
		{
			DPL(7, "Not deleting loaded test %x, its refcount is %u.",
				2, pLoadedTest, pLoadedTest->m_dwRefCount);
		} // end else (shouldn't delete object)

		pLoadedTest = NULL;
	} // end for (each loaded test)


#ifndef _XBOX // timeGetTime not supported
	DPL(1, "Built API test list containing %i total test permutation(s) from %i loaded test(s), %u ms elapsed.",
		3, this->m_remainingtests.Count(), iNumLoadedTests,
		timeGetTime() - dwTime);
#else // ! XBOX
	DPL(1, "Built API test list containing %i total test permutation(s) from %i loaded test(s), %u ms elapsed.",
		3, this->m_remainingtests.Count(), iNumLoadedTests,
		GetTickCount() - dwTime);
#endif // XBOX

	// Ignore error
	this->Log(TNLST_CONTROLLAYER_INFO,
			"...Built API test list containing %i total test permutatio%s from %i loaded tes%s.",
			4, this->m_remainingtests.Count(),
			((this->m_remainingtests.Count() != 1) ? "ns" : "n"),
			iNumLoadedTests,
			((iNumLoadedTests != 1) ? "ts" : "t"));


	// If there aren't any valid permutations this time around, nobody will
	// be able to run any further repetitions either, so might as remove
	// all remaining repetitions (they would have already been marked as
	// skipped above).
	if (this->m_remainingtests.Count() == 0)
	{
		DPL(0, "WARNING: Removing all %i loaded tests from list.",
			1, this->m_loadedtests.Count());

		hr = this->m_loadedtests.RemoveAll();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove all remaining loaded tests from list!", 0);
			goto DONE;
		} // end if (couldn't get first item)
	} // end if (didn't get any permutations)
#if 0
#pragma BUGBUG(vanceo, "               DISABLE THIS WHEN DONE DEBUGGING")
	else
	{
		for(i = 0; i < this->m_remainingtests.Count(); i++)
		{
			pTest = (PTNTESTINSTANCEM) this->m_remainingtests.GetItem(i);

			DPL(0, "Test %u (%x, \"%s-%s\", %i machines):",
				5, pTest->m_dwUniqueID,
				pTest,
				pTest->m_pCase->m_pszID,
				pTest->m_pLoadedTest->m_pszInstanceID,
				pTest->m_iNumMachines);

			for(j = 0; j < pTest->m_iNumMachines; j++)
			{
				DPL(0, "     %i=%s (%x, ID %u, refcount %u)",
					5, j,
					pTest->m_paTesterSlots[j].pSlave->m_szComputerName,
					pTest->m_paTesterSlots[j].pSlave,
					pTest->m_paTesterSlots[j].pSlave->m_id.dwTime,
					pTest->m_paTesterSlots[j].pSlave->m_dwRefCount);
			} // end for (each tester slot)
		} // end for (each test)
	} // end else (did get permutations)
#endif



DONE:

	if (paiSlaveTesterSlots != NULL)
	{
		LocalFree(paiSlaveTesterSlots);
		paiSlaveTesterSlots = NULL;
	} // end if (allocated array)

	if (papSlotRankings != NULL)
	{
		if (pLoadedTest == NULL)
		{
			DPL(0, "WARNING: Loaded test pointer is NULL!  Assuming no slot ranking lists!", 0);
		} // end if (no loaded test pointer)
		else
		{
			// Delete the slot ranking lists.
			for(i = 0; i < pLoadedTest->m_iNumMachines; i++)
			{
				if (papSlotRankings[i] != NULL)
				{
					delete (papSlotRankings[i]);
					papSlotRankings[i] = NULL;
				} // end if (there's a list pointer there)
			} // end for (each tester slot)
		} // end else (loaded test pointer)

		LocalFree(papSlotRankings);
		papSlotRankings = NULL;
	} // end if (allocated array)


	// If we still have the popped test object, take off our ref.
	if (pLoadedTest != NULL)
	{
		pLoadedTest->m_dwRefCount--;
		if (pLoadedTest->m_dwRefCount == 0)
		{
			DPL(7, "Deleting loaded test %x.", 1, pLoadedTest);
			delete (pLoadedTest);
		} // end if (should delete object)
		else
		{
			DPL(7, "Not deleting loaded test %x, its refcount is %u.",
				2, pLoadedTest, pLoadedTest->m_dwRefCount);
		} // end else (shouldn't delete object)

		pLoadedTest = NULL;
	} // end if (still have loaded test object)

	return (hr);


#undef SLAVETESTERSLOT
#undef MATRIX2D_ELEMENT

} // CTNMaster::BuildAPITestList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::BuildTestPermutationsList()"
//==================================================================================
// CTNMaster::BuildTestPermutationsList
//----------------------------------------------------------------------------------
//
// Description: Fills the passed in list with all the valid permutations of the
//				given test.  All slaves currently in the session are used.  The
//				module's CanRun function is called, if it exists, to weed out any
//				possibilities that won't work.
//				It's important to note that the tests in the list will NOT have
//				valid test IDs, and that value stored in piMaxNumPermutations will
//				not be valid for TNTPM_ONCE tests.
//				The slave list lock is assumed to be held with only one level of
//				recursion.
//
// Arguments:
//	PTNTESTFROMFILE pBaseTest	Pointer to test to permute.
//	PTNTESTMSLIST pTestList		List to place results in.
//	int* piMaxNumPermutations	Place to store maximum number of permutations that
//								could be used (not necessarily number of items
//								actually in built test list).
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::BuildTestPermutationsList(PTNTESTFROMFILE pBaseTest,
											PTNTESTMSLIST pTestList,
											int* piMaxNumPermutations)
{
	HRESULT				hr = S_OK;
	BOOL				fUsingMaxNumSlaves = FALSE;
	PTNSLAVESLIST*		papPossibleSlaves = NULL;
	PTNSLAVEINFO*		papTesters = NULL;
	PTNSLAVEINFO		pSlave = NULL;
	int					i;
	int					j;
	int					k;
	int					iTesterNum;
	PLSTRING			pAssignString;
	char*				pszTemp = NULL;
	char*				pszValue;
	BOOL				fInvertAssign;
	DWORD				dwTempSize;
	DWORD				dwNumFields;


	// Start by resetting the count.
	(*piMaxNumPermutations) = 0;


	// If the loaded test wants to use all slaves in the session, we want to note
	// that.
	if (pBaseTest->m_iNumMachines <= 0)
	{
		fUsingMaxNumSlaves = TRUE;
		pBaseTest->m_iNumMachines = this->m_slaves.Count();

#ifdef DEBUG
		if (pBaseTest->m_pCase->m_iNumMachines > 0)
		{
			// This case should have been caught when test was loaded.
			DPL(0, "Arbitrary number of machines specified when case requires specific number (%i)!?  DEBUGBREAK()-ing.",
				1, pBaseTest->m_pCase->m_iNumMachines);
			DEBUGBREAK();
		} // end if (specific number of machines required)
		else
		{
#endif // DEBUG
#pragma BUGBUG(vanceo, "Remove first if part when all tests switch to ADDTESTDATA")
			if ((pBaseTest->m_pCase->m_iNumMachines < 0) &&
				(pBaseTest->m_iNumMachines < (-1 * pBaseTest->m_pCase->m_iNumMachines)))
			{
				DPL(1, "Not enough machines to even run \"%s-%s\" (requires at least %i, there are %i slaves), not permuting.",
					4, pBaseTest->m_pCase->m_pszID, pBaseTest->m_pszInstanceID,
					pBaseTest->m_pCase->m_iNumMachines, pBaseTest->m_iNumMachines);
				goto DONE;
			} // end if (minimum number of machines required)
#ifdef DEBUG
		} // end else (not specific number of machines required)
#endif // DEBUG

		DPL(8, "Loaded test \"%s-%s\" will be permutated with all %i slaves currently in session.",
			3, pBaseTest->m_pCase->m_pszID, pBaseTest->m_pszInstanceID,
			pBaseTest->m_iNumMachines);
	} // end if (should use max num slaves)
	else
	{
		if (this->m_slaves.Count() < pBaseTest->m_iNumMachines)
		{
			DPL(1, "Not enough machines to even run \"%s-%s\" (requires exactly %i, there are %i slaves), not permuting.",
				4, pBaseTest->m_pCase->m_pszID, pBaseTest->m_pszInstanceID,
				pBaseTest->m_iNumMachines, this->m_slaves.Count());
			goto DONE;
		} // end if (not enough machines)
	} // end else (number of machines specified)


	// Array of lists of slave possibilities for each testing slot.
	papPossibleSlaves = (PTNSLAVESLIST*) LocalAlloc(LPTR, (pBaseTest->m_iNumMachines * sizeof (PTNSLAVESLIST)));
	if (papPossibleSlaves == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Storage space for particular permutation setup array.
	papTesters = (PTNSLAVEINFO*) LocalAlloc(LPTR, (pBaseTest->m_iNumMachines * sizeof (PTNSLAVEINFO)));
	if (papTesters == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	
	for(i = 0; i < pBaseTest->m_iNumMachines; i++)
	{
		papPossibleSlaves[i] = new (CTNSlavesList);
		if (papPossibleSlaves[i] == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)


		// If the user specified specific testing assignments for this test,
		// we need to take those into consideration.  Otherwise, just copy all
		// the possible slaves into the list.

		// Set this to NULL.  We use this to key off of whether we found an
		// assignment for this tester slot or not.  Somewhat obscure, but I'm
		// documenting it here, okay?
		pszValue = NULL;

		if (pBaseTest->m_pTesterAssignments != NULL)
		{
			for (j = 0; j < pBaseTest->m_pTesterAssignments->Count(); j++)
			{
				pAssignString = (PLSTRING) pBaseTest->m_pTesterAssignments->GetItem(j);
				if (pAssignString == NULL)
				{
					DPL(0, "Couldn't get tester assignment string %i!", 1, j);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get an item)


				// Allocate a string with plenty of room for the key (it can't
				// be larger than the entire string, so use that size).
				pszTemp = (char*) LocalAlloc(LPTR, (strlen(pAssignString->GetString()) + 1));
				if (pszTemp == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				StringGetKey(pAssignString->GetString(), pszTemp);
				if (strcmp(pszTemp, "") == 0)
				{
					DPL(0, "Tester number for tester assignment string %i (\"%s\") is invalid!",
						2, i, pAssignString->GetString());
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get strings key value)

				if (StringStartsWith(pszTemp, "~", TRUE))
				{
					// Skip the tilde
					iTesterNum = StringToInt(pszTemp + 1);
					fInvertAssign = TRUE;
				} // end if (it's a negative assignment)
				else
				{
					iTesterNum = StringToInt(pszTemp);
					fInvertAssign = FALSE;
				} // end else (it's a positive assignment)

				LocalFree(pszTemp);
				pszTemp = NULL;


				// If this assignment string refers to some other slot, skip it.
				if (iTesterNum != i)
					continue;


				pszValue = pAssignString->GetKeyValue();
				if (pszValue == NULL)
				{
					DPL(0, "Couldn't get slave name for tester assignment string %i (\"%s\")!",
						2, i, pAssignString->GetString());
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get strings key value)

				//Ignore error, assume BUFFER_TOO_SMALL
				this->ExpandBuiltinStringVars(pszValue, NULL, &dwTempSize);

				pszTemp = (char*) LocalAlloc(LPTR, dwTempSize);
				if (pszTemp == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				hr = this->ExpandBuiltinStringVars(pszValue, pszTemp, &dwTempSize);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't expand built-in string vars in \"%s\"!",
						1, pszValue);
					goto DONE;
				} // end if (couldn't expand built-in vars)


				// This destroys the original string, but we don't care since
				// it's a temp buffer anyway.
				dwNumFields = StringSplitIntoFields(pszTemp, "; ", pszTemp, &dwTempSize);

				// If it's inverted, we want to build a list with everyone but
				// the specified slaves.  If it's normal, build a list with only
				// the specified slaves.
				for(k = 0; k < this->m_slaves.Count(); k++)
				{
					pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(k);
					if (pSlave == NULL)
					{
						DPL(0, "Couldn't get slave %i!", 1, k);
						hr = E_FAIL;
						goto DONE;
					} // end if (the other slave doesn't exist)

					// Reuse the temp size variable
					for(dwTempSize = 0; dwTempSize < dwNumFields; dwTempSize++)
					{
						// We xor here:
						// if we're not invert assigning and we matched the name,
						// or we are invert assigning but we didn't match the name,
						// then we want to add the slave to the list.
						if ((fInvertAssign) ^
							(StringCmpNoCase(StringGetFieldPtr(pszTemp, dwTempSize), pSlave->m_szComputerName)))
						{
							hr = papPossibleSlaves[i]->Add(pSlave);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't add slave %s to list!",
									1, pSlave->m_szComputerName);
								goto DONE;
							} // end if (couldn't allocate object)
						} // end if (should copy slave)
					} // end for (each name)
				} // end for (each slave currently in the session)

				// We only handle 1 assignment string for each tester slot,
				// so stop looping since we've got one.
				break;
			} // end for (each testing assignment)
		} // end if (there are testing assignments)


		// pszValue will still be NULL if we didn't have any tester assignments
		// to check, or we did but didn't find anything pertaining to this tester
		// slot, so we have to build the generic list of usual suspects (i.e.
		// everybody).
		if (pszValue == NULL)
		{
			for(j = 0; j < this->m_slaves.Count(); j++)
			{
				pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(j);
				if (pSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i!", j);
					hr = E_FAIL;
					goto DONE;
				} // end if (the other slave doesn't exist)

				// Just add the slave.
				hr = papPossibleSlaves[i]->Add(pSlave);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't add slave %s to list!",
						1, pSlave->m_szComputerName);
					goto DONE;
				} // end if (couldn't allocate object)
			} // end for (each slave currently in the session)
		} // end else (there aren't related testing assignments)

		// If we didn't find any potential slaves for this tester slot, we
		// can't permute this test.  Bail.
		if (papPossibleSlaves[i]->Count() <= 0)
		{
			DPL(1, "No potential slaves for tester slot %i.", 1, i);
			goto DONE;
		} // end if (we didn't find any slaves for this list)
	} // end for (each testing slot)


	// If we got here, then all the slots had some possibilities.  Kick off
	// the recursion at slot 0.
	hr = this->PermuteRemainingSlots(pBaseTest, 0, papTesters,
									papPossibleSlaves, pTestList,
									piMaxNumPermutations);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't permute the slots!", 0);
		goto DONE;
	} // end if (couldn't permute remaining slots)


	DPL(1, "Built %i permutations for test \"%s-%s\".",
		3, pTestList->Count(), pBaseTest->m_pCase->m_pszID,
		pBaseTest->m_pszInstanceID);


DONE:

	if (papPossibleSlaves != NULL)
	{
		for(i = 0; i < pBaseTest->m_iNumMachines; i++)
		{
			if (papPossibleSlaves[i] != NULL)
			{
				delete (papPossibleSlaves[i]);
				papPossibleSlaves[i] = NULL;
			} // end if (allocated list for slot)
		} // end for (each tester slot)

		LocalFree(papPossibleSlaves);
		papPossibleSlaves = NULL;
	} // end if (allocated memory)

	if (papTesters != NULL)
	{
		LocalFree(papTesters);
		papTesters = NULL;
	} // end if (allocated memory)

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated string)

	if (fUsingMaxNumSlaves)
	{
		pBaseTest->m_iNumMachines = 0;
		fUsingMaxNumSlaves = FALSE;
	} // end if (used max num slaves)

	return (hr);
} // CTNMaster::BuildTestPermutationsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::PermuteRemainingSlots()"
//==================================================================================
// CTNMaster::PermuteRemainingSlots
//----------------------------------------------------------------------------------
//
// Description: Recursive function that builds all the possible valid test instances
//				for each configuration of tester assignments.  If the module doesn't
//				deny it (via the optional CanRun callback), then each is added to
//				the test list passed in.
//				The passed in arrays will be used for recursion.  The PTNSLAVEINFO
//				array must be NULLed out, and the PTNSLAVESLIST array must be
//				populated with lists of valid testers at the first call to this
//				function.  Both must be iNumMachines long at the start, although
//				papRemainingPossibleLists will be bumped further and further down
//				at each recursion level.
//				It's important to note that the tests in the list will NOT have
//				valid test IDs.
//				The slave list lock is assumed to be held with only one level of
//				recursion.
//
// Arguments:
//	PTNTESTFROMFILE pBaseTest					Test to use.
//	int iTestingSlot							Which slot this recursive call
//												should begin at, initialize to 0.
//	PTNSLAVEINFO* apTesters						Current tester list configuration.
//	PTNSLAVESLIST* papRemainingPossibleLists	Array of lists of slaves for each of
//												the remaining tester slots.
//	PTNTESTMSLIST pResults						List to add valid configurations to.
//	int* piMaxNumPermutations					Place to store maximum number of
//												permutations that could be used (not
//												necessarily number of items actually
//												in built test list).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::PermuteRemainingSlots(PTNTESTFROMFILE pBaseTest,
										int iTestingSlot,
										PTNSLAVEINFO* apTesters,
										PTNSLAVESLIST* papRemainingPossibleLists,
										PTNTESTMSLIST pResults,
										int* piMaxNumPermutations)
{
	HRESULT				hr = S_OK;
	int					iRemainingSlots;
	PTNSLAVESLIST*		papSubRemainingPossibleLists = NULL;
	int					i;
	int					j;
	int					k;
	BOOL				fSkip;
	PTNSLAVEINFO		pSlave;
	PTNSLAVEINFO		pOtherSlave;


	iRemainingSlots = pBaseTest->m_iNumMachines - iTestingSlot - 1;

	// We need to use a temporary duplicate array for each remaining slot.
	papSubRemainingPossibleLists = (PTNSLAVESLIST*) LocalAlloc(LPTR, (iRemainingSlots * sizeof (PTNSLAVESLIST)));
	if (papSubRemainingPossibleLists == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Loop through all the possible slaves that can be in this slot (the first
	// list in the remaining lists array).  
	for(i = 0; i < papRemainingPossibleLists[0]->Count(); i++)
	{
		pSlave = (PTNSLAVEINFO) papRemainingPossibleLists[0]->GetItem(i);
		if (pSlave == NULL)
		{
			DPL(0, "Couldn't get possible slave %i for slot %i",
				2, i, iTestingSlot);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't allocate memory)

		apTesters[iTestingSlot] = pSlave;

		fSkip = FALSE;

		// Rebuild the remaining lists without the current slave.
		for(j = 0; j < iRemainingSlots; j++)
		{
			papSubRemainingPossibleLists[j] = new (CTNSlavesList);
			if (papSubRemainingPossibleLists[j] == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			for(k = 0; k < papRemainingPossibleLists[j + 1]->Count(); k++)
			{
				pOtherSlave = (PTNSLAVEINFO) papRemainingPossibleLists[j + 1]->GetItem(k);
				if (pOtherSlave == NULL)
				{
					DPL(0, "Couldn't get slave %i for remaining list %i",
						2, k, j);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't allocate memory)

				// Only copy the item if it's not the current slave we're working
				// with.
				if (pOtherSlave != pSlave)
				{
					hr = papSubRemainingPossibleLists[j]->Add(pOtherSlave);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add slave %s to sub list!",
							1, pOtherSlave->m_szComputerName);
						goto DONE;
					} // end if (couldn't allocate object)
				} // end if (it's not the current guy)
			} // end for (each item in the list)

			// If nothing was copied in the sub possibilities list, its obviously an
			// invalid configuration, so skip it.
			if (papSubRemainingPossibleLists[j]->Count() <= 0)
			{
				fSkip = TRUE;
				break;
			} // end if (nothing was copied)
		} // end for (each remaining list)

		// If this is still a valid permutation, continue.  If we're the last slot to
		// check, let the module accept or reject it, otherwise, recursively call
		// ourselves to get to that last slot.
		if (! fSkip)
		{
			if (iRemainingSlots == 0)
			{
				// Here's a possible configuration, increase the count.
				(*piMaxNumPermutations)++;

				if (pBaseTest->m_pCase->m_pfnCanRun != NULL)
				{
					TNCANRUNDATA	tncrd;


					ZeroMemory(&tncrd, sizeof (TNCANRUNDATA));
					tncrd.dwSize = sizeof (TNCANRUNDATA);
					tncrd.pMaster = this;
					tncrd.pTest = pBaseTest->m_pCase;
					tncrd.iNumMachines = pBaseTest->m_iNumMachines;
					tncrd.apTesters = (PTNMACHINEINFO*) apTesters;
					tncrd.pStringData = pBaseTest->m_pStoredDataList;
					// tncrd.fCanRun = FALSE;

					hr = pBaseTest->m_pCase->m_pfnCanRun(&tncrd);
					if (hr != S_OK)
					{
						DPL(0, "Module's CanRun callback failed!", 0);
						goto DONE;
					} // end if (app's callback failed)

					fSkip = ! (tncrd.fCanRun);
				} // end if (the module wants to verify the machine can run the test)
				else
				{
					// Since the module doesn't care, then they must allow this
					// configuration.  fSkip will be FALSE.
				} // end else (module doesn't care)


				// If the module didn't deny this permutation, create a test instance
				// object for it, and add it to the list
				if (! fSkip)
				{
					PTNTESTINSTANCEM	pPermutation;


					pPermutation = new (CTNTestInstanceM)(0xFFFFFFFF,
														pBaseTest,
														pBaseTest->m_pCase,
														pBaseTest->m_iNumMachines,
														//0,
														NULL);
					if (pPermutation == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate object)

					// Copy the list of people in this permutation over to the
					// testing slots.
					for(j = 0; j < pBaseTest->m_iNumMachines; j++)
					{
						apTesters[j]->m_dwRefCount++; // it's in the slot now
						pPermutation->m_paTesterSlots[j].pSlave = apTesters[j];
					} // end for (each permutation)


					hr = pResults->Add(pPermutation);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add permutation %x to list!",
							1, pPermutation);

						delete (pPermutation);
						pPermutation = NULL;

						goto DONE;
					} // end if (couldn't add)
				} // end if (can run this permutation)
			} // end if (we're the last slot)
			else
			{
				hr = this->PermuteRemainingSlots(pBaseTest,
												iTestingSlot + 1,
												apTesters,
												papSubRemainingPossibleLists,
												pResults,
												piMaxNumPermutations);
				if (hr != S_OK)
				{
					DPL(0, "Recursively filling remaining slots failed!", 0);
					goto DONE;
				} // end if (couldn't fill remaining test slots)
			} // end else (there are more slots)
		} // end if (a valid permutation)


		for(j = 0; j < iRemainingSlots; j++)
		{
			if (papSubRemainingPossibleLists[j] != NULL)
			{
				delete (papSubRemainingPossibleLists[j]);
				papSubRemainingPossibleLists[j] = NULL;
			} // end if (allocated list)
		} // end for (each array entry)

		// Reset this back to how it was before
		apTesters[iTestingSlot] = NULL;


		// If we only need one permutation, and we now have it, we can bail early.
		if ((pBaseTest->m_dwPermutationMode == TNTPM_ONCE) &&
			(pResults->Count() > 0))
		{
			break;
		} // end if (already have once permutation needed)
	} // end for (each possible slave for this slot)


DONE:

	if (papSubRemainingPossibleLists != NULL)
	{
		for(i = 0; i < iRemainingSlots; i++)
		{
			if (papSubRemainingPossibleLists[i] != NULL)
			{
				delete (papSubRemainingPossibleLists[i]);
				papSubRemainingPossibleLists[i] = NULL;
			} // end if (allocated list)
		} // end for (each array entry)

		LocalFree(papSubRemainingPossibleLists);
		papSubRemainingPossibleLists = NULL;
	} // end if (allocated array)

	return (hr);
} // CTNMaster::PermuteRemainingSlots
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::GetActiveTest()"
//==================================================================================
// CTNMaster::GetActiveTest
//----------------------------------------------------------------------------------
//
// Description: Searches for an already active test that matches the given case ID
//				and tester array.  The test is then returned.  If no matching test
//				exists, NULL is returned.
//				The slave lock is assumed to be held.
//
// Arguments:
//	PTNTESTINSTANCEM pParentTest	Parent test to search under, if any.
//	char* szCaseID					Module defined case ID of test.
//	int iNumMachines				Number of machines in the test.
//	TNCTRLMACHINEID* aTestersArray	Array of machine IDs in tester number order.
//
// Returns: Pointer to test, or NULL if not found.
//==================================================================================
PTNTESTINSTANCEM CTNMaster::GetActiveTest(PTNTESTINSTANCEM pParentTest,
										char* szCaseID,
										int iNumMachines,
										TNCTRLMACHINEID* aTestersArray)
{
	PTNTESTINSTANCEM	pTest = NULL;
	int					i;
	int					j;
	PTNSLAVEINFO		pSlave = NULL;
	PTNSLAVEINFO		pRequestedTester = NULL;
	PTNSLAVEINFO		pExistingTester = NULL;


	// If there's a parent test look through all subtests of that parent test.  If
	// not, look through all the slaves current tests for an unfinished test with
	// the same case ID and testerlist.  pTest will point to it if we find it.
	
	for(i = 0; i < ((pParentTest != NULL) ? pParentTest->m_subtests.Count() : this->m_slaves.Count()); i++)
	{
		if (pParentTest != NULL)
		{
			pTest = (PTNTESTINSTANCEM) pParentTest->m_subtests.GetItem(i);
			if (pTest == NULL)
			{
				DPL(0, "Couldn't get subtest %i!", 1, i);
				goto DONE;
			} // end if (couldn't get slave)
		} // end if (there's a parent test)
		else
		{
			pSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
			if (pSlave == NULL)
			{
				DPL(0, "Couldn't get slave %i!", 1, i);
				goto DONE;
			} // end if (couldn't get slave)

			// If the slave isn't currently working, skip him.
			if (pSlave->m_pCurrentTest == NULL)
				continue;

			pTest = pSlave->m_pCurrentTest;

#pragma BUGBUG(vanceo, "What about ongoing tests?")
		} // end else (there's no parent test)

		// Make sure the test we're looking at is the right case and hasn't
		// already started.
		if ((strcmp(pTest->m_pCase->m_pszID, szCaseID) == 0) &&
			(! pTest->m_fStarted))
		{
#ifdef DEBUG
			if (pTest->m_iNumMachines != iNumMachines)
			{
				DPL(0, "A test \"%s\" exists (with %i testers) but the slave announced it needs %i testers!?",
					3, pTest->m_pCase->m_pszID, pTest->m_iNumMachines,
					iNumMachines);

				DEBUGBREAK();

				pTest = NULL;

				goto DONE;
			} // end if (there's a disparate number of testers)
#endif // DEBUG

			// For this loop, we go through to see if the list of people
			// assigned to the test match what the caller is specifying as its
			// requested list.  We need to assume we found the test, so that if
			// we find even one person who differs, we can refute this.
			for(j = 0; j < iNumMachines; j++)
			{
				pRequestedTester = this->m_slaves.GetSlaveByID(&(aTestersArray[j]));
				pExistingTester = pTest->m_paTesterSlots[j].pSlave;

#ifdef DEBUG
				if (pExistingTester == NULL)
				{
					DPL(0, "Active test found (ID %u) but a testing slot (%i) is empty!?",
						2, pTest->m_dwUniqueID, j);

					DEBUGBREAK();

					pTest = NULL;
					goto DONE;
				} // end if (nobody)
#endif // DEBUG

				// It only takes one person being in a different position to
				// determine we found a different subtest (that happens to have
				// the same case ID).
				if (pRequestedTester != pExistingTester)
				{
					DPL(1, "Requested tester for slot %i (%s) is not the same as existing tester in slot %i (%s) of test %u, not using test.",
						5, j, pRequestedTester->m_szComputerName, j,
						pExistingTester->m_szComputerName,
						pSlave->m_pCurrentTest->m_dwUniqueID);

					pTest = NULL;
					break;
				} // end if (the testers don't match)
			} // end for (each subtest tester)

			// If it did match, we're done.
			if (pTest != NULL)
				break;
		} // end if (the test we found looks correct)
		else
		{
			pTest = NULL;
		} // end else (the test we found is not correct)
	} // end for (each slave)


DONE:

	return (pTest);
} // CTNMaster::GetActiveTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::StartJobThread()"
//==================================================================================
// CTNMaster::StartJobThread
//----------------------------------------------------------------------------------
//
// Description: Starts the Job thread if it wasn't already running.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::StartJobThread(void)
{
	HRESULT		hr;
	DWORD		dwThreadID;


	// Create kill thread manual reset event if it doesn't exist
	if (this->m_hKillJobThreadEvent == NULL)
	{
		this->m_hKillJobThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (this->m_hKillJobThreadEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create kill Job thread event!", 0);
			return (hr);
		} // end if (couldn't create kill thread event)
	} // end if (kill event doesn't exist)

	// Create the actual thread if it doesn't exist
	if (this->m_hJobThread == NULL)
	{
		this->m_hJobThread = CreateThread(NULL, 0, MasterJobThread,
										this, 0, &dwThreadID);
		if (this->m_hJobThread == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create Job thread!", 0);
			return (hr);
		} // end if (couldn't create thread)
	} // end if (thread doesn't exist)

	return (S_OK);
} // CTNMaster::StartJobThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::KillJobThread()"
//==================================================================================
// CTNMaster::KillJobThread
//----------------------------------------------------------------------------------
//
// Description: Shuts down the Job thread.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::KillJobThread(void)
{
	HRESULT		hr = S_OK;
	DWORD		dwExitCode;


	// Close all items associated with the thread
	if (this->m_hJobThread != NULL)
	{
		// First check to see if it already died on its own
		if (! GetExitCodeThread(this->m_hJobThread, &dwExitCode))
		{
			hr = GetLastError();
			DPL(0, "Couldn't get thread exit code!", 0);
			goto DONE;
		} // end if (couldn't get the thread's exit code)

		if (dwExitCode != STILL_ACTIVE)
		{
			DPL(0, "Job thread killed itself!  %e", 1, dwExitCode);

			// Note: hr == S_OK
			goto DONE;
		} // end if (the thread must have killed itself)

		if (this->m_hKillJobThreadEvent == NULL) // ack, we won't be able to tell it to die
		{
			DPL(0, "Kill thread event doesn't exist, can't tell it to die!", 0);
			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (kill event doesn't exist)

		if (! SetEvent(this->m_hKillJobThreadEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set kill thread event!", 0);
			goto DONE;
		} // end if (couldn't set kill event)

		hr = WaitForSingleObject(this->m_hJobThread, TIMEOUT_DIE_PERIODICCHECKTHREAD);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// what we want
				hr = S_OK;
			  break;

			case WAIT_TIMEOUT:
				// Hmm, let's try getting the exit code again
				if (! GetExitCodeThread(this->m_hJobThread, &dwExitCode))
				{
					hr = GetLastError();
					DPL(0, "Couldn't get thread exit code!", 0);
					goto DONE;
				} // end if (couldn't get the thread's exit code)

				if (dwExitCode != STILL_ACTIVE)
				{
					DPL(0, "Job thread died in unusual way!  %e", 1, dwExitCode);

					// Note: hr == S_OK
					goto DONE;
				} // end if (the thread must have killed itself)


				DPL(0, "Waited %i ms for job thread to die but it didn't!",
					1, TIMEOUT_DIE_PERIODICCHECKTHREAD);


#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
				DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

			  break;

			default:
				DPL(0, "Got unexpected return code (%i) from WaitForSingleObject on the test thread!",
					1, hr);
			  break;
		} // end switch (on result of waiting for thread to die)

	} // end if (the ping thread exists)


DONE:

	if (this->m_hJobThread != NULL)
	{
		CloseHandle(this->m_hJobThread);
		this->m_hJobThread = NULL;
	} // end if (the slave ping thread existed)

	if (this->m_hKillJobThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillJobThreadEvent);
		this->m_hKillJobThreadEvent = NULL;
	} // end if (the kill ping thread event existed)


	return (hr);
} // CTNMaster::KillJobThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::SendPingTo()"
//==================================================================================
// CTNMaster::SendPingTo
//----------------------------------------------------------------------------------
//
// Description: Sends a ping to the specific slave,
//
// Arguments:
//	PTNSLAVEINFO pSlave		Pointer to the slave info object.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::SendPingTo(PTNSLAVEINFO pSlave)
{
	HRESULT						hr = S_OK;
	PTNSENDDATA					pSendData = NULL;
	PCTRLMSG_MASTERPINGSLAVE	pPingMsg = NULL;
	LPVOID						pvAddress = NULL;
	DWORD						dwAddressSize = pSlave->m_commdata.dwAddressSize;


	
	if (pSlave->m_commdata.fDropped)
	{
		DPL(0, "Can't send ping to %s because his connection has been dropped!",
			1, pSlave->m_szComputerName);
		hr = TNERR_CONNECTIONDROPPED;
		goto DONE;
	} // end if (connection to slave is down)

	// Update the number of pings we've transmitted
	pSlave->m_dwNumPingsSinceReceive++;


	if (NUM_PINGS_TO_ATTEMPT == 1)
	{
		DPL(0, "Sending a ping to %s", 1, pSlave->m_szComputerName);
	} // end if (we'll only try sending one ping)
	else
	{
		DPL(0, "Sending ping #%u to %s",
			2, pSlave->m_dwNumPingsSinceReceive,
			pSlave->m_szComputerName);
	} // end else (we'll try pinging multiple times)


	pSendData = new CTNSendData;
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pSendData->m_fGuaranteed = TRUE;


	// The Send object deletes the address in its destructor so we work
	// around this by copying the address

	pvAddress = LocalAlloc(LPTR, dwAddressSize);
	if (pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Copy address to pvAddress
	CopyMemory(pvAddress, pSlave->m_commdata.pvAddress, dwAddressSize);
	
	pSendData->m_dwAddressSize = dwAddressSize; 		
	pSendData->m_pvAddress = pvAddress;
	pvAddress = NULL;
	pSendData->m_dwDataSize = sizeof(CTRLMSG_MASTERPINGSLAVE);

	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)	

	// Copy the master ping slave data back
	// This consists mainly of the slave ID so that the master can use
	// it for reference
	
	ZeroMemory(pSendData->m_pvData, pSendData->m_dwDataSize);

	pPingMsg = (PCTRLMSG_MASTERPINGSLAVE) pSendData->m_pvData;
	pPingMsg->dwSize = pSendData->m_dwDataSize;
	pPingMsg->dwType = CTRLMSGID_MASTERPINGSLAVE;

	// Queue the send
	
	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	}

	pSendData = NULL;
	

DONE:

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (have object)

	if (pvAddress != NULL)
	{
		LocalFree(pvAddress);
		pvAddress = NULL;
	} // end if (allocated memory)

	return hr;
} // CTNMaster::SendPingTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::InternalExpandStringVars()"
//==================================================================================
// CTNMaster::InternalExpandStringVars
//----------------------------------------------------------------------------------
//
// Description: Expands all variables of type "STRING" found in the lists passed in
//				contained in the given string.  If the buffer pointer is NULL, the
//				size need to hold the new string is placed in pdwBufferSize, and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* szString					String to expand.
//	char* pszBuffer					Pointer to buffer to place results, or NULL to
//									retrieve size required.
//	DWORD* pdwBufferSize			Pointer to size of buffer, or place to store
//									size required.
//	PTNOUTPUTVARSLIST* apLists		Array of pointers to lists to search, in
//									priority order.
//	DWORD dwNumLists				Number of lists in preceding array.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::InternalExpandStringVars(char* szString, char* pszBuffer,
											DWORD* pdwBufferSize,
											PTNOUTPUTVARSLIST* apLists,
											DWORD dwNumLists)
{
#define VARNAME_BUFFER_INCREMENT	50

	HRESULT		hr;
	char*		pszTemp = NULL;
	DWORD		dwTempSize = 0;
	char*		pszName;
	DWORD		dwNameLen;
	char*		pszCurrentSrc;
	char*		pszCurrentDest;
	DWORD		dwTemp;
	PVOID		pvValue;
	DWORD		dwValueSize;


	pszCurrentSrc = szString;
	pszCurrentDest = pszBuffer;
	(*pdwBufferSize) = 0;
	do
	{
		hr = this->SearchStringForDynamicVars(pszCurrentSrc, &pszName, &dwNameLen);
		if (hr != S_OK)
		{
			DPL(0, "Failed searching string for dynamic variables!", 0);
			goto DONE;
		} // end if (failed searching string)

		// If we didn't find any more variables, finish off the size required and
		// we're done.
		if (pszName == NULL)
		{
			// Include room for rest of string plus NULL termination.
			(*pdwBufferSize) += strlen(pszCurrentSrc) + 1;

			// Actually copy rest of string if there's a buffer.
			if (pszBuffer != NULL)
				strcpy(pszCurrentDest, pszCurrentSrc);

			break;
		} // end if (no more dynamic variables)

		// Move the current source pointer past the found item + the trailing @
		pszCurrentSrc = pszName + dwNameLen + 1;


		// Make sure the temp buffer can hold the variable name
		if (dwNameLen >= dwTempSize)
		{
			SAFE_LOCALFREE(pszTemp);

			dwTempSize += VARNAME_BUFFER_INCREMENT;
			pszTemp = (char*) LocalAlloc(LPTR, dwTempSize);
			if (pszTemp == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
		} // end if (need to increase buffer)

		CopyMemory(pszTemp, pszName, dwNameLen);
		pszTemp[dwNameLen] = '\0'; // make sure it's NULL terminated

		for (dwTemp = 0; dwTemp < dwNumLists; dwTemp++)
		{
			hr = apLists[dwTemp]->GetVariable(pszTemp, "STRING", &pvValue, &dwValueSize);

			//BUGBUG is this what we want?

			// If it's a real variable, just not a "STRING" type, then we want to
			// skip over it.
			if (hr == ERROR_INVALID_DATA)
				hr = ERROR_NOT_FOUND;

			if (hr == S_OK)
			{
				(*pdwBufferSize) += dwValueSize;

				if (pszBuffer != NULL)
				{
					CopyMemory(pszCurrentDest, pvValue, dwValueSize);
					pszCurrentDest += dwValueSize;
				} // end if (there's a buffer)

				// Stop looking through the lists
				break;
			} // end if (found variable)
			else if (hr != ERROR_NOT_FOUND)
			{
				DPL(0, "Couldn't get variable \"%s\" from list %u!",
					2, pszTemp, dwTemp);
				goto DONE;
			} // end if (wacky error)
		} // end for (each list to search)


		// If we didn't find the item in any list, just copy what we thought was
		// a variable, including the @ tokens.
		if (hr == ERROR_NOT_FOUND)
		{
			(*pdwBufferSize) += dwNameLen + 2; // + 2 for @ tokens

			if (pszBuffer != NULL)
			{
				CopyMemory(pszCurrentDest, pszName - 1, dwNameLen + 2);
				pszCurrentDest += dwNameLen + 2;
			} // end if (there's a buffer)

			hr = S_OK;
		} // end if (didn't find variable)
	} // end do (while still more dynamic variables)
	while (TRUE);

	if (pszBuffer == NULL)
		hr = ERROR_BUFFER_TOO_SMALL;
	else
		hr = S_OK;


DONE:

	SAFE_LOCALFREE(pszTemp);

	return (hr);
} // CTNMaster::InternalExpandStringVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::InternalIsVarOfType()"
//==================================================================================
// CTNMaster::InternalIsVarOfType
//----------------------------------------------------------------------------------
//
// Description: Checks if the given string is a dynamic variable of the given type
//				contained in one of the lists passed in.  If it is, then a pointer
//				to its value and its size are returned.  Otherwise they are set to
//				NULL and 0.
//
// Arguments:
//	char* szString				String to check.
//	char* szType				Type of variable required.
//	PVOID* ppvValueLocation		Pointer to set to value.
//	DWORD* pdwValueSize			Place to store size of the value.
//	PTNOUTPUTVARSLIST* apLists	Array of pointers to lists to search, in priority
//								order.
//	DWORD dwNumLists			Number of lists in preceding array.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::InternalIsVarOfType(char* szString, char* szType,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize,
									PTNOUTPUTVARSLIST* apLists,
									DWORD dwNumLists)
{
	HRESULT		hr;
	char*		pszTemp = NULL;
	char*		pszName;
	DWORD		dwNameLen;
	DWORD		dwTemp;


	(*ppvValueLocation) = NULL;
	(*pdwValueSize) = 0;

	hr = this->SearchStringForDynamicVars(szString, &pszName, &dwNameLen);
	if (hr != S_OK)
	{
		DPL(0, "Failed searching string for dynamic variables!", 0);
		goto DONE;
	} // end if (failed searching string)

	// If it doesn't contain a dynamic variable, we're done
	if (pszName == NULL)
		goto DONE;


	// Grab the variable name

	pszTemp = (char*) LocalAlloc(LPTR, dwNameLen + 1);
	if (pszTemp == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory))

	CopyMemory(pszTemp, pszName, dwNameLen);


	// Loop through the lists, looking for the variable
	for (dwTemp = 0; dwTemp < dwNumLists; dwTemp++)
	{
		hr = apLists[dwTemp]->GetVariable(pszTemp, szType, ppvValueLocation,
											pdwValueSize);
		if (hr == S_OK)
		{
			// Stop looking through the lists
			break;
		} // end if (found variable)
		else if (hr != ERROR_NOT_FOUND)
		{
			DPL(0, "Couldn't get variable \"%s\" from list %u!",
				2, pszTemp, dwTemp);
			goto DONE;
		} // end if (wacky error)
	} // end for (each list to search)


	// If we didn't find it in any list, oh well.
	if (hr == ERROR_NOT_FOUND)
		hr = S_OK;


DONE:

	SAFE_LOCALFREE(pszTemp);

	return (hr);
} // CTNMaster::InternalIsVarOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::FreeOutputVarsAndData()"
//==================================================================================
// CTNMaster::FreeOutputVarsAndData
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
//	PTNSLAVEINFO pSlave		Slave to match, or NULL for all.
//	char* pszName			Output variable name to match, or NULL for all.
//	char* pszType			Output variable type to match, or NULL for all.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMaster::FreeOutputVarsAndData(char* pszCaseID, char* pszInstanceID,
										PTNSLAVEINFO pSlave, char* pszName,
										char* pszType)
{
	HRESULT				hr = S_OK;
	PTNTESTINSTANCEM	pTest;
	DWORD				dwNumSlotsWithData;
	PTNOUTPUTVAR		pVar;
	int					i;
	int					j;
	int					k;



	DPL(1, "Freeing output variables and data (%s, %s, %x, %s, %s).",
		5, pszCaseID, pszInstanceID, pSlave, pszName, pszType);

	this->m_testswithvars.EnterCritSection();


	// Loop backwards (so our counter doesn't get off) through all the output vars
	// in the list.
	for(i = this->m_testswithvars.Count() - 1; i >= 0; i--)
	{
		pTest = (PTNTESTINSTANCEM) this->m_testswithvars.GetItem(i);
		if (pTest == NULL)
		{
			DPL(0, "Couldn't get item %i of test with vars list!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)


		// If we're matching case ID, make sure it does.
		if (pszCaseID != NULL)
		{
			if (strcmp(pTest->m_pCase->m_pszID, pszCaseID) != 0)
				continue;
		} // end if (should check case ID)

		// If we're matching instance ID, make sure it does.
		if (pszInstanceID != NULL)
		{
			if ((pTest->m_pLoadedTest == NULL) ||
				(strcmp(pTest->m_pLoadedTest->m_pszInstanceID, pszInstanceID) != 0))
			{
				continue;
			} // end if (no loaded test, or doesn't match instance ID)
		} // end if (should check instance ID)



		dwNumSlotsWithData = 0;


		// Loop through all the tester slots for this test.
		for(j = 0; j < pTest->m_iNumMachines; j++)
		{
#ifdef DEBUG
			// There should never be no output data but have vars, or output data
			// but no vars, because we only keep the data if vars existed.
			if (((pTest->m_paTesterSlots[j].pvOutputData == NULL) &&
				(pTest->m_paTesterSlots[j].pVars != NULL)) ||
				((pTest->m_paTesterSlots[j].pvOutputData != NULL) &&
				(pTest->m_paTesterSlots[j].pVars == NULL)))
			{
				DPL(0, "Tester slot %i of test %u output data = %x but vars list = %x!  DEBUGBREAK()-ing.",
					4, j, pTest->m_dwUniqueID,
					pTest->m_paTesterSlots[j].pvOutputData,
					pTest->m_paTesterSlots[j].pVars);

				DEBUGBREAK();
			} // end if (no output data but vars, or vice versa)
#endif // DEBUG

			if (pTest->m_paTesterSlots[j].pVars != NULL)
			{
				// This slot has data, so note it.
				dwNumSlotsWithData++;


				// If we're matching case ID, make sure it does.
				if (pszCaseID != NULL)
				{
					if (strcmp(pTest->m_pCase->m_pszID, pszCaseID) != 0)
						continue;
				} // end if (should check case ID)

				// If we're matching instance ID, make sure it does.
				if (pszInstanceID != NULL)
				{
					if ((pTest->m_pLoadedTest == NULL) ||
						(strcmp(pTest->m_pLoadedTest->m_pszInstanceID, pszInstanceID) != 0))
					{
						continue;
					} // end if (no loaded test, or doesn't match instance ID)
				} // end if (should check instance ID)

				// If we're matching slaves, make sure it does.
				if (pSlave != NULL)
				{
					if (pTest->m_paTesterSlots[j].pSlave != pSlave)
						continue;
				} // end if (should check slave)

				// If we're matching individual variable names or types, check
				// each one.
				if ((pszName != NULL) || (pszType != NULL))
				{
					// Loop through all the output variables (backwards so our
					// count doesn't get off).
					for(k = pTest->m_paTesterSlots[j].pVars->Count() - 1; k >= 0; k--)
					{
						pVar = (PTNOUTPUTVAR) pTest->m_paTesterSlots[j].pVars->GetItem(k);
						if (pVar == NULL)
						{
							DPL(0, "Couldn't get var %i!", 1, k);
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
							hr = pTest->m_paTesterSlots[j].pVars->Remove(k);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't remove item %i from test %u's tester %i output vars list!",
									3, k, pTest->m_dwUniqueID, j);
								goto DONE;
							} // end if (couldn't remove item)
						} // end if (matching names and it does)
					} // end for (each output variable)

					// If we didn't remove all the variables, we can't remove
					// the whole list.
					if (pTest->m_paTesterSlots[j].pVars->Count() > 0)
					{
						continue;
					} // end if (still variables left)
				} // end if (should check names or types)


				// If we got here, it means we can remove all the variables left
				// plus the output data.
				delete (pTest->m_paTesterSlots[j].pVars);
				pTest->m_paTesterSlots[j].pVars = NULL;

				LocalFree(pTest->m_paTesterSlots[j].pvOutputData);
				pTest->m_paTesterSlots[j].pvOutputData = NULL;

				pTest->m_paTesterSlots[j].dwOutputDataSize = 0;


				// We deleted the data, so decrement the count.
				dwNumSlotsWithData--;
			} // end if (there are vars)
		} // end for (each tester slot)


		// If we removed all the slots with data, there's no sense in keeping
		// the test in the list.
		if (dwNumSlotsWithData == 0)
		{
			hr = this->m_testswithvars.Remove(i);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't remove item %i from test with vars list!", 1, i);
				goto DONE;
			} // end if (couldn't remove item)
		} // end if (no more data)
	} // end for (each test)


DONE:

	this->m_testswithvars.LeaveCritSection();

	return (hr);
} // CTNMaster::FreeOutputVarsAndData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMaster::PerformReachCheck()"
//==================================================================================
// CTNMaster::PerformReachCheck
//----------------------------------------------------------------------------------
//
// Description: Has the source slave try to contact the target slave using the given
//				method.
//				The slave list lock is assumed to be held once (i.e. no levels of
//				recursion).  It will be dropped during the course of this call.
//
// Arguments:
//	DWORD dwMethod					Method of communication to try.
//	PTNSLAVEINFO pSource			Slave who will try to connect.
//	PVOID pvSourceMethodData		Pointer to additional data for source slave to
//									use when contacting, if any.
//	DWORD dwSourceMethodDataSize	Size of additional source method data, if any.
//	PVOID pvAddressesToTry			Pointer to buffer with addresses for source to
//									try.
//	DWORD dwAddressesToTrySize		Size of addresses buffer.
//	PTNSLAVEINFO pTarget			Slave who will be contacted.
//	PVOID pvTargetMethodData		Pointer to additional data for target slave to
//									use when preparing to be contacted, if any.
//	DWORD dwTargetMethodDataSize	Size of additional target method data, if any.
//	BOOL* pfResult					Pointer to boolean to fill with result (success or
//									failure).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNMaster::PerformReachCheck(DWORD dwMethod,
									PTNSLAVEINFO pSource,							
									PVOID pvSourceMethodData,
									DWORD dwSourceMethodDataSize,
									PVOID pvAddressesToTry,
									DWORD dwAddressesToTrySize,
									PTNSLAVEINFO pTarget,
									PVOID pvTargetMethodData,
									DWORD dwTargetMethodDataSize,
									BOOL* pfResult)
{
	HRESULT						hr;
	int							i;
	PTNSLAVEINFO				pOtherSlave = NULL;
	TNCTRLMACHINEID				idPreviousTarget;
	BOOL						fDroppedSlaveLock = FALSE;
	PTNREACHCHECK				pReachCheck = NULL;
	PTNREACHCHECKTARGET			pReachCheckTarget = NULL;
	PTNSENDDATA					pSendData = NULL;
	LPBYTE						lpCurrent;
	PCTRLMSG_ACCEPTREACHCHECK	pAcceptReachCheckMsg;
	PCTRLMSG_CONNECTREACHCHECK	pConnectReachCheckMsg;


	DPL(9, "==>(%u, %x [%s], %x, %u, %x, %u, %x [%s], %x, %u, %x)",
		12,
		dwMethod,
		pSource,
		pSource->m_szComputerName,
		pvSourceMethodData,
		dwSourceMethodDataSize,
		pvAddressesToTry,
		dwAddressesToTrySize,
		pTarget,
		pTarget->m_szComputerName,
		pvTargetMethodData,
		dwTargetMethodDataSize,
		pfResult);

	(*pfResult) = FALSE;


	// We want to make sure they don't get deleted when we drop the lock.  It should
	// never happen, because we should only be called from within the job thread,
	// and slaves can only be killed within the job thread, but I'm being paranoid.
	pSource->m_dwRefCount++;
	pTarget->m_dwRefCount++;


	// If there are multiple slave instances on the target machine in this session,
	// we may have already performed a reach check for the machine.  If so, we still
	// tell the slave about this reach check so he can copy the information he
	// gathered from the other instance.
	// Start by assuming we won't find this handy shortcut.
	ZeroMemory(&idPreviousTarget, sizeof (TNCTRLMACHINEID));
	if (pTarget->m_iNumOtherInstancesInSession > 0)
	{
		for(i = 0; i < this->m_slaves.Count(); i++)
		{
			pOtherSlave = (PTNSLAVEINFO) this->m_slaves.GetItem(i);
			if (pOtherSlave == NULL)
			{
				DPL(0, "Couldn't get existing slave %i!", 1, i);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get slave)

			if (this->AreOnSameMachine(pOtherSlave, pTarget))
			{
				// Has this exact reach check been performed already?
				pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pOtherSlave->m_id));
				if (pReachCheckTarget != NULL)
				{
					for(i = 0; i < pReachCheckTarget->m_checks.Count(); i++)
					{
						pReachCheck = (PTNREACHCHECK) pReachCheckTarget->m_checks.GetItem(i);
						if (pReachCheck == NULL)
						{
							DPL(0, "Couldn't get reach check %i!", i);
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't get item)


						// If we're found a check already existing, we have
						// the result we need.
						if ((pReachCheck->m_dwMethod == dwMethod) &&
							(pReachCheck->m_dwMethodDataSize == dwSourceMethodDataSize) &&
							((dwSourceMethodDataSize == 0) || (memcmp(pReachCheck->m_pvMethodData, pvSourceMethodData, dwSourceMethodDataSize) == 0)))
						{
							if (pReachCheck->m_fResult)
							{
								DPL(3, "%s already successfully reached another slave instance on %s earlier.",
									2, pSource->m_szComputerName,
									pTarget->m_szComputerName);
							} // end if (check succeeded)
							else
							{
								DPL(3, "%s couldn't reached another slave instance on %s earlier.",
									2, pSource->m_szComputerName,
									pTarget->m_szComputerName);
							} // end else (check failed)

							// Save this ID
							memcpy(&idPreviousTarget, &(pOtherSlave->m_id),
									sizeof (TNCTRLMACHINEID));

							// Stop searching.
							break;
						} // end if (found serial check)
					} // end for (each target)
				} // end if (found any reach check targets)
			} // end if (they're on the same machine)

			// If we found a reach check, we can stop looping.
			if (! IsZero(&idPreviousTarget, sizeof (TNCTRLMACHINEID)))
				break;
		} // end for (each slave)
	} // end if (multiple machines on same machine)



#pragma BUGBUG(vanceo, "No way to store target method data")

	// Build a reach test object for the source
	pReachCheck = new (CTNReachCheck)(this->m_dwCurrentUniqueID++, dwMethod,
									pvSourceMethodData, dwSourceMethodDataSize);
	if (pReachCheck == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pReachCheck->m_dwRefCount++; // we're using it


	// Make sure there's an item in the source's list for the target.

	pReachCheckTarget = pSource->m_reachchecktargets.GetTarget(&(pTarget->m_id));
	if (pReachCheckTarget == NULL)
	{
		pReachCheckTarget = new (CTNReachCheckTarget)(&(pTarget->m_id));
		if (pReachCheckTarget == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		hr = pSource->m_reachchecktargets.Add(pReachCheckTarget);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add reach check target (%s) to source's list!",
				1, pTarget->m_szComputerName);

			delete (pReachCheckTarget);
			pReachCheckTarget = NULL;

			goto DONE;
		} // end if (couldn't add item)
	} // end if (we haven't checked anything for target yet)

	// Add this check to the source's list for the target.
	hr = pReachCheckTarget->m_checks.Add(pReachCheck);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add reach check to source's list for target %s!",
			1, pTarget->m_szComputerName);
		goto DONE;
	} // end if (couldn't add item)




	// Create an event so we can be signalled when the acceptor (and also the
	// connector later) is ready
	pReachCheck->m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (pReachCheck->m_hEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create accept/connect reach check event!", 0);
		goto DONE;
	} // end if (couldn't create event)


	// If we didn't find a reach check result for a different instance on the target
	// machine already, we need to perform one.
	if (IsZero(&idPreviousTarget, sizeof (TNCTRLMACHINEID)))
	{
		// Make sure there's a temp self list for the target.

		pReachCheckTarget = pTarget->m_reachchecktargets.GetTarget(&(pTarget->m_id));
		if (pReachCheckTarget == NULL)
		{
			pReachCheckTarget = new (CTNReachCheckTarget)(&(pTarget->m_id));
			if (pReachCheckTarget == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			hr = pTarget->m_reachchecktargets.Add(pReachCheckTarget);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add self target list for %s!",
					1, pTarget->m_szComputerName);

				delete (pReachCheckTarget);
				pReachCheckTarget = NULL;

				goto DONE;
			} // end if (couldn't add item)
		} // end if (we haven't had anything checked for target yet)

		// Add this check to the target's self list.
		hr = pReachCheckTarget->m_checks.Add(pReachCheck);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add reach check to target's self list (target = %s)!",
				1, pTarget->m_szComputerName);
			goto DONE;
		} // end if (couldn't add item)


		// Tell the target to start accepting communication from the source.

		pSendData = new (CTNSendData);
		if (pSendData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		pSendData->m_fGuaranteed = TRUE;
		pSendData->m_dwAddressSize = pTarget->m_commdata.dwAddressSize;
		pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
		if (pSendData->m_pvAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (out of memory)
		CopyMemory(pSendData->m_pvAddress, pTarget->m_commdata.pvAddress,
				pSendData->m_dwAddressSize);

		pSendData->m_dwDataSize = sizeof (CTRLMSG_ACCEPTREACHCHECK)
								+ dwTargetMethodDataSize;
		pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
		if (pSendData->m_pvData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		pAcceptReachCheckMsg = (PCTRLMSG_ACCEPTREACHCHECK) pSendData->m_pvData;
		pAcceptReachCheckMsg->dwSize = pSendData->m_dwDataSize;
		pAcceptReachCheckMsg->dwType = CTRLMSGID_ACCEPTREACHCHECK;
		pAcceptReachCheckMsg->dwID = pReachCheck->m_dwID;
		pAcceptReachCheckMsg->dwMethod = dwMethod;
		pAcceptReachCheckMsg->dwMethodDataSize = dwTargetMethodDataSize;

		if (dwTargetMethodDataSize > 0)
		{
			CopyMemory((pAcceptReachCheckMsg + 1), pvTargetMethodData,
						dwTargetMethodDataSize);
		} // end if (there's method data)
		

		hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add send item to queue!", 0);
			goto DONE;
		} // end if (failed to add item to queue)

		pSendData = NULL; // forget about it so it doesn't get freed below


		// Wait for his response (successfully got ready to accept, or failed to do
		// the same).  We need to drop the slave list lock (which is assumed to be
		// held with only one level of recursion) so the receive thread can actually
		// do something with the replies.

		DPL(8, "Waiting for %s to get ready for connections by %s (method %u, reach check %u)...",
			4, pTarget->m_szComputerName, pSource->m_szComputerName,
			dwMethod, pReachCheck->m_dwID);

		fDroppedSlaveLock = TRUE;
		this->m_slaves.LeaveCritSection();


#pragma BUGBUG(vanceo, "No way to user cancel")
		hr = WaitForSingleObject(pReachCheck->m_hEvent, INFINITE);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// If the acceptor failed getting ready, we have to bail.
				if (! pReachCheck->m_fResult)
				{
					DPL(0, "%s was unable to start accepting a reach check by %s, error code is %e",
						3, pTarget->m_szComputerName, pSource->m_szComputerName,
						pReachCheck->m_hresult);
					goto DONE;
				} // end if (failed starting up)
			  break;

			default:
				// What?
				DPL(0, "Unexpected result from WaitForSingleObject!", 0);
				goto DONE;
			  break;
		} // end switch (on the wait result)

		this->m_slaves.EnterCritSection();
		fDroppedSlaveLock = FALSE;
	} // end if (didn't already contact the machine)


	// Tell the source to try contacting the target, include what addresses are
	// available.

	pSendData = new (CTNSendData);
	if (pSendData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pSendData->m_fGuaranteed = TRUE;
	pSendData->m_dwAddressSize = pSource->m_commdata.dwAddressSize;
	pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
	if (pSendData->m_pvAddress == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (out of memory)
	CopyMemory(pSendData->m_pvAddress, pSource->m_commdata.pvAddress,
			pSendData->m_dwAddressSize);

	pSendData->m_dwDataSize = sizeof (CTRLMSG_CONNECTREACHCHECK)
							+ dwSourceMethodDataSize
							+ dwAddressesToTrySize;
	pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
	if (pSendData->m_pvData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	pConnectReachCheckMsg = (PCTRLMSG_CONNECTREACHCHECK) pSendData->m_pvData;
	pConnectReachCheckMsg->dwSize = pSendData->m_dwDataSize;
	pConnectReachCheckMsg->dwType = CTRLMSGID_CONNECTREACHCHECK;
	pConnectReachCheckMsg->dwID = pReachCheck->m_dwID;
	CopyMemory(&(pConnectReachCheckMsg->idTarget), &(pTarget->m_id),
				sizeof (TNCTRLMACHINEID));
	pConnectReachCheckMsg->dwMethod = dwMethod;
	pConnectReachCheckMsg->dwMethodDataSize = dwSourceMethodDataSize;
	pConnectReachCheckMsg->dwAddressesSize = dwAddressesToTrySize;
	CopyMemory(&(pConnectReachCheckMsg->idPreviousTarget), &idPreviousTarget,
				sizeof (TNCTRLMACHINEID));

	lpCurrent = (LPBYTE) (pConnectReachCheckMsg + 1);

	if (dwSourceMethodDataSize > 0)
	{
		CopyAndMoveDestPointer(lpCurrent, pvSourceMethodData,
								dwSourceMethodDataSize);
	} // end if (there's method data)

	if (dwAddressesToTrySize > 0)
	{
		//CopyAndMoveDestPointer(lpCurrent, pvAddressesToTry, dwAddressesToTrySize);
		CopyMemory(lpCurrent, pvAddressesToTry, dwAddressesToTrySize);
	} // end if (there's address data)
	

	hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add send item to queue!", 0);
		goto DONE;
	} // end if (failed to add item to queue)

	pSendData = NULL; // forget about it so it doesn't get freed below


	// Wait for his response (successfully got connected to the target, or failed
	// to do the same).  We need to drop the slave list lock (which is assumed to
	// be held with only one level of recursion) so the receive thread can
	// actually do something with the replies.

	DPL(8, "Waiting for %s to try connecting to %s (method %u, reach check %u)...",
		4, pSource->m_szComputerName, pTarget->m_szComputerName,
		dwMethod, pReachCheck->m_dwID);


	fDroppedSlaveLock = TRUE;
	this->m_slaves.LeaveCritSection();


#pragma BUGBUG(vanceo, "No way to user cancel")
	hr = WaitForSingleObject(pReachCheck->m_hEvent, INFINITE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Note if the connector succeeded or not.
			if (! pReachCheck->m_fResult)
			{
				DPL(0, "%s was unable to reach %s, error code is %e",
					3, pSource->m_szComputerName, pTarget->m_szComputerName,
					pReachCheck->m_hresult);
			} // end if (failed connecting)
		  break;

		default:
			// What?
			DPL(0, "Unexpected result from WaitForSingleObject!", 0);
			goto DONE;
		  break;
	} // end switch (on the wait result)

	this->m_slaves.EnterCritSection();
	fDroppedSlaveLock = FALSE;


	CloseHandle(pReachCheck->m_hEvent);
	pReachCheck->m_hEvent = NULL;



	// Tell the target that he can stop accepting communication from the source
	// (if we even told him to start in the first place).
	if (IsZero(&idPreviousTarget, sizeof (TNCTRLMACHINEID)))
	{
		pSendData = new (CTNSendData);
		if (pSendData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		pSendData->m_fGuaranteed = TRUE;
		pSendData->m_dwAddressSize = pTarget->m_commdata.dwAddressSize;
		pSendData->m_pvAddress = LocalAlloc(LPTR, pSendData->m_dwAddressSize);
		if (pSendData->m_pvAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (out of memory)
		CopyMemory(pSendData->m_pvAddress, pTarget->m_commdata.pvAddress,
				pSendData->m_dwAddressSize);

		pSendData->m_dwDataSize = sizeof (CTRLMSG_ACCEPTREACHCHECK);
		pSendData->m_pvData = LocalAlloc(LPTR, pSendData->m_dwDataSize);
		if (pSendData->m_pvData == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		pAcceptReachCheckMsg = (PCTRLMSG_ACCEPTREACHCHECK) pSendData->m_pvData;
		pAcceptReachCheckMsg->dwSize = pSendData->m_dwDataSize;
		pAcceptReachCheckMsg->dwType = CTRLMSGID_ACCEPTREACHCHECK;
		pAcceptReachCheckMsg->dwID = pReachCheck->m_dwID;
		pAcceptReachCheckMsg->dwMethod = TNRCM_CANCEL;
		pAcceptReachCheckMsg->dwMethodDataSize = 0;

		hr = this->m_pCtrlComm->m_sendqueue.AddSendData(pSendData);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add send item to queue!", 0);
			goto DONE;
		} // end if (failed to add item to queue)

		pSendData = NULL; // forget about it so it doesn't get freed below
	} // end if (didn't already contact the machine)



	(*pfResult) = pReachCheck->m_fResult;



DONE:

	if (fDroppedSlaveLock)
	{
		this->m_slaves.EnterCritSection();
		fDroppedSlaveLock = FALSE;
	} // end if (dropped slave lock)

	if (pSendData != NULL)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (allocated a send object)

	if (pReachCheck != NULL)
	{
		if (pReachCheck->m_hEvent != NULL)
		{
			CloseHandle(pReachCheck->m_hEvent);
			pReachCheck->m_hEvent = NULL;
		} // end if (have accept event)

		pReachCheckTarget = pTarget->m_reachchecktargets.GetTarget(&(pTarget->m_id));
		if (pReachCheckTarget != NULL)
		{
			// Ignore error (we don't care if it actually made it on the
			// list or not).  We had a refcount to it anyway.
			pReachCheckTarget->m_checks.RemoveFirstReference(pReachCheck);
		} // end if (target has a self list)

		pReachCheck->m_dwRefCount--;
		if (pReachCheck->m_dwRefCount == 0)
		{
			DPL(7, "Deleting reach test %x.", 1, pReachCheck);
			delete (pReachCheck);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting reach test %x, its refcount is %u.",
				2, pReachCheck, pReachCheck->m_dwRefCount);
		} // end else (shouldn't delete object)

		pReachCheck = NULL;
	} // end if (have reach test object)


	pSource->m_dwRefCount--;
	if (pSource->m_dwRefCount == 0)
	{
		DPL(7, "Deleting source slave %x.", 1, pSource);
		delete (pSource);
	} // end if (last reference)
	else
	{
		DPL(7, "Not deleting source slave %x, its refcount is %u.",
			2, pSource, pSource->m_dwRefCount);
	} // end else (shouldn't delete object)

	pTarget->m_dwRefCount--;
	if (pTarget->m_dwRefCount == 0)
	{
		DPL(7, "Deleting target slave %x.", 1, pTarget);
		delete (pTarget);
	} // end if (last reference)
	else
	{
		DPL(7, "Not deleting target slave %x, its refcount is %u.",
			2, pTarget, pTarget->m_dwRefCount);
	} // end else (shouldn't delete object)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNMaster::PerformReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"MasterReadFileCB()"
//==================================================================================
// MasterReadFileCB
//----------------------------------------------------------------------------------
//
// Description: Callback that handles a line read from a text file.
//
// Arguments:
//	char* szLine			Line that was read.
//	PFILELINEINFO pInfo		Line that was read.
//	PVOID pvContext			Pointer to context.  Cast as a READSCRIPTFILECONTEXT
//							pointer.
//	BOOL* pfStopReading		Pointer to boolean to set to TRUE to discontinue reading
//							from the file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT MasterReadFileCB(char* szLine, PFILELINEINFO pInfo, PVOID pvContext,
						BOOL* pfStopReading)
{
	HRESULT						hr = S_OK;
	PREADSCRIPTFILECONTEXT		pContext = (PREADSCRIPTFILECONTEXT) pvContext;



	//DPL(0, "Parsing: %s", 1, szLine);

	switch (pInfo->dwType)
	{
		case FILERTFT_SECTION:
			// Commit the previous test, if it exists.
			hr = pContext->pMaster->StoreReadTest(pContext);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't store read test!", 0);
				goto DONE;
			} // end if (couldn't store the last test)


			if ((TRUE) &&
				(StringStartsWith(szLine, "Exec: ", FALSE)))
			{
				int		iPos;


				iPos = StringContainsChar(szLine, '-', TRUE, 0);
				if (iPos >= 0)
				{
					//BUGBUG should treat string as read only
					szLine[iPos] = '\0';

					if (pContext->pszInstanceID != NULL)
					{
						DPL(0, "WARNING: Replacing old test instance ID \"%s\" with a later one within the same section!",
							1, pContext->pszInstanceID);
						LocalFree(pContext->pszInstanceID);
						pContext->pszInstanceID = NULL;
					} // end if (instance ID already exists)

					// Duplicate the instance ID
					pContext->pszInstanceID = (char*) LocalAlloc(LPTR, (strlen(szLine + iPos + 1) + 1));
					if (pContext->pszInstanceID == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate memory)
					strcpy(pContext->pszInstanceID, (szLine + iPos + 1));

					// Find the matching case
					pContext->pCase = pContext->pMaster->m_testtable.GetTest(szLine + strlen("Exec: "));
					if (pContext->pCase == NULL)
					{
						DPL(0, "Couldn't find test ID \"%s\" in table, skipping!",
							1, szLine + strlen("Exec: "));

						// Ignore error
						pContext->pMaster->Log(TNLST_CRITICAL,
												"Couldn't find test ID \"%s\" in table, skipping!",
												1, szLine + strlen("Exec: "));


						// Note we continue anyway, we just skip this case.
					} // end if (couldn't get test from table)
					else
						pContext->dwUniqueID = 0xFFFFFFFF; // all tests get reassigned real IDs later

					// Restore the dash.  Not really necessary, but I'm doing it anyway.
					szLine[iPos] = '-';
				} // end if (found a valid instance ID)
				else
				{
					DPL(0, "WARNING: Didn't find a test instance ID in \"%s\"!",
						1, szLine);

					pContext->pMaster->Log(TNLST_CONTROLLAYER_INFO,
											"WARNING: Didn't find a test instance ID in \"%s\"!",
											1, szLine);
				} // end else (didn't find a valid instance ID)
			} // end if (it's in a readable script section)
		  break;

		case FILERTFT_SUBSECTION:
			if (StringStartsWith(szLine, "Tester: ", FALSE))
			{
				pContext->iTesterNum = StringToInt(szLine + strlen("Tester: "));
			} // end if (this is a tester description block)
		  break;
		
		default:
			// If we're not in a section, don't bother checking anything
			if (pInfo->pszCurrentSection == NULL)
				break;

			if (StringStartsWith(pInfo->pszCurrentSection, "Info: ", FALSE))
			{
				//BUGBUG add flags to adjust this behavior
				if (
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "Session", FALSE))
				   )
				{
					if (pInfo->pszCurrentSubsection != NULL) 
					{
						//BUGBUG add flags to adjust this behavior
						if (
							((TRUE) &&
							(strcmp(pInfo->pszCurrentSubsection, "ModuleStartupData") == 0))
						   )
						{
							hr = pContext->pStartupStrings->AddString(szLine);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't add string to startup data!", 0);
								goto DONE;
							} // end if (couldn't add startup data string)
						} // end if (we're in the startup data block)
					} // end if (we're in a block)

				} // end if (this is the session info section)
			} // end if (this is an info section)

			else if (StringStartsWith(pInfo->pszCurrentSection, "Report: ", FALSE))
			{
				//BUGBUG add flags to adjust this behavior
				if (
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "Assigned", FALSE))
						||
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "Success", FALSE))
						||
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "Failure", FALSE))
						||
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "Warning", FALSE))
						||
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "CompleteSuccessfully", FALSE))
						||
					((TRUE) &&
					StringEndsWith(pInfo->pszCurrentSection, "CompleteWithFailure", FALSE))
				   )
				{
					if (pInfo->pszCurrentSubsection == NULL)
					{
						if (StringIsKey(szLine, "UniqueTestID"))
						{
							if (pContext->dwUniqueID != 0)
							{
								DPL(0, "Overwriting unique test ID %u with a later one within the same section!",
									1, pContext->dwUniqueID);
							} // end if (we already have a unique test ID for this)

							pContext->dwUniqueID = StringToDWord(StringGetKeyValue(szLine));

							if (pContext->dwUniqueID == 0)
							{
								DPL(0, "Unique test ID specified on line %u is 0!",
									1, pInfo->dwLineNumber);
							} // end if (didn't get valid ID)
						} // end if (this is a unique test ID line)
						else if (StringIsKey(szLine, "ModuleCaseID"))
						{
							if (pContext->pCase != NULL)
							{
								DPL(0, "Overwriting module test ID %s with a later one within the same section!",
									1, pContext->pCase->m_pszID);
							} // end if (we already have an app test ID for this)

							pContext->pCase = pContext->pMaster->m_testtable.GetTest(StringGetKeyValue(szLine));
							if (pContext->pCase == NULL)
							{
								DPL(0, "Couldn't find test ID \"%s\" in table, skipping!",
									1, szLine + strlen("Exec: "));

								// Ignore error
								pContext->pMaster->Log(TNLST_CRITICAL,
														"Couldn't find test ID \"%s\" in table, skipping!",
														1, szLine + strlen("Exec: "));


								// Note we continue anyway.
							} // end if (couldn't get test from table)
						} // end else if (this is an app test ID line)
						else if (StringIsKey(szLine, "TestInstanceID"))
						{
							if (pContext->pszInstanceID != NULL)
							{
								DPL(0, "Overwriting test instance ID %s with a later one within the same section!",
									1, pContext->pszInstanceID);
								LocalFree(pContext->pszInstanceID);
								pContext->pszInstanceID = NULL;
							} // end if (we already have an instance ID for this)

							pContext->pszInstanceID = (char*) LocalAlloc(LPTR, (strlen(StringGetKeyValue(szLine)) + 1));
							if (pContext->pszInstanceID == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't allocate memory)
							strcpy(pContext->pszInstanceID, StringGetKeyValue(szLine));
						} // end else if (this is an instance ID line)
						else if (StringIsKey(szLine, "FriendlyName"))
						{
							if (pContext->pszFriendlyName != NULL)
							{
								DPL(0, "WARNING: Replacing previous test friendly name \"%s\" with a later one within the same section!",
									1, pContext->pszFriendlyName);
								LocalFree(pContext->pszFriendlyName);
								pContext->pszFriendlyName = NULL;
							} // end if (test name already exists)

							pContext->pszFriendlyName = (char*) LocalAlloc(LPTR, (strlen(StringGetKeyValue(szLine)) + 1));
							if (pContext->pszFriendlyName == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't allocate memory)

							strcpy(pContext->pszFriendlyName, StringGetKeyValue(szLine));
						} // end else if (this is an app test ID line)
						else if (StringIsKey(szLine, "NumMachines"))
						{
							if (pContext->fNumMachinesSpecified)
							{
								DPL(0, "WARNING: Replacing previous number of machines (%i) with a later one within the same section!",
									1, pContext->iNumMachines);
							} // end if (test name already exists)

							pContext->iNumMachines = StringToInt(StringGetKeyValue(szLine));
							pContext->fNumMachinesSpecified = TRUE;
						} // end else if (this is an app test ID line)
						else if (StringIsKey(szLine, "Subtest"))
						{
							if (StringIsTrue(StringGetKeyValue(szLine)))
							{
								if (pContext->dwUniqueID != 0)
								{
									DPL(2, "Ignoring test %u because it is marked as a subtest.",
										1, pContext->dwUniqueID);

									// Setting this to zero makes sure that no data gets
									// committed when we come around to the next
									// StoreReadTest call.
									pContext->dwUniqueID = 0;
								} // end if (had a test going)
							} // end if (it's a subtest indicator)
						} // end else if (this is a subtest indicator line)

						//BUGBUG do we want to use this option when reading in a script?  probably not
						/*
						else if (strcmp(szLine, "EveryoneMustRun= true") == 0)
						{
							dwTestOptions |= TNTIO_EVERYONEMUSTRUN;
						} // end else if (everyone must run this test)
						*/

					} // end if (we're not in a block)
					else
					{
						if (
							((TRUE) &&
							(StringCmpNoCase(pInfo->pszCurrentSubsection, "ModuleData")))
						   )
						{
							if (pContext->pStoredData == NULL)
							{
								pContext->pStoredData = new (CTNStoredData);
								if (pContext->pStoredData == NULL)
								{
									hr = E_OUTOFMEMORY;
									goto DONE;
								} // end if (couldn't create object)

								if (StringEndsWith(pInfo->pszCurrentSection, "TestAssigned", FALSE))
									pContext->pStoredData->m_dwLocation = TNTSDL_REPORTASSIGN;
								else if (StringEndsWith(pInfo->pszCurrentSection, "Success", FALSE))
									pContext->pStoredData->m_dwLocation = TNTSDL_REPORTSUCCESS;
								else if (StringEndsWith(pInfo->pszCurrentSection, "Failure", FALSE))
									pContext->pStoredData->m_dwLocation = TNTSDL_REPORTFAILURE;
								else if (StringEndsWith(pInfo->pszCurrentSection, "Warning", FALSE))
									pContext->pStoredData->m_dwLocation = TNTSDL_REPORTWARNING;

								pContext->pStoredData->m_iTesterNum = pContext->iTesterNum;
							} // end if (we haven't created a stored data object)

							hr = pContext->pStoredData->m_strings.AddString(szLine);
							if (hr != S_OK)
							{
								DPL(0, "Couldn't add string to stored data!", 0);
								goto DONE;
							} // end if (couldn't get test from table)
						} // end else if (we're in the module data block)

					} // end else (we're in a block)

				} // end if (we should parse this report type)
			} // end else if (we're working within a report section)

			//BUGBUG add flags to change this
			else if ((TRUE) &&
					(StringStartsWith(pInfo->pszCurrentSection, "Exec: ", FALSE)))
			{
				if (pInfo->pszCurrentSubsection == NULL)
				{
					// hmm, nothing to do here yet

				} // end if (we're not in a block)
				else
				{
					//BUGBUG add flags to change this
					if (
						((TRUE) &&
						(StringCmpNoCase(pInfo->pszCurrentSubsection, "TesterList")))
					   )
					{
						if (pContext->pTesterAssignments == NULL)
						{
							pContext->pTesterAssignments = new (CLStringList);
							if (pContext->pTesterAssignments == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't create object)
						} // end if (we haven't created a stored data object)

						hr = pContext->pTesterAssignments->AddString(szLine);
						if (hr != S_OK)
						{
							DPL(0, "Couldn't add string to tester assignments!", 0);
							goto DONE;
						} // end if (couldn't add string to list)
					} // end if (we're in the tester list block)

					else if (
							 ((TRUE) &&
							 (StringCmpNoCase(pInfo->pszCurrentSubsection, "Options")))
							)
					{
						if (StringIsKey(szLine, "NumMachines"))
						{
							if (pContext->iNumMachines >= 0)
							{
								DPL(0, "WARNING: Replacing previous number of machines (%i) with a later one within the same section!",
									1, pContext->iNumMachines);
							} // end if (test name already exists)

							pContext->iNumMachines = StringToInt(StringGetKeyValue(szLine));
							pContext->fNumMachinesSpecified = TRUE;
						} // end if (this is an app test ID line)
						else if (StringIsKey(szLine, "FriendlyName"))
						{
							if (pContext->pszFriendlyName != NULL)
							{
								DPL(0, "WARNING: Replacing old test friendly name \"%s\" with a later one within the same section!",
									1, pContext->pszFriendlyName);
								LocalFree(pContext->pszFriendlyName);
								pContext->pszFriendlyName = NULL;
							} // end if (test name already exists)

							pContext->pszFriendlyName = (char*) LocalAlloc(LPTR, (strlen(StringGetKeyValue(szLine)) + 1));
							if (pContext->pszFriendlyName == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't allocate memory)

							strcpy(pContext->pszFriendlyName,
									StringGetKeyValue(szLine));
						} // end else if (it's a friendly name)
						else if (StringIsKey(szLine, "PermutationMode"))
						{
							char*	pszValue;


							if (pContext->dwPermutationMode != 0)
							{
								DPL(0, "WARNING: Replacing old permutations mode %u with a later one within the same section!",
									1, pContext->dwPermutationMode);
								//pContext->dwPermutations = 0;
							} // end if (test name already exists)

							pszValue = (char*) LocalAlloc(LPTR, strlen(StringGetKeyValue(szLine)) + 1);
							if (pszValue == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't allocate memory)

							strcpy(pszValue, StringGetKeyValue(szLine));
							_strlwr(pszValue); // need underscore for NT BUILD
							StringPopTrailingChars(pszValue, " \t", TRUE);

							if (strcmp(pszValue, "allslaves") == 0)
								pContext->dwPermutationMode |= TNTPM_ALLSLAVES;
							else if (strcmp(pszValue, "allslavesastesters0andnon0") == 0)
								pContext->dwPermutationMode |= TNTPM_ALLSLAVESASTESTERS0ANDNON0;
							else if (strcmp(pszValue, "allslavesasalltesters") == 0)
								pContext->dwPermutationMode |= TNTPM_ALLSLAVESASALLTESTERS;
							else if (strcmp(pszValue, "allpossible") == 0)
								pContext->dwPermutationMode |= TNTPM_ALLPOSSIBLE;
							else
								pContext->dwPermutationMode = TNTPM_ONCE;

							LocalFree(pszValue);
							pszValue = NULL;
						} // end else if (it's everyone must run)
					} // end else if (we're in the options block)

					else if (
							 ((TRUE) &&
							 (StringCmpNoCase(pInfo->pszCurrentSubsection, "InputData")))
							)
					{
						if (pContext->pStoredData == NULL)
						{
							pContext->pStoredData = new (CTNStoredData);
							if (pContext->pStoredData == NULL)
							{
								hr = E_OUTOFMEMORY;
								goto DONE;
							} // end if (couldn't create object)

							pContext->pStoredData->m_dwLocation = TNTSDL_SCRIPTSECTION;
						} // end if (we haven't created a stored data object)

						hr = pContext->pStoredData->m_strings.AddString(szLine);
						if (hr != S_OK)
						{
							DPL(0, "Couldn't add string to stored data!", 0);
							goto DONE;
						} // end if (couldn't add string to list)
					} // end else if (we're in the initial data block)

				} // end else (we're in a block)

			} // end else if (we're working within a script section)
		  break;
	} // end switch (on line type)


DONE:

	return (hr);
} // MasterReadFileCB
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"MasterJobThread()"
//==================================================================================
// MasterJobThread
//----------------------------------------------------------------------------------
//
// Description: This thread periodically wakes up and performs the maintenance
//				functions from the job queue.
//
// Arguments:
//	LPVOID lpvParameter		Thread startup parameter.  Cast to a pointer to a master
//							object.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
DWORD WINAPI MasterJobThread(LPVOID lpvParameter)
{
	HRESULT		hr = S_OK;
	PTNMASTER	pMaster = (PTNMASTER) lpvParameter;
	HANDLE		ahWaitObjects[3];
	DWORD		dwInterval;


	DPL(8, "Starting up.", 0);


	ahWaitObjects[0] = pMaster->m_hKillJobThreadEvent;
	ahWaitObjects[1] = pMaster->m_jobs.m_hNewJobEvent;
	ahWaitObjects[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

	
	do
	{
		hr = pMaster->ProcessJobQueue(&dwInterval);
		if (hr != S_OK)
		{
			DPL(0, "Failed processing job queue!", 0);
			goto DONE;
		} // end if (failed processing job queue)

		hr = WaitForMultipleObjects(2, ahWaitObjects, FALSE, dwInterval);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// Time for this thread to die
				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// The job list changed.  Repeat loop.
			  break;

			case WAIT_TIMEOUT:
				// Time to do some work.  Repeat loop.
			  break;

			default:
				DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
				goto DONE;
			  break;
		} // end switch (on wait result)
	} // end do (while haven't been told to die)
	while (TRUE);


DONE:

	if (hr != S_OK)
	{
		DPL(0, "Master job thread aborting!  DEBUGBREAK()-ing.  %e", 1, hr);

		// Ignoring error
		pMaster->Log(TNLST_CRITICAL,
					"Master job thread aborting!  DEBUGBREAK()-ing.  %e",
					1, hr);

		DEBUGBREAK();
	} // end if (quitting with a failure)

	DPL(8, "Exiting.", 0);

	return (hr);
} // MasterJobThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX