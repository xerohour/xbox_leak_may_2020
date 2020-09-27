#include "std.h"
#include "XApp.h"
#include "Node.h"

#ifdef _DEBUG

bool g_bDumpMemState;
int g_nFrameCount;
long _dbgBreakAlloc = -1;

int __cdecl DebugAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char* szFileName, int nLine)
{
	if (lRequest == _dbgBreakAlloc)
		_CrtDbgBreak();

	return TRUE;
}

const TCHAR* GetFacilityName(int nFacility)
{
	const TCHAR* szFacility;

	switch (nFacility)
	{
	default:
		szFacility = _T("Unknown Facility");
		break;

	case FACILITY_NULL:
		szFacility = _T("null");
		break;

	case FACILITY_RPC:
		szFacility = _T("RPC");
		break;

	case FACILITY_DISPATCH:
		szFacility = _T("Dispatch");
		break;

	case FACILITY_STORAGE:
		szFacility = _T("Storage");
		break;

	case FACILITY_ITF:
		szFacility = _T("ITF");
		break;

	case FACILITY_WIN32:
		szFacility = _T("Win32");
		break;

	case FACILITY_WINDOWS:
		szFacility = _T("Windows");
		break;

	case FACILITY_SSPI:
		szFacility = _T("SSPI");
		break;

	case FACILITY_CONTROL:
		szFacility = _T("Control");
		break;

	case FACILITY_CERT:
		szFacility = _T("Cert");
		break;

	case FACILITY_INTERNET:
		szFacility = _T("Internet");
		break;

	case FACILITY_MEDIASERVER:
		szFacility = _T("Media Server");
		break;

	case FACILITY_MSMQ:
		szFacility = _T("MSMQ");
		break;

	case FACILITY_SETUPAPI:
		szFacility = _T("Setup API");
		break;

	case 0x15: // _FACDPV
		szFacility = _T("DirectPlayVoice");
		break;

	case 0x876: // _FACDD, _FACD3D
		szFacility = _T("Direct3D");
		break;

	case 0x877: // _FACDP
		szFacility = _T("DirectPlay");
		break;

	case 0x878: // _FACDS
		szFacility = _T("DirectSound");
		break;
	}

	return szFacility;
}

void XAppGetErrorString(HRESULT hr, TCHAR* szErrorBuf, int cchErrorBuf)
{
	int nCode = HRESULT_CODE(hr);
	int nFacility = HRESULT_FACILITY(hr);

	const TCHAR* szFacility = GetFacilityName(nFacility);
	_tcsncpy(szErrorBuf, szFacility, cchErrorBuf);
	int cchFacility = _tcslen(szFacility);
	szErrorBuf += cchFacility;
	cchErrorBuf -= cchFacility;

	if (cchErrorBuf > 2)
	{
		*szErrorBuf++ = ':';
		*szErrorBuf++ = ' ';
		cchErrorBuf -= 2;
	}

#ifdef _LAN
	if (nFacility == FACILITY_INTERNET)
	{
		extern void GetInternetErrorString(int nCode, TCHAR* szBuf, int cchBuf);
		GetInternetErrorString(nCode, szErrorBuf, cchErrorBuf);
		return;
	}
#endif

	if (nFacility == FACILITY_WIN32)
	{
#ifndef _XBOX
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, nCode, 0, szErrorBuf, cchErrorBuf, NULL) == 0)
#endif
			_stprintf(szErrorBuf, _T("Lookup %d in winerror.h!"), nCode);
		return;
	}

	D3DXGetErrorString(hr, szErrorBuf, cchErrorBuf);
}

const TCHAR* XAppGetErrorString(HRESULT hr)
{
	static TCHAR szBuf [100];
	XAppGetErrorString(hr, szBuf, countof(szBuf));
	return szBuf;
}

void LogComError(HRESULT hr, const char* szFunc/*= NULL*/)
{
	TCHAR szError [100];
	XAppGetErrorString(hr, szError, countof(szError));

#ifdef _UNICODE
	TCHAR szFunction [100];
	Unicode(szFunction, szFunc, 100);

	Trace(_T("\001Error in function: %s\n\001%s\n"), szFunc == NULL ? _T("unknown") : szFunction, szError);
#else
	Trace(_T("\001Error in function: %s\n\001%s\n"), szFunc == NULL ? _T("unknown") : szFunc, szError);
#endif
}

void LogError(const char* szFunc)
{
	Trace(_T("\001Error in function: %s\n"), szFunc);
}

