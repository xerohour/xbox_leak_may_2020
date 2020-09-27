// dispatch.cpp

#include "stdafx.h"

CDispatcher::CDispatcher ()
{
}

CDispatcher::~CDispatcher ()
{
}

void CDispatcher::SendMsg (CUutMsg* msg)
{
	BYTE buffer [2048];
	CStore st (buffer, 2048);
	msg->Store (st);

	g_host.iSendHost (msg->Type, st.GetBuffer (), st.GetLength ());
}

void CDispatcher::TestingStarted (void)
{
	CMsg::TestingStarted msg;
	SendMsg (&msg);
}

void CDispatcher::TestingActive (void)
{
	CMsg::TestingActive msg;
	SendMsg (&msg);
}

void CDispatcher::TestingEnded ()
{
	CMsg::TestingEnded msg;
	SendMsg (&msg);
}

void CDispatcher::TestStarted (CTestObj *test)
{
	CMsg::TestStarted msg (test->GetFullName());
	SendMsg (&msg);
}

void CDispatcher::TestEnded (CTestObj *test)
{
	CMsg::TestEnded msg (test->GetFullName(), 
		test->GetElapsedTime(), test->GetTestPassed());
	SendMsg (&msg);
}

void CDispatcher::WarningMessage (CTestObj *test, LPCTSTR message)
{
	CMsg::Warning msg (test->GetFullName(), message);
	SendMsg (&msg);
}

void CDispatcher::DebugMessage (CTestObj *test, LPCTSTR message)
{
	CMsg::Debug msg (test->GetFullName(), message);
	SendMsg (&msg);
}

void CDispatcher::StatisticsMessage (CTestObj *test, LPCTSTR key, LPCTSTR message)
{
	CMsg::Statistic msg (test->GetFullName(), key, message);
	SendMsg (&msg);
}

void CDispatcher::ErrorMessage (LPCTSTR name,
						 LPCTSTR message,
						 int moduleNumber,
						 int testNumber,
						 int errorCode)
{
	ASSERT(moduleNumber < 0x1000);
	ASSERT(testNumber < 0x100);
	ASSERT(errorCode < 0x1000u);
	DWORD sec = (moduleNumber << 20) + (testNumber << 12) + errorCode;

//	CAutoString msg (_T("*** Error %s.%s\n%sStandard Error Code = %8.8x\n"),
//							moduleName, testName, message, sec);

	CMsg::Error msg (name, sec, message);
	SendMsg (&msg);
}

void CDispatcher::ErrorMessage (CTestObj *test, WORD errorCode, LPCTSTR message)
{
	// Try to get the test object from the thread manager
	if (test == NULL)
		test = (CTestObj *) TlsGetValue (g_tlsTestObj);

	// If the thread manager can't find it, it must be the executive
	if (test == NULL)
		ErrorMessage (_T("XMTA.TestExecutive"), 
						message, 
						MODNUM_XMTA, 
						0, 
						errorCode);
	else
		ErrorMessage (test->GetFullName (), 
						message, 
						test->GetModuleNumber (), 
						test->GetTestNumber (), 
						errorCode);
}


