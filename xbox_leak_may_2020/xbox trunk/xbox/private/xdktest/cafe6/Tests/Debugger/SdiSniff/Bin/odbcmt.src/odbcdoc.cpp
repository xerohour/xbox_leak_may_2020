// ODBCMTDoc.cpp : implementation of the CODBCMTDoc class
//

#include "stdafx.h"

#include "ODBCThrd.h"
#include "ODBCMT.h"

#include "ODBCDoc.h"
#include "EditDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CODBCMTDoc

IMPLEMENT_DYNCREATE(CODBCMTDoc, CDocument)

BEGIN_MESSAGE_MAP(CODBCMTDoc, CDocument)
	//{{AFX_MSG_MAP(CODBCMTDoc)
	ON_COMMAND(ID_CONNECT_ADD, OnCreateConnection)
	ON_COMMAND(ID_CONNECT_EDIT, OnEditConnect)
	ON_COMMAND(ID_CONNECT_RECONNECT, OnConnectConnect)
	ON_COMMAND(ID_CONNECT_DISCONNECT, OnConnectDisconnect)
	ON_COMMAND(ID_CONNECT_CLEAR, OnConnectKillAll)
	ON_COMMAND(ID_CREATE_THREAD, OnCreateThreads)
	ON_COMMAND(ID_EXECUTE_ALL, OnExecuteAll)
	ON_COMMAND(ID_THREAD_KILL_ALL, OnThreadKillAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODBCMTDoc construction/destruction

CODBCMTDoc::CODBCMTDoc()
{
	// TODO: add one-time construction code here
	InitializeCriticalSection(&m_critsecRunning);
	m_iCountConect = 0;
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountRunning = 0,0); //running
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountWaiting = 0,1); //waiting
	m_sizeDSN=CSize(200,200);
}

CODBCMTDoc::~CODBCMTDoc()
{
	CODBCThread* pTempThreadPTR;

	while (m_iCountConect--) {
		pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(m_iCountConect);

		pTempThreadPTR->KillThread();

		m_ThreadArray.RemoveAt(m_iCountConect);
		delete pTempThreadPTR;
	}
	DeleteCriticalSection(&m_critsecRunning);
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountRunning = 0,0); //running
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountWaiting = 0,1); //waiting
}

BOOL CODBCMTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CODBCMTDoc serialization

void CODBCMTDoc::Serialize(CArchive& ar)
{
	char buffer[12]; //for storing the number
	if (ar.IsStoring())
	{
//		ar << m_iCountConect;
//		ar << m_sizeDSN;
		for(int i=0;i<m_iCountConect;i++)
		{
			CODBCThread *pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
			
			ar.WriteString(pTempThreadPTR->m_pSPName);ar.WriteString("\n");
//			ar << pTempThreadPTR->m_Threads;
			sprintf(buffer,"  %-6d",pTempThreadPTR->m_msecs);
			ar.WriteString(buffer);
			ar.WriteString(pTempThreadPTR->m_Connect);ar.WriteString("\n");
		}
	} //storing
	else
	{
//		ar >> m_iCountConect;
//		ar >> m_sizeDSN;
		m_iCountConect=0;
		m_sizeDSN.cx=200;m_sizeDSN.cy=200; //arbitrary, event. calculate later
		CString strProc;
		CString strConnect;
		do
		{
			if(ar.ReadString(strProc)==FALSE ||
				ar.ReadString(strConnect)==FALSE)
				break;
			CODBCThread *pthreadnext=new CODBCThread;
			pthreadnext->m_pSPName=strProc;
			strConnect.TrimLeft();
			pthreadnext->m_Threads=1; //one by default (although stored otherwise before)
			pthreadnext->m_msecs=atoi(strcpy(buffer,strConnect.Left(6)));
			pthreadnext->m_Connect=strConnect.Right(strConnect.GetLength()-6);
			
			//create new DataSource, Recordset
			CDatabase *pdbnext= new CDatabase;
			pdbnext->SetLoginTimeout(30);
			pdbnext->SetQueryTimeout(1000);
			// pdbnext->SetConnect(dsnConnect); can't do that
			
			// replace Datasource & Recordset in new thread
			pthreadnext->m_pDatasource=pdbnext;
			pthreadnext->m_pRecSet = new CRecordset(pthreadnext->m_pDatasource);
			
			
			POSITION pos = GetFirstViewPosition();
			ASSERT(pos != NULL);
			CView* pView = GetNextView(pos);
			ASSERT(pView != NULL);
			pthreadnext->m_hwndNotifyStatus = pView->m_hWnd;  //  Set up for status report
			
			
			m_ThreadArray.Add(pthreadnext);
			m_iCountConect++;
		}while(1);

	} //reading
}

