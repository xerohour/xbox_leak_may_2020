#ifndef _XBOX // no IPC supported
//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <mmsystem.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"

#include "leech.h"



//==================================================================================
// Structures
//==================================================================================
typedef struct tagLEECHCONNECTDATAHEADER
{
	TNCTRLMACHINEID		idSlave; // machine ID of the top slave
	DWORD				dwRealConnectDataSize; // size of real connect data

	// Anything after this is part of the real connection data.
} LEECHCONNECTDATAHEADER, * PLEECHCONNECTDATAHEADER;



//==================================================================================
// Messages
//==================================================================================
#define LEECHMSGID_LOGSTRING			101

typedef struct tagLEECHMSG_LOGSTRING
{
	DWORD		dwLogStringType; // reason/type of string to log

	// Anything after this is part of the string to log.
} LEECHMSG_LOGSTRING, * PLEECHMSG_LOGSTRING;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_REPORT				102

typedef struct tagLEECHMSG_REPORT
{
	DWORD		dwTopLevelUniqueID; // top level test ID for report
	DWORD		dwTestUniqueID; // test ID which generated this report
	HRESULT		hresult; // result code for report
	BOOL		fTestComplete; // whether it's a completion report or not
	BOOL		fSuccess; // whether it's a success report or not
	BOOL		fExecCaseExitReport; // whether it's the report generated when the ExecCase function returns
	DWORD		dwOutputDataSize; // size of output data in buffer
	DWORD		dwOutputVarsSize; // size of output variables in buffer

	// Anything after this is part of the output data and vars.
} LEECHMSG_REPORT, * PLEECHMSG_REPORT;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_WARNING				103

typedef struct tagLEECHMSG_WARNING
{
	DWORD		dwTopLevelUniqueID; // top level test ID for warning
	DWORD		dwTestUniqueID; // test ID which generated this warning
	HRESULT		hresult; // result code for warning
	DWORD		dwUserDataSize; // size of user data

	// Anything after this is part of the user data.
} LEECHMSG_WARNING, * PLEECHMSG_WARNING;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_CREATESUBTEST		104

typedef struct tagLEECHMSG_CREATESUBTEST
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwParentUniqueID; // ID of parent test
	int			iNumMachines; // number of testers in tester array

	// Anything after this is part of the case ID string and testers array blob
} LEECHMSG_CREATESUBTEST, * PLEECHMSG_CREATESUBTEST;

// The reply for this is just a DWORD with the unique ID assigned to the test.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_SYNC					105

typedef struct tagLEECHMSG_SYNC
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of syncing test
	int			iNumMachines; // number of testers in tester array
	DWORD		dwSendDataSize; // size of outgoing sync data

	// Anything after this is part of the testers array and send data blob
} LEECHMSG_SYNC, * PLEECHMSG_SYNC;

typedef struct tagLEECHMSG_SYNCREPLY
{
	HRESULT		hrResult; // result code of operation
	DWORD		dwSyncDataSize; // size of sync data, if any
	
	// Anything after this is the sync data, if any
} LEECHMSG_SYNCREPLY, * PLEECHMSG_SYNCREPLY;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_ISTESTERONMACHINE	106

typedef struct tagLEECHMSG_ISTESTERONMACHINE
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of checking test
	int			iTesterNum; // tester number to check
} LEECHMSG_ISTESTERONMACHINE, * PLEECHMSG_ISTESTERONMACHINE;

// The reply for this is a pointer to BOOL result.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_GETTESTERIP			107

typedef struct tagLEECHMSG_GETTESTERIP
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of retrieving test
	int			iTesterNum; // tester number to retrieve
	WORD		wPort; // port to retrieve IP on
} LEECHMSG_GETTESTERIP, * PLEECHMSG_GETTESTERIP;

// The reply for this is a pointer to the IP string.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_GETTESTERPHONENUM	108

typedef struct tagLEECHMSG_GETTESTERPHONENUM
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of retrieving test
	int			iTesterNum; // tester number to retrieve
} LEECHMSG_GETTESTERPHONENUM, * PLEECHMSG_GETTESTERPHONENUM;

typedef struct tagLEECHMSG_GETTESTERPHONENUMREPLY
{
	HRESULT		hrResult; // result code of operation
	
	// Anything after this is the phone number, if successful
} LEECHMSG_GETTESTERPHONENUMREPLY, * PLEECHMSG_GETTESTERPHONENUMREPLY;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_GETTESTERCOMPORT		109

typedef struct tagLEECHMSG_GETTESTERCOMPORT
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of retrieving test
	int			iTesterNum; // tester number to retrieve
} LEECHMSG_GETTESTERCOMPORT, * PLEECHMSG_GETTESTERCOMPORT;

typedef struct tagLEECHMSG_GETTESTERCOMPORTREPLY
{
	HRESULT		hrResult; // result code of operation
	DWORD		dwCOMPort; // COM port found, if successful
} LEECHMSG_GETTESTERCOMPORTREPLY, * PLEECHMSG_GETTESTERCOMPORTREPLY;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_FREERESULTS			110

typedef struct tagLEECHMSG_FREERESULTS
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of freein test
	DWORD		dwCaseIDSize; // size of ID of case to match, if any
	DWORD		dwInstanceIDSize; // size of ID of case to match, if any

	// Anything after this is part of the case and instance ID strings.
} LEECHMSG_FREERESULTS, * PLEECHMSG_FREERESULTS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_FREEOUTPUTVARS		111

typedef struct tagLEECHMSG_FREEOUTPUTVARS
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of freein test
	DWORD		dwCaseIDSize; // size of ID of case to match, if any
	DWORD		dwInstanceIDSize; // size of ID of case to match, if any
	DWORD		dwNameSize; // size of variable name to match, if any
	DWORD		dwTypeSize; // size of variable type to match, if any
	BOOL		fFreeData; // whether to free data if no more vars

	// Anything after this is part of the case ID, instance ID, name, and type
	// strings.
} LEECHMSG_FREEOUTPUTVARS, * PLEECHMSG_FREEOUTPUTVARS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_RUNTEST				112

typedef struct tagLEECHMSG_RUNTEST
{
	DWORD		dwTestUniqueID; // ID of test
	int			iNumMachines; // total number of testers running the test
	DWORD		dwInputDataSize; // size of input data for test, if any

	// Anything after this is part of the case ID string, tester array, and input
	// data blob
} LEECHMSG_RUNTEST, * PLEECHMSG_RUNTEST;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_GETMACHINEINFO		113

typedef struct tagLEECHMSG_GETMACHINEINFO
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of retrieving test
	int			iTesterNum; // tester number to retrieve
} LEECHMSG_GETMACHINEINFO, * PLEECHMSG_GETMACHINEINFO;

typedef struct tagLEECHMSG_GETMACHINEINFOREPLY
{
	HRESULT		hrResult; // result code of operation
	DWORD		dwMachineInfoSize; // size of machine info buffer
} LEECHMSG_GETMACHINEINFOREPLY, * PLEECHMSG_GETMACHINEINFOREPLY;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LEECHMSGID_LOSTTESTER			114

typedef struct tagLEECHMSG_LOSTTESTER
{
	DWORD		dwTopLevelUniqueID; // ID of top level test
	DWORD		dwTestUniqueID; // ID of test that lost tester
	int			iLostTesterNum; // tester number that was lost
} LEECHMSG_LOSTTESTER, * PLEECHMSG_LOSTTESTER;




