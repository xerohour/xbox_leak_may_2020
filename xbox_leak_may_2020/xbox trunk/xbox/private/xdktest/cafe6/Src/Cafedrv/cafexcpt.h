///////////////////////////////////////////////////////////////////////////////
//	cafexcpt.h
//
//	Created by :			Date :
//		BrianCr				11/29/94
//
//	Description :
//		Declaration of the CCAFEException class
//

#ifndef __CAFEXCPT_H__
#define __CAFEXCPT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CTestException class

class CCAFEException: public CException
{
	DECLARE_DYNAMIC(CCAFEException)

public:
	enum CAFEExceptionCause {
		causeUnknown,			// invoker didn't specify a cause
		causeGeneric,			// no current use
		NUM_CAUSES
	};

// Constructor
	CCAFEException(CString strMsg = "", CAFEExceptionCause cause = CCAFEException::causeUnknown, LPCSTR lpszFileName = NULL, int nLine = -1);

// operations
public:
	CString GetMessage(void)				{ return m_strMsg; }
	CAFEExceptionCause GetCause(void)		{ return m_cause; }

// data
protected:
	CString m_strMsg;
	CAFEExceptionCause m_cause;
	LPCSTR m_file;
	int m_line;

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif //__CAFEXCPT_H__
