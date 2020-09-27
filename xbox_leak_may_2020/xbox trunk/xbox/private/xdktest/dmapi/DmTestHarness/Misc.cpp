//
// Debug API - Miscellaneous Functions
//

#include "tests.h"
#include "logging.h"
#include <wchar.h>

void testBreakpoint()
{
	LogNote("*** testBreakpoint not ready for primetime ***");
	return;
	HRESULT	hr;
/*
	hr = DmSetBreakpoint(0);					// success
	hr = DmSetBreakpoint((void *)0x0040000);	// success
	hr = DmSetBreakpoint((void *)0xFFFFFFF);	// success

	hr = DmRemoveBreakpoint(0);					// success
	hr = DmRemoveBreakpoint((void *)0x0040000);	// success
	hr = DmRemoveBreakpoint((void *)0x0040000);	// 2nd time should fail (XBDM_UNDEFINED)
	hr = DmRemoveBreakpoint((void *)0xFFFFFFF);	// success

	hr = DmSetDataBreakpoint(0, DMBREAK_READWRITE, 1);	// success
	hr = DmSetDataBreakpoint(0, DMBREAK_READWRITE, 0);	// E_INVALIDARG
	hr = DmSetDataBreakpoint((void *)0x0040000, DMBREAK_READWRITE, 1);	// success
	hr = DmSetDataBreakpoint((void *)0x0040000, DMBREAK_READWRITE, 0);	// E_INVALIDARG
*/
	hr = DmSetInitialBreakpoint();			// XBDM_UNDEFINED
}

void testExecution()
{
	LogNote("*** testExecution not ready for primetime ***");
	return;
	HRESULT	hr;

	hr = DmGo();								// XBDM_UNDEFINED
	hr = DmStop();								// success
	hr = DmStop();								// XBDM_UNDEFINED
	hr = DmGo();								// success

	hr = DmStopOn(DMSTOP_CREATETHREAD, true);
	hr = DmStopOn(DMSTOP_CREATETHREAD, false);
	hr = DmStopOn(DMSTOP_FCE, true);
	hr = DmStopOn(DMSTOP_FCE, false);
	hr = DmStopOn(DMSTOP_DEBUGSTR, true);
	hr = DmStopOn(DMSTOP_DEBUGSTR, false);
	hr = DmStopOn(9999, true);					// success ???
	hr = DmStopOn(9999, false);					// success ???
}

void testDmGetXtlData()
{
	DM_XTLDATA xtlData;

	// TODO: DmGetXtlData - get more info

	DmGetXtlData(&xtlData);
}

void testDmGetSystemTime()
{
#ifndef _XBOX
	HRESULT		hr;
	SYSTEMTIME	st;

	// Pass NULL for LPSYSTEMTIME

	VERIFY_HRESULT(DmGetSystemTime(NULL), E_INVALIDARG);

	// Verify SYSTEMTIME returned is valid

	memset(&st, 0, sizeof st);
//	wsprintf(buffer, "SYSTEMTIME: %02d/%02x/%02s %02d:%02d:%02d.%03d\n",
//		st.wMonth, st.wDay, st.wYear, st.wHour, 
//		st.wMinute, st.wSecond, st.wMilliseconds);
//	LogNote(buffer);
	VERIFY_HRESULT(DmGetSystemTime(&st), XBDM_NOERR);
#endif
}

