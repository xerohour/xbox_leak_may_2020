///////////////////////////////////////////////////////////////////////////////
//	cafexcpt.cpp
//
//	Created by :			Date :
//		BrianCr				11/29/94
//
//	Description :
//		Implementation of the CCAFEException class
//

#include "stdafx.h"
#include "cafexcpt.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_DYNAMIC(CCAFEException, CException)

CCAFEException::CCAFEException(CString strMsg /*= ""*/, CCAFEException::CAFEExceptionCause cause /*= CCAFEException::causeUnknown*/, LPCSTR lpszFileName /*= NULL*/, int nLine /*= -1*/)
: m_strMsg(strMsg),
  m_cause(cause),
  m_file(lpszFileName),
  m_line(nLine)
{
}

//////////////////////////////////////////////////////////////////////////////
//	Debug CCAFEException functions

#ifdef _DEBUG
// character strings to use for dumping CCAFEException
static char BASED_CODE szUnknown[] = "unknown";
static char BASED_CODE szGeneric[] = "generic";

static LPCSTR BASED_CODE rgszCCAFEExceptionCause[CCAFEException::NUM_CAUSES] =
{
	szUnknown,
	szGeneric,
};
#endif //_DEBUG

#ifdef _DEBUG
void CCAFEException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
//	AFX_DUMP0(dc, " m_cause = ");

	dc << "msg = " << m_strMsg << "; cause = " << rgszCCAFEExceptionCause[m_cause];
}
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////////////
//	Other CCAFEException functions
