//////////////////////////////////////////////////////////////////////
//
//	Module:		apitrace.dll
//	Author:		Galen C. Hunt
//	Copyright:	1996-1999, Microsoft Corporation
//

//////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT		0x0400
#define WIN32
#define NT

#define DBG_TRACE   0

#include <windows.h>
#include <stdio.h>
#include "detours.h"

#define PULONG_PTR			PVOID
#define PLONG_PTR			PVOID
#define ULONG_PTR			PVOID
#define LONG_PTR			PVOID
#define ENUMRESNAMEPROCA 	PVOID
#define ENUMRESNAMEPROCW 	PVOID
#define ENUMRESLANGPROCA 	PVOID
#define ENUMRESLANGPROCW 	PVOID
#define ENUMRESTYPEPROCA 	PVOID
#define ENUMRESTYPEPROCW 	PVOID

//////////////////////////////////////////////////////////////////////

#ifndef _DEFINED_ASMBREAK_
#define _DEFINED_ASMBREAK_
#define ASMBREAK()	__asm { int 3 }
//#define ASMBREAK()	DebugBreak()
#endif	// _DEFINED_ASMBREAK_


#define ASSERT_ALWAYS(x)   \
    do {                                                        \
	if (!(x)) {                                                 \
            AssertMessage(#x, __FILE__, __LINE__);              \
			ASMBREAK();                                         \
	}                                                           \
    } while (0)

#ifndef NDEBUG
#define ASSERT(x)           ASSERT_ALWAYS(x)
#else
#define ASSERT(x)
#endif

#define UNUSED(c)    (c) = (c)

//////////////////////////////////////////////////////////////////////

VOID _PrintEnter(const CHAR *psz, ...);
VOID _PrintExit(const CHAR *psz, ...);
VOID _Print(const CHAR *psz, ...);
VOID _VPrint(PCSTR msg, va_list args, PCHAR pszBuf, LONG cbBuf);

//////////////////////////////////////////////////////////////////////////////
//
#include "_win32.cpp"
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////// Logging System.
//
static PCHAR do_hex(PCHAR pszOut, DWORD dwValue)
{
	CHAR szHex[9];
	for (int nHex = 7; nHex >= 0; nHex--) {
		szHex[nHex] = "0123456789ABCDEF"[dwValue & 0xf];
		dwValue >>= 4;
	}
	for (nHex = 0; nHex < 7 && szHex[nHex] == '0'; nHex++) {
		// skip leading zeros.
	}
	for (; nHex < 8; nHex++) {
		*pszOut++ = szHex[nHex];
	}
	return pszOut;
}

static VOID _VPrint(PCSTR pszMsg, va_list args, PCHAR pszBuffer, LONG cbBuffer)
{
	PCHAR pszOut = pszBuffer;
	pszBuffer[0] = '\0';

	__try {
		while (*pszMsg) {
			if (*pszMsg == '%') {
				pszMsg++;
				
				BOOL fLarge = FALSE;
				if (*pszMsg == 'h') {
					fLarge = FALSE;
					pszMsg++;
				}
				else if (*pszMsg == 'l') {
					fLarge = TRUE;
					pszMsg++;
				}

				if (*pszMsg == 's') {
					pszMsg++;
					PCHAR pszBuf = pszOut;
					PVOID pvData = va_arg(args, PVOID);
					
					__try {
						*pszOut++ = '\"';
						if (fLarge) {
							PWCHAR pwz = (PWCHAR)pvData;

							for (int n = 0; *pwz && n < 120; n++) {
								WCHAR w = *pwz++;
								*pszOut++ = (w >= ' ' && w < 127) ? (CHAR)w : '.';
							}
						}
						else {
							PCHAR psz = (PCHAR)pvData;

							for (int n = 0; *psz && n < 120; n++) {
								CHAR c = *psz++;
								*pszOut++ = (c >= ' ' && c < 127) ? c : '.';
							}
						}
						*pszOut++ = '\"';
					} __except(EXCEPTION_EXECUTE_HANDLER) {
						pszOut = pszBuf;
						*pszOut++ = 'x';
						pszOut = do_hex(pszOut, (DWORD)pvData);
					}
				}
				else if (*pszMsg == 'x') {
					pszMsg++;
					*pszOut++ = 'x';
					pszOut = do_hex(pszOut, va_arg(args, DWORD));
				}	
				else {
					*pszOut++ = *pszMsg++;
				}
			}
			else {
				*pszOut++ = *pszMsg++;
			}
		}
		*pszOut = '\0';
		pszBuffer[cbBuffer - 1] = '\0';
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		strcpy(pszBuffer, "<exception>");
	}
}

class CLog
{
  public:
	static VOID Print(PCSTR msg, ...);
	static VOID Write(PCHAR pszBuf, LONG cbBuf);
	static VOID Close();

  protected:
	static BOOL Open();
	
	static HANDLE	s_hLogFile;
	static WCHAR	s_wzLogFile[];
};

HANDLE 	CLog::s_hLogFile = (HANDLE)-2L;
WCHAR 	CLog::s_wzLogFile[] = L"\\apitrace.log";

BOOL CLog::Open()
{
	if (s_hLogFile == (HANDLE)-2L) {
		s_hLogFile = Real_CreateFileW(s_wzLogFile,
									  GENERIC_WRITE,
									  FILE_SHARE_READ,
									  NULL,
									  CREATE_ALWAYS,
									  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
									  NULL);
	}
	return (s_hLogFile != INVALID_HANDLE_VALUE);
}

VOID CLog::Write(PCHAR pszBuf, LONG cbBuf)
{
	if (Open()) {
		DWORD cbWritten = 0;
		Real_WriteFile(s_hLogFile, pszBuf, cbBuf, &cbWritten, NULL);
	}
}

VOID CLog::Close()
{
	if (s_hLogFile != (HANDLE)-2L && s_hLogFile != INVALID_HANDLE_VALUE) {
		Real_CloseHandle(s_hLogFile);
		s_hLogFile = INVALID_HANDLE_VALUE;
	}
}

VOID CLog::Print(PCSTR pszMsg, ...)
{
	CHAR szBuf[4096];
	
	va_list args;
	va_start(args, pszMsg);
	_VPrint(pszMsg, args, szBuf, sizeof(szBuf));
	va_end(args);

	Write(szBuf, strlen(szBuf));
}

//
//////////////////////////////////////////////////////////////////////////////

static BOOL s_bLog = 1;
static LONG s_nTlsIndent = -1;
static LONG s_nTlsThread = -1;
static LONG s_nThreadCnt = 0;

VOID _PrintEnter(const CHAR *psz, ...)
{
	DWORD dwErr = GetLastError();
	
	LONG nIndent = 0;
	LONG nThread = 0;
	if (s_nTlsIndent >= 0) {
		nIndent = (LONG)TlsGetValue(s_nTlsIndent);
		TlsSetValue(s_nTlsIndent, (PVOID)(nIndent + 1));
	}
	if (s_nTlsThread >= 0) {
		nThread = (LONG)TlsGetValue(s_nTlsThread);
	}
		
	if (s_bLog) {
		CHAR szBuf[4096];
		LONG nLen = 3 + (nIndent < 35 ? nIndent * 2 : 70);
		FillMemory(szBuf, nLen, ' ');

		szBuf[0] = '0' + ((nThread / 10) % 10);
		szBuf[1] = '0' + ((nThread / 1) % 10);
		
		va_list  args;
		va_start(args, psz);
		_VPrint(psz, args, szBuf + nLen, sizeof(szBuf) - nLen);
		va_end(args);
		
		CLog::Write(szBuf, strlen(szBuf));
	}
	SetLastError(dwErr);
}

VOID _PrintExit(const CHAR *psz, ...)
{
	DWORD dwErr = GetLastError();
	
	LONG nIndent = 0;
	LONG nThread = 0;
	if (s_nTlsIndent >= 0) {
		nIndent = (LONG)TlsGetValue(s_nTlsIndent) - 1;
		TlsSetValue(s_nTlsIndent, (PVOID)nIndent);
	}
	if (s_nTlsThread >= 0) {
		nThread = (LONG)TlsGetValue(s_nTlsThread);
	}
		
	if (s_bLog) {
		CHAR szBuf[4096];
		LONG nLen = 3 + (nIndent < 35 ? nIndent * 2 : 70);
		FillMemory(szBuf, nLen, ' ');
	
		szBuf[0] = '0' + ((nThread / 10) % 10);
		szBuf[1] = '0' + ((nThread / 1) % 10);
		
		va_list  args;
		va_start(args, psz);
		_VPrint(psz, args, szBuf + nLen, sizeof(szBuf) - nLen);
		va_end(args);
		
		CLog::Write(szBuf, strlen(szBuf));
	}
	SetLastError(dwErr);
}

VOID _Print(const CHAR *psz, ...)
{
	DWORD dwErr = GetLastError();
	
	LONG nIndent = 0;
	LONG nThread = 0;
	if (s_nTlsIndent >= 0) {
		nIndent = (LONG)TlsGetValue(s_nTlsIndent);
	}
	if (s_nTlsThread >= 0) {
		nThread = (LONG)TlsGetValue(s_nTlsThread);
	}

	CHAR szBuf[4096];
	LONG nLen = 3 + (nIndent < 35 ? nIndent * 2 : 70);
	FillMemory(szBuf, nLen, ' ');
	
	szBuf[0] = '0' + ((nThread / 10) % 10);
	szBuf[1] = '0' + ((nThread / 1) % 10);
		
    va_list  args;
    va_start(args, psz);
	_VPrint(psz, args, szBuf + nLen, sizeof(szBuf) - nLen);
    va_end(args);
	
	CLog::Write(szBuf, strlen(szBuf));
	
	SetLastError(dwErr);
}

VOID AssertMessage(CONST PCHAR pszMsg, CONST PCHAR pszFile, ULONG nLine)
{
	CHAR szBuf[512];
    sprintf(szBuf, "ASSERT(%s) failed in %s, line %d.\n", pszMsg, pszFile, nLine);

	CLog::Write(szBuf, strlen(szBuf));
}

VOID NullExport()
{
}

//////////////////////////////////////////////////////////////////////////////
//
// DLL module information
//
BOOL ThreadAttach(HMODULE hDll)
{
	if (s_nTlsIndent >= 0) {
		TlsSetValue(s_nTlsIndent, (PVOID)0);
	}
	if (s_nTlsThread >= 0) {
		LONG nThread = InterlockedIncrement(&s_nThreadCnt);
		TlsSetValue(s_nTlsThread, (PVOID)nThread);
	}
	return TRUE;
}

BOOL ThreadDetach(HMODULE hDll)
{
	if (s_nTlsIndent >= 0) {
		TlsSetValue(s_nTlsIndent, (PVOID)0);
	}
	if (s_nTlsThread >= 0) {
		TlsSetValue(s_nTlsThread, (PVOID)0);
	}
	return TRUE;
}

BOOL ProcessAttach(HMODULE hDll)
{
	s_bLog = FALSE;
	s_nTlsIndent = TlsAlloc();
	s_nTlsThread = TlsAlloc();

	WCHAR wzDllName[MAX_PATH];
	WCHAR wzExeName[MAX_PATH];

	Real_GetModuleFileNameW(hDll, wzDllName, sizeof(wzDllName));
	Real_GetModuleFileNameW(NULL, wzExeName, sizeof(wzExeName));

	CLog::Print("##################################################################\n");
	CLog::Print("### %ls\n", wzExeName);
	CLog::Print("### %ls built %hs %hs\n", wzDllName, __DATE__, __TIME__);
	TrampolineWith();

	ThreadAttach(hDll);
	
	s_bLog = TRUE;
	return TRUE;
}

BOOL ProcessDetach(HMODULE hDll)
{
	ThreadDetach(hDll);
	
	Cleanup();
	s_bLog = FALSE;
	CLog::Close();

	if (s_nTlsIndent >= 0) {
		TlsFree(s_nTlsIndent);
	}
	if (s_nTlsThread >= 0) {
		TlsFree(s_nTlsThread);
	}
	return TRUE;
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, PVOID lpReserved)
{
	switch (dwReason) {
	  case DLL_PROCESS_ATTACH:
		return ProcessAttach(hModule);
	  case DLL_PROCESS_DETACH:
		return ProcessDetach(hModule);
	  case DLL_THREAD_ATTACH:
		return ThreadAttach(hModule);
	  case DLL_THREAD_DETACH:
		return ThreadDetach(hModule);
	}
	return TRUE;
}
//
///////////////////////////////////////////////////////////////// End of File.
