//
//	PROJCOMP.CPP
//
//		Definition of interfaces defined in PROJCOMP.H
//
#include "stdafx.h"

#pragma hdrstop

#include "projcomp.h"

#include <prjapi.h>
#include <prjguid.h>

IMPLEMENT_DYNAMIC (COutputWinEC, CErrorContext)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static BOOL g_fUserStoppedBuild = FALSE;
HANDLE g_hStdIn = NULL;
extern BOOL g_bHTMLLog;

///////////////////////////////////////////////////////////////////////////////

CVPROJIdeInterface::CVPROJIdeInterface()
{
	m_pEditDebugStatus = NULL;
	m_pOutputWindow = NULL;
	m_pDLLInfo = NULL;
}

CVPROJIdeInterface::~CVPROJIdeInterface()
{
}

BOOL CVPROJIdeInterface::Initialize()
{
	if (theApp.m_bInvokedCommandLine)
		return FALSE;

	VERIFY(GetEditDebugStatus());
	VERIFY(GetOutputWindow());
	GetDLLInfo();
	return((m_pEditDebugStatus != NULL) && (m_pOutputWindow != NULL) /* && (m_pDLLInfo != NULL) */);
}

void CVPROJIdeInterface::Terminate()
{
	ReleaseEditDebugStatus();
	ReleaseOutputWindow();
	ReleaseDLLInfo();
}

CVPROJIdeInterface g_VPROJIdeInterface;
CSpawner g_Spawner;

//	Yield callback function passed to the IDE:
BOOL YieldFunction ()
{
	return theApp.IdleAndPump ();
}
///////////////////////////////////////////////////////////////////////////////
CSpawner::CSpawner()
{
	m_pSpawner = NULL;
	m_InitCount = 0;
	m_bBuildTerminated = FALSE;
}

BOOL CSpawner::CanSpawn()
{
	return(m_pSpawner == NULL);
}

COutputWinEC *CSpawner::InitSpawn(BOOL bClear /* = TRUE */)
{
	m_bBuildTerminated = FALSE;
	m_fClear = FALSE; // bClear;
	m_dwProjError = m_dwProjWarning = 0;
	m_dwFileErrors = m_dwFileWarnings = 0;
	if (!theApp.m_bInvokedCommandLine)
	{
		VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetOutputWindow()->OwinGetBuildCommandID(&m_nOutputWindowID)));
		ASSERT(m_nOutputWindowID != -1);
	}
	else
	{
		m_nOutputWindowID = 0;
	}


	if ((theApp.m_bInvokedCommandLine) && (!g_hStdIn))
	{
		g_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
	}

	if (!m_InitCount)
	{
		LPSOURCECONTROLSTATUS pInterface;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
		BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
		pInterface->Release();
		LPSOURCECONTROL pSccManager;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
		if (bIsSccInstalled) pSccManager->Enable(FALSE);
		pSccManager->Release();

		// our COMSPEC environment variable
		_tcscpy(m_szComspec, NotOnNT() ? "command.com" : "cmd.exe");
		TCHAR * szEnvStr;
		if (szEnvStr = getenv("COMSPEC"))
		{
			// use this and normalise
			(void) _tcscpy(m_szComspec, szEnvStr);
			(void) _tcslwr(m_szComspec);
		}
		  
		if (!theApp.m_bInvokedCommandLine)
		{
			if (bClear)
			{
				g_VPROJIdeInterface.GetOutputWindow()->OutputWindowVwinClear(m_nOutputWindowID);
				// note: window not actually cleared until subsequent SelectVwin call
			}
			// selected output window is brought to the top (and optionally cleared)
			g_VPROJIdeInterface.GetOutputWindow()->OutputWindowSelectVwin(m_nOutputWindowID, TRUE);
		}

		m_InitCount++;
	}

	return (COutputWinEC *)g_buildengine.OutputWinEC();
}

