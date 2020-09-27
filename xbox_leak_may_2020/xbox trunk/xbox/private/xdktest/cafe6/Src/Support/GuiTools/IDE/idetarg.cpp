/////////////////////////////////////////////////////////////////////////////
// idetarg.cpp
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Implementation of the CTarget class

#include "stdafx.h"
#include "afxdllx.h"
#include "testxcpt.h"
#include "settings.h"
#include "idepage.h"
#include "registry.h"
#include "target.h"
#include "idetarg.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "..\testutil.h"
#include "shl\uwbframe.h"
#include "dbg\codebug.h"
#include "shl\uioptdlg.h"
#include "shl\wbutil.h"
#include "shl\uiconset.h"
#include "shl\uitipdlg.h"
#include "prj\coprject.h"
#include "applog.h"
#include "toolset.h"
#include "evnthdlr.h"
#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CIDE

// register the HeapUse message
UINT CIDE::m_msgHeapUse = RegisterWindowMessage("HeapUse");

CIDE::CIDE()
: m_dwLaunchTime(0),
  m_pConnectionInfo(NULL),
  CGUITarget(GetLocString(IDSS_WORKBENCH_TITLEBAR))
{
	// filename of the heap use log file
	const char * const pfnHeapUse = "heapuse.log";

	// get the current directory
	::GetCurrentDirectory(MAX_PATH, m_strHeapUseFilename.GetBuffer(MAX_PATH));
	m_strHeapUseFilename.ReleaseBuffer();
	m_strHeapUseFilename = m_strHeapUseFilename + '\\' + pfnHeapUse;

	APPLOG("Deleting the heap use log file (%s).", (LPCSTR)m_strHeapUseFilename);

	// delete the heap use file
	::DeleteFile(m_strHeapUseFilename);

	// if we're running on nt/j, we need to initialize imehook.dll.
	// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).
	if(m_bRunningOnNTJ = (GetSystem() & SYSTEM_NT) && (GetSystem() & SYSTEM_JAPAN))
		{
		// load imehook.dll into cafe's address space.
		if(m_hIMEHookDll = LoadLibrary("imehook.dll"))
			{
			// load the address of HookIDE().
			if(m_lpfnHookIDE = (void (FAR WINAPI *)(HWND))GetProcAddress(m_hIMEHookDll, "HookIDE"))
				{
				// load the address of UnhookIDE().
				if(!(m_lpfnUnhookIDE = (void (FAR WINAPI *)(HWND))GetProcAddress(m_hIMEHookDll, "UnhookIDE")))
					{
					APPLOG("WARNING: error %d getting proc address of UnhookIDE()."
						   "CAFE will not be able to access the IME\n", GetLastError());
					}
				}
			else
				{
				APPLOG("WARNING: error %d getting proc address of HookIDE()."
					   "CAFE will not be able to access the IME\n", GetLastError());
				}	
			}
		else
			APPLOG("WARNING: error %d loading imehook.dll. "
				   "CAFE will not be able to access the IME\n", GetLastError());
		}
}

CIDE::~CIDE()
{
	// destroy the connection info object
	delete m_pConnectionInfo;

	// if the IDE is still around, exit it
	if (IsLaunched() && m_psettingsTarget->GetBooleanValue(settingClose))
	{

		// if we are persisting the target between subsuites, we only want to exit the target after the last subsuite.
		if(!gbPersistTarget || gbLastSubSuite)
		{
			Exit();

			// if we were running on NT/J, we need to unload imehook.dll.
			// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).
			if(m_bRunningOnNTJ && m_hIMEHookDll)
				FreeLibrary(m_hIMEHookDll);
		}
	}
}

