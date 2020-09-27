// bldspawner.cpp
//
// low-level build routines to spawn tools

#include "stdafx.h"

#include <direct.h>
#include <string.h>
#include <register.h>
#include "bldspawner.h"
#include "dllapi.h"
#include "path2.h"
#include "BuildEngine.h"
#include "util2.h"

// #define SHOW_SPAWNER_DIAGNOSTICS		// uncomment this if you want the ATLTRACE calls in this file

#define MAX_TOOL_MACRO  16					// max length of macro name
#define MAX_LINE_SIZE   2048
#define MAX_USER_LINE   MAX_LINE_SIZE
#define TMP_STRING_SIZE MAX_LINE_SIZE + 60	// all purpose strings

extern CTestableSection g_sectionSpawner;

PfnReadPipe pfnReadPipe = (GetVersion() & 0x80000000) ? FWin95ReadPipe : FWinNTReadPipe;

// characters that indicate to us that we need to spawn a command
// processor in front of tools (redirection and command chaining)
const wchar_t rgchCmdProcChar[] = L"<>&|";


#define MSG_ERR		0
#define MSG_WARN	1
#define MSG_INFO	2
#define MSG_LINE	3
#define MSG_LOG		4

int GetMessageFileAndLine(CStringW &strIn, CStringW &strMsg, CStringW &strFile, ULONG *ulLine, CStringW &strErrNumber)
{
	int nSect = -1;
	//int nOff = 1;
	int nType = MSG_INFO;
	
	// Determine if it is meant straight for the log
	if (strIn.GetLength() > 16 && _wcsnicmp(strIn, L"/*/*/LOGLOG/*/*/", 16) == 0)
	{
		strMsg = strIn.Right(strIn.GetLength()-16);
		return MSG_LOG;
	}

    // Find the end of section 1
	nSect = strIn.Find(L": ");
	if(nSect == -1 )
	{
		strMsg = strIn;
		*ulLine = 0;
		return nType;
	} 
	strFile = strIn.Left(nSect);

	// Now look for the end of section 2
	CStringW strRemainder = strIn.Mid(nSect+2);
	nSect = strRemainder.Find(L": ");
	if( nSect == -1 )
	{
		strMsg = strIn;
		*ulLine = 0;
		return nType;
	}

	// Now check section 2 to see what kind of message this is.
	CStringW strError = strRemainder.Left(nSect);
	int nErr = 0;
	nErr = strError.Find(L"error");
	if( nErr == -1)
	{
		nErr = strError.Find(L"warning");
		if( nErr == -1)
		{
			// this is an info message
			nType = MSG_INFO;
			strMsg = strIn;
			*ulLine = 0;
			return nType;
		}
		else
		{
			// this is an warning message
			nType = MSG_WARN;
		}
	}
	else
	{
		// this is an error message
		nType = MSG_ERR;
	}

	// Now try to get the error number XXX0000
	strError.TrimRight();
	int nErrNumberPos = strError.ReverseFind(L' ');
	if( nErrNumberPos > -1 )
	{
		strErrNumber = strError.Right(strError.GetLength()-nErrNumberPos -1);
	}

	strMsg = strRemainder;

	// Its an error or a warning so try and find the line number
	int nBeg = strFile.Find(L"("); // FIXME : these should use reverse find.
	int nEnd = strFile.Find(L")");
	if( nBeg == -1 || nBeg > nEnd )
	{
		// couldn't find a line number buts thats ok.
		*ulLine = 0;
		return nType;  
	}
	else
	{
		CStringW strLine = strIn.Mid(nBeg+1, nBeg+nEnd);
		int nComma = strLine.Find(L",");
		if( nComma > -1 )
		{
		    strLine = strLine.Left(nComma);
		}
		*ulLine = (ULONG)_wtoi(strLine);
		strFile = strFile.Left(nBeg);
		return nType;
	}
}

