//
//	PROJCOMP.H
//
//			Intefaces for starting build tools and accessing the IDE.
//
#ifndef _PROJCOMP_H_
#define _PROJCOMP_H_

#ifndef _SUSHI_PROJECT
// If you are encluding this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#include "dllgrid.h"

#include <utilbld_.h>

#include <srcapi.h>
#include <srcguid.h>
#include <dbgpkapi.h>
#include <dbgpguid.h>

class COutputWinEC;

///////////////////////////////////////////////////////////////////////////////
//
//	Class for spawning a process and invoking a tool.  Also, this is the global
//	reposititory about building/spawning state information:  
//
class CSpawner 
{
public:
	CSpawner();

	BOOL CanSpawn ();

	//	Calls InitSpawn down in the IDE
	COutputWinEC * InitSpawn(BOOL bClear = TRUE);
	int DoSpawn
	(
		CPtrList & plCmds,
		CDir & dir,
		BOOL fCheckForComspec = FALSE,
		BOOL fIgnoreErrors = FALSE,
		CErrorContext & EC = g_DummyEC,
		BOOL fAsyncSpawn = TRUE
	);

	void TermSpawn();
	void DoSpawnCancel();
	void ReInitSpawn();
	void GetErrorCount(DWORD & ErrCnt, DWORD & WarnCnt);

	void WriteStringToOutputWindow
	(
		const char *pchar, 
		BOOL bToolError = FALSE,
		BOOL bAddCtlLF  = TRUE
	);
	void WriteStringToOutputWindow(UINT id);

	EXEFROM GetExecutableFilename(PSTR strName, UINT size);
	CDocument * GetLastDocWin();
	BOOL IsSaveBeforeRunningTools(BOOL * pbQuery);

	__inline BOOL SpawnActive() {return m_InitCount > 0;}

	// Our project facility errors/warnings we'd like logged
	// ie. errors/warnings that get added to the tool
	// totals we get back from the spawns that occur in VCPP32
	DWORD m_dwProjError;
	DWORD m_dwProjWarning;

private:
	// OS command shell specification
	TCHAR m_szComspec[_MAX_PATH];

	BOOL m_bBuildTerminated;

	// Count of requests for InitSpawn
	int	m_InitCount;

	// output window ID (for 'Build' pane)
	UINT m_nOutputWindowID;
	// spawner object (defined in shell) used to actually run the build
	CConsoleSpawner *m_pSpawner;
	DWORD m_dwFileWarnings;
	DWORD m_dwFileErrors;
	BOOL  m_fClear;
};

extern CSpawner g_Spawner;

///////////////////////////////////////////////////////////////////////////////
//	Error context that writes to the output window in the IDE.  Note the 
//	the IDE must be in output window "mode" so that the output window is
//	visible, etc:
class COutputWinEC : public CErrorContext 
{
	DECLARE_DYNCREATE (COutputWinEC);
protected:
	CStringList m_StringList;
public:

	virtual void AddString ( const TCHAR *pstr ); 
	virtual void AddString ( UINT ResID ); 

};

///////////////////////////////////////////////////////////////////////////
//	Class to interface to the IDE.

class CVPROJIdeInterface 
{
	friend class CSpawner;

public:
	CVPROJIdeInterface();
	~CVPROJIdeInterface();

	BOOL Initialize();
	void Terminate();

	// cache the spawner interface
	LPEDITDEBUGSTATUS GetEditDebugStatus()
	{
		if (m_pEditDebugStatus == NULL)
			theApp.FindInterface(IID_IEditDebugStatus, (LPVOID FAR *)&m_pEditDebugStatus);
		return(m_pEditDebugStatus);
	}

	void ReleaseEditDebugStatus()
	{
		if (m_pEditDebugStatus != NULL)
			m_pEditDebugStatus->Release();
	}

	// cache the output window interface
	LPOUTPUTWINDOW GetOutputWindow()
	{
		ASSERT(!theApp.m_bInvokedCommandLine);

		if (m_pOutputWindow == NULL)
			theApp.FindInterface(IID_IOutputWindow, (LPVOID FAR *)&m_pOutputWindow);
		return(m_pOutputWindow);
	}

	void ReleaseOutputWindow()
	{
		if (m_pOutputWindow != NULL)
			m_pOutputWindow->Release();
	}

	// cache the DLL grid interface
	LPDLLINFO GetDLLInfo()
	{
		if (m_pDLLInfo == NULL)
			theApp.FindInterface(IID_IDLLInfo, (LPVOID FAR *)&m_pDLLInfo);
		return(m_pDLLInfo);
	}

	void ReleaseDLLInfo()
	{
		if (m_pDLLInfo != NULL)
			m_pDLLInfo->Release();
	}

private:
	LPEDITDEBUGSTATUS	m_pEditDebugStatus;
	LPOUTPUTWINDOW		m_pOutputWindow;
	LPDLLINFO			m_pDLLInfo;
};

extern CVPROJIdeInterface g_VPROJIdeInterface; 

#endif	 // _PROJCOMP_H_
