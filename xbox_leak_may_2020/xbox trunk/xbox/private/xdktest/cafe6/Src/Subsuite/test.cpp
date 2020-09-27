/////////////////////////////////////////////////////////////////////////////
// test.cpp
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Implementation of the CTest classes

#include "stdafx.h"
#include "test.h"
#include "rawstrng.h"
#include "subsuite.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
//	CTest class

IMPLEMENT_DYNAMIC(CTest, CObject);

CTest::CTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/)
: m_pSubSuite(pSubSuite),
  m_strName(szName),
  m_nExpectedCompares(nExpectedCompares),
  m_strListFilename(szListFilename),
  m_rs(CTest::RS_NoRun),
  m_pLog(NULL),
  m_pDependency(NULL)
{
  m_pPlatformList = new CPlatformList; // create it as an empty list
	// determine the type of test
	m_nType = m_strListFilename.IsEmpty() ? TEST_TYPE_STANDARD : TEST_TYPE_LIST;
}

CTest::~CTest()
{
  if (m_pPlatformList)
  {

    while (!m_pPlatformList->IsEmpty())
    {
      delete m_pPlatformList -> RemoveHead();
    }

    delete m_pPlatformList;
  }
}

///////////////////////////////////////////////////////////////////////////////
//	Operations

void CTest::AddSupportedPlatform(const CPlatform::COS &rOS, const CPlatform::CLanguage &rLang)
{
  CPlatform *pP = new CPlatform(rOS, rLang);

  m_pPlatformList->AddTail(pP);
}


void CTest::PreRun(void)
{
	m_timeStart = CTime::GetCurrentTime();

	// store the log
	m_pLog = GetSubSuite()->GetLog();

	// get the current working directory for this test and store it
	m_strCWD = GetSubSuite()->GetCWD();

	// start the test
    GetLog()->BeginTest(m_strName, m_nExpectedCompares);
}

void CTest::PreRun(CRawListString& strRaw)
{
    CTest::PreRun();

    GetLog()->RecordInfo("--------------------------------------------------------------");
    GetLog()->RecordInfo("Input for this iteration of List Based Test");
    GetLog()->RecordInfo("%s", (LPCSTR)strRaw);
    GetLog()->RecordInfo("--------------------------------------------------------------");
}

void CTest::Run(void)
{
}

void CTest::Run(CRawListString &)
{
    CTest::Run();
}

void CTest::PostRun(void)
{
	m_timeEnd = CTime::GetCurrentTime();
	CTimeSpan elapsed_time(m_timeEnd - m_timeStart);
	GetLog()->RecordInfo("Test elapsed time %s", elapsed_time.Format("%H:%M:%S"));

	// notify the log that a test ended
	GetLog()->EndTest();

}

void CTest::PostRun(CRawListString&)
{
    CTest::PostRun();
}

BOOL CTest::RunAsDependent(void)
{
	// always run this test as a dependency, if asked
	// override this function, if needed
	return TRUE;
}

CLog* CTest::GetLog(void)
{
	// the log cannot be NULL
	ASSERT(m_pLog);

	return m_pLog;
}

CSubSuite* CTest::GetSubSuite(void)
{
	// the subsuite cannot be NULL
	ASSERT(m_pSubSuite);

	return m_pSubSuite;
}

CString CTest::GetCWD(void)
{
	// the current working directory cannot be empty
	ASSERT(!m_strCWD.IsEmpty());

	return m_strCWD;
}

BOOL CTest::SetName(LPCSTR szName)
{
	// the name should not be empty
	ASSERT(szName);

	// once the name is set, it can't be set again
	// (to prevent other clients from changing the name)
	ASSERT(m_strName.IsEmpty());

	if (!m_strName.IsEmpty()) {
		return FALSE;
	}

	m_strName = szName;

	return TRUE;
}

CString CTest::GetName(void)
{
	// this can't be called unless the name has been set
	ASSERT(!m_strName.IsEmpty());

	return m_strName;
}

BOOL CTest::SetRunState(RunState rs)
{
	m_rs = rs;

	return TRUE;
}

CTest::RunState CTest::GetRunState(void)
{
	return m_rs;
}

BOOL CTest::SetListFilename(LPCSTR szListFilename)
{
	// the filename should not be empty
	ASSERT(szListFilename);

	// once the name is set, it can't be set again
	// (to prevent other clients from changing the name)
	ASSERT(m_strListFilename.IsEmpty());

	if (!m_strListFilename.IsEmpty()) {
		return FALSE;
	}

	m_strListFilename = szListFilename;

	return TRUE;
}

// BEGIN_HELP_COMMENT
// Function: CString CListTest::GetListFilename()
// Return: A CString containing the name of the file containing
//         a list of items.  The name returned is identical to what is
//         specified at list creation.
// END_HELP_COMMENT
CString CTest::GetListFilename()
{
    return m_strListFilename;
}

BOOL CTest::SetDependency(CTest* pTest)
{
	// the dependency must be valid
	ASSERT(pTest);

	m_pDependency = pTest;

	return TRUE;
}

CTest* CTest::GetDependency(void)
{
	return m_pDependency;
}

BOOL CTest::CanTestPlatform(CPlatform *pPlatform)
{
    // In this function we iterate through the list of supported platforms, and
	//   declare success if we find any match at all in the list.

#if 0
  // use this to test the enabling/disabling functionality
  CPlatform p(CPlatform::Win32Intel, CPlatform::AnyLanguage);

  return (p.Includes(*pPlatform));
#endif

// real implementation:

    POSITION pos;
  	CPlatform *pNext;

	pos = m_pPlatformList -> GetHeadPosition();

  if (!pos)  // if list is empty
    return TRUE;  // then default to running against any platform

  do
	{
      pNext = m_pPlatformList->GetNext(pos);

	    // does this platform support the platform the driver is offering?

      if (pNext->Includes(*pPlatform))  // if yes, we're all set
	    return TRUE;
	}
  while (pos);

  // if we fall through, this test cannot be run on the proposed platform

	return FALSE;
}

int CTest::GetType(void)
{
	return m_nType;
}

#if 0	// Removed for global Sleep Override...
void CTest::Sleep(DWORD msec)
{
    WaitStepInstructions("Sleep for %d msecs", msec);
    ::Sleep(msec);
}
#endif

// WriteLog is provided for backward compatibility only
// GetLog()->RecordCompare should be used instead
void CTest::WriteLog(int LT, LPCSTR szFmt, ...)
{
	char szBuf[1024+1];
	va_list marker;

	va_start( marker, szFmt );
	_vsnprintf( szBuf, 1024, szFmt, marker );
	va_end( marker );

	GetLog()->RecordCompare(LT == PASSED, szBuf);
}