BOOL CIDE::Launch(void)
{
	DWORD dwStartTime;
	DWORD dwEndTime;

	BOOL bLaunch;

	// tell the registry module which key to use
	gstrKeyExt = GetKeyExt();
	
	// slow links (win9x specific problem) must be turned on in the devstudio key before the ide launches. 
	// currently it requires that we launch without -i too.
	if(CMDLINE->GetBooleanValue("slowlinks", FALSE))
	
	{
		HKEY hKey;

		// open the key we need to modify.
		if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\DevStudio\\6.0\\Build System",
						0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
		{
			// turn slow links on.
			DWORD dwSetting = 2;
			
			// write the value to the registry
			if(RegSetValueEx(hKey, "Slow Links for Win95", 0, REG_DWORD, 
							 (const unsigned char *) &dwSetting, sizeof(DWORD)) != ERROR_SUCCESS)
				APPLOG("WARNING: got error %d setting slow links reg key value.\n", GetLastError());

		// close the registry
		RegCloseKey(hKey);
		}
		
		else
			APPLOG("WARNING: got error %d opening build system reg key for slow links.\n", GetLastError());
	}
	
	else
	
	{
		/*
		emmang@xbox - don't delete VC keys for xbox testing

		// delete whatever alternate key is to be used
		DeleteKey();

		// do we restore the key?
		if (m_psettingsTarget->GetBooleanValue(settingIDEUseKey)) {
			// restore the key to the registry
			ReplaceKey(m_psettingsTarget->GetTextValue(settingIDEKey));
		}
		*/
	}

    WaitStepInstructions("Launching %s", (LPCSTR)(GetPath() + GetFileName()));

	// store the tick count before launching
	dwStartTime = ::GetTickCount();

	//prepare the titles of dialogs to be handled automatically by event thread
//set from here because GetLocString() cannot be reached from TARGET.DLL or GUI.DLL
	m_AssertionMsgText=GetLocString(IDSS_ASSERTION_FAILED);
	m_AssertionMsgTitle=GetLocString(IDSS_ASSERTION_FROM_DEBUGLIB);
//	currently, "Find Symbols" & "Find Source" dialogs are added to the list of automatically handled
	SetHandleMsgTitle(GetLocString(IDSS_FS_TITLE));
	SetHandleMsgTitle(GetLocString(IDSS_FIND_SYMBOLS_DLG_TITLE));
	SetHandleMsgs(TRUE);

	// call the base class to launch
	bLaunch = CGUITarget::Launch();

	// store the tick count after launching
	dwEndTime = ::GetTickCount();

	// compute the launch time
	// note that this includes the time it takes for the IDE's window to actually be visible
	m_dwLaunchTime = dwEndTime - dwStartTime;

	// was the launch successful?


	MST.DoKeys("{ESC}") ; // HACK HACK HACK REMOVE ..
	if (bLaunch) {
	
		// is the Info Title Not Found dialog staring us in the face?
		UIDialog nd(GetLocString(IDSS_INFOTITLE_NOT_FOUND));
		if (nd.WaitAttachActive(1000))
		{
			// Cancel out of it
			nd.Cancel();
		}
		Sleep(2000) ; // HACK HACK HACK REMOVE
		MST.DoKeys("{ESC}");

		// is the tip of the day dialog showing?
		UITipDialog dlgTip;
		if (dlgTip.WaitAttachActive(500)) {
			// close it
			dlgTip.Close();
		}

		WaitForInputIdle(g_hTargetProc, 5000);		// Make sure the IDE gets the focus
		
		// REVIEW(briancr): This is for backward compatibility. We really should change this.
		// (JimGries) This comment refers to the removal or hiding of the UIWBFrame class within
		//            the CIDE class or whatever.
		// set up UIWB so it's valid
		UIWB.Attach(m_hWnd);
		//REVIEW(chriskoz): above is absolutely correct. I should also consider hooking the events (like Active Accessibility, ASSERTIONs) earlier in the Launch cycle but this is not in CAFE
		UIWB.m_pEventHandler = new CEventHandler;
		UIWB.m_pEventHandler->Connect(m_psettingsTarget->GetBooleanValue(settingDebugBuild)); //connect the AplicationEvents & Debugger Events & pass the pointer to target which would implement the Debugger Events thread
		UIWB.m_pTarget = this; //to know the target
		// Set the keymapping to VC2.0 by default. 
		// Much of CAFE support layer was written during the v2 time frame, and we requires these key mappings.

		// we must attach cafe's and the ide's thread inputs in order for cafe to call WGetFocus on the ide.
		// we are asssuming that the ide is in the foreground.
		HWND hWndFore = GetForegroundWindow();
		DWORD dwThreadFore = GetWindowThreadProcessId(hWndFore, NULL);
		AttachThreadInput(GetCurrentThreadId(), dwThreadFore, TRUE);

		UIOptionsTabDlg CompDlg ;
		CompDlg.ShowTabsTab() ;	
		MST.WComboItemClk(GetLabel(VCPP32_IDC_LANGUAGES), 2) ;
		MST.WOptionSelect(GetLabel(VCPP32_IDC_SMART_AUTOINDENT)) ;
		CompDlg.ChangeCompatility(EMULATE_VC2) ;

		// if we are running on nt/j, we need to hook the ide to access its ime.
		// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).
		if(m_bRunningOnNTJ && m_lpfnHookIDE)
			// call HookIDE() in imehook.dll, passing the ide's main window handle.
			(*(m_lpfnHookIDE))(m_hWnd);
			 
		return TRUE;
	}

	return FALSE;
}

BOOL CIDE::Exit(void)
{
    WaitStepInstructions("Shutting down %s...", (LPCSTR)GetFileName());

	// if debugging, stop debugging
	//REVIEW(chriskoz): removed the COdebug::StopDebugging(). It should not be there. No idea why it got here?
	UINT state = UIWB.GetIDEState();
	if((state & ST_DEBUGRUN) || (state & ST_DEBUGBREAK))
		UIWB.StopDebugging(ASSUME_NORMAL_TERMINATION);
	// close all open windows
	UIWB.CloseAllWindows();		// closes any open source files

	// if we were running on nt/j, we need to unhook the ide.
	// see cafe\support\guitools\imehook.cpp (michma - 1/1/97).
	if(m_bRunningOnNTJ && m_lpfnUnhookIDE)
		// call UnhookIDE() in imehook.dll, passing the ide's main window handle.
		(*(m_lpfnUnhookIDE))(m_hWnd);

	UIWB.m_pEventHandler->Disconnect(); //to disconnect the pointer to Debugger in Target Class
	delete UIWB.m_pEventHandler;
	UIWB.m_pEventHandler=NULL;
	UIWB.m_pTarget = NULL; //REVIEW(chriskoz): should we wait until the process is shut down?

	// call the base class
	if(CGUITarget::Exit())
	
	{
		// check to see if the slow links switch was set (see Launch for explanation).
		if(CMDLINE->GetBooleanValue("slowlinks", FALSE))
		
		{
			HKEY hKey;

			// open the key we need to modify.
			if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\DevStudio\\6.0\\Build System",
							0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
			{
				// turn off slow links.
				DWORD dwSetting = 0;
				
				// write the value to the registry
				if(RegSetValueEx(hKey, "Slow Links for Win95", 0, REG_DWORD, 
								 (const unsigned char *) &dwSetting, sizeof(DWORD)) != ERROR_SUCCESS)
					APPLOG("WARNING: got error %d setting slow links reg key value.\n", GetLastError());

			// close the registry
			RegCloseKey(hKey);
			}
			
			else
				APPLOG("WARNING: got error %d opening build system reg key for slow links.\n", GetLastError());
		}

		return TRUE;
	}

	return FALSE;
}


