//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\debugprint.h"
#include "..\tncommon\strutils.h"


#include "tncontrl.h"
#include "main.h"

#include "resource.h"

#include "script.h"








//==================================================================================
// Prototypes
//==================================================================================
HRESULT ScriptGet_FreeOutputVars(PTNGETINPUTDATA pTNgid);

HRESULT ScriptExec_FreeOutputVars(PTNEXECCASEDATA pTNecd);


HRESULT ScriptGet_FreeResults(PTNGETINPUTDATA pTNgid);

HRESULT ScriptExec_FreeResults(PTNEXECCASEDATA pTNecd);


HRESULT ScriptGet_Sleep(PTNGETINPUTDATA pTNgid);

HRESULT ScriptExec_Sleep(PTNEXECCASEDATA pTNecd);


HRESULT ScriptGet_Wait(PTNGETINPUTDATA pTNgid);

HRESULT ScriptExec_Wait(PTNEXECCASEDATA pTNecd);


INT_PTR CALLBACK WaitForUserOKDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptLoadTestTable()"
//==================================================================================
// ScriptLoadTestTable
//----------------------------------------------------------------------------------
//
// Description: Loads all the possible tests into the table passed in:
//				FreeOutputVars	Built-in script: FreeOutputVars
//				FreeResults		Built-in script: FreeResults
//				Sleep			Built-in script: Sleep
//				WaitForUserOK	Built-in script: WaitForUserOK
//
// Arguments:
//	PTNTESTTABLEGROUP pTable	Group/table to fill with tests in this file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ScriptLoadTestTable(PTNTESTTABLEGROUP pTable)
{
	PTNTESTTABLEGROUP	pSubGroup;
	TNADDTESTDATA		tnatd;



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// BuiltIn			Built-in script functions
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	NEWSUBGROUP(pTable, "BuiltIn", "Built-in script functions", &pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// FreeOutputVars	Built-in script: FreeOutputVars
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "FreeOutputVars";
	tnatd.pszName			= "Built-in script: FreeOutputVars";

	tnatd.pszDescription	= "Frees all output variables (matching the optional input data) that have accumulated.";

	tnatd.pszInputDataHelp	= "CaseID = frees only output variables generated from the matching case ID\n"
								"InstanceID = frees only output variables generated from that matching instance ID\n"
								"Name = frees only output variables with this name\n"
								"Type = frees only output variables with this type\n"
								"FreeData = TRUE to free the output data if all variables are gone, FALSE if not";

	tnatd.iNumMachines		= -1;
	tnatd.dwOptionFlags		= TNTCO_BUILTIN
								| TNTCO_API | TNTCO_STRESS | TNTCO_POKE | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= ScriptGet_FreeOutputVars;
	tnatd.pfnExecCase		= ScriptExec_FreeOutputVars;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// FreeResults		Built-in script: FreeResults
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "FreeResults";
	tnatd.pszName			= "Built-in script: FreeResults";

	tnatd.pszDescription	= "Frees all results (matching the optional input data) that have accumulated.";

	tnatd.pszInputDataHelp	= "CaseID = frees only results that match this case ID\n"
								"InstanceID = frees only results that match this instance ID";

	tnatd.iNumMachines		= -1;
	tnatd.dwOptionFlags		= TNTCO_BUILTIN
								| TNTCO_API | TNTCO_STRESS | TNTCO_POKE | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= ScriptGet_FreeResults;
	tnatd.pfnExecCase		= ScriptExec_FreeResults;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);



	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Sleep		Built-in script: Sleep
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "Sleep";
	tnatd.pszName			= "Built-in script: Sleep";

	tnatd.pszDescription	= "Causes the machine to do nothing for the given amount of time (see WaitForEventOrCancel).";

	tnatd.pszInputDataHelp	= "MS = Number of milliseconds to sleep";

	tnatd.iNumMachines		= -1;
	tnatd.dwOptionFlags		= TNTCO_BUILTIN
								| TNTCO_API | TNTCO_STRESS | TNTCO_POKE | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= ScriptGet_Sleep;
	tnatd.pfnExecCase		= ScriptExec_Sleep;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);


	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// WaitForUserOK		Built-in script: WaitForUserOK
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	ZeroMemory(&tnatd, sizeof (TNADDTESTDATA));
	tnatd.dwSize			= sizeof (TNADDTESTDATA);
	tnatd.pszCaseID			= "WaitForUserOK";
	tnatd.pszName			= "Built-in script: WaitForUserOK";

	tnatd.pszDescription	= "Prompts the user to acknowledge a dialog box before continuing.";

	tnatd.pszInputDataHelp	= "Timeout = Number of seconds to wait before automatically closing dialog\n"
								"Message = Message string to display in dialog";

	tnatd.iNumMachines		= -1;
	tnatd.dwOptionFlags		= TNTCO_BUILTIN
								| TNTCO_API | TNTCO_STRESS | TNTCO_POKE | TNTCO_PICKY
								| TNTCO_TOPLEVELTEST | TNTCO_SUBTEST;

	tnatd.pfnCanRun			= NULL;
	tnatd.pfnGetInputData	= ScriptGet_Wait;
	tnatd.pfnExecCase		= ScriptExec_Wait;
	tnatd.pfnWriteData		= NULL;
	tnatd.pfnFilterSuccess	= NULL;

	tnatd.paGraphs			= NULL;
	tnatd.dwNumGraphs		= 0;

	ADDTESTTOGROUP(&tnatd, pSubGroup);
