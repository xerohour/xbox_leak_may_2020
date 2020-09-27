///////////////////////////////////////////////////////////////////////////////
//	DLGCASES.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Edited By :				Date :
//		ScottSe					8/29/96
//
//	Description :
//		Declaration of the CDlgTestCases class
//

#ifndef __DLGCASES_H__
#define __DLGCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CDlgTestCases class

class CDlgTestCases : public CTest
{
	DECLARE_TEST(CDlgTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	UIDlgEdit m_uDlgEd;
//	CWnd*	m_pDlgEd;	 // Pointers to the common dlg editor
//	CWnd*	m_pDlg;

// Test Cases
protected:
	BOOL CreateDlgRes(void);
	BOOL DropControls(void);
	BOOL TestDialog(void);
	BOOL FindControls(void);
	BOOL CheckCreation(CString CtrlID);
	BOOL SaveOpen(void);
};

#endif //__DLGCASES_H__
