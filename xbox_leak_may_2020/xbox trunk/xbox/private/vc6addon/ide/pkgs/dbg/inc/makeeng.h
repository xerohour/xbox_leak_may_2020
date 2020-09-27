/*********************************************************************

	File:   			makeeng.h

	Date created:    	27/8/90

	Author:				Tim Bell

	Description:

	Windows Make Engine API

	Modified:

*********************************************************************/
#ifndef MAKEENG_H
#define MAKEENG_H

typedef enum
{
	EXEC_RESTART,
	EXEC_GO,
	EXEC_STEPANDGO,
	EXEC_TOCURSOR,
	EXEC_TRACEINTO,
	EXEC_TRACEFUNCTION,
	EXEC_STEPOVER,
	EXEC_SRC_STEPOVER,
	EXEC_SRC_TRACEINTO,
	EXEC_SRC_TRACEFUNCTION,
	EXEC_ANIMATE,
	EXEC_STEPTORETURN,
	EXEC_TARGET,
	EXEC_EDITANDCONTINUE,	// Apply code changes, no run
	EXEC_ENC_FORCEGO,		// Apply code changes, then force a "go"
							// to simulate seamless ENC when the debuggee
							// is running
} EXECTYPE;

void PASCAL UpdateProjectState(int doc, LPCTSTR docFileName, BOOL ChangedOnDisk, BOOL ClearDebuggeeState);

void PASCAL ExecDebuggee(EXECTYPE ExecType);

// Return exe name from most recently active, still-open window on an EXE
const CString& SzLastExeWindow(VOID);

// Return whether we're the parent of the specified process
BOOL FParentOfProcess(DWORD pid, const char **pszPath);

// Clear list of PIDs for which we'll be the JIT debugger
void ClearPidExecList(void);

BOOL PASCAL DocInProject(LPCTSTR docFileName);

// Indicates if we need to update the remote target before debugging
extern BOOL fUpdateRemoteTarget;
extern BOOL g_bDebugTime; // Report build timings to the output window.

// Java debugging support
BOOL GetJavaBrowser(CString& strBrowser, CString& strBrowserArgs);
BOOL GetJavaStandalone(CString& strStandalone, CString& strStandaloneArgs);

#endif	// MAKEENG_H
