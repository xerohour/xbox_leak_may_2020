//cpodbc.h
// history:
//  basic outline taken from inside odbc cpp sample
//  modified by bartonp 4/96

#ifndef _CPPODBC_DEFS
#define _CPPODBC_DEFS
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

#define RC_SUCCESSFUL(rc) (!((rc)>>1))
#define RC_NOTSUCCESSFUL(rc) ((rc)>>1)

//  Class CODBCExec - Allows user to connect, execute, and disconnect from
//  a data source.
class CODBCExec
{
 private:
        // ODBC handles and data
  HENV      m_henv;                 // Environment handle
  HDBC      m_hdbc;                 // Connection handle
  HSTMT     m_hstmt;                // Statement handle

        // Execution settings
        //
  TCHAR      m_szDSN[SQL_MAX_DSN_LENGTH]; // Data source name
  TCHAR      m_szDBMS[128];  // Database name

        // Private access member functions
        //
  void GatherConnectInfo(void);
  void vEnumerateErrors(HENV henv,      // Environment handle
                        HDBC hdbc,      // Connection handle
                        HSTMT hstmt,    // Statement handle
                        LPSTR pszState = NULL);  


 public:
        // Construction/destruction
        //
  CODBCExec();
  ~CODBCExec();

        // ODBC member functions
        //
  BOOL fConnect(LPCSTR pszConnStr, 
	            UWORD SQLAsyncEnable = SQL_ASYNC_ENABLE_OFF);
  BOOL fExecSql(LPCSTR pszSqlStr);
  void vDisconnect(void);

        // Access member functions
        //
  inline LPCSTR GetDSNName(void);
  inline LPCSTR GetDBMSName(void);

  HDBC hdbcGetHDBC(void) { return m_hdbc;} 
  HENV henvGetHENV(void) { return m_henv;} 

  void vDumpResults(void);

        // Status member functions
        //
  inline BOOL IsConnected(void);
  void vShowAllErrors(void);

};

//
// Access member functions
//

inline LPCSTR CODBCExec::GetDSNName(void)
{
  return m_szDSN;
}

inline LPCSTR CODBCExec::GetDBMSName(void)
{
  return m_szDBMS;
}

//
// Status member functions
//

inline BOOL CODBCExec::IsConnected(void)
{
  return (NULL != m_hdbc);
}

#endif