BOOL CIDE::IsActive(void)
{
 //	Sleep(10000);
	if( UIWB.IsAlert() )
	{
		LOG->RecordCriticalError("XSAFETY: Unexpected Workbench Alert");
		TEST_EXCEPTION(causeTargetGone);     // REVIEW: Should there be an exception cause for this case
		return FALSE;
	}

	if( !UIWB.IsActive() )
	{
		HWND hwnd = MST.WGetActWnd(0);
		char acBuf[256];
		GetText(hwnd, acBuf, 255);
		LOG->RecordCriticalError("XSAFETY: %x (%s) active when expecting %x (%s)", hwnd, acBuf, (HWND)UIWB, "target app" /*WORKBENCH_EXENAME*/);
		TEST_EXCEPTION(causeTargetGone);
		return FALSE;
	}
	return TRUE;
}

BOOL CIDE::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
{
	// instantiate the settings object for this target
	strKey += "\\"+strIDETargetKey;
	m_psettingsTarget = new CSettings(strKey);

	// instantiate the property page for this target
	m_ppageTarget = new CIDEPage(m_psettingsTarget, this);

	// initialize settings
	BOOL bResult = InitializeSettings();

	if (bResult == FALSE)
		LOG->RecordFailure("CIDE::Initialize: Couldn't initialize the settings for the CIDE object");
	else  // Now initialize the filename and path member variables.
	{
		// Set the filename and path to this target based on the the Path environment variable.
		if (nBuildType == CToolset::Debug)	// If cafe is set to run debug version of target...
		{
			m_strFileName = m_psettingsTarget->GetTextValue(settingDebugFilename);
			m_psettingsTarget->SetBooleanValue(settingDebugBuild, TRUE);	// Required for IDE property page.
		}
		else								// Let's run retail...
		{
			m_strFileName = m_psettingsTarget->GetTextValue(settingFilename);
			m_psettingsTarget->SetBooleanValue(settingDebugBuild, FALSE);	// Required for IDE property page.
		}

		m_strFileName += ".exe";

		// Find the file on the path sent by CAFE.
		CString strPath;
		CString strEnvPath;
		m_mapEnv.Lookup("PATH", strEnvPath);

		LPTSTR lpszJunk;
		if (::SearchPath(strEnvPath, m_strFileName, NULL, 1023, strPath.GetBuffer(1023), &lpszJunk) == 0)
		{
			APPLOG("CTarget::GetFullCmdLine: unable to find the target (%s) on the path (%s)\n", (LPCTSTR)m_strFileName, (LPCTSTR)strEnvPath);
			bResult = FALSE;
		}
		else
		{
			strPath.ReleaseBuffer();

			// set the path and filename to target
			char acDrive[_MAX_DRIVE];
			char acDir[_MAX_DIR];
			char acFileName[_MAX_FNAME];
			char acExt[_MAX_EXT];
			_splitpath(strPath, acDrive, acDir, acFileName, acExt);

			m_strPath = CString(acDrive) + acDir;
		}
	}

	return bResult;
}

BOOL CIDE::ApplySettings(CSettings* pSettings)
{
	// call the base class
	CGUITarget::ApplySettings(pSettings);

	// apply command line overrides:
	// ASSERTIONs/Exceptions handling
	m_psettingsTarget->SetBooleanValue(settingIgnoreASSERT, pSettings->GetBooleanValue(settingIgnoreASSERT, m_psettingsTarget->GetBooleanValue(settingIgnoreASSERT)));
	m_psettingsTarget->SetBooleanValue(settingBreakASSERT, pSettings->GetBooleanValue(settingBreakASSERT, m_psettingsTarget->GetBooleanValue(settingBreakASSERT)));
	// persist IDE between subsuites
	m_psettingsTarget->SetBooleanValue(settingPersist, pSettings->GetBooleanValue(settingPersist, m_psettingsTarget->GetBooleanValue(settingPersist)));
	//command line
	m_psettingsTarget->SetTextValue(settingCmdLine, pSettings->GetTextValue(settingCmdLine, m_psettingsTarget->GetTextValue(settingCmdLine)));

	// key extension
	if (!pSettings->GetBooleanValue(settingIDEKeyExt, TRUE)) {
		DeleteKeyExt();
	}
	// use key
	m_psettingsTarget->SetBooleanValue(settingIDEUseKey, pSettings->GetBooleanValue(settingIDEKey, m_psettingsTarget->GetBooleanValue(settingIDEUseKey)));
	// key
	m_psettingsTarget->SetTextValue(settingIDEKey, pSettings->GetTextValue(settingIDEKey, m_psettingsTarget->GetTextValue(settingIDEKey)));

	return TRUE;
}


