///////////////////////////////////////////////////////////////////////////////
//	CTRCASES.H
//
//	Created by :			Date :
//		Ivanl					3/17/94
//
//	Description :
//		Declaration of the CCtrlWizardCases class
//

#ifndef __CTRCASES_H__
#define __CTRCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AwxTestCases class

class CCtrlWizardCases : public CTest
{
	DECLARE_TEST(CCtrlWizardCases, CSniffDriver)

// Attributes
	int iteration ;
	int LangDlls ;
	COProject prj;
public:
// Operations
// Data members
	CString  szDir ; 
	CString szDirRes ;
	CString ProjName ;
	CString ProjDir ;
	CString CurrDir ;// ('\0',256) ;

public:
	virtual void Run(void);

// Data
protected:
	UICtrlWizard m_ctrlwiz;

// Test Cases
protected:
	void NewProject(void);
	void SetProjOptions(void);
	void SetCtrlOptions(void);
	void GenerateProject(void) ;
	void AddControlMethods(void) ;
	void AddControlEvents(void) ;
	void AddControlProperty(void) ;
};

#endif //__CTRCASES_H__
