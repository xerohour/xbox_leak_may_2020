///////////////////////////////////////////////////////////////////////////////
//	MENCASES.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Modified by :			Date :			Action :
//		ScottSe					9/4/96			Major rewrite of entire test
//
//	Description :
//		Declaration of the CMenTestCases class
//

#ifndef __MENCASES_H__
#define __MENCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CMenTestCases class
 
class CMenTestCases : public CTest
{
	DECLARE_TEST(CMenTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
//protected:
//	BOOL m_bSafeIDs;
//	CString m_strID1;
//	CString m_strID2;

// Test Cases
protected:
	BOOL CreateMenuRes(void);
	BOOL SaveOpen(void);
	BOOL EnterMenuItems(void);
	BOOL CopyAndPaste(void);
	BOOL CheckCreation(CString, int);

};

#endif //__MENCASES_H__
