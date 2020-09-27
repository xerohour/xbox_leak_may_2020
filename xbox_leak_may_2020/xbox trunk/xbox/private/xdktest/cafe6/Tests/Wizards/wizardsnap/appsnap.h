///////////////////////////////////////////////////////////////////////////////
//	APPSNAP.H
//
//	Created by :			Date :
//		Ivanl				12/29/94
//
//	Description :
//		Declaration of the CTestAppWizardCases class
//

#ifndef __APPSNAP_H__
#define __APPSNAP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AppTestCases class

class CTestAppWizardCases : public CTest
{
	DECLARE_TEST(CTestAppWizardCases, CSniffDriver)

// Attributes
	int iteration ;
	int LangDlls ;

// Operations
public:
	virtual void Run();

// Data
protected:
	UITestAppWizard m_utstaw;

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

#endif //__APPSNAP_H__
