/////////////////////////////////////////////////////////////////////////////
// target.cpp
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

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTarget

const LPCTSTR CTarget::eventTerminate = "Terminate ";
const LPCTSTR CTarget::eventTargetSuccess = "Success ";
const LPCTSTR CTarget::eventTargetFail = "Fail ";

int CTarget::nCount = 0;



CTarget::CTarget()
:	m_hthreadDebug(NULL),
	m_ppageTarget(NULL),
	m_psettingsTarget(NULL),
	m_bInitProcess(FALSE),
	m_nExpectedLeaks(0)
{

	// set up unique id used for creating unique for system objects
	m_nId = ::GetTickCount()+nCount++;

	m_procinfoTarget.hProcess = 0;
	m_procinfoTarget.hThread = 0;
	m_procinfoTarget.dwProcessId = 0;
	m_procinfoTarget.dwThreadId = 0;

	// each target has a default environment
	InitEnv();

//save the handle to the current thread in hTestthread for suspenssion
	if(::DuplicateHandle(::GetCurrentProcess(), GetCurrentThread(), ::GetCurrentProcess(), &m_hTestthread, 0, FALSE, DUPLICATE_SAME_ACCESS)==0)
		m_hTestthread=NULL;
	m_hEventThread=NULL;
	m_handlemsgs=FALSE;
	m_nWndtoHandle=0;
}

CTarget::~CTarget()
{
	const char* const THIS_FUNCTION = "CTarget::~CTarget";
	
	// if the thread was created...
	if (m_hTerminate && (m_hthreadDebug || m_hEventThread))
	{
		// terminate the threads
		::SetEvent(m_hTerminate);

		// wait for the threads to terminate
		if (m_hthreadDebug && WaitForSingleObject(m_hthreadDebug, 2000) != WAIT_OBJECT_0) {
			TRACE("%s: unable to terminate debug thread (%lx) by request; forcing termination\n", THIS_FUNCTION, m_hthreadDebug);
			TerminateThread(m_hthreadDebug, DWORD(-1));
		}
		if (m_hEventThread && WaitForSingleObject(m_hEventThread, 2000) != WAIT_OBJECT_0) {
			TRACE("%s: unable to terminate event thread (%lx) by request; forcing termination\n", THIS_FUNCTION, m_hEventThread);
			TerminateThread(m_hEventThread, DWORD(-1));
		}
		::CloseHandle(m_hTerminate);
	}
	//close the handles, ::CreateProcess() bumped up their count to 2
	//we couldn't close these handles until now after debug thread has terminated
	if(m_procinfoTarget.hProcess)
		::CloseHandle(m_procinfoTarget.hProcess);
	if(m_procinfoTarget.hThread)
		::CloseHandle(m_procinfoTarget.hThread);

	// delete the property page associated with this target
	if (m_ppageTarget) {
		delete m_ppageTarget;
	}

	// delete the settings object associated with this target
	if (m_psettingsTarget) {
		delete m_psettingsTarget;
	}
	if(m_hTestthread!=NULL)
		::CloseHandle(m_hTestthread);
	if(m_hEventThread!=NULL)
		::CloseHandle(m_hEventThread);
}