BOOL CIDE::InitializeSettings(void)
{
	CString strDrive;
	CString strDir;

	BOOL bResult = TRUE;

	// call the base class
	bResult &= CGUITarget::InitializeSettings();

	// initialize IDE default settings
	m_psettingsTarget->SetBooleanValue(settingPersist, FALSE);
	// filename
	//xbox - we dont have access to debug builds of msdev
	//xbox   m_psettingsTarget->SetTextValue(settingDebugFilename, "msdevd");
	m_psettingsTarget->SetTextValue(settingDebugFilename, "msdev");
	m_psettingsTarget->SetTextValue(settingFilename, "msdev");

	// window class
	m_psettingsTarget->SetTextValue(settingWndClass, "Afx:");
	// cmd line
	//xbox - dont use the _auto registry key
	//xbox   m_psettingsTarget->SetTextValue(settingCmdLine, "-y0 -i_auto -noidelog -nocmdlog");
	m_psettingsTarget->SetTextValue(settingCmdLine, "-y0 -noidelog -nocmdlog");
	// don't use a key by default
	m_psettingsTarget->SetBooleanValue(settingIDEUseKey, FALSE);
	// key
	m_psettingsTarget->SetTextValue(settingIDEKey, "msdev.key");

	// REVIEW(briancr): don't forget remote settings
	// actually, remote settings probably go in the derived class(es)
	// remote settings...

	// load settings from the registry
	bResult &= m_psettingsTarget->ReadRegistry();
	// write the settings back out to initialize registry
	bResult &= m_psettingsTarget->WriteRegistry();

	// REVIEW(briancr): this sucks. When we go to the new method of using member vars, this problem will be more easily dealt with
	// prepend path to start up location in front of key name (only if there isn't one already)
	_splitpath(m_psettingsTarget->GetTextValue(settingIDEKey), NULL, strDir.GetBuffer(_MAX_DIR), NULL, NULL);
	strDir.ReleaseBuffer();
	if (strDir.IsEmpty()) {
		_splitpath(AfxGetApp()->m_pszHelpFilePath, strDrive.GetBuffer(_MAX_DRIVE), strDir.GetBuffer(_MAX_DIR), NULL, NULL);
		strDrive.ReleaseBuffer();
		strDir.ReleaseBuffer();

		m_psettingsTarget->SetTextValue(settingIDEKey, strDrive + strDir + m_psettingsTarget->GetTextValue(settingIDEKey));
	}

	return bResult;
}

COConnection* CIDE::GetConnectionInfo(void)
{
	// the connection info object should be initialized by now
	ASSERT(m_pConnectionInfo);

	return m_pConnectionInfo;
}

BOOL CIDE::RecordHeapUse(void)
{
	const ccBuf = 256;
	char acBuf[ccBuf];
	CStdioFile fileHeapUse;
	UINT nOpenFlags;
	CFileStatus status;
	BOOL bResult = FALSE;

	// get the atom that contains the heap use information from the IDE
	ATOM atomHeapUse = (ATOM)::SendMessage(m_hWnd, m_msgHeapUse, 0, 0L);

	// get the string out of the atom
	if (::GlobalGetAtomName(atomHeapUse, acBuf, ccBuf) != 0) {
		// define open mode for the file
		nOpenFlags = CFile::modeWrite | CFile::modeNoInherit | CFile::typeText | CFile::shareDenyWrite;
		// if the file doesn't exist, create it (MFC doesn't do this for us)
		if (!CFile::GetStatus(m_strHeapUseFilename, status)) {
			nOpenFlags |= CFile::modeCreate;
		}

		// open the file
		if (fileHeapUse.Open(m_strHeapUseFilename, nOpenFlags)) {
			APPLOG("Recording IDE heap use: %s", acBuf);
			// move the file pointer to the end of the file
			fileHeapUse.SeekToEnd();
			// write out the heap use information
			fileHeapUse.WriteString(acBuf);
			// close the file
			fileHeapUse.Close();
			bResult = TRUE;
		}
		else {
			APPLOG("Unable to record IDE heap use: %s", acBuf);
		}
	}

	// delete the atom
	::GlobalDeleteAtom(atomHeapUse);

	return bResult;
}

CString CIDE::GetKeyExt(void)
{
	CString strKeyExt;
	
	CString strCmdLine = m_psettingsTarget->GetTextValue(settingCmdLine);
	strCmdLine.MakeLower();
	int nBeginExt = strCmdLine.Find("-i");
	if (nBeginExt < 0) {
		nBeginExt = strCmdLine.Find("/i");
	}
	// found the beginning
	if (nBeginExt >= 0) {
		// skip -i
		nBeginExt += 2;
		int nEndExt = (strCmdLine.Mid(nBeginExt)).Find(' ') + nBeginExt;
		if (nEndExt < nBeginExt) {
			nEndExt = strCmdLine.GetLength();
		}
		strKeyExt = strCmdLine.Mid(nBeginExt, nEndExt-nBeginExt);
	}
	return strKeyExt;
}

