// testobj.h - this include is required to build a module

#pragma once

class CTestFactory;

// CTestObj
//
// All Test objects must ultimately derive from this class.
// This object encapsulates the Test Thead.
// The XMTA API used by the tests is a subset of the 
// protected members of this class.
//
class CTestObj : public CLoopable
{
	HANDLE m_hThread;

	int m_errorCount;
	DWORD m_startTime;
	DWORD m_elapsedTime;
	DWORD m_settimeout;
	DWORD m_lastCheckAbort;
	_TCHAR m_lastCheckAbortHere [128];

	bool m_handlesExceptions;
	bool m_handlesLooping;

	ULONGLONG m_lastThreadTime;
	bool IsThreadRunning ();

	HANDLE m_semaphore;
	HANDLE m_mutexTest;

protected: // Implementation of CLoopable virtuals
	virtual bool LoopMain ();
	virtual bool LoopStart ();
	virtual void LoopExit ();
	virtual LPCTSTR GetName () {return GetFullName ();}


private: // Test Factory can create an instance of any registered test

	static CTestFactory m_factory;

public: // Access to the Test Factory

	static void RegisterTest (LPCTSTR name, CreateFunction func);
	static bool LookupTest (LPCTSTR module, LPCTSTR test); 
	static CTestObj *CreateTest (LPCTSTR name); 

protected: // XMTA API functions ...

	bool GetCfgPresent (LPCTSTR name) {return (GetCfgString (name, NULL) != NULL);}
	bool GetCfgBoolean (LPCTSTR name);
	int GetCfgInt (LPCTSTR name, int defaultInt);
	UINT GetCfgUint (LPCTSTR name, UINT defaultUint);
	LPCTSTR GetCfgString (LPCTSTR name, LPCTSTR defaultStr);
	int GetCfgChoice (LPCTSTR name, int defaultChoice, ...);

	bool ReportError (WORD errorCode, LPCTSTR format, ...);
	bool ReportError (CXmtaException &x) {return ReportError (x.GetErrorCode(), x.GetMessage ());}
	bool vReportError (WORD errorCode, LPCTSTR format, va_list argptr);

	void ReportWarning (LPCTSTR format, ...);
	void ReportDebug (DWORD debugFilter, LPCTSTR format, ...);
	void ReportStatistic (LPCTSTR key, LPCTSTR format, ...);

	void SetTimeoutLimit (int limit);

public: 
	bool CheckAbort (LPCTSTR here); // CheckAbort() needs to be accessible.
	DWORD GetElapsedTime () {return m_elapsedTime;}
	DWORD GetTestPassed () {return m_errorCount == 0;}

protected: // helper functions

	CParameterItem *FindParameter (LPCTSTR name);
	bool AbortSensitiveWait (HANDLE h, LPCTSTR footprint);

	bool LockModule (void);
	void UnlockModule (void);
	bool LockTest (void);
	void UnlockTest (void);

protected: // virtual functions implemented in derived classes

	virtual bool InitializeParameters ();
	virtual void TestMain () = 0;

public: // These virtuals are implemented by the DECLARE macros

	virtual LPCTSTR GetFullName () = 0;
	virtual LPCTSTR GetModuleName () = 0;
	virtual LPCTSTR GetTestName () = 0;
	virtual WORD GetModuleNumber () = 0;
	virtual WORD GetTestNumber () = 0;
	virtual bool WantModuleLocked () = 0;
	virtual bool WantTestLocked () = 0;

public: // These functions are used by the executive to control the test

	CTestObj ();
	virtual ~CTestObj (void);

	HANDLE StartTest ();
	void StopTest ();
	virtual bool CheckTimeout (DWORD maxTimeOut, DWORD loadFactor);

	void OnSignalAbort () {StopTest ();}

};


// CTestRegistrar
//
// This class exists only for the purpose of auto registration
// Instances of this class are declared statically in each test
// The constructor is run at start-up and register the test
// in with the test factory.  The test factory is a static member
// of the CTestObj class.
//
class CTestRegistrar
{
public:
	CTestRegistrar (LPCTSTR name, CreateFunction func) 
	{
		CTestObj::RegisterTest (name, func);
	}
};


// These are the macros simplify the constrution of tests and modules
//
#define DECLARE_XMTAMODULE(mclass, mname, mnumber) \
	virtual WORD GetModuleNumber () {return mnumber;} \
	virtual LPCTSTR GetModuleName () {return _T(mname);} \
	virtual bool WantModuleLocked ();

#define IMPLEMENT_MODULELOCKED(mclass) \
	bool mclass::WantModuleLocked () {return true;}
#define IMPLEMENT_MODULEUNLOCKED(mclass) \
	bool mclass::WantModuleLocked () {return false;}


#define REGISTER_TEST(tclass, mname, tname) \
	CTestRegistrar tclass::m_registrar \
		(mname _T(".") tname, &tclass::CreateTest)

//
#define DECLARE_XMTATEST(mclass,tclass,mname,tname,tnumber,tlocked) \
	virtual WORD GetTestNumber () {return tnumber;} \
	virtual LPCTSTR GetTestName () {return tname;} \
	virtual LPCTSTR GetFullName () {return mname _T(".") tname;} \
	virtual bool WantTestLocked () {return tlocked;} \
	static CTestRegistrar m_registrar; \
	static CTestObj *CreateTest () {return new tclass;} \
	virtual void TestMain (); 

#define IMPLEMENT_XMTATEST(mclass,tclass,mname,tname,tnumber,locked) \
	class tclass : public mclass \
	{public: DECLARE_XMTATEST (mclass,tclass,mname,tname,tnumber,locked);};\
	REGISTER_TEST (tclass, mname, tname); \
	void tclass::TestMain ()
	
#define IMPLEMENT_TESTLOCKED(m,t,n) IMPLEMENT_XMTATEST \
			(CXModule##m, CXModule##m##Test##t, _T(#m), _T(#t), n, true)

#define IMPLEMENT_TESTUNLOCKED(m,t,n) IMPLEMENT_XMTATEST \
			(CXModule##m, CXModule##m##Test##t, _T(#m), _T(#t), n, false)

#define GET_TESTOBJ_PTR ((CTestObj *) TlsGetValue (g_tlsTestObj))
	
// Use these #defines to specify the selection bits for debug messages.
//
#define BIT0  0x00000001
#define BIT1  0x00000002
#define BIT2  0x00000004
#define BIT3  0x00000008
#define BIT4  0x00000010
#define BIT5  0x00000020
#define BIT6  0x00000040
#define BIT7  0x00000080
#define BIT8  0x00000100
#define BIT9  0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000


// This macro creates a constant wide char string
// that identifies line number and file.
//
#define _QUOTE(x) L#x
#define QUOTE(x) _QUOTE(x)
#define HERE _T(__FILE__) _T("(") QUOTE(__LINE__) _T(")")
//#define HERE NULL
//#define HERE __FILE__ "(" QUOTE(__LINE__) ")"



