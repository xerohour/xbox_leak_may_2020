//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\excptns.h"
#include "..\tncommon\stack.h"
#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "excptns.h"







#ifndef _XBOX // We don't support GetModuleFileName and this function ultimately depends on it
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNHandleCatchExceptionList()"
//==================================================================================
// TNHandleCatchExceptionList
//----------------------------------------------------------------------------------
//
// Description: Handles a thrown exception list.
//
// Arguments:
//	PTNEXCEPTIONSLIST pExceptions	Pointer to exception object thrown.
//	PTNTESTRESULT pTestResult		Pointer to test result to set to the exception
//									thrown.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT TNHandleCatchExceptionList(PTNEXCEPTIONSLIST pExceptions,
									PTNTESTRESULT pTestResult)
{
	HRESULT			hr;
	PTNEXCEPTION	pException;
	TNGETSTACKDATA	gsd;
	PVOID			pvCallersCaller;
	char			szModule[256];
	char			szSection[256];
	char			szFile[256];
	int				iLineNum;
	char			szInfoString[256];
	DWORD			dwNoteAge;

	
	pException = (PTNEXCEPTION) pExceptions->GetItem(0);
	if (pException == NULL)
	{
		DPL(0, "Couldn't get first exception in list!", 0);
		return (E_FAIL);
	} // end if (couldn't get first item)
	
	hr = pException->PrintSelf();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't have most recent exception print itself!", 0);
		return (hr);
	} // end if (failed printself)

	(*pTestResult) = pException->m_dwExceptionCode;
	


	if (TNDebugGetThreadBlockNote(GetCurrentThreadId(), szModule, szSection, szFile, &iLineNum, szInfoString, &dwNoteAge))
	{
		DPL(0, "Last known position (%u ms ago) = %s:%s (%s line %i) \"%s\".",
			6, dwNoteAge, szModule, szSection, szFile, iLineNum, szInfoString);
	} // end if (got last known thread position)
	

	
	ZeroMemory(&gsd, sizeof (TNGETSTACKDATA));
	gsd.dwSize = sizeof (TNGETSTACKDATA);
	gsd.fCallStack = TRUE;
	TNStackGetCallersAddress(&(gsd.pvIgnoreCallsAboveFunction), &pvCallersCaller);
	gsd.dwRawStackDepth = 250;
	hr = TNStackGetStackForCurrentThread(&gsd);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get stack for this thread!", 0);
		return (hr);
	} // end if (failed getting the stack for the current thread)


	//BUGBUG make runtime option
	DEBUGBREAK();


	return (S_OK);
} // TNHandleCatchExceptionList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // We don't support GetModuleFileName and this function ultimately depends on it
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNHandleCatchGeneric()"
//==================================================================================
// TNHandleCatchGeneric
//----------------------------------------------------------------------------------
//
// Description: Handles a generic exception.
//
// Arguments: None.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT TNHandleCatchGeneric(void)
{
	HRESULT			hr;
	TNGETSTACKDATA	gsd;
	PVOID			pvCallersCaller;
	char			szModule[256];
	char			szSection[256];
	char			szFile[256];
	int				iLineNum;
	char			szInfoString[256];
	DWORD			dwNoteAge;
	char			szTemp[1024];



	if (TNDebugGetThreadBlockNote(GetCurrentThreadId(), szModule, szSection, szFile, &iLineNum, szInfoString, &dwNoteAge))
	{
		DPL(0, "Last known position (%u ms ago) = %s:%s (%s line %i) \"%s\".",
			6, dwNoteAge, szModule, szSection, szFile, iLineNum, szInfoString);
	} // end if (got last known thread position)
	

	ZeroMemory(&gsd, sizeof (TNGETSTACKDATA));
	gsd.dwSize = sizeof (TNGETSTACKDATA);
	gsd.fCallStack = TRUE;
	TNStackGetCallersAddress(&(gsd.pvIgnoreCallsAboveFunction), &pvCallersCaller);
	gsd.dwRawStackDepth = 250;
	hr = TNStackGetStackForCurrentThread(&gsd);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get stack for this thread!", 0);
		return (hr);
	} // end if (failed getting the stack for the current thread)

	StringGetCurrentDateStr(szTemp);
	StringPrepend(szTemp, "Exception mem log dump printed ");
	
	//BUGBUG I'm not sure I like forced memlog dump locations like this
	TNDebugDumpMemoryLogToFile("reports\\exception.txt", szTemp, TRUE);


	//BUGBUG make runtime option
	DEBUGBREAK();

	
	return (S_OK);
} // TNHandleCatchGeneric
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX

