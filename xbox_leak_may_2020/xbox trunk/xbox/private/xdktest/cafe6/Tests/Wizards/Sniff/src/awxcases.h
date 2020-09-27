///////////////////////////////////////////////////////////////////////////////
//	AWXCASES.H
//
//	Created by :			Date :
//		Ivanl					10/17/94
//
//	Description :
//		Declaration of the CAwxTestCases class
//

#ifndef __AWXCASES_H__
#define __AWXCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	AwxTestCases class

class CAwxTestCases : public CTest
{
	DECLARE_TEST(CAwxTestCases, CSniffDriver)

// Attributes
	int iteration ;
	int LangDlls ;

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
	UIExtWizard m_extwiz;

// Test Cases
protected:
	void NewProject(void);
	void ChooseZapper(void);
	void ChooseCustom(void);
	void ChooseApwSequence(void);
};

#endif //__AWXCASES_H__
