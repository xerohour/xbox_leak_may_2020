// cppodbc.cpp

#include "cppodbc.h"
#include "iostream.h"  // cout in vEnumerateErrors

extern CRITICAL_SECTION csCriticalSection; 

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::CODBCExec - ctor
//
// Returns:  Nothing
/////////////////////////////////////////////////////////////////////////////
CODBCExec::
CODBCExec()
: m_henv(NULL),
m_hdbc(NULL),
m_hstmt(NULL)
{
	*m_szDSN = '\0';
	*m_szDBMS = '\0';
}

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::~CODBCExec - dtor
//
// Returns:  Nothing
/////////////////////////////////////////////////////////////////////////////
CODBCExec::
~CODBCExec()
{
	// Delete any storage we have left
	//
	
	// Disconnect if the user forgot to do so.
	//
	if(m_hdbc) vDisconnect();
}

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::GatherConnectInfo - Gather information the user may want
//
// Returns:  Nothing
/////////////////////////////////////////////////////////////////////////////

void 
CODBCExec::
GatherConnectInfo(void)
{
	RETCODE         rc;
	
	// Data source name is good for window titles and new connections
	//
	rc = SQLGetInfo(m_hdbc,
		SQL_DATA_SOURCE_NAME,
		m_szDSN,
		sizeof(m_szDSN),
		NULL);
	if(RC_NOTSUCCESSFUL(rc)) 
	{
		*m_szDSN = '\0';
		vShowAllErrors();
	}
	// DBMS names is a good source of generic capabilites
	//
	rc = SQLGetInfo(m_hdbc,
		SQL_DBMS_NAME,
		m_szDBMS,
		sizeof(m_szDBMS),
		NULL);
	if(RC_NOTSUCCESSFUL(rc))
	{
		*m_szDBMS = '\0';
		vShowAllErrors();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::fConnect - Connect to a data source using the caller's string.
//
// Returns:  TRUE if successful, FALSE on error
/////////////////////////////////////////////////////////////////////////////
BOOL 
CODBCExec::
fConnect(LPCSTR pszConnStr,                    // Input connection string
		 UWORD SQLAsyncEnable /*=SQL_ASYNC_ENABLE_OFF*/) // or ENABLE_ON
{
	RETCODE         rc;
	
	if( m_hdbc != NULL) vDisconnect();
	
	// Allocate the environment handle for this application
	//
	rc = SQLAllocEnv(&m_henv);
	if(RC_SUCCESSFUL(rc)) 
	{
		// Allocate a connection handle
		//
		rc = SQLAllocConnect(m_henv, &m_hdbc);
		if(RC_SUCCESSFUL(rc)) 
		{
			
			SQLSetConnectOption(m_hdbc, SQL_ASYNC_ENABLE, SQLAsyncEnable);
			
			// Connect to the data source of choice
			SWORD swSL = 0;
			if(pszConnStr != NULL) swSL =strlen(pszConnStr);
			else swSL = 0; 
			
			rc = SQLDriverConnect(m_hdbc, NULL,
				(UCHAR FAR *)pszConnStr, swSL, //SQL_NTS,
				NULL, 0, NULL,
				SQL_DRIVER_COMPLETE);
			if(RC_SUCCESSFUL(rc)) 
			{
				// Allocate a statement handle
				//
				rc = SQLAllocStmt(m_hdbc, &m_hstmt);
				if(RC_SUCCESSFUL(rc)) 
				{
					GatherConnectInfo();
					return TRUE;
				} // alloc stmt
			} // connect
		} // alloc hdbc
	} // alloc henv
	// An error occurred. Show the messages to the user and
	// free up any handles that were allocated.
	//
	vShowAllErrors();
	vDisconnect();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::fExecSql - Execute a SQL statement, check for return values,
//                                      then retrieve them if present.
//
// Returns:  TRUE if successful, FALSE on error
/////////////////////////////////////////////////////////////////////////////

BOOL 
CODBCExec::
fExecSql(LPCSTR pszSqlStr)  // SQL string to execute
{
	RETCODE         rc;
	BOOL            fRtn = TRUE;
	
	if(pszSqlStr == NULL) return FALSE;
	
	// Execute the statement directly
	//
	rc = SQLExecDirect(m_hstmt, (UCHAR FAR *)pszSqlStr, SQL_NTS);
	//  while(rc = SQLExecDirect(m_hstmt, (UCHAR FAR *)pszSqlStr, SQL_NTS) == SQL_STILL_EXECUTING)
	//  { Sleep(1000); }
	
	if(RC_SUCCESSFUL(rc) || rc == SQL_STILL_EXECUTING)
	{
		fRtn = TRUE;
	}
	else
	{
		vShowAllErrors();
		fRtn = FALSE;
	}
	return fRtn;
}

/////////////////////////////////////////////////////////////////////////////
// CODBCExec::vDisconnect - Disconnect from a data source.
//
// Returns:  TRUE if successful, FALSE on error
/////////////////////////////////////////////////////////////////////////////
void 
CODBCExec::
vDisconnect(void)
{
	if(m_hstmt)
		SQLFreeStmt(m_hstmt, SQL_DROP);
	if(m_hdbc) 
	{
		SQLDisconnect(m_hdbc);
		SQLFreeConnect(m_hdbc);
	}
	if(m_henv) SQLFreeEnv(m_henv);
	m_hstmt = NULL;
	m_hdbc = NULL;
	m_henv = NULL;
	*m_szDSN = '\0';
	*m_szDBMS = '\0';
}

void 
CODBCExec::
vShowAllErrors(void)
{
	vEnumerateErrors(m_henv,m_hdbc,m_hstmt);
}

/////////////////////////////////////////////////////////////////////////////
// vEnumerateErrors - Iterate all errors on valid handles
//
// Returns:  Nothing
/////////////////////////////////////////////////////////////////////////////
void 
CODBCExec::
vEnumerateErrors(HENV henv,      // Environment handle
                 HDBC hdbc,      // Connection handle
                 HSTMT hstmt,    // Statement handle
                 LPSTR pszState /* = NULL*/)  // Return of last state, NULL is valid
{
	RETCODE                 rc;
	SDWORD                  lNativeError;
	char                    szState[6];
	char                    szMsg[SQL_MAX_MESSAGE_LENGTH];
	
	EnterCriticalSection(&csCriticalSection); 
	cout << "ERROR: " << endl;
	if(m_szDSN != NULL)  cout << m_szDSN << " ";
	if(m_szDBMS != NULL) cout << m_szDBMS << endl;
	
	if(henv==NULL) 
		cout << "Environment handle == NULL, no error enumerated." << endl;

	rc = SQLError(henv, hdbc, hstmt,
		(UCHAR FAR *)szState, &lNativeError,
		(UCHAR FAR *)szMsg, sizeof(szMsg), NULL);
	
	while(RC_SUCCESSFUL(rc)) 
	{
		// Display error for user
		//
		//iMessageBox(hDlg, MB_ICONEXCLAMATION | MB_OK,
		//                                szODBC, szODBCMsg,
		//                                (LPSTR)szState, lNativeError, (LPSTR)szMsg);
		cout << szState << " " << lNativeError  << " " <<szMsg << endl;
		// Store the state if user wants it
		//
		if(pszState) lstrcpy(pszState, szState);
		
		// Go for next error
		//
		rc = SQLError(henv, hdbc, hstmt,
			(UCHAR FAR *)szState, &lNativeError,
			(UCHAR FAR *)szMsg, sizeof(szMsg), NULL);
		//    while( (rc = SQLError(henv, hdbc, hstmt,
		//                (UCHAR FAR *)szState, &lNativeError,
		//                (UCHAR FAR *)szMsg, sizeof(szMsg), NULL))== SQL_STILL_EXECUTING)
		//    {  Sleep(1000);}
		
	}
	cout << rc << endl;
	LeaveCriticalSection(&csCriticalSection); 

}



void 
CODBCExec::
vDumpResults(void)
{
	const int ci = 257;
	TCHAR    tcData[ci];
	RETCODE  rc1,rc2;
	SDWORD   cbSize;
	
	do
	{
		for(rc1=SQLFetch(m_hstmt); RC_SUCCESSFUL(rc1); rc1= SQLFetch(m_hstmt))
		{
			int i = 1;
			for(rc2 = SQLGetData(m_hstmt,i++,SQL_C_CHAR,tcData,ci - 1,&cbSize);
			RC_SUCCESSFUL(rc2);
			rc2 = SQLGetData(m_hstmt,i++,SQL_C_CHAR,tcData,ci - 1,&cbSize))
				cout << tcData <<  " ";
			cout << endl;
		}
	}
	while( RC_SUCCESSFUL(SQLMoreResults(m_hstmt))); 
}