//==================================================================================
// Globals
//==================================================================================
// This is a somewhat hacky way to ensure uniqueness, for a single process anyway.
DWORD		g_dwUniquenessValue = 0;





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::CTNLeech()"
//==================================================================================
// Overloaded function
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNLeech object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNLeech::CTNLeech(void):
	CTNIPCObject(NULL)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNLeech));
} // CTNLeech::CTNLeech
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::~CTNLeech()"
//==================================================================================
// CTNLeech destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNLeech object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNLeech::~CTNLeech(void)
{
	DPL(9, "this = %x", 1, this);

	// If we allocated the executor, delete it.
	if ((this->m_pExecutor != NULL) &&
		(this->m_pExecutor->m_pOwningLeech == this))
	{
		delete (this->m_pExecutor);
		this->m_pExecutor = NULL;
	} // end if (have local executor)
} // CTNLeech::~CTNLeech
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::PerformLeechTesting()"
//==================================================================================
// CTNLeech::PerformLeechTesting
//----------------------------------------------------------------------------------
//
// Description: Attaches this leech object to a slave previously expecting a
//				connection, and waits to perform the testing it requests.
//
// Arguments:
//	PTNPERFORMLEECHTESTINGDATA pTNpltd	Pointer to parameter block to use when
//										initializing and using the leech.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::PerformLeechTesting(PTNPERFORMLEECHTESTINGDATA pTNpltd)
{
	HRESULT						hr;
	HANDLE						ahWaitObjects[3];
	DWORD						dwNumWaitObjects = 0;
	DWORD						dwTemp;
	PLEECHCONNECTDATAHEADER		pConnectData = NULL;
	DWORD						dwConnectDataSize = 0;


	ZeroMemory(ahWaitObjects, 3 * sizeof (HANDLE));


	DPL(9, "==>(%x)", 1, pTNpltd);

	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we got passed a bad pointer)

	if (pTNpltd == NULL)
	{
		DPL(0, "Must pass a valid structure pointer!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if (pTNpltd->dwSize != sizeof (TNPERFORMLEECHTESTINGDATA))
	{
		DPL(0, "Must pass a structure of correct size!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (got passed an invalid parameter)

	if (pTNpltd->dwAPIVersion != CURRENT_TNCONTROL_API_VERSION)
	{
		DPL(0, "WARNING: Caller requesting different API version (%i != %i), problems may occur.",
			2, pTNpltd->dwAPIVersion, CURRENT_TNCONTROL_API_VERSION);
	} // end if (got passed an invalid parameter)

	if ((pTNpltd->apfnLoadTestTable == NULL) ||
		(pTNpltd->dwNumLoadTestTables < 1) ||
		(pTNpltd->apfnLoadTestTable[0] == NULL) ||
		(pTNpltd->pszAttachmentID == NULL))
	{
		DPL(0, "Must pass a valid LoadTestTable callback array and attachment ID!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (no test table function or attachment ID)

	this->m_pExecutor = new (CTNExecutorPriv);
	if (this->m_pExecutor == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't create executor)

#pragma TODO(vanceo, "Don't do this manually here, have an init function like slave does")

	this->m_pExecutor->m_pOwningLeech = this;

	CopyMemory(&(this->m_pExecutor->m_moduleID), pTNpltd->pModuleID,
				sizeof (TNMODULEID));

	if (pTNpltd->hUserCancelEvent != NULL)
	{
		if (! DuplicateHandle(GetCurrentProcess(), pTNpltd->hUserCancelEvent,
								GetCurrentProcess(), &(this->m_pExecutor->m_hUserCancelEvent),
								0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			hr = GetLastError();
			DPL(0, "Couldn't duplicate user cancel event handle (%x)!",
				2, pTNpltd->hUserCancelEvent);
			goto ERROR_EXIT;
		} // end if (couldn't duplicate handle)
	} // end if (there's a user cancel event)

	this->m_pExecutor->m_pfnInitializeTesting = pTNpltd->pfnInitializeTesting;
	this->m_pExecutor->m_pfnCleanupTesting = pTNpltd->pfnCleanupTesting;
	this->m_pExecutor->m_pfnGetTestingWindow = pTNpltd->pfnGetTestingWindow;
	this->m_pExecutor->m_fInitCOMForTestThread = pTNpltd->fInitializeCOMInTestThread;


	for(dwTemp = 0; dwTemp < pTNpltd->dwNumLoadTestTables; dwTemp++)
	{
		hr = pTNpltd->apfnLoadTestTable[dwTemp](&(this->m_pExecutor->m_testtable));
		if (hr != S_OK)
		{
			DPL(0, "Module's LoadTestTable callback %u failed!", 1, dwTemp);
			goto ERROR_EXIT;
		} // end if (module's callback failed)
	} // end for (each LoadTestTable callback)


	// We need this running before we notify the slave that we're here.  If we did
	// it after Connect(), we might get preempted and the slave could tell us to run
	// something before we got it ready, and it the message handler would fail.
	hr = this->m_pExecutor->StartTestThread();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't start test thread!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't start test thread)



	ahWaitObjects[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (ahWaitObjects[0] == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create connection event!", 0);
		goto ERROR_EXIT;
	} // end if (module's callback failed)

	dwNumWaitObjects++;


	if (pTNpltd->fStaticAttachmentID)
	{
		hr = ((PTNIPCOBJECT) this)->ConnectWithStaticID(pTNpltd->pModuleID,
														pTNpltd->pszAttachmentID,
														ahWaitObjects[0],
														pTNpltd->pvSendConnectData,
														pTNpltd->dwSendConnectDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't attach leech with static ID \"%s\"!",
				1, pTNpltd->pszAttachmentID);
			goto ERROR_EXIT;
		} // end if (couldn't attach leech)
	} // end if (using static attachment ID)
	else
	{
		hr = ((PTNIPCOBJECT) this)->Connect(FALSE,
											pTNpltd->pModuleID,
											pTNpltd->pszAttachmentID,
											ahWaitObjects[0],
											pTNpltd->pvSendConnectData,
											pTNpltd->dwSendConnectDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't attach leech with ID \"%s\"!",
				1, pTNpltd->pszAttachmentID);
			goto ERROR_EXIT;
		} // end if (couldn't attach leech)
	} // end else (not using static attachment ID)


	// Wait for the other side to connect back, if it's not there already
	DPL(6, "Waiting for slave connection...", 0);


	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT:

	// Make sure the thread has started up and copied the necessary parameters so we
	// can let the context variable go out of scope.
	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								INFINITE, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the completion signal.  Make sure we're cool.

			if (this->m_fConnected)
			{
				DPL(5, "Completion indicated we are connected.", 0);
				hr = S_OK;

				// Continue...

			} // end if (connected)
			else
			{
				DPL(0, "Completion indicated we failed to connect!", 0);
				hr = TNCWR_TIMEOUT;
				goto ERROR_EXIT;
			} // end else (not connected)
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for slave connection!", 0);

			hr = TNCWR_USERCANCEL;
			goto ERROR_EXIT;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		/*
		case WAIT_TIMEOUT:
			DPL(0, "Timed out waiting %i milliseconds for slave connection!",
				1, PERIODICSENDTHREAD_STARTUP_TIMEOUT);

			hr = TNCWR_TIMEOUT;
			goto ERROR_EXIT;
		  break;
		*/

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			goto ERROR_EXIT;
		  break;
	} // end switch (on wait result)


	// Get the machine ID from the slave.
	hr = this->GetConnectData((PVOID*) &pConnectData, &dwConnectDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get connect data!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't get connect data)

	if ((pConnectData == NULL) || (dwConnectDataSize < sizeof (LEECHCONNECTDATAHEADER)))
	{
		DPL(0, "Didn't get expected connect data from slave (%x is NULL, or size %u < %u)!",
			3, pConnectData, dwConnectDataSize, sizeof (LEECHCONNECTDATAHEADER));
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (didn't get expected connect data)

	CopyMemory(&(this->m_pExecutor->m_id), &(pConnectData->idSlave),
				sizeof (TNCTRLMACHINEID));


#pragma BUGBUG(vanceo, "This is such a huge hack, need to do this right")

	// We still have the pConnectData pointer, so we can overwrite the
	// this->m_xx versions.
	if (pConnectData->dwRealConnectDataSize == 0)
	{
		this->m_pvConnectData = NULL;
		this->m_dwConnectDataSize = 0;
	} // end if (no real connect data)
	else
	{
		this->m_pvConnectData = LocalAlloc(LPTR, pConnectData->dwRealConnectDataSize);
		if (this->m_pvConnectData == NULL)
		{
			LocalFree(pConnectData);
			pConnectData = NULL;

			hr = E_OUTOFMEMORY;
			goto ERROR_EXIT;
		} // end if (couldn't allocate memory)
		CopyMemory(this->m_pvConnectData, (pConnectData + 1), pConnectData->dwRealConnectDataSize);
		this->m_dwConnectDataSize = pConnectData->dwRealConnectDataSize;
	} // end else (real connect data)

	LocalFree(pConnectData);
	pConnectData = NULL;




	DPL(6, "Now connected to slave, his control machine ID = %u.",
		1, this->m_pExecutor->m_id.dwTime);

	// We don't need the event anymore.
	CloseHandle(ahWaitObjects[0]);
	ahWaitObjects[0] = NULL;


#pragma TODO(vanceo, "Do UI?")

	DPL(9, "<== S_OK", 0);

	return (S_OK);


ERROR_EXIT:

	this->Disconnect(); // ignore errors

	if (this->m_pExecutor != NULL)
	{
		delete (this->m_pExecutor);
		this->m_pExecutor = NULL;
	} // end if (created executor object)

	if (ahWaitObjects[0] != NULL)
	{
		CloseHandle(ahWaitObjects[0]);
		ahWaitObjects[0] = NULL;
	} // end if (have event)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNLeech::PerformLeechTesting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::GetAttachPointID()"
//==================================================================================
// CTNLeech::GetAttachPointID
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the dynamically generated attach point ID
//				string for this object.
//				Note: this is not a copy of the string.
//
// Arguments: None.
//
// Returns: A pointer to the leeches attach point ID string.
//==================================================================================
char* CTNLeech::GetAttachPointID(void)
{
	return (this->m_pszAttachPointID);
} // CTNLeech::GetAttachPointID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::GetOwnerConnectData()"
//==================================================================================
// CTNLeech::GetOwnerConnectData
//----------------------------------------------------------------------------------
//
// Description: Sets the pointers passed in to the data sent by the owner when it
//				created the leech attachment.
//				Note: this is not a copy of the data.
//
// Arguments:
//	PVOID* ppvData			Pointer to have set to point to data sent by owner when
//							it created the leech attachment.
//	DWORD* pdwDataSize		Place to store size of data sent by owner.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::GetOwnerConnectData(PVOID* ppvData, DWORD* pdwDataSize)
{


#pragma TODO(vanceo, "Parameter validation")




	return (this->GetConnectData(ppvData, pdwDataSize));
} // CTNLeech::GetOwnerConnectData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::LogToOwner()"
//==================================================================================
// CTNLeech::LogToOwner
//----------------------------------------------------------------------------------
//
// Description: Logs the given string (parsing for special tokens) one level up the
//				object heirarchy.  Eventually the top level slave will receive it
//				and print it to its window.
//
// Arguments:
//	DWORD dwLogStringType	Type of string this is.
//	char* szFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms		How many parameters are in the following variable
//							parameter list.
//	...						Variable list of parameters to parse.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::LogToOwner(DWORD dwLogStringType, char* szFormatString,
							DWORD dwNumParms, ...)
{
	HRESULT					hr;
	char*					pszBuffer = NULL;
	PVOID*					papvParms = NULL;
	va_list					currentparam;
	DWORD					dwTemp = 0;
	PLEECHMSG_LOGSTRING		pLogStringMsg = NULL;
	DWORD					dwLogStringMsgSize = 0;


	if (dwNumParms > 0)
	{
		papvParms = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
		if (papvParms == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		va_start(currentparam, dwNumParms);

		for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
		{
			papvParms[dwTemp] = va_arg(currentparam, LPVOID);
		} // end for (each parameter)

		va_end(currentparam);
	} // end if (there are parameters to check)


	// Print the items.
	TNsprintf_array(&pszBuffer, szFormatString, dwNumParms, papvParms);

	LocalFree(papvParms);
	papvParms = NULL;


	dwLogStringMsgSize = sizeof (LEECHMSG_LOGSTRING)
						+ strlen(pszBuffer) + 1;

	hr = this->PrepareSendBuffer(LEECHMSGID_LOGSTRING,
								(PVOID*) &pLogStringMsg,
								dwLogStringMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pLogStringMsg->dwLogStringType = dwLogStringType;
	strcpy((char*) (pLogStringMsg + 1), pszBuffer);



	// Send the log string message to the other side.
	hr = this->SendBuffer(pLogStringMsg, dwLogStringMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send log string message!", 0);
	} // end if (couldn't send message)


DONE:

	this->ReturnMessageBuffers(pLogStringMsg, dwLogStringMsgSize, NULL, 0);

	TNsprintf_free(&pszBuffer);

	if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (allocated array)

	return (hr);
} // CTNLeech::LogToOwner
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::ReportToOwner()"
//==================================================================================
// CTNLeech::ReportToOwner
//----------------------------------------------------------------------------------
//
// Description: Reports test results to the owner.  This will continue up the object
//				heirarchy until the top level slave sends this to the master.
//
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
//	PTNOUTPUTVARSLIST pOutputVars	Optional pointer to list of output variables
//										associated with result.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::ReportToOwner(PTNTESTINSTANCES pTest, HRESULT hresult,
								BOOL fTestComplete, BOOL fSuccess,
								BOOL fExecCaseExitReport,
								PVOID pvOutputData, DWORD dwOutputDataSize,
								PTNOUTPUTVARSLIST pOutputVars)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTopLevelTest;
	PLEECHMSG_REPORT	pReportMsg = NULL;
	DWORD				dwReportMsgSize = 0;
	DWORD				dwOutputVarsSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	dwReportMsgSize = sizeof (LEECHMSG_REPORT)
					+ dwOutputDataSize;
	if (pOutputVars != NULL)
	{
		// Ignore error, assume BUFFER_TOO_SMALL
		pOutputVars->PackIntoBuffer(NULL, &dwOutputVarsSize);
		dwReportMsgSize += dwOutputVarsSize;
	} // end if (theres a vars list)


	hr = this->PrepareSendBuffer(LEECHMSGID_REPORT,
								(PVOID*) &pReportMsg,
								dwReportMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pReportMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pReportMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pReportMsg->hresult = hresult;
	pReportMsg->fTestComplete = fTestComplete;
	pReportMsg->fSuccess = fSuccess;
	pReportMsg->fExecCaseExitReport = fExecCaseExitReport;
	pReportMsg->dwOutputDataSize = dwOutputDataSize;
	pReportMsg->dwOutputVarsSize = dwOutputVarsSize;

	if (pvOutputData != NULL)
		CopyMemory((pReportMsg + 1), pvOutputData, dwOutputDataSize);

	if (dwOutputVarsSize > 0)
	{
		hr = pOutputVars->PackIntoBuffer(((LPBYTE) (pReportMsg + 1)) + dwOutputDataSize,
											&dwOutputVarsSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't pack output vars into a buffer!", 0);
			goto DONE;
		} // end if (couldn't pack into buffer)
	} // end if (output vars to copy)

	// Send the report message to the other side.
	hr = this->SendBuffer(pReportMsg, dwReportMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send report message!", 0);
	} // end if (couldn't send message)


DONE:

	this->ReturnMessageBuffers(pReportMsg, dwReportMsgSize, NULL, 0);

	return (hr);
} // CTNLeech::ReportToOwner
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::WarnOwner()"
//==================================================================================
// CTNLeech::WarnOwner
//----------------------------------------------------------------------------------
//
// Description: Sends a warning to the owner.  This will continue up the object
//				heirarchy until the top level slave sends this to the master.
//
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test instance this warning pertains to.
//	HRESULT hresult			Warning code.
//	PVOID pvUserData		Optional pointer to data to send with warning.
//	DWORD dwUserDataSize	Size of data to send with warning.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::WarnOwner(PTNTESTINSTANCES pTest, HRESULT hresult,
							PVOID pvUserData, DWORD dwUserDataSize)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTopLevelTest;
	PLEECHMSG_WARNING	pWarningMsg = NULL;
	DWORD				dwWarningMsgSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	dwWarningMsgSize = sizeof (LEECHMSG_WARNING)
					+ dwUserDataSize;

	hr = this->PrepareSendBuffer(LEECHMSGID_WARNING,
								(PVOID*) &pWarningMsg,
								dwWarningMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pWarningMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pWarningMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pWarningMsg->hresult = hresult;
	pWarningMsg->dwUserDataSize = dwUserDataSize;

	if (pvUserData != NULL)
		CopyMemory((pWarningMsg + 1), pvUserData, dwUserDataSize);


	// Send the warning message to the other side.
	hr = this->SendBuffer(pWarningMsg, dwWarningMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send warning message!", 0);
	} // end if (couldn't send message)


DONE:

	this->ReturnMessageBuffers(pWarningMsg, dwWarningMsgSize, NULL, 0);

	return (hr);
} // CTNLeech::WarnOwner
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerCreateSubTest()"
//==================================================================================
// CTNLeech::HaveOwnerCreateSubTest
//----------------------------------------------------------------------------------
//
// Description: Requests that the owner create the given subtest (to maintain
//				parity) and assign it an ID.
//				This function blocks until the top level slave has announced this
//				test to the master and received a reply.
//				The ID will be filled out upon successful function exit.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to subtest to have owner create.
//	int iNumMachines			Number of integers in the following array.
//	PTNCTRLMACHINEID aTesters	Array of IDs indicating the testers in their
//								respective testing positions for the new test.
//
// Returns: None.
//==================================================================================
HRESULT CTNLeech::HaveOwnerCreateSubTest(PTNTESTINSTANCES pTest, int iNumMachines,
										PTNCTRLMACHINEID aTesters)
{
	HRESULT						hr;
	PTNTESTINSTANCES			pTopLevelTest = NULL;
	PTNTESTINSTANCES			pParentTest = NULL;
	DWORD						dwCreateSubTestMsgSize = 0;
	PLEECHMSG_CREATESUBTEST		pCreateSubTestMsg = NULL;
	DWORD*						pdwReplyBuffer = NULL;
	DWORD						dwReplyBufferSize = 0;
	LPBYTE						lpCurrent;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!", 1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)

	//BUGBUG what about poke mode?  will they have parent tests?
	pParentTest = (PTNTESTINSTANCES) pTest->m_pParentTest;
	if (pParentTest == NULL)
	{
		DPL(0, "No parent test for test ID %u!", 1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (no parent test)


	dwCreateSubTestMsgSize = sizeof (LEECHMSG_CREATESUBTEST)
							+ strlen(pTest->m_pCase->m_pszID) + 1
							+ (iNumMachines * sizeof (TNCTRLMACHINEID));

	DPL(7, "Requesting new test ID for case \"%s\".", 1, pTest->m_pCase->m_pszID);

	hr = this->PrepareSendBuffer(LEECHMSGID_CREATESUBTEST,
								(PVOID*) &pCreateSubTestMsg,
								dwCreateSubTestMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pCreateSubTestMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pCreateSubTestMsg->dwParentUniqueID = pParentTest->m_dwUniqueID;
	pCreateSubTestMsg->iNumMachines = iNumMachines;


	lpCurrent = (LPBYTE) (pCreateSubTestMsg + 1);

	CopyAndMoveDestPointer(lpCurrent, pTest->m_pCase->m_pszID, (strlen(pTest->m_pCase->m_pszID) + 1));

	//CopyAndMoveDestPointer(lpCurrent, aTesters, (iNumMachines * sizeof (TNCTRLMACHINEID)));
	CopyMemory(lpCurrent, aTesters, (iNumMachines * sizeof (TNCTRLMACHINEID)));


	// Send the create subtest message to the other side.
	hr = this->SendBufferAndGetReply(pCreateSubTestMsg,
									dwCreateSubTestMsgSize,
									(PVOID*) &pdwReplyBuffer,
									&dwReplyBufferSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send create subtest message!", 0);
		goto DONE;
	} // end if (couldn't send message)


	if ((pdwReplyBuffer == NULL) || (dwReplyBufferSize != sizeof (DWORD)))
	{
		DPL(0, "Didn't get valid reply data (expected DWORD, got %x, %u)!",
			2, pdwReplyBuffer, dwReplyBufferSize);
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't get valid reply data)

	pTest->m_dwUniqueID = (*pdwReplyBuffer);

	DPL(7, "Got test ID of %u.", 1, pTest->m_dwUniqueID);


DONE:

	this->ReturnMessageBuffers(pCreateSubTestMsg, dwCreateSubTestMsgSize,
								pdwReplyBuffer, dwReplyBufferSize);

	return (hr);
} // CTNLeech::HaveOwnerCreateSubTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerSync()"
//==================================================================================
// CTNLeech::HaveOwnerSync
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
// Returns: TNSR_INSYNC if successful, or TNSR_xxx error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerSync(PTNTESTINSTANCES pTest,
								char* szSyncName,
								PVOID pvSendData,
								DWORD dwSendDataSize,
								PTNSYNCDATALIST pReceiveData,
								int iNumMachines,
								int* aiTesters)
{
	HRESULT					hr;
	PTNTESTINSTANCES		pTopLevelTest;
	DWORD					dwNameSize = 0;
	PLEECHMSG_SYNC			pSyncMsg = NULL;
	DWORD					dwSyncMsgSize = 0;
	PLEECHMSG_SYNCREPLY		pSyncReplyMsg = NULL;
	DWORD					dwSyncReplyMsgSize = 0;
	LPBYTE					lpCurrent;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	dwNameSize = strlen(szSyncName) + 1;

	dwSyncMsgSize = sizeof (LEECHMSG_SYNC)
					+ dwNameSize
					+ (iNumMachines * (sizeof (int)))
					+ dwSendDataSize;

	hr = this->PrepareSendBuffer(LEECHMSGID_SYNC, (PVOID*) &pSyncMsg,
								dwSyncMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pSyncMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pSyncMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pSyncMsg->iNumMachines = iNumMachines;
	pSyncMsg->dwSendDataSize = dwSendDataSize;

	lpCurrent = (LPBYTE) (pSyncMsg + 1);

	CopyAndMoveDestPointer(lpCurrent, szSyncName, dwNameSize);
	CopyAndMoveDestPointer(lpCurrent, aiTesters, (iNumMachines * (sizeof (int))));

	if (pvSendData != NULL)
	{
		//CopyAndMoveDestPointer(lpCurrent, pvSendData, dwSendDataSize);
		CopyMemory(lpCurrent, pvSendData, dwSendDataSize);
	} // end if (there's send data)


	// Send the sync message to the other side.
	hr = this->SendBufferAndGetReply(pSyncMsg,
									dwSyncMsgSize,
									(PVOID*) &pSyncReplyMsg,
									&dwSyncReplyMsgSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send sync message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	if (dwSyncReplyMsgSize < sizeof (LEECHMSG_SYNCREPLY))
	{
		DPL(0, "Reply isn't large enough to be a SyncReply message!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (not valid reply)

	// Try to unpack the results, if any.
	if (pSyncReplyMsg->dwSyncDataSize > 0)
	{
		hr = pReceiveData->UnpackFromBuffer((pSyncReplyMsg + 1),
											pSyncReplyMsg->dwSyncDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack received sync data buffer!", 0);
			goto DONE;
		} // end if (couldn't unpack buffer)
	} // end if (there's sync reply data)

	// Now return the result that was generated by the call.
	hr = pSyncReplyMsg->hrResult;


DONE:

	this->ReturnMessageBuffers(pSyncMsg, dwSyncMsgSize,
								pSyncReplyMsg, dwSyncReplyMsgSize);

	return (hr);
} // CTNLeech::HaveOwnerSync
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerCheckIfTesterOnMachine()"
//==================================================================================
// CTNLeech::HaveOwnerCheckIfTesterOnMachine
//----------------------------------------------------------------------------------
//
// Description: Checks if the specified tester is located on the same machine.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test requesting info.
//	int iTesterNum				Tester number for which to retrieve IP address.
//	WORD wPort					IP port to verify reachability.
//	char* szIPString			String to store results in (must be 16 chars).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
BOOL CTNLeech::HaveOwnerCheckIfTesterOnMachine(PTNTESTINSTANCES pTest,
												int iTesterNum)
{
	BOOL							fResult = FALSE;
	HRESULT							hr;
	PTNTESTINSTANCES				pTopLevelTest;
	PLEECHMSG_ISTESTERONMACHINE		pIsTesterOnMachineMsg = NULL;
	BOOL*							pfBool = NULL;
	DWORD							dwBoolSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		goto DONE;
	} // end if (couldn't get top level test)


	hr = this->PrepareSendBuffer(LEECHMSGID_ISTESTERONMACHINE,
								(PVOID*) &pIsTesterOnMachineMsg,
								sizeof (LEECHMSG_ISTESTERONMACHINE));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!  %e", 1, hr);
		goto DONE;
	} // end if (couldn't allocate memory)

	pIsTesterOnMachineMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pIsTesterOnMachineMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pIsTesterOnMachineMsg->iTesterNum = iTesterNum;

	// Send the GetTesterIP message to the other side.
	hr = this->SendBufferAndGetReply(pIsTesterOnMachineMsg,
									sizeof (LEECHMSG_ISTESTERONMACHINE),
									(PVOID*) &pfBool,
									&dwBoolSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send IsTesterOnMachine message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	// Make sure it's a valid reply.
	if ((pfBool == NULL) || (dwBoolSize != sizeof (BOOL)))
	{
		DPL(0, "Didn't get valid reply data (%x is NULL, %u != %u)!",
			3, pfBool, dwBoolSize, sizeof (BOOL));
		goto DONE;
	} // end if (didn't get valid reply data)
	
	fResult = (*pfBool);


DONE:

	this->ReturnMessageBuffers(pIsTesterOnMachineMsg,
								sizeof (LEECHMSG_ISTESTERONMACHINE),
								pfBool,
								dwBoolSize);

	return (fResult);
} // CTNLeech::HaveOwnerCheckIfTesterOnMachine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerGetTesterIP()"
//==================================================================================
// CTNLeech::HaveOwnerGetTesterIP
//----------------------------------------------------------------------------------
//
// Description: Retrieves the IP address of the specified tester and places it in
//				the passed in string buffer.  The buffer must be at least 15
//				characters + NULL termination long.
//				The reachability test must already have been run (and passed) for
//				the specified machine.  See the master object for more details.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test requesting info.
//	int iTesterNum				Tester number for which to retrieve IP address.
//	WORD wPort					IP port to verify reachability.
//	char* szIPString			String to store results in (must be 16 chars).
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerGetTesterIP(PTNTESTINSTANCES pTest,
									int iTesterNum, WORD wPort,
									char* szIPString)
{
	HRESULT					hr;
	PTNTESTINSTANCES		pTopLevelTest;
	PLEECHMSG_GETTESTERIP	pGetTesterIPMsg = NULL;
	char*					pszIPString = NULL;
	DWORD					dwIPStringSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	hr = this->PrepareSendBuffer(LEECHMSGID_GETTESTERIP, (PVOID*) &pGetTesterIPMsg,
								sizeof (LEECHMSG_GETTESTERIP));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pGetTesterIPMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pGetTesterIPMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pGetTesterIPMsg->iTesterNum = iTesterNum;
	pGetTesterIPMsg->wPort = wPort;

	// Send the GetTesterIP message to the other side.
	hr = this->SendBufferAndGetReply(pGetTesterIPMsg,
									sizeof (LEECHMSG_GETTESTERIP),
									(PVOID*) &pszIPString,
									&dwIPStringSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send GetTesterIP message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	// Make sure it's a valid reply.
	// 8 = 0.0.0.0 + NULL termination, 16 = 255.255.255.255 + NULL termination.
	if ((pszIPString == NULL) || (dwIPStringSize < 8) || (dwIPStringSize > 16))
	{
		DPL(0, "Didn't get valid reply data (%x is NULL, %u <8 or >16)!",
			2, pszIPString, dwIPStringSize);
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't get valid reply data)
	
	CopyMemory(szIPString, pszIPString, dwIPStringSize);


DONE:

	this->ReturnMessageBuffers(pGetTesterIPMsg, sizeof (LEECHMSG_GETTESTERIP),
								pszIPString, dwIPStringSize);

	return (hr);
} // CTNLeech::HaveOwnerGetTesterIP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerGetTesterPhoneNum()"
//==================================================================================
// CTNLeech::HaveOwnerGetTesterPhoneNum
//----------------------------------------------------------------------------------
//
// Description: Requests the phone number for the specified tester, and returns
//				the result in the string buffer given.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test requesting info.
//	int iTesterNum				Tester number for which to retrieve COM port.
//	char* szPhoneNumber			Place to store result.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerGetTesterPhoneNum(PTNTESTINSTANCES pTest,
											int iTesterNum, char* szPhoneNumber)
{
	HRESULT								hr;
	PTNTESTINSTANCES					pTopLevelTest;
	PLEECHMSG_GETTESTERPHONENUM			pGetTesterPhoneNumMsg = NULL;
	PLEECHMSG_GETTESTERPHONENUMREPLY	pReplyMsg = NULL;
	DWORD								dwReplyMsgSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	hr = this->PrepareSendBuffer(LEECHMSGID_GETTESTERPHONENUM,
								(PVOID*) &pGetTesterPhoneNumMsg,
								sizeof (LEECHMSG_GETTESTERPHONENUM));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pGetTesterPhoneNumMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pGetTesterPhoneNumMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pGetTesterPhoneNumMsg->iTesterNum = iTesterNum;

	// Send the GetTesterPhoneNum message to the other side.
	hr = this->SendBufferAndGetReply(pGetTesterPhoneNumMsg,
									sizeof (LEECHMSG_GETTESTERPHONENUM),
									(PVOID*) &pReplyMsg,
									&dwReplyMsgSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send GetTesterCOMPort message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	// Make sure it's a valid reply.
	if ((pReplyMsg == NULL) || (dwReplyMsgSize < sizeof (LEECHMSG_GETTESTERPHONENUMREPLY)))
	{
		DPL(0, "Didn't get valid reply data (%x is NULL, %u < %u)!",
			3, pReplyMsg, dwReplyMsgSize,
			sizeof (LEECHMSG_GETTESTERPHONENUMREPLY));
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't get valid reply data)
	
	hr = pReplyMsg->hrResult;
	if (hr == S_OK)
		strcpy(szPhoneNumber, (char*) (pReplyMsg + 1));


DONE:

	this->ReturnMessageBuffers(pGetTesterPhoneNumMsg,
								sizeof (LEECHMSG_GETTESTERPHONENUM),
								pReplyMsg, dwReplyMsgSize);

	return (hr);
} // CTNLeech::HaveOwnerGetTesterPhoneNum
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerGetTesterCOMPort()"
//==================================================================================
// CTNLeech::HaveOwnerGetTesterCOMPort
//----------------------------------------------------------------------------------
//
// Description: Requests the COM port connected to the specified tester, and returns
//				the result in the DWORD pointer given.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test requesting info.
//	int iTesterNum			Tester number for which to retrieve COM port.
//	DWORD* pdwCOMPort		Place to store result.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerGetTesterCOMPort(PTNTESTINSTANCES pTest,
											int iTesterNum, DWORD* pdwCOMPort)
{
	HRESULT								hr;
	PTNTESTINSTANCES					pTopLevelTest;
	PLEECHMSG_GETTESTERCOMPORT			pGetTesterCOMPortMsg = NULL;
	PLEECHMSG_GETTESTERCOMPORTREPLY		pReplyMsg = NULL;
	DWORD								dwReplyMsgSize = 0;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	hr = this->PrepareSendBuffer(LEECHMSGID_GETTESTERCOMPORT,
								(PVOID*) &pGetTesterCOMPortMsg,
								sizeof (LEECHMSG_GETTESTERCOMPORT));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pGetTesterCOMPortMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pGetTesterCOMPortMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pGetTesterCOMPortMsg->iTesterNum = iTesterNum;

	// Send the GetTesterCOMPort message to the other side.
	hr = this->SendBufferAndGetReply(pGetTesterCOMPortMsg,
									sizeof (LEECHMSG_GETTESTERCOMPORT),
									(PVOID*) &pReplyMsg,
									&dwReplyMsgSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send GetTesterCOMPort message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	// Make sure it's a valid reply.
	if ((pReplyMsg == NULL) || (dwReplyMsgSize != sizeof (LEECHMSG_GETTESTERCOMPORTREPLY)))
	{
		DPL(0, "Didn't get valid reply data (%x is NULL, %u != %u)!",
			3, pReplyMsg, dwReplyMsgSize,
			sizeof (LEECHMSG_GETTESTERCOMPORTREPLY));
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't get valid reply data)
	
	hr = pReplyMsg->hrResult;
	(*pdwCOMPort) = pReplyMsg->dwCOMPort;


DONE:

	this->ReturnMessageBuffers(pGetTesterCOMPortMsg,
								sizeof (LEECHMSG_GETTESTERCOMPORT),
								pReplyMsg, dwReplyMsgSize);

	return (hr);
} // CTNLeech::HaveOwnerGetTesterCOMPort
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerFreeResults()"
//==================================================================================
// CTNLeech::HaveOwnerFreeResults
//----------------------------------------------------------------------------------
//
// Description: Has the owner free results matching the given criteria.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test freeing results.
//	char* pszCaseID			Case ID to match, or NULL for all.
//	char* pszInstanceID		Instance ID to match, or NULL for all.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerFreeResults(PTNTESTINSTANCES pTest, char* pszCaseID,
										char* pszInstanceID)
{
	HRESULT					hr;
	PTNTESTINSTANCES		pTopLevelTest;
	PLEECHMSG_FREERESULTS	pFreeResultsMsg = NULL;
	DWORD					dwMsgSize = 0;
	LPBYTE					lpCurrent;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	dwMsgSize = sizeof (LEECHMSG_FREERESULTS);
	if (pszCaseID != NULL)
		dwMsgSize += strlen(pszCaseID) + 1;
	if (pszInstanceID != NULL)
		dwMsgSize += strlen(pszInstanceID) + 1;

	hr = this->PrepareSendBuffer(LEECHMSGID_FREERESULTS,
								(PVOID*) &pFreeResultsMsg,
								dwMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)


	pFreeResultsMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pFreeResultsMsg->dwTestUniqueID = pTest->m_dwUniqueID;

	lpCurrent = (LPBYTE) (pFreeResultsMsg + 1);

	if (pszCaseID != NULL)
	{
		pFreeResultsMsg->dwCaseIDSize = strlen(pszCaseID) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszCaseID,
								pFreeResultsMsg->dwCaseIDSize);
	} // end if (there's a case ID)

	if (pszInstanceID != NULL)
	{
		pFreeResultsMsg->dwInstanceIDSize = strlen(pszInstanceID) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszInstanceID,
								pFreeResultsMsg->dwInstanceIDSize);
	} // end if (there's an instance ID)

	// Send the FreeResults message to the other side.
	hr = this->SendBuffer(pFreeResultsMsg, dwMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send FreeResults message!", 0);
		goto DONE;
	} // end if (couldn't send message)


DONE:

	this->ReturnMessageBuffers(pFreeResultsMsg, dwMsgSize, NULL, NULL);

	return (hr);
} // CTNLeech::HaveOwnerFreeResults
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerFreeOutputVars()"
//==================================================================================
// CTNLeech::HaveOwnerFreeOutputVars
//----------------------------------------------------------------------------------
//
// Description: Has the owner free output vars matching the given criteria.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test freeing output vars.
//	char* pszCaseID			Case ID to match, or NULL for all.
//	char* pszInstanceID		Instance ID to match, or NULL for all.
//	char* pszName			Output variable name to match, or NULL for all.
//	char* pszType			Output variable type to match, or NULL for all.
//	BOOL fFreeData			Whether to free the output data, if no vars.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerFreeOutputVars(PTNTESTINSTANCES pTest, char* pszCaseID,
										char* pszInstanceID, char* pszName,
										char* pszType, BOOL fFreeData)
{
	HRESULT						hr;
	PTNTESTINSTANCES			pTopLevelTest;
	PLEECHMSG_FREEOUTPUTVARS	pFreeOutputVarsMsg = NULL;
	DWORD						dwMsgSize = 0;
	LPBYTE						lpCurrent;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	dwMsgSize = sizeof (LEECHMSG_FREEOUTPUTVARS);
	if (pszCaseID != NULL)
		dwMsgSize += strlen(pszCaseID) + 1;
	if (pszInstanceID != NULL)
		dwMsgSize += strlen(pszInstanceID) + 1;
	if (pszName != NULL)
		dwMsgSize += strlen(pszName) + 1;
	if (pszType != NULL)
		dwMsgSize += strlen(pszType) + 1;

	hr = this->PrepareSendBuffer(LEECHMSGID_FREEOUTPUTVARS,
								(PVOID*) &pFreeOutputVarsMsg,
								dwMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pFreeOutputVarsMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pFreeOutputVarsMsg->dwTestUniqueID = pTest->m_dwUniqueID;

	lpCurrent = (LPBYTE) (pFreeOutputVarsMsg + 1);

	if (pszCaseID != NULL)
	{
		pFreeOutputVarsMsg->dwCaseIDSize = strlen(pszCaseID) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszCaseID,
								pFreeOutputVarsMsg->dwCaseIDSize);
	} // end if (there's a case ID)

	if (pszInstanceID != NULL)
	{
		pFreeOutputVarsMsg->dwInstanceIDSize = strlen(pszInstanceID) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszInstanceID,
								pFreeOutputVarsMsg->dwInstanceIDSize);
	} // end if (there's an instance ID)

	if (pszName != NULL)
	{
		pFreeOutputVarsMsg->dwNameSize = strlen(pszName) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszName,
								pFreeOutputVarsMsg->dwNameSize);
	} // end if (there's a name)

	if (pszType != NULL)
	{
		pFreeOutputVarsMsg->dwTypeSize = strlen(pszType) + 1;
		CopyAndMoveDestPointer(lpCurrent, pszType,
								pFreeOutputVarsMsg->dwTypeSize);
	} // end if (there's a type)

	pFreeOutputVarsMsg->fFreeData = fFreeData;

	// Send the FreeOutputVars message to the other side.
	hr = this->SendBuffer(pFreeOutputVarsMsg, dwMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send FreeOutputVars message!", 0);
		goto DONE;
	} // end if (couldn't send message)


DONE:

	this->ReturnMessageBuffers(pFreeOutputVarsMsg, dwMsgSize, NULL, NULL);

	return (hr);
} // CTNLeech::HaveOwnerFreeOutputVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveOwnerGetTestersMachineInfo()"
//==================================================================================
// CTNLeech::HaveOwnerGetTestersMachineInfo
//----------------------------------------------------------------------------------
//
// Description: Requests the COM port connected to the specified tester, and returns
//				the result in the DWORD pointer given.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test requesting info.
//	int iTesterNum			Tester number for which to retrieve COM port.
//	DWORD* pdwCOMPort		Place to store result.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveOwnerGetTestersMachineInfo(PTNTESTINSTANCES pTest,
												int iTesterNum,
												PTNMACHINEINFO* ppInfo)
{
	HRESULT							hr;
	PTNTESTINSTANCES				pTopLevelTest;
	PLEECHMSG_GETMACHINEINFO		pGetMachineInfoMsg = NULL;
	PLEECHMSG_GETMACHINEINFOREPLY	pReplyMsg = NULL;
	DWORD							dwReplyMsgSize = 0;
	PTNOTHERMACHINEINFO				pInfo = NULL;


	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (we got passed a bad pointer)


	pTopLevelTest = (PTNTESTINSTANCES) pTest->GetTopLevelTest();
	if (pTopLevelTest == NULL)
	{
		DPL(0, "Couldn't get top level test for test ID %u!",
			1, pTest->m_dwUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't get top level test)


	hr = this->PrepareSendBuffer(LEECHMSGID_GETMACHINEINFO,
								(PVOID*) &pGetMachineInfoMsg,
								sizeof (LEECHMSG_GETMACHINEINFO));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto DONE;
	} // end if (couldn't allocate memory)

	pGetMachineInfoMsg->dwTopLevelUniqueID = pTopLevelTest->m_dwUniqueID;
	pGetMachineInfoMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pGetMachineInfoMsg->iTesterNum = iTesterNum;

	// Send the GetMachineInfo message to the other side.
	hr = this->SendBufferAndGetReply(pGetMachineInfoMsg,
									sizeof (LEECHMSG_GETMACHINEINFO),
									(PVOID*) &pReplyMsg,
									&dwReplyMsgSize,
									INFINITE);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send GetMachineInfo message and get reply!", 0);
		goto DONE;
	} // end if (couldn't send message)

	// Make sure it's a valid reply.
	if ((pReplyMsg == NULL) || (dwReplyMsgSize < sizeof (LEECHMSG_GETMACHINEINFOREPLY)))
	{
		DPL(0, "Didn't get valid reply data (%x is NULL, %u < %u)!",
			3, pReplyMsg, dwReplyMsgSize,
			sizeof (LEECHMSG_GETMACHINEINFOREPLY));
		hr = E_FAIL;
		goto DONE;
	} // end if (didn't get valid reply data)
	
	hr = pReplyMsg->hrResult;
	
	if (hr == S_OK)
	{
		pInfo = new (CTNOtherMachineInfo)(&(pTest->m_paTesterSlots[iTesterNum].id));
		if (pInfo == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		hr = pInfo->UnpackFromBuffer((pReplyMsg + 1), pReplyMsg->dwMachineInfoSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack machine info!", 0);

			delete (pInfo);
			pInfo = NULL;

			goto DONE;
		} // end if (couldn't unpack machine info)

		// One reference for our caller.
		pInfo->m_dwRefCount++;

		(*ppInfo) = pInfo;
		pInfo = NULL;
	} // end if (succeeded)


DONE:

	this->ReturnMessageBuffers(pGetMachineInfoMsg,
								sizeof (LEECHMSG_GETMACHINEINFO),
								pReplyMsg, dwReplyMsgSize);

	return (hr);
} // CTNLeech::HaveOwnerGetTestersMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HaveLeechRunTest()"
//==================================================================================
// CTNLeech::HaveLeechRunTest
//----------------------------------------------------------------------------------
//
// Description: Requests that the leech on the other side run the given test with
//				the specified input data.  This will block until the leech returns
//				from its ExecCase function.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to run.
//	PVOID pvInputData		Pointer to input data for test.
//	DWORD dwInputDataSize	Size of input data.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::HaveLeechRunTest(PTNTESTINSTANCES pTest, PVOID pvInputData,
								DWORD dwInputDataSize)
{
	HRESULT				hr;
	DWORD				dwCaseIDSize = 0;
	PLEECHMSG_RUNTEST	pRunTestMsg = NULL;
	DWORD				dwRunTestMsgSize = 0;
	LPBYTE				lpCurrent;
	int					i;
	HANDLE				ahWaitObjects[3];
	DWORD				dwNumWaitObjects = 0;


	DPL(9, "==>(%x  [%u], %x, %u)",
		4, pTest, pTest->m_dwUniqueID, pvInputData, dwInputDataSize);

	pTest->m_dwRefCount++; // we're using it

	if (this == NULL)
	{
		DPL(0, "Leech object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (we got passed a bad pointer)

	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Can't use this function with standalone leech!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto ERROR_EXIT;
	} // end if (not owned by an executor object)


	dwCaseIDSize = strlen(pTest->m_pCase->m_pszID) + 1;

	dwRunTestMsgSize = sizeof (LEECHMSG_RUNTEST)
					+ dwCaseIDSize
					+ (pTest->m_iNumMachines * sizeof (TNCTRLMACHINEID))
					+ dwInputDataSize;

	hr = this->PrepareSendBuffer(LEECHMSGID_RUNTEST, (PVOID*) &pRunTestMsg,
								dwRunTestMsgSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't allocate send buffer!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	pRunTestMsg->dwTestUniqueID = pTest->m_dwUniqueID;
	pRunTestMsg->iNumMachines = pTest->m_iNumMachines;
	pRunTestMsg->dwInputDataSize = dwInputDataSize;

	lpCurrent = (LPBYTE) (pRunTestMsg + 1);

	CopyAndMoveDestPointer(lpCurrent, pTest->m_pCase->m_pszID, dwCaseIDSize);

	for(i = 0; i < pTest->m_iNumMachines; i++)
	{
		CopyAndMoveDestPointer(lpCurrent, &(pTest->m_paTesterSlots[i].id),
								sizeof (TNCTRLMACHINEID));
	} // end for (each tester)

	if (dwInputDataSize > 0)
	{
		//CopyAndMoveDestPointer(lpCurrent, pvInputData, dwInputDataSize);
		CopyMemory(lpCurrent, pvInputData, dwInputDataSize);
	} // end if (there's an instance ID)



	// Create an event so we can block until the leech returns from ExecCase.
	pTest->m_hExecCaseExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (pTest->m_hExecCaseExitEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create exec case exit event!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't create event)



	// Send the RunTest message to the other side.
	hr = this->SendBuffer(pRunTestMsg, dwRunTestMsgSize, IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send RunTest message!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't send message)

	this->ReturnMessageBuffers(pRunTestMsg, dwRunTestMsgSize, NULL, 0);



	// Now wait for the leech to return from ExecCase.

	ahWaitObjects[dwNumWaitObjects++] = pTest->m_hExecCaseExitEvent;

	if (this->m_pExecutor->m_hUserCancelEvent != NULL)
		ahWaitObjects[dwNumWaitObjects++] = this->m_pExecutor->m_hUserCancelEvent;

	ahWaitObjects[dwNumWaitObjects] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array


REWAIT:

	hr = WaitForMultipleObjectsEx(dwNumWaitObjects, ahWaitObjects, FALSE,
								INFINITE, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the ready signal, we're cool.

			hr = S_OK;
		  break;

		case WAIT_OBJECT_0 + 1:
			// Got user cancel event

			DPL(0, "User cancelled waiting for leech to run test!", 0);

			hr = TNWR_USERCANCEL;
		  break;

		case WAIT_IO_COMPLETION:
			DPL(1, "I/O Completion.", 0);
			goto REWAIT;
		  break;

		/*
		case WAIT_TIMEOUT:
			DPL(0, "Timed out waiting %i milliseconds for leech to run test!",
				1, PERIODICSENDTHREAD_STARTUP_TIMEOUT);

			hr = TNWR_TIMEOUT;
		  break;
		*/

		default:
			// What?
			DPL(0, "Received unexpected return from WaitForMultipleObjectsEx!", 0);

			if (hr == S_OK)
				hr = E_FAIL;
		  break;
	} // end switch (on wait result)


	CloseHandle(pTest->m_hExecCaseExitEvent);
	pTest->m_hExecCaseExitEvent = NULL;


	pTest->m_dwRefCount--;
	if (pTest->m_dwRefCount == 0)
	{
		DPL(7, "Deleting test %x.", 1, pTest);
		delete (pTest);
		pTest = NULL;
	} // end if (can delete object)
	else
	{
		DPL(7, "Not deleting test %x, its refcount = %u.",
			2, pTest, pTest->m_dwRefCount);
	} // end else (can't delete object)

	DPL(9, "<== %e", 1, hr);

	return (hr);


ERROR_EXIT:

	if (pTest->m_hExecCaseExitEvent != NULL)
	{
		CloseHandle(pTest->m_hExecCaseExitEvent);
		pTest->m_hExecCaseExitEvent = NULL;
	} // end if (have event)

	// If these are NULL, then it's still cool.
	this->ReturnMessageBuffers(pRunTestMsg, dwRunTestMsgSize, NULL, 0);

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNLeech::HaveLeechRunTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::SendLostTesterMsgToOtherSide()"
//==================================================================================
// CTNLeech::SendLostTesterMsgToOtherSide
//----------------------------------------------------------------------------------
//
// Description: Sends a lost tester message to the other side.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test under which a tester was lost.
//	DWORD dwTestUniqueID		ID of test with problem.
//	int iLostTesterNum			Tester number who was lost.
//
// Returns: S_OK if successful or error code otherwise.
//==================================================================================
HRESULT CTNLeech::SendLostTesterMsgToOtherSide(DWORD dwTopLevelUniqueID,
											DWORD dwTestUniqueID,
											int iLostTesterNum)
{
	HRESULT					hr;
	LEECHMSG_LOSTTESTER		losttestermsg;


	ZeroMemory(&losttestermsg, sizeof (LEECHMSG_LOSTTESTER));
	losttestermsg.dwTopLevelUniqueID = dwTopLevelUniqueID;
	losttestermsg.dwTestUniqueID = dwTestUniqueID;
	losttestermsg.iLostTesterNum = iLostTesterNum;

	hr = this->SendMessage(LEECHMSGID_LOSTTESTER,
							&losttestermsg, sizeof (LEECHMSG_LOSTTESTER),
							IPCOBJ_ACK_WAIT_TIMEOUT);
	if (hr != S_OK)
	{
		DPL(0, "Failed sending LostTester message!", 0);
	} // end if (failed sending message)

	return (hr);
} // CTNLeech::SendLostTesterMsgToOtherSide
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleMessage()"
//==================================================================================
// CTNLeech::HandleMessage
//----------------------------------------------------------------------------------
//
// Description: Handles a non-standard message from the other end of the connection.
//
// Arguments:
//	DWORD dwCommand				ID of command.
//	PVOID pvSentData			Pointer to sent command data, if any.
//	DWORD dwSentDataSize		Size of sent data, if any.
//	PVOID pvReplyData			Pointer to room for reply data.
//	DWORD* pdwReplyDataSize		Pointer to size of reply data buffer.  Should be set
//								to the size used (including 0, if not used) on exit.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleMessage(DWORD dwCommand,
								PVOID pvSentData, DWORD dwSentDataSize,
								PVOID pvReplyData, DWORD* pdwReplyDataSize)
{
	HRESULT		hr;


	switch (dwCommand)
	{
		case LEECHMSGID_LOGSTRING:
			if (dwSentDataSize <= sizeof (LEECHMSG_LOGSTRING))
			{
				DPL(0, "Message too small to be valid LogString message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_LOGSTRING));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleLogStringMsg(((PLEECHMSG_LOGSTRING) pvSentData)->dwLogStringType,
												(char*) (((PLEECHMSG_LOGSTRING) pvSentData) + 1));
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_REPORT:
			if (dwSentDataSize < sizeof (LEECHMSG_REPORT))
			{
				DPL(0, "Message too small to be valid Report message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_REPORT));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleReportMsg(((PLEECHMSG_REPORT) pvSentData)->dwTopLevelUniqueID,
											((PLEECHMSG_REPORT) pvSentData)->dwTestUniqueID,
											((PLEECHMSG_REPORT) pvSentData)->hresult,
											((PLEECHMSG_REPORT) pvSentData)->fTestComplete,
											((PLEECHMSG_REPORT) pvSentData)->fSuccess,
											((PLEECHMSG_REPORT) pvSentData)->fExecCaseExitReport,
											((PLEECHMSG_REPORT) pvSentData)->dwOutputDataSize,
											((PLEECHMSG_REPORT) pvSentData)->dwOutputVarsSize,
											(((PLEECHMSG_REPORT) pvSentData) + 1));
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_WARNING:
			if (dwSentDataSize < sizeof (LEECHMSG_WARNING))
			{
				DPL(0, "Message too small to be valid Warning message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_WARNING));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleWarningMsg(((PLEECHMSG_WARNING) pvSentData)->dwTopLevelUniqueID,
											((PLEECHMSG_WARNING) pvSentData)->dwTestUniqueID,
											((PLEECHMSG_WARNING) pvSentData)->hresult,
											((((PLEECHMSG_WARNING) pvSentData)->dwUserDataSize > 0) ? (((PLEECHMSG_WARNING) pvSentData) + 1) : NULL),
											((PLEECHMSG_WARNING) pvSentData)->dwUserDataSize);
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_CREATESUBTEST:
			if (dwSentDataSize < sizeof (LEECHMSG_CREATESUBTEST))
			{
				DPL(0, "Message too small to be valid CreateSubTest message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_CREATESUBTEST));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleCreateSubTestMsg(((PLEECHMSG_CREATESUBTEST) pvSentData)->dwTopLevelUniqueID,
												((PLEECHMSG_CREATESUBTEST) pvSentData)->dwParentUniqueID,
												((PLEECHMSG_CREATESUBTEST) pvSentData)->iNumMachines,													
												(((PLEECHMSG_CREATESUBTEST) pvSentData) + 1),
												(DWORD*) pvReplyData);

				(*pdwReplyDataSize) = sizeof (DWORD);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_SYNC:
			if (dwSentDataSize < sizeof (LEECHMSG_SYNC))
			{
				DPL(0, "Message too small to be valid Sync message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_SYNC));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleSyncMsg(((PLEECHMSG_SYNC) pvSentData)->dwTopLevelUniqueID,
										((PLEECHMSG_SYNC) pvSentData)->dwTestUniqueID,
										((PLEECHMSG_SYNC) pvSentData)->iNumMachines,													
										((PLEECHMSG_SYNC) pvSentData)->dwSendDataSize,													
										(((PLEECHMSG_SYNC) pvSentData) + 1),
										pvReplyData,
										pdwReplyDataSize);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_ISTESTERONMACHINE:
			if (dwSentDataSize < sizeof (LEECHMSG_ISTESTERONMACHINE))
			{
				DPL(0, "Message too small to be valid IsTesterOnMachine message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_ISTESTERONMACHINE));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleIsTesterOnMachineMsg(((PLEECHMSG_ISTESTERONMACHINE) pvSentData)->dwTopLevelUniqueID,
													((PLEECHMSG_ISTESTERONMACHINE) pvSentData)->dwTestUniqueID,
													((PLEECHMSG_ISTESTERONMACHINE) pvSentData)->iTesterNum,													
													(BOOL*) pvReplyData);
				(*pdwReplyDataSize) = sizeof (BOOL);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_GETTESTERIP:
			if (dwSentDataSize < sizeof (LEECHMSG_GETTESTERIP))
			{
				DPL(0, "Message too small to be valid GetTesterIP message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_GETTESTERIP));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleGetTesterIPMsg(((PLEECHMSG_GETTESTERIP) pvSentData)->dwTopLevelUniqueID,
												((PLEECHMSG_GETTESTERIP) pvSentData)->dwTestUniqueID,
												((PLEECHMSG_GETTESTERIP) pvSentData)->iTesterNum,													
												((PLEECHMSG_GETTESTERIP) pvSentData)->wPort,													
												(char*) pvReplyData,
												pdwReplyDataSize);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_GETTESTERPHONENUM:
			if (dwSentDataSize < sizeof (LEECHMSG_GETTESTERPHONENUM))
			{
				DPL(0, "Message too small to be valid GetTesterPhoneNum message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_GETTESTERPHONENUM));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleGetTesterPhoneNumMsg(((PLEECHMSG_GETTESTERPHONENUM) pvSentData)->dwTopLevelUniqueID,
													((PLEECHMSG_GETTESTERPHONENUM) pvSentData)->dwTestUniqueID,
													((PLEECHMSG_GETTESTERPHONENUM) pvSentData)->iTesterNum,													
													(char*) pvReplyData,
													pdwReplyDataSize);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_GETTESTERCOMPORT:
			if (dwSentDataSize < sizeof (LEECHMSG_GETTESTERCOMPORT))
			{
				DPL(0, "Message too small to be valid GetTesterCOMPort message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_GETTESTERCOMPORT));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleGetTesterCOMPortMsg(((PLEECHMSG_GETTESTERCOMPORT) pvSentData)->dwTopLevelUniqueID,
													((PLEECHMSG_GETTESTERCOMPORT) pvSentData)->dwTestUniqueID,
													((PLEECHMSG_GETTESTERCOMPORT) pvSentData)->iTesterNum,													
													pvReplyData);
				(*pdwReplyDataSize) = sizeof (LEECHMSG_GETTESTERCOMPORTREPLY);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_FREERESULTS:
			if (dwSentDataSize < sizeof (LEECHMSG_FREERESULTS))
			{
				DPL(0, "Message too small to be valid FreeResults message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_FREERESULTS));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleFreeResultsMsg(((PLEECHMSG_FREERESULTS) pvSentData)->dwTopLevelUniqueID,
												((PLEECHMSG_FREERESULTS) pvSentData)->dwTestUniqueID,
												((PLEECHMSG_FREERESULTS) pvSentData)->dwCaseIDSize,
												((PLEECHMSG_FREERESULTS) pvSentData)->dwInstanceIDSize,
												((LPBYTE) pvSentData) + sizeof (LEECHMSG_FREERESULTS));
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_FREEOUTPUTVARS:
			if (dwSentDataSize < sizeof (LEECHMSG_FREEOUTPUTVARS))
			{
				DPL(0, "Message too small to be valid FreeOutputVars message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_FREEOUTPUTVARS));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleFreeOutputVarsMsg(((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwTopLevelUniqueID,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwTestUniqueID,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwCaseIDSize,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwInstanceIDSize,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwNameSize,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->dwTypeSize,
													((PLEECHMSG_FREEOUTPUTVARS) pvSentData)->fFreeData,
													((LPBYTE) pvSentData) + sizeof (LEECHMSG_FREEOUTPUTVARS));
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_RUNTEST:
			if (dwSentDataSize < sizeof (LEECHMSG_RUNTEST))
			{
				DPL(0, "Message too small to be valid RunTest message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_RUNTEST));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleRunTestMsg(((PLEECHMSG_RUNTEST) pvSentData)->dwTestUniqueID,
											((PLEECHMSG_RUNTEST) pvSentData)->iNumMachines,
											((PLEECHMSG_RUNTEST) pvSentData)->dwInputDataSize,													
											(((PLEECHMSG_RUNTEST) pvSentData) + 1));
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_GETMACHINEINFO:
			if (dwSentDataSize < sizeof (LEECHMSG_GETMACHINEINFO))
			{
				DPL(0, "Message too small to be valid GetMachineInfo message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_GETMACHINEINFO));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleGetMachineInfoMsg(((PLEECHMSG_GETMACHINEINFO) pvSentData)->dwTopLevelUniqueID,
													((PLEECHMSG_GETMACHINEINFO) pvSentData)->dwTestUniqueID,
													((PLEECHMSG_GETMACHINEINFO) pvSentData)->iTesterNum,													
													pvReplyData,
													pdwReplyDataSize);
			} // end else (message appears to be valid)
		  break;

		case LEECHMSGID_LOSTTESTER:
			if (dwSentDataSize < sizeof (LEECHMSG_LOSTTESTER))
			{
				DPL(0, "Message too small to be valid LostTester message (%u <= %u)!",
					2, dwSentDataSize, sizeof (LEECHMSG_LOSTTESTER));
				hr = E_FAIL;
			} // end if (message too small)
			else
			{
				hr = this->HandleLostTesterMsg(((PLEECHMSG_LOSTTESTER) pvSentData)->dwTopLevelUniqueID,
												((PLEECHMSG_LOSTTESTER) pvSentData)->dwTestUniqueID,
												((PLEECHMSG_LOSTTESTER) pvSentData)->iLostTesterNum);
				(*pdwReplyDataSize) = 0;
			} // end else (message appears to be valid)
		  break;

		default:
			DPL(0, "Unrecognized message type %u!", 1, dwCommand);
			hr = E_FAIL;

			(*pdwReplyDataSize) = 0;
		  break;
	} // end switch (on command)

	return (hr);
} // CTNLeech::HandleMessage
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleDisconnect()"
//==================================================================================
// CTNLeech::HandleDisconnect
//----------------------------------------------------------------------------------
//
// Description: Handles a disconnection by the other side.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleDisconnect(void)
{
	HRESULT		hr = S_OK;
	HWND		hWnd = NULL;


	// Make sure the test thread is going down.
	if (this->m_fOwnerCopy)
	{
		DPL(0, "WARNING: Got disconnect message on owner side!", 0);

#pragma TODO(vanceo, "handle issues")
	} // end if (this is owner side)
	else
	{
		hr = this->m_pExecutor->KillTestThread(FALSE);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't kill test thread!  %e", 1, hr);

			// Note: continuing...
			//goto DONE;

		} // end if (couldn't kill test thread)

		if (this->m_pExecutor->m_pfnGetTestingWindow != NULL)
		{
			hr = this->m_pExecutor->m_pfnGetTestingWindow(&hWnd);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get testing window!  %e", 1, hr);

				// Note: continuing...
				//goto DONE;

			} // end if (couldn't get window)
			else
			{
				DPL(1, "Closing window %x because leech %x (\"%s\") was disconnected.",
					3, hWnd, this, this->m_pszAttachPointID);

				if (! PostMessage(hWnd, WM_CLOSE, 0, 0))
				{
					hr = GetLastError();
					DPL(0, "Couldn't post Close message to %x!", 1, hWnd);
				} // end if (couldn't post close message)
			} // end else (did get window)
		} // end if (have window to post)
	} // end else (this is process side)

	return (hr);
} // CTNLeech::HandleDisconnect
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleLogStringMsg()"
//==================================================================================
// CTNLeech::HandleLogStringMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to log a string.
//
// Arguments:
//	DWORD dwCommand		ID of command.
//	LPVOID lpvData		Pointer to command data, if any.
//	DWORD dwDataSize	Size of data, if any.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleLogStringMsg(DWORD dwLogStringType, char* szString)
{
	HRESULT		hr;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to log a string?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	hr = this->m_pExecutor->LogInternal(dwLogStringType, szString);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't log string?", 0);
	} // end if (failed logging)

	return (hr);
} // CTNLeech::HandleLogStringMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleReportMsg()"
//==================================================================================
// CTNLeech::HandleReportMsg
//----------------------------------------------------------------------------------
//
// Description: Handles an incoming report.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test for report.
//	DWORD dwTestUniqueID		ID of test generating report.
//	HRESULT hresult				Result code of report.
//	BOOL fTestComplete			Whether the report indicates completion or not.
//	BOOL fSuccess				Whether the report indicates sucess or not.
//	BOOL fExecCaseExitReport	Is this the report automatically generated when the
//								ExecCase function returns?
//	DWORD dwOutputDataSize		Size of output data, if any.
//	DWORD dwOutputVarsSize		Size of output variables, if any.
//	PVOID pvRemainingData		Pointer to output data & vars buffers, if any.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleReportMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID, 
									HRESULT hresult, BOOL fTestComplete,
									BOOL fSuccess, BOOL fExecCaseExitReport,
									DWORD dwOutputDataSize, DWORD dwOutputVarsSize,
									PVOID pvRemainingData)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;
	PTNRESULT			pResult = NULL;
	PVOID				pvOutputData = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a report?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't find leech test corresponding to ID %u:%u!",
			2, dwTopLevelUniqueID, dwTestUniqueID);
		return (ERROR_INVALID_PARAMETER);
	} // end if (we don't know about this test)


	// Intercept this report, and duplicate it in our owner's list.

	// Create the report object.
	pResult = this->m_pExecutor->m_results.NewResult(pTest);
	if (pResult == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate object)

	// Store the results
	hr = pResult->SetResultCodeAndBools(hresult, fTestComplete, fSuccess);
	if (pResult == NULL)
	{
		DPL(0, "Couldn't set result code and booleans!", 0);
		return (hr);
	} // end if (couldn't allocate object))

	// Store the output data and output variables
	if (dwOutputDataSize > 0)
	{
		pvOutputData = pResult->CreateOutputDataBuffer(dwOutputDataSize);
		if (pvOutputData == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate memory)

		CopyMemory(pvOutputData, pvRemainingData, dwOutputDataSize);

		if (dwOutputVarsSize > 0)
		{
			hr = pResult->m_vars.UnpackFromBuffer(((LPBYTE) pvRemainingData) + dwOutputDataSize,
													dwOutputVarsSize);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't unpack output variables!", 0);
				return (hr);
			} // end if (couldn't unpack temp vars from buffer)
		} // end if (there are output vars)
	} // end if (there is output data)

	hr = this->m_pExecutor->ReportInternal(pTest, hresult, fTestComplete,
											fSuccess, fExecCaseExitReport,
											pvOutputData, dwOutputDataSize,
											((dwOutputVarsSize > 0) ? (&pResult->m_vars) : NULL));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send report!", 0);
	} // end if (failed reporting)

	return (hr);
} // CTNLeech::HandleReportMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleWarningMsg()"
//==================================================================================
// CTNLeech::HandleWarningMsg
//----------------------------------------------------------------------------------
//
// Description: Handles an incoming warning.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test for warning.
//	DWORD dwTestUniqueID		ID of test generating warning.
//	HRESULT hresult				Result code of warning.
//	PVOID pvUserData			Pointer to user data, if any.
//	DWORD dwUserDataSize		Size of user data, if any.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleWarningMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID, 
									HRESULT hresult, PVOID pvUserData,
									DWORD dwUserDataSize)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a warning?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't find leech test corresponding to ID %u:%u!",
			2, dwTopLevelUniqueID, dwTestUniqueID);
		return (ERROR_INVALID_PARAMETER);
	} // end if (we don't know about this test)


	hr = this->m_pExecutor->WarnInternal(pTest, hresult, pvUserData, dwUserDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't send warning!", 0);
	} // end if (failed warning)

	return (hr);
} // CTNLeech::HandleWarningMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleCreateSubTestMsg()"
//==================================================================================
// CTNLeech::HandleCreateSubTestMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to create a subtest.
//
// Arguments:
//	DWORD dwTopLevelUniqueID		ID of top level test to create under.
//	DWORD dwParentUniqueID			ID of parent test to create under.
//	int iNumMachines				Number of testers in tester array.
//	PVOID pvRemainingData			Pointer to case string and tester array data.
//	DWORD* pdwAssignedUniqueID		Pointer to place to store ID assigned to new
//									test.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleCreateSubTestMsg(DWORD dwTopLevelUniqueID,
										DWORD dwParentUniqueID,
										int iNumMachines,													
										PVOID pvRemainingData,
										DWORD* pdwAssignedUniqueID)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pParentTest = NULL;
	char*				pszCaseID = NULL;
	PTNTESTTABLECASE	pCase = NULL;
	PTNCTRLMACHINEID	paTesters = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to create a subtest?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pParentTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID,
													dwParentUniqueID);
	if (pParentTest == NULL)
	{
		DPL(0, "Couldn't get parent leech test ID %u (under ID %u)!",
			2, dwParentUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)

	pszCaseID = (char*) pvRemainingData;

	pCase = this->m_pExecutor->m_testtable.GetTest(pszCaseID);
	if (pCase == NULL)
	{
		DPL(0, "Couldn't get test case %s!", 1, pszCaseID);
		return (E_FAIL);
	} // end if (couldn't get case)

	paTesters = (PTNCTRLMACHINEID) (pszCaseID + strlen(pszCaseID) + 1);

	hr = this->m_pExecutor->CreateSubTest(pParentTest, pCase, iNumMachines,
										paTesters);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't create subtest!", 0);
	} // end if (creating subtest failed)

	(*pdwAssignedUniqueID) = pParentTest->m_pSubTest->m_dwUniqueID;

	return (hr);
} // CTNLeech::HandleCreateSubTestMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleSyncMsg()"
//==================================================================================
// CTNLeech::HandleSyncMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to synchronize under the given test.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test syncing.
//	DWORD dwTestUniqueID		ID of test actually syncing.
//	int iNumMachines			Number of testers in tester array.
//	DWORD dwSendDataSize		Size of send data.
//	PVOID pvRemainingData		Pointer to tester array and send data.
//	PVOID pvReplyData			Pointer to buffer that can hold reply data.
//	DWORD* pdwReplyDataSize		Pointer to place to store size of reply data.
//								Initially holds the max size of the buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleSyncMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
								int iNumMachines, DWORD dwSendDataSize,
								PVOID pvRemainingData, PVOID pvReplyData,
								DWORD* pdwReplyDataSize)
{
	HRESULT				hr;
	HRESULT				hrSyncResult;
	PTNTESTINSTANCES	pTest = NULL;
	char*				pszSyncName = NULL;
	int*				paiTesterArray = NULL;
	LPVOID				pvSendData = NULL;
	CTNSyncDataList		receivedata;
	DWORD				dwSyncDataSize;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to sync?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get syncing leech test ID %u (under ID %u)!", 2, dwTestUniqueID,
			dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)

	pszSyncName = (char*) pvRemainingData;
	paiTesterArray = (int*) (pszSyncName + strlen(pszSyncName) + 1);

	if (dwSendDataSize > 0)
		pvSendData = ((LPBYTE) paiTesterArray) + (iNumMachines * sizeof (int));

	hrSyncResult = this->m_pExecutor->SyncInternal(pTest,
													pszSyncName,
													pvSendData,
													dwSendDataSize,
													&receivedata,
													iNumMachines,
													paiTesterArray);
	if (hrSyncResult != S_OK)
	{
		DPL(0, "WARNING: Couldn't internally sync!  Returning result via reply buffer.  %e",
			1, hrSyncResult);
	} // end if (syncing failed)


#pragma TODO(vanceo, "It would be nice not to have to unpack and repack every time")

	// Ignore error, assume BUFFER_TOO_SMALL
	receivedata.PackDataForTesterIntoBuffer(-1, NULL, &dwSyncDataSize);

	if (dwSyncDataSize > ((*pdwReplyDataSize) - sizeof (LEECHMSG_SYNCREPLY)))
	{
		DPL(0, "Can't fit sync data into reply buffer!", 0);
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (can't fit data into buffer)


	((PLEECHMSG_SYNCREPLY) pvReplyData)->hrResult = hrSyncResult;
	((PLEECHMSG_SYNCREPLY) pvReplyData)->dwSyncDataSize = dwSyncDataSize;


	hr = receivedata.PackDataForTesterIntoBuffer(-1,
												(((LPBYTE) pvReplyData) + sizeof (LEECHMSG_SYNCREPLY)),
												&dwSyncDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack sync data into buffer!", 0);
		return (hr);
	} // end if (syncing failed)

	(*pdwReplyDataSize) = sizeof (LEECHMSG_SYNCREPLY) + dwSyncDataSize;

	return (hr);
} // CTNLeech::HandleSyncMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleIsTesterOnMachineMsg()"
//==================================================================================
// CTNLeech::HandleIsTesterOnMachineMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a check if a given tester is on the same machine.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test checking.
//	DWORD dwTestUniqueID		ID of test actually checking.
//	int iTesterNum				Tester number whose IP is to be retrieved.
//	WORD wPort					Port to use when retrieving ID.
//	char* pszReplyString		Pointer to buffer to hold resulting IP string.
//	DWORD* pdwReplyStringSize	Pointer to place to store size of IP string.
//								Initially holds the max size of the buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleIsTesterOnMachineMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
											int iTesterNum, BOOL* pfResult)
{
	PTNTESTINSTANCES	pTest = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to check tester?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)

	(*pfResult) = this->m_pExecutor->IsTesterOnSameMachineInternal(pTest, iTesterNum);
	return (S_OK);
} // CTNLeech::HandleIsTesterOnMachineMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleGetTesterIPMsg()"
//==================================================================================
// CTNLeech::HandleGetTesterIPMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request for a tester's IP address under the given test.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test getting the IP address.
//	DWORD dwTestUniqueID		ID of test actually retrieving.
//	int iTesterNum				Tester number whose IP is to be retrieved.
//	WORD wPort					Port to use when retrieving ID.
//	char* pszReplyString		Pointer to buffer to hold resulting IP string.
//	DWORD* pdwReplyStringSize	Pointer to place to store size of IP string.
//								Initially holds the max size of the buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleGetTesterIPMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
										int iTesterNum, WORD wPort,
										char* pszReplyString, DWORD* pdwReplyStringSize)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to retrieve a tester IP?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)

	hr = this->m_pExecutor->GetTesterIPInternal(pTest, iTesterNum, wPort,
												pszReplyString);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get tester IP!", 0);
		return (hr);
	} // end if (getting IP failed)

	(*pdwReplyStringSize) = strlen(pszReplyString) + 1;

	return (hr);
} // CTNLeech::HandleGetTesterIPMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleGetTesterPhoneNumMsg()"
//==================================================================================
// CTNLeech::HandleGetTesterPhoneNumMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request for a tester's phone number under the given test.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test getting the phone number.
//	DWORD dwTestUniqueID		ID of test actually retrieving.
//	int iTesterNum				Tester number whose COM port is to be retrieved.
//	PVOID pvReplyMsg			Pointer to message for reply.  Cast to a
//								LEECHMSG_GETTESTERPHONENUMREPLY pointer.
//	DWORD* pdwReplyMsgSize		Pointer to place to store size of reply message.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleGetTesterPhoneNumMsg(DWORD dwTopLevelUniqueID,
											DWORD dwTestUniqueID,
											int iTesterNum, PVOID pvReplyMsg,
											DWORD* pdwReplyMsgSize)
{
	HRESULT								hr;
	PTNTESTINSTANCES					pTest = NULL;
	PLEECHMSG_GETTESTERPHONENUMREPLY	pReplyMsg = (PLEECHMSG_GETTESTERPHONENUMREPLY) pvReplyMsg;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to retrieve a tester phone number?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)

#pragma BUGBUG(vanceo, "Mapping essentially dynamic string to static")

	hr = this->m_pExecutor->GetTesterPhoneNumInternal(pTest, iTesterNum,
													(char*) (pReplyMsg + 1));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get tester phone number!", 0);
		return (hr);
	} // end if (getting phone number failed)

#pragma BUGBUG(vanceo, "Actually use this correctly")
	/*
	if (lpReplyMsg->? == 0)
		lpReplyMsg->hrResult = E_FAIL;
	else
	*/
	{
		pReplyMsg->hrResult = S_OK;
		(*pdwReplyMsgSize) = sizeof (LEECHMSG_GETTESTERPHONENUMREPLY)
							+ strlen((char*) (pReplyMsg + 1)) + 1;
	} // end else (successfull)

	return (hr);
} // CTNLeech::HandleGetTesterPhoneNumMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleGetTesterCOMPortMsg()"
//==================================================================================
// CTNLeech::HandleGetTesterCOMPortMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request for a tester's COM port under the given test.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test getting the COM port.
//	DWORD dwTestUniqueID		ID of test actually retrieving.
//	int iTesterNum				Tester number whose COM port is to be retrieved.
//	PVOID pvReplyMsg			Pointer to message for reply.  Cast to a
//								LEECHMSG_GETTESTERCOMPORTREPLY pointer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleGetTesterCOMPortMsg(DWORD dwTopLevelUniqueID,
											DWORD dwTestUniqueID,
											int iTesterNum, PVOID pvReplyMsg)
{
	HRESULT								hr;
	PTNTESTINSTANCES					pTest = NULL;
	PLEECHMSG_GETTESTERCOMPORTREPLY		pReplyMsg = (PLEECHMSG_GETTESTERCOMPORTREPLY) pvReplyMsg;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to retrieve a tester COM port?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)


	hr = this->m_pExecutor->GetTesterCOMPortInternal(pTest, iTesterNum,
													&(pReplyMsg->dwCOMPort));
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get tester COM port!", 0);
		return (hr);
	} // end if (getting COM port failed)