void CIDE::DeleteKeyExt(void)
{
	CString strCmdLine = m_psettingsTarget->GetTextValue(settingCmdLine);
	strCmdLine.MakeLower();
	int nBeginExt = strCmdLine.Find("-i");
	if (nBeginExt < 0) {
		nBeginExt = strCmdLine.Find("/i");
	}
	// found the beginning
	if (nBeginExt >= 0) {
		int nEndExt = (strCmdLine.Mid(nBeginExt)).Find(' ') + nBeginExt;
		if (nEndExt < nBeginExt) {
			nEndExt = strCmdLine.GetLength();
		}
		strCmdLine = strCmdLine.Left(nBeginExt) + strCmdLine.Mid(nEndExt);
	}
	m_psettingsTarget->SetTextValue(settingCmdLine, strCmdLine);
}


CX86IDE::CX86IDE()
: m_psettingsTCPIP(NULL),
  m_psettingsSerial(NULL)
{
}

CX86IDE::~CX86IDE()
{
	delete m_psettingsTCPIP;
	delete m_psettingsSerial;
}


BOOL CX86IDE::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
	
	{
	m_psettingsTCPIP = new CSettings(strKey + "\\" + strIDETargetKey + "\\tcp/ip");
	m_psettingsSerial = new CSettings(strKey + "\\" + strIDETargetKey + "\\serial");
	
	// base class will call initialize settings
	return CIDE::Initialize(nBuildType, strKey);
	}


BOOL CX86IDE::ApplySettings(CSettings* pSettings)
	
	{
	// apply command line overrides for current connection
	if(pSettings->IsDefined(settingConnection))
		m_psettingsTarget->SetIntValue(settingConnection, m_connec.ConnectionTextToInt(pSettings->GetTextValue(settingConnection)));
	if(pSettings->IsDefined(settingRemoteLocalPath))
		m_psettingsTarget->SetTextValue(settingRemoteLocalPath, pSettings->GetTextValue(settingRemoteLocalPath));
	if(pSettings->IsDefined(settingRemoteSharePath))
		m_psettingsTarget->SetTextValue(settingRemoteSharePath, pSettings->GetTextValue(settingRemoteSharePath));

	// call the base class
	CIDE::ApplySettings(pSettings);

	// apply command line overrides for tcp/ip connection
	if(pSettings->IsDefined(settingRemoteIPAddress))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, pSettings->GetTextValue(settingRemoteIPAddress));
	if(pSettings->IsDefined(settingRemoteIPPassword))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, pSettings->GetTextValue(settingRemoteIPPassword));

	// apply command line overrides for serial connection
	if(pSettings->IsDefined(settingConnector))
		m_psettingsSerial->SetIntValue(settingConnector, m_connec.ConnectorTextToInt(pSettings->GetTextValue(settingConnector)));
	if(pSettings->IsDefined(settingBaudRate))
		m_psettingsSerial->SetIntValue(settingBaudRate, m_connec.BaudRateTextToInt(pSettings->GetTextValue(settingBaudRate)));
	if(pSettings->IsDefined(settingFlowControl))
		m_psettingsSerial->SetIntValue(settingFlowControl, m_connec.FlowControlTextToInt(pSettings->GetTextValue(settingFlowControl)));
	
	CSettings *psettings[3] = {m_psettingsTarget};

	switch(m_psettingsTarget->GetIntValue(settingConnection))
		{
		case CONNECTION_TCP_IP:
			psettings[1] = m_psettingsTCPIP;
			break;
		case CONNECTION_SERIAL:
			psettings[2] = m_psettingsSerial;
		}

	delete m_pConnectionInfo;
	m_pConnectionInfo = new COConnection(PLATFORM_WIN32_X86, psettings);
	gpConnec = m_pConnectionInfo;

	return TRUE;
	}


BOOL CX86IDE::InitializeSettings(void)
	
	{
	BOOL bResult = TRUE;
	m_psettingsTarget->SetIntValue(settingConnection, CONNECTION_LOCAL);
	m_psettingsTarget->SetTextValue(settingRemoteLocalPath, "");
	m_psettingsTarget->SetTextValue(settingRemoteSharePath, "");

	// call the base class
	bResult &= CIDE::InitializeSettings();

	// initialize tcp/ip default settings
	m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, "");
	m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, "");

	// initialize serial default settings
	m_psettingsSerial->SetIntValue(settingConnector, CONNECTOR_COM1);
	m_psettingsSerial->SetIntValue(settingBaudRate, BAUD_RATE_57600);
	m_psettingsSerial->SetIntValue(settingFlowControl, FLOW_CONTROL_HARDWARE);

	// load settings from the registry
	bResult &= m_psettingsTCPIP->ReadRegistry();
	bResult &= m_psettingsSerial->ReadRegistry();

	// write the settings back out to initialize registry
	bResult &= m_psettingsTCPIP->WriteRegistry();
	bResult &= m_psettingsSerial->WriteRegistry();
	
	return bResult;
	}


BOOL CX86IDE::SetConnections(HWND hwndDlg)
	{
	CSettings *psettings[3] = {m_psettingsTarget, m_psettingsTCPIP, m_psettingsSerial};
	m_connec.Connections(hwndDlg, psettings, PLATFORM_WIN32_X86);
	return TRUE;
	}


CWin32sIDE::CWin32sIDE()
: m_psettingsSerial(NULL)
{
}

