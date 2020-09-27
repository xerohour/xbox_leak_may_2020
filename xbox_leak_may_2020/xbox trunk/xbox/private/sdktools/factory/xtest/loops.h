// loops.h 
	 
class CHandleArray : public CArray <HANDLE, MAX_WAIT_HANDLES>
{
	DWORD Wait (bool all, DWORD millisec);
	
public:
	CHandleArray () {}
	~CHandleArray () {}

	void CloseAll (void);
	DWORD WaitForAny (DWORD millisec) {return Wait (false, millisec);}
	DWORD WaitForAll (DWORD millisec) {return Wait (true, millisec);}
};

class CThreadable
{
	HANDLE m_hThread;
	static unsigned __stdcall ThreadEntry (LPVOID pParam);

protected:
	virtual bool ThreadMain () = 0;

public:
	CThreadable ():m_hThread(NULL) {}
	HANDLE StartThread (LPCTSTR name);
};

//class CParameterItem;

class CLoopable : public CTimeable, public CThreadable
{
protected:
	CParameterTable *m_localParameters;
	CLoopable *m_objHaltOnError;

	int m_looplimit;
	int m_timelimit;
	int m_haltonerror;

	int m_debugmask;
	int m_maxthreads;
	int m_maxtimeout;
	int m_reportwarnings;
	int m_reportstatistics;
	int m_senduutstarting;

	long m_abortFlag;

public:
	CLoopable ():m_looplimit(1), m_timelimit(0), m_haltonerror(0),
		m_debugmask(0), m_maxthreads(64), m_maxtimeout(MAX_TIMEOUT),
		m_reportwarnings(1), m_reportstatistics(1), m_objHaltOnError(NULL),
		m_senduutstarting(1), m_localParameters(NULL), m_abortFlag (false) {}
	virtual ~CLoopable () {}

	void OnTimer () {OnSignalAbort ();}
	void InitLoopControl (CParameterTable *params, CLoopable *parent);

	virtual void OnSignalAbort () {InterlockedIncrement (&m_abortFlag);}
	virtual bool CheckTimeout (DWORD, DWORD) {return false;}

	virtual bool ThreadMain ();
	virtual bool LoopStart () {return true;}
	virtual bool LoopMain () = 0;
	virtual void LoopExit () {}
	virtual LPCTSTR GetName () = 0;

};

class CTestObj;

class CLoopManager : public CLoopable
{
	CHandleArray m_threadArray;
	CArray <CLoopable*, MAX_WAIT_HANDLES> m_objectArray;

	CCommandItem *m_loop;
	CLoopable *m_parent;

	long m_pollingPeriod;

	void AddThread (CLoopable *test, HANDLE thread);
	void RemoveThread (int arrayIndex);
	void CheckThreads ();

public:
	CLoopManager ();
	CLoopManager (CCommandItem *loop, CLoopable *parent = NULL);
	~CLoopManager ();

	void StartTesting (CCommandItem *loop);
	virtual bool LoopMain ();
	virtual LPCTSTR GetName () {return m_loop->GetName ();}

	void StartLoop (CCommandItem *loop);
	void RunLoop (CCommandItem *loop);
	void WaitLoop ();

	void StartTest (LPCTSTR name, CParameterTable *local);
	void RunTest (LPCTSTR name, CParameterTable *local);
	void StopThreads ();

	void OnSignalAbort () {StopThreads();}
};
