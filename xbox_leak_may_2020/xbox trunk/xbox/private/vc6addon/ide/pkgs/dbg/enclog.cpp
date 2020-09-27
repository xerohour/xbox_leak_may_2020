///////////////////////////////////////////////////////////////////////////////
// 
// enclog.cpp
//
// Support for logging edit-and-continue results to the net for QA
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop
#include <io.h>
#include <time.h>
#include <stdlib.h>
#define assert ASSERT
#include "ref.h"
#include "simparray.h"
#include "enc.h"
#include "enclog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

void ENCLogPerformance(clock_t &start_clock, char* szStatus)
{	// start_clock set to clock() before task
	// Appends EnC performance data to a local log file in the format:
	// Duration,szStatus<CR/LF>

	double duration = (double)(clock() - start_clock)/CLOCKS_PER_SEC;

	BOOL fLoggingEnabled = theApp.m_bEncPerfLog;
	if (!fLoggingEnabled)
		return;

	HANDLE hLocalFile;
	DWORD dwBytesWritten;
	char szPerfLogFilePath[256];
	GetTempPath(255, szPerfLogFilePath);
	lstrcat(szPerfLogFilePath, "ENCPerf.log");

	CString csLogString;
	csLogString.Format("%f,%s\r\n", duration, szStatus);

	if ( (hLocalFile = CreateFile(szPerfLogFilePath, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
		!= INVALID_HANDLE_VALUE )
		{
			SetFilePointer(hLocalFile, 0, NULL, FILE_END);	// Append to file
			WriteFile(hLocalFile, csLogString, csLogString.GetLength(), &dwBytesWritten, NULL);
			CloseHandle(hLocalFile);
		}
}


#ifdef _ENCLOG
static char szLogFileDir[] = "\\\\lang1\\ECLogs";
static char szLogFileName[] = "\\\\lang1\\ECLogs\\ECNetLog.dat";
static char szDLLFileName[] = "MSENC10.DLL";

bool FCanSeeNet() {
	static bool fCanSeeNet = false;
	static bool fTriedOnce = false;
	if (!fTriedOnce) {
		fCanSeeNet = (_access(szLogFileDir, 00) != -1);
		fTriedOnce = true;
	}
	return fCanSeeNet;
}


UINT ENCLogToNetCString(LPVOID pParam)
{
	// Like ENCLogToNet, but accepts a pointer to a CString 
	// and then destructs it.
	CString *pStr = (CString *) pParam;
	UINT retval = ENCLogToNet((LPVOID) (const char *)*pStr);
	delete pStr;
	return retval;
}

UINT ENCLogToNet(LPVOID pParam)
{	// Worker thread function to write EnC data to a log file on the net.
	// Appends the information to the file in the following format:
	// 12/25/97,09:15:33,MachineName,DateOfMSENC10.DLL,Success/FailingFunction<CR/LF>

	LPCTSTR lszParam = (LPCTSTR) pParam;

	if (lszParam == NULL)
		return 1;
	
	BOOL fLoggingEnabled = theApp.m_bEncLog;
	if (!fLoggingEnabled)
		return 2;

	HKEY hKey;
	HANDLE hNetFile;
	HANDLE hEngineDLL;
	FILETIME FileTime;
	SYSTEMTIME SystemTime;
	
	DWORD dwBytesWritten;
	DWORD dwRegType;	

	const DWORD BUFFER_MAX = 512;
	char buffer[BUFFER_MAX];
	char logString[BUFFER_MAX * 2];
	unsigned long lBufferSize = sizeof(buffer);
	short nNetAccessAttempts;

	_strdate(logString);							// Today's date
	lstrcat(logString, ",");
		
	 _strtime(buffer);								// Current time
	lstrcat(logString, buffer);
	lstrcat(logString, ",");

	GetComputerName(buffer, (LPDWORD)&BUFFER_MAX);	// Machine name
	lstrcat(logString, buffer);
	lstrcat(logString, ",");
	HMODULE hModule = GetModuleHandle(szDLLFileName);

	if (hModule && GetModuleFileName(hModule, buffer, sizeof(buffer)))
	{
		if ( (hEngineDLL = CreateFile(buffer, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE &&
			GetFileTime(hEngineDLL, NULL, NULL, &FileTime) )
		{
			CloseHandle(hEngineDLL);
			FileTimeToSystemTime(&FileTime, &SystemTime);
			lstrcat(logString, _itoa(SystemTime.wMonth, buffer, 10));
			lstrcat(logString, "/");
			lstrcat(logString, _itoa(SystemTime.wDay, buffer, 10));
			lstrcat(logString, "/");
			lstrcat(logString, _itoa(SystemTime.wYear, buffer, 10));
		}
	}

	lstrcat(logString, ",");
	lstrcat(logString, lszParam);
	lstrcat(logString, "\r\n");
	
	if (!FCanSeeNet())
	{
		// Can't see net directory, so log to local file
		HANDLE hLocalFile;
		char szLocalLogFilePath[256];
		GetTempPath(255, szLocalLogFilePath);
		lstrcat(szLocalLogFilePath, "ENC.log");
		
		if ( (hLocalFile = CreateFile(szLocalLogFilePath, GENERIC_WRITE,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
			!= INVALID_HANDLE_VALUE )
		{
			SetFilePointer(hLocalFile, 0, NULL, FILE_END);	// Append to file
			WriteFile(hLocalFile, logString, lstrlen(logString), &dwBytesWritten, NULL);
			CloseHandle(hLocalFile);
		}
		else
			return 3;	// Can't open local file
	}
	else
	{
		//  Keep trying to open the net log for writing
		nNetAccessAttempts = 0;
		while ( (hNetFile = CreateFile(szLogFileName, GENERIC_WRITE,
				FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
				== INVALID_HANDLE_VALUE && nNetAccessAttempts < MAX_NET_ACCESS_ATTEMPTS)
			{
				nNetAccessAttempts++;
				Sleep(250);	// Sleep(250) = 10 second timeout if MAX_NET_ACCESS_ATTEMPTS = 40
			}

		SetFilePointer(hNetFile, 0, NULL, FILE_END);	// Append to file
		WriteFile(hNetFile, logString, lstrlen(logString), &dwBytesWritten, NULL);
		CloseHandle(hNetFile);
	}

	if (!_tcscmp((char*) lszParam, LOG_PUSHNEWEDIT)	||		// Ask for repro case?
		!_tcscmp((char*) lszParam, LOG_HRESULT_EXPORTPUB)	||
		!_tcscmp((char*) lszParam, LOG_HRESULT_MEMWRITE)	||
		!_tcscmp((char*) lszParam, LOG_HRESULT_THUNKWRITE))
	{
		MessageBox( NULL,
					"\nAn Edit and Continue error has occurred.\n\nPlease IMMEDIATELY send the modified source files,\nalong with a description of the recent code modifications to:    \n\nE-mail alias:  VCENC@Microsoft.com",
					"Edit and Continue",
					MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND | MB_TOPMOST
					);
	}
	
	return 0;
}


void ENCLogForHResult(HRESULT hResult)
{
	switch (hResult)
	{
	  case E_MEMMAP:		NET_LOG(LOG_HRESULT_MEMMAP);		break;	// Prob. illegal edit
	  case E_INITMEMALLOC:	NET_LOG(LOG_HRESULT_INITMEMALLOC);	break;
	  case E_EXPORTPUB:		NET_LOG(LOG_HRESULT_EXPORTPUB);		break;
	  case E_FIXUP:			NET_LOG(LOG_HRESULT_FIXUP);			break;	// Prob. illegal edit
	  case E_MEMWRITE:		NET_LOG(LOG_HRESULT_MEMWRITE);		break;
	  case E_THUNKWRITE:	NET_LOG(LOG_HRESULT_THUNKWRITE);	break;
	  case E_NOPDBSUPPORT:	NET_LOG(LOG_HRESULT_NOPDBSUPPORT);	break;
	  default:				NET_LOG(LOG_HRESULT_UNKNOWN);		break;
	}
}

static DWORD FileSize(LPCTSTR szFile)
{
	DWORD dwSize = 0;
	HANDLE hFile;
	if 	((hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE )
	{
		dwSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
	}
	return dwSize;
}

void ENCLogHmod(HMOD hmod)		
{
	char buf[80];
	HEXE hexe = SHHexeFromHmod(hmod);
	CString strMod = SHGetObjPath(hmod);
	CString strExe = SHGetExeName(hexe);

	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];
	
	_splitpath(strExe, NULL, NULL, szFname, szExt);

	sprintf(buf, "%d,%d,%s%s", FileSize(strMod), FileSize (strExe), szFname, szExt);

	NET_LOG_EX(buf);
}

#endif // _ENCLOG