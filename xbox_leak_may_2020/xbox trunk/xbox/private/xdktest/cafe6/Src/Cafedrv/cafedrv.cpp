///////////////////////////////////////////////////////////////////////////////
//  CAFEDrv.CPP
//
//  Created by :            Date :
//      DavidGa                 8/13/93
//
//  Description :
//      Implementation of the CCAFEDriver class
//

#include "stdafx.h"
#include "caferes.h"
#include "cafepage.h"
#include "logpage.h"
#include "suitedoc.h"
#include "suitevw.h"
#include "aplogdlg.h"
#include "cafedrv.h"
#include "mmsystem.h"
#include "cafemdi.h"



#include "logview.h"


#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
//CViewportView * vConsole, * vTextLog ;
// REVIEW(briancr): provided only for backward compatibility
extern AFX_EXT_DATA CSettings* gpsettingsCmdLine;

// the address of this object is passed to a timer function used to slow down the suite thread
SlowSuiteInfo slow_info;

/////////////////////////////////////////////////////////////////////////////
// The one and only CCAFEDriver object

CCAFEDriver CAFEApp;

/////////////////////////////////////////////////////////////////////////////
// CCAFEDriver

BEGIN_MESSAGE_MAP(CCAFEDriver, CWinApp)
    //{{AFX_MSG_MAP(CCAFEDriver)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(IDM_OptionsSingleStep, OnOptionsSingleStep)
    ON_UPDATE_COMMAND_UI(IDM_OptionsSingleStep, OnUpdateOptionsSingleStep)
    ON_COMMAND(IDM_OptionsBreakStep, OnOptionsBreakStep)
    ON_COMMAND(IDM_OptionsToolset, OnOptionsToolset)
    ON_COMMAND(IDM_FileViewLog, OnViewLog)
	ON_COMMAND(IDM_OptionsSave,OnOptionSave) //REVIEW(chriskoz) TEMP: hooked up the machine name. only a proof of concept

    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAFEDriver construction

CCAFEDriver::CCAFEDriver()
{
    char szDriverFileName[MAX_PATH];
    char szDrive[_MAX_DRIVE];
    char szDir[_MAX_DIR];

    m_pMainWnd = NULL;
    // store the starting directory
    ::GetModuleFileName(::GetModuleHandle(NULL), szDriverFileName, MAX_PATH);
    _splitpath(szDriverFileName, szDrive, szDir, NULL, NULL);
    m_strDriverDir = CString(szDrive) + CString(szDir);
    ::GetCurrentDirectory(MAX_PATH, m_strWorkingDir.GetBuffer(MAX_PATH));
    m_strWorkingDir.ReleaseBuffer();
    m_strWorkingDir += '\\';

    // build the help filename
    m_strHelpFile = m_strDriverDir + "cafe.hlp";

    // tell MFC the name of our help file
    m_pszHelpFilePath = (LPCTSTR)_tcsdup(m_strHelpFile);

    m_idLagTimer = 0;
	m_bHotKey = TRUE;

	// determine whether we're on Win95 or not
	DWORD dwVersion = ::GetVersion();
	m_bOnWin95 = (dwVersion & 0x80000000) && (LOBYTE(dwVersion) > 3);
}

CCAFEDriver::~CCAFEDriver()
{
    if(m_pMainWnd != NULL)
        delete m_pMainWnd;

    // delete settings objects
    if (gpsettingsCmdLine) {
        delete gpsettingsCmdLine;
		gpsettingsCmdLine=NULL;
    }
    if (m_pstngCAFEDrv) {
        delete m_pstngCAFEDrv;
    }
    if (m_pstngLog) {
        delete m_pstngLog;
    }
    if (m_pstngSingleStep) {
        delete m_pstngSingleStep;
    }

/*
    // delete toolset
    if (m_pToolset) {
        delete m_pToolset;
    }
*/
}

/////////////////////////////////////////////////////////////////////////////
// CCAFEDriver initialization
//REVIEW(this method of restricting to single instance was replaced by a mutex
//#pragma data_seg("Share")
//LONG g_CAFERunning=-1;
//#pragma data_seg()
//#pragma comment(linker, "-section:Share,RWS")
//BOOL bFirstInstance=0;

BOOL CCAFEDriver::InitInstance()
{
#if 0 //old method, not recommended
	bFirstInstance=(::InterlockedIncrement(&g_CAFERunning)==0);
	if(!bFirstInstance)
	{
		::AfxMessageBox("CAFE is running. OK to switch focus");
		HWND hTopwnd=::GetTopWindow(NULL);
		char szFound[81];
		while(hTopwnd!=NULL)
		{
			::GetWindowText(hTopwnd, szFound, 80);
			if(strstr(szFound,m_pszAppName)!=NULL) //found the window "CAFE v6"
			{
				if(::IsIconic(hTopwnd))
					::OpenIcon(hTopwnd);
				::SetForegroundWindow(hTopwnd);
				::BringWindowToTop(hTopwnd);
				break;
			}
			hTopwnd=::GetNextWindow(hTopwnd,GW_HWNDNEXT);
		}
		::InterlockedDecrement(&g_CAFERunning);
		return FALSE;
	}
#else
	m_hmutexStartup = CreateMutex(NULL, FALSE, STARTUPMUTEXNAME);
	ASSERT(m_hmutexStartup);
	// Grab the mutex without waiting.  If we fail because another copy of
	// CAFEDRV, CAFEMON, CAFECMD has the mutex, then exit.
	if (WaitForSingleObject(m_hmutexStartup, 0) != WAIT_OBJECT_0)
	{
		CloseHandle(m_hmutexStartup);
		::AfxMessageBox("Other instance of CAFE is running. Please close all types of drivers.");
		m_hmutexStartup=NULL;
		return FALSE;
	}
#endif
    LPCSTR pszBanner1 = "=========================================";
    LPCSTR pszBanner2 = "== CAFE Driver -- %s -- %s ==";
    char acTime[10];
    char acDate[10];

    // emit banner to the app log
    APPLOG(pszBanner1);
    _strtime(acTime);
    _strdate(acDate);
    APPLOG(pszBanner2, acTime, acDate);
	APPLOG("== Process ID: %x", GetCurrentProcessId());
	APPLOG("== Process Handle: %x", GetCurrentProcess());
    APPLOG(pszBanner1);

    // Initialize OLE 2.0 libraries
    if (!AfxOleInit())
    {
	AfxMessageBox(IDP_OLE_INIT_FAILED);
	return FALSE;
    }

    Enable3dControls();
	// Load the icons for the tree view.
	hIfail = AfxGetApp() ->LoadIcon( IDI_FAILED) ;
	hIpass = AfxGetApp() ->LoadIcon( IDI_PASS) ;

    // read the command line
    m_pstngCmdLine = ReadCmdLineSwitches(m_lpCmdLine);
    // REVIEW(briancr): initialize the global cmd line settings object in the support system
    // this is for backward compatibility
    gpsettingsCmdLine = m_pstngCmdLine;

    // initialize the driver settings
    m_pstngCAFEDrv = InitCAFEDrvSettings();
    m_strKeyName = m_pstngCAFEDrv->GetTextValue(settingCAFEKey);

	// did the user specify that CAFE's key should be deleted (-newkey)?
	if (m_pstngCmdLine->GetBooleanValue(settingNewKey)) {
		// delete CAFE's key
		DeleteKey(m_strKeyName);
	}

    // tell MFC the name of the registry key
    SetRegistryKey("Microsoft");
    m_pszProfileName = (LPCSTR)_tcsdup(m_strKeyName);

    // load standard settings from the registry
    // and enable the MRU list
    LoadStdProfileSettings(16);

    // initialize the log settings
    m_pstngLog = InitLogSettings();

    // initialize the single step settings
    m_pstngSingleStep = InitSingleStepSettings();

    // add test collection doc/view to the document template
    CMultiDocTemplate* pmdtSuite = new CMultiDocTemplate(IDR_SUITETYPE, RUNTIME_CLASS(CSuiteDoc), RUNTIME_CLASS(CCafeDrvMDIChild), RUNTIME_CLASS(CSuiteView));
    AddDocTemplate(pmdtSuite);
    // add subsuite doc/view to the document template
	CMultiDocTemplate* pmdtDLL = new CMultiDocTemplate(IDR_DLLTYPE, RUNTIME_CLASS(CSuiteDoc), RUNTIME_CLASS(CCafeDrvMDIChild), RUNTIME_CLASS(CSuiteView));
	AddDocTemplate(pmdtDLL);

 // add viewport doc/view to the document template
	CMultiDocTemplate* pmdtLogDoc = new CMultiDocTemplate(IDR_PORTTYPE, RUNTIME_CLASS(CViewportDoc), RUNTIME_CLASS(CMDIChildWnd),RUNTIME_CLASS( CLogView));
	AddDocTemplate(pmdtLogDoc);	 

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;

    pMainFrame->ShowWindow(m_nCmdShow);
    m_pMainWnd = pMainFrame;

	// open .agn file or prompt user to open one
//      OnFileOpen();

    // REVIEW(briancr): in order to use the old single step window, I've added this code
	m_bHotKey = m_pstngCmdLine->GetBooleanValue("hotkey_thread", TRUE);  //disable StepWnd (ie -hotkey_thread:false)
	if (m_bHotKey) //is StepWnd disabled? (ie -hotkey_thread:false)
	{
		m_pthreadHotKey = (CHotKeyThread*)AfxBeginThread(RUNTIME_CLASS(CHotKeyThread), NULL);
		m_pthreadHotKey->m_bAutoDelete = FALSE;
	}

    m_hSuiteReadyEvent = CreateEvent(
    0, // security attributes
    TRUE, // manual reset; we have to call ResetEvent()
    FALSE, // nonsignaled initially
    0);  // make it anonymous

    m_hTerminationEvent = CreateEvent(
    0, // security attributes
    TRUE, // manual reset; we have to call ResetEvent()
    FALSE, // nonsignaled initially
    0);  // make it anonymous

	// start the thread that will run the suites
	m_threadSuite = AfxBeginThread(WrapSuiteExecutionLoop, this);
	m_threadSuite->m_bAutoDelete = FALSE;
	// verify that the thread was created
	if (m_threadSuite == NULL) {
		ErrorMsg("Unable to create the thread that runs the suite. GetLastError = %d. Terminating.", ::GetLastError());
		::PostQuitMessage(2);
	}

    // set up timer to slow down execution of suite thread
	slow_info.m_hthread = m_threadSuite->m_hThread;
    slow_info.m_delay = m_pstngCmdLine->GetIntValue("delay", 0);
    int period = m_pstngCmdLine->GetIntValue("period", 50);

    if(slow_info.m_delay)
    {
        APPLOG("Setting up lag timer.  period = %d, delay = %d", period, slow_info.m_delay);
        VERIFY(timeBeginPeriod(10)!= TIMERR_NOCANDO);
        VERIFY(m_idLagTimer = timeSetEvent(period, 0, SlowSuiteTimeProc, (DWORD)&slow_info, TIME_PERIODIC));
        if (m_idLagTimer == NULL)
            APPLOG("Couldn't create the lag timer.");
    }

    return TRUE;
}