BOOL CTarget::Launch(void)
{
	const char* const THIS_FUNCTION = "CTarget::Launch";
	HANDLE hTargetSuccess;
	HANDLE hTargetFail;
	HANDLE hthreadDebug;
	DWORD dwWaitResult;
	CString strMsg;

	APPLOG("%s: launching target '%s'...", THIS_FUNCTION, (LPCSTR)GetFileName());

	m_EnableExceptionMessageHandling = TRUE;

	// create an event for signalling the worker threads
	m_hTerminate = ::CreateEvent(NULL, TRUE, FALSE, eventTerminate + GetUniqueName());

	// create events for successful/failed launching
	hTargetSuccess = ::CreateEvent(NULL, FALSE, FALSE, eventTargetSuccess + GetUniqueName());
	hTargetFail = ::CreateEvent(NULL, FALSE, FALSE, eventTargetFail + GetUniqueName());

	// Event handles must be valid
	ASSERT(m_hTerminate && hTargetSuccess && hTargetFail);
	if (!m_hTerminate || !hTargetSuccess || !hTargetFail) {
		strMsg.Format("%s: unable to create events to launch target '%s'.", THIS_FUNCTION, (LPCSTR)GetFileName());
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// create a worker thread to launch the target
	DWORD dwThreadId;
	m_hthreadDebug = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CTarget::WrapLaunchTarget, this, 0, &dwThreadId);
	if (!m_hthreadDebug) {
		strMsg.Format("%s: unable to create a thread to launch the target '%s'. GetLastError = %d.", THIS_FUNCTION, (LPCSTR)GetFileName(), GetLastError());
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// create a duplicate handle to the debug thread
	// if the thread terminates, all handles to it go away and it's invalid
	::DuplicateHandle(::GetCurrentProcess(), m_hthreadDebug, ::GetCurrentProcess(), &hthreadDebug, 0, FALSE, DUPLICATE_SAME_ACCESS);

	// wait for either success or failure for 30 seconds.  That should be long enough.
	HANDLE hObjects[] = { hTargetSuccess, hTargetFail, m_hthreadDebug };
	dwWaitResult = ::WaitForMultipleObjects(sizeof(hObjects)/sizeof(HANDLE), hObjects, FALSE, 30000);

	// if launching was not successful
	if (dwWaitResult != WAIT_OBJECT_0) {
		if (dwWaitResult == WAIT_TIMEOUT) {
			strMsg.Format("timeout waiting for the target '%s' to launch.", (LPCSTR)GetFileName());
		}
		else {
			// get the result from the debug thread
			strMsg = GetThreadResult(hthreadDebug);
		}
		strMsg = CString(THIS_FUNCTION) + strMsg;
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	DWORD dwEventThreadId;
	m_hEventThread=::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CTarget::WrapEventHandler, this, 0, &dwEventThreadId);
	
	// close handle to thread
	::CloseHandle(hthreadDebug);

	// close handles to events
	::CloseHandle(hTargetSuccess);
	::CloseHandle(hTargetFail);

	return (dwWaitResult == WAIT_OBJECT_0);
}

BOOL CTarget::Attach(void)
{
	const char* const THIS_FUNCTION = "CTarget::Attach";
	HANDLE hTargetSuccess;
	HANDLE hTargetFail;
	HANDLE hthreadDebug;
	DWORD dwWaitResult;
	CString strMsg;

	APPLOG("%s: attaching to target '%s'...", THIS_FUNCTION, GetFileName());

	// the process handle must be valid
	ASSERT(m_procinfoTarget.hProcess);
	if (!m_procinfoTarget.hProcess) {
		strMsg.Format("%s: process handle (%lx) is invalid; unable to attach to target '%s'.", THIS_FUNCTION, m_procinfoTarget.hProcess, (LPCSTR)GetFileName());
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// create an event for signalling the worker thread
	m_hTerminate = ::CreateEvent(NULL, TRUE, FALSE, eventTerminate + GetUniqueName());

	// create events for successful/failed launching
	hTargetSuccess = ::CreateEvent(NULL, FALSE, FALSE, eventTargetSuccess + GetUniqueName());
	hTargetFail = ::CreateEvent(NULL, FALSE, FALSE, eventTargetFail + GetUniqueName());

	// the event handle must be valid
	ASSERT(m_hTerminate && hTargetSuccess && hTargetFail);
	if (!m_hTerminate || !hTargetSuccess || !hTargetFail) {
		strMsg.Format("%s: unable to create events to attach to target '%s'.", THIS_FUNCTION, (LPCSTR)GetFileName());
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// create a worker thread to attach to the target
	DWORD dwThreadId;
	m_hthreadDebug = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CTarget::WrapAttachTarget, this, 0, &dwThreadId);
	if (!m_hthreadDebug) {
		strMsg.Format("%s: unable to create a thread to attach to the target '%s'. GetLastError = %d.", THIS_FUNCTION, (LPCSTR)GetFileName(), GetLastError());
		APPLOG(strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}

	// create a duplicate handle to the debug thread
	// if the thread terminates, all handles to it go away and it's invalid
	::DuplicateHandle(::GetCurrentProcess(), m_hthreadDebug, ::GetCurrentProcess(), &hthreadDebug, 0, FALSE, DUPLICATE_SAME_ACCESS);

	// wait for either success or failure
	HANDLE hObjects[] = { hTargetSuccess, hTargetFail, m_hthreadDebug };
	dwWaitResult = ::WaitForMultipleObjects(sizeof(hObjects)/sizeof(HANDLE), hObjects, FALSE, INFINITE);

	// did the target launch successfully?
	if (dwWaitResult != WAIT_OBJECT_0) {
		// get the result from the debug thread
		strMsg = GetThreadResult(hthreadDebug);
		TRACE("CTarget::Attach: %s\n", (LPCTSTR)strMsg);
		throw CTestException(strMsg, CTestException::causeOperationFail);
	}
	DWORD dwEventThreadId;
	m_hEventThread=::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CTarget::WrapEventHandler, this, 0, &dwEventThreadId);

	// close handle to thread
	::CloseHandle(hthreadDebug);

	// close handles to events
	::CloseHandle(hTargetSuccess);
	::CloseHandle(hTargetFail);

	return (dwWaitResult == WAIT_OBJECT_0);
}

BOOL CTarget::Exit(void)
{
	const char* const THIS_FUNCTION = "CTarget::Exit";
	// wait for the target to exit (or 5 minutes)
	if(m_psettingsTarget->GetBooleanValue(settingClose))
	{ //REVIEW(chriskoz) we rely on the settingClose be correct 
	//we do not terminate event handler (settingClose never be false when the target really shuts down) 
		if(::WaitForSingleObject(m_procinfoTarget.hProcess, 5*60000) == WAIT_TIMEOUT)
		{
			// if we are persisting the target between subsuites, we only want to exit the target after the last subsuite.
			if(!gbPersistTarget || gbLastSubSuite)
			{
				char acBuf[255];
				sprintf(acBuf, "Time out waiting for target (%s) to shut down. Process id:%x still in memory.", (LPCSTR)GetFileName(), m_procinfoTarget.dwProcessId);
				m_pLog->RecordFailure(acBuf);
				APPLOG(acBuf);
				// clear the process information
				m_procinfoTarget.hProcess = 0;
				m_procinfoTarget.hThread = 0;
				m_procinfoTarget.dwProcessId = 0;
				m_procinfoTarget.dwThreadId = 0;
			}
		}
		//Terminate the Event handler thread explicitly (don't rely on it to shut down itself
		//I think we don't need to terminate the Debug thread as it shuts down itself when the Target thread is gone
		if (m_hTerminate && m_hEventThread)
		{
			::SetEvent(m_hTerminate);
			if(WaitForSingleObject(m_hEventThread, 2000) != WAIT_OBJECT_0) {
				TRACE("%s: unable to terminate event thread (%lx) by request; forcing termination\n", THIS_FUNCTION, m_hEventThread);
				TerminateThread(m_hEventThread, DWORD(-1));
			}
			::CloseHandle(m_hEventThread);
			m_hEventThread=NULL;
			ResetEvent(m_hTerminate); //reuse the event if target is launched once again
		}
	}
	return TRUE;
}

BOOL CTarget::ApplySettings(CSettings* pSettings)
{
	// launch as a debug process
	m_psettingsTarget->SetBooleanValue(settingDebugProcess, pSettings->GetBooleanValue(settingDebugProcess, m_psettingsTarget->GetBooleanValue(settingDebugProcess)));

	return TRUE;
}

BOOL CTarget::SetEnvironmentVar(LPCSTR pszEnvVar, LPCSTR pszValue)
{
	CString strEnvVar = pszEnvVar;

	// upper case the variable name
	strEnvVar.MakeUpper();
	m_mapEnv.SetAt(strEnvVar, pszValue);
	return TRUE;
}

CString CTarget::GetPath(void)
{
	return m_strPath;
}

CString CTarget::GetFileName(void)
{
	return m_strFileName;
}

BOOL CTarget::InitEnv(void)
{
	// default set of environment vars
	LPCSTR aszEnvVars[] = { "ComSpec", "Include", "Lib", "OS", "Path", "PROCESSOR_ARCHITECTURE", "PROCESSOR_LEVEL", "Source", "SystemRoot", "SystemDrive", "windir", "TMP", NULL };
	const ccBuf = 1024;
	char acValue[ccBuf];
	DWORD dwResult;

	// fill in the environment vars map
	for (int i = 0; aszEnvVars[i] != NULL; i++) {
		// get the env var's value
		dwResult = ::GetEnvironmentVariable(aszEnvVars[i], acValue, ccBuf);
		if (dwResult > 0 && dwResult <= ccBuf) {
			// set the env var's value in the map
			SetEnvironmentVar(aszEnvVars[i], acValue);
		}
		else if(strcmp(aszEnvVars[i],"PROCESSOR_ARCHITECTURE")==0)
			SetEnvironmentVar(aszEnvVars[i], "x86");
		else
		{
			
			if (dwResult == 0) {
				TRACE("Environment variable (%s) not defined.", aszEnvVars[i]);
			}
			else {
				TRACE("Error getting value for environment variable (%s): the buffer is too small.", aszEnvVars[i]);
			}
		}
	}

	// add the current directory for each drive (=X:=current dir)
	char acDrives[ccBuf];
	LPCSTR pcDrive = acDrives;
	char acDrive[] = "=X:";

	// get the list of drives
	dwResult = ::GetLogicalDriveStrings(ccBuf, acDrives);
	if (dwResult > 0 && dwResult <= ccBuf) {
		// iterate through each drive
		while (pcDrive[0]) {
			// copy the drive letter
			acDrive[1] = pcDrive[0];
			// get the value for this drive
			dwResult = ::GetEnvironmentVariable(acDrive, acValue, ccBuf);
			if (dwResult > 0 && dwResult <= ccBuf) {
				// set this drive's value in the map
				SetEnvironmentVar(acDrive, acValue);
			}
			else {
				if (dwResult == 0) {
					TRACE("Drive (%s) not defined.", acDrive);
				}
				else {
					TRACE("Error getting value for drive (%s): the buffer is too small.", acDrive);
				}
			}
			// move to the next drive
			pcDrive += strlen(pcDrive) + 1;
		}
	}
	else {
		if (dwResult == 0) {
			TRACE("Error getting the list of logical drives (%d).", ::GetLastError());
		}
		else {
			TRACE("Error getting the list of logical drives: the buffer's too small.");
		}
	}
	return TRUE;
}


CString CTarget::GetFullCmdLine(void)
{
	m_strLastCmdLine = GetPath() + GetFileName() + " " + m_psettingsTarget->GetTextValue(settingCmdLine);

	return m_strLastCmdLine;
}

CString CTarget::GetEnvBlock(void)
{
	CStringList listEnvVars;
	CString strVar;
	CString strValue;
	CString strEnvVar;
	CString strCur;
	POSITION posMap;
	POSITION posList;
	POSITION posCur;
	BOOL bInserted;


	// iterate through all env vars in the map and sort them
	for (posMap = m_mapEnv.GetStartPosition(); posMap != NULL; ) {
		m_mapEnv.GetNextAssoc(posMap, strVar, strValue);
		// build the environment var string (envvar=value)
		strEnvVar = strVar + "=" + strValue;
		// insert it into the list
		// find its sorted position
		bInserted = FALSE;
		for (posList = listEnvVars.GetHeadPosition(); posList != NULL; ) {
			posCur = posList;
			strCur = listEnvVars.GetNext(posList);
			// does this env var belong before the current env var?
			if (strEnvVar < strCur) {
				listEnvVars.InsertBefore(posCur, strEnvVar);
				bInserted = TRUE;
				break;
			}
		}
		// if the env var was inserted, add it to the end of the list
		if (!bInserted) {
			listEnvVars.AddTail(strEnvVar);
		}
	}

	APPLOG("Using the following environment for target (%s):", (LPCSTR)GetFileName());

	CString strEnvBlock;
	char* pEnvBlockStart;
	char* pEnvBlock;

	// get a pointer to a large buffer for the environment
	pEnvBlockStart = strEnvBlock.GetBuffer(10000);
	pEnvBlock = pEnvBlockStart;

	// iterate through the list of sorted environment variables
	for (posList = listEnvVars.GetHeadPosition(); posList != NULL; ) {
		strEnvVar = listEnvVars.GetNext(posList);
		// add the env var to the block
		pEnvBlock = strcpy(pEnvBlock, strEnvVar);
		pEnvBlock += strEnvVar.GetLength() + 1;
		APPLOG("    %s", strEnvVar);
	}

	// add final \0
	pEnvBlock = strncpy(pEnvBlock, "\0", 1);
	pEnvBlock += 1;

	strEnvBlock.ReleaseBuffer(pEnvBlock - pEnvBlockStart);

	return strEnvBlock;
}

UINT CTarget::WrapLaunchTarget(LPVOID pParam)  // static member
{

	TRACE1("WrapLaunchTarget: Starting thread 0x%X\n", GetCurrentThreadId());
	DWORD ret=((CTarget*)pParam)->LaunchTarget();
	TRACE1("WrapLaunchTarget: Exiting thread 0x%X\n",GetCurrentThreadId());
	return ret;
}


// BEGIN_HELP_COMMENT
// Function: HWND CTarget::LaunchTarget() 
// Description: The target thread procedure. Runs the Debug events loop if we attach to the target as debugger. Otherwise returns immediately after having launched the target.
// Return: dtr_success: if target launched successfully.
// END_HELP_COMMENT
UINT CTarget::LaunchTarget(void)
{
	HANDLE hTerminate;
	HANDLE hTargetSuccess;
	HANDLE hTargetFail;

	CString strCmdLine;
	CString strEnv;
	CString strWorkDir;

	DWORD dwCreate;

	BOOL bProcess;

	UINT nResult;

	// open the termination event
	hTerminate = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTerminate + GetUniqueName());

	// the handle must be valid
	ASSERT(hTerminate);
	if (!hTerminate) {
		TRACE("CTarget::LaunchTarget: unable to open the terminate event\n");
		return dtr_invalidevent;
	}

	// open the events for launch success/fail
	hTargetSuccess = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTargetSuccess + GetUniqueName());
	hTargetFail = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTargetFail + GetUniqueName());

	// the handles must valid
	ASSERT(hTargetSuccess && hTargetFail);
	if (!hTargetSuccess || !hTargetFail) {
		TRACE("CTarget::LaunchTarget: unable to open the launch event(s)\n");
		::CloseHandle(hTerminate);
		return dtr_invalidevent;
	}

	// launch the target

	// get the target's full command line (path\filename params)
	strCmdLine = GetFullCmdLine();
	// an empty command line means the target wasn't found
	if (strCmdLine.IsEmpty()) {
		return dtr_exenotfound;
	}

	// get the target's environment
	strEnv = GetEnvBlock();

	// get the target's working directory
	strWorkDir = GetSettings()->GetTextValue(settingWorkingDir);

	// determine creation flags for CreateProcess
	dwCreate = NORMAL_PRIORITY_CLASS | (GetSettings()->GetBooleanValue(settingDebugProcess) ? DEBUG_ONLY_THIS_PROCESS : 0);

  // create the process
  // bProcess = CreateTargetProcess(strCmdLine, dwCreate, strEnv, strWorkDir);
  bProcess = CreateTargetProcess(strCmdLine, dwCreate, NULL, strWorkDir);

	// based on the success of CreateProcess(), set an event to indicate the
	// target's starting or not starting
	if (bProcess) {
		// set event to indicate launch successful
		::SetEvent(hTargetSuccess);
		
		// handle the debug events if we're debugging
		if (GetSettings()->GetBooleanValue(settingDebugProcess)) {
			HandleDebugEvents();
		}

		nResult = dtr_success;
	}
	else {
		// set event to indicate launch not successful
		::SetEvent(hTargetFail);

		nResult = dtr_invalidexe;
	}

	// close handles
	::CloseHandle(hTargetFail);
	::CloseHandle(hTargetSuccess);
	::CloseHandle(hTerminate);

	return nResult;
}


UINT CTarget::WrapAttachTarget(LPVOID pParam)  // static member
{
	TRACE1("WrapAttachTarget: Starting thread 0x%X\n", GetCurrentThreadId());
	DWORD ret=((CTarget*)pParam)->AttachTarget();
	TRACE1("WrapAttachTarget: Exiting thread 0x%X\n",GetCurrentThreadId());
	return ret;
}


// BEGIN_HELP_COMMENT
// Function: HWND CTarget::AttachTarget() 
// Description: The target thread procedure. Runs the Debug events loop if we attach to the target as debugger. Otherwise returns immediately after having attached to the target.
// Return: dtr_success: if target Attached.
// END_HELP_COMMENT
UINT CTarget::AttachTarget(void)
{
	HANDLE hTerminate;
	HANDLE hTargetSuccess;
	HANDLE hTargetFail;

	UINT nResult;

	// open the termination event
	hTerminate = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTerminate + GetUniqueName());

	// the handle must be valid
	ASSERT(hTerminate);
	if (!hTerminate) {
		TRACE("CTarget::AttachTarget: unable to open the terminate event\n");
		return dtr_invalidevent;
	}

	// open the events for launch success/fail
	hTargetSuccess = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTargetSuccess + GetUniqueName());
	hTargetFail = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTargetFail + GetUniqueName());

	// the handles must valid
	ASSERT(hTargetSuccess && hTargetFail);
	if (!hTargetSuccess || !hTargetFail) {
		TRACE("CTarget::AttachTarget: unable to open the launch event(s)\n");
		::CloseHandle(hTerminate);
		return dtr_invalidevent;
	}


	// are we debugging the target application?
	if (GetSettings()->GetBooleanValue(settingDebugProcess)) {

		// attach to the target application as a debugger by calling DebugActiveProcess
		if (::DebugActiveProcess(m_procinfoTarget.dwProcessId)) {

			// set event to indicate attach successful
			::SetEvent(hTargetSuccess);
	
			// handle the debug events
			HandleDebugEvents();

			nResult = dtr_success;
		}
		else {
			// set event to indicate attach failed
			::SetEvent(hTargetFail);

			nResult = dtr_exenotfound;
		}
	}
	else {
		::SetEvent(hTargetSuccess);

		nResult = dtr_success;
	}

	// close handles
	::CloseHandle(hTargetFail);
	::CloseHandle(hTargetSuccess);
	::CloseHandle(hTerminate);

	return nResult;
}