// implemented in project.cpp
extern void GetFirstFile(CString &, CString &, CString &);

#define szVCSpawnEchoCmd	TEXT("~vcecho!")
#define szVCSpawnTimeCmd	TEXT("~vctime!")
#define szVCSpawnSleepCmd	TEXT("~vcsleep!")

// Note the the integer error codes returned by this function are
// actually  exit codes of NTSPAWN.  Look there is you want to know
// what they mean.
int CSpawner::DoSpawn(CPtrList &plCmds, CDir &dir, BOOL fCheckForComspec, BOOL fIgnoreErrors, CErrorContext &EC, BOOL fAsyncSpawn)
{
	// create the spawner object here--InitSpawn() not called for every DoSpawn()
	ASSERT(m_pSpawner == NULL);
	m_pSpawner = new CConsoleSpawner(&m_pSpawner);

	VERIFY(m_pSpawner->InitializeSpawn((LPTSTR)(LPCTSTR)dir, m_nOutputWindowID, m_fClear, FALSE, &EC));
	m_fClear = FALSE;

    // Just join all the lists together for now.
	// NOTE this must be changed to support parrallel stuff.
	int nCount=0;
	POSITION pos1 = plCmds.GetHeadPosition();
    while(pos1){
		CCmdLine *pCmdLine =(CCmdLine *)plCmds.GetNext(pos1);
		nCount += pCmdLine->slCommandLines.GetCount();
	}	

	// anything to do?
	if (nCount == 0)
		return 0;	// ol

	// ASSERT(slCmds.GetCount() == slDesc.GetCount());
	ASSERT(!fIgnoreErrors);	// not supported

	// two words
	// - loword == return value
	// - hiword == command that returned the value
	int nRetval = 0;
	int iCmd = 1;

	CStringList slSpawnCmds;
	CString strDescLine;

	POSITION pos2 = plCmds.GetHeadPosition();
    while(pos2){
		CCmdLine *pCmdLine =(CCmdLine *)plCmds.GetNext(pos2);
		CStringList &slCmds = pCmdLine->slCommandLines;
		CStringList &slDesc = pCmdLine->slDescriptions;
		POSITION posCmd = slCmds.GetHeadPosition();
		POSITION posDesc = slDesc.GetHeadPosition();
		while (posCmd != (POSITION)NULL)
		{
			CString & strCmdLine = slCmds.GetNext(posCmd);
	
			// check to see if we need to prefix with a comspec?
			if (fCheckForComspec)
			{
				// found this command?
				BOOL fGotPath;
	
				// split into command and arguments
				CString strCaller, strArgs;
				GetFirstFile(strCmdLine, strCaller, strArgs);
	
				TCHAR * psz, szCmdPath[_MAX_PATH + 1];
				if (!(fGotPath = SearchPath(NULL, (const TCHAR *)strCaller, _TEXT(".exe"), _MAX_PATH, szCmdPath, &psz)))
						// we failed, so just use the name
						_tcscpy(szCmdPath, (const TCHAR *)strCaller);
	
				// normalize
				// (void) _tcslwr(szCmdPath);
	
				// is this a GUI command? (assume no if not found)
				BOOL fIsGUI = FALSE;
				BOOL fUseComSpec = TRUE;
				if (fGotPath)
				{
					WORD wSubSystem;
					WORD wExeType = GetExeType(szCmdPath, &wSubSystem);
	
					fIsGUI = wExeType == EXE_WIN || ((wExeType == EXE_NT) && (wSubSystem == IMAGE_SUBSYSTEM_WINDOWS_GUI));
					fUseComSpec = wExeType == EXE_NONE || wExeType == EXE_FAIL;
				}
	
				// make sure not executing command shell
				BOOL fIsShell = FALSE;
				if (!fIsGUI)
				{
					CPath pathComspec;
					if (pathComspec.Create(m_szComspec))
					{
						TCHAR szCmdPathTmp[_MAX_PATH + 1];
	
						_tcscpy(szCmdPathTmp, szCmdPath);
						_tcslwr(szCmdPathTmp);
						fIsShell = _tcsstr(szCmdPathTmp, (const TCHAR *)pathComspec) != (const TCHAR *)NULL;
						fUseComSpec = fUseComSpec && !fIsShell;
					}
				}
	
				// use the command-spec if we have re-direction chars.
				fUseComSpec = fUseComSpec || (strCmdLine.FindOneOf(_TEXT("<>|&^")) != -1);
	
				// don't allow spawning just the shell!
				if (fIsShell && strArgs.IsEmpty())
				{
					// fail the execute
					// make a note + index of command for multi-commands
					nRetval = 1 | (iCmd << 16);
				}
	
				// need to prefix with comspec?
				else if (fUseComSpec)
				{
					// re-construct our command-line
					strCmdLine = m_szComspec;
					strCmdLine += _TEXT(" /c ");
					strCmdLine += szCmdPath;
					strCmdLine += _TEXT(" ");
					strCmdLine += strArgs;
				}
			}
	
			// index of the next command
			iCmd += 2;

			// hack for Win95 FAT16 timing problems
			if (pCmdLine->bDelay)
				slSpawnCmds.AddTail(szVCSpawnSleepCmd);
	
			// add the description
			// as '~vcecho<desc>'
			strDescLine = szVCSpawnEchoCmd;
			strDescLine += slDesc.GetNext(posDesc);
			slSpawnCmds.AddTail(strDescLine);
	
			// add the command
			slSpawnCmds.AddTail(strCmdLine);
	
			// make sure we show spawn-time if /Y3
			if (g_bSpawnTime)
				slSpawnCmds.AddTail(szVCSpawnTimeCmd);
		}
		delete pCmdLine;
	}	

	BOOL bUserCancel = FALSE;
	BOOL bSpawnFailed = FALSE;
	g_fUserStoppedBuild = FALSE;
	// already failed the execute in pre-check above?
	if (!nRetval)
	{
#if defined(_DEBUG)
		// message so we de-lineate our vcspawns
		if (g_bSpawnTime)
			EC.AddString(_TEXT("Spawning 'vcspawn'...\n"));
#endif
		m_pSpawner->m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_pSpawner->PerformSpawnMultiple(&slSpawnCmds))
		{
			ASSERT(FALSE);  // Spawner failed!
			m_dwFileErrors += 1;
			bSpawnFailed = TRUE;
		}
		else
		{
			if (fAsyncSpawn)
				theApp.IdleAndPump (m_pSpawner->m_hNotifyEvent);
			else  {
				while (WaitForSingleObject (m_pSpawner->m_hNotifyEvent, 0) == WAIT_TIMEOUT ) 
				{
					MSG msg;
					AfxGetApp ()->DoWaitCursor (1);
					Sleep (1000 / 10);		// 1/10th second
					// If a whole ton of output is coming back from
					// the spawned process, then our Output window code is going to
					// set a timer in order to have the main thread dequeue some
					// of the strings, and then it's going to block.  This
					// produces a deadlock situation.  To hack around this, while
					// we're waiting we dequeue all WM_TIMER messages.
					while (PeekMessage (&msg, 0, WM_TIMER, WM_TIMER, PM_REMOVE))
					{
						if (!theApp.PreTranslateMessage (&msg))
							DispatchMessage (&msg);
					}
				}
			}
			nRetval = m_pSpawner->m_dwTermStatus;
			// check to see if user cancelled the build
			if (WaitForSingleObject(m_pSpawner->m_hCancelEvent, 0) == WAIT_OBJECT_0)
				bUserCancel = TRUE;
			
			ULONG errors = 0;
			ULONG warnings = 0;
			m_pSpawner->GetErrorWarningCount(&errors, &warnings);
			m_dwFileErrors += errors;
			m_dwFileWarnings +=warnings;

			HANDLE hProcess = GetCurrentProcess();
			HANDLE hThread;
			VERIFY(DuplicateHandle(hProcess, m_pSpawner->m_hWaitThread, hProcess,
				&hThread, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS));
			SetEvent(m_pSpawner->m_hCleanEvent);
			WaitForSingleObject(hThread, INFINITE);
			VERIFY(CloseHandle(hThread));
			if( g_fUserStoppedBuild ){
				m_InitCount=0;
			}
		}
	}

	// unpack return value and index of command that failed
	// for multiple commands
	iCmd = (int)((nRetval >> 16) & 0xffff);
	nRetval = (int)(short)(nRetval & 0xffff);

	// why did we fail the execute?
	POSITION pos = slSpawnCmds.FindIndex(iCmd);
	if (nRetval && pos != (POSITION)NULL)
	{
		CString & strCmdLine = slSpawnCmds.GetAt(pos);

		// Get end of exe name:
		const TCHAR * pchCmdLine = (const TCHAR *)strCmdLine;
		unsigned short nExeNameLen = 0;
		const TCHAR * pchEndQuote = 0;
		if (pchCmdLine[0] == '"' && GetQuotedString(pchCmdLine, pchEndQuote)) {
			// ENC: handle quoted commands that may contain spaces
			// 12/22/97 georgiop [vs98 23180]
			nExeNameLen = pchEndQuote - pchCmdLine;
		}
		else while (pchCmdLine[nExeNameLen] != _T('\0') &&
			   !_istspace(pchCmdLine[nExeNameLen])
			  )
		{
			nExeNameLen++;
		}

		// load the error string
		UINT nIDError;
		if (nRetval == -1)
			nIDError = IDS_FAIL_SPAWN;
		else if (bUserCancel)
			nIDError = IDS_TERMINATE_USER_REQ;
		else
			nIDError = IDS_FAIL_EXECUTE;

		CString strErr;
		VERIFY(strErr.LoadString(nIDError));
			
		// format the error message
		TCHAR buf[512];
		_snprintf(buf, 512, strErr, nExeNameLen, nExeNameLen, pchCmdLine);

		g_fUserStoppedBuild = FALSE;
		EC.AddString(buf);
	}
	else if (bSpawnFailed)
	{
		CString strErr;
		VERIFY(strErr.LoadString(IDS_FAIL_VCSPAWN));
		EC.AddString(strErr);
	}

	if (bUserCancel)
		nRetval = -2;

	g_fUserStoppedBuild = FALSE;
	return nRetval;
}

