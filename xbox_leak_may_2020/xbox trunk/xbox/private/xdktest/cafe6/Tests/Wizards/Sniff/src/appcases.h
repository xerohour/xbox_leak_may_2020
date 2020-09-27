///////////////////////////////////////////////////////////////////////////////
//	APPCASES.H
//
//	Created by :			Date :
//		DavidGa					10/27/93
//
//	Description :
//		Declaration of the CAppTestCases class
//

#ifndef __APPCASES_H__
#define __APPCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AppTestCases class

class CAppTestCases : public CTest
{
	DECLARE_TEST(CAppTestCases, CSniffDriver)

// Attributes
	int iteration ;
	int LangDlls ;

public:
	// Data members
	CString  szDir ; 
	CString szDirRes ;
	CString ProjName ;
	CString ProjDir ;
	CString CurrDir ;// ('\0',256) ;
// Operations
public:
	virtual void Run(void);

// Data
protected:
	COProject m_prj;
	UIAppWizard m_uaw;
	UIProjectWizard m_prjwiz;

// Test Cases
protected:
	void NewProject(void);
	void MDIApp(void);
	void Classes(void);
	void ChangeAppType(void);
	void NoAbout(void);
	void JumpConfirm(void);
	void CreateProject(void);
	void VerifyFiles(void);
};

#endif //__APPCASES_H__