UINT CTarget::WrapEventHandler(LPVOID pParam)  // static member
{
	TRACE1("WrapEventHandler: Starting thread 0x%X\n", GetCurrentThreadId());
	DWORD ret=((CTarget*)pParam)->EventHandler();
	TRACE1("WrapEventHandler: Exiting thread 0x%X\n",GetCurrentThreadId());
	return ret;
}




//REVIEW(chriskoz) these are ugly ESDed globals but it's just a proof of concept.
//After everything is smoothed out, they should endup as members of CTarget
#define MAX_CLASS 256
#define MAX_TITLE 256

void CTarget::SetHandleMsgTitle(CString &title)
{
	for(int i=0;i<m_nWndtoHandle;i++)
		if(m_HandleMsgTitles[i]==title)
			return;
	ASSERT(m_nWndtoHandle<5);//arbitrary number, maybe increased
	if(m_nWndtoHandle>=5)return;
	m_HandleMsgTitles[m_nWndtoHandle]=title;
	m_nWndtoHandle++; //I don't think the thread safety is an issue here
}



// BEGIN_HELP_COMMENT
// Function: HWND CTarget::FindAssertText() 
// Description: CALLBACK function: searches for the text inside the ASSERTION dialog
// Return: FALSE: found 
// END_HELP_COMMENT
BOOL CALLBACK CTarget::FindAssertText(HWND hAssert, CTarget *_this) //static function
{
	CHAR szClassName[MAX_CLASS];
	CHAR szTitleName[MAX_TITLE];
	memset(szTitleName, 0, sizeof(szTitleName)); //NULL the title
	::GetWindowText(hAssert, szTitleName, MAX_TITLE-1);
	::GetClassName(hAssert, szClassName, MAX_CLASS);
	if(strstr(szTitleName, _this->m_AssertionMsgText)==szTitleName)
	{
		_this->m_AssertionMsgText= //REVIEW(chriskoz) I think the thread sefety issue does not apply here
			szTitleName+_this->m_AssertionMsgText.GetLength(); //skip the known part of the text
		return FALSE; //found
	}
	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: HWND CTarget::FindExceptionText() 
// Description: CALLBACK function: searches for the text inside the EXCEPTION dialog
// Return: FALSE: found 
// END_HELP_COMMENT
BOOL CALLBACK CTarget::FindExceptionText(HWND hAssert, char buffer[MAX_TITLE]) //static function
{
	CHAR szClassName[MAX_CLASS];
	CHAR szTitleName[MAX_TITLE];
	memset(szTitleName, 0, sizeof(szTitleName)); //NULL the title
	::GetWindowText(hAssert, szTitleName, MAX_TITLE-1);
	::GetClassName(hAssert, szClassName, MAX_CLASS);
	if(strlen(szTitleName)>40 //what a shame! I think this strlength is accurate
	&& strcmp(szClassName,"Static")==0) //must be static text
	{
		strncpy(buffer, szTitleName, MAX_TITLE);
		return FALSE; //found
	}
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: HWND CTarget::FindMsgToHandle() 
// Description: CALLBACK function: searches enumerated windows in a search for ASSERTION dialog & other dialogs for be handled automatically
// Return: FALSE: ASSERTION found 
// END_HELP_COMMENT
BOOL CALLBACK CTarget::FindMsgToHandle(HWND hwnd, CTarget *_this) //static function
{
	CHAR szClassName[MAX_CLASS];
	CHAR szTitleName[MAX_TITLE];
	DWORD idThread,idProcess;
	BOOL isException=FALSE;
	idThread=::GetWindowThreadProcessId(hwnd, &idProcess);
	::GetClassName(hwnd, szClassName, MAX_CLASS);
	if(idProcess!=_this->m_procinfoTarget.dwProcessId)
	{//this window does not belong to our process
		if(!IsWindowVisible(hwnd))
			return TRUE; //saves lots of calls (and how about doing this on all windows?)
		if(strstr(szClassName,"#32770")==NULL)
			return TRUE;  //it's not an exception
		::GetWindowText(hwnd, szTitleName, MAX_TITLE);
		if(strstr(szTitleName, " - Application Error")!=NULL)//TODO make it into LOCALE
		{ //definitely found an exception window
	#if 0 //TODO: disabled this extra care right now, because the overall code appears to be too complicated & unstable. Requires more testing before it's enabled
			CString strTitle=szTitleName;
			strTitle.MakeLower();
			//TODO: so much confusion because of Debug/Non-debug differenciation here
			BOOL bIsDebug=_this->m_psettingsTarget->GetBooleanValue(settingDebugBuild);
			CString strmodule=_this->m_psettingsTarget->GetTextValue(bIsDebug?settingDebugFilename:settingFilename);
			strTitle.MakeLower();
			if(strTitle.Find(strmodule)!=-1)
	#endif
			{//found an EXCEPTION: stop enumeration
				_this->m_hAssert=hwnd;
				_this->m_AssertionMsgTitle = szTitleName; //TODO remove this hack when "- Application error" message ready
				return FALSE;
			}
		}
		return TRUE; //not a true EXCEPTION
	}
//we have the window belonging to our process
	::GetWindowText(hwnd, szTitleName, MAX_TITLE);
	if(strstr(szClassName,"#32770")!=NULL)
	{
		if(strcmp(szTitleName, _this->m_AssertionMsgTitle)==0)
		{ //found an ASSERTION: stop enumeration
			_this->m_hAssert=hwnd;
			return FALSE;
		}
	}
//automatically handle all dialogs to be handled by hitting ESC without stopping enumeration
if(_this->m_handlemsgs) //only in case when test thread does not want to handle them.
	for(int i=0; i<_this->m_nWndtoHandle; i++) //arbitrary number, maybe increased
	{
		if(strcmp(szTitleName, _this->m_HandleMsgTitles[i])==0) //REVIEW(chriskoz): do we need to allow partial match?
		{ //found Find Symbols dialog: dismiss it without any question
			SetForegroundWindow(hwnd);
			keybd_event(VK_ESCAPE, 0,0,0);
			keybd_event(VK_ESCAPE, 0,KEYEVENTF_KEYUP,0);
			_this->m_pLog->RecordInfo("Dialog '%s' detected. ESC-handled by default", szTitleName);
			break;
		}
	}
	return TRUE;	
}



// BEGIN_HELP_COMMENT
// Function: HWND CTarget::EventHandler() 
// Description: Events thread. Runs the loop which handles unexpected dialogs: ASSERTION & dialogs set by SetHandleMsgTitle(). Currently, it's implemented by polling all top-level windows every .5sec. When AAccessibility is avail, we can switch to it. 
// Return: 0 if thread exits clean (without TerminateThread() help from the driver).
// END_HELP_COMMENT
UINT CTarget::EventHandler(void)
{
	BOOL bClickIgnore = GetSettings()->GetBooleanValue(settingIgnoreASSERT);
	BOOL bBreak = GetSettings()->GetBooleanValue(settingBreakASSERT);

	// open the termination event
	HANDLE hTerminate = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTerminate + GetUniqueName());
	do{
		Sleep(500); //randomly selected value. Do we need to tune it?
		m_hAssert=NULL;
	
		//TODO: remove this hack when the storage for Exception is ready
		TCHAR szAssertTitle[MAX_TITLE];
		strncpy(szAssertTitle, m_AssertionMsgTitle, MAX_TITLE); //save the original text, to be changed by FindAssertText callback
		::EnumWindows((WNDENUMPROC)FindMsgToHandle, (LPARAM)this); //enum all windows as EnumChildWindows(m_UIMainFrame) does not return popup Dialogs
		if(m_hAssert)
		{
			if(strcmp(szAssertTitle, m_AssertionMsgTitle)==0)
				HandleAssertion(bClickIgnore, bBreak);
			else
			{//TODO: we pass an info about Exception in this whako way here
				HandleExceptionDlg(bClickIgnore, bBreak, m_AssertionMsgTitle);
				m_AssertionMsgTitle=szAssertTitle;//restore the original title
			}
		}
		if (::WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0)
		{ //main thread setup the terminate event
			CloseHandle(hTerminate);
			return 0;
		}
		if(m_procinfoTarget.hProcess &&
			::WaitForSingleObject(m_procinfoTarget.hProcess, 0) == WAIT_OBJECT_0)
		{ //the target process exited unexpectedly
			CloseHandle(hTerminate);
			return 0;
		}
	}while(1);
}


// BEGIN_HELP_COMMENT
// Function: HWND CTarget::HandleAssertion() 
// Description: Logs the ASSERTION data & handles the ASSETION dialog according to the user settings. Suspends the test thread while handling the ASSERTION
// Param: bClickIgnore - handle by clicking "Ignore"
// Param: bBreak - suspend the test run & display the modal message about assertion
// Return: void
// END_HELP_COMMENT
void CTarget::HandleAssertion(BOOL bClickIgnore, BOOL bBreak)
{
	DWORD dsuspendcount=0xFFFFFFFF;
	if(m_hTestthread)
	{
		dsuspendcount=::SuspendThread(m_hTestthread);
		TRACE1("Suspend test thread returned: %d\n", dsuspendcount);
	}
	TCHAR szAssertText[MAX_TITLE];
	strncpy(szAssertText, m_AssertionMsgText, MAX_TITLE); //save the original text, to be changed by FindAssertText callback
	::EnumChildWindows(m_hAssert, (WNDENUMPROC)FindAssertText, (LPARAM)this); //sets the text
	m_pLog->RecordFailure("ASSERTION dialog detected: %s",
		m_AssertionMsgText);
	if(bBreak)
	{
		char messageText[512];
		sprintf(messageText,"Test [***FAILED***]:\nProcess (%d) hit an unexpected assertion:\n", m_procinfoTarget.dwProcessId);
		strcat(messageText, m_AssertionMsgText);
		if(bClickIgnore)
			strcat(messageText, "\nDialog will be handled by clicking Ignore.\n");
		else
			strcat(messageText, "\nDialog will not be handled!!!\n");
		strcat(messageText, "Continue the test?");
		::MessageBox(NULL, messageText, AfxGetAppName(), MB_ICONWARNING|MB_TOPMOST|MB_YESNO);
	}
	m_AssertionMsgText=szAssertText; //REVIEW(chriskoz) I think the thread sefety issue does not apply here
	if(bClickIgnore && ::IsWindow(m_hAssert)) //make sure the ASSERT window is still there
	{
		SetForegroundWindow(m_hAssert);
		keybd_event(VK_MENU, 0,0,0);
		keybd_event(LOBYTE(VkKeyScan('i')), 0,0,0);
		keybd_event(LOBYTE(VkKeyScan('i')), 0,KEYEVENTF_KEYUP,0);
		keybd_event(VK_MENU, 0,KEYEVENTF_KEYUP,0);
		Sleep(500); //randomly selected value. Do we need to tune it?
	}
	if(m_hTestthread && dsuspendcount!=0xFFFFFFFF)
	{
		dsuspendcount=::ResumeThread(m_hTestthread);
		TRACE1("Resume test thread returned: %d\n", dsuspendcount);
	}
}

// BEGIN_HELP_COMMENT
// Function: HWND CTarget::HandleExceptionDlg() 
// Description: Logs the EXCEPTION data & handles the EXECPTION dialog according to the user settings. Suspends the test thread while handling the EXCEPTION
// Param: bIgnore - do not handle: just let it run
// Param: bBreak - suspend the test run & display the modal message about exception
// Param: msgTilte - if != NULL - means the dialog is valid, else: no dialog (the exception was caught by CAFE running as a debugger
// Return: void
// END_HELP_COMMENT
void CTarget::HandleExceptionDlg(BOOL bIgnore, BOOL bBreak, LPCSTR msgTitle)
{
	DWORD dsuspendcount=0xFFFFFFFF;
	if(m_hTestthread)
	{
		dsuspendcount=::SuspendThread(m_hTestthread);
		TRACE1("Suspend test thread returned: %d\n", dsuspendcount);
	}
	if(msgTitle  && m_EnableExceptionMessageHandling)
	{ //called when EXCEPTION window is displayed
		TCHAR szExceptText[MAX_TITLE];
		::EnumChildWindows(m_hAssert, (WNDENUMPROC)FindExceptionText, (LPARAM)szExceptText); //sets the text
		m_pLog->RecordFailure("EXCEPTION dialog detected: %s",
			szExceptText);
		if(bBreak)
		{
			char messageText[512];
			sprintf(messageText,"Test [***FAILED***]:\nException dialog detected:\n%s\n%s",
				msgTitle, szExceptText);
			if(!bIgnore)
				strcat(messageText, "\nDialog will be handled by clicking Enter.\n");
			else
				strcat(messageText, "\nDialog will not be handled!!!\n");
			strcat(messageText, "Continue the test?");
			::MessageBox(NULL, messageText, AfxGetAppName(), MB_ICONWARNING|MB_TOPMOST|MB_YESNO);
		}
		if(!bIgnore && ::IsWindow(m_hAssert)) //make sure the ASSERT window is still there
		{
			SetForegroundWindow(m_hAssert);
			keybd_event(VK_RETURN, 0,0,0);
			keybd_event(VK_RETURN, 0,KEYEVENTF_KEYUP,0);
			Sleep(500); //randomly selected value. Do we need to tune it?
		}
	}
	else
	{ //called when no window is displayed
		//TODO fill this later
	}
	if(m_hTestthread && dsuspendcount!=0xFFFFFFFF)
	{
		dsuspendcount=::ResumeThread(m_hTestthread);
		TRACE1("Resume test thread returned: %d\n", dsuspendcount);
	}
}



BOOL CTarget::InitializeSettings(void)
{
	BOOL bResult = TRUE;

	// initialize general default settings
	// filename
	m_psettingsTarget->SetTextValue(settingFilename, "filename");
	// cmd line
	m_psettingsTarget->SetTextValue(settingCmdLine, "cmd line");
	// working dir
	m_psettingsTarget->SetTextValue(settingWorkingDir, ".");
	// launch as debug process
	m_psettingsTarget->SetBooleanValue(settingDebugProcess, TRUE);

	return bResult;
}

CString CTarget::GetThreadResult(HANDLE hThread)
{
	DWORD dwThreadResult;
	CString strMsg;

	// wait for the debug thread to terminate
	::WaitForSingleObject(hThread, INFINITE);

	// get the exit code from the thread
	if (!::GetExitCodeThread(hThread, &dwThreadResult)) {
		TRACE("CTarget::GetDebugThreadResult: unable to retrieve thread's exit code, error: %d\n", ::GetLastError());
	}

	// prepare message based on termination value
	switch ((DebugThreadResult)dwThreadResult) {
		case dtr_success : {
			strMsg.Empty();
			break;
		}
		case dtr_invalidevent : {
			strMsg.Format("Unable to create thread to launch target (%s).", (LPCTSTR)GetFileName());
			break;
		}
		case dtr_exenotfound : {
			strMsg.Format("Unable to find target (%s) on path.", (LPCTSTR)GetFileName());
			break;
		}
		case dtr_invalidexe : {
			strMsg.Format("Invalid EXE (%s).", (LPCTSTR)GetFileName());
			break;
		}
		default : {
			strMsg.Format("Unknown failure launching target (%s).", (LPCTSTR)GetFileName());
			break;
		}
	}

	return strMsg;
}

CString CTarget::GetUniqueName(void)
{
	CString strName;

	strName.Format("Target %d", m_nId);

	return strName;
}

BOOL CTarget::SetLog(CLog* pLog)
{
	// the log shouldn't be set to NULL
	ASSERT(pLog);

	// set the target's log
	m_pLog = pLog;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTarget debug event handlers

#if defined(_M_MRX000)
CArray<HANDLE, HANDLE> tid_handle;
#endif

void CTarget::HandleDebugEvents(void)
{
	DEBUG_EVENT DebugEvent;
	DWORD dwContinue;
	HANDLE hTerminate;

	// open the termination event
	hTerminate = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, eventTerminate + GetUniqueName());

	// the handle must be valid
	ASSERT(hTerminate);
	if (!hTerminate) {
		TRACE("CTarget::HandleDebugEvent: unable to open the terminate event\n");
		return;
	}

#if defined(_M_MRX000)
	tid_handle.RemoveAll();
#endif
	do {
		// handle any debug events that are waiting
		if (::WaitForDebugEvent(&DebugEvent, 1000)) {

			// handle the debug event
			switch(DebugEvent.dwDebugEventCode)
			{
				case EXCEPTION_DEBUG_EVENT:
					dwContinue = HandleException(DebugEvent);
					break;
				case CREATE_THREAD_DEBUG_EVENT:
					dwContinue = HandleCreateThread(DebugEvent);
					break;
				case CREATE_PROCESS_DEBUG_EVENT:
					dwContinue = HandleCreateProcess(DebugEvent);
					break;
				case EXIT_THREAD_DEBUG_EVENT:
					dwContinue = HandleExitThread(DebugEvent);
					break;
				case EXIT_PROCESS_DEBUG_EVENT:
					dwContinue = HandleExitProcess(DebugEvent);
					break;
				case LOAD_DLL_DEBUG_EVENT:
					dwContinue = HandleLoadDLL(DebugEvent);
					break;
				case UNLOAD_DLL_DEBUG_EVENT:
					dwContinue = HandleUnloadDLL(DebugEvent);
					break;
				case OUTPUT_DEBUG_STRING_EVENT:
					dwContinue = HandleOutputDebugString(DebugEvent);
					break;
				case RIP_EVENT:
					dwContinue = HandleRIP(DebugEvent);
					break;
			}
			VERIFY(::ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, dwContinue));
		}
		else {
			if (::WaitForSingleObject(hTerminate, 0) == WAIT_OBJECT_0) {
				::CloseHandle(hTerminate);
				return;
			}
		}
	}
	// if the process being debugged exits, we should exit
	while (!(DebugEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT && DebugEvent.dwProcessId == m_procinfoTarget.dwProcessId));

	// close handles
	::CloseHandle(hTerminate);
}

DWORD CTarget::HandleException(const DEBUG_EVENT& DebugEvent)
{
	// apparently there is a breakpoint exception that is thrown when a process (just the IDE?)
	// starts up. We need to ignore this exception the first time. (m_bInitProcess == TRUE the first time.)

	// is it a breakpoint exception?
	if(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT && m_bInitProcess) {
		m_bInitProcess = FALSE;

#if defined(_M_MRX000)
		HANDLE hThread;
		DWORD idx = DebugEvent.dwThreadId;
		if (INVALID_HANDLE_VALUE != (hThread = tid_handle[idx])) {
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_CONTROL;
			if (!GetThreadContext(hThread, &ctx)) {
				return(DBG_EXCEPTION_NOT_HANDLED);
			}
			ctx.Fir += 4; // skip over breakpoint
			if (!SetThreadContext(hThread, &ctx)) {
				return(DBG_EXCEPTION_NOT_HANDLED);
			}
		}
#endif

		// we handled this special case exception
		return DBG_CONTINUE;
	}
	// is it a second chance exception?
	else if (DebugEvent.u.Exception.dwFirstChance == 0) {
		// we got an exception
		// generate a description
		char acBuf[255];
		sprintf(acBuf, "Detected an exception (%s) in process (0x%X) at address (0x%X).",
			GetExceptionDescription(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode),
			DebugEvent.dwProcessId, DebugEvent.u.Exception.ExceptionRecord.ExceptionAddress);
		// log the exception
		m_pLog->RecordFailure(acBuf);
		
		BOOL bIgnore = GetSettings()->GetBooleanValue(settingIgnoreASSERT);
		BOOL bBreak = GetSettings()->GetBooleanValue(settingBreakASSERT);
		if(!bIgnore && bBreak)
		{
			strcat(acBuf, "Attach the JIT debugger?");
			::MessageBox(NULL, acBuf, AfxGetAppName(), MB_ICONWARNING|MB_TOPMOST|MB_YESNO);
		}

		APPLOG(acBuf);
		return DBG_EXCEPTION_NOT_HANDLED;
	}

	// by default we don't handle exceptions
	return DBG_EXCEPTION_NOT_HANDLED;
}

