/////////////////////////////////////////////////////////////////////////////
// target.h
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Interface of the CTarget class

#ifndef __TARGET_H__
#define __TARGET_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "settings.h"
#include "log.h"
#include "toolset.h"

#include "targxprt.h"


/////////////////////////////////////////////////////////////////////////////
// CTarget

class TARGET_CLASS CTarget
{
// ctor/dtor
public:
	CTarget();
	virtual ~CTarget();

// operations
public:
	// initialize the target
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey) = 0;
	
	// get the property page that contains this target's UI
	virtual CPropertyPage* GetPage(void) { return m_ppageTarget; }

	// launch the target
	virtual BOOL Launch(void);
	virtual BOOL Attach(void);
	
	// exit the target
	virtual BOOL Exit(void);

	// apply settings in the given object to the target
	virtual BOOL ApplySettings(CSettings* pSettings);

	virtual BOOL SetEnvironmentVar(LPCSTR pszEnvVar, LPCSTR pszValue);

	// get target information
	CString GetPath(void);
	CString GetFileName(void);

	CSettings* GetSettings(void)					{ ASSERT(m_psettingsTarget); return m_psettingsTarget; }
  virtual CString GetVersion();
	CString GetFullCmdLine(void);

	// set the log the target will use
	virtual BOOL SetLog(CLog* pLog);

// attributes
public:
	// indicate whether the target is available to test
	virtual BOOL IsLaunched(void) { DWORD dwCode; GetExitCodeProcess(m_procinfoTarget.hProcess, &dwCode); return dwCode == STILL_ACTIVE;}

// operations (internal)
protected:
	// initialize the target's default environment
	BOOL InitEnv(void);

	// launch the target
	static UINT WrapLaunchTarget(LPVOID pParam);
  UINT LaunchTarget(void);

	// attach to the target
	static UINT WrapAttachTarget(LPVOID pParam);
	UINT AttachTarget(void);

	//event handlers
private:
	static BOOL CALLBACK FindMsgToHandle(HWND hwnd, CTarget *_this);
	static BOOL CALLBACK FindAssertText(HWND hAssert, CTarget *_this);
	static BOOL CALLBACK FindExceptionText(HWND hAssert, char buffer[]);

	static UINT WrapEventHandler(LPVOID pParam);
	UINT EventHandler(void);
	void HandleAssertion(BOOL bClickIgnore, BOOL bBreak);
	void HandleExceptionDlg(BOOL bIgnore, BOOL bBreak, LPCSTR msgTilte);
protected:
  // create the target's process
  virtual BOOL CreateTargetProcess(LPCSTR szCmdLine, DWORD dwCreationFlags, LPCSTR szEnvBlock, LPCSTR szWorkDir) = 0;

	// helper functions for LaunchTarget
	CString GetEnvBlock(void);

	// helper functions for Launch and Attach
	CString GetThreadResult(HANDLE hThread);

	// get a unique name per target for uniquely named system objects
	CString GetUniqueName(void);

// Debug event handlers
private:
	virtual void  HandleDebugEvents(void);
	virtual DWORD HandleException(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleCreateThread(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleCreateProcess(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleExitThread(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleExitProcess(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleLoadDLL(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleUnloadDLL(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleOutputDebugString(const DEBUG_EVENT& DebugEvent);
	virtual DWORD HandleRIP(const DEBUG_EVENT& DebugEvent);

	LPCSTR GetExceptionDescription(DWORD dwCode);
	CString& GetOutputDebugString(const DEBUG_EVENT& DebugEvent);
	LPCSTR GetRIPDescription(DWORD dwType);
protected:
	// initialize this target's settings
	virtual BOOL InitializeSettings(void);

public:
	void SetHandleMsgTitle(CString &title);
	void SetHandleMsgs(BOOL enablehandling)
	{
		m_handlemsgs=enablehandling; //don't think thread safety is important here
	};
// data
protected:
//ASSERTIONS are not set by SetHandleMsgTitle (always handled)
	CString m_AssertionMsgTitle;
	CString m_AssertionMsgText;
	BOOL m_EnableExceptionMessageHandling;

private:
	BOOL m_handlemsgs;
	HWND m_hAssert;
	int m_nWndtoHandle;
	CString m_HandleMsgTitles[5]; //arbitrary number, maybe increased
public:
	// the number of expected memory leaks (0 - no leaks, -1:turn off memleak report)
	int m_nExpectedLeaks;

// data types
protected:
	enum DebugThreadResult { dtr_success, dtr_invalidevent, dtr_exenotfound, dtr_invalidexe };

// data
protected:
	CSettings* m_psettingsTarget;
	CPropertyPage* m_ppageTarget;

public:
	PROCESS_INFORMATION m_procinfoTarget;

protected:
	HANDLE m_hTerminate;
	HANDLE m_hthreadDebug;

	// storage for unqiue ids
	static int nCount;
	DWORD m_nId;

	// storage for target's environment
	CMapStringToString m_mapEnv;

	// storage for the value of the MFC Tracer flag
	int m_nTracer;

	// store the number of memory leaks
	int m_nLeaks;

	// storage for the OutputDebugString string
	CString m_strDebugOutput;

	// storage for the flag to indicate the process has been created
	// apparently there is a breakpoint exception that is thrown when a process (just the IDE?)
	// starts up. We need to ignore this exception the first time. m_bInitProcess indicates
	// if the target app just started.
	BOOL m_bInitProcess;

	// the log the target will use
	CLog* m_pLog;

	static const LPCTSTR eventTerminate;
	static const LPCTSTR eventTargetSuccess;
	static const LPCTSTR eventTargetFail;

  CString m_strLastCmdLine;  // holds command line of last invocation

	// holds the path the target
	CString m_strPath;
	// holds the filename of the target
	CString m_strFileName;
private:
	HANDLE m_hEventThread; //event thread which handles the unexpected dialogs
	HANDLE m_hTestthread; //used for thread suspension from the event thread
};

#define TRACER_INI		"afx.ini"
#define TRACER_SECTION	"Diagnostics"
#define TRACER_KEY		"TraceEnabled"
#define TRACER_DEFAULT	0
#define TRACER_ON		"1"
#define TRACER_OFF		"0"

#endif // __TARGET_H__
