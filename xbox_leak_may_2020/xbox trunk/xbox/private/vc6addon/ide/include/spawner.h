//	REVIEW(PatBr):  This needs to be drastically simplified and moved
//                  into shlsrvc.h.

// spawner.h
//

#ifndef __SPAWNER_H__
#define __SPAWNER_H__

#include <process.h>

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

#define	KEYENTRY				long	nKEY;

#define	KEYSET(ptr, key) ((ptr) -> nKEY = key)
BOOL AuxPrintf(
	LPSTR text,
	...);

#define	KEYCHECKRET(ptr, key, ret)	\
if(!(ptr) || ((ptr) -> nKEY != key)) \
{ _KeyError(__FILE__, __LINE__); return ret;}

#define	KEY0CHECKRET(ptr, key, ret)	\
if((ptr) && ((ptr) -> nKEY != key)) \
{ _KeyError(__FILE__, __LINE__); return ret;}

#define	KEYCHECK(ptr, key)	\
if(!(ptr) || ((ptr) -> nKEY != key)) { _KeyError(__FILE__, __LINE__); }

#define	KEY0CHECK(ptr, key)	\
if((ptr) && ((ptr) -> nKEY != key)) { _KeyError(__FILE__, __LINE__); }

void	_KeyError(LPSTR pszFileName, UINT nLine);

#else	// !_DEBUG

#define	KEYENTRY
#define	KEYSET(ptr, key)

#define	KEYCHECKRET(ptr, key, ret)
#define	KEY0CHECKRET(ptr, key, ret)
#define	KEYCHECK(ptr, key)
#define	KEY0CHECK(ptr, key)

#endif	// _DEBUG

struct TERROR;
typedef TERROR* PTERROR;
typedef TERROR* LPTERROR;
class CErrorContext;
// Key for the queue structure
#define	ERRORKEY		((long)-857229)

// create and destroy and error context
LPTERROR CreateErrorContext(UINT nErrorDepth, UINT nErrorMin, UINT nErrorMax);
void DestroyErrorContext(LPTERROR);

BOOL PushError(LPTERROR lpterror, ULONG nError);
LONG PopError(LPTERROR lpterror);
BOOL ClearError(LPTERROR lpterror);
INT GetDepth(LPTERROR lpterror);
LONG IndexError(LPTERROR lpterror, UINT nIndex);
int LoadError(HANDLE hInst, UINT nErrorNum, LPSTR szBuffer, size_t nBufsize);

#define	SYSTEM_ERROR	0x10000000

/////////////////////////////////////////////////////////////////////////////

// define some shared data structures between the VC ide and VcSpawn
// in order to do batched commands via named shared memory (aka mapped files)
#define chMultipleCmdFlag	TEXT('m')
#define chEventFlag			TEXT('e')
#define chToolFlag			TEXT('t')
#define chPauseFlag			TEXT('p')
#define chCmdSep			TEXT('\n')
#define szVCSpawnCmd		TEXT("vcspawn")
#define szVCSpawnEchoCmd	TEXT("~vcecho!")
#define szVCSpawnTimeCmd	TEXT("~vctime!")
#define szVCSpawnSleepCmd   TEXT("~vcsleep!")
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

static const TCHAR szSpawnTool[] = "vcspawn -e %lu %s ";
static const TCHAR szMultCmd[] = { TEXT('-'), chMultipleCmdFlag, 0 };
#define cbSpawnExpanded	(sizeof(szSpawnTool) + sizeof(szMultCmd) + 5)

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

///////////////////////////////////////////////////////////////////
// CConsoleSpawner

class CConsoleSpawner
{
// Construction
public:
	CConsoleSpawner(CConsoleSpawner **ppSpawner);
	~CConsoleSpawner();

// Implementation
	BOOL CanSpawn() { return(m_fEnableSpawn); }
	BOOL InitializeSpawn(LPTSTR lpszSpawnDir, UINT nOutputWindowID, BOOL bClearOutputWindow, BOOL bSaveAll = TRUE, CErrorContext *pEC = NULL);
	BOOL PerformSpawn(LPTSTR lpszCommandLine, BOOL bMultipleCommand);
	BOOL PerformSpawnMultiple(CStringList *plistCommands);
	void CancelSpawn();
	void DisplayOutputFromConsole(LPTSTR lpszText);
	BOOL IsItErrorOrWarning(char *pszString);
	void GetErrorWarningCount(DWORD *pdwErrors, DWORD *pdwWarnings);

public:
	CConsoleSpawner **m_ppSpawner;	// back pointer
	HANDLE m_hNotifyEvent;
	HANDLE m_hCleanEvent;
	HANDLE m_hSpawnThread;
	HANDLE m_hWaitThread;
	HANDLE m_hSpawnEvent;
	HANDLE m_hDoneEvent;
	HANDLE m_hCancelEvent;
	HANDLE m_hReadPipe;
	HANDLE m_hNulStdin;		// handle to null device for input
	BOOL   m_fThreadDie;
	BOOL   m_fCloseWindow;
	BOOL   m_fGUITool;
	BOOL   m_fReloadNoPrompt;
	BOOL   m_fEnableSpawn;
	PROCESS_INFORMATION m_procinfo;
	SECURITY_ATTRIBUTES m_sa;
	char   m_szOutput[CB_COMM_BUF+1];
	int    m_ichOutput;
	UINT   m_nOutputWindowID;
	DWORD  m_dwTermStatus;

	CString   m_strError;
	CString   m_strOutput;
	CString   m_strSpawnDir;
	CErrorContext *m_pEC;

private:
	BOOL InitializeInternal(BOOL bClearOutputWindow);
	BOOL PerformInternal(LPTSTR lpszCommandLine, BOOL bMultipleCommand);
	BOOL PerformExternal(LPTSTR lpszCommandLine, BOOL bMultipleCommand);

public:
	static void _cdecl ReadChildProcessOutput2(LPVOID lpv);
private:
	static void _cdecl ReadChildProcessOutput(LPVOID lpv);
	static void _cdecl WaitChildProcessFinish(LPVOID lpv);

	HANDLE m_hpipeWriteOut;	// stdout handle for child process
	HANDLE m_hpipeWriteErr;	// stderr handle for child process
	HANDLE m_hGeneric;
	int    m_nFileWarnings;
	int    m_nFileErrors;

	LPTERROR  m_hSpawnErrors;

	COMPILESTATE m_CompileState;
	STARTUPINFO m_startinfo;

	static BOOL m_fUserWarned;		// has user been warned about missing VCSPAWN.EXE?
	static CHAR m_szSpawnTool[];	// string with which CreateProcess string is generated
};

///////////////////////////////////////////////////////////////////

#endif	// __SPAWNER_H__
