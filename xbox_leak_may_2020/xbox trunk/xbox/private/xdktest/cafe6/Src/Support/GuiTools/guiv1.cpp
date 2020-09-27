/////////////////////////////////////////////////////////////////////////////
// guiv1.cpp
//
// email	date		change
// briancr	12/05/94	created
//
// copyright 1994 Microsoft

// Backward compatibility

// REVIEW(briancr): these objects are declared as globals for backward compatibility

#include "stdafx.h"
#include "guiv1.h"
#include "testxcpt.h"
#include "testutil.h"
#include "guitarg.h"

#define new DEBUG_NEW

// REVIEW(briancr): this is just to make things work. Is there a better way to do this?
// provide for a global log pointer
GUI_DATA CLog *gpLog;
// provide a global cmd line settings object
GUI_DATA CSettings *gpsettingsCmdLine;

//REVIEW(michma): this is a work-around to allow suite dlls to call
// GetUserTargetPlatforms (in shl.dll) while the test lists are being
// processed at load time.  GetUserTargetPlatforms() used to use a
// COConnection object for its work, but now COConnection is initialized
// by an IDE object which doesn't exist while suite dlls are being loaded.

GUI_DATA CString gplatformStr;

// REVIEW(briancr): we currently store the support system's language in this
// global. In the future, after the support system hierarchy is reworked,
// the support system will be able to query the toolset to find out the langauge
// and this global won't be needed anymore
int glang;
// handle to the dll that contains the localized ide strings.
HINSTANCE ghLangDll;

GUI_API int GetLang(void)
{
	return glang;
}

GUI_API void SetLang(int lang)
{
	glang = lang;
}

GUI_API HINSTANCE GetLangDllHandle(void)
{
	return ghLangDll;
}

GUI_API void SetLangDllHandle(HINSTANCE hLangDll)
{
	ghLangDll = hLangDll;
}

GUI_API int GetSysLang(void)
{
	switch (GetSystem()) {
		case SYSTEM_JAPAN: {
			return CAFE_LANG_JAPANESE;
			break;
		}
		default: {
			return CAFE_LANG_ENGLISH;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// hot key global variables and functions

GUI_DATA HANDLE g_hSingleStep = NULL;
GUI_DATA HANDLE g_hUserAbort = NULL;
GUI_DATA HWND g_hwndHotKey = NULL;
GUI_DATA EHotKey g_hkToDo = HOTKEY_RUN;
GUI_DATA int g_stopStep = -1;

// BEGIN_HELP_COMMENT
// Function: void WaitStepInstructions(LPCSTR szFmt, ...)
// Description: Put a message in the single step window. Wait for the next step command from the user if in single step mode.
// Return: <none>
// Param: szFmt A pointer to a string that contains the format specifiers for the remaining parameters. The format of this string is the same as the format-specifier string passed to printf.
// Param: ... A variable number of arguments that are expanded as specified in the format-specifier string.
// END_HELP_COMMENT
GUI_API void WaitStepInstructions( LPCSTR szFmt, ... )

{
	// this is to help alleviate timing problems.
	Sleep(CMDLINE->GetIntValue("StepSleep", 0));

    static int nStepNum = 0;

    if (!strcmp(szFmt, INITIALIZE_STRING))
        nStepNum = 0;

	if( szFmt != NULL )				// post notice of what next step will do
	{
		char szBuf[256], szFmtCpy[256] ;
		CString szTestNm, stepNum;
		strcpy(szFmtCpy,szFmt) ;
		int i = 0;
		if (!gpLog->GetTestName().IsEmpty())
		{
			i += sprintf(szBuf, "%s", (LPCSTR)gpLog->GetTestName());
			if(g_stopStep>nStepNum)
	        	i += sprintf(&szBuf[i], "(*%d)", g_stopStep); //mark the breakpoint
        	i += sprintf(&szBuf[i], " %d: ", ++nStepNum);
			i += sprintf( &szBuf[i], " F=%d ", gpLog->GetTotalFailCount() + gpLog->GetTestFailCount());
			szTestNm = szBuf ;
		}
		va_list marker;
		va_start( marker, szFmt );
		_vsnprintf( &szBuf[i], 255 - i, szFmt, marker );
	   	va_end( marker );

		SetWindowText(g_hwndHotKey, szBuf);	  
		// Record the key stroke without the test information.
		if (szTestNm.IsEmpty())
			gpLog->RecordTestKeys("%s",szBuf);
		else 
		{
//			stepNum.Format("%d",nStepNum);stepNum += "\t" ;
			stepNum.Format("%-6d",nStepNum); //chriskoz: /t caused paint problem
			CString szlogTxt, szTmp = szBuf ;
			int iKeyLen = szTmp.GetLength() - szTestNm.GetLength() ;
			szlogTxt = stepNum+	szTmp.Right(iKeyLen);
			gpLog->RecordTestKeys("%s",szlogTxt.GetBuffer(szlogTxt.GetLength()));
		} 
	}

	HANDLE ahObjects[2];
	ahObjects[0] = g_hSingleStep;
	ahObjects[1] = g_hUserAbort;
	if( g_hkToDo == HOTKEY_RUN && g_stopStep>nStepNum)
		g_hkToDo = HOTKEY_SINGLE; //force it to be single step when breakpoint set
	if( g_hkToDo == HOTKEY_SINGLE && g_stopStep>nStepNum)
		SetEvent(g_hSingleStep); // do not wait if breakpoint set
	DWORD dwObject = WaitForMultipleObjects(2, ahObjects, FALSE, INFINITE);	// wait for hot key
	if( dwObject - WAIT_OBJECT_0 == 1 )	// index 1 in ahObjects array is UserAbort
		TEST_EXCEPTION(causeGeneric);		// throws exception
	if(g_stopStep-1>nStepNum)
		g_hkToDo = HOTKEY_SINGLE; //set single mode for breakpoint
	if( g_hkToDo == HOTKEY_SINGLE && g_stopStep-1<=nStepNum)	// if we're single stepping
	{	
		ResetEvent(g_hSingleStep);		// reset event so we wait at next step
		SetWindowText(g_hwndHotKey, "Running...");
	}

	if (WaitForInputIdle(g_hTargetProc, 60000) == WAIT_TIMEOUT)
		gpLog->RecordInfo("WaitForInputIdle timed out in WaitStepInstructions()");
}