/////////////////////////////////////////////////////////////////////////////
// CODBCMTDoc diagnostics

#ifdef _DEBUG
void CODBCMTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CODBCMTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CODBCMTDoc commands

/*
** void CODBCMTDoc::OnCreateThread()
*/
void CODBCMTDoc::OnCreateConnection() 
{
	
	CODBCThread *thread=new CODBCThread;

	// Browse to user for the connection
	
	if (!thread->ConnectDatasource()) 
	{
		/*  
		To Do:  Generate an internal error condition
		Get rid of the thread and clean up the Array from the calling routine
		*/
		delete thread;
		return;  
	}
	
	// Prepare the SQL statement
	
	if(!thread->GetNewSP()) 
	{
		/*  
		To Do:  Generate an internal error condition
		Get rid of the thread and clean up the Array from the calling routine
		*/
		delete thread;
		return;
	};
	
	//create the events for thread synchronization


	POSITION pos = GetFirstViewPosition();
	ASSERT(pos != NULL);
	CView* pView = GetNextView(pos);
	ASSERT(pView != NULL);
	thread->m_hwndNotifyStatus = pView->m_hWnd;  //  Set up for status report


	m_ThreadArray.Add(thread);
	m_iCountConect++;
	
	int i;

	for(i=1;i<int(thread->m_Threads);i++)
	{	// create the rest of the threads

		CODBCThread *pthreadnext=new CODBCThread;
		// *pthreadnext=thread;
		pthreadnext->m_pSPName=thread->m_pSPName;
		pthreadnext->m_Threads=thread->m_Threads;
		pthreadnext->m_msecs=thread->m_msecs;

		Sleep(thread->m_msecs);
		
		//create new DataSource, Recordset
		CDatabase *pdbnext= new CDatabase;
		pdbnext->SetLoginTimeout(30);
		pdbnext->SetQueryTimeout(1000);
		pdbnext->Open( NULL, FALSE, FALSE, thread->m_Connect );
		
		// replace Datasource & Recordset in new thread
		pthreadnext->m_pDatasource=pdbnext;
		pthreadnext->m_pRecSet = new CRecordset(pthreadnext->m_pDatasource);
		pthreadnext->RetrieveConnect();

		POSITION pos = GetFirstViewPosition();
		ASSERT(pos != NULL);
		CView* pView = GetNextView(pos);
		ASSERT(pView != NULL);
		pthreadnext->m_hwndNotifyStatus = pView->m_hWnd;  //  Set up for status report
	
				
		m_ThreadArray.Add(pthreadnext);
		m_iCountConect++;
	}
	int length=thread->m_Connect.GetLength();
	if(length>m_sizeDSN.cx)
		m_sizeDSN.cx=length;
	UpdateAllViews(NULL);	
}


void CODBCMTDoc::OnEditConnect() 
{
	if (!m_iCountConect) return;
	CEditSPDlg dlg(this);
	if(dlg.DoModal() == IDOK)
	{
		int i;
		for(i=0;i<m_iCountConect;i++)
		{
			CODBCThread *pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
			int length=pTempThreadPTR->m_Connect.GetLength();
			if(length>m_sizeDSN.cx)
				m_sizeDSN.cx=length;
		}
		UpdateAllViews(NULL);
	}
}

void CODBCMTDoc::OnConnectConnect()
{  // reconnects all connections in thread table
		int i;
		for(i=0;i<m_iCountConect;i++)
		{
			CODBCThread *pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
			CString dsnConnect=pTempThreadPTR->m_Connect;
			if(pTempThreadPTR->m_pDatasource->Open( NULL, FALSE, FALSE, dsnConnect )==0)
				TRACE("Failed to open Datasource: %d\n",dsnConnect);
	 		Sleep(pTempThreadPTR->m_msecs);
		}
}

