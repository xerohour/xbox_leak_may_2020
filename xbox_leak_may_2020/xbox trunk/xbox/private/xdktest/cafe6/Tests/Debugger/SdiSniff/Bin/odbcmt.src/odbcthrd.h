/*
**
**	File name ODBCTHread.h
**
*/

#include "afxdb.h"

#define WM_USER_NOTIFY_STATUS (WM_USER)
#define WM_USER_RUN_PROGRESS (WM_USER+1)

class CODBCThread : public CObject
{
public:
	CODBCThread();

	virtual ~CODBCThread();

public:
	HANDLE m_hEventStartExecute;
	HANDLE m_hSemaphoreExecDone;
	HANDLE m_hEventKillExecuteThread;
	HANDLE m_hEventAllThreadKilled;
	HANDLE m_hEventAllThreadExecuted;

//	CWinThread* m_pWorkerThread;
	HWND m_hwndNotifyStatus;

public:
	CString		m_pSPName;
	UINT		m_Threads;
	UINT		m_msecs;
	CDatabase	*m_pDatasource;
	CString		m_Connect;
	CRecordset	*m_pRecSet;
	BOOL		m_ThreadsExist;
public:
	BOOL ActivateThread();
	BOOL ConnectDatasource();
	void CODBCThread::RetrieveConnect();
	BOOL GetNewSP();
   	void CreateThreadEvents();
	void CloseThreadEvents();
	BOOL CreateThreads();
	BOOL KillThread();
 };

UINT ODBCThreadProc(LPVOID pParam /* CODBCThreadInfo ptr */);