DWORD CTarget::HandleCreateThread(const DEBUG_EVENT& DebugEvent)
{
#if defined(_M_MRX000)
	tid_handle.SetAtGrow(DebugEvent.dwThreadId, DebugEvent.u.CreateThread.hThread);
#else
	UNREFERENCED_PARAMETER(DebugEvent);
#endif
	return DBG_CONTINUE;

}

DWORD CTarget::HandleCreateProcess(const DEBUG_EVENT& DebugEvent)
{
	// no memory leaks detected yet
	m_nLeaks = 0;

	// apparently there is a breakpoint exception that is thrown when a process (just the IDE?)
	// starts up. We need to ignore this exception the first time.
	m_bInitProcess = TRUE;

#if defined(_M_MRX000)
	tid_handle.SetAtGrow(DebugEvent.dwThreadId, DebugEvent.u.CreateProcessInfo.hThread);
#else
	UNREFERENCED_PARAMETER(DebugEvent);
#endif
	return DBG_CONTINUE;

}

DWORD CTarget::HandleExitThread(const DEBUG_EVENT& DebugEvent)
{
#if defined(_M_MRX000)
	tid_handle[DebugEvent.dwThreadId] = INVALID_HANDLE_VALUE;
#else
	UNREFERENCED_PARAMETER(DebugEvent);
#endif
	return DBG_CONTINUE;
}

DWORD CTarget::HandleExitProcess(const DEBUG_EVENT& DebugEvent)
{
	return DBG_CONTINUE;

	UNREFERENCED_PARAMETER(DebugEvent);
}

DWORD CTarget::HandleLoadDLL(const DEBUG_EVENT& DebugEvent)
{
	return DBG_CONTINUE;

	UNREFERENCED_PARAMETER(DebugEvent);
}

DWORD CTarget::HandleUnloadDLL(const DEBUG_EVENT& DebugEvent)
{
	return DBG_CONTINUE;

	UNREFERENCED_PARAMETER(DebugEvent);
}

DWORD CTarget::HandleOutputDebugString(const DEBUG_EVENT& DebugEvent)
{
	static bLeakDump = FALSE;
	static LPCSTR szAssert = "Assertion Failed:";
	static LPCSTR szLeakBegin = "Dumping objects ->";
	static LPCSTR szLeakEnd = "Object dump complete";
	static const char cLeak1 = '{';
	static const char cLeak2 = '}';

	m_strDebugOutput += GetOutputDebugString(DebugEvent);

	// Wait till we get a full string.
	if (strchr(m_strDebugOutput, '\n') || strchr(m_strDebugOutput, '\r')) {

		char* psz = m_strDebugOutput.GetBuffer(m_strDebugOutput.GetLength());

		// If the debug string starts with "$CafeLog" and DebugOutput is turned
		// on, then log the message.
		if (m_pLog->GetLogDebugOutput() && strstr(psz, "$CafeLog") == 0) {
			// Strip off any trailing newlines.
			char* p;
			if (p = strchr(psz, '\n'))
				*p = '\0';

			if (p = strchr(psz, '\r'))
				*p = '\0';

			m_pLog->RecordInfo("DbgMsg - %s", psz);
		}

		m_strDebugOutput.ReleaseBuffer();

		char acBuf[1024];

		// dump memory leaks
		if (bLeakDump) {
			// when bLeakDump is set, dump string until terminating line.
			if (strncmp(m_strDebugOutput, szLeakEnd, sizeof(szLeakEnd) - 1) == 0) {
				// generate a message for the end of the memory leaks
				sprintf(acBuf, "Detected %d memory leaks in the application.", m_nLeaks);
				// log the number of memory leaks
				m_pLog->RecordInfo(acBuf);
//				m_pLog->RecordInfo("View memory leaks in the application log (log toolbar button).");
				APPLOG(acBuf);
// log a failure if the number of leaks is greater than expected
// log an info if any leaks are detected but expected
// do not log anything if m_nExpectedLeaks == -1 (turned off)
				if (m_nExpectedLeaks != -1) {
					if (m_nLeaks > m_nExpectedLeaks) {
						m_pLog->RecordFailure("The number of memory leaks detected in the application (%d) is greater than the expected number of leaks (%d).", m_nLeaks, m_nExpectedLeaks);
					}
					else if (m_nLeaks < m_nExpectedLeaks) {
						m_pLog->RecordInfo("The number of memory leaks detected in the application (%d) is less than the expected number of leaks (%d).", m_nLeaks, m_nExpectedLeaks);
						m_pLog->RecordInfo("Please contact the sniff owner to adjust the expected number of memory leaks.");
					}
				}
				
				bLeakDump = FALSE;
			}
			else {
				// log the memory leak
				m_strDebugOutput.TrimRight();
				// Make "%" into "%%" for printf, only does the first one (bjoyce)
				m_strDebugOutput.Replace("%", "%%");
				m_pLog->RecordInfo((LPCSTR)m_strDebugOutput);
				APPLOG((LPCSTR)m_strDebugOutput);
				// each memory leak may take multiple lines:
				// each new leak contains a { followed by a } somewhere in the line
				int nLeak1 = m_strDebugOutput.Find(cLeak1);
				int nLeak2 = m_strDebugOutput.Find(cLeak2);
				if (nLeak1 < nLeak2) {
					m_nLeaks++;
				}
			}
		}
		// is this an assert?
		else if (strncmp(m_strDebugOutput, szAssert, sizeof(szAssert) - 1) == 0) {
			// generate a message to describe the assert
			sprintf(acBuf, "Detected an assert: %s.", (LPCSTR)m_strDebugOutput);
			// log the assert
			m_pLog->RecordFailure(acBuf);
			APPLOG(acBuf);

			// REVIEW(briancr): don't we need to clear the assert message box?
		}
		// is this the beginning of memory leaks?
		else if (strncmp(m_strDebugOutput, szLeakBegin, sizeof(szLeakBegin) - 1) == 0) {
			bLeakDump = TRUE;
			sprintf(acBuf, "Detected the following memory leaks:");
			m_pLog->RecordInfo(acBuf);
			APPLOG(acBuf);
		}

		m_strDebugOutput.Empty();
	}

	return DBG_CONTINUE;
}

