/*
**
** File name: ODBCThread.cpp
** 
*/

#include "stdafx.h"
#include "ODBCThrd.h"
#include "SPDLG.h"


void ExecuteSQLSP(LPVOID pParam)
{
 	CODBCThread* pODBCThreadInfo = (CODBCThread*)pParam;

	TRY
	{
// 		RETCODE retcode;
//		retcode = SQLExecDirect(pODBCThreadInfo->m_pRecSet->m_hstmt, spchar, 12);

//		pODBCThreadInfo->m_pRecSet->m_strSort = _T("colRecID");
//		pODBCThreadInfo->m_pRecSet->m_strFilter = _T("");
		 if(pODBCThreadInfo->m_pDatasource->IsOpen()==0)
		 {
			 CString dsnConnect=pODBCThreadInfo->m_Connect;
			 pODBCThreadInfo->m_pDatasource->Open( NULL, FALSE, FALSE, dsnConnect);
		 }


		//  the following is just a test
		/*
		Get Field value
		*/
//		CDBVariant var;
//		pODBCThreadInfo->m_pRecSet->GetFieldValue(_T("colRecID"),var);

		/* 
		Get Field Info
		*/

//		CODBCFieldInfo FieldInfo;
//		pODBCThreadInfo->m_pRecSet->GetODBCFieldInfo(_T("colRecID"),FieldInfo);	

		TRACE("Before %s\n", LPCSTR(pODBCThreadInfo->m_pSPName));
     	pODBCThreadInfo->m_pDatasource->ExecuteSQL(pODBCThreadInfo->m_pSPName);
		TRACE("After  %s\n", LPCSTR(pODBCThreadInfo->m_pSPName));
	}
	CATCH(CDBException, e)
	{
		// The error code is in e->m_nRetCode
		TRACE("Error in %s: %s\n", LPCSTR(pODBCThreadInfo->m_pSPName),
			LPCSTR(e->m_strError));
	}
	END_CATCH
  	

}

UINT ODBCThreadProc(LPVOID pParam)
{
//	DWORD dwExitCode;
	long proccount;

	CODBCThread* pODBCThreadInfo = (CODBCThread*)pParam;
#pragma warning (disable: 4390)
	if (WaitForSingleObject(pODBCThreadInfo->m_hSemaphoreExecDone, /*INFINITE*/0))
		; //i hope here nothing bad hapens (always returns)
	while (TRUE)
	{
//		bRecalcCompleted = FALSE;

		// Wait for signal to start executing SPs
		if (WaitForSingleObject(pODBCThreadInfo->m_hEventStartExecute, INFINITE)
			!= WAIT_OBJECT_0)
			break;
		//check the semaphore count by calling Release & WaitForSingleObject
		// the first thread shd lock the exit by resetting the event
		ReleaseSemaphore(pODBCThreadInfo->m_hEventStartExecute,1,&proccount);
		WaitForSingleObject(pODBCThreadInfo->m_hEventStartExecute, INFINITE);
		// if(UINT(proccount+1)==pODBCThreadInfo->m_Threads) // I'm the first thread in this group entering the execution
		// always first - only one thread per connection
			ResetEvent(pODBCThreadInfo->m_hEventAllThreadExecuted);
		// the last thread shd unlock the exit by seting the event
		if(proccount==0) // I'm the last thread in this group entering the execution
			SetEvent(pODBCThreadInfo->m_hEventAllThreadExecuted);

		// Exit the thread if the main application sets the "Kille execute"
		// event. The main application will set the "start execute" event
		// before setting the "kill execute" event.

		if (WaitForSingleObject(pODBCThreadInfo->m_hEventKillExecuteThread, 0)
			== WAIT_OBJECT_0)
			break; // Terminate this thread by existing the proc.


		// Reset event to indicate "not done", that is, recalculation is in progress.
//		ResetEvent(pODBCThreadInfo->m_hEventExecuteDone);

		ExecuteSQLSP(pParam);

		::PostMessage(pODBCThreadInfo->m_hwndNotifyStatus,
			WM_USER_NOTIFY_STATUS, 0, 0);
  
		// wait here (ssynchronize) all threads in a group
		if (WaitForSingleObject(pODBCThreadInfo->m_hEventAllThreadExecuted, INFINITE)
			!= WAIT_OBJECT_0)
			break; 

	}

	ReleaseSemaphore(pODBCThreadInfo->m_hSemaphoreExecDone,1,&proccount);
	// if(UINT(proccount+1)==pODBCThreadInfo->m_Threads) // I'm the last thread in this group
	// always the last - always one thread per connection
		SetEvent(pODBCThreadInfo->m_hEventAllThreadKilled);

	return 0;
#pragma warning (default: 4390)
}


CODBCThread::CODBCThread()
{
	m_ThreadsExist = FALSE;
	m_pRecSet=NULL;
	m_pDatasource=NULL;

}



void CODBCThread::CreateThreadEvents()
{
	/*
	Create necessary event objects
	*/
 	m_hEventStartExecute = CreateSemaphore(NULL, 0, 2 /* no - only one thread per connection m_Threads+1  */, NULL);		
	// manual reset to allow multiple threads, initially reset
	m_hSemaphoreExecDone = CreateSemaphore(NULL, 1 /* m_Threads no - only one thread per connection*/, m_Threads, NULL);
	//  initially set to number of threads
	m_hEventKillExecuteThread = CreateEvent(NULL, FALSE, FALSE, NULL);	// auto reset, initially reset
	m_hEventAllThreadKilled = CreateEvent(NULL, FALSE, FALSE, NULL);// auto reset, initially reset
	m_hEventAllThreadExecuted=CreateEvent(NULL, TRUE, FALSE, NULL);
	// manual reset, to allow multiple thread to pass

	ASSERT(m_hEventStartExecute != NULL);
	ASSERT(m_hSemaphoreExecDone != NULL);
	ASSERT(m_hEventKillExecuteThread != NULL);
	ASSERT(m_hEventAllThreadKilled != NULL);
	ASSERT(m_hEventAllThreadExecuted != NULL);
}