CWin32sIDE::~CWin32sIDE()
{
	delete m_psettingsSerial;
}


BOOL CWin32sIDE::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
	
	{
	m_psettingsSerial = new CSettings(strKey + "\\" + strIDETargetKey + "\\serial");
	
	// base class will call initialize settings
	return CIDE::Initialize(nBuildType, strKey);
	}


BOOL CWin32sIDE::ApplySettings(CSettings* pSettings)
	
	{
	// apply command line overrides for current connection
	if(pSettings->IsDefined(settingConnection))
		m_psettingsTarget->SetIntValue(settingConnection, m_connec.ConnectionTextToInt(pSettings->GetTextValue(settingConnection)));
	if(pSettings->IsDefined(settingRemoteLocalPath))
		m_psettingsTarget->SetTextValue(settingRemoteLocalPath, pSettings->GetTextValue(settingRemoteLocalPath));
	if(pSettings->IsDefined(settingRemoteSharePath))
		m_psettingsTarget->SetTextValue(settingRemoteSharePath, pSettings->GetTextValue(settingRemoteSharePath));

	// call the base class
	CIDE::ApplySettings(pSettings);

	// apply command line overrides for serial connection
	if(pSettings->IsDefined(settingConnector))
		m_psettingsSerial->SetIntValue(settingConnector, m_connec.ConnectorTextToInt(pSettings->GetTextValue(settingConnector)));
	if(pSettings->IsDefined(settingBaudRate))
		m_psettingsSerial->SetIntValue(settingBaudRate, m_connec.BaudRateTextToInt(pSettings->GetTextValue(settingBaudRate)));
	if(pSettings->IsDefined(settingFlowControl))
		m_psettingsSerial->SetIntValue(settingFlowControl, m_connec.FlowControlTextToInt(pSettings->GetTextValue(settingFlowControl)));
	
	// win32s doesn't support tcp/ip, so psettings[1] is NULL, but we never use it.
	CSettings *psettings[3] = {m_psettingsTarget};
	psettings[2] = m_psettingsSerial;
	delete m_pConnectionInfo;
	m_pConnectionInfo = new COConnection(PLATFORM_WIN32S_X86, psettings);
	gpConnec = m_pConnectionInfo;
	return TRUE;
	}


BOOL CWin32sIDE::InitializeSettings(void)
	
	{
	BOOL bResult = TRUE;
	m_psettingsTarget->SetIntValue(settingConnection, CONNECTION_SERIAL);
	m_psettingsTarget->SetTextValue(settingRemoteLocalPath, "");
	m_psettingsTarget->SetTextValue(settingRemoteSharePath, "");

	// call the base class
	bResult &= CIDE::InitializeSettings();

	// initialize serial default settings
	m_psettingsSerial->SetIntValue(settingConnector, CONNECTOR_COM1);
	m_psettingsSerial->SetIntValue(settingBaudRate, BAUD_RATE_57600);
	m_psettingsSerial->SetIntValue(settingFlowControl, FLOW_CONTROL_HARDWARE);

	// load settings from the registry
	bResult &= m_psettingsSerial->ReadRegistry();

	// write the settings back out to initialize registry
	bResult &= m_psettingsSerial->WriteRegistry();
	
	return bResult;
	}


BOOL CWin32sIDE::SetConnections(HWND hwndDlg)
	{
	// win32s doesn't support tcp/ip, so psettings[1] is NULL, but we never use it.
	CSettings *psettings[3] = {m_psettingsTarget, NULL, m_psettingsSerial};
	m_connec.Connections(hwndDlg, psettings, PLATFORM_WIN32S_X86);
	return TRUE;
	}


C68KIDE::C68KIDE()
: m_psettingsTCPIP(NULL),
  m_psettingsSerial(NULL),
  m_psettingsAT(NULL)
{
}

C68KIDE::~C68KIDE()
{
	delete m_psettingsTCPIP;
	delete m_psettingsSerial;
	delete m_psettingsAT;
}


BOOL C68KIDE::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
	
	{
	m_psettingsTCPIP = new CSettings(strKey + "\\" + strIDETargetKey + "\\tcp/ip");
	m_psettingsSerial = new CSettings(strKey + "\\" + strIDETargetKey + "\\serial");
	m_psettingsAT = new CSettings(strKey + "\\" + strIDETargetKey + "\\appletalk");
	
	// base class will call initialize settings
	return CIDE::Initialize(nBuildType, strKey);
	}


