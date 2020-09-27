// loops.cpp 

#include "stdafx.h"

void CHandleArray::CloseAll (void)
{
	for (int ii = 0; ii < GetCount(); ii++)
	{
		CloseHandle ((*this)[ii]);
	}
}

DWORD CHandleArray::Wait (bool all, DWORD millisec)
{
	return WaitForMultipleObjects (GetCount(), GetArray(), all, millisec);
}

// StartThread - used to start a test
//
// By using _beginthreadex and _endthreadex, the thread handle
// is not automatically closed on exit.  This is important
// because we are using the handle to monitor thread activity
// so we don't want the handle value to be reused by the OS
// until we have purged it from our tables.
//
HANDLE CThreadable::StartThread (LPCTSTR name)
{
	ASSERT (m_hThread == NULL);
	
	UINT ignore; // threadID
	DWORD result;
	result = _beginthreadex (NULL, 0, &ThreadEntry, this, 0, &ignore);

	if (result == 0)
	{
		LPCTSTR pTemp = CGetLastError();
		g_error.ThrowFailedToStartThread (name, pTemp);
	}

	return m_hThread = (HANDLE) result;
}

// ThreadEntry 
//
// This is the first code that is executed in the thread context
//
unsigned __stdcall CThreadable::ThreadEntry (LPVOID pParam)
{
	CThreadable *obj = (CThreadable *) pParam;


	bool result = obj->ThreadMain ();

	// And return the result 
	//
	_endthreadex (result); 
	return result;
}

bool CLoopable::ThreadMain ()
{
	bool failed = true; // assume the worst

	// Setup a custom exception handler so that win32 structured
	// exceptions can be converted into C++ exceptions
	//
	_set_se_translator (CustomExceptionTranslator);

	try
	{
		CTimer timer (this, m_timelimit * 1000);

		if (LoopStart () && !m_abortFlag)
		{
			int count = 0; 
			while (((count < m_looplimit)||(m_looplimit == 0)) && !m_abortFlag)
			{
				LoopMain ();
				count++;
			}

			failed = false;
		}
	}
	catch (CXmtaException &x) 
	{
		if (x.GetErrorCode())
			g_dispatch.ErrorMessage (x);
	}
//#ifndef _DEBUG
	catch (CStructuredException &e)
	{
		g_main.StopThreads ();
		g_error.ReportStructuredExecption (
			e.GetExceptionString (),
			e.GetExceptionCode (),
			e.GetExceptionAddress ());
	}
	catch (...)
	{
		g_main.StopThreads ();
		g_error.ReportStructuredExecption (_T("Unidentified exception"), 0, 0);
	}
//#endif

	LoopExit ();
	return failed;
}

void CLoopable::InitLoopControl (CParameterTable *local, CLoopable *parent)
{
	if (parent && parent->m_objHaltOnError)
		m_objHaltOnError = parent->m_objHaltOnError;

	if (local == NULL)
		return;

	m_localParameters = local;

	// Not inherited 
	//
	m_looplimit = local->GetCfgInt (NULL, _T("looplimit"), 1);
	m_debugmask = local->GetCfgUint (NULL, _T("debugmask"), 0);
	m_timelimit = local->GetCfgInt (NULL, _T("timelimit"), 0);

	// Query the host for elapsed time if required.
	//
	int elapsedlimit = local->GetCfgInt (NULL, _T("elapsedlimit"), 0);
	if (elapsedlimit)
	{
		int elapsed = (int) g_host.GetHostDword (MID_UUT_GET_ELAPSED_TIME, 0);
		if (elapsed >= elapsedlimit)
			OnSignalAbort();
		else
			m_timelimit = (elapsedlimit - elapsed);
	}

	// Set the halt on error object ...
	// when an error occurs and this value is not NULL,
	// the objects OnSignalAbort() is called which will cascade the 
	// abort signalls to all its children
	//
	m_haltonerror = local->GetCfgBoolean (NULL, _T("haltonerror"), false);
	if (!m_objHaltOnError)
	{
		if (m_haltonerror)
			m_objHaltOnError = this;
		else
			m_objHaltOnError = NULL;
	}

	// Inherited
	//
	if (parent)
	{
		m_maxthreads = parent->m_maxthreads;
		m_maxtimeout = parent->m_maxtimeout;
		m_reportwarnings = parent->m_reportwarnings;
		m_reportstatistics = parent->m_reportstatistics;
	}
	m_maxthreads = local->GetCfgInt (NULL, _T("maxthreads"), m_maxthreads);
	m_maxtimeout = local->GetCfgInt (NULL, _T("maxtimeout"), m_maxtimeout);

	m_reportwarnings = local->GetCfgBoolean (NULL, _T("reportwarnings"), m_reportwarnings ? true : false);
	m_reportstatistics = local->GetCfgBoolean (NULL, _T("reportstatistics"), m_reportstatistics ? true : false);
}


CLoopManager::CLoopManager ():
	m_loop(NULL), m_parent(NULL)
{
	m_pollingPeriod = 100;
}

CLoopManager::CLoopManager (CCommandItem *loop, CLoopable *parent):
	m_loop(loop), m_parent(parent)
{
	m_pollingPeriod = 100;
}

CLoopManager::~CLoopManager ()
{
}

void CLoopManager::StartTesting (CCommandItem *loop)
{
	m_loop = loop;

	CParameterTable *pPT = m_loop->GetParameters ();
	InitLoopControl (pPT, m_parent);

	m_senduutstarting = pPT->GetCfgBoolean (NULL, _T("senduutstarting"), m_senduutstarting ? true : false);

	if (m_senduutstarting)
		g_dispatch.TestingStarted ();
	LoopMain ();
	g_dispatch.TestingEnded ();
}

void CLoopManager::AddThread (CLoopable *test, HANDLE thread)
{
	ASSERT (!m_objectArray.IsFull ());

	m_objectArray.Append (test);
	m_threadArray.Append (thread);
}

void CLoopManager::RemoveThread (int arrayIndex)
{
	ASSERT (!m_threadArray.IsEmpty ());

	HANDLE thread = m_threadArray.RemoveAt (arrayIndex);
	CLoopable *obj = m_objectArray.RemoveAt (arrayIndex);

	CloseHandle (thread);
	delete obj;
}

void CLoopManager::StopThreads ()
{
	InterlockedIncrement (&m_abortFlag);

	for (int ii = 0; ii < m_objectArray.GetCount (); ii++)
	{
		CLoopable *obj = m_objectArray[ii];
		obj->OnSignalAbort ();
	}
}

void CLoopManager::StartTest (LPCTSTR name, CParameterTable *local)
{
	if (m_objectArray.IsFull ())
		return;

	CTestObj *test = NULL;
	HANDLE thread = NULL;

	try
	{
		test = CTestObj::CreateTest (name);
		test->InitLoopControl (local, this);
		thread = test->StartThread (test->GetFullName ());
	}
	catch (...) // catches any constructor exceptions 
	{
		delete test;
		g_main.StopThreads ();
	}

	AddThread (test, thread);
}

void CLoopManager::StartLoop (CCommandItem *cmd)
{
	if (m_objectArray.IsFull ())
		return;

	CLoopManager *loop = new CLoopManager (cmd, this);
	loop->InitLoopControl (cmd->GetParameters (), this);
	HANDLE thread = loop->StartThread (cmd->GetName ());

	AddThread (loop, thread);
}

void CLoopManager::RunTest (LPCTSTR name, CParameterTable *local)
{
	StartTest (name, local);
	WaitLoop ();
}

void CLoopManager::RunLoop (CCommandItem *cmd)
{
	StartLoop (cmd);
	WaitLoop ();
}

bool CLoopManager::LoopMain ()
{
	TlsSetValue (g_tlsTestObj, NULL);

	CCommandItem *cmd = (CCommandItem *) m_loop->GetFirstChild ();

	while (cmd && !m_abortFlag)
	{
		switch (cmd->GetCommandType())
		{
		case XCMD_STARTTEST:
			StartTest (cmd->GetName(), cmd->GetParameters());
			break;
		case XCMD_RUNTEST:
			RunTest (cmd->GetName(), cmd->GetParameters());
			break;
		case XCMD_STARTLOOP:
			StartLoop (cmd);
			break;
		case XCMD_RUNLOOP:
			RunLoop (cmd);
			break;
		case XCMD_EXIT:
			g_host.iSendHost (MID_UUT_RESTARTING, NULL, 0);
#ifndef STAND_ALONE_MODE
			g_host.DisconnectFromHost ();
#endif
#ifdef _XBOX
			// do xbox exit here
#endif
			_exit(0);
			break;
		default:
			ASSERT_HERE;
			g_error.ThrowInvalidCommand (_T("Unknown"));
		}

		cmd = (CCommandItem *) cmd->GetNextSibling ();
	}

	WaitLoop ();
	return true;
}

void CLoopManager::WaitLoop ()
{
	while (!m_threadArray.IsEmpty ())
	{
		DWORD result = m_threadArray.WaitForAny (m_pollingPeriod);
		ASSERT (result != WAIT_FAILED);
		if (result == WAIT_TIMEOUT)
			CheckThreads ();
		else
			RemoveThread (result - WAIT_OBJECT_0);
	}
}

void CLoopManager::CheckThreads ()
{
	ASSERT (!m_objectArray.IsEmpty());

	for (int ii = 0; ii < m_objectArray.GetCount (); ii++)
	{
		CLoopable *obj = m_objectArray[ii];
		if (obj->CheckTimeout (m_maxtimeout, m_objectArray.GetCount()))
		{
			// Test has timed out.
			// It could either be waiting on an object
			// or burning cycles in an infinite loop
			// but for now we will just punt
			//
			// g_error.ThrowThreadAppearsHung (obj->GetName ());
			SuspendThread (m_threadArray[ii]);
			RemoveThread (ii);
		}
	}
}
