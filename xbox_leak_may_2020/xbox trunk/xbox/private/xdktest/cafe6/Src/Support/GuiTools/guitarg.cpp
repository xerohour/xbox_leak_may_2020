/////////////////////////////////////////////////////////////////////////////
// guitargt.cpp
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Implementation of the CTarget class

#include "stdafx.h"
#include "afxdllx.h"
#include "applog.h"
#include "testxcpt.h"
#include "target.h"
#include "mstwrap.h"  // hack hack

//#include "toolset.h"
#include "guitarg.h"

#define new DEBUG_NEW

HANDLE g_hTargetProc;

/////////////////////////////////////////////////////////////////////////////
// CGUITarget

CGUITarget::CGUITarget(const CString &strTitle)
: m_hWnd(NULL), m_strTitle(strTitle)
{
}

CGUITarget::~CGUITarget()
{
	// if the GUI app is still around, exit it
	if (IsLaunched()) {
		Exit();
	}
}

BOOL CGUITarget::Launch(void)
{
	CString strMsg;
	int i;

	// does the user want to launch a new GUI or attach to an existing one
	if (m_psettingsTarget->GetBooleanValue(settingNewApp)) {
		// call the base class to launch
		CTarget::Launch();
		
		// wait for input idle in the GUI
		::WaitForInputIdle(m_procinfoTarget.hProcess, 120000);

		// find the window that matches the process id/handle
		m_hWnd = FindTarget(m_psettingsTarget->GetTextValue(settingWndClass), m_procinfoTarget.dwProcessId);

		// m_hWnd should be filled in at some point here
		if (!::IsWindow(m_hWnd)) {
			// throw exception: unable to successfully launch the target

			strMsg.Format("Unable to successfully launch target (%s).", (LPCTSTR)GetFileName());

			TRACE(strMsg);
			throw CTestException(strMsg, CTestException::causeOperationFail);
		}
	}
	else {
		// do the specific GUI attach work to get the HWND, and, from that, get the process handle and id
		Attach();
		// call the base class to do basic attach stuff, like start the debug event handler
		CTarget::Attach();
	}

	// the HWND must be a valid window
	ASSERT(::IsWindow(m_hWnd));

	// Attach the event handler   EventHandler
//	pEventHandler = new CEventHandler;
//	pEventHandler->Connect();

	// wait until the window is visible (up to twenty seconds)
	for (i = 0; i < 200; i++) {
			MST.DoKeys("{ESC}") ;
			if (::IsWindowVisible(m_hWnd)) {
						return TRUE;
		}
		Sleep(100);
	}

	// window never became visible: this is a critical error
	strMsg.Format("Timeout waiting for target's window to be visible (%s).", (LPCTSTR)GetFileName());
	throw CTestException(strMsg, CTestException::causeOperationFail);
	return FALSE;
}