BOOL C68KIDE::ApplySettings(CSettings* pSettings)
	
	{
	// apply command line overrides for current connection
	if(pSettings->IsDefined(settingConnection))
		m_psettingsTarget->SetIntValue(settingConnection, m_connec.ConnectionTextToInt(pSettings->GetTextValue(settingConnection)));
	if(pSettings->IsDefined(settingRemoteLocalPath))
		m_psettingsTarget->SetTextValue(settingRemoteLocalPath, pSettings->GetTextValue(settingRemoteLocalPath));

	// call the base class
	CIDE::ApplySettings(pSettings);

	// apply command line overrides for tcp/ip connection
	if(pSettings->IsDefined(settingRemoteIPAddress))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, pSettings->GetTextValue(settingRemoteIPAddress));
	if(pSettings->IsDefined(settingRemoteIPPassword))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, pSettings->GetTextValue(settingRemoteIPPassword));

	// apply command line overrides for serial connection
	if(pSettings->IsDefined(settingConnector))
		m_psettingsSerial->SetIntValue(settingConnector, m_connec.ConnectorTextToInt(pSettings->GetTextValue(settingConnector)));
	if(pSettings->IsDefined(settingBaudRate))
		m_psettingsSerial->SetIntValue(settingBaudRate, m_connec.BaudRateTextToInt(pSettings->GetTextValue(settingBaudRate)));
	if(pSettings->IsDefined(settingDataBits))
		m_psettingsSerial->SetIntValue(settingDataBits, m_connec.DataBitsTextToInt(pSettings->GetTextValue(settingDataBits)));
	if(pSettings->IsDefined(settingParity))
		m_psettingsSerial->SetIntValue(settingParity, m_connec.ParityTextToInt(pSettings->GetTextValue(settingParity)));
	if(pSettings->IsDefined(settingStopBits))
		m_psettingsSerial->SetIntValue(settingStopBits, m_connec.StopBitsTextToInt(pSettings->GetTextValue(settingStopBits)));
	
	// apply command line overrides for appletalk connection
	if(pSettings->IsDefined(settingRemoteMachName))
		m_psettingsAT->SetTextValue(settingRemoteMachName, pSettings->GetTextValue(settingRemoteMachName));
	if(pSettings->IsDefined(settingRemoteZone))
		m_psettingsAT->SetTextValue(settingRemoteZone, pSettings->GetTextValue(settingRemoteZone));
	if(pSettings->IsDefined(settingRemoteATPassword))
		m_psettingsAT->SetTextValue(settingRemoteATPassword, pSettings->GetTextValue(settingRemoteATPassword));

	CSettings *psettings[4] = {m_psettingsTarget};

	switch(m_psettingsTarget->GetIntValue(settingConnection))
		{
		case CONNECTION_TCP_IP:
			psettings[1] = m_psettingsTCPIP;
			break;
		case CONNECTION_SERIAL:
			psettings[2] = m_psettingsSerial;
			break;
		case CONNECTION_APPLETALK:
			psettings[3] = m_psettingsAT;
		}

	delete m_pConnectionInfo;
	m_pConnectionInfo = new COConnection(PLATFORM_MAC_68K, psettings);
	gpConnec = m_pConnectionInfo;

	return TRUE;
	}


BOOL C68KIDE::InitializeSettings(void)

	{
	BOOL bResult = TRUE;
	m_psettingsTarget->SetIntValue(settingConnection, CONNECTION_TCP_IP);
	m_psettingsTarget->SetTextValue(settingRemoteLocalPath, "");

	// call the base class
	bResult &= CIDE::InitializeSettings();

	// cmd line
	m_psettingsTarget->SetTextValue(settingCmdLine, "-y0 -noidelog -nocmdlog");

	// initialize tcp/ip default settings
	m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, "");
	m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, "");

	// initialize serial default settings
	m_psettingsSerial->SetIntValue(settingConnector, CONNECTOR_COM1);
	m_psettingsSerial->SetIntValue(settingBaudRate, BAUD_RATE_57600);
	m_psettingsSerial->SetIntValue(settingDataBits, DATA_BITS_8);
	m_psettingsSerial->SetIntValue(settingParity, PARITY_NONE);
	m_psettingsSerial->SetIntValue(settingStopBits, STOP_BITS_1);

	// initialize appletalk default settings
	m_psettingsAT->SetTextValue(settingRemoteMachName, "");
	m_psettingsAT->SetTextValue(settingRemoteZone, "");
	m_psettingsAT->SetTextValue(settingRemoteATPassword, "");

	// load settings from the registry
	bResult &= m_psettingsTCPIP->ReadRegistry();
	bResult &= m_psettingsSerial->ReadRegistry();
	bResult &= m_psettingsAT->ReadRegistry();

	// write the settings back out to initialize registry
	bResult &= m_psettingsTCPIP->WriteRegistry();
	bResult &= m_psettingsSerial->WriteRegistry();
	bResult &= m_psettingsAT->WriteRegistry();

	return bResult;
	}


BOOL C68KIDE::SetConnections(HWND hwndDlg)
	{
	CSettings *psettings[4] = {m_psettingsTarget, m_psettingsTCPIP, m_psettingsSerial, m_psettingsAT};
	m_connec.Connections(hwndDlg, psettings, PLATFORM_MAC_68K);
	return TRUE;
	}


CPMacIDE::CPMacIDE()
: m_psettingsTCPIP(NULL),
  m_psettingsSerial(NULL),
  m_psettingsAT(NULL)
{
}

CPMacIDE::~CPMacIDE()
{
	delete m_psettingsTCPIP;
	delete m_psettingsSerial;
	delete m_psettingsAT;
}


BOOL CPMacIDE::Initialize(CToolset::BUILD_TYPE nBuildType, CString strKey)
	
	{
	m_psettingsTCPIP = new CSettings(strKey + "\\" + strIDETargetKey + "\\tcp/ip");
	m_psettingsSerial = new CSettings(strKey + "\\" + strIDETargetKey + "\\serial");
	m_psettingsAT = new CSettings(strKey + "\\" + strIDETargetKey + "\\appletalk");

	// base class will call initialize settings
	return CIDE::Initialize(nBuildType, strKey);
	}


