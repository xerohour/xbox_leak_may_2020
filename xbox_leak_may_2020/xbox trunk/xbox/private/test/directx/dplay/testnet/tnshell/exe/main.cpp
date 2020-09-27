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

#include "..\dll\version.h"
#include "..\dll\resource.h"
#include "..\dll\tnshell.h"
#include "..\dll\main.h"
#include "..\dll\prefs.h"
#include "..\dll\select.h"

#include "xlog.h"

#ifdef _XBOX // handle for starting and stopping test
HANDLE  hStopEvent = NULL;
HANDLE  hLog = NULL;
#endif


//==================================================================================
// wmain
//----------------------------------------------------------------------------------
//
// Description: Entry point for Xbox executable
//
// Arguments:
//	INT    argc			Number of parameters passed in.
//  WCHAR  *argvW		Parameter list
// Returns: S_OK if no failure
//==================================================================================
int _cdecl wmain(INT argc, WCHAR *argvW[])
{
	HRESULT			hr = S_OK;
	DWORD			dwError = 0;
	BOOL			fNetInitialized = FALSE;


    // Create the xLog log object
   hLog = xCreateLog(L"z:\\tnshell.log",
					NULL, 
					INVALID_SOCKET, 
					XLL_LOGALL, 
					XLO_DEBUG | XLO_STATE | XLO_REFRESH);

	if((hLog == INVALID_HANDLE_VALUE) || (hLog == NULL))
	{
		dwError = GetLastError();
		xLog(hLog, XLL_FAIL, "Failed to create xLog: 0x%x(%d)", dwError, dwError);
		goto DONE;
	}

	// Initialize the Xbox networking libraries
	if(NO_ERROR != XnetInitialize(NULL, TRUE))
	{
		dwError = GetLastError();
		xLog(hLog, XLL_FAIL, "Failed to initialize Xnet");
		goto DONE;
	}
	fNetInitialized = TRUE;

	// Run the test
	StartTest(hLog);

	// Clean up
	EndTest();
	
DONE:
	
    // Close the xLog log object
    xCloseLog(hLog);

	if(fNetInitialized)
		XnetCleanup();

	return (hr);
}
