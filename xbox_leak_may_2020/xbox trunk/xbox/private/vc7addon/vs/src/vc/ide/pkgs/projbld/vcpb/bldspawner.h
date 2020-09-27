// bldspawner.h
//

#ifndef __BLDSPAWNER_H__
#define __BLDSPAWNER_H__

#pragma once

#include <process.h>
#include <vccolls.h>

// classes defined elsewhere
class CDynamicBuildEngine;

/////////////////////////////////////////////////////////////////////////////

#ifndef __SPAWNER_H__

interface IVCBuildErrorContext;

/////////////////////////////////////////////////////////////////////////////
// define some shared data structures between the VC ide and VcSpawn
// in order to do batched commands via named shared memory (aka mapped files)
#define chMultipleCmdFlag	L'm'
#define chEventFlag			L'e'
#define chToolFlag			L't'
#define chPauseFlag			L'p'
#define chCodePageFlag		L'c'
#define chCmdSep			L'\n'
#define szVCSpawnCmd		L"vcspawn"
#define szVCSpawnEchoCmd	L"~vcecho!"
#define szVCSpawnTimeCmd	L"~vctime!"
// for multiple commands to be spawned, use the following:
// 'vcspawn -e -m commands-separated-by-chCmdSep'
// is the format of a command line that should be used.
//
// The return code of vcspawn is formatted thusly:
//	DWORD	dwRet, where:
//		index of failing command is HIWORD(dwRet)
//		return code of failing command is LOWORD(dwRet)
//	dwRet == 0 implies all commands were successful.

#define CB_COMM_BUF     (2048+1)

#define SPAWN_ERROR_DEPTH 10	// Depth of the error context stack
#define cbThrdStack 32768		// thread stack size

// shared buffer size for communication between spawned process and us
#define cbProcOutputBuf 4096
#define cbPipeBuf       (1L * cbProcOutputBuf)

static const wchar_t szMultCmd[] = { L'-', chMultipleCmdFlag, 0 };

typedef enum CompileState
{
	CS_START,
	CS_CREATEALLOC,
	CS_CREATEEVENT,
	CS_CREATEPIPE,
	CS_DUPLICATEHANDLES,
	CS_CREATENULFILE,
	CS_CREATEPROCESS,
	CS_KILLEDPROCESS,
	CS_SUCCESSFUL
} COMPILESTATE;

// ReadFile for pipes handling is different on Win95 from NT due to bugs in Win95.
BOOL FWin95ReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE hAuxStop);
BOOL FWinNTReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE hAuxStop);
typedef BOOL (*PfnReadPipe)(HANDLE, LPVOID, DWORD, DWORD &, HANDLE);

#endif	// __SPAWNER_H__

///////////////////////////////////////////////////////////////////
// CBldConsoleSpawner

class CBldConsoleSpawner
{
// Construction
public:
	CBldConsoleSpawner(CBldConsoleSpawner **ppSpawner);
	~CBldConsoleSpawner();

// Implementation
	BOOL CanSpawn() { return m_fEnableSpawn; }
	BOOL InitializeSpawn(LPCOLESTR lpszSpawnDir, BOOL bClearOutputWindow, BOOL bSaveAll = TRUE, IVCBuildErrorContext *pEC = NULL);
	BOOL PerformSpawn(LPCOLESTR lpszCommandLine, BOOL bMultipleCommand, CDynamicBuildEngine* pBldEngine, BOOL fUseConsoleCP);
	BOOL PerformSpawnMultiple(CVCStringWList *plistCommands, CDynamicBuildEngine* pBldEngine, BOOL fUseConsoleCP);
	void CancelSpawn();
	BOOL GetConsoleCodePage();
	void DisplayOutputFromConsole(const char *lpszText);
	void CloseDisplay();
	void DoCleanUp();
	void CleanupHandles();
	BOOL CloseOneHandle(HANDLE& hHandle, HANDLE hNewHandleValue = NULL);

public:
	CBldConsoleSpawner **m_ppSpawner;	// back pointer
	HANDLE m_hNotifyEvent;
	HANDLE m_hCleanEvent;
	HANDLE m_hSpawnThread;
	HANDLE m_hWaitThread;
	HANDLE m_hSpawnEvent;
	HANDLE m_hCancelEvent;
	HANDLE m_hReadPipe;
	HANDLE m_hNulStdin;		// handle to null device for input
	BOOL   m_fThreadDie;
	BOOL   m_fEnableSpawn;
	BOOL   m_fUseConsoleCP;
	PROCESS_INFORMATION m_procinfo;
	SECURITY_ATTRIBUTES m_sa;
	int    m_ichOutput;
	char   m_szOutput[CB_COMM_BUF+1];  // yes, ANSI - This is a buffer used from the tool
	DWORD  m_dwTermStatus;

	CStringW   m_strError;
	CStringW   m_strOutput;
	CStringW   m_strSpawnDir;
	CComPtr<IVCBuildErrorContext> m_pEC;

private:
	void AddSpawnerError(UINT idErr, BSTR bstrHelpID);
	void AddSpawnerWarning(UINT idWarn, BSTR bstrHelpID);

	static unsigned _stdcall ReadChildProcessOutput(LPVOID lpv);
	static unsigned _stdcall WaitChildProcessFinish(LPVOID lpv);

	HANDLE m_hpipeWriteOut;	// stdout handle for child process
	HANDLE m_hpipeWriteErr;	// stderr handle for child process
	HANDLE m_hGeneric;

//	LPTERROR  m_hSpawnErrors;

	COMPILESTATE m_CompileState;
	STARTUPINFOW m_startinfo;

//	static BOOL m_fUserWarned;					// has user been warned about missing VCSPAWN.EXE?
	static wchar_t	s_szSpawnToolArgs[];		// string with which CreateProcess string is generated
	static wchar_t	s_szSpawnForCodePageToolArgs[];	// string with which we get a fast call to vcspawn to get the code page
};

///////////////////////////////////////////////////////////////////

#endif	// __BLDSPAWNER_H__
