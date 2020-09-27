//
// Debug API - Socket Functions
//

#include "tests.h"
#include "logging.h"

static void testDmQueryPerformanceCounter();
static void testDmWalkPerformanceCounters();
static void testDmCloseCounters();
static void testDmEnableGPUCounter();
static void testDmScreenShot();
static void testDmPixelShaderSnapshot();
static void testDmVertextShaderSnapshot();

void testPerfCounters()
{
	testDmQueryPerformanceCounter();
	testDmWalkPerformanceCounters();
	testDmCloseCounters();
	testDmEnableGPUCounter();
	testDmScreenShot();
	testDmPixelShaderSnapshot();
	testDmVertextShaderSnapshot();
}

void testDmQueryPerformanceCounter()
{
#ifndef _XBOX
	HRESULT				hr;
	PDM_WALK_COUNTERS	walk_counters = NULL;
	DM_COUNTINFO		counter;
	DM_COUNTDATA		data;

	VERIFY_HRESULT(DmUseSharedConnection(TRUE), XBDM_NOERR);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% CPU:total", 0x21, &data), XBDM_NOERR);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% CPU:TOTAL", 0x21, &data), XBDM_NOERR);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% CPU:total",    0, &data), E_INVALIDARG);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% CPU:total", 0x21,  NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmQueryPerformanceCounter(      "BOGUS", 0x21, &data), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmQueryPerformanceCounter(           "", 0x21, &data), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmQueryPerformanceCounter(         NULL, 0x21, &data), E_INVALIDARG);

	VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	while (SUCCEEDED(hr) && (hr != XBDM_ENDOFLIST))
	{
		VERIFY_HRESULT_ALT(DmQueryPerformanceCounter(counter.Name, counter.Type, &data), XBDM_NOERR, XBDM_COUNTUNAVAILABLE);

		VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	}
	VERIFY_HRESULT(DmCloseCounters(walk_counters), XBDM_NOERR);
#endif
}

void testDmWalkPerformanceCounters()
{
	HRESULT				hr;
	PDM_WALK_COUNTERS	walk_counters = NULL;
	DM_COUNTINFO		counter;
	char				buf[256];

	OutputDebugString("*** begin perf counter walk ***\n");
	VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	while (SUCCEEDED(hr) && (hr != XBDM_ENDOFLIST))
	{
		wsprintf(buf, "counter: %-16s type: 0x%08X\n", 
				counter.Name,
				counter.Type
				);
		OutputDebugString(buf);
		VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	}
	OutputDebugString("*** end perf counter walk ***\n");
	VERIFY_HRESULT(DmCloseCounters(walk_counters), XBDM_NOERR);
}

void testDmCloseCounters()
{
	HRESULT				hr;
	PDM_WALK_COUNTERS	walk_counters = NULL;
	DM_COUNTINFO		counter;

	// no walk, immediate close

	VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	VERIFY_HRESULT(DmCloseCounters(walk_counters), XBDM_NOERR);

	// full walk

	walk_counters = NULL;
	VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	while (SUCCEEDED(hr) && (hr != XBDM_ENDOFLIST))
	{
		VERIFY_HRESULT_ALT(DmWalkPerformanceCounters(&walk_counters, &counter), XBDM_NOERR, XBDM_ENDOFLIST);
	}
	VERIFY_HRESULT(DmCloseCounters(walk_counters), XBDM_NOERR);

	// null and ptr to null

	VERIFY_HRESULT(DmCloseCounters(NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmCloseCounters(walk_counters = NULL), E_INVALIDARG);
}

void testDmEnableGPUCounter()
{
	HRESULT	hr;
	DM_COUNTDATA gpu, frontend, backend;

	VERIFY_HRESULT(DmEnableGPUCounter(TRUE), XBDM_NOERR);
	// enable when already enabled
	VERIFY_HRESULT(DmEnableGPUCounter(TRUE), XBDM_NOERR);

	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU", 0x21, &gpu), XBDM_NOERR);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU Frontend", 0x21, &frontend), XBDM_NOERR);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU Backend", 0x21, &backend), XBDM_NOERR);

	VERIFY_HRESULT(DmEnableGPUCounter(FALSE), XBDM_NOERR);

	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU", 0x21, &gpu), XBDM_COUNTUNAVAILABLE);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU Frontend", 0x21, &frontend), XBDM_COUNTUNAVAILABLE);
	VERIFY_HRESULT(DmQueryPerformanceCounter("% GPU Backend", 0x21, &backend), XBDM_COUNTUNAVAILABLE);

	// disable already disabled
	VERIFY_HRESULT(DmEnableGPUCounter(FALSE), XBDM_NOERR);
}

void testDmScreenShot()
{
#ifndef _XBOX
	HRESULT hr;
	char szTempPath[MAX_PATH];

	DWORD dwNeeded = GetEnvironmentVariableA("TEMP", szTempPath, sizeof szTempPath);
	if (!dwNeeded || (dwNeeded > sizeof szTempPath))
		return;
	strcat(szTempPath, "\\scrnshot.bmp");

	VERIFY_HRESULT(DmScreenShot(szTempPath), XBDM_NOERR);
	HANDLE hScreenShot = CreateFile(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	VERIFY_TRUE(hScreenShot != INVALID_HANDLE_VALUE);
	VERIFY_TRUE(GetFileSize(hScreenShot, NULL) != 0);
	CloseHandle(hScreenShot);
	DeleteFile(szTempPath);

	// reuse filename
	VERIFY_HRESULT(DmScreenShot(szTempPath), XBDM_NOERR);
	VERIFY_HRESULT(DmScreenShot(szTempPath), XBDM_NOERR);
	hScreenShot = CreateFile(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	VERIFY_TRUE(hScreenShot != INVALID_HANDLE_VALUE);
	VERIFY_TRUE(GetFileSize(hScreenShot, NULL) != 0);
	CloseHandle(hScreenShot);
	DeleteFile(szTempPath);

	VERIFY_HRESULT(DmScreenShot(NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmScreenShot(""), XBDM_FILEERROR);
	VERIFY_HRESULT(DmScreenShot("C:\\Invalid+File%Name|.bmp"), XBDM_FILEERROR);
	VERIFY_HRESULT(DmScreenShot("C:\\PathThatDoesntExist\\scrnshot.bmp"), XBDM_FILEERROR);
	VERIFY_HRESULT(DmScreenShot("\\\\xdk\\xdktest\\scrnshot.bmp"), XBDM_NOERR);
#endif
}

void testDmPixelShaderSnapshot()
{
#ifndef _XBOX
	// TODO: DmPixelShaderSnapshot - figure out
#endif
}

void testDmVertextShaderSnapshot()
{
#ifndef _XBOX
	// TODO: DmVertexShaderSnapshot - figure out
#endif
}