#ifdef _XBOX // We don't support these, but we'll add them just so checksums match
#pragma TODO(tristanj, "We don't support WaitForUserOK, but we need to make the checksums match")
#endif // XBOX


	return (S_OK);
} // ScriptLoadTestTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptGet_FreeOutputVars()"
//==================================================================================
// ScriptGet_FreeOutputVars
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				FreeOutputVars - Built-in script: FreeOutputVars
//
// Arguments:
//	PTNGETINPUTDATA pTNgid		Pointer to parameter block with information on how
//								and where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ScriptGet_FreeOutputVars(PTNGETINPUTDATA pTNgid)
{
	PTID_SCRIPT_FREEOUTPUTVARS	pParams = (PTID_SCRIPT_FREEOUTPUTVARS) pTNgid->pvData;
	PLSTRINGLIST				pStrings = NULL;
	char*						pszTemp;
	LPBYTE						lpCurrent;


	// Start off with base size.
	pTNgid->dwDataSize = sizeof (TID_SCRIPT_FREEOUTPUTVARS);


	if (pParams != NULL)
	{
		// Initialize to defaults.
		pParams->dwCaseIDSize = 0;
		pParams->dwInstanceIDSize = 0;
		pParams->dwNameSize = 0;
		pParams->dwTypeSize = 0;
		pParams->fFreeData = FALSE;

		lpCurrent = (LPBYTE) (pParams + 1);
	} // end if (there's a buffer)

	pStrings = pTNgid->pStringData->GetScriptInputDataStrings();

#pragma TODO(vanceo, "Check assignment reports")

	// Check for a message string
	if (pStrings != NULL)
	{
		// Find the CaseID key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("CaseID");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwCaseIDSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwCaseIDSize);
			} // end else (actually getting data)
		} // end if (got key value)


		// Find the InstanceID key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("InstanceID");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwInstanceIDSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwInstanceIDSize);
			} // end else (actually getting data)
		} // end if (got key value)


		// Find the name key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("Name");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwNameSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwInstanceIDSize);
			} // end else (actually getting data)
		} // end if (got key value)


		// Find the type key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("Type");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwTypeSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwInstanceIDSize);
			} // end else (actually getting data)
		} // end if (got key value)


		// If there's a buffer to copy too
		if (pParams != NULL)
		{
			// Find the name key, case insensitive.
			pParams->fFreeData = pStrings->IsKeyTrue("FreeData");
		} // end if (there's a buffer)
	} // end if (there was script data)


	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // ScriptGet_FreeOutputVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptExec_FreeOutputVars()"
