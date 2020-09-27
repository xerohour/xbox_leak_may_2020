//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"

#include "sendq.h"
#include "comm.h"
#include "control.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNControlLayer::CTNControlLayer()"
//==================================================================================
// CTNControlLayer constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNControlLayer object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNControlLayer::CTNControlLayer(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNControlLayer));

	this->m_pfnLogString = NULL;
	this->m_pCtrlComm = NULL;
	this->m_hCompletionOrUpdateEvent = NULL;
} // CTNControlLayer::CTNControlLayer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNControlLayer::~CTNControlLayer()"
//==================================================================================
// CTNControlLayer destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNControlLayer object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNControlLayer::~CTNControlLayer(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pCtrlComm != NULL)
	{
		this->m_pCtrlComm->Release(); // ignoring error
		delete (this->m_pCtrlComm);
		this->m_pCtrlComm = NULL;
	} // end if (there is still a control method object)
} // CTNControlLayer::~CTNControlLayer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNControlLayer::Log()"
//==================================================================================
// CTNControlLayer::Log
//----------------------------------------------------------------------------------
//
// Description: Logs the passed in string to the shell, noting whether this is
//				critical, normal or verbose information.  The string will be
//				parsed for special tokens, see tncommon\sprintf.cpp for possible
//				arguments.
//
// Arguments:
//	DWORD dwLogStringType	What significance does the string have (see TNLST_xxx).
//	char* szFormatString	String with optional special tokens to log.
//	DWORD dwNumParms		Number of parameters in the following variable parameter
//							list.
//	...						Variable parameter list; items are interpreted as the
//							special token replacements.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNControlLayer::Log(DWORD dwLogStringType, char* szFormatString,
							DWORD dwNumParms, ...)
{
	HRESULT		hr;
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp;
	char*		pszLogString = NULL;


	// If the shell doesn't have a log function
	if (this->m_pfnLogString == NULL)
	{
		//BUGBUG is this what we want?
		hr = S_OK;
		goto DONE;
	} // end if (no log function)

	
	if (dwNumParms == 0)
	{
#ifndef _XBOX
		hr = this->m_pfnLogString(dwLogStringType, szFormatString);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't log string!", 0);
		} // end if (shell's log string function failed)
#else // ! XBOX
#pragma TODO(tristanj, "Map this logging to Xbox's logging subsystem")
#endif // XBOX

		goto DONE;
	} // end if (there are format items)


#ifndef _XBOX // no TNsprintf functions
	papvParms = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
	if (papvParms == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)


	// Loop through the variable arguments and put them into an array for passing
	// to sprintf.
	va_start(currentparam, dwNumParms);
	for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
	{
		papvParms[dwTemp] = va_arg(currentparam, PVOID);
	} // end for (each variable parameter)
	va_end(currentparam);

	TNsprintf_array(&pszLogString, szFormatString, dwNumParms, papvParms);

	hr = this->m_pfnLogString(dwLogStringType, pszLogString);
	if (hr != S_OK)
	{
		DPL(0, "Shell's LogString function failed!", 0);
		goto DONE;
	} // end if (shell's log string function failed)
#else // ! XBOX
#pragma TODO(tristanj, "Need to map control layer logging to xLog")
#endif // XBOX


DONE:

	if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (have array)

	if (pszLogString != NULL)
		TNsprintf_free(&pszLogString);

	return (hr);
} // CTNControlLayer::Log
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