BOOL CPMacIDE::ApplySettings(CSettings* pSettings)
	
	{
	// apply command line overrides for current connection
	if(pSettings->IsDefined(settingConnection))
		m_psettingsTarget->SetIntValue(settingConnection, m_connec.ConnectionTextToInt(pSettings->GetTextValue(settingConnection)));
	if(pSettings->IsDefined(settingRemoteLocalPath))
		m_psettingsTarget->SetTextValue(settingRemoteLocalPath, pSettings->GetTextValue(settingRemoteLocalPath));

	// call the base class
	CIDE::ApplySettings(pSettings);

	// apply command line overrides for tcp/ip connection
	if(pSettings->IsDefined(settingRemoteIPAddress))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, pSettings->GetTextValue(settingRemoteIPAddress));
	if(pSettings->IsDefined(settingRemoteIPPassword))
		m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, pSettings->GetTextValue(settingRemoteIPPassword));

	// apply command line overrides for serial connection
	if(pSettings->IsDefined(settingConnector))
		m_psettingsSerial->SetIntValue(settingConnector, m_connec.ConnectorTextToInt(pSettings->GetTextValue(settingConnector)));
	
	// apply command line overrides for appletalk connection
	if(pSettings->IsDefined(settingRemoteMachName))
		m_psettingsAT->SetTextValue(settingRemoteMachName, pSettings->GetTextValue(settingRemoteMachName));
	if(pSettings->IsDefined(settingRemoteZone))
		m_psettingsAT->SetTextValue(settingRemoteZone, pSettings->GetTextValue(settingRemoteZone));
	if(pSettings->IsDefined(settingRemoteATPassword))
		m_psettingsAT->SetTextValue(settingRemoteATPassword, pSettings->GetTextValue(settingRemoteATPassword));

	CSettings *psettings[4] = {m_psettingsTarget};

	switch(m_psettingsTarget->GetIntValue(settingConnection))
		{
		case CONNECTION_TCP_IP:
			psettings[1] = m_psettingsTCPIP;
			break;
		case CONNECTION_SERIAL:
			psettings[2] = m_psettingsSerial;
			break;
		case CONNECTION_APPLETALK:
			psettings[3] = m_psettingsAT;
		}

	delete m_pConnectionInfo;
	m_pConnectionInfo = new COConnection(PLATFORM_MAC_PPC, psettings);
	gpConnec = m_pConnectionInfo;

	return TRUE;
	}


BOOL CPMacIDE::InitializeSettings(void)

	{
	BOOL bResult = TRUE;
	m_psettingsTarget->SetIntValue(settingConnection, CONNECTION_TCP_IP);
	m_psettingsTarget->SetTextValue(settingRemoteLocalPath, "");

	// call the base class
	bResult &= CIDE::InitializeSettings();

	// cmd line
	m_psettingsTarget->SetTextValue(settingCmdLine, "-y0 -noidelog -nocmdlog");

	// initialize tcp/ip default settings
	m_psettingsTCPIP->SetTextValue(settingRemoteIPAddress, "");
	m_psettingsTCPIP->SetTextValue(settingRemoteIPPassword, "");

	// initialize serial default settings
	m_psettingsSerial->SetIntValue(settingConnector, CONNECTOR_COM1);

	// initialize appletalk default settings
	m_psettingsAT->SetTextValue(settingRemoteMachName, "");
	m_psettingsAT->SetTextValue(settingRemoteZone, "");
	m_psettingsAT->SetTextValue(settingRemoteATPassword, "");

	// load settings from the registry
	bResult &= m_psettingsTCPIP->ReadRegistry();
	bResult &= m_psettingsSerial->ReadRegistry();
	bResult &= m_psettingsAT->ReadRegistry();

	// write the settings back out to initialize registry
	bResult &= m_psettingsTCPIP->WriteRegistry();
	bResult &= m_psettingsSerial->WriteRegistry();
	bResult &= m_psettingsAT->WriteRegistry();

	return bResult;
	}


BOOL CPMacIDE::SetConnections(HWND hwndDlg)
	{
	CSettings *psettings[4] = {m_psettingsTarget, m_psettingsTCPIP, m_psettingsSerial, m_psettingsAT};
	m_connec.Connections(hwndDlg, psettings, PLATFORM_MAC_PPC);
	return TRUE;
	}


CMIPSIDE::CMIPSIDE()
{
}

CMIPSIDE::~CMIPSIDE()
{
}

CAlphaIDE::CAlphaIDE()
{
}

CAlphaIDE::~CAlphaIDE()
{
}

CPowerPCIDE::CPowerPCIDE()
{
}

CPowerPCIDE::~CPowerPCIDE()
{
}

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("IDE DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("IDE DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}

BOOL CIDE::SetEnvironmentVar(LPCSTR pszEnvVar, LPCSTR pszValue)
{
	CString strEnvVar = pszEnvVar;

	// upper case the variable name
	strEnvVar.MakeUpper();
	m_mapEnv.SetAt(strEnvVar, pszValue);
	return TRUE;
}

BOOL CIDE::GetEnvironmentVar(LPCSTR pszEnvVar, CString &strValue)
{
	return 	m_mapEnv.Lookup(pszEnvVar, strValue);
}