//==================================================================================
// ScriptExec_FreeOutputVars
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				FreeOutputVars - Built-in script: FreeOutputVars
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data:
//	Optional case and instance IDs to filter.
//	Optional variable name and type to filter.
//	Whether to free the output data if all the variables are removed, too.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ScriptExec_FreeOutputVars(PTNEXECCASEDATA pTNecd)
{
	HRESULT						hr;
	PTID_SCRIPT_FREEOUTPUTVARS	pInput = (PTID_SCRIPT_FREEOUTPUTVARS) pTNecd->pvInputData;
	BOOL						fSuccess = TRUE;
	LPBYTE						lpCurrent;
	char*						pszCaseID = NULL;
	char*						pszInstanceID = NULL;
	char*						pszName = NULL;
	char*						pszType = NULL;


	if (pTNecd->dwInputDataSize != sizeof (TID_SCRIPT_FREEOUTPUTVARS))
	{
		DPL(0, "Didn't get expected input data (size %u != %u)!",
			2, pTNecd->dwInputDataSize, sizeof (TID_SCRIPT_FREEOUTPUTVARS));
		return (E_FAIL);
	} // end if (didn't get expected input data)


	lpCurrent = (LPBYTE) (pInput + 1);

	if (pInput->dwCaseIDSize > 0)
	{
		pszCaseID = (char*) lpCurrent;
		lpCurrent += pInput->dwCaseIDSize;
	} // end if (there's a case ID string)

	if (pInput->dwInstanceIDSize > 0)
	{
		pszInstanceID = (char*) lpCurrent;
		lpCurrent += pInput->dwInstanceIDSize;
	} // end if (there's an instance ID string)

	if (pInput->dwNameSize > 0)
	{
		pszName = (char*) lpCurrent;
		lpCurrent += pInput->dwNameSize;
	} // end if (there's a name string)

	if (pInput->dwTypeSize > 0)
	{
		pszType = (char*) lpCurrent;
		lpCurrent += pInput->dwTypeSize;
	} // end if (there's a type string)


	hr = pTNecd->pExecutor->FreeOutputVars(pszCaseID, pszInstanceID, pszName, pszType,
											pInput->fFreeData);
	if (hr != S_OK)
		fSuccess = FALSE;

	hr = pTNecd->pFinalResult->SetResultCodeAndBools(hr, TRUE, fSuccess);
	if (hr != S_OK)
	{
		DPL(0, "Setting test results failed!", 0);
	} // end if (setting results failed)

	return (hr);
} // ScriptExec_FreeOutputVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptGet_FreeResults()"
//==================================================================================
// ScriptGet_FreeResults
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				FreeResults - Built-in script: FreeResults
//
// Arguments:
//	PTNGETINPUTDATA pTNgid		Pointer to parameter block with information on how
//								and where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ScriptGet_FreeResults(PTNGETINPUTDATA pTNgid)
{
	PTID_SCRIPT_FREERESULTS		pParams = (PTID_SCRIPT_FREERESULTS) pTNgid->pvData;
	PLSTRINGLIST				pStrings = NULL;
	char*						pszTemp;
	LPBYTE						lpCurrent;


	// Start off with base size.
	pTNgid->dwDataSize = sizeof (TID_SCRIPT_FREERESULTS);


	if (pParams != NULL)
	{
		// Initialize to defaults.
		pParams->dwCaseIDSize = 0;
		pParams->dwInstanceIDSize = 0;

		lpCurrent = (LPBYTE) (pParams + 1);
	} // end if (there's a buffer)

	pStrings = pTNgid->pStringData->GetScriptInputDataStrings();

#pragma TODO(vanceo, "Check assignment reports")

	// Check for a message string
	if (pStrings != NULL)
	{
		// Find the CaseID key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("CaseID");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwCaseIDSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwCaseIDSize);
			} // end else (actually getting data)
		} // end if (got key value)


		// Find the InstanceID key, case insensitive.
		pszTemp = pStrings->GetValueOfKey("InstanceID");
		if (pszTemp != NULL)
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (getting size)
			else
			{
				pParams->dwInstanceIDSize = strlen(pszTemp) + 1;
				CopyAndMoveDestPointer(lpCurrent, pszTemp, pParams->dwInstanceIDSize);
			} // end else (actually getting data)
		} // end if (got key value)
	} // end if (there was script data)


	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // ScriptGet_FreeResults
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptExec_FreeResults()"
//==================================================================================
// ScriptExec_FreeResults
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				FreeResults - Built-in script: FreeResults
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data:
//	Optional case and instance IDs to filter.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ScriptExec_FreeResults(PTNEXECCASEDATA pTNecd)
{
	HRESULT						hr;
	PTID_SCRIPT_FREERESULTS		pInput = (PTID_SCRIPT_FREERESULTS) pTNecd->pvInputData;
	BOOL						fSuccess = TRUE;
	LPBYTE						lpCurrent;
	char*						pszCaseID = NULL;
	char*						pszInstanceID = NULL;


	if (pTNecd->dwInputDataSize != sizeof (TID_SCRIPT_FREERESULTS))
	{
		DPL(0, "Didn't get expected input data (size %u != %u)!",
			2, pTNecd->dwInputDataSize, sizeof (TID_SCRIPT_FREERESULTS));
		return (E_FAIL);
	} // end if (didn't get expected input data)


	lpCurrent = (LPBYTE) (pInput + 1);

	if (pInput->dwCaseIDSize > 0)
	{
		pszCaseID = (char*) lpCurrent;
		lpCurrent += pInput->dwCaseIDSize;
	} // end if (there's a case ID string)

	if (pInput->dwInstanceIDSize > 0)
	{
		pszInstanceID = (char*) lpCurrent;
		lpCurrent += pInput->dwInstanceIDSize;
	} // end if (there's an instance ID string)


	hr = pTNecd->pExecutor->FreeResults(pszCaseID, pszInstanceID);
	if (hr != S_OK)
		fSuccess = FALSE;

	hr = pTNecd->pFinalResult->SetResultCodeAndBools(hr, TRUE, fSuccess);
	if (hr != S_OK)
	{
		DPL(0, "Setting test results failed!", 0);
	} // end if (setting results failed)

	return (hr);
} // ScriptExec_FreeResults
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptGet_Sleep()"
//==================================================================================
// ScriptGet_Sleep
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				Sleep - Built-in script: Sleep
//
// Arguments:
//	PTNGETINPUTDATA pTNgid		Pointer to parameter block with information on how
//								and where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ScriptGet_Sleep(PTNGETINPUTDATA pTNgid)
{
	PTID_SCRIPT_SLEEP	pParams = (PTID_SCRIPT_SLEEP) pTNgid->pvData;
	PLSTRINGLIST		pStrings = NULL;
	char*				pszTemp;


	pTNgid->dwDataSize = sizeof (TID_SCRIPT_SLEEP);

	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);


	pStrings = pTNgid->pStringData->GetScriptInputDataStrings();

#pragma TODO(vanceo, "Check assignment reports, improve")

	
	if ((pStrings == NULL) || (pStrings->Count() != 1))
	{
		DPL(0, "Didn't get expected script initial data (requires \"MS=\")", 0);
		return (S_OK);
	} // end if (didn't get expected parameters)


	// Find the key MS, case insensitive.
	pszTemp = pStrings->GetValueOfKey("MS");
	if (pszTemp == NULL)
	{
		DPL(0, "Couldn't get value of key \"MS\"!", 0);
		return (S_OK);
	} // end if (couldn't get key value)

	pParams->dwMS = StringToDWord(pszTemp);

	return (S_OK);
} // ScriptGet_Sleep
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptExec_Sleep()"
//==================================================================================
// ScriptExec_Sleep
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				Sleep - Built-in script: Sleep
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data:
//	Number of millisconds to sleep.
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ScriptExec_Sleep(PTNEXECCASEDATA pTNecd)
{
	HRESULT				hr;
	PTID_SCRIPT_SLEEP	pInput = (PTID_SCRIPT_SLEEP) pTNecd->pvInputData;


	if (pTNecd->dwInputDataSize != sizeof (TID_SCRIPT_SLEEP))
	{
		DPL(0, "Didn't get expected input data (size %u != %u)!",
			2, pTNecd->dwInputDataSize, sizeof (TID_SCRIPT_SLEEP));
		return (E_FAIL);
	} // end if (didn't get expected input data)


	DPL(0, "Built-in script: Sleep;  Sleeping for %u milliseconds",
		1, pInput->dwMS);


	hr = pTNecd->pExecutor->WaitForEventOrCancel(NULL, 0, NULL, 0, pInput->dwMS, NULL);
	if (hr != TNWR_TIMEOUT)
	{
		DPL(0, "Had problem waiting for event or cancel!  %e", 1, hr);
		pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID,
							"Had problem waiting for event or cancel!  %e", 1, hr);
		return (hr);
	} // end if (waiting failed)


	// Of course this function always is successful.

	hr = pTNecd->pFinalResult->SetResultCodeAndBools(S_OK, TRUE, TRUE);
	if (hr != S_OK)
	{
		DPL(0, "Setting test results failed!", 0);
	} // end if (setting results failed)

	return (hr);
} // ScriptExec_Sleep
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptGet_Wait()"
//==================================================================================
// ScriptGet_Wait
//----------------------------------------------------------------------------------
//
// Description: Callback that retrieves the initial data for the test case(s):
//				WaitForUserOK	Built-in script: WaitForUserOK
//
// Arguments:
//	PTNGETINPUTDATA pTNgid	Pointer to parameter block with information on how and
//							where to place the input data.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT ScriptGet_Wait(PTNGETINPUTDATA pTNgid)
{
#ifndef _XBOX
	PTID_SCRIPT_WAIT	pParams = (PTID_SCRIPT_WAIT) pTNgid->pvData;
	PLSTRINGLIST		pStrings = NULL;
	char*				pszTemp;


	pTNgid->dwDataSize = sizeof (TID_SCRIPT_WAIT);

	pStrings = pTNgid->pStringData->GetScriptInputDataStrings();

#pragma TODO(vanceo, "Check assignment reports")

	// Check for a message string
	if (pStrings != NULL)
	{
		// Find the key Timeout, case insensitive.
		pszTemp = pStrings->GetValueOfKey("Timeout");
		if (pszTemp == NULL)
		{
			if (pParams != NULL)
			{
				DPL(0, "There are strings, but couldn't get value of key \"Timeout\"!  Using default.", 0);
				pParams->dwSecs = 0;
			} // end if (not just retrieving size)

		} // end if (couldn't get key value)
		else if (pParams != NULL)
			pParams->dwSecs = StringToDWord(pszTemp);

		
		// Find the key Message, case insensitive.
		pszTemp = pStrings->GetValueOfKey("Message");
		if (pszTemp == NULL)
		{
			if (pParams != NULL)
			{
				DPL(0, "There are strings, but couldn't get value of key \"Message\"!  Using default.", 0);
				pParams->dwStringSize = 0;
			} // end if (not just retrieving size)

		} // end if (couldn't get key value)
		else
		{
			if (pParams == NULL)
			{
				pTNgid->dwDataSize += strlen(pszTemp) + 1;
			} // end if (just retrieve size)
			else
			{
				pParams->dwStringSize = strlen(pszTemp) + 1;
				strcpy((char*) (pParams + 1), pszTemp);
			} // end else (not just retrieving size)
		} // end else (successfully found message)

	} // end if (there was script data)
	else if (pParams != NULL)
	{
		pParams->dwSecs = 0;
		pParams->dwStringSize = 0;
	} // end else if (not just retrieving size)


	if (pParams == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
#else // ! XBOX
#pragma BUGBUG(tristanj, "Adding Wait case back so that the checksums match, but we don't support it")
	return E_FAIL;
#endif // XBOX
} // ScriptGet_Wait
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"ScriptExec_Wait()"
//==================================================================================
// ScriptExec_Wait
//----------------------------------------------------------------------------------
//
// Description: Callback that executes the test case(s):
//				WaitForUserOK	Built-in script: WaitForUserOK
//
// Arguments:
//	PTNEXECCASEDATA pTNecd	Pointer to structure with parameters for test case.
//
// Expected input data:
//	Number of seconds to timeout (optional).
//
// Output data: None.
//
// Dynamic variables set: None.
//
// Returns: S_OK if the act of running the test was successful (not whether the
//			test itself was successful), the error code otherwise.
//==================================================================================
HRESULT ScriptExec_Wait(PTNEXECCASEDATA pTNecd)
{
#ifndef _XBOX
	HRESULT				hr;
	PTID_SCRIPT_WAIT	pInput = (PTID_SCRIPT_WAIT) pTNecd->pvInputData;
	INT_PTR				iMsgBoxResult;


	if ((pInput == NULL) || (pInput->dwSecs == 0) || (pInput->dwSecs == INFINITE))
	{
		//DPL(0, "Built-in script: WaitForUserOK;  WARNING: Displaying dialog box without timeout.", 0);
		pTNecd->pExecutor->Log(TNLF_IMPORTANT | TNLF_PREFIX_TESTUNIQUEID,
								"NOTE: Displaying dialog box without timeout.",
								0);
	} // end if (we're not being timed)
	else
	{
		/*
		DPL(0, "Built-in script: WaitForUserOK;  Displaying dialog box with timeout of %u seconds.",
			1, pData->dwSecs);
		*/
		pTNecd->pExecutor->Log(TNLF_VERBOSE | TNLF_PREFIX_TESTUNIQUEID,
								"Displaying dialog box with timeout of %u seconds.",
								1, pInput->dwSecs);
	} // end else (we're being timed)


	iMsgBoxResult = DialogBoxParam(s_hInstance, MAKEINTRESOURCE(IDD_WAITFORUSEROK), NULL,
				                   WaitForUserOKDlgProc, (LPARAM) pInput);


	// Of course this function always is successful.

	hr = pTNecd->pFinalResult->SetResultCodeAndBools(S_OK, TRUE, TRUE);
	if (hr != S_OK)
	{
		DPL(0, "Setting test results failed!", 0);
	} // end if (setting results failed)

	return (hr);
#else // ! XBOX
#pragma BUGBUG(tristanj, "Adding Wait case back so that the checksums match, but we don't support it")
	return E_FAIL;
#endif // XBOX
} // ScriptExec_Wait
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"WaitForUserOKDlgProc()"
//==================================================================================
// WaitForUserOKDlgProc
//----------------------------------------------------------------------------------
//
// Description: WaitForUserOK dialog window procedure.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK WaitForUserOKDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DWORD	dwSecs = 0;

	char			szTemp[32];


	switch (uMsg)
	{
		case WM_INITDIALOG:
			PTID_SCRIPT_WAIT	pData;
			
			
			pData = (PTID_SCRIPT_WAIT) lParam;


			if ((pData != NULL) && (pData->dwStringSize > 0))
			{
				SetWindowText(GetDlgItem(hWnd, IDS_DESCRIPTION), (char*) (pData + 1));
			} // end if (there's a custom string)


			if ((pData == NULL) || (pData->dwSecs == 0) || (pData->dwSecs == INFINITE))
			{
				DestroyWindow(GetDlgItem(hWnd, IDS_TONLY_TIMERDESC));
				DestroyWindow(GetDlgItem(hWnd, IDT_TONLY_TIMER));
			} // end if (we're not being timed)
			else
			{
				dwSecs = pData->dwSecs;

				if (dwSecs != 1)
					wsprintf(szTemp, "%i seconds.", dwSecs);
				else
					strcpy(szTemp, "1 second.");


				SetWindowText(GetDlgItem(hWnd, IDT_TONLY_TIMER), szTemp);

				SetTimer(hWnd, 1, 1000, NULL);
			} // end else (we're being timed)
		  break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					DPL(0, "User pressed 'OK'.", 0);

					KillTimer(hWnd, 1);
					EndDialog(hWnd, IDOK);
				  break;
			} // end switch (on the button pressed/control changed)
		  break;

		case WM_TIMER:
			dwSecs--;

			if (dwSecs > 0)
			{
				if (dwSecs != 1)
					wsprintf(szTemp, "%i seconds.", dwSecs);
				else
					strcpy(szTemp, "1 second.");

				SetWindowText(GetDlgItem(hWnd, IDT_TONLY_TIMER), szTemp);

				SetTimer(hWnd, 1, 1000, NULL);
			} // end if (time limit has not been reached)
			else
			{
				KillTimer(hWnd, 1);
				EndDialog(hWnd, IDOK);
			} // end else  (time limit has been reached)

			return (TRUE);
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
} // WaitForUserOKDlgProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX