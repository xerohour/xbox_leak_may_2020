// dispatcher.h

#pragma once

class CDispatcher
{
private:
	void SendMsg (CUutMsg* msg);

public:
	CDispatcher ();
	virtual ~CDispatcher ();

	void TestingStarted (void);
	void TestingActive (void);
	void TestingEnded ();
	void TestStarted (CTestObj *test);
	void TestEnded (CTestObj *test);
	void WarningMessage (CTestObj *test, LPCTSTR message);
	void DebugMessage (CTestObj *test, LPCTSTR message);
	void StatisticsMessage (CTestObj *test, LPCTSTR key, LPCTSTR message);
	void ErrorMessage (CTestObj *test, WORD errorCode, LPCTSTR message);
	void ErrorMessage (CXmtaException &x) 
		{ErrorMessage (NULL, x.GetErrorCode(), x.GetMessage ());}
	void ErrorMessage (	LPCTSTR fullName,
						LPCTSTR message,
						int moduleNumber,
						int testNumber,
						int errorCode);
};