#pragma BUGBUG(vanceo, "Actually use this correctly")
	if (pReplyMsg->dwCOMPort == 0)
		pReplyMsg->hrResult = E_FAIL;
	else
		pReplyMsg->hrResult = S_OK;

	return (hr);
} // CTNLeech::HandleGetTesterCOMPortMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleFreeResultsMsg()"
//==================================================================================
// CTNLeech::HandleFreeResultsMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to free results.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test requesting the freeresults.
//	DWORD dwTestUniqueID		ID of test actually freeing results.
//	DWORD dwCaseIDSize			Size of case ID string, if any.
//	DWORD dwInstanceIDSize		Size of instance ID string, if any.
//	PVOID pvRemainingData		Data buffer containing remaining data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleFreeResultsMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
									DWORD dwCaseIDSize, DWORD dwInstanceIDSize,
									PVOID pvRemainingData)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;
	LPBYTE				lpCurrent;
	char*				pszCaseID = NULL;
	char*				pszInstanceID = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to free results?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)


	lpCurrent = (LPBYTE) pvRemainingData;

	if (dwCaseIDSize > 0)
	{
		pszCaseID = (char*) lpCurrent;
		lpCurrent += dwCaseIDSize;
	} // end if (there's a case ID)

	if (dwInstanceIDSize > 0)
	{
		pszInstanceID = (char*) lpCurrent;
		lpCurrent += dwInstanceIDSize;
	} // end if (there's a case ID)


	// We know it's user called, or else we wouldn't be here.  Pass the specified
	// test in.
	hr = this->m_pExecutor->FreeResultsInternal(pszCaseID, pszInstanceID,
												pTest);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't free results!", 0);
		//return (hr);
	} // end if (freeing results failed)

	return (hr);
} // CTNLeech::HandleFreeResultsMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleFreeOutputVarsMsg()"
//==================================================================================
// CTNLeech::HandleFreeOutputVarsMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to free output vars.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test requesting the freeoutputvars.
//	DWORD dwTestUniqueID		ID of test actually freeing output vars.
//	DWORD dwCaseIDSize			Size of case ID string, if any.
//	DWORD dwInstanceIDSize		Size of instance ID string, if any.
//	DWORD dwNameSize			Size of variable name string, if any.
//	DWORD dwTypeSize			Size of variable type string, if any.
//	BOOL fFreeData				Whether to remove output data, if no vars.
//	PVOID pvRemainingData		Data buffer containing remaining data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleFreeOutputVarsMsg(DWORD dwTopLevelUniqueID, DWORD dwTestUniqueID,
										DWORD dwCaseIDSize, DWORD dwInstanceIDSize,
										DWORD dwNameSize, DWORD dwTypeSize,
										BOOL fFreeData, PVOID pvRemainingData)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;
	LPBYTE				lpCurrent;
	char*				pszCaseID = NULL;
	char*				pszInstanceID = NULL;
	char*				pszName = NULL;
	char*				pszType = NULL;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to free output vars?", 0);
		return (E_FAIL);
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		return (E_FAIL);
	} // end if (couldn't find parent test with given ID)


	lpCurrent = (LPBYTE) pvRemainingData;

	if (dwCaseIDSize > 0)
	{
		pszCaseID = (char*) lpCurrent;
		lpCurrent += dwCaseIDSize;
	} // end if (there's a case ID)

	if (dwInstanceIDSize > 0)
	{
		pszInstanceID = (char*) lpCurrent;
		lpCurrent += dwInstanceIDSize;
	} // end if (there's a case ID)

	if (dwNameSize > 0)
	{
		pszName = (char*) lpCurrent;
		lpCurrent += dwNameSize;
	} // end if (there's a case ID)

	if (dwTypeSize > 0)
	{
		pszType = (char*) lpCurrent;
		lpCurrent += dwTypeSize;
	} // end if (there's a case ID)


	// We know it's user called, or else we wouldn't be here.  Pass TRUE in.
	hr = this->m_pExecutor->FreeOutputVarsInternal(pszCaseID, pszInstanceID,
													pszName, pszType, fFreeData,
													pTest);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't free output vars!", 0);
		//return (hr);
	} // end if (freeing output vars failed)

	return (hr);
} // CTNLeech::HandleFreeOutputVarsMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleRunTestMsg()"
//==================================================================================
// CTNLeech::HandleRunTestMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request to run a test.
//
// Arguments:
//	DWORD dwTestUniqueID		Unique ID of test to run.
//	int iNumMachines			Number of testers running the instance.
//	DWORD dwInputDataSize		Size of input data buffer.
//	PVOID pvRemainingData		Pointer to buffer containing case ID and input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleRunTestMsg(DWORD dwTestUniqueID, int iNumMachines,
									DWORD dwInputDataSize, PVOID pvRemainingData)
{
	HRESULT				hr;
	PTNTESTINSTANCES	pTest = NULL;
	char*				pszCaseID = NULL;
	PTNCTRLMACHINEID	paTesters = NULL;
	PVOID				pvInputData = NULL;


	if (this->m_fOwnerCopy)
	{
		DPL(0, "Not leech side but getting a request to run a test?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (not leech side)

	pszCaseID = (char*) pvRemainingData;

	paTesters = (PTNCTRLMACHINEID) (pszCaseID + strlen(pszCaseID) + 1);

	if (dwInputDataSize > 0)
		pvInputData = paTesters + iNumMachines;


	hr = this->m_pExecutor->HandleNewTest(dwTestUniqueID, pszCaseID, NULL,
										iNumMachines, paTesters,
										pvInputData, dwInputDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't handle new test (ID %u)!", 1, dwTestUniqueID);
		goto DONE;
	} // end if (couldn't handle new test)


DONE:

	return (hr);
} // CTNLeech::HandleRunTestMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleGetMachineInfoMsg()"
//==================================================================================
// CTNLeech::HandleGetMachineInfoMsg
//----------------------------------------------------------------------------------
//
// Description: Handles a request for a tester's machine info under the given test.
//
// Arguments:
//	DWORD dwTopLevelUniqueID	ID of top level test to for GetTesterMachineInfo.
//	DWORD dwTestUniqueID		ID of test actually retrieving.
//	int iTesterNum				Tester number whose info is to be retrieved.
//	PVOID pvReplyMsg			Pointer to buffer for reply.  Cast as a
//								PLEECHMSG_GETMACHINEINFOREPLY.
//	DWORD* pdwReplyMsgSize		Pointer to size of reply buffer.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleGetMachineInfoMsg(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iTesterNum,
										PVOID pvReplyMsg,
										DWORD* pdwReplyMsgSize)
{
	HRESULT							hr;
	HRESULT							hrGetResult;
	PTNTESTINSTANCES				pTest = NULL;
	PTNOTHERMACHINEINFO				pInfo = NULL;
	PLEECHMSG_GETMACHINEINFOREPLY	pReplyMsg = (PLEECHMSG_GETMACHINEINFOREPLY) pvReplyMsg;
	DWORD							dwMachineInfoSize = 0;


	if (! this->m_fOwnerCopy)
	{
		DPL(0, "Not owner side but getting a request to retrieve a tester's info?", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (not owner side)


	pTest = this->m_pExecutor->GetLeechTest(dwTopLevelUniqueID, dwTestUniqueID);
	if (pTest == NULL)
	{
		DPL(0, "Couldn't get requesting leech test ID %u (under ID %u)!",
			2, dwTestUniqueID, dwTopLevelUniqueID);
		hr = E_FAIL;
		goto DONE;
	} // end if (couldn't find parent test with given ID)


	hrGetResult = this->m_pExecutor->GetTestersMachineInfoInternal(pTest,
																	iTesterNum,
																	(PTNMACHINEINFO*) (&pInfo));
	if (hrGetResult != S_OK)
	{
		DPL(0, "WARNING: Couldn't get tester's machine info!  Passing error to other side.  %e",
			1, hrGetResult);
	} // end if (getting COM port failed)



#pragma TODO(vanceo, "It would be nice not to have to unpack and repack every time")

	// Ignore error
	pInfo->PackIntoBuffer(NULL, &dwMachineInfoSize);


	// Just make sure there's enough room for message in reply buffer
	if ((dwMachineInfoSize + sizeof (LEECHMSG_GETMACHINEINFOREPLY)) > (*pdwReplyMsgSize))
	{
		DPL(0, "Not enough room in reply buffer (%u > %u)!",
			2, dwMachineInfoSize + sizeof (LEECHMSG_GETMACHINEINFOREPLY),
			(*pdwReplyMsgSize));
		hr = E_FAIL;
		goto DONE;
	} // end if (not enough room for message)

	pReplyMsg->hrResult = hrGetResult;
	pReplyMsg->dwMachineInfoSize = dwMachineInfoSize;


	hr = pInfo->PackIntoBuffer((pReplyMsg + 1), &dwMachineInfoSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't repack machine info into buffer!", 0);
		goto DONE;
	} // end if (packing machine info failed)

	(*pdwReplyMsgSize) = dwMachineInfoSize + sizeof (LEECHMSG_GETMACHINEINFOREPLY);


DONE:


	// If we have an info object, we don't need the object reference any more.
	if (pInfo != NULL)
	{
		pInfo->m_dwRefCount--;
		if (pInfo->m_dwRefCount == 0)
		{
			DPL(0, "Info object %x refcount hit 0!?  DEBUGBREAK()-ing.", 1, pInfo);
			DEBUGBREAK();
		} // end if (info refcount hit 0)
		else
		{
			DPL(7, "Not deleting info object %x, refcount is %u.",
				2, pInfo, pInfo->m_dwRefCount);
		} // end else (refcount is fine)
	} // end if (have info object)

	return (hr);
} // CTNLeech::HandleGetMachineInfoMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeech::HandleLostTesterMsg()"
//==================================================================================
// CTNLeech::HandleLostTesterMsg
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
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeech::HandleLostTesterMsg(DWORD dwTopLevelUniqueID,
										DWORD dwTestUniqueID,
										int iLostTesterNum)
{
	HRESULT		hr;


	if (this->m_fOwnerCopy)
	{
		DPL(0, "Not leech side but getting lost tester message?", 0);
		return (E_FAIL);
	} // end if (not leech side)

	hr = this->m_pExecutor->HandleLostTester(dwTopLevelUniqueID, dwTestUniqueID,
											iLostTesterNum);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't handle lost tester message!", 0);
	} // end if (couldn't handle message)

	return (hr);
} // CTNLeech::HandleLostTesterMsg
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeechesList::CTNLeechesList()"
//==================================================================================
// CTNLeechesList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNLeechesList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNLeechesList::CTNLeechesList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNLeechesList));
} // CTNLeechesList::CTNLeechesList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeechesList::~CTNLeechesList()"
//==================================================================================
// CTNLeechesList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNLeechesList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNLeechesList::~CTNLeechesList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNLeechesList::~CTNLeechesList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeechesList::NewLeech()"
//==================================================================================
// CTNLeechesList::NewLeech
//----------------------------------------------------------------------------------
//
// Description: Creates a new leech object and returns a pointer to it, or returns
//				NULL if an error occurred.
//
// Arguments:
//	LPTNMODULEID lpTNModuleID			Pointer to module ID for object.
//	PTNEXECUTORPRIV pOwningExecutor		Pointer to executor object that will own the
//										leech.
//	PVOID pvSendConnectData				Optional pointer to data to send to other
//										side.
//	DWORD dwSendConnectDataSize			Size of data to send to other side.
//
// Returns: Pointer to the new leech object, or NULL if an error occurred.
//==================================================================================
PTNLEECH CTNLeechesList::NewLeech(PTNMODULEID pTNModuleID,
								PTNEXECUTORPRIV pOwningExecutor,
								PVOID pvSendConnectData,
								DWORD dwSendConnectDataSize)
{
	HRESULT						hr;
	PTNLEECH					pNewItem = NULL;
	PLEECHCONNECTDATAHEADER		pConnectData = NULL;
	char						szNumber[32];



#pragma BUGBUG(vanceo, "Hack, see above")
	pConnectData = (PLEECHCONNECTDATAHEADER) LocalAlloc(LPTR, sizeof (LEECHCONNECTDATAHEADER) + dwSendConnectDataSize);
	if (pConnectData == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	CopyMemory(&(pConnectData->idSlave), &(pOwningExecutor->m_id),
				sizeof (TNCTRLMACHINEID));
	pConnectData->dwRealConnectDataSize = dwSendConnectDataSize;

	if (pvSendConnectData != NULL)
	{
		CopyMemory((pConnectData + 1), pvSendConnectData,
					dwSendConnectDataSize);
	} // end if (there's connect data)


	pNewItem = new (CTNLeech);
	if (pNewItem == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pNewItem->m_pExecutor = pOwningExecutor;


	// Generate a (hopefully) unique ID to use for connecting.
#ifndef _XBOX // timeGetTime not supported
	wsprintf(szNumber, "%u-%u", timeGetTime(), g_dwUniquenessValue++);
#else // ! XBOX
	wsprintf(szNumber, "%u-%u", GetTickCount(), g_dwUniquenessValue++);
#endif // XBOX

	hr = pNewItem->Connect(TRUE, pTNModuleID, szNumber, NULL, pConnectData,
							sizeof (LEECHCONNECTDATAHEADER) + dwSendConnectDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't connect leech %x using \"%s\"!", 2, pNewItem, szNumber);
		delete (pNewItem);
		pNewItem = NULL;
		goto DONE;
	} // end if (couldn't connect)


	// Add the item to the list
	hr = this->AddObject(pNewItem);

	if (hr != S_OK)
	{
		DPL(0, "Couldn't add IPC object!  %e", 1, hr);
		delete (pNewItem);
		pNewItem = NULL;
		//goto DONE;
	} // end if (couldn't add object)


DONE:

	if (pConnectData != NULL)
	{
		LocalFree(pConnectData);
		pConnectData = NULL;
	} // end if (there's a connect data header)

	return (pNewItem);
} // CTNLeechesList::NewLeech
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNLeechesList::RemoveLeech()"
//==================================================================================
// CTNLeechesList::RemoveLeech
//----------------------------------------------------------------------------------
//
// Description: Detaches the specified leech from the list and destroys it.  Note
//				that the pointer passed in becomes invalid.
//
// Arguments:
//	PTNLEECH pTNLeech	Pointer to leech to remove.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNLeechesList::RemoveLeech(PTNLEECH pTNLeech)
{
	return (this->RemoveObject(pTNLeech));
} // CTNLeechesList::RemoveLeech
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX