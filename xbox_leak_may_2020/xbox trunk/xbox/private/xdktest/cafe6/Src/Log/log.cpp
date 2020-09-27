/////////////////////////////////////////////////////////////////////////////
// log.cpp
//
// email        date            change
// cflaat       11/01/94        created
//
// copyright 1994 Microsoft

// CLog member functions

#include "stdafx.h"
#include "afxdllx.h"
#include <tchar.h>

#define EXPORT_LOG      // Make sure implicit lib linkage doesn't occur.
#include "log.h"

#define new DEBUG_NEW

// NOTE: when you override any virtual member functions, be sure to call the base class's 
// implementation at the beginning of your override

CLog::CLog(HANDLE hOutput /* =0*/,      // output handle for tagged log entries & comments
	   BOOL bRecordComments)
// UGLY_HACK(briancr): this code is here to log info to a server about who runs CAFE
// remove it as soon as possible
: m_bBigBro(TRUE)
{
  m_acArgBuf = new char[m_cchArgBuf] ;
  m_bRecordComments = bRecordComments;

  m_cTestsFinished = 0;
  m_bTestRunning = FALSE;
  m_cSuiteFailures = 0;

  m_cTotalFailures = 0;
  m_cTotalTestFailures = 0;

  // note that while CFile stores its handle as an int, it casts it to a HANDLE and
  // uses WriteFile for output

  m_pfOutput = hOutput ? new CFile((int)hOutput) : 0;

  m_bLogDebugOutput = FALSE;

}

CLog::~CLog()
{
  if (m_pfOutput)
    delete m_pfOutput;
  if (m_acArgBuf)
    delete m_acArgBuf;
}

// BeginSubSuite
//
// Description: This function is called on starting a new subsuite.
//              The szOwner parameter is the e-mail name of the 
//              person who "owns" this subsuite.
//
// Return value: TRUE
BOOL CLog::BeginSubSuite(LPCSTR szSubSuiteName, LPCSTR szOwner/* =0 */)
{
  m_strSubSuiteName = szSubSuiteName;
  m_strSubSuiteOwner = (szOwner ? szOwner : "Nobody");

  m_cSubSuiteFailures = 0;

  return TRUE;
}

// EndSubSuite
//
// Description: This function is called on ending a subsuite.
//
// Return value: TRUE
BOOL CLog::EndSubSuite(void)
{
#ifdef BIG_BRO
	// UGLY_HACK(briancr): this code is here to log info to a server about who runs CAFE
	// remove it as soon as possible
	if (m_bBigBro) {
		// log information to a server, ala Big Brother
		// Just use a hard-coded file name as this is completely internal.
		static TCHAR szLogFileName[] = _T("\\\\dtqalogs\\db\\cafe\\bigbro.dat");
		const int maxRetries = 2;
		HANDLE hFile;

		for (int i = 0; i < maxRetries ; i++ ) {
			if ((hFile = CreateFile(szLogFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL)) != INVALID_HANDLE_VALUE) {
				break;
			}
			Sleep(300); // wait 0.3 seconds if it failed.
		}
		if ( i >= maxRetries )
			return TRUE; // No luck couldn't open the file.

		// Start writing at the end of the file.
		SetFilePointer(hFile, 0, NULL, FILE_END);

		// Write out the relevant information to the file in the following format:
		// machine name, date, time, subsuite, fail count
		const DWORD MAX_SIZE = 1024;
		TCHAR buffer[MAX_SIZE]; // Buffer to hold intermediate values.
		TCHAR logString[MAX_SIZE]; // Complete string.
		DWORD cBytesWritten;

		_tcscpy(logString, _T("\0"));

		// get computer name
		BOOL bMsg = FALSE;
		GetComputerName(buffer, (LPDWORD)&MAX_SIZE);
		if (_tcsstr(buffer, _T("BRIANCR")) != NULL) {
			bMsg = TRUE;
		}
		_tcscat(logString, buffer);
		_tcscat(logString, _T(","));

		// get the date
		_tstrdate(buffer);
		_tcscat(logString, buffer);
		_tcscat(logString, _T(","));

		// get the time
		_tstrtime(buffer);
		_tcscat(logString, buffer);
		_tcscat(logString, _T(","));

		// get the subsuite name
		_tcscat(logString, m_strSubSuiteName);
		_tcscat(logString, _T(","));

		// get the fail count
		_itot(m_cSubSuiteFailures, buffer, 10);
		_tcscat(logString, buffer);

		if (bMsg) {
			_tcscat(logString, _T(" <-- Great job Brian!!"));
		}

		_tcscat(logString, "\r\n");

		// write the line to the file
		WriteFile(hFile, logString, _tcslen(logString), &cBytesWritten, NULL);

		// close the file.
		CloseHandle(hFile);
	}
#endif

	return TRUE;
}

