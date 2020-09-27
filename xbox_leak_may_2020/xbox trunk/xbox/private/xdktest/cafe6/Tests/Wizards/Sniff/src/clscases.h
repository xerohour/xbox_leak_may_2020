///////////////////////////////////////////////////////////////////////////////
//	CLSCASES.H
//
//	Created by :			Date :
//		DavidGa					10/27/93
//
//	Description :
//		Declaration of the CClsTestCases class
//

#ifndef __CLSCASES_H__
#define __CLSCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AppTestCases class

class CClsTestCases : public CTest
{
	DECLARE_TEST(CClsTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
public:
	// Data members
	CString  szDir ; 
	CString szDirRes ;
	CString ProjName ;
	CString ProjDir ;
	CString CurrDir ;// ('\0',256) ;
	CString strAWBase ;
	CString szAWproj ;

protected:
	CString m_strCWD;		// the test's current working directory

// Test Cases
protected:
	void OpenProject(void);
	void WizardOnProject(void);
	void TabThroughPages(void);
	void AddClassesAndMessages(void) ;
};

#endif //__CLSCASES_H__
