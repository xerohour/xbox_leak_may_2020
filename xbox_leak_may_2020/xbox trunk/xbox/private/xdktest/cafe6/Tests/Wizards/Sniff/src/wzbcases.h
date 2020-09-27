///////////////////////////////////////////////////////////////////////////////
//	wzbcases.H
//
//	Created by :			Date :
//		Anita George		9/3/96		copied from clscases.h & modified
//
//	Description :
//		Declaration of the CWzbTestCases class
//

#ifndef __wzbcases_H__
#define __wzbcases_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AppTestCases class

class CWzbTestCases : public CTest
{
	DECLARE_TEST(CWzbTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
public:
	// Data members
	CString szDir ; 
	CString ProjName ;
	CString ProjDir ;
	CString CurrDir ;// ('\0',256) ;
	CString szWBproj ;
	CString ClassName;
	CString CodeString;

protected:
	CString m_strCWD;		// the test's current working directory
 	UIProjectWizard m_prjwiz;

// Test Cases
protected:
	BOOL CreateProject();
	BOOL ShowWizBar(void);
};

#endif //__wzbcases_H__