extern "C" bool AssertFailed(const TCHAR* szFile, int nLine, HRESULT hr)
{
	TCHAR szBuffer [256];

	if (hr == 0)
	{
		_stprintf(szBuffer, _T("Assertion failed in %s at line %d (last error code is 0x%x)."), szFile, nLine, GetLastError());
	}
	else
	{
		TCHAR szComError [100];
		XAppGetErrorString(hr, szComError, countof(szComError));
		_stprintf(szBuffer, _T("COM Failure in %s at line %d.\n\nFacility: %d %s\nCode %d (0x%04x)\n\n%s"),
			szFile, nLine, HRESULT_FACILITY(hr), GetFacilityName(HRESULT_FACILITY(hr)), HRESULT_CODE(hr), HRESULT_CODE(hr), szComError);
	}

#ifdef _XBOX
	Trace(_T("\001XApp: %s\n"), szBuffer);
	return true;
#else
	_tcscat(szBuffer, _T("\n\nDo you want to debug?"));
	return XAppMessageBox(szBuffer, MB_YESNO) == IDYES;
#endif
}

#ifdef _XBOX
bool bShowComments = false;
#endif

extern "C" void Trace(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

#ifdef _XBOX
	if (*szMsg > 0 && *szMsg < 8)
		szMsg += 1; // skip over message type (color) specifier
	else if (!bShowComments)
		return;
#endif

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, countof(szBuffer), szMsg, args);

	const TCHAR* szODS = szBuffer;
	OutputDebugString(szODS);

	va_end(args);
}

void Debug_Init()
{
	TRACE(_T("This is a debug version of XApp!\n"));

	_CrtSetAllocHook(DebugAllocHook);
}

void Debug_Exit()
{
	_CrtDumpMemoryLeaks();
}

void DumpHex(const BYTE* pbData, int cbData, int cbMax/*=0*/)
{
	TCHAR szBuf [256];
	TCHAR szBuf2 [32];

	bool bTruncated = false;
	if (cbMax != 0 && cbData > cbMax)
	{
		bTruncated = true;
		cbData = cbMax;
	}

	TCHAR* pch = szBuf;
	TCHAR* pch2 = szBuf2;
	for (int i = 0; i < cbData; i += 1)
	{
		BYTE b = *pbData++;

		pch += _stprintf(pch, _T("%02x "), b);

		if (b >= ' ' && b < 128)
			*pch2++ = b;
		else
			*pch2++ = '.';

		if ((i & 15) == 15 || i == cbData - 1)
		{
			*pch2 = '\0';
			Trace(_T("%-48s %s\n"), szBuf, szBuf2);
			pch = szBuf;
			pch2 = szBuf2;
		}
	}

	if (bTruncated)
		Trace(_T("...\n"));
}

void Debug_Frame()
{
	g_nFrameCount += 1;

	static XTIME nextHeapCheckTime;
	if (XAppGetNow() < nextHeapCheckTime)
		return;

	nextHeapCheckTime = XAppGetNow() + 5.0f; // every 5 seconds...


	// Check heap consistency...
	switch (_heapchk())
	{
	case _HEAPBADBEGIN:
		TRACE(_T("\001HEAPCHK: Initial header information is bad or cannot be found\n"));
		_CrtDbgBreak();
		break;

	case _HEAPBADNODE:
		TRACE(_T("\001HEAPCHK: Bad node has been found or heap is damaged\n"));
		_CrtDbgBreak();
		break;

	case _HEAPBADPTR:
		TRACE(_T("\001HEAPCHK: Pointer into heap is not valid\n"));
		_CrtDbgBreak();
		break;

	case _HEAPEMPTY:
		TRACE(_T("\001HEAPCHK: Heap has not been initialized\n"));
		_CrtDbgBreak();
		break;

	case _HEAPOK:
		break;
	}


#if 0
	// BLOCK: Compute and dump heap size...
	{
		DWORD dwTotalAlloc = 0;		
		_HEAPINFO hinfo;
		hinfo._pentry = NULL;
		while (_heapwalk(&hinfo) == _HEAPOK)
		{
			if (hinfo._useflag == _USEDENTRY)
				dwTotalAlloc += hinfo._size;
		}

		TRACE(_T("Heap size: %dKB (%d bytes)\n"), ((dwTotalAlloc + 1023) / 1024), dwTotalAlloc);
	}

	// BLOCK: Dump any leaked objects...
	{
		static _CrtMemState memState;

		if (g_bDumpMemState)
		{
			_CrtMemDumpAllObjectsSince(&memState);
			g_bDumpMemState = false;
		}

		_CrtMemCheckpoint(&memState);

#ifdef _XBOX
//		g_bDumpMemState = true;
#endif
	}
#endif
}


void DumpRegisteredClasses()
{
	TRACE(_T("Registered Classes:\n"));
	for (CNodeClass* pClass = CNodeClass::c_pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		TRACE(_T("\t%s\n"), pClass->m_szClassName);
	}
}

#endif