void CSpawner::TermSpawn()
{
//	Called at end of spawning session.
	if (--m_InitCount > 0) return;

	m_InitCount = 0;

	LPSOURCECONTROLSTATUS pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
	pInterface->Release();
	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
	if (bIsSccInstalled) pSccManager->Enable(TRUE);
	pSccManager->Release();
}

void CSpawner::DoSpawnCancel()
{
	m_bBuildTerminated = TRUE;

	ASSERT(m_pSpawner != NULL);
	g_fUserStoppedBuild = TRUE;
	m_pSpawner->CancelSpawn();

	LPSOURCECONTROLSTATUS pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
	pInterface->Release();
	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
	if (bIsSccInstalled) pSccManager->Enable(TRUE);
	pSccManager->Release();
}

void CSpawner::ReInitSpawn ()
{
//	Called after DoSpawnCancel to reinitialise the
//	cancelled spawner
	m_bBuildTerminated = FALSE;

	m_InitCount = 1;

	LPSOURCECONTROLSTATUS pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
	pInterface->Release();
	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
	if (bIsSccInstalled) pSccManager->Enable(FALSE);
	pSccManager->Release();
}

void CSpawner::GetErrorCount(DWORD &dwErrCnt, DWORD &dwWarnCnt)
{
	dwErrCnt = m_dwProjError + m_dwFileErrors;
	dwWarnCnt = m_dwProjWarning + m_dwFileWarnings;
}

EXEFROM CSpawner::GetExecutableFilename(PSTR strName, UINT size)
{
	if (!g_VPROJIdeInterface.Initialize())
		return exefromNone;

	EXEFROM exefrom;
	VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetEditDebugStatus()->GetExecutableFilename(strName, size, &exefrom)));
	return(exefrom);
}

BOOL CSpawner::IsSaveBeforeRunningTools(BOOL *pbQuery)
{
	if (!g_VPROJIdeInterface.Initialize())
		return FALSE;

	return(g_VPROJIdeInterface.GetEditDebugStatus()->IsSaveBeforeRunningTools(pbQuery) == S_OK);
}

CDocument * CSpawner::GetLastDocWin()
{
	if (!g_VPROJIdeInterface.Initialize())
		return NULL;

	CDocument *pDoc;
	VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetEditDebugStatus()->GetLastDocWin(&pDoc)));
	return(pDoc);
}

void CSpawner::WriteStringToOutputWindow(const char *pchar, BOOL bToolError /*= FALSE*/, BOOL bAddCtlLF /*= TRUE*/)
{
	if (theApp.m_bInvokedCommandLine)
	{
		theApp.WriteLog(pchar, bAddCtlLF);
	}
	else
	{
		VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(pchar, bToolError, bAddCtlLF)));
	}
}

void CSpawner::WriteStringToOutputWindow(UINT id)
{
	CString str;
	VERIFY(str.LoadString(id));
	if (theApp.m_bInvokedCommandLine)
	{
		theApp.WriteLog(str, TRUE);
	}
	else
	{
		VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(str, FALSE, TRUE)));
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------ COutputWinEC --------------------------------
//
///////////////////////////////////////////////////////////////////////////////
// An error context for writing to the output window through the CVPROJIdeInterface:
#define MAX_OUTPUT  249		// Max chars debugger can show in output window

void COutputWinEC::AddString ( const TCHAR *pstr )
{
	// write to the output log if there is one.
	LogTrace( "%s\n", pstr );
	
	if (g_fUserStoppedBuild)
		return;

	if (theApp.m_bInvokedCommandLine)
	{
        if (*pstr)
        {
            if (*pstr != '\n')
            {
                theApp.WriteLog(pstr, TRUE);
            }
			else
			{
				theApp.WriteLog("", TRUE);
			}
        }

		// poll stdin for a ctrl-C; stop build if found
		char buffer[80];
		DWORD i, nBytesRead, nBytesAvail, nBytesLeft;
		if ((g_hStdIn != NULL) && (::PeekNamedPipe(g_hStdIn, buffer, 1, &nBytesRead, &nBytesAvail, &nBytesLeft)) && (nBytesRead > 0))

		{
			if (nBytesAvail > 80)
				nBytesAvail = 80;

			if (::ReadFile(g_hStdIn, buffer, nBytesAvail, &nBytesRead, NULL))
			{
				for (i = 0; i < nBytesRead; i++)
				{
					if (buffer[i] == '')
					{
						AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_PROJECT_STOP_BUILD);
						break;
					}
				}
			}
		}
		return;

	}

	TCHAR buf [MAX_OUTPUT+1];
	int i;
	// The Outout window doesn't handle returns very well, so eat
	// return/linefeed pairs and genrate multiple lines:
	while (*pstr)
	{
		i =0;
		while (*pstr && *pstr != 0x0A && *pstr != 0x0D && i < MAX_OUTPUT)
		{
		#ifdef _MBCS
			if (IsDBCSLeadByte((BYTE)*pstr))
							buf[i++] = *pstr++;
			ASSERT (*pstr);	// Second byte shouldn't be null;
		#endif
			buf[i++] = *pstr++;
		}
		buf[i]	= 0;
		VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(buf, TRUE, TRUE)));
		while (*pstr && (*pstr == 0x0A || *pstr == 0x0D) ) 	pstr++;
	}
}

