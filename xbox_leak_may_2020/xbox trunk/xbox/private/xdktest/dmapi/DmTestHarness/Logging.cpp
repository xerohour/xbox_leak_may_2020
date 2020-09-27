//
// Automation Logging Functions
//

#include "logging.h"
#ifdef _XBOX
#include <xbdm.h>
#else
#include <xboxdbg.h>
#endif

/*

  Log format

  level,info

  000,Title
  000,Build # / Retail/Debug
  000,Run time date
  000,Start time

  002,Status messages 
  001,"Function","Test Case Title","PASS/FAIL","Error Message"

  000,End Time
  000,Total run time




  To do:

  Cache Test title for Begin/End
  Allow nested Begin/End Logging
  Re-write logging code as a C++ class
*/


//
// LogWrite
//
// Helper function to write Log data in the correct CSV format
//
void LogWrite(int level, char * str)
{
	char buf[2048];

	wsprintf(buf, "%03d,%s\n", level, str);
	OutputDebugString(buf);
}

//
// LogTestResult
//
// Used to log the result after each test pass
//
bool LogTestResult(char * Title, bool Pass, char * Err)
{
	char buf[1024];

#ifndef VERBOSE
	if (Pass)
		return Pass;
#endif
	wsprintf(buf, "\"%s\",\"%s\",\"%s\"", 
		Title ? Title : "", 
		Pass ? "PASS" : "FAIL", 
		Err ? Err : ""
		);
	LogWrite(LOG_RESULTS, buf);
	return Pass;
}

HRESULT LogVerifyHResult(HRESULT hrActual, char * Title, HRESULT hrExpected)
{
	char buf[1024];
	char hrExpectedTrans[256];
	char hrActualTrans[256];

	if (hrExpected != XBDM_NOERR)
		DmTranslateError(hrExpected, hrExpectedTrans, sizeof hrExpectedTrans);
	else
		strcpy(hrExpectedTrans, "XBDM_NOERR");

	if (hrActual != XBDM_NOERR)
		DmTranslateError(hrActual, hrActualTrans, sizeof hrActualTrans);
	else
		strcpy(hrActualTrans, "XBDM_NOERR");

#ifndef VERBOSE
	if (hrExpected == hrActual)
		return hrActual;
#endif
	wsprintf(buf, "\"%s\",\"%s\", \"Expected %08X (%s), Received %08X (%s)\"", 
		Title ? Title:"", 
		hrExpected == hrActual ? "PASS" : "FAIL", 
		hrExpected,
		hrExpectedTrans,
		hrActual,
		hrActualTrans
		);
	LogWrite(LOG_RESULTS, buf);
	return hrActual;
}

HRESULT LogVerifyHResultAlt(HRESULT hrActual, char * Title, HRESULT hrExpected, HRESULT hrAlternate)
{
	char buf[1024];
	char hrExpectedTrans[256];
	char hrActualTrans[256];
	char hrAlternateTrans[256];

	if (hrExpected != XBDM_NOERR)
		DmTranslateError(hrExpected, hrExpectedTrans, sizeof hrExpectedTrans);
	else
		strcpy(hrExpectedTrans, "XBDM_NOERR");

	if (hrActual != XBDM_NOERR)
		DmTranslateError(hrActual, hrActualTrans, sizeof hrActualTrans);
	else
		strcpy(hrActualTrans, "XBDM_NOERR");

	if (hrAlternate != XBDM_NOERR)
		DmTranslateError(hrAlternate, hrAlternateTrans, sizeof hrAlternateTrans);
	else
		strcpy(hrAlternateTrans, "XBDM_NOERR");

#ifndef VERBOSE
	if ((hrExpected == hrActual) || (hrAlternate == hrActual))
		return hrActual;
#endif
	wsprintf(buf, "\"%s\",\"%s\", \"Expected %08X (%s) or %08X (%s), Received %08X (%s)\"", 
		Title ? Title:"", 
		(hrExpected == hrActual) || (hrAlternate == hrActual) ? "PASS" : "FAIL", 
		hrExpected,
		hrExpectedTrans,
		hrAlternate,
		hrAlternateTrans,
		hrActual,
		hrActualTrans
		);
	LogWrite(LOG_RESULTS, buf);
	return hrActual;
}

//
// LogAPIError
//
// Helper function to log ongoing API error codes
//
void LogAPIError(HRESULT hr)
{
	char errortext[256];
	char buf[1024];

	DmTranslateError(hr, errortext, 256);
	wsprintf(buf, "\"APIERROR\",\"0x%08x\",\"%s\"", hr, errortext);
	LogWrite(LOG_NOTES, buf);
}

void LogTestResultAPI(char * Title, HRESULT hr)
{
	char errortext[256] = {""};

	DmTranslateError(hr, errortext, 256);
	LogTestResult(Title, (hr == XBDM_NOERR), errortext);
}