BOOL CGUITarget::Attach(void)
{
	// find a suitable GUI target
	m_hWnd = FindTarget(m_psettingsTarget->GetTextValue(settingWndClass));

	// the HWND should be valid
	if (!::IsWindow(m_hWnd)) {
		// throw exception: unable to successfully attach to the target
		CString strMsg;

		strMsg.Format("Unable to successfully attach to target (%s).", (LPCTSTR)GetFileName());

		TRACE(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// get the target's process handle and id
	m_procinfoTarget.dwThreadId = ::GetWindowThreadProcessId(m_hWnd, &m_procinfoTarget.dwProcessId);
	m_procinfoTarget.hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_procinfoTarget.dwProcessId);
	m_procinfoTarget.hThread = 0; //TODO: is there a way to get the handle of dwThreadId?

	return TRUE;
}


// REVIEW(CFlaat): can this function be generalized to handle profiling?
BOOL CGUITarget::CreateTargetProcess(LPCSTR szCmdLine, DWORD dwCreationFlags, LPCSTR szEnvBlock, LPCSTR szWorkDir)
{
  BOOL bResult;

	static STARTUPINFO si = { sizeof(STARTUPINFO),	// size of this structure
							NULL,					// lpReserved
							NULL,					// lpDesktop
							NULL,					// lpTitle
							0, 0,					// dwX, dwY
							0, 0,					// dwXSize, dwYSize
							0, 0,					// dwXCountChars, dwYCountChars
							0,						// dwFillAttribute
							STARTF_USESHOWWINDOW,	// dwFlags
							SW_SHOWMAXIMIZED,		// wShowWindow
							0,						// cbReserved
							NULL,					// lpReserved
							0,						// hStdInput 
							0,						// hStdOutput
							0};						// hStdError

	// spawn the target application
	bResult = CreateProcess(NULL,
								(LPTSTR)szCmdLine,						// command line
								NULL,											// default security attributes of process
								NULL,											// default security attributes of thread
								FALSE,											// don't inherit handles
								dwCreationFlags,										// normal priority, and maybe debug
								(void*)szEnvBlock,						// environment variables
								szWorkDir,										// working directory
								&si,											// startup info
								&m_procinfoTarget);					// process info
	g_hTargetProc = m_procinfoTarget.hProcess;
	return bResult;
}



BOOL CGUITarget::Exit(void)
{
	// do we want to close the target on exit?
	if (m_psettingsTarget->GetBooleanValue(settingClose)) 
	{
		// if we are persisting the target between subsuites, we only want to exit the target after the last subsuite.
		if(!gbPersistTarget || gbLastSubSuite)
		{
			if (::IsWindow(m_hWnd))
			{
				// Disconnect the event handler EventHandler
				//pEventHandler->Disconnect();
				//delete pEventHandler;

				// send the close msg
				::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
		}
	}

	// call the base class
	return CTarget::Exit();
}

BOOL CGUITarget::IsActive(void)
{
	return ::IsWindow(m_hWnd);
}

HWND CGUITarget::FindTarget(CString strClass, DWORD dwProcessId /*= 0*/)
{
	HWND hwnd;
	DWORD dwId;
	CString strActualTitle;
	CString strActualClass;

    ASSERT(!m_strTitle.IsEmpty());

	// find a suitable target by iterating through all the currently running applications
	for (hwnd = ::GetTopWindow(NULL); hwnd != NULL; hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT)) {

		// get the window title
		::GetWindowText(hwnd, strActualTitle.GetBuffer(255), 255);
		strActualTitle.ReleaseBuffer();
		// get the window class
		::GetClassName(hwnd, strActualClass.GetBuffer(255), 255);
		strActualClass.ReleaseBuffer();
		// do the title and class match what's expected for the target
		// the title and class strings for the target are substrings of
		// what might be found for the entire title or class of the app
		if (strActualTitle.Find(m_strTitle) != -1 && strActualClass.Find(strClass) != -1) {
			// does the process id match?
			::GetWindowThreadProcessId(hwnd, &dwId);
			if (dwProcessId == 0 || dwProcessId == dwId) {
				return hwnd;
			}
		}
	}

	// no target found
	return NULL;
}

BOOL CGUITarget::ApplySettings(CSettings* pSettings)
{
	// call the base class
	CTarget::ApplySettings(pSettings);

	// launch new app
	m_psettingsTarget->SetBooleanValue(settingNewApp, pSettings->GetBooleanValue(settingNewApp, m_psettingsTarget->GetBooleanValue(settingNewApp)));

	// close after test run
	m_psettingsTarget->SetBooleanValue(settingClose, pSettings->GetBooleanValue(settingClose, m_psettingsTarget->GetBooleanValue(settingClose)));

	return TRUE;
}

BOOL CGUITarget::InitializeSettings(void)
{
	// call the base class
	CTarget::InitializeSettings();

	//ASSERTIONs handling
	m_psettingsTarget->SetBooleanValue(settingIgnoreASSERT, TRUE);
	m_psettingsTarget->SetBooleanValue(settingBreakASSERT, FALSE);

	// launch new app
	m_psettingsTarget->SetBooleanValue(settingNewApp, TRUE);

	// close after test run
	m_psettingsTarget->SetBooleanValue(settingClose, TRUE);

	// window class
	m_psettingsTarget->SetTextValue(settingWndClass, "Afx:");

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("GUI DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("GUI DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}

	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}