void COutputWinEC::AddString ( UINT ResID )
{
	if (g_fUserStoppedBuild)
		return;

	CString str;
	if (str.LoadString (ResID))
	{
		if (theApp.m_bInvokedCommandLine)
		{
			theApp.WriteLog(str, TRUE);
		}
		else
		{
			VERIFY(SUCCEEDED(g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(str, TRUE, TRUE)));
		}
	}
	else TRACE ("CStorageEC::AddString failed to load string %i\n", ResID );
};

///////////////////////////////////////////////////////////////////////////////
// Log helpers:
#ifdef PROJ_LOG
FILE *g_pLogFile = NULL;
void OpenProjLog(CProject *pProject)
{
	if (g_pLogFile != NULL) return;	// Alread open
	if( g_bWriteBuildLog == FALSE )
		return;

	const CPath *pPath = pProject->GetFilePath();
	CPath FilePath;
	ASSERT (pPath);
	FilePath = *pPath;
	FilePath.ChangeExtension ("plg");
	if ((g_pLogFile = fopen( (const TCHAR *) FilePath, _TEXT("w+t"))) == NULL )
	{
		TRACE ("Opening project log file %s failed\n", (const TCHAR *) FilePath);
		return;
	}
	else
	{
		CString strHeader;
		if( g_bHTMLLog )
			strHeader.LoadString(IDS_HTML_HEADER);
		else
			strHeader.LoadString(IDS_NORMAL_HEADER);
		
		// LogTrace("<html>\n<body>\n<pre>\n<h1>BuildLog</h1>\n");
		LogTrace(strHeader);
	}
}
void CloseProjLog	()
{
	if (!g_pLogFile) return;

	if( g_bHTMLLog ){
		CString strFooter;
		strFooter.LoadString(IDS_HTML_FOOTER);
		LogTrace(strFooter);
	}

	fclose (g_pLogFile);
	g_pLogFile = NULL;
}

#define DBG_BUFSIZE 2048
#define MAX_DBG_BUFSIZE (512*DBG_BUFSIZE)	// 1 meg worth.