#define TRANSLATEERRORTEST(TYPE,STRING) \
	hr = DmTranslateErrorA(TYPE, buffer, sizeof buffer); \
	if (strcmp(buffer,STRING)) \
		LogTestResult("DmTranslateErrorA(" #TYPE ", buffer, 512)", false, #TYPE " should return: " #STRING); \
	else \
		LogTestResult("DmTranslateErrorA(" #TYPE ", buffer, 512)", true, ""); \

	/*
	hr = DmTranslateErrorW(TYPE, wbuffer, sizeof wbuffer); \
	if (wcscmp(wbuffer,L##STRING)) \
		LogTestResult("DmTranslateErrorW(" #TYPE ", wbuffer, 512)", false, #TYPE " should return: " #STRING); \
	else \
		LogTestResult("DmTranslateErrorW(" #TYPE ", wbuffer, 512)", true, "");
	*/

void testDmTranslateError()
{
#ifndef _XBOX
	HRESULT		hr;
	char		buffer[512];
	WCHAR		wbuffer[512];

	// Pass NULL for lpBuffer

	VERIFY_HRESULT(DmTranslateError(XBDM_UNDEFINED, NULL, 512), E_INVALIDARG);

	// Pass 0 for nBufferMax

	VERIFY_HRESULT(DmTranslateError(XBDM_UNDEFINED, buffer, 0), E_INVALIDARG);

	// Pass 1 for nBufferMax
	// fails because of BUG 3396 (won't fix)
	// VERIFY_HRESULT(DmTranslateError(XBDM_UNDEFINED, buffer, 1), XBDM_BUFFER_TOO_SMALL);

	// Pass # > error string size for nBufferMax

	VERIFY_HRESULT(DmTranslateError(XBDM_UNDEFINED, buffer, 512), XBDM_NOERR);

	// Pass 0 for HRESULT

	VERIFY_HRESULT(DmTranslateError(0, buffer, 512), XBDM_NOERRORSTRING);

	// Pass all Error codes for HRESULT

	TRANSLATEERRORTEST(XBDM_UNDEFINED       , "Undefined network error");
	TRANSLATEERRORTEST(XBDM_MAXCONNECT      , "Maximum number of Xbox connections exceeded");
	TRANSLATEERRORTEST(XBDM_NOSUCHFILE      , "No such file exists on the Xbox");
	TRANSLATEERRORTEST(XBDM_NOMODULE        , "No such module");
	TRANSLATEERRORTEST(XBDM_MEMUNMAPPED     , "Xbox memory unmapped");
	TRANSLATEERRORTEST(XBDM_NOTHREAD        , "No such Xbox thread id");
	TRANSLATEERRORTEST(XBDM_INVALIDCMD      , "Unexpected Xbox network error");
	TRANSLATEERRORTEST(XBDM_NOTSTOPPED      , "Thread not stopped");
	TRANSLATEERRORTEST(XBDM_MUSTCOPY        , "File must be copied, not moved");
	TRANSLATEERRORTEST(XBDM_ALREADYEXISTS   , "File already exists");
	TRANSLATEERRORTEST(XBDM_DIRNOTEMPTY     , "Directory not empty");
	TRANSLATEERRORTEST(XBDM_BADFILENAME     , "Illegal filename");
	TRANSLATEERRORTEST(XBDM_CANNOTCREATE    , "Cannot create file");
	TRANSLATEERRORTEST(XBDM_CANNOTACCESS    , "Cannot access file");
	TRANSLATEERRORTEST(XBDM_DEVICEFULL      , "Device is full");
	TRANSLATEERRORTEST(XBDM_NOTDEBUGGABLE   , "Title is not debuggable");
	TRANSLATEERRORTEST(XBDM_CANNOTCONNECT   , "Cannot connect to Xbox");
	TRANSLATEERRORTEST(XBDM_CONNECTIONLOST  , "Connection to Xbox lost");
	TRANSLATEERRORTEST(XBDM_FILEERROR       , "Unexpected file error");
	TRANSLATEERRORTEST(XBDM_ENDOFLIST       , "");
	TRANSLATEERRORTEST(XBDM_BUFFER_TOO_SMALL, "");
	TRANSLATEERRORTEST(XBDM_NOTXBEFILE      , "File is not an Xbox executable image");
	TRANSLATEERRORTEST(XBDM_MEMSETINCOMPLETE, "Not all requested memory could be written");
	TRANSLATEERRORTEST(XBDM_NOXBOXNAME      , "No address for Xbox has been set");
	TRANSLATEERRORTEST(XBDM_NOERRORSTRING   , "");

	// Pass all Success codes for HRESULT

	VERIFY_HRESULT(DmTranslateError(XBDM_NOERR, buffer, 512), XBDM_NOERRORSTRING);
	VERIFY_HRESULT(DmTranslateError(XBDM_CONNECTED, buffer, 512), XBDM_NOERRORSTRING);
	VERIFY_HRESULT(DmTranslateError(XBDM_MULTIRESPONSE, buffer, 512), XBDM_NOERRORSTRING);
	VERIFY_HRESULT(DmTranslateError(XBDM_BINRESPONSE, buffer, 512), XBDM_NOERRORSTRING);
	VERIFY_HRESULT(DmTranslateError(XBDM_READYFORBIN, buffer, 512), XBDM_NOERRORSTRING);
	VERIFY_HRESULT(DmTranslateError(XBDM_DEDICATED, buffer, 512), XBDM_NOERRORSTRING);
#endif
}

void testDmGetDriveList()
{
#ifndef _XBOX
	HRESULT hr;
	char szDrives[26];
	DWORD dwSize;

	dwSize = sizeof szDrives;
	VERIFY_HRESULT(DmGetDriveList(szDrives, &dwSize), XBDM_NOERR);
	dwSize -= 1;
	VERIFY_HRESULT(DmGetDriveList(szDrives, &dwSize), XBDM_BUFFER_TOO_SMALL);
	/*
	TODO: DmGetDriveList - file bugs against these!  
	dwSize = sizeof szDrives;
	VERIFY_HRESULT(DmGetDriveList(NULL, &dwSize), E_INVALIDARG);
	dwSize = sizeof szDrives;
	VERIFY_HRESULT(DmGetDriveList(szDrives, NULL), E_INVALIDARG);
	*/
#endif
}

void testDmGetDiskFreeSpace()
{
#ifndef _XBOX
	HRESULT hr;
	char szDrives[26];
	char szDrive[4] = "A:\\";
	DWORD dwSize;
	ULARGE_INTEGER nFreeToCaller, nTotal, nTotalFree;

	dwSize = sizeof szDrives;
	DmGetDriveList(szDrives, &dwSize);
	for (unsigned int i = 0; i < dwSize; i++)
	{
		szDrive[0] = szDrives[i];
		VERIFY_HRESULT(DmGetDiskFreeSpace(szDrive, &nFreeToCaller, &nTotal, &nTotalFree), XBDM_NOERR);
		VERIFY_HRESULT(DmGetDiskFreeSpace(szDrive, NULL, &nTotal, &nTotalFree), E_INVALIDARG);
		VERIFY_HRESULT(DmGetDiskFreeSpace(szDrive, &nFreeToCaller, NULL, &nTotalFree), E_INVALIDARG);
		VERIFY_HRESULT(DmGetDiskFreeSpace(szDrive, &nFreeToCaller, &nTotal, NULL), E_INVALIDARG);
	}

	VERIFY_HRESULT(DmGetDiskFreeSpace(NULL, &nFreeToCaller, &nTotal, &nTotalFree), E_INVALIDARG);
	szDrive[0] = 'Q';
	VERIFY_HRESULT(DmGetDiskFreeSpace(szDrive, &nFreeToCaller, &nTotal, &nTotalFree), E_INVALIDARG);
#endif
}

void testDmReboot()
{
	LogNote("*** testDmReboot not ready for primetime ***");
	return;
	HRESULT	hr;

	// Pass DMBOOT_WAIT

	VERIFY_HRESULT(DmReboot(DMBOOT_WAIT), XBDM_NOERR);
	Sleep(10000);

	// Pass DMBOOT_WARM

	VERIFY_HRESULT(DmReboot(DMBOOT_WARM), XBDM_NOERR);
	Sleep(10000);
	
	// Pass invalid value for dwFlags

#define INVALID_DMBOOT_FLAGS (~(DMBOOT_WAIT|DMBOOT_WARM|DMBOOT_NODEBUG|DMBOOT_STOP))
	VERIFY_HRESULT(DmReboot(INVALID_DMBOOT_FLAGS), E_INVALIDARG);
	Sleep(10000);

	/* TODO: DmReboot - pass combo of valid flags

	TEST_BEGIN("DmReboot(DMBOOT_WARM | DMBOOT_NODEBUG)");
	hr = DmReboot(DMBOOT_WARM | DMBOOT_NODEBUG);
	VERIFY_HRESULT("DmReboot(DMBOOT_WARM | DMBOOT_NODEBUG)", hr);
	TEST_END("DmReboot(DMBOOT_WARM | DMBOOT_NODEBUG)");
	*/
}