CODBCThread::~CODBCThread()
{

//	DWORD dwExitCode;

//TO DO (suspicious - must check if it's set)
//	if (m_pWorkerThread != NULL &&
//	GetExitCodeThread(m_pWorkerThread->m_hThread, &dwExitCode) &&
//	dwExitCode == STILL_ACTIVE)
	if(m_ThreadsExist)
	{
		long number;
		SetEvent(m_hEventKillExecuteThread);
		// do{ //no: only one thread per connection
		ReleaseSemaphore(m_hEventStartExecute,1,&number);
		// }while(number < long(m_Threads));
		CloseThreadEvents();
		m_pDatasource->Close();

		WaitForSingleObject(m_hEventAllThreadKilled, INFINITE);
	}

	if(m_pRecSet!=NULL)
		delete m_pRecSet;
	if(m_pDatasource!=NULL)
		delete m_pDatasource;

//	m_pWorkerThread = NULL;

}

BOOL CODBCThread::ConnectDatasource()
{
	m_pDatasource = new CDatabase;

	m_pDatasource->SetLoginTimeout(30);
	m_pDatasource->SetQueryTimeout(1000);
	//m_pDatasource->SetSynchronousMode(TRUE);

	if (!m_pDatasource->Open(NULL)) {
		delete m_pDatasource;
		m_pDatasource=NULL;
		return FALSE;
	}

	m_pRecSet = new CRecordset(m_pDatasource);
	RetrieveConnect();
	
	return TRUE;
}

void CODBCThread::RetrieveConnect()
{	//retrieves the important part of the connsction string from m_pDatasource
	char fullconnect[1024];
	strncpy(fullconnect,m_pDatasource->GetConnect(),1024);
	CString connectbuf;

	char *token;
	token = strtok(fullconnect,";");
	for(;token != NULL;token = strtok( NULL, ";" )) 
	{
		if(strstr(token,"APP=")==token)
			continue; //skip APP
		if(strstr(token,"WSID=")==token)
			continue; //skip WSID
		if(strstr(token,"LANGUAGE=")==token)
			continue; //skip WSID
		connectbuf+=token;
		connectbuf+=';';
	}
	m_Connect=connectbuf.Left(connectbuf.GetLength()-1);
}



BOOL CODBCThread::GetNewSP()
{
	CNewSPDlg dlg;

	if (dlg.DoModal() == IDOK) 
	{
		m_pSPName = dlg.m_SPName;
		m_Threads = dlg.m_NoThreads;
		m_msecs   = dlg.m_Delay;
		return TRUE;
	} 
	else 
	{
		return FALSE;
	}
}


BOOL CODBCThread::CreateThreads()
{
	// The events are initially set or reset in the CreateEvent call;
	// but they may be left in an improperly initialized state if
	// a worker thread has been previously started and then prematurely
	// killed.  Set/reset the events to the proper initial state.
	// Set the "start recalc" event last, since it is the event the
	// triggers the starting of the worker thread recalculation.

	if(	m_ThreadsExist == FALSE)
	{
		CreateThreadEvents();
		ResetEvent(m_hEventKillExecuteThread);
		ResetEvent(m_hEventAllThreadKilled);
		ResetEvent(m_hEventAllThreadExecuted);

	/*	m_pWorkerThread = */
		// UINT i;
		// for(i=0;i<m_Threads;i++)
		//only one thread per connection
		AfxBeginThread(ODBCThreadProc, this);
		m_ThreadsExist = TRUE;
		// RecalcDone() will be called by the view when the thread sends a
		// WM_USER_RECALC_DONE message.
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CODBCThread::ActivateThread()
{
	long number,active;
//	ReleaseSemaphore(m_hEventStartExecute,0,&active); value 0 does not work
//	if(active=!0)
//		TRACE("Didn't start all threads, %d are still waiting...",active);
	if(m_ThreadsExist == FALSE)
	{
		TRACE("Thread %s: not created\n",m_pSPName);
		return FALSE;
	}
	active=ReleaseSemaphore(m_hEventStartExecute,1 /* m_Threads (no, only one thread per connection*/ ,&number);
	return TRUE;
	//SetEvent(m_hEventStartExecute);
}

BOOL CODBCThread::KillThread()
{
	long number;
	if(m_ThreadsExist == FALSE)
	{
		TRACE("Thread %s: nothing to kill\n",m_pSPName);
		return FALSE;
	}
	SetEvent(m_hEventKillExecuteThread);
	// do{ TODO only one thread
		ReleaseSemaphore(m_hEventStartExecute,1,&number);
	// }while(number < long(m_Threads));
//	SetEvent(m_hEventStartExecute);
	
	WaitForSingleObject(m_hEventAllThreadKilled, INFINITE);

	CloseThreadEvents();
	m_ThreadsExist = FALSE;
	return TRUE;
}

void CODBCThread::CloseThreadEvents()
{
 	CloseHandle(m_hEventStartExecute);
	CloseHandle(m_hSemaphoreExecDone);
	CloseHandle(m_hEventKillExecuteThread);
	CloseHandle(m_hEventAllThreadKilled);
	CloseHandle(m_hEventAllThreadExecuted);
}