STDMETHODIMP ParseLine(IVCBuildErrorContext *pEC, BSTR bstr)
{
	CStringW str;
	str = bstr;
	if( bstr[0] != NULL )
		str += L"\n";
	
	CStringW strFileName;
	CStringW strMsg;
	CStringW strError;
	CComBSTR bstrFileName;
	
	ULONG ulLine = 0;
	int nType = GetMessageFileAndLine(str, strMsg, strFileName, &ulLine, strError);
	if( (!strFileName.IsEmpty()) && (ulLine != 0) )
	{

		strFileName.TrimLeft();
		CPathW path;
		CComBSTR bstrBaseDir;
		pEC->get_BaseDirectory(&bstrBaseDir);
		CDirW dir;
		dir.CreateFromString(bstrBaseDir);
		path.CreateFromDirAndFilename(dir, strFileName);
		if( path.ExistsOnDisk() )
		{
		    bstrFileName = path;
		}
		else
		{
		    bstrFileName = strFileName;
		}
	}
	else
	{
		pEC->get_ProjectName(&bstrFileName);
	}
	
	CComBSTR bstrError = strError;
	CComBSTR bstrMsg = strMsg;
	
	switch( nType )
	{
		case MSG_ERR:
			pEC->AddError(bstrMsg, bstrError, bstrFileName, ulLine, bstr );
			break;
		case MSG_WARN:
			pEC->AddWarning(bstrMsg, bstrError, bstrFileName, ulLine, bstr );
			break;
		case MSG_INFO:
			pEC->AddInfo(bstrMsg);
			break;
		case MSG_LINE:
			pEC->AddLine(bstrMsg);
			break;
		case MSG_LOG:
			pEC->WriteLog(bstrMsg);
			break;
		default:
			break;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// FWin95ReadPipe
//
// Necessary function to be able to always return from a ReadFile on a pipe
// when Win95 doesn't realize the pipe has been broken.  We use an auxilliary
// event to stop on.  In this case, it will be the handle to the process that
// we spawn.  When both the process handle is signalled AND the pipe is empty,
// we consider the connection broken.  Otherwise, we pseudo-block until either
// there is data to be read (and we read it) or the process is gone.
//-----------------------------------------------------------------------------
BOOL FWin95ReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE hAuxStop)
{
	DWORD cbAvail = 0;
	
	// while the pipe is still valid, we don't have data, and the aux stop event
	// is not signalled, we pseudo-block
	while (PeekNamedPipe(hpipe, NULL, 0, NULL, &cbAvail, NULL) && cbAvail == 0 &&
			WaitForSingleObject(hAuxStop, 0) == WAIT_TIMEOUT)
	{
		Sleep(10);	// give up our timeslice in a pseudo-block
	}

	cbRead = 0;
	if (cbAvail)
	{
		return ReadFile(hpipe, pvBuf, __min(cbToRead, cbAvail), &cbRead, NULL);
	}

	// else, the only other reason to get out of the loop above is if the pipe
	// has been broken or the aux stop event is signalled.
	return FALSE;
}

//-----------------------------------------------------------------------------
// FWinNTReadPipe
//
// Thunk to ReadFile
//-----------------------------------------------------------------------------
BOOL FWinNTReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE /*hAuxStop*/)
{
	return ReadFile(hpipe, pvBuf, cbToRead, &cbRead, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CBldConsoleSpawner

#ifdef _WIN64
wchar_t CBldConsoleSpawner::s_szSpawnToolArgs[] = L" -e %I64u %s ";
#else
wchar_t CBldConsoleSpawner::s_szSpawnToolArgs[] = L" -e %lu %s ";
#endif // _WIN64
wchar_t CBldConsoleSpawner::s_szSpawnForCodePageToolArgs[] = L" -c";

const wchar_t * szVCSpawn = L"vcspawn";

unsigned _stdcall CBldConsoleSpawner::ReadChildProcessOutput(LPVOID lpv)
{
	CBldConsoleSpawner *pSpawner = (CBldConsoleSpawner *)lpv;
	DWORD cb = 0;	// bytes in output buffer
	char rgchProcOutputBuf[cbProcOutputBuf];
	HRESULT hr;

	hr = CoInitialize(NULL);
	VSASSERT(SUCCEEDED(hr), "Failed to CoInitialize for the build spawner");

	SetThreadLocale(LOCALE_SYSTEM_DEFAULT);
	do {
		// wait until process has been spawned
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: waiting for process spawn event: hSpawnEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		WaitForSingleObject(pSpawner->m_hSpawnEvent, INFINITE);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: returned from wait for process spawn event\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS

		if (pSpawner->m_fThreadDie)
			break;

#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: going to read from pipe handle = 0x%08x\n", pSpawner, pSpawner->m_hReadPipe);
#endif	// SHOW_SPAWNER_DIAGNOSTICS

		// read pipe until it's empty
		while ((*pfnReadPipe)(pSpawner->m_hReadPipe, rgchProcOutputBuf, cbProcOutputBuf - 1,
						cb, pSpawner->m_procinfo.hProcess) && cb > 0)
		{
			// append '\0' to end of buffer
			// NOTE: always okay because only filled up to cbProcOutputBuf-1 bytes
			rgchProcOutputBuf[cb] = '\0';

			// write buffer to output window
#ifdef SHOW_SPAWNER_DIAGNOSTICS
			ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: going to DisplayOutputFromConsole\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
			pSpawner->DisplayOutputFromConsole(rgchProcOutputBuf);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
			ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: returned from to read from DisplayOutputFromConsole\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		}

#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : ReadChildProcessOutput: GetLastError() from ReadFile = 0x%08x\n", pSpawner, GetLastError());
#endif	// SHOW_SPAWNER_DIAGNOSTICS

		// send window message to main thread
		// PostMessage(hwndFrame, WM_NULL, 0, 0);
	} while (TRUE);

	pSpawner->CloseDisplay();
	CoUninitialize();

#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : ReadChildProcessOutput thread committing suicide\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	_endthreadex(0);
	return 0;
}

unsigned _stdcall CBldConsoleSpawner::WaitChildProcessFinish(LPVOID lpv)
{
	CBldConsoleSpawner *pSpawner = (CBldConsoleSpawner *)lpv;

	// wait until process has been terminated
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: waiting for process to terminate: m_procinfo.hProcess\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	WaitForSingleObject(pSpawner->m_procinfo.hProcess, INFINITE);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: returned from wait for process to terminate: m_procingo.hProcess\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS

	GetExitCodeProcess(pSpawner->m_procinfo.hProcess, &(pSpawner->m_dwTermStatus));

	pSpawner->m_fThreadDie = TRUE;
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: SetEvent hSpawnEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	SetEvent(pSpawner->m_hSpawnEvent);

#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: waiting for ReadChildProcessOutput thread to terminate: hSpawnThread\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	WaitForSingleObject(pSpawner->m_hSpawnThread, INFINITE);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: returned from wait for ReadChildProcessOutput thread to terminate\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	pSpawner->CloseOneHandle(pSpawner->m_hSpawnThread);

	// if there's text in the output hold buffer (m_szOutput), flush it now
	if (pSpawner->m_ichOutput)
		pSpawner->DisplayOutputFromConsole("\r\n");

	if (pSpawner->m_hNotifyEvent != NULL)
	{
		// assume that notified owner will output termination messages
		VSASSERT(pSpawner->m_hCleanEvent == NULL, "Spawner CleanEvent already set!");
		pSpawner->m_hCleanEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		VSASSERT(pSpawner->m_hCleanEvent != NULL, "Spawner CleanEvent cannot be created.");
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: SetEvent hNotifyEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		SetEvent(pSpawner->m_hNotifyEvent);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: waiting for hCleanEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		WaitForSingleObject(pSpawner->m_hCleanEvent, INFINITE);
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: returned from wait for hCleanEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		VSASSERT(pSpawner == *(pSpawner->m_ppSpawner), "Unmatched spawner!!!");
		*(pSpawner->m_ppSpawner) = NULL;
		BOOL bOK = pSpawner->CloseOneHandle(pSpawner->m_hNotifyEvent);
		VSASSERT(bOK, "Failed to close handle for spawner's notify event thread!");
		bOK = pSpawner->CloseOneHandle(pSpawner->m_hCleanEvent);
		VSASSERT(bOK, "Failed to close handle for spawner's clean event thread!");
	}
	else
	{
		VSASSERT(FALSE, "Spawner NotifyEvent was NULL"); // TODO: DRA (remove some time) catch this in the debugger.
		// check to see if we killed the process so we can put a message out
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: waiting for process to terminate: hCancelEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		if (WaitForSingleObject(pSpawner->m_hCancelEvent, 0) == WAIT_OBJECT_0)
		{
		}
		else
		{
			pSpawner->m_strOutput.Format(pSpawner->m_strError, pSpawner->m_dwTermStatus);
		}
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: returned from wait for process to terminate : hCancelEvent\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		VSASSERT(pSpawner == *(pSpawner->m_ppSpawner), "Unmatched spawner!!!");
		*(pSpawner->m_ppSpawner) = NULL;
	}
	
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: cleaning up spawner object\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	pSpawner->CleanupHandles();

	// reset flag enabling subsequent spawns
	pSpawner->m_fEnableSpawn = TRUE;

#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish: deleting spawner object\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	delete pSpawner;

#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : WaitChildProcessFinish thread committing suicide\n", pSpawner);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	_endthreadex(0);
	return 0;
}

CBldConsoleSpawner::CBldConsoleSpawner(CBldConsoleSpawner **ppSpawner)
{
	// set up the back pointer which is set to NULL upon spawn termination
	m_ppSpawner = ppSpawner;
	// Create an error context
	m_ichOutput = 0;
	memset((void *)&m_startinfo, 0, sizeof(STARTUPINFO));
	memset((void *)&m_procinfo, 0, sizeof(PROCESS_INFORMATION));
	m_sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_sa.lpSecurityDescriptor = NULL;
	m_sa.bInheritHandle = TRUE;
	m_fEnableSpawn = TRUE;
	m_fUseConsoleCP = FALSE;
	m_hNotifyEvent = NULL;
	m_hCleanEvent = NULL;
	m_hSpawnThread = NULL;
	m_hWaitThread = NULL;
	m_hSpawnEvent = NULL;
	m_hCancelEvent = NULL;
	m_hReadPipe = INVALID_HANDLE_VALUE;
	m_hNulStdin = INVALID_HANDLE_VALUE;
	m_fThreadDie = FALSE;
	m_dwTermStatus = 0;
	m_hpipeWriteOut = INVALID_HANDLE_VALUE;	// stdout handle for child process
	m_hpipeWriteErr = INVALID_HANDLE_VALUE;	// stderr handle for child process
	m_hGeneric = NULL;
	m_CompileState = CS_START;
}

CBldConsoleSpawner::~CBldConsoleSpawner()
{
	CleanupHandles();
}

BOOL CBldConsoleSpawner::CloseOneHandle(HANDLE& hHandle, HANDLE hNewHandleValue /* = NULL */)
{
	BOOL bOK = TRUE;
	if (hHandle != hNewHandleValue)
	{
		bOK = CloseHandle(hHandle);
		hHandle = hNewHandleValue;
	}
	return bOK;
}

void CBldConsoleSpawner::CleanupHandles()
{
	// Prevent access to spawner by other threads.
	CritSectionT cs(g_sectionSpawner);

	// Destroy any remaining handles
	BOOL bOK = CloseOneHandle(m_procinfo.hThread);
	VSASSERT(bOK, "Failed to close handle for spawner's m_procinfo.hThread!");
	bOK = CloseOneHandle(m_procinfo.hProcess);
	VSASSERT(bOK, "Failed to close handle for spawner's m_procinfo.hProcess!");
	bOK = CloseOneHandle(m_hNulStdin, INVALID_HANDLE_VALUE);
	VSASSERT(bOK, "Failed to close handle for spawner's m_hNulStdin!");
	bOK = CloseOneHandle(m_hReadPipe, INVALID_HANDLE_VALUE);
	VSASSERT(bOK, "Failed to close handle for spawner's m_hReadPipe!");
	bOK = CloseOneHandle(m_hCancelEvent);
	VSASSERT(bOK, "Failed to close handle for spawner's cancel event thread!");
	bOK = CloseOneHandle(m_hSpawnEvent);
	VSASSERT(bOK, "Failed to close handle for spawner's spawn event thread!");
	bOK = CloseOneHandle(m_hWaitThread);
	VSASSERT(bOK, "Failed to close handle for spawner's wait thread!");
	bOK = CloseOneHandle(m_hNotifyEvent);
	VSASSERT(bOK, "Failed to close handle for spawner's notify event thread!");
	bOK = CloseOneHandle(m_hCleanEvent);
	VSASSERT(bOK, "Failed to close handle for spawner's clean event thread!");
	bOK = CloseOneHandle(m_hSpawnThread);
	VSASSERT(bOK, "Failed to close handle for spawner's spawn thread!");
	bOK = CloseOneHandle(m_hpipeWriteOut, INVALID_HANDLE_VALUE);
	VSASSERT(bOK, "Failed to close handle to pipe's write end!");
	bOK = CloseOneHandle(m_hpipeWriteErr, INVALID_HANDLE_VALUE);
	VSASSERT(bOK, "Failed to close handle to pipe's read end!");
	CloseOneHandle(m_hGeneric);
}

BOOL CBldConsoleSpawner::InitializeSpawn(LPCOLESTR lpszSpawnDir, BOOL bClearOutputWindow, BOOL bSaveAll /* = TRUE */, 
	IVCBuildErrorContext *pEC)
{
	m_ichOutput = 0;
	m_pEC = pEC;

	// create semaphore for indicating process spawned
	if ((m_hSpawnEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
	{
		AddSpawnerError(IDS_ERR_RESOURCES_EVENT_PRJ0011, L"PRJ0011");
		return FALSE;
	}

	// create thread for reading output from process
#ifdef SHOW_SPAWNER_DIAGNOSTICS
	ATLTRACE("spawner 0x%08x : InitializeSpawn: Starting thread : hSpawnThread\n", this);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
	unsigned int dwThreadID;
	if ( (m_hSpawnThread = (HANDLE)_beginthreadex( NULL, cbThrdStack, CBldConsoleSpawner::ReadChildProcessOutput, this, 0, &dwThreadID)) == (HANDLE)0 )
	{
		AddSpawnerError(IDS_ERR_RESOURCES_THREAD_PRJ0012, L"PRJ0012");
		return FALSE;
	}

	// save away the project build directory for the call to CreateProcess
	m_strSpawnDir = lpszSpawnDir;
	return TRUE;
}

BOOL CBldConsoleSpawner::PerformSpawnMultiple(CVCStringWList *plistCommands, CDynamicBuildEngine* pBldEngine, 
	BOOL fUseConsoleCP)
{
	static const wchar_t szCmdSep[] = { chCmdSep, 0 };
	CStringW strCommands;
	INT_PTR  nCommands = plistCommands->GetCount();
	BOOL bMultipleCommand = nCommands > 1;

	VCPOSITION pos = plistCommands->GetHeadPosition();
	while (pos)
	{
		strCommands += plistCommands->GetNext(pos);
		if (pos)
			strCommands += szCmdSep;
	}

	if (pBldEngine->m_bUserCanceled)
	{
		pBldEngine->InformUserCancel();
		return FALSE;
	}

	if (!strCommands.IsEmpty())
		return PerformSpawn(strCommands, bMultipleCommand, pBldEngine, fUseConsoleCP);

	return FALSE;
}

void CBldConsoleSpawner::CancelSpawn()
{
	if (m_hCancelEvent != (HANDLE)0)
	{
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : CancelSpawn: SetEvent hCancelEvent\n", this);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		BOOL bOK = SetEvent(m_hCancelEvent);
		VSASSERT(bOK, "Failed to set Cancel Event for spawner!");
	}
}

void CBldConsoleSpawner::CloseDisplay()
{
	if (m_pEC)
		m_pEC->Close();
}

void CBldConsoleSpawner::DisplayOutputFromConsole(const char *lpszTextIn)
{
	int cb;
	const char *lpszText = lpszTextIn;
	const char *lpszBegin = lpszText;

	// loop through all characters in string
	while (*lpszText)
	{
		// process special characters
		//
		switch (*lpszText)
		{
			case '\t':
				// if enough room, convert tabs into spaces
				if (m_ichOutput < MAX_USER_LINE)
					m_szOutput[m_ichOutput ++] = ' ';
				else
					// no room
					goto OverFlow ;
				break;

			case '\r':
				// Ignore CR
				break;

			case '\n':
				// treat LF as end of line marker, but only if the next character is NOT a tab or space...
				if (*(lpszText + 1) != '\t')
				{
					bool bContinuation = false;
					if ( (*(lpszText + 1) == ' ') && ((*(lpszText + 2) == ' ') || (*(lpszText + 2) == '\t')) )
						bContinuation = true;
					if (!bContinuation)
					{
						// terminate string*
						m_szOutput[m_ichOutput] = '\0';
						// add to output window
						if (m_pEC != NULL)
						{
							CComBSTR bstrOutput;
							wchar_t wszOutput[CB_COMM_BUF+1];
							MultiByteToWideChar(CDynamicBuildEngine::GetCodePage(m_fUseConsoleCP), MB_PRECOMPOSED, m_szOutput, -1, wszOutput, CB_COMM_BUF+1);
							bstrOutput = wszOutput;
							ParseLine(m_pEC, bstrOutput);
						}
						// set up for next line
						m_ichOutput = 0;
					}
					else
						m_szOutput[m_ichOutput++] = '\n';
				}
				break;

			default:
				// we have a regular character.  Save it if
				// we've got the room
				if (m_ichOutput + (cb = (int) _mbclen((const unsigned char *)lpszText)) < MAX_USER_LINE + 1)
				{
					// olympus 1281:  The read from the pipe may have truncated
					// a double-byte character.  Make sure we don't copy a bogus
					// trail byte into the buffer.  NOTE that the input buffer *MUST*
					// be null-terminated!
					cb = (*(lpszText + 1) == '\0') ? 1 : cb;
					_mbccpy((unsigned char *)m_szOutput + m_ichOutput, (const unsigned char *)lpszText);
					m_ichOutput += cb;
				}
				else
				{
					// we don't have the room.  Terminate the
					// string here
OverFlow:
					// terminate
					//
					m_szOutput[m_ichOutput] = '\0';
					// Make sure that the character we just boofed gets included
					// in the next line.  (I.e., negate the inc that happens at the
					// bottom of the loop.)
					if (lpszText > lpszBegin)
						lpszText = (const char *)_mbsdec((const unsigned char *)lpszBegin, (const unsigned char *)lpszText);

					// add to output window
					if (m_pEC != NULL)
					{
						CComBSTR bstrOutput;
						if (CDynamicBuildEngine::GetCodePage(m_fUseConsoleCP) == 0)
							bstrOutput = m_szOutput;
						else	// need to convert it properly
						{
							wchar_t wszOutput[CB_COMM_BUF+1];
							MultiByteToWideChar(CDynamicBuildEngine::GetCodePage(m_fUseConsoleCP), MB_PRECOMPOSED, m_szOutput, -1, wszOutput, CB_COMM_BUF+1);
							bstrOutput = wszOutput;
						}
						ParseLine(m_pEC, bstrOutput);
					}
					// set up for next line
					m_ichOutput = 0;
				}
				break;
		} // switch

		// advance to next character
		//
		// olympus 1281:  We don't know if a DBC has been split...
		cb = (int) _mbclen((const unsigned char *)lpszText);
		cb = (*(lpszText + 1) == '\0') ? 1 : cb;
		lpszText += cb;
	} // while
}


BOOL CBldConsoleSpawner::PerformSpawn(LPCOLESTR lpszCommandLine, BOOL bMultipleCommand, CDynamicBuildEngine* pBldEngine,
	BOOL fUseConsoleCP)
{
	CStringW strSpawn;
	m_fUseConsoleCP = fUseConsoleCP;
	BOOL fCreateSuccess;	// create process succeeded?
	DWORD dwError;

	CStringW strSpawnTool;
	DWORD dwLen = ::SearchPathW(NULL, szVCSpawn, L".exe", 0, NULL, NULL);
	if (dwLen > 0)
	{
		wchar_t* szPath = strSpawnTool.GetBuffer(dwLen+1);
		if (szPath == NULL)		// out of memory
		{
			DoCleanUp();
			return FALSE;
		}
		::SearchPathW(NULL, szVCSpawn, L".exe", dwLen, szPath, NULL);
		strSpawnTool.ReleaseBuffer();
		strSpawnTool = L"\"" + strSpawnTool;
		strSpawnTool += L"\"";
	}
	else	// not found on the path
	{
		CStringW strMsvcPath;
		UtilGetCommon7Folder(strMsvcPath);
		strMsvcPath += L"Tools\\";
		strSpawnTool = L"\"" + strMsvcPath;
		strSpawnTool += szVCSpawn;
		strSpawnTool += L".exe\"";
	}
	
	// set flag disabling subsequent spawns
	m_fEnableSpawn = FALSE;

	m_CompileState = CS_START;

	if (pBldEngine->m_bUserCanceled)
	{
		DoCleanUp();
		pBldEngine->InformUserCancel();
		return FALSE;
	}

	m_CompileState = CS_CREATEALLOC;

	// create an event object to signal vcspawn.exe when to kill
	//  the child process tree.  note that we have the manual reset
	//  option set so that we can detect when we killed the process
	//  w/o using another flag.
	if (!(m_hCancelEvent = CreateEvent(&m_sa, TRUE, FALSE, NULL)))
	{
		AddSpawnerError(IDS_ERR_RESOURCES_EVENT_PRJ0011, L"PRJ0011");
		DoCleanUp();
		return FALSE;
	}

	m_CompileState = CS_CREATEEVENT;

	// Create the spawn string with the vcspawn prepended
	strSpawn.Format(s_szSpawnToolArgs, (DWORD_PTR)m_hCancelEvent, bMultipleCommand ? szMultCmd : L"");
	strSpawn = strSpawnTool + strSpawn;

	// create string for combined lpUtil and lpCmdLine
	// form the rest of the command line
	strSpawn += lpszCommandLine;

	if (!CreatePipe(&m_hReadPipe, &m_hpipeWriteOut, &m_sa, cbPipeBuf))
	{
		AddSpawnerError(IDS_ERR_RESOURCES_PIPE_PRJ0013, L"PRJ0013");
		DoCleanUp();
		return FALSE;
	}

	m_CompileState = CS_CREATEPIPE;

	// must use a different handle for stderr because some apps (like cl.exe)
	// use dup2() which closes the current target handle if open.  this has
	// the effect of closing both handles if we only pass in the one handle
	// in both hStdOutput and hStdError fields of STARTUPINFO
	m_hGeneric = GetCurrentProcess();
	if (!DuplicateHandle(
			m_hGeneric,					// source process
			m_hpipeWriteOut,			// source handle to dupe
			m_hGeneric,					// target process
			&m_hpipeWriteErr,			// new handle
			DUPLICATE_SAME_ACCESS,		// access flags
			TRUE,						// make it inheritable
			DUPLICATE_SAME_ACCESS))		// option flags
	{
		AddSpawnerError(IDS_ERR_DEVNULL_PRJ0015, L"PRJ0015");
		DoCleanUp();
		return FALSE;
	}

	m_CompileState = CS_DUPLICATEHANDLES;

	// create a handle to /dev/nul
	m_hNulStdin = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		&m_sa, OPEN_ALWAYS, 0, INVALID_HANDLE_VALUE);

	if (m_hNulStdin == INVALID_HANDLE_VALUE)
	{
		AddSpawnerError(IDS_ERR_DEVNULL_PRJ0015, L"PRJ0015");
		DoCleanUp();
		return FALSE;
	}

	m_CompileState = CS_CREATENULFILE;

	m_startinfo.cb = sizeof(STARTUPINFOW);
	m_startinfo.lpReserved = NULL;
	m_startinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	m_startinfo.hStdOutput = m_hpipeWriteOut;
	m_startinfo.hStdError = m_hpipeWriteErr;
	m_startinfo.hStdInput = m_hNulStdin;
	m_startinfo.wShowWindow = SW_HIDE;

	// check to see if we're allowed to run vcspawn
	if( !CanCreateProcess( szVCSpawn ) )
	{
		fCreateSuccess = FALSE;
		AddSpawnerError(IDS_ERR_SECURITY_PRJ0016, L"PRJ0016");
	}
	else
	{
		if (pBldEngine->m_bUserCanceled)
		{
			DoCleanUp();
			CancelSpawn();
			return FALSE;
		}

		fCreateSuccess = CreateProcessW(NULL, strSpawn.GetBuffer(strSpawn.GetLength()), NULL, NULL, TRUE,
			CREATE_NEW_CONSOLE, NULL, m_strSpawnDir, &m_startinfo, &m_procinfo);
		strSpawn.ReleaseBuffer();

		dwError = GetLastError();
		// GetLastError() returns ERROR_ACCESS_DENIED on NT German when path contains extended chars!
		if (!fCreateSuccess && (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_ACCESS_DENIED))
		{
			// Unable to run vcspawn with explicit path, try without...
			strSpawn.Format(s_szSpawnToolArgs, (DWORD_PTR)m_hCancelEvent, bMultipleCommand ? szMultCmd : L"");
			strSpawnTool = L"\"";
			strSpawnTool += szVCSpawn;
			strSpawnTool += L".exe\"";
			strSpawn = strSpawnTool + strSpawn;
			strSpawn += lpszCommandLine;
			fCreateSuccess = CreateProcessW(NULL, strSpawn.GetBuffer(strSpawn.GetLength()), NULL, NULL, TRUE,
				CREATE_NEW_CONSOLE, NULL, m_strSpawnDir, &m_startinfo, &m_procinfo);
			strSpawn.ReleaseBuffer();
		}

		// Unable to run vcspawn (either explicitly pathed or not)...
		if( !fCreateSuccess )
		{
			dwError = GetLastError();
			if( dwError == ERROR_FILE_NOT_FOUND)
			{
				// vc spawn really missing.
				AddSpawnerError(IDS_FAIL_VCSPAWN_PRJ0001, L"PRJ0001");
			}
			else if( dwError == ERROR_DIRECTORY)
			{
				// INVALID WORKING DIR
				AddSpawnerError(IDS_ERR_NOCWD_PRJ0017, L"PRJ0017");
			}
			else
			{
				VSASSERT(0, "Unrecognized Error code from CreateProcess");
				AddSpawnerError(IDS_FAIL_VCSPAWN_PRJ0001, L"PRJ0001");
			}
		}
	}

	if (fCreateSuccess)
	{
		if (pBldEngine->m_bUserCanceled)
		{
			CancelSpawn();
			DoCleanUp();
			return FALSE;
		}

		// create thread which waits for process termination
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : PerformSpawn: Starting thread : hWaitThread\n", this);
#endif	// SHOW_SPAWNER_DIAGNOSTICS

		unsigned int dwThreadID;
		m_hWaitThread = (HANDLE)_beginthreadex( NULL, cbThrdStack, CBldConsoleSpawner::WaitChildProcessFinish, this, 0, &dwThreadID);

		VSASSERT(m_hWaitThread != (HANDLE)0, "Could not create WaitThread");	// REVIEW: UNDONE report error: could not create thread

		// indicate process spawned
#ifdef SHOW_SPAWNER_DIAGNOSTICS
		ATLTRACE("spawner 0x%08x : PerformSpawn: SetEvent hSpawnEvent\n", this);
#endif	// SHOW_SPAWNER_DIAGNOSTICS
		m_fThreadDie = FALSE;
		SetEvent(m_hSpawnEvent);

		// Prevent access to spawner by other threads.
		CritSectionT cs(g_sectionSpawner);

		BOOL bOK;
		// close our copies of pipe write handles
		bOK = CloseOneHandle(m_hpipeWriteOut, INVALID_HANDLE_VALUE);
		VSASSERT(bOK, "Failed to close handle m_hpipeWriteOut!");
		bOK = CloseOneHandle(m_hpipeWriteErr, INVALID_HANDLE_VALUE);
		VSASSERT(bOK, "Failed to close handle m_hpipeWriteErr!");
		// Do NOT call DoCleanUp here no matter HOW tempting it looks.  That's a real good way to
		// cause us to crash while waiting on the cancel event elsewhere...
		return TRUE;
	}

	DoCleanUp();
	return FALSE;
}

void CBldConsoleSpawner::DoCleanUp()
{
	// Prevent access to spawner by other threads.
	CritSectionT cs(g_sectionSpawner);

	BOOL bOK;
	switch(m_CompileState)
	{
		default:
			VSASSERT(FALSE, "No legal default state for CompileState...");
			break;
		case CS_SUCCESSFUL:
		case CS_KILLEDPROCESS:
		case CS_CREATEPROCESS:
			// Successful CreateProcess
			bOK = CloseOneHandle(m_procinfo.hThread);
			VSASSERT(bOK, "Failed to close handle m_procinfo.hThread!");
			bOK = CloseOneHandle(m_procinfo.hProcess);
			VSASSERT(bOK, "Failed to close handle m_procinfo.hProcess!");
		case CS_CREATENULFILE:
			// Successful CreateFile("NUL")
			bOK = CloseOneHandle(m_hNulStdin, INVALID_HANDLE_VALUE);
			VSASSERT(bOK, "Failed to close handle m_hNulStdin!");
		case CS_DUPLICATEHANDLES:
		case CS_CREATEPIPE:
			// Successful DuplicateHandle
			// Successful CreatePipe
			bOK = CloseOneHandle(m_hReadPipe, INVALID_HANDLE_VALUE);
			VSASSERT(bOK, "Failed to close handle m_hReadPipe!");
		case CS_CREATEEVENT:
			// Successful CreateEvent
			bOK = CloseOneHandle(m_hCancelEvent);
			VSASSERT(bOK, "Failed to close handle m_hCancelEvent!");
		case CS_CREATEALLOC:
		case CS_START:
			bOK = CloseOneHandle(m_hSpawnEvent);
			VSASSERT(bOK, "Failed to close handle m_hSpawnEvent!");
			break;
	}
	
	// reset flag enabling subsequent spawns
	m_fEnableSpawn = TRUE;
	*m_ppSpawner = NULL;
}



void CBldConsoleSpawner::AddSpawnerError(UINT idErr, BSTR bstrHelpID)
{
	CComBSTR bstrMsg;
	bstrMsg.LoadString(idErr);
	CComBSTR bstrProj;
	bstrProj.LoadString(IDS_PROJECT_WORD);
	m_pEC->AddError(bstrMsg, bstrHelpID, bstrProj, 0, NULL );
}

void CBldConsoleSpawner::AddSpawnerWarning(UINT idWarn, BSTR bstrHelpID)
{
	CComBSTR bstrMsg;
	bstrMsg.LoadString(idWarn);
	CComBSTR bstrProj;
	bstrProj.LoadString(IDS_PROJECT_WORD);
	m_pEC->AddWarning(bstrMsg, bstrHelpID, bstrProj, 0, NULL );
}
