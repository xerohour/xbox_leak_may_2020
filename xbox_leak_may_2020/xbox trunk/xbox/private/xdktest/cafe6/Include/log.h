/////////////////////////////////////////////////////////////////////////////
// log.h
//
// email        date            change
// cflaat       11/01/94        created
//
// copyright 1994 Microsoft

// CLog interface

#ifndef __LOG_H
#define __LOG_H

#pragma warning(disable: 4251)

#ifndef EXPORT_LOG
	#ifdef _DEBUG
		#pragma comment(lib, "logd.lib")
	#else
		#pragma comment(lib, "log.lib")
	#endif
#endif

/*
    CLog
    ====

    This file and design copyright 1993-94 by Microsoft Corp.
    Created by CFlaat, based on the CLogRecorder class by RickKr.
    
      This class represents an abstract interface for a self-contained
    logging system.  A specific implementation of this interface is
    required to actually do anything.  [TODO(CFlaat): more discussion]

*/

#ifndef PRINTF_ARGS

// this is for declaring a printf-style argument list

#define PRINTF_ARGS LPCSTR a_szFormat,...

// this macro can only be used by members of CLog-derived classes which use PRINTF_ARGS

#define ParseArguments()\
{\
	va_list marker;\
	va_start( marker, a_szFormat );\
	_vsnprintf( m_acArgBuf, m_cchArgBuf, a_szFormat, marker );\
	va_end( marker );\
}
#else

#pragma message("multiple definitions of PRINTF_ARGS!!!")
#endif



class AFX_EXT_CLASS CLog
{
public:

  CLog(HANDLE hOutput = 0,      // output handle for tagged log entries & comments
       BOOL bRecordComments = FALSE);

  virtual ~CLog(void);

  virtual BOOL BeginSubSuite(LPCSTR, LPCSTR = 0);
  virtual BOOL EndSubSuite(void);

  virtual BOOL BeginTest(LPCSTR szTestName, long cCompares = -1);
  virtual BOOL EndTest(void);                         // this is called after each test case

// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordSuccess(LPCSTR a_szFormat, ...)
// Description: Record a success in the log for the current test. A call to this function causes the current test to log a success and increments the success count by 1.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL RecordSuccess(PRINTF_ARGS);            // product passed a test
// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordFailure(LPCSTR a_szFormat, ...)
// Description: Record a failure in the log for the current test. A call to this function causes the current test to fail and increments the fail count by 1.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL RecordFailure(PRINTF_ARGS);            // product failed a test
// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordInfo(LPCSTR a_szFormat, ...)
// Description: Record an informational message in the log for the current test.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL RecordTestKeys(PRINTF_ARGS) = 0;                       // records test keystrokes.
// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordTestKeys(LPCSTR szFile, LPCSTR a_szFormat, ...)
// Description: Record the contents of a text file in the log for the current test.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFile A pointer to a string that contains the name of the file to include in the log.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
												 
  virtual BOOL RecordInfo(PRINTF_ARGS) = 0;                           // recorded but ignored
// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordTextFile(LPCSTR szFile, LPCSTR a_szFormat, ...)
// Description: Record the contents of a text file in the log for the current test.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: szFile A pointer to a string that contains the name of the file to include in the log.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL RecordTextFile(LPCSTR szFile, PRINTF_ARGS) = 0;  // append a text file to the log
// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordCriticalError(LPCSTR a_szFormat, ...)
// Description: Record a critical error in the log for the current test. Use this function when a unrecoverable error occurs while testing. A call to this function causes the current test to fail and increments the fail count by 1.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL RecordCriticalError(PRINTF_ARGS) = 0;  // called for errors that force test termination

// BEGIN_HELP_COMMENT
// Function: BOOL CLog::RecordCompare(BOOL bSuccess, LPCSTR a_szFormat, ...)
// Description: Record information or a failure in the log for the current test.
// Return: A Boolean value that indicates the result of bSuccess.
// Param: bSuccess A Boolean value that specifies whether to record information (TRUE) or a failure (FALSE).
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
	virtual BOOL RecordCompare(BOOL bSuccess, PRINTF_ARGS) = 0;             // called to record based on bSuccess

// BEGIN_HELP_COMMENT
// Function: BOOL CLog::Comment(LPCSTR a_szFormat, ...)
// Description: Record a comment in the log for the current test.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: a_szFormat A pointer to a string that contains the format-specifiers for the remaining variable number of arguments. This format-specifier string is the same format as the format-specifier string for printf.
// Param: ... A variable number of arguments that are expanded into a string as specified by the format-specifier string.
// END_HELP_COMMENT
  virtual BOOL Comment(PRINTF_ARGS) = 0;              // chatty trace commentary that can be ignored
						      // or dumped at the CAFE driver's discretion

  virtual long GetSuiteFailCount(void)               // current failures in this suite
    { return m_cSuiteFailures; }

  virtual long GetSubSuiteFailCount(void)            // current failures in this subsuite
    { return m_cSubSuiteFailures; }

  virtual long GetTestSuccessCount(void)             // current successes in this test,
    { return m_cTestSuccesses; }                      // i.e. # times RecordSuccess called

  virtual long GetTestFailCount(void)                // current failures in this test,
    { return m_cTestFailures; }                      // i.e. # times RecordFailure called
  
  virtual long GetTotalFailCount(void)                           // Total # of failures in this suite run.
    { return m_cTotalFailures; }
  
  const CString& GetTestName()                                           // Current running test.
	{ return m_strTestName; }


  BOOL GetLogDebugOutput(void);         // Should debug output from target be logged?
  BOOL SetLogDebugOutput(BOOL);         // Set state of logging target debug output.

protected:

  BOOL m_bTestRunning;

  long     // type is long in case we want to utilize negative values
    m_cTestsFinished,
    m_cSuiteFailures,
    m_cSubSuiteFailures,
    m_cTestFailures,
	m_cTestSuccesses,
	m_cTotalFailures,                                // a count of the total number of failures (number of times RecordFailure is called)
	m_cTotalTestFailures,                            // a count of the total number of failures during a test (number of times RecordFailure is called while a test is running)
	m_cExpectedCompares;                             // a count of the expected number of successes+failures for each test

  CString
    m_strSubSuiteName,
    m_strSubSuiteOwner,
    m_strTestName;

  BOOL m_bRecordComments;
  BOOL m_bLogDebugOutput;
  CFile *m_pfOutput;

// these next two are for handling printf-style arguments in conjunction with the PARSE_ARGS macro

  enum { m_cchArgBuf = 4000 };
  char * m_acArgBuf; // [m_cchArgBuf];

protected:
  CString GetOSDescription(void); // e.g. "Windows NT 3.5, build 807"

	// UGLY_HACK(briancr): this code is here to log info to a server about who runs CAFE
	// remove it as soon as possible
public:
	BOOL m_bBigBro;
};

#endif  // __LOG_H