DWORD CTarget::HandleRIP(const DEBUG_EVENT& DebugEvent)
{
	// generate a message for the RIP
	char acBuf[255];
	sprintf(acBuf, "Detected a RIP (%s) in process (0x%X) with error value (%d).",
		GetRIPDescription(DebugEvent.u.RipInfo.dwType),
		DebugEvent.dwProcessId, DebugEvent.u.RipInfo.dwError);
	m_pLog->RecordFailure(acBuf);
	APPLOG(acBuf);

	return DBG_CONTINUE;
}

LPCSTR CTarget::GetExceptionDescription(DWORD dwCode)
{
	switch (dwCode) {
// from WINBASE.H
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "Datatype misalignment";
		case EXCEPTION_BREAKPOINT:
			return "Breakpoint";
		case EXCEPTION_SINGLE_STEP:
			return "Single step";
		case EXCEPTION_ACCESS_VIOLATION:
			return "Access violation";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "Array bound exceeded";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "Floating-point denormal operand";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "Floating-point divide by zero";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return "Floating-point inexact result";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return "Floating-point invalid operation";
		case EXCEPTION_FLT_OVERFLOW:
			return "Floating-point overflow";
		case EXCEPTION_FLT_STACK_CHECK:
			return "Floating-point stack check";
		case EXCEPTION_FLT_UNDERFLOW:
			return "Floating-point underflow";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "Integer divide by zero";
		case EXCEPTION_INT_OVERFLOW:
			return "Integer overflow";
		case EXCEPTION_PRIV_INSTRUCTION:
			return "Privileged instruction";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return "Noncontinuable Exception";
/*
// from NTSTATUS.H
		case STATUS_DLL_NOT_FOUND:
			return "DLL not found";
*/
		default:
			return "Unknown exception code";
	}
}

CString& CTarget::GetOutputDebugString(const DEBUG_EVENT& DebugEvent)
{
	static CString str;

	ASSERT(DebugEvent.dwDebugEventCode == OUTPUT_DEBUG_STRING_EVENT);

	const OUTPUT_DEBUG_STRING_INFO* pODSI = &DebugEvent.u.DebugString;
	LPSTR pch = str.GetBufferSetLength(pODSI->nDebugStringLength + 1);
	DWORD dwRead;

	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, DebugEvent.dwProcessId);
	if (!ReadProcessMemory( hProcess,					// process to read
							pODSI->lpDebugStringData,	// address to read at
							(LPVOID)pch,				// address to copy to
							pODSI->nDebugStringLength,	// bytes to copy
							&dwRead) 					// bytes actually read
			&& (dwRead > 0) ) {

//		WriteWarning("Problem retrieving OutputDebugString.  Partial read was: %s", pch);
	}
	else {
		if (pch[dwRead] != '\0') {
			pch[dwRead] = '\0';
		}
	}

	str.ReleaseBuffer(-1);

	return str;
}

LPCSTR CTarget::GetRIPDescription(DWORD dwType)
{
	switch (dwType)
	{
		case SLE_ERROR:
			return "Error";
		case SLE_MINORERROR:
			return "Minor error";
		case SLE_WARNING:
			return "Warning";
		default:
			return "0";
	}
}

// BEGIN_HELP_COMMENT
// Function: CString CTarget::GetVersion()
// Return: Returns a string that represents the version number of the target.
// END_HELP_COMMENT
CString CTarget::GetVersion()
{
	DWORD   dwVerInfoSize;
	DWORD   dwVerHnd;
  CString strVersion("Not Available");

	int nPos;
	CString strFullFileName;

	if ((nPos = m_strLastCmdLine.Find(".exe ")) != -1)
		strFullFileName = m_strLastCmdLine.Left(nPos+4);
	else
		strFullFileName = m_strLastCmdLine;

	dwVerInfoSize = GetFileVersionInfoSize((LPSTR)(LPCSTR)strFullFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		LPVOID  lpstrVffInfo;
		HANDLE  hMem;
		LPSTR   lpVersion = NULL;       
		UINT    uVersionLen = 0;
	
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpstrVffInfo  = GlobalLock(hMem);
		GetFileVersionInfo((LPSTR)(LPCSTR)strFullFileName, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

		//Todo (ScotF): This is a U.S. English specific query, will not work on Japanese IDE
		if(VerQueryValue(lpstrVffInfo, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID *)&lpVersion, (UINT *)&uVersionLen))
			strVersion = lpVersion;

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}

  return strVersion;
}

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Target DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Target DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);

	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}
