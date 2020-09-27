///////////////////////////////////////////////////////////////////////////////
//	testxcpt.cpp
//
//	Created by :			Date :
//		BrianCr				11/29/94
//
//	Description :
//		Implementation of the CTestException class
//

#include "stdafx.h"
// #include "test.h"
#include "testxcpt.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma message("Working around Olympus bugs 3244 and 3245.")

/*
IMPLEMENT_DYNAMIC(CTestException, CException)
*/

CTestException::CTestException(CString strMsg /*= ""*/, CTestException::TestExceptionCause cause /*= CTestException::causeUnknown*/, LPCSTR lpszFileName /*= NULL*/, int nLine /*= -1*/)
: m_strMsg(strMsg),
  m_cause(cause),
  m_file(lpszFileName),
  m_line(nLine)
{
}

//////////////////////////////////////////////////////////////////////////////
//	Debug CTestException functions

#ifdef _DEBUG
// character strings to use for dumping CTestException
static char BASED_CODE szUnknown[] = "unknown";
static char BASED_CODE szGeneric[] = "generic";
static char BASED_CODE szOperationFail[] = "operation failed";
static char BASED_CODE szAssumptionFail[] = "assumption failed";
static char BASED_CODE szExpectFail[] = "expectation failed";
static char BASED_CODE szSafetyFail[] = "safety failed";
static char BASED_CODE szTargetGone[] = "target gone";
static char BASED_CODE szTargetAssert[] = "target assert";
static char BASED_CODE szTargetGPF[] = "target crash";

static LPCSTR BASED_CODE rgszCTestExceptionCause[CTestException::NUM_CAUSES] =
{
	szUnknown,
	szGeneric,
	szOperationFail,
	szAssumptionFail,
	szExpectFail,
	szSafetyFail,
	szTargetGone,
	szTargetAssert,
	szTargetGPF
};
#endif //_DEBUG

/*
#ifdef _DEBUG
void CTestException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "msg = " << m_strMsg << "; cause = " << rgszCTestExceptionCause[m_cause];
}
#endif //_DEBUG
*/

//////////////////////////////////////////////////////////////////////////////
//	Other CTestException functions
