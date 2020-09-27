///////////////////////////////////////////////////////////////////////////////
//	VERCASES.H
//
//	Created by :			Date :
//		Enriquep					8/27/93
//
//	Modifies by :			Date :
//		ScottSe						9/13/96		Major rewrite of the entire test
//
//	Description :
//		Declaration of the CVerTestCases class
//

#ifndef __VERCASES_H__
#define __VERCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CVerTestCases class

class CVerTestCases : public CTest
{
	DECLARE_TEST(CVerTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	UIVerEdit m_uVerEd;

// Test Cases
protected:
	BOOL CreateVersionRes(void);
	BOOL EditCommentsField(void);
	BOOL CopyPaste(void);
	BOOL NewStringBlock(void);
	BOOL ChangeToStringID(void);
	BOOL DeleteStringBlock(void);
	BOOL SaveOpen(void);
};

#endif //__VERCASES_H__