void CODBCMTDoc::OnConnectDisconnect()
{  // kills all threads  & disconnects all connections in thread table
	//  does not destroy the connection information
	OnThreadKillAll();
	int i;
		for(i=0;i<m_iCountConect;i++)
		{
			CODBCThread *pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
			pTempThreadPTR->m_pDatasource->Close( );
		}
}
		
void CODBCMTDoc::OnConnectKillAll()
{  // kills all threads  & disconnects all connections in thread table
	// destroy the connection information (document is clean)
	OnThreadKillAll();
	while(m_iCountConect>0)
	{
		CODBCThread *pTempThreadPTR;
		pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(--m_iCountConect);
		pTempThreadPTR->KillThread(); //waits INFINITE time
		m_ThreadArray.RemoveAt(m_iCountConect);
		delete pTempThreadPTR;
	}
}

void CODBCMTDoc::OnCreateThreads()
{	// creates all threads and leaves them in a paused statte
	// if they exist, they are stopped & paused
	CODBCThread* pTempThreadPTR;

	for (int i = 0; i < m_iCountConect; i++) {
 		pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
		pTempThreadPTR->CreateThreads();
	}
}

/*
** void CODBCMTDoc::OnExecuteAll()
*/
void CODBCMTDoc::OnExecuteAll() 
{//TODO: reconnect all threads 
//	(the best would be to reconnect inside each thread)
//  when executing all while not connected
	if (!m_iCountConect) return;
	BOOL allowrun=FALSE;
	EnterCriticalSection(&m_critsecRunning);
	if(m_iCountRunning==0)  //here allow chain
		allowrun=TRUE;
	if(allowrun==FALSE)
	{
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
			++m_iCountWaiting,1);
		TRACE("Threads scheduled to run\n");
		return;
	}
	LeaveCriticalSection(&m_critsecRunning);
	TRACE("\n\n");

	CODBCThread* pTempThreadPTR;

	for (int i = 0; i < m_iCountConect; i++) 
	{
 		pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
		if(pTempThreadPTR->m_pDatasource->IsOpen())
		{
			if(pTempThreadPTR->ActivateThread()!=FALSE)
			{
				EnterCriticalSection(&m_critsecRunning);
				m_iCountRunning++;
				LeaveCriticalSection(&m_critsecRunning);
			}
		}
		else
		{
			if(pTempThreadPTR->ActivateThread()!=FALSE)
			{
				EnterCriticalSection(&m_critsecRunning);
				m_iCountRunning++;
				LeaveCriticalSection(&m_critsecRunning);
				::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
					m_iCountRunning,0);
				Sleep(pTempThreadPTR->m_msecs);
			}
		}
	}
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountRunning,0);
}



void CODBCMTDoc::OnThreadKillAll() 
{
	CODBCThread* pTempThreadPTR;
	int i;
	for(i=0; i < m_iCountConect; i++) 
	{
		pTempThreadPTR = (CODBCThread*)m_ThreadArray.GetAt(i);
		pTempThreadPTR->KillThread(); //waits IINFINITE time
	}
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountRunning = 0,0);
}

// call when you want to reconnect & execute all threads
// without getting through UI
void CODBCMTDoc::CreateAndExecute(BOOL Assync /* =FALSE */, int NbRuns /* =1 */)
{
	if(Assync==FALSE)
		OnConnectConnect();
	OnCreateThreads();
	m_iCountWaiting=NbRuns; //executed once
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		--m_iCountWaiting,1);
	if(m_iCountWaiting<0)m_iCountWaiting=0; //safety
	OnExecuteAll();	
}



// called when the execution of threads is done
void CODBCMTDoc::ExecutionDone()
{
	BOOL nextexec=FALSE;
	// Thread ID should be passed in
	// Find the correct Arrey entry point and get the SP name
	// Display SP name 
	// If there are any results, display that as well

	EnterCriticalSection(&m_critsecRunning);
	if(--m_iCountRunning<0)m_iCountRunning=0; //safety
	if(m_iCountRunning==0 && m_iCountWaiting>0)  //here allow chain
		nextexec=TRUE;
	LeaveCriticalSection(&m_critsecRunning);
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
		m_iCountRunning,0);
	if(nextexec)
	{
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_USER_RUN_PROGRESS,
			--m_iCountWaiting,1);
		if(m_iCountWaiting<0)m_iCountWaiting=0; //safety
		OnExecuteAll();
	}
}



