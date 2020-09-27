///////////////////////////////////////////////////////////////////////////////
//	CLASSVW.H
//
//	Created by :			Date :
//		JimGries			6/29/95
//
//	Description :
//		Declaration of the CComprehensiveTest and related
//

#ifndef __CLASSVW_H__
#define __CLASSVW_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CComprehensiveTest class

class CComprehensiveTest : public CTest
{
	DECLARE_TEST(CComprehensiveTest, CClassViewSubSuite)

private:
	COProject m_proj;				// Project to add file to.
	CString m_strProjBaseName;		// Just the base name of the project.
    CString m_strHFile;				// Name of header file that is a dependency of the following.
    CString m_strCPPFile;			// Name of the cpp that contains "interesting" classes.
    CString m_strBaseLineFile;		// File containing parsed information that should appear in
									// the class view.  See the file header for format details.
// Operations
public:
    virtual void PreRun(CRawListString &);
	virtual void Run(CRawListString&);
	virtual void PostRun(CRawListString &);

};

#endif //__CLASSVW_H__