int CCAFEDriver::Run()
{
    CSuiteDoc* pDoc = NULL;

    // open the stf file the user specified on the cmd line
    if (m_pstngCmdLine->IsDefined(settingSTFFile)) {
        pDoc = (CSuiteDoc*)OpenDocumentFile(m_pstngCmdLine->GetTextValue(settingSTFFile));
    }

    // if -run is specified, -stf must be specified as well
    if (m_pstngCmdLine->GetBooleanValue(settingRun)) {
        if (pDoc) {
            pDoc->Run();
        }
        else {
            AfxMessageBox("No STF file specified with -run.");
        }
    }

    // call the base class
    return CWinApp::Run();
}

/*  ExitInstance
 *      We want WinMain to return the number of failed tests, so that zero
 *      means "Everything Passed", >0 means "something failed", and <0 is
 *      undefined.
 *      The code below can thus return an error code in one of two senses:
 *      If CWinApp::ExitInstance != 0, then we return that.  Otherwise, we
 *      return our own number of failed tests.
 */
int CCAFEDriver::ExitInstance()
{
	if(m_hmutexStartup==NULL) //not the first instance - nothing was initialized whan failed to grab STARTUPMUTEXTNAME
	{
	    CWinApp::ExitInstance();
		return 0;
	}
	const char* const THIS_FUNCTION = "CCAFEDriver::ExitInstance";
	DWORD dwExitCode;

    // Kill the lag timer so that after we end the thread there's no
    // chance that the timer function will get called and try to
    // suspend the defunct m_pSuiteThread.
	if (m_idLagTimer != NULL)
	{
		timeKillEvent(m_idLagTimer);
		timeEndPeriod(10);
	}

    // Make sure the suite thread get's aborted if it's still running.  Originally
    // tried to do a fancier wait for the thread to end by first signalling the
    // g_hUserAbort event, but this event only triggers a test exception, and seems to
    // allow the subsuite to keep running.  I (JG) didn't have time to look into it
    // further, so I decided to just bite the bullet and terminate the thread right
    // here.  Review at some time in the future for a cleaner method of ending this
    // thread.
    
	// tell the suite thread to terminate
	SetEvent(m_hTerminationEvent);  // for our worker thread
	// wait for the suite thread to terminate
	if (m_threadSuite) //is m_threadSuite created?
	{
	if (WaitForSingleObject(m_threadSuite->m_hThread, 500) != WAIT_OBJECT_0) {
		TRACE("%s: timeout waiting for the suite thread to terminate.\n", THIS_FUNCTION);

		// if the suite thread is still running, terminate it
		GetExitCodeThread(m_threadSuite->m_hThread, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
		{
			APPLOG("%s: Forcing termination of suite thread (%lx)...", THIS_FUNCTION, m_threadSuite->m_nThreadID);
			TerminateThread(m_threadSuite->m_hThread, DWORD(-1));
		}
	}
	delete m_threadSuite;
	}

	if (m_bHotKey) //is StepWnd disabled? (ie -hotkey_thread:false)
	{
		// close the hot key window
		m_pthreadHotKey->GetHKWnd()->SendMessage(WM_CLOSE);

		// wait for the thread to terminate
		if (WaitForSingleObject(m_pthreadHotKey->m_hThread, 500) != WAIT_OBJECT_0) {
			TRACE("%s: timeout waiting for the hot key thread to terminate.\n", THIS_FUNCTION);

			// Make sure the hotkey thread get's aborted if it's still running.
			GetExitCodeThread(m_pthreadHotKey->m_hThread, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE)
			{
				APPLOG("%s: Forcing termination of hot key thread (%lx)...", THIS_FUNCTION, m_pthreadHotKey->m_nThreadID);
				TerminateThread(m_pthreadHotKey->m_hThread, DWORD(-1));
			}
		}
	
	    delete m_pthreadHotKey;

	}

    ::CloseHandle(m_hSuiteReadyEvent);
    ::CloseHandle(m_hTerminationEvent);
#if 0 //REVIEW(chriskoz) - forget about pupil.don, we've abandoned TP3 plans
    // write out pupil.don file for TP/3
    // REVIEW(briancr): for now we'll always write out a 0 (which means done)
    CStdioFile file;
    if (file.Open(m_strWorkingDir + "pupil.don", CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
        file.WriteString("0");
        file.Close();
    }
#endif
    CWinApp::ExitInstance();

	SaveStdProfileSettings();
    if (gpsettingsCmdLine) {
		delete gpsettingsCmdLine;
		gpsettingsCmdLine=NULL;
    }
	ReleaseMutex(m_hmutexStartup);
	CloseHandle(m_hmutexStartup);
	APPLOG("Exiting %s", THIS_FUNCTION);
    return 0;
}

/*
BOOL CCAFEDriver::OnIdle(LONG lCount)
{
    POSITION pos;
    CMultiDocTemplate* pTemplate;
    CViewportDoc* pDoc;
    CString strDocName;

    // call the base class
    if (!CWinApp::OnIdle(lCount)) {

        // when the base class finishes doing idle time processing,
        // we'll do ours

        // find the template for viewport doc/views
        for (pos = m_templateList.GetHeadPosition(); pos != NULL; ) {

            // get an element from the template list
            pTemplate = (CMultiDocTemplate*)m_templateList.GetNext(pos);

            // is it the viewport template
            // REVIEW(briancr): can we get "Viewport" from the string table?
            pTemplate->GetDocString(strDocName, CDocTemplate::docName);
            if (strDocName == "Viewport") {
                break;
            }
        }

        // iterate through all viewport documents and update their views
        for (pos = pTemplate->GetFirstDocPosition(); pos != NULL; ) {
            pDoc = (CViewportDoc*)pTemplate->GetNextDoc(pos);

            // update this doc's view
            pDoc->UpdateViews();
        }
    }

    return TRUE;
}
*/


///////////////////////////////////////////////////////////////////////////////
// Operations


///////////////////////////////////////////////////////////////////////////////
// CCAFEDriver commands

CViewportDoc* CCAFEDriver::NewViewport(void)
{
    CMultiDocTemplate *pTemplate;
    CString strDocName;

    // iterate through the template list until the viewport template is found
    for (POSITION pos = GetFirstDocTemplatePosition(); pos != NULL; ) {
        // get an element from the template list
        pTemplate = (CMultiDocTemplate*)GetNextDocTemplate(pos);

        // is it the viewport template
        // REVIEW(briancr): can we get "Viewport" from the string table?
        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
        if (strDocName == "Viewport") {
		return (CViewportDoc*)pTemplate->CreateNewDocument( );

//		return (CViewportDoc*)pTemplate->OpenDocumentFile (NULL);
        }

    }

    // we should *always* find the viewport doc template
    ASSERT(0);
    return NULL;
}


void CCAFEDriver::ErrorMsg(UINT nIDPrompt, ...)
{
    CString strErr;
    CString strPrompt;
    va_list vaMarker;

    // load the prompt string from the resource
    strPrompt.LoadString(nIDPrompt);

    // point to the first var arg
    va_start(vaMarker, nIDPrompt);
    // printf into strErr
    _vsnprintf(strErr.GetBuffer(1023), 1023, strPrompt, vaMarker);
    strErr.ReleaseBuffer();
    // reset the var arg pointer
    va_end(vaMarker);

    // display the error
    AfxMessageBox(strErr);
}

void CCAFEDriver::ErrorMsg(LPCSTR szMsg, ...)
{
	const int cBuf = 1000;
	char aBuf[cBuf];
    va_list vaMarker;

    // point to the first var arg
    va_start(vaMarker, szMsg);
    // printf into strErr
    _vsnprintf(aBuf, cBuf, szMsg, vaMarker);
    // reset the var arg pointer
    va_end(vaMarker);

	// write the message to the app log
	APPLOG("CAFE Driver error: %s", aBuf);

	// if we're not in automated mode, display the error
    if (!m_pstngCmdLine->GetBooleanValue(settingRun, FALSE)) {
		AfxMessageBox(aBuf, MB_ICONEXCLAMATION | MB_OK);
	}
}

void CCAFEDriver::Message(LPCSTR szMsg, ...)
{
	const int cBuf = 1000;
	char aBuf[cBuf];
    va_list vaMarker;

    // point to the first var arg
    va_start(vaMarker, szMsg);
    // printf into strErr
    _vsnprintf(aBuf, cBuf, szMsg, vaMarker);
    // reset the var arg pointer
    va_end(vaMarker);

	// write the message to the app log
	APPLOG("CAFE Driver message: %s", aBuf);

	// if we're not in automated mode, display the error
    if (!m_pstngCmdLine->GetBooleanValue(settingRun, FALSE)) {
		AfxMessageBox(aBuf, MB_ICONINFORMATION | MB_OK);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCAFEDriver commands

/*
int CCAFEDriver::GetSysLangOffset(void)
{
    if( GetSystem() & SYSTEM_JAPAN )
        return LANG_JAPAN;
    else if( GetSystem() & SYSTEM_DBCS )
        return 0;   // TODO - other DBCS languages
    else
        return 0;
}
*/



void CCAFEDriver::OnAppAbout()
{
    CDialog dlg(IDD_ABOUTBOX);
    dlg.DoModal();
}


CToolset* CCAFEDriver::NewToolset(HANDLE hOutput)
{
    CString strToolset;
    CToolset* pToolset;

    strToolset = m_pstngCAFEDrv->GetTextValue(settingPlatform);

    // COMPATIBILITY(briancr): for backward compatibility, we must put the currently
    // selected toolset on the command line so that the COConnection stuff will know
    // what the currently selected toolset/platform is.
    m_pstngCmdLine->SetTextValue(settingPlatform, strToolset);

    CPlatform::COS *pOS;
    CPlatform::CLanguage *pLang;

    pOS = GetOSFromShortName(strToolset);
    pLang = GetLangFromName(m_pstngCAFEDrv->GetTextValue(settingLanguage));

    pToolset = CreateToolset(pOS, pLang, m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);

#if 0
    // figure out which toolset we want and instantiate it
    if (strToolset.CompareNoCase("mac_68k") == 0)
        pToolset = new C68KTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
    else if (strToolset.CompareNoCase("mac_ppc") == 0) 
		pToolset = new CPMacTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
    else if (strToolset.CompareNoCase("win32_mips") == 0) 
        pToolset = new CMIPSTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
    else if (strToolset.CompareNoCase("win32_alpha") == 0)
        pToolset = new CAlphaTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
    else if (strToolset.CompareNoCase("win32_ppc") == 0)
		pToolset = new CPowerPCTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
    else if (strToolset.CompareNoCase("win32s_x86") == 0)
		pToolset = new CWin32sTools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
	else
		pToolset = new CX86Tools(m_pstngCAFEDrv->GetBooleanValue(settingDebugBuild)? CToolset::Debug : CToolset::Retail, hOutput);
#endif


    // initialize the toolset
    pToolset->Initialize(m_pstngCAFEDrv->GetTextValue(settingCAFEKey));

    // apply the command line to this toolset
    pToolset->ApplySettings(m_pstngCmdLine);

    // apply CAFEDrv settings to this toolset
    // REVIEW(briancr): actually we're only interested in the settingDebugBuild and settingLanguage settings
    // we could build a settings object that contains this subset and apply that...
    pToolset->ApplySettings(m_pstngCAFEDrv);

#if 0

    // set the language in the toolset
    if (m_pstngCAFEDrv->GetTextValue(settingLanguage) == "English")
        pToolset->SetLanguage(CToolset::TL_ENGLISH);
    else if (m_pstngCAFEDrv->GetTextValue(settingLanguage) == "German")
        pToolset->SetLanguage(CToolset::TL_GERMAN);
    else if (m_pstngCAFEDrv->GetTextValue(settingLanguage) == "Japanese")
        pToolset->SetLanguage(CToolset::TL_JAPANESE);

#endif

    return pToolset;
}

BOOL CCAFEDriver::DeleteToolset(CToolset* pToolset)
{
    // the toolset must not be NULL
    ASSERT(pToolset);
    if (!pToolset) {
        return FALSE;
    }

    delete pToolset;

    return TRUE;
}

/*
BOOL CCAFEDriver::ChangeToolset(CString strToolset)
{
    // the toolset name must not be empty
    ASSERT(!strToolset.IsEmpty());

    // if we've already instantiated a toolset, delete it
    if (m_pToolset) {
        delete m_pToolset;
    }

    // figure out which toolset we want and instantiate it
    if (strToolset.CompareNoCase("mac_68k") == 0) {
        m_pToolset = new C68KTools;
    }
    else {
        if (strToolset.CompareNoCase("mac_ppc") == 0) {
            m_pToolset = new CPMacTools;
        }
        else {
            if (strToolset.CompareNoCase("win32_mips") == 0) {
                m_pToolset = new CMIPSTools;
            }
            else {
                m_pToolset = new CX86Tools;
            }
        }
    }

    // initialize the toolset
    m_pToolset->Initialize(m_pstngCAFEDrv->GetTextValue(settingCAFEKey));

    // apply the command line to this toolset
    m_pToolset->ApplySettings(m_pstngCmdLine);

    return TRUE;
}
*/

CSettings* CCAFEDriver::InitCAFEDrvSettings(void)
{
    CSettings* pstngCAFEDrv;
    CString strToolset;

    // there must be a command line settings object
    ASSERT(m_pstngCmdLine);

    // create a settings object for the driver settings
    pstngCAFEDrv = new CSettings(m_pstngCmdLine->GetTextValue(settingCAFEKey, m_pszAppName)); //by default "CAFE v?"

    // set up driver defaults
    pstngCAFEDrv->SetTextValue(settingCAFEKey, m_pstngCmdLine->GetTextValue(settingCAFEKey, m_pszAppName)); //by default "CAFE v?"
    pstngCAFEDrv->SetTextValue(settingPlatform, "Win32_x86");
    pstngCAFEDrv->SetBooleanValue(settingDebugBuild, TRUE);
    pstngCAFEDrv->SetTextValue(settingLanguage, "English");

    // Initialize globals used by CAFE and the tests.
	g_bInfiniteIdleWaits = m_pstngCmdLine->GetBooleanValue(settingInfiniteIdleWaits, FALSE);
	g_nSleepModifier = m_pstngCmdLine->GetIntValue(settingSleep, 100);

    // REVIEW(briancr): in order to use the old single step window, I've added this code
    pstngCAFEDrv->SetBooleanValue(settingDoSingleStep, FALSE);

    // read registry settings for the driver
    pstngCAFEDrv->ReadRegistry();
    // write the settings back out to initialize registry
    pstngCAFEDrv->WriteRegistry();

    // apply command line overrides
    pstngCAFEDrv->SetTextValue(settingPlatform, m_pstngCmdLine->GetTextValue(settingPlatform, pstngCAFEDrv->GetTextValue(settingPlatform)));
    pstngCAFEDrv->SetBooleanValue(settingDebugBuild, m_pstngCmdLine->GetBooleanValue(settingDebugBuild, pstngCAFEDrv->GetBooleanValue(settingDebugBuild)));
    pstngCAFEDrv->SetTextValue(settingLanguage, m_pstngCmdLine->GetTextValue(settingLanguage, pstngCAFEDrv->GetTextValue(settingLanguage)));

    #ifdef _DEBUG
        pstngCAFEDrv->DumpSettings("CAFEDrv settings");
    #endif // _DEBUG

    return pstngCAFEDrv;
}

CSettings* CCAFEDriver::InitLogSettings(void)
{
    CSettings* pstngLog;
    int nMaxFail;
    BOOL bPost;

    // there must be a command line settings object
    ASSERT(m_pstngCmdLine);
    // there must be a CAFEDrv settings object
    ASSERT(m_pstngCAFEDrv);

    // create a settings object for the log settings
    pstngLog = new CSettings(m_pstngCAFEDrv->GetTextValue(settingCAFEKey)+"\\Logging");

    // set up driver defaults
    pstngLog->SetTextValue(settingLogResultsFile, "results.log");
    pstngLog->SetTextValue(settingLogSummaryFile, "summary.log");
    pstngLog->SetTextValue(settingLogWorkDir, ".");
    pstngLog->SetIntValue(settingLogMaxFail, 100);
    pstngLog->SetBooleanValue(settingLogUseCom, FALSE);
    pstngLog->SetTextValue(settingLogCom, "Com1");
    pstngLog->SetTextValue(settingLogBaudRate, "57600");
    pstngLog->SetBooleanValue(settingLogPost, FALSE);
    pstngLog->SetBooleanValue(settingLogComments, FALSE);

    // REVIEW(briancr): any other defaults?

    // read registry settings for the driver
    pstngLog->ReadRegistry();
    // write the settings back out to initialize registry
    pstngLog->WriteRegistry();

    // apply command line overrides

	CString logfile;
	logfile=m_pstngCmdLine->GetTextValue(settingLogResultsFile);
	if(!logfile.IsEmpty())
		pstngLog->SetTextValue(settingLogResultsFile, logfile);
	nMaxFail = m_pstngCmdLine->GetIntValue(settingLogMaxFail);
    if (nMaxFail != 0) {
        pstngLog->SetIntValue(settingLogMaxFail, nMaxFail);
    }
    bPost = m_pstngCmdLine->GetBooleanValue(settingLogPost);
    if (bPost) {
        pstngLog->SetBooleanValue(settingLogPost, bPost);
    }

    #ifdef _DEBUG
        pstngLog->DumpSettings("Log settings");
    #endif // _DEBUG

    return pstngLog;
}

CSettings* CCAFEDriver::InitSingleStepSettings(void)
{
    return NULL;
}

BOOL CCAFEDriver::DocToRegion(CDocument *pDoc, ClientRegion eRegion)
{
  // we just position the first view

  POSITION pos = pDoc->GetFirstViewPosition();

  ASSERT(pos);

  CView *pV = pDoc->GetNextView(pos);

  ASSERT(pV);
  ASSERT(pV -> GetParentFrame());

  if (!MoveToRegion(pV -> GetParentFrame(), eRegion))
    return FALSE;

//  pDoc->UpdateAllViews(NULL);

  return TRUE;
}


BOOL CCAFEDriver::MoveToRegion(CWnd *pWnd, ClientRegion eRegion)
{
  CRect rect;
  int nLeft, nTop, nWidth, nHeight, nMidX, nMidY;

  ::GetClientRect(((CMDIFrameWnd*)m_pMainWnd) -> m_hWndMDIClient, rect);

  nMidX = rect.right >> 2;   // left 1/4
  nMidY = (rect.bottom << 1) / 5;  // top 2/5

  switch(eRegion)
  {
    case Suite:
    {
      nLeft = nTop = 0;
      nWidth = nMidX;
      nHeight = nMidY;
      break;
    }
    case Console:
    {
      nTop = 0;
      nLeft = nMidX;
      nWidth = rect.right - nLeft;
      nHeight = nMidY;
      break;
    }
    case Log:
    {
      nTop = nMidY;
      nLeft = 0;
      nWidth = rect.right;
      nHeight = rect.bottom - nMidY;
      break;
   }
    default:
      return FALSE;
  }

  BOOL bResult = pWnd -> SetWindowPos(0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER);

  return bResult;
}


BOOL CCAFEDriver::DeleteKey(LPCSTR pszKeyName)
{
	const char* const THIS_FUNCTION = "CCAFEDriver::DeleteKey";
	const char* const pszBaseKey = "Software\\Microsoft\\";
	BOOL bResult = TRUE;
	HKEY hKey;

	APPLOG("Deleting key '%s'.", pszKeyName);
	// open the key, if it exists
	if (RegOpenKeyEx(HKEY_CURRENT_USER, CString(pszBaseKey) + pszKeyName, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
		// delete all the subkeys in this key
		DeleteSubKeys(hKey);
		RegCloseKey(hKey);
		// delete the key itself
		if (RegOpenKeyEx(HKEY_CURRENT_USER, pszBaseKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
			if (RegDeleteKey(hKey, pszKeyName) != ERROR_SUCCESS) {
				TRACE("%s: unable to delete key '%s'.", THIS_FUNCTION, pszKeyName);
				bResult = FALSE;
			}
			RegCloseKey(hKey);
		}
		else {
			TRACE("%s: unable to open key '%s'.", THIS_FUNCTION, pszBaseKey);
			bResult = FALSE;
		}
	}
	else {
		TRACE("%s: unable to open key '%s'.", THIS_FUNCTION, (LPCSTR)(CString(pszBaseKey) + pszKeyName));
		bResult = FALSE;
	}

	return bResult;
}

void CCAFEDriver::DeleteSubKeys(HKEY hKey)
{
	const char* const THIS_FUNCTION = "CCAFEDriver::DeleteSubKeys";

	DWORD iSubKey = 0;
	TCHAR szSubKey[MAX_PATH+1];
	CStringArray* pKeyArray = new CStringArray;

	// build an array of subkeys in this key
	while (RegEnumKey( hKey, iSubKey++, szSubKey, MAX_PATH ) == ERROR_SUCCESS) {
		pKeyArray->Add(szSubKey);
	}

	// iterate through the subkeys in this key
	for (int iCur = 0, iTop = pKeyArray->GetUpperBound(); iCur <= iTop; iCur++) {
		HKEY hSubKey;
		// open the subkey
		if (RegOpenKeyEx(hKey, (*pKeyArray)[iCur], 0, KEY_WRITE | KEY_READ, &hSubKey) == ERROR_SUCCESS) {
			// delete all its subkeys
			DeleteSubKeys(hSubKey);
			RegCloseKey(hSubKey);
		}
		else {
			TRACE("%s: unable to open key '%s'.", THIS_FUNCTION, (LPCSTR)(*pKeyArray)[iCur]);
		}
		// delete the subkey
		if (RegDeleteKey(hKey, (*pKeyArray)[iCur]) != ERROR_SUCCESS) {
			TRACE("%s: unable to delete key '%s'.", THIS_FUNCTION, (LPCSTR)(*pKeyArray)[iCur]);
		}
	}
	delete pKeyArray;
}

// ReadCmdLineSwitches
//
// Description: This function reads the command line and stores switches
// (arguments preceded by '-' or '/') in a CMapStringToString object for later
// retrieval. Switch name case sensitivity is not preserved. Switch value
// case sensitivity is preserved.
//
// Return value: --none--
// Example: sniff.exe -c="-run=false post=true" -run -post:false -logdir=h:\logs
//   This would map in the following manner:
//   C      -> "-run=false post=true"
//   RUN    -> ""
//   POST   -> "false"
//   LOGDIR -> "h:\logs"

CSettings* CCAFEDriver::ReadCmdLineSwitches(CString strCmdLine)
{
    CString strSwitch, strValue;
    CString strCmdLineCopy = strCmdLine;
    CSettings* pstngCmdLine;

    // create a new settings object for the cmd line switches
    pstngCmdLine = new CSettings;

    // just keep getting the next switch until we run out
    while (GetNextSwitch(strCmdLineCopy, strSwitch, strValue)) {
        pstngCmdLine->SetTextValue(strSwitch, strValue);
    }

    #ifdef _DEBUG
        pstngCmdLine->DumpSettings("CmdLine switches");
    #endif // _DEBUG

    return pstngCmdLine;
}

// GetNextSwitch
//
// Description: This function gets the next switch/value pair in the
// given command line
//
// Return value: TRUE if a switch/value pair is found
BOOL CCAFEDriver::GetNextSwitch(CString &strCmdLine, CString &strSwitch, CString &strValue)
{
    int iSwitchBegin;
    int iSwitchLength;
    int iValueBegin;
    int iValueLength;
    int iNextSwitch;

    strSwitch = "";
    strValue = "";

    // find the beginning of the switch
    iSwitchBegin = strCmdLine.FindOneOf("-/");
    // if there is no switch, return FALSE
    if (iSwitchBegin == -1) {
        return FALSE;
    }
    // skip the leading - or / (assume no whitespace)
    iSwitchBegin += 1;

    // get the length of the switch by finding the end relative to the beginning
    iSwitchLength = (strCmdLine.Mid(iSwitchBegin)).FindOneOf(" \t:=");
    // if there is no end of the switch, assume the length of the string
    if (iSwitchLength == -1) {
        iSwitchLength = strCmdLine.GetLength() - iSwitchBegin;
    }

    // find the next switch (length from the beginning of the first switch)
    iNextSwitch = (strCmdLine.Mid(iSwitchBegin)).FindOneOf("-/");
    // if the length to the next switch is less than the length to the end
    // of the first switch, then the switch is improperly defined
    if (iNextSwitch != -1 && iSwitchLength > iNextSwitch) {
        // bad switch, give an error
        // error processing switch "strCmdLine.Mid(0, iNextSwitch)"
        strCmdLine = strCmdLine.Mid(iSwitchBegin + iNextSwitch);
        return TRUE;
    }

    // assign switch
    strSwitch = strCmdLine.Mid(iSwitchBegin, iSwitchLength);
    // remove the switch from the command line
    strCmdLine = strCmdLine.Mid(iSwitchBegin + iSwitchLength);

    // if the length of the remaining string < 2 then there's
    // not enough string left to be a : or = plus a value
    if (strCmdLine.GetLength() < 2) {
        // exit with switch defined
        return TRUE;
    }

    // the value now begins at position 0 (if one exists)
    iValueBegin = 0;

    // is there a value associated with this switch? (value begin == : or =)
    if (strCmdLine.GetAt(iValueBegin) == ':' || strCmdLine.GetAt(iValueBegin) == '=') {

        // skip the : or = (assume no white space)
        iValueBegin += 1;

        // is this a quoted value? (beginning of value == ")
        if (strCmdLine.GetAt(iValueBegin) == '\"') {
            // skip the beginning "
            iValueBegin += 1;
            // length of quoted value is to the next "
            iValueLength = (strCmdLine.Mid(iValueBegin)).Find('\"');
            // if there is no ending quote, assume the end of the string
            if (iValueLength == -1) {
                iValueLength = strCmdLine.GetLength() - iValueBegin;
            }
        }
        // not a quoted value
        else {
            // figure length of value to next <space> or <tab>
            iValueLength = (strCmdLine.Mid(iValueBegin)).FindOneOf(" \t");
            // if there is no ending space or tab, assume the end of the string
            if (iValueLength == -1) {
                iValueLength = strCmdLine.GetLength() - iValueBegin;
            }
        }

        // assign value
        strValue = strCmdLine.Mid(iValueBegin, iValueLength);
        // remove the value from the command line
        strCmdLine = strCmdLine.Mid(iValueBegin + iValueLength);
    }

    return TRUE;
}

void CCAFEDriver::OnOptionsSingleStep()
{
    m_pstngCAFEDrv->SetBooleanValue(settingDoSingleStep, !m_pstngCAFEDrv->GetBooleanValue(settingDoSingleStep));
}

void CCAFEDriver::OnOptionsBreakStep()
{
	CStepBreak dlg(0);
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		if((g_stopStep=dlg.m_iStepNumber)>=0)
			g_hkToDo=HOTKEY_SINGLE; //to force the stepping mode when the break is hit
	}
	else if (nResponse == IDCANCEL)
	{
	}
}


void CCAFEDriver::OnUpdateOptionsSingleStep(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_pstngCAFEDrv->GetBooleanValue(settingDoSingleStep));
}

void CCAFEDriver::OnOptionsToolset()
{
    CToolset* pToolset;

    // instantiate a toolset in order to set options
    pToolset = NewToolset(0);

    // set toolset options
    pToolset->ToolsetOptions();

    DeleteToolset(pToolset);
}

void CCAFEDriver::OnViewLog()
{
    CAppLogDlg dlgAppLog(&applog);

    dlgAppLog.DoModal();
}

void CCAFEDriver::OnOptionSave()
{
	//not implemented in this version (to much work). V6x has implemented
}

void CCAFEDriver::CallRunSuite(CSuiteDoc *pSuiteDoc)
{
  m_pExecutingSuite = pSuiteDoc;
  SetEvent(m_hSuiteReadyEvent);  // notify our thread that we're ready to go
}

unsigned long CCAFEDriver::SuiteExecutionLoop(void)
{
    DWORD dwWait;
    HANDLE ahEvents[] = { m_hSuiteReadyEvent, m_hTerminationEvent };

    for (;;)
    {
        dwWait = WaitForMultipleObjects(2, ahEvents, FALSE, INFINITE);

        if (dwWait == WAIT_OBJECT_0)
        {
            // show the single step window if there
			if (m_bHotKey) m_pthreadHotKey->GetHKWnd()->ShowWindow(SW_SHOWNOACTIVATE);
            // run the suite
            m_pExecutingSuite -> RunSuite();
            // hide the single step window if there
			if (m_bHotKey) m_pthreadHotKey->GetHKWnd()->ShowWindow(SW_HIDE);

            ResetEvent(m_hSuiteReadyEvent);
        }
        else
        {
            return 0;
        }
    }
}


void CALLBACK SlowSuiteTimeProc(UINT IDEvent, UINT uRsrv, DWORD pslow_info, DWORD dwRsrv1, DWORD dwRsrv2)
{
    SlowSuiteInfo* pInfo = (SlowSuiteInfo*)pslow_info;

	// suspend the thread
	if (SuspendThread(pInfo->m_hthread) !=0 ) { 
		APPLOG("Unable to suspend the thread to delay it (i.e. suspend count != 0). GetLastError = %d.", ::GetLastError());
	}

    Sleep(pInfo->m_delay);

    // The == 1 checks to make sure the thread was actually suspended and
    // was successfully restarted.
	
	if (ResumeThread(pInfo->m_hthread) != 1) {
		APPLOG("Unable to resume the delayed thread (i.e. suspend count != 1). GetLastError = %d.", ::GetLastError());
	}
}

CPlatform *CCAFEDriver::CreateCurrentPlatform(void) // NOTE: you're responsible for freeing the resulting ptr
{
  CString strOSShortName, strLanguage;

  strOSShortName = m_pstngCAFEDrv->GetTextValue(settingPlatform);
  strLanguage = m_pstngCAFEDrv->GetTextValue(settingLanguage);

  CPlatform::COS *pOS = GetOSFromShortName(strOSShortName);
  CPlatform::CLanguage *pLang = GetLangFromName(strLanguage);

  return new CPlatform(*pOS, *pLang);
}

/////////////////////////////////////////////////////////////////////////////
// CStepBreak dialog

CStepBreak::CStepBreak(int CurrentStep,CWnd* pParent /*=NULL*/)
	: CDialog(CStepBreak::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStepBreak)
	m_iStepNumber = CurrentStep;
	//}}AFX_DATA_INIT
}


void CStepBreak::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStepBreak)
	DDX_Text(pDX, IDC_STEP_NO, m_iStepNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStepBreak, CDialog)
	//{{AFX_MSG_MAP(CStepBreak)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarsDlg message handlers

BOOL CStepBreak::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  
}