// BeginTest
//
// Description: This function is called on starting a new test.
//
// Return value: TRUE
BOOL CLog::BeginTest(LPCSTR szTestName, long cCompares /*= -1*/)
{
  ASSERT(!m_bTestRunning);

  if (m_bTestRunning)
    return FALSE;

  m_bTestRunning = TRUE;
  
  m_cTestSuccesses = 0;
  m_cTestFailures = 0;

  m_strTestName = szTestName;

  m_cExpectedCompares = cCompares;

  return TRUE;
}


BOOL CLog::EndTest(void)                     // this is called by derived classes
{
  // the test must be running in order to end it
  ASSERT(m_bTestRunning);
  if (!m_bTestRunning)
    return FALSE;

  // do we check the expected comparison count? (-1 is a special value that indicates ignore the comparison count)
  if (m_cExpectedCompares != -1) {
	int cActualCompares = m_cTestSuccesses + m_cTestFailures;
	// did we log more comparisons than expected?
	if (cActualCompares > m_cExpectedCompares) {
	  RecordInfo("The number of comparisons made during this test (%d) is greater than the expected number of comparisons (%d).", cActualCompares, m_cExpectedCompares);
	  RecordInfo("If no failures were reported, the test passed. Please contact the sniff owner to fix this test problem.");
	}
	// did we log fewer comparisons than expected?
	else if (cActualCompares < m_cExpectedCompares) {
      RecordFailure("This test did not fully complete because the number of successes + failures reported is less than expected.");
	  RecordInfo("The number of comparisons made during this test (%d) is less than the expected number of comparisons (%d).", cActualCompares, m_cExpectedCompares);
	}
  }

  // increment the count of finished tests
  m_cTestsFinished++;

  // if there were any failures during this test, increment the count of suite and subsuite failures
  if (m_cTestFailures) {
    m_cSuiteFailures++;
    m_cSubSuiteFailures++;
  }

  // keep track of the total number of test failures
  m_cTotalTestFailures += m_cTestFailures;

  // no test is running, now
  m_bTestRunning = FALSE;

  return TRUE;
}

BOOL CLog::RecordSuccess(PRINTF_ARGS)            // product passed a test
{
	m_cTestSuccesses++;

	return TRUE;
}

BOOL CLog::RecordFailure(PRINTF_ARGS)            // product failed a test
{
  // keep track of the total number of failures
  m_cTotalFailures++;

  // is a test running?
  if (m_bTestRunning) {
    m_cTestFailures++;
  }

  return TRUE;
}


CString CLog::GetOSDescription(void)  // e.g. "Windows NT 3.5, build 807"
{
  static LPCSTR szErr = "[Error calling GetVersionEx()]";

  OSVERSIONINFO osvi;

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  // required before calling GetVersionEx()

  if (!GetVersionEx(&osvi))
    return szErr;

  CString strD;

  switch (osvi.dwPlatformId)  // find the name of the OS
  {
    case VER_PLATFORM_WIN32_NT:
    {
      strD = "Windows NT";
      break;
    }
    case VER_PLATFORM_WIN32_WINDOWS:
    {
      strD = "Windows ";
	  if(osvi.dwMinorVersion==0)
		strD+="95";
	  else
		strD+="98";	//currently, we run on Win95,Win98 only (add new WIN32_WINDOWS versions here, if applicable)
		osvi.dwBuildNumber&=0x0000FFFF; //clean the HIWORD which duplicates the Version number (no idea why GetVersionEx API was designed so bogusly)
      break;
    }
    case VER_PLATFORM_WIN32s:
    {
      strD = "Win32s";
      break;
    }
    default:
    {
      return szErr;
    }
  }

  strD += ' ';  // add a space after the name

  CString strV;

  strV.Format("version %u.%u, build %u. %s", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.szCSDVersion);

  return strD + strV;
}

BOOL CLog::SetLogDebugOutput(BOOL bValue)
{
	m_bLogDebugOutput = bValue;

	return !bValue;
}

BOOL CLog::GetLogDebugOutput(void)
{
	return m_bLogDebugOutput;
}

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Log DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Log Terminating!\n"); 
	
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}
