// testobj.cpp 
//

#include "stdafx.h"

//
// Test Factory stuff
//

bool CTestObj::LookupTest (LPCTSTR module, LPCTSTR test) 
{
	return (m_factory.LookupTest (module, test) != NULL);
}

CTestObj *CTestObj::CreateTest (LPCTSTR name) 
{
	return m_factory.CreateTest (name);
}

void CTestObj::RegisterTest (LPCTSTR name, CreateFunction func)
{
	m_factory.RegisterTest (name, func);
}

bool CTestObj::LoopMain ()
{
	if (!CheckAbort (NULL))
	{
		m_startTime = GetTickCount ();
		g_dispatch.TestStarted (this);
		TestMain ();
		m_elapsedTime = GetTickCount() - m_startTime;
		g_dispatch.TestEnded (this);
	}

	return true;
}

bool CTestObj::LoopStart ()
{
	// Store a pointer to the test in thread local storage at
	// a known index.  This allows functions in other classes
	// to identify the test without passing the test object ptr
	// through function arguments.
	//
	TlsSetValue (g_tlsTestObj, this);

	LockModule ();
	LockTest ();

	return InitializeParameters ();
}

void CTestObj::LoopExit ()
{
	UnlockTest ();
	UnlockModule ();
}


// 
// Public interface to the test object
//

// StopTest - used to stop a test
//
void CTestObj::StopTest ()
{
	InterlockedIncrement (&m_abortFlag);
}

// Constructor
//
CTestObj::CTestObj ()
{
	m_hThread = NULL;
	m_abortFlag = 0;
	m_settimeout = 0;
	m_startTime = 0;
	m_elapsedTime = 0;

	m_lastCheckAbort = GetTickCount ();
	m_lastCheckAbortHere [0] = 0;
	m_handlesExceptions = false;

	m_semaphore = NULL;
	m_mutexTest = NULL;

	m_lastThreadTime = 0;
	m_errorCount = 0;
}

bool CTestObj::InitializeParameters ()
{
	return true;
}

// Destructor
//
CTestObj::~CTestObj (void)
{
	// We expect that the thread has been long dead at this point
	// 
	if (m_hThread != NULL)
	{
		DWORD result = WaitForSingleObject (m_hThread, 0);
		ASSERT (result != WAIT_TIMEOUT);
		if (result == WAIT_TIMEOUT)
			g_error.ReportTestThreadWontDie ();
	}
}

bool CTestObj::IsThreadRunning ()
{
//	const ULONGLONG ONE_SECOND = 10000000i64;
//	const ULONGLONG ONE_MILSEC = 10000i64;

	ULONGLONG creation, exit, kernal, user;

	GetThreadTimes (m_hThread, 
		(FILETIME *) &creation, 
		(FILETIME *) &exit, 
		(FILETIME *) &kernal, 
		(FILETIME *) &user);

	ULONGLONG thisThreadTime = (kernal + user);
	bool running = (thisThreadTime > m_lastThreadTime);
	m_lastThreadTime = thisThreadTime;
	return running;
}

bool CTestObj::CheckTimeout (DWORD /*maxtimeout*/, DWORD loadfactor)
{
	DWORD now;
	DWORD settimeout =  (m_settimeout)? m_settimeout:m_maxtimeout;
	DWORD dwLocalCheckAbort;

	dwLocalCheckAbort = m_lastCheckAbort;
	now = GetTickCount ();
	
	// First check to see if we have exceeded the limit set by the test
	//
	if ((now - dwLocalCheckAbort) > (settimeout * 1000 * loadfactor))
	{
		ReportWarning (_T("B Timelimit %lu current %lu load %lu"), 
			settimeout, now-dwLocalCheckAbort, loadfactor);
		g_error.ReportTestTimeout (GetFullName (), IsThreadRunning (), 
			settimeout, loadfactor, m_lastCheckAbortHere);
		return true;
	}

	// Second check to see if the test has raised the timeout value
	// higher than the INI file value.
	//
	if ((now - dwLocalCheckAbort) > (m_maxtimeout * 1000 * loadfactor))
	{
		ReportWarning (_T("%s set timeout limit higher than default (ie, %d > %d)"), 
			GetFullName (), m_settimeout, m_maxtimeout);
	}

	return false;
}

//
// XMTA Test API
// 

CParameterItem *CTestObj::FindParameter (LPCTSTR name)
{
	CParameterItem *param = m_localParameters?
		m_localParameters->FindParameter (NULL, name):NULL;
	return param? param:g_parameters.FindParameter (GetModuleName(), name);
}

bool CTestObj::GetCfgBoolean (LPCTSTR name)
{
	CParameterItem *param = FindParameter (name);
	return (param != NULL)? param->GetBoolean ():false;
}

int CTestObj::GetCfgInt (LPCTSTR name, int defaultInt)
{
	CParameterItem *param = FindParameter (name);
	return (param != NULL)? param->GetInt ():defaultInt;
}

UINT CTestObj::GetCfgUint (LPCTSTR name, UINT defaultUint)
{
	CParameterItem *param = FindParameter (name);
	return (param != NULL)? param->GetUint ():defaultUint;
}

LPCTSTR CTestObj::GetCfgString (LPCTSTR name, LPCTSTR defaultStr)
{
	CParameterItem *param = FindParameter (name);
	return (param != NULL)? param->GetValue ():defaultStr;
}

int CTestObj::GetCfgChoice (LPCTSTR name, int defaultChoice, ...)
{
	va_list arglist;
	va_start (arglist, defaultChoice);

	CParameterItem *param = FindParameter (name);
	return (param != NULL)? param->GetChoice (arglist):defaultChoice;
}

bool CTestObj::vReportError (WORD errorCode, LPCTSTR format, va_list argptr)
{
	_TCHAR message [2048];
	_vsntprintf (message, 2048, format, argptr);

	int last = _tcslen(message) - 1;
	if (message [last] == _T('\n'))
		message [last] = _T('\0');

	ASSERT (GetModuleNumber() < 0x1000);
	ASSERT (GetTestNumber() < 0x100);
	ASSERT (errorCode < 0x1000u);

	g_dispatch.ErrorMessage (this, errorCode, message);

	m_errorCount++;

	if (m_objHaltOnError)
		m_objHaltOnError->OnSignalAbort (); // this just sets the abort flag

	if (m_handlesExceptions && m_abortFlag)
		throw CXmtaException (0, NULL);

	return (m_abortFlag != 0); // if abort return true
}

bool CTestObj::ReportError (WORD errorCode, LPCTSTR format, ...)
{
	va_list argptr;
	va_start (argptr, format);

	return vReportError (errorCode, format, argptr);
}

void CTestObj::ReportWarning (LPCTSTR format, ...)
{
	if (!m_reportwarnings)
		return;

	va_list argptr;
	va_start (argptr, format);

	_TCHAR message [2048];
	_vsntprintf (message, 2048, format, argptr);

	int last = _tcslen(message) - 1;
	if (message [last] == _T('\n'))
		message [last] = _T('\0');

	g_dispatch.WarningMessage (this, message);
}

void CTestObj::ReportDebug (DWORD debugFilter, LPCTSTR format, ...)
{
	if ((debugFilter & m_debugmask) == 0)
		return;

	va_list argptr;
	va_start (argptr, format);

	_TCHAR message [2048];
	_vsntprintf (message, 2048, format, argptr);

	int last = _tcslen(message) - 1;
	if (message [last] == _T('\n'))
		message [last] = _T('\0');

	g_dispatch.DebugMessage (this, message);
}

void CTestObj::ReportStatistic (LPCTSTR key, LPCTSTR format, ...)
{
	if (!m_reportstatistics)
		return;

	va_list argptr;
	va_start (argptr, format);

	_TCHAR message [2048];
	_vsntprintf (message, 2048, format, argptr);

	int last = _tcslen(message) - 1;
	if (message [last] == _T('\n'))
		message [last] = _T('\0');

	g_dispatch.StatisticsMessage (this, key, message);
}

void CTestObj::SetTimeoutLimit (int limit)
{
	InterlockedExchange ((long*) &m_settimeout, limit);
}

bool CTestObj::AbortSensitiveWait (HANDLE h, LPCTSTR footprint)
{
	DWORD result = WAIT_TIMEOUT;
	while (result == WAIT_TIMEOUT)
	{
		if (CheckAbort (footprint))
			return false;

		result = WaitForSingleObject (h, 1000);
	}
	return true;
}

bool CTestObj::LockTest () 
{
	char szFName[256];

	if (!WantTestLocked ())
	{
		m_mutexTest = INVALID_HANDLE_VALUE;
		return false;
	}

	if (m_mutexTest != NULL)
	{
		ReportError (0x04, _T("LockTest can not be called from a test"));
		return false;
	}
	sprintf(szFName, "%S", GetFullName());	
	m_mutexTest = CreateMutexA (NULL, true, szFName);
	return AbortSensitiveWait (m_mutexTest, _T("waiting on test lock"));
}

void CTestObj::UnlockTest () 
{
	if (!WantTestLocked ())
		return;

	ReleaseMutex (m_mutexTest); 
	CloseHandle (m_mutexTest);
}

#define XMTA_THREAD_LIMIT 10

bool CTestObj::LockModule () 
{
	char szFName[256];

	int threadLimit = XMTA_THREAD_LIMIT;

	int needed = WantTestLocked()? threadLimit:1;
	if (!WantModuleLocked ())
	{
		m_semaphore = INVALID_HANDLE_VALUE;
		return false;
	}

	ASSERT (m_semaphore == NULL);
	if (m_semaphore != NULL)
	{
		ReportError (0x04, _T("LockModule can not be called from a test"));
		return false;
	}

	sprintf(szFName, "%S", GetModuleName());	
	m_semaphore = CreateSemaphoreA (NULL, threadLimit, threadLimit, szFName);

	while (needed > 0)
	{
		if (!AbortSensitiveWait (m_semaphore, _T("Waiting on module semaphore")))
			return false;
		needed--;
	}
	
	return true;
}

void CTestObj::UnlockModule () 
{
	int needed = WantTestLocked()? XMTA_THREAD_LIMIT:1;
	if (!WantModuleLocked ())
		return;

	ASSERT (m_semaphore != NULL);
	if (m_semaphore == NULL)
		return;

	ReleaseSemaphore (m_semaphore, needed, NULL); 
	CloseHandle (m_semaphore);
	m_semaphore = NULL;
}


// CheckAbort
//
// This is a multipurpose function.
// It check the abort flag to see if its time to exit,
// it sets a string for footprinting
// and it updates the timestamp.
//
bool CTestObj::CheckAbort (LPCTSTR here)
{
	if (here) // foot printing
	{
		// skip the slashes
		LPCTSTR lastslash = _tcsrchr (here, _T('\\'));
		if (lastslash)
			here = _tcsinc (lastslash);

		_tcscpy (m_lastCheckAbortHere, here); 
	}

	InterlockedExchange ((long*) &m_lastCheckAbort, GetTickCount ());
	return (m_abortFlag != 0);
}