void CDECL LogTrace(LPCSTR pszFormat, ...)
{
	int nWritten = -1;
	int nBuf = DBG_BUFSIZE;
	LPTSTR szBuffer = NULL;
	if( g_bWriteBuildLog == FALSE )
		return;

	const TCHAR* pszLocalFormat;

	if (!g_pLogFile) return;   	

	pszLocalFormat = pszFormat;

	va_list args;
	va_start(args, pszFormat);

	//
	// reallocate buffer, write into buffer, loop while buffer too small.
	//
	for (nBuf=DBG_BUFSIZE; -1==nWritten && MAX_DBG_BUFSIZE >= nBuf; nBuf *= 2)
	{
		// reallocate
		delete szBuffer;
		szBuffer = new TCHAR[nBuf];
		if (NULL==szBuffer)
			break;

		//
		// Returns -1 if buffer is too small.
		//
		nWritten = _vsnprintf(szBuffer, nBuf, pszLocalFormat, args);
		szBuffer[nBuf-1] = 0;
		ASSERT(nWritten <= nBuf);
	}

	if (szBuffer==NULL)
	{
		if (fputs ("\nError writing log file\n", g_pLogFile) == EOF)
			CloseProjLog();
	}
	else
	{
		if (fputs (szBuffer, g_pLogFile) == EOF)
			CloseProjLog();

		if (-1==nWritten && fputs ("\nSome information just truncated in log file\n", g_pLogFile) == EOF)
			CloseProjLog();

		delete szBuffer;
	}

	va_end(args);
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// ------------------------------- CTempMaker ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////

CTempMaker::~CTempMaker ()
{
	ASSERT (m_Files.IsEmpty ());
}

BOOL CTempMaker::CreateTempFile (
					CStdioFile *&pFile,
					CPath &rPath,
					CErrorContext& EC, /*=g_DummyEC*/
					BOOL fBatFile /* =FALSE */
					)
{
	TCHAR buf[MAX_PATH *  sizeof (TCHAR)];
	CFileException e;

	// Get the temp path if we don't alread have it:
	if (m_Files.IsEmpty())
	{
		if (!m_DirTempPath.CreateTemporaryName () )
		{
			EC.AddString ( IDS_COULD_NOT_GET_TEMP_PATH );
			// OS call failed, use CWD:
			if (!m_DirTempPath.CreateFromCurrent ())
			{
				EC.AddString ( IDS_COULD_NOT_GET_CWD );
				return FALSE;
			}
		}
	}

	if ( (::GetTempFileName ( (const TCHAR *) m_DirTempPath,_TEXT ("RSP"), 0, buf ) == 0)  || 
		 !rPath.Create ( buf )
	   )
	{
		
		EC.AddString ( IDS_COULD_NOT_GET_TEMP_NAME );
		return FALSE;
	}

	DWORD fOpen = CFile::modeWrite | CFile::typeText;
	if( fBatFile ){
		CStdioFile *pTFile = new CStdioFile;
		pTFile->Open ((const TCHAR *) rPath, fOpen , &e);
		pTFile->Close();
		m_Files.SetAt ( rPath, pTFile );

		rPath.ChangeExtension( ".bat" );
		fOpen |= CFile::modeCreate;
	}

	pFile = new CStdioFile;	

	if ( !pFile->Open ((const TCHAR *) rPath, fOpen , &e) )
	{
		CString str;
		EC.AddString (MsgText ( str, rPath, IDS_COULD_NOT_OPEN_TEMP_FILE, &e));
		delete (pFile);
		return FALSE;
	}

	m_Files.SetAt ( rPath, pFile );
	return TRUE;
}	

// Remove all the files from disk:
void CTempMaker::NukeFiles (CErrorContext& EC/*=g_DummyEC*/)
{
	CPath cp;
	POSITION pos;
	CStdioFile *pFile;

	for (pos = m_Files.GetStartPosition (); pos != NULL; )
	{
		m_Files.GetNextAssoc ( pos, cp, (CObject *&) pFile );

		// Check that the file has alread been closed:
	   	ASSERT (pFile->m_hFile == (UINT) CFile::hFileNull);
	   	delete (pFile);

		if (!::DeleteFile ( (const TCHAR *) cp ))
		{
			CString str;
			DWORD lOsError = ::GetLastError ();
			CFileException e(CFileException::OsErrorToException(lOsError),
				lOsError);
			EC.AddString (
				::MsgText ( str, cp, IDS_COULD_NOT_DELETE_TEMP_FILE, &e)
				);
		}
	}
	m_Files.RemoveAll ();
}

BOOL CTempMaker::CreateTempFileFromString (	
					CString& str,
					CPath &rPath,
					CErrorContext& EC,/*=g_DummyEC*/
					BOOL fBatFile
					)
{
	CStdioFile *pFile;
	if (!CreateTempFile ( pFile, rPath, EC, fBatFile )) return FALSE;
	BOOL bRetval;
	#ifdef PROJ_LOG
	CString strCreate;
	strCreate.LoadString(IDS_CREATING_TEMPFILE);

	PBUILD_TRACE ((const TCHAR *)strCreate,
			(const TCHAR *)	rPath,
			(const TCHAR *) str
			);
	#endif
	TRY
	{
		pFile->WriteString ( str );
		pFile->Close ();
		bRetval = TRUE;
	}
	CATCH_ALL (e)
	{
		#ifdef PROJ_LOG
		PBUILD_TRACE ("Creating temp file failed!!\n");
		#endif

		bRetval = FALSE;
		pFile->Abort ();
	}
	END_CATCH_ALL
   	return bRetval;
}


